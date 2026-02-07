// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Save/SaveManager.h"

#include "Object/Save/ValkyrieWarSaveGame.h"
#include "Object/SaveGame/CheckAccountSaveGame.h"

#include "Kismet/GameplayStatics.h"
#include "GameSystem/Library/DataEncryptHelper.h"


void USaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitSetDataAction();

	//CachedSaveGame = Cast<UValkyrieWarSaveGame>(UGameplayStatics::CreateSaveGameObject(UValkyrieWarSaveGame::StaticClass()));
	//LoadGame();
}

void USaveManager::Deinitialize()
{
	//SaveGame();
	//CachedSaveGame = nullptr;

	Super::Deinitialize();
}

//void USaveManager::SetPlayerAccountData(const FPlayerAccountData& InPlayerAccountData)
//{
	//CurrentPlayerAccountData = InPlayerAccountData;
	
	//SaveGame();
//}
//
//void USaveManager::SaveGame()
//{
//	if (CachedSaveGame)
//	{
//		CachedSaveGame->PlayerAccountData = CurrentPlayerAccountData;
//
//		bool bIsSuccess = UGameplayStatics::SaveGameToSlot(CachedSaveGame, SaveSlotName, SaveIndex);
//		if (bIsSuccess)
//		{
//			UE_LOG(LogTemp, Log, TEXT("저장 성공"));
//		}
//		else
//		{
//			UE_LOG(LogTemp, Log, TEXT("저장 실패"));
//		}
//	}
//
//}
//
//void USaveManager::LoadGame()
//{
//	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, SaveIndex))
//	{
//		UValkyrieWarSaveGame* LoadedSaveGame = Cast<UValkyrieWarSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, SaveIndex));
//
//		if (LoadedSaveGame)
//		{
//			CurrentPlayerAccountData = LoadedSaveGame->PlayerAccountData;
//			CachedSaveGame->PlayerAccountData = CurrentPlayerAccountData;
//			UE_LOG(LogTemp, Log, TEXT("저장파일 로드 성공"));
//		}
//		else
//		{
//			UE_LOG(LogTemp, Log, TEXT("저장파일 로드 실패"));
//		}
//	}
//
//}

void USaveManager::LoadAllData()
{
	const UEnum* EnumPtr = StaticEnum<ESaveType>();

	int32 AmountToLoad = EnumPtr->NumEnums() - 1;

	if (EnumPtr)
	{
		for (int32 i = 1; i < EnumPtr->NumEnums(); ++i)
		{
			// 숨겨진 메타데이터(예: "umax")는 제외하고 싶을 때 체크
			if (EnumPtr->HasMetaData(TEXT("Hidden"), i)) continue;

			// 이름(FString) 가져오기
			FString EnumName = EnumPtr->GetNameStringByIndex(i);

			FOnSaveGameLoaded OnLoadedDelegate;
			OnLoadedDelegate.BindDynamic(this, &USaveManager::OnDataLoaded);

			//데이터 암호화 로드 비동기...
			UDataEncryptHelper::LoadGameEncryptedAsync(EnumName, OnLoadedDelegate, static_cast<ESaveType>(EnumPtr->GetValueByIndex(i)));
		}
	}
}

void USaveManager::LoadCheckAccount()
{
	FOnSaveGameLoaded OnLoadedDelegate;
	OnLoadedDelegate.BindDynamic(this, &USaveManager::OnDataLoaded);

	//체크용 데이터는 동기식으로 로드
	USaveGame* LoadData = UDataEncryptHelper::LoadGameEncrypted(UEnum::GetValueAsString(ESaveType::CheckAccount));

	//TODO: 데이터 유무 및 아이디 존재에 따라 반환
	//이후 로그인할 지 계정생성할 지 로직 탈 수 있도록 세팅
	//저장 시 방식 설정..
	//저장할 때 뭔가 Add/Remove 이런거로만 할 지
	//직접 캐시된 데이터에 접근해서 세팅하고
	//저장은 이후 해당 파일만 하는 형태로?
	
}

void USaveManager::InitSetDataAction()
{
	ActionSetData.Add(ESaveType::CheckAccount, [this](USaveGame* InData) { CheckAccount = Cast<UCheckAccountSaveGame>(InData); });
}

void USaveManager::LoadDataInternal(ESaveType InSaveType, USaveGame* InLoadedData)
{
	if (ActionSetData.Contains(InSaveType))
	{
		ActionSetData[InSaveType](InLoadedData);
	}
}

void USaveManager::OnDataLoaded(USaveGame* LoadedSaveGame, bool bIsSuccess, ESaveType InSaveType)
{
	//데이터 캐싱
	if (bIsSuccess)
	{
		LoadDataInternal(InSaveType, LoadedSaveGame);
	}
	else
	{
		//로드 실패
		UE_LOG(LogTemp, Error, TEXT("Failed to load data : %s"), *UEnum::GetValueAsString(InSaveType));
	}
}
