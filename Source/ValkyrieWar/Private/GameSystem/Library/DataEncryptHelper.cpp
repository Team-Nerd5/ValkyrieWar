// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Library/DataEncryptHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/AES.h"
#include "HAL/PlatformFileManager.h"

bool UDataEncryptHelper::SaveGameEncrypted(USaveGame* SaveGameObject, ESaveType InSaveType)
{
	if (!SaveGameObject) return false;

	FString KeyStr = GetEncryptKey();
	if (!IsKeyValid(KeyStr))
	{
		UE_LOG(LogTemp, Error, TEXT("[SaveGameHelper] Encryption Key must be exactly 32 characters long."));
		return false;
	}

	// 1. SaveGame 객체를 바이트 배열로 직렬화
	TArray<uint8> ObjectBytes;
	if (!UGameplayStatics::SaveGameToMemory(SaveGameObject, ObjectBytes))
	{
		return false;
	}

	// [수정됨] 2. PKCS#7 패딩 적용 (Zero 패딩 대신)
	int32 BytesToPad = FAES::AESBlockSize - (ObjectBytes.Num() % FAES::AESBlockSize);
	for (int32 i = 0; i < BytesToPad; ++i)
	{
		ObjectBytes.Add(static_cast<uint8>(BytesToPad));
	}

	// 3. AES 암호화 수행
	FTCHARToUTF8 KeyUtf8(*KeyStr);
	FAES::FAESKey AESKey;
	FMemory::Memcpy(AESKey.Key, KeyUtf8.Get(), 32);

	FAES::EncryptData(ObjectBytes.GetData(), ObjectBytes.Num(), AESKey);

	// 4. 파일로 저장
	FString SaveFile = GetSaveFilePath(StaticEnum<ESaveType>()->GetNameStringByValue(static_cast<int64>(InSaveType)));
	return FFileHelper::SaveArrayToFile(ObjectBytes, *SaveFile);
}

USaveGame* UDataEncryptHelper::LoadGameEncrypted(ESaveType InSaveType)
{
	if (!IsKeyValid(GetEncryptKey()))
	{
		UE_LOG(LogTemp, Error, TEXT("[SaveGameHelper] Encryption Key must be exactly 32 characters long."));
		return nullptr;
	}

	FString SaveFile = GetSaveFilePath(StaticEnum<ESaveType>()->GetNameStringByValue(static_cast<int64>(InSaveType)));

	// 1. 파일이 존재하는지 확인
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*SaveFile))
	{
		return nullptr;
	}

	// 2. 파일 내용을 바이트 배열로 로드
	TArray<uint8> LoadedBytes;
	if (!FFileHelper::LoadFileToArray(LoadedBytes, *SaveFile))
	{
		return nullptr;
	}

	// 3. AES 복호화 수행
	FTCHARToUTF8 KeyUtf8(*GetEncryptKey());
	FAES::FAESKey AESKey;
	FMemory::Memcpy(AESKey.Key, KeyUtf8.Get(), 32);

	FAES::DecryptData(LoadedBytes.GetData(), LoadedBytes.Num(), AESKey);

	// 4. 바이트 배열을 SaveGame 객체로 역직렬화
	return UGameplayStatics::LoadGameFromMemory(LoadedBytes);
}

void UDataEncryptHelper::LoadGameEncryptedAsync(FOnSaveGameLoaded OnLoaded, ESaveType InSaveType)
{
	FString SlotName = StaticEnum<ESaveType>()->GetNameStringByValue(static_cast<int64>(InSaveType));

	// [수정됨] 게임 스레드에서 미리 키를 가져옴
	FString KeyStr = GetEncryptKey();

	// 람다 캡처에 KeyStr을 값 복사로 전달
	Async(EAsyncExecution::Thread, [SlotName, OnLoaded, InSaveType, KeyStr]()
		{
			FString SaveFile = GetSaveFilePath(SlotName);
			TArray<uint8> LoadedBytes;
			bool bReadSuccess = false;

			if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*SaveFile))
			{
				if (FFileHelper::LoadFileToArray(LoadedBytes, *SaveFile))
				{
					bReadSuccess = true;
				}
			}

			if (!bReadSuccess)
			{
				AsyncTask(ENamedThreads::GameThread, [OnLoaded, InSaveType]()
					{
						OnLoaded.ExecuteIfBound(nullptr, false, InSaveType);
					});
				return;
			}

			if (KeyStr.Len() == 32)
			{
				FTCHARToUTF8 KeyUtf8(*KeyStr);
				FAES::FAESKey AESKey;
				FMemory::Memcpy(AESKey.Key, KeyUtf8.Get(), 32);

				// 복호화 수행
				FAES::DecryptData(LoadedBytes.GetData(), LoadedBytes.Num(), AESKey);

				// [수정됨] 복호화 직후 PKCS#7 패딩 제거
				if (LoadedBytes.Num() > 0)
				{
					uint8 PadValue = LoadedBytes.Last();
					// 추가된 패딩 값이 AES 블록 사이즈(16) 이내의 유효한 값인지 확인
					if (PadValue > 0 && PadValue <= FAES::AESBlockSize)
					{
						// 원본 크기로 배열 축소
						LoadedBytes.RemoveAt(LoadedBytes.Num() - PadValue, PadValue);
					}
				}
			}

			// 메인 스레드로 넘겨서 직렬화(LoadGameFromMemory) 처리
			AsyncTask(ENamedThreads::GameThread, [LoadedBytes, OnLoaded, InSaveType]()
				{
					USaveGame* LoadedGame = nullptr;

					if (LoadedBytes.Num() > 0)
					{
						// 이제 원본 크기와 정확히 일치하므로 안전하게 로드됨
						LoadedGame = UGameplayStatics::LoadGameFromMemory(LoadedBytes);
					}

					OnLoaded.ExecuteIfBound(LoadedGame, (LoadedGame != nullptr), InSaveType);
				});
		});
}

bool UDataEncryptHelper::IsKeyValid(const FString& Key)
{
	return Key.Len() >= 32;
}

FString UDataEncryptHelper::GetSaveFilePath(const FString& SlotName, int32 UserIndex)
{
	return FString::Printf(TEXT("%sSaveGames/%s.sav"), *FPaths::ProjectSavedDir(), *SlotName);
}

FString UDataEncryptHelper::GetEncryptKey()
{
	FString PartA = TEXT("X7v9Lk2Mq4nP8zR1");
	FString PartB = TEXT("w3sJ6tH5bG0cF2dQ");

	return PartA + PartB;
}
