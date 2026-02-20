// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/SaveManager.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/DataEncryptHelper.h"
#include "GameSystem/Library/GameSaveHelper.h"
#include "GameSystem/Library/GameBaseLibrary.h"

#include "Object/Save/ValkyrieWarSaveGame.h"

#include "Object/SaveGame/CheckAccountSaveGame.h"
#include "Object/SaveGame/AccountSaveGame.h"
#include "Object/SaveGame/GachaSaveGame.h"
#include "Object/SaveGame/GoodsSaveGame.h"
#include "Object/SaveGame/ItemSaveGame.h"
#include "Object/SaveGame/StageSaveGame.h"
#include "Object/SaveGame/UnitUpgradeSaveGame.h"
#include "Object/SaveGame/ValkyrieSaveGame.h"

#include "Kismet/GameplayStatics.h"


void USaveManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitSetDataAction();

	InitSaveDataAction();
}

void USaveManager::Deinitialize()
{
	Super::Deinitialize();
}


int32 USaveManager::LoadAllData()
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

			LoadTask++;
			//데이터 암호화 로드 비동기...
			UDataEncryptHelper::LoadGameEncryptedAsync(OnLoadedDelegate, static_cast<ESaveType>(EnumPtr->GetValueByIndex(i)));
		}
	}
	return LoadTask;
}

uint64 USaveManager::GetUserId()
{
	if (CheckAccount)
		return CheckAccount->UserId;

	//로드를 하고 없으면 UserID를 증가시켜서 새로 만들어줌
	USaveGame* LoadData = UDataEncryptHelper::LoadGameEncrypted(ESaveType::CheckAccount);
	if (!LoadData)
	{
		LoadData = UGameSaveHelper::MakeSaveGame(ESaveType::CheckAccount);
	}
	CheckAccount = Cast<UCheckAccountSaveGame>(LoadData);

	CheckAccount->UserId++;

	SaveInternal(ESaveType::CheckAccount);

	return CheckAccount->UserId;
}

bool USaveManager::IsAcountExist()
{
	USaveGame* LoadData = UDataEncryptHelper::LoadGameEncrypted(ESaveType::CheckAccount);

	if (!LoadData)
		return false;

	CheckAccount = Cast<UCheckAccountSaveGame>(LoadData);

	return CheckAccount && CheckAccount->UserId > 0;
}

void USaveManager::InitSetDataAction()
{
	//ActionSetData.Add(ESaveType::CheckAccount, [this](USaveGame* InData) { CheckAccount = Cast<UCheckAccountSaveGame>(InData); });
	ActionSetData.Add(ESaveType::Account, [this](USaveGame* InData) { Account = Cast<UAccountSaveGame>(InData); });
	ActionSetData.Add(ESaveType::Gacha, [this](USaveGame* InData) { Gacha = Cast<UGachaSaveGame>(InData); });
	ActionSetData.Add(ESaveType::Goods, [this](USaveGame* InData) { Goods = Cast<UGoodsSaveGame>(InData); });
	ActionSetData.Add(ESaveType::Item, [this](USaveGame* InData)
		{
			Item = Cast<UItemSaveGame>(InData);
			SetItemData();
		});
	ActionSetData.Add(ESaveType::Stage, [this](USaveGame* InData) { Stage = Cast<UStageSaveGame>(InData); });
	ActionSetData.Add(ESaveType::UnitUpgrade, [this](USaveGame* InData) { UnitUpgrade = Cast<UUnitUpgradeSaveGame>(InData); });
	ActionSetData.Add(ESaveType::Valkyrie, [this](USaveGame* InData) { Valkyrie = Cast<UValkyrieSaveGame>(InData); });
}

void USaveManager::InitSaveDataAction()
{
	
	ActionSaveData.Add(ESaveType::CheckAccount, [this]()
		{
			UDataEncryptHelper::SaveGameEncrypted(CheckAccount,ESaveType::CheckAccount);
		});
	ActionSaveData.Add(ESaveType::Account, [this]()
		{
			UDataEncryptHelper::SaveGameEncrypted(Account,ESaveType::Account);
		});
	ActionSaveData.Add(ESaveType::Gacha, [this]()
		{
			UDataEncryptHelper::SaveGameEncrypted(Gacha, ESaveType::Gacha);
		});
	ActionSaveData.Add(ESaveType::Goods, [this]()
		{
			UDataEncryptHelper::SaveGameEncrypted(Goods, ESaveType::Goods);
		});
	ActionSaveData.Add(ESaveType::Item, [this]()
		{
			UDataEncryptHelper::SaveGameEncrypted(Item, ESaveType::Item);
		});
	ActionSaveData.Add(ESaveType::Stage, [this]()
		{
			UDataEncryptHelper::SaveGameEncrypted(Stage, ESaveType::Stage);
		});
	ActionSaveData.Add(ESaveType::UnitUpgrade, [this]()
		{
			UDataEncryptHelper::SaveGameEncrypted(UnitUpgrade, ESaveType::UnitUpgrade);
		});
	ActionSaveData.Add(ESaveType::Valkyrie, [this]()
		{
			UDataEncryptHelper::SaveGameEncrypted(Valkyrie, ESaveType::Valkyrie);
		});
}

void USaveManager::LoadDataInternal(ESaveType InSaveType, USaveGame* InLoadedData)
{
	if (ActionSetData.Contains(InSaveType))
	{
		ActionSetData[InSaveType](InLoadedData);
	}
}

void USaveManager::SaveInternal(ESaveType InSaveType)
{
	if (ActionSaveData.Contains(InSaveType))
	{
		ActionSaveData[InSaveType]();
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
		//저장된 데이터가 없으면 새로 만들어준다.
		USaveGame* Data = UGameSaveHelper::MakeSaveGame(InSaveType);
		if (Data)
		{
			LoadDataInternal(InSaveType, Data);
		}		
	}

	UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this);
	if (EventSystem)
	{
		EventSystem->Login.OnDataLoadComplete.Broadcast();
	}
}


void USaveManager::UpdateItem(uint64 InUID, int32 InAmount, uint64 InEquipCharacter)
{
	ItemDataStruct* ItemData = *Item->ItemDataList.Find(InUID);

	if (ItemData)
	{
		ItemData->Amount = InAmount;
		ItemData->EquipCharacter = InEquipCharacter;
	}

	SaveInternal(ESaveType::Item);
}

void USaveManager::SetItemData()
{
	if (Item)
	{
		UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>();

		if (DataManager)
		{
			for (auto Data : Item->ItemDataList)
			{				
				DataManager->GetItemModule()->LoadItem(Data.Value->UID, Data.Value->Amount, Data.Value->EquipCharacter);
			}
		}
	}
}

void USaveManager::CreateAccount(FString& InNickname)
{
	if (Account)
	{
		Account->Nickname = InNickname;

		SaveInternal(ESaveType::Account);
	}
}

void USaveManager::LoadData(ESaveType InSaveType)
{
	USaveGame* Data = UGameSaveHelper::MakeSaveGame(InSaveType);
	if (Data)
	{
		LoadDataInternal(InSaveType, Data);
	}
}

void USaveManager::SaveData(ESaveType InSaveType)
{
	SaveInternal(InSaveType);
}
