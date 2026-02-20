// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Library/DataEncryptHelper.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/AES.h"
#include "HAL/PlatformFileManager.h"

bool UDataEncryptHelper::SaveGameEncrypted(USaveGame* SaveGameObject, ESaveType InSaveType)
{
	if (!SaveGameObject) return false;
	if (!IsKeyValid(GetEncryptKey()))
	{
		UE_LOG(LogTemp, Error, TEXT("[SaveGameHelper] Encryption Key must be exactly 32 characters long."));
		return false;
	}

	// 1. SaveGame 객체를 바이트 배열로 직렬화 (메모리에 저장)
	TArray<uint8> ObjectBytes;
	if (!UGameplayStatics::SaveGameToMemory(SaveGameObject, ObjectBytes))
	{
		return false;
	}

	// [핵심 수정] 2. AES 블록 크기(16바이트)에 맞게 패딩 추가
	// 부족한 바이트 수만큼 배열 끝에 0(Zero)을 채워 16의 배수로 만듭니다.
	int32 BytesToPad = FAES::AESBlockSize - (ObjectBytes.Num() % FAES::AESBlockSize);
	ObjectBytes.AddZeroed(BytesToPad);

	// 3. AES 암호화 수행
	FTCHARToUTF8 KeyUtf8(*GetEncryptKey());
	FAES::FAESKey AESKey;
	FMemory::Memcpy(AESKey.Key, KeyUtf8.Get(), 32); // 32바이트 복사

	// 이제 ObjectBytes의 크기가 16의 배수이므로 안전하게 암호화됩니다.
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

	Async(EAsyncExecution::Thread, [SlotName, OnLoaded, InSaveType]()
		{
			// 이 안쪽은 게임(메인) 스레드가 아님! UObject 접근 주의!

			FString SaveFile = GetSaveFilePath(SlotName); // 경로는 문자열 처리라 괜찮음
			TArray<uint8> LoadedBytes;
			bool bReadSuccess = false;

			// 파일 존재 확인 및 읽기
			if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*SaveFile))
			{
				if (FFileHelper::LoadFileToArray(LoadedBytes, *SaveFile))
				{
					bReadSuccess = true;
				}
			}

			// 읽기 실패 시 메인 스레드로 보고
			if (!bReadSuccess)
			{
				AsyncTask(ENamedThreads::GameThread, [OnLoaded, InSaveType]()
					{
						OnLoaded.ExecuteIfBound(nullptr, false, InSaveType);
					});
				return;
			}

			// 암호화 키 가져오기 (이전에 만든 함수)
			FString KeyStr = GetEncryptKey();
			if (KeyStr.Len() == 32)
			{
				FTCHARToUTF8 KeyUtf8(*KeyStr);
				FAES::FAESKey AESKey;
				FMemory::Memcpy(AESKey.Key, KeyUtf8.Get(), 32);

				// 무거운 작업: 복호화 (여기서 시간 소요)
				FAES::DecryptData(LoadedBytes.GetData(), LoadedBytes.Num(), AESKey);
			}

			// 2. 직렬화(UObject 생성)는 반드시 메인 스레드(GameThread)에서 해야 함!
			// 데이터를 메인 스레드로 넘김
			AsyncTask(ENamedThreads::GameThread, [LoadedBytes, OnLoaded, InSaveType]()
				{
					// 여기는 다시 메인 스레드
					USaveGame* LoadedGame = nullptr;

					if (LoadedBytes.Num() > 0)
					{
						// 메모리에서 USaveGame 객체 생성
						LoadedGame = UGameplayStatics::LoadGameFromMemory(LoadedBytes);
					}

					// 블루프린트(또는 호출자)에게 결과 전달
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
