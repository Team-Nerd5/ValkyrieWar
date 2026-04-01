// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/SaveManager.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/DataEncryptHelper.h"
#include "GameSystem/Library/GameSaveHelper.h"
#include "GameSystem/Library/GameBaseLibrary.h"
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

	InitInitDataAction();

	InitSetDataAction();

	InitSaveDataAction();
}

void USaveManager::Deinitialize()
{
	Super::Deinitialize();
}

void USaveManager::InitSetDataAction()
{
	ActionSetData.Add(ESaveType::CheckAccount, [this](USaveGame* InData) { CheckAccount = Cast<UCheckAccountSaveGame>(InData); });
	ActionSetData.Add(ESaveType::Account, [this](USaveGame* InData)
		{
			Account = Cast<UAccountSaveGame>(InData);
			SetAccountData();
		});
	ActionSetData.Add(ESaveType::Gacha, [this](USaveGame* InData) { Gacha = Cast<UGachaSaveGame>(InData); });
	ActionSetData.Add(ESaveType::Goods, [this](USaveGame* InData) {
		Goods = Cast<UGoodsSaveGame>(InData);
		SetGoodsData();
		});
	ActionSetData.Add(ESaveType::Item, [this](USaveGame* InData)
		{
			Item = Cast<UItemSaveGame>(InData);
			SetItemData();
		});
	ActionSetData.Add(ESaveType::Stage, [this](USaveGame* InData) { Stage = Cast<UStageSaveGame>(InData); });
	ActionSetData.Add(ESaveType::UnitUpgrade, [this](USaveGame* InData) {
		Unit = Cast<UUnitUpgradeSaveGame>(InData);
		SetUnitData();
		});
	ActionSetData.Add(ESaveType::Valkyrie, [this](USaveGame* InData)
		{
			Valkyrie = Cast<UValkyrieSaveGame>(InData);
			SetValkyrieData();
		});
}

void USaveManager::InitInitDataAction()
{
	ActionInitData.Add(ESaveType::CheckAccount, [this](USaveGame* InData) { CheckAccount = Cast<UCheckAccountSaveGame>(InData); });
	ActionInitData.Add(ESaveType::Account, [this](USaveGame* InData) { Account = Cast<UAccountSaveGame>(InData); });
	ActionInitData.Add(ESaveType::Gacha, [this](USaveGame* InData) { Gacha = Cast<UGachaSaveGame>(InData); });
	ActionInitData.Add(ESaveType::Goods, [this](USaveGame* InData) { Goods = Cast<UGoodsSaveGame>(InData); });
	ActionInitData.Add(ESaveType::Item, [this](USaveGame* InData)  { Item = Cast<UItemSaveGame>(InData);  });
	ActionInitData.Add(ESaveType::Stage, [this](USaveGame* InData) { Stage = Cast<UStageSaveGame>(InData); });
	ActionInitData.Add(ESaveType::UnitUpgrade, [this](USaveGame* InData) { Unit = Cast<UUnitUpgradeSaveGame>(InData); });
	ActionInitData.Add(ESaveType::Valkyrie, [this](USaveGame* InData) { Valkyrie = Cast<UValkyrieSaveGame>(InData);	});
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
			UDataEncryptHelper::SaveGameEncrypted(Unit, ESaveType::UnitUpgrade);
		});
	ActionSaveData.Add(ESaveType::Valkyrie, [this]()
		{
			UDataEncryptHelper::SaveGameEncrypted(Valkyrie, ESaveType::Valkyrie);
		});
}

void USaveManager::InitDataInternal(ESaveType InSaveType, USaveGame* InLoadedData)
{
	if (ActionInitData.Contains(InSaveType))
	{
		ActionInitData[InSaveType](InLoadedData);
	}
}

void USaveManager::SetDataInternal(ESaveType InSaveType, USaveGame* InLoadedData)
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

void USaveManager::InitAllData()
{
	const UEnum* EnumPtr = StaticEnum<ESaveType>();

	int32 AmountToLoad = EnumPtr->NumEnums() - 1;

	if (EnumPtr)
	{
		for (int32 i = 0; i < EnumPtr->NumEnums(); ++i)
		{
			ESaveType Type = static_cast<ESaveType>(EnumPtr->GetValueByIndex(i));

			USaveGame* Data = UGameSaveHelper::MakeSaveGame(Type);
			InitDataInternal(Type, Data);
		}
	}
}

int32 USaveManager::LoadAllData()
{
	const UEnum* EnumPtr = StaticEnum<ESaveType>();

	int32 AmountToLoad = EnumPtr->NumEnums() - 1;

	if (EnumPtr)
	{
		for (int32 i = 1; i < EnumPtr->NumEnums(); ++i)
		{
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

void USaveManager::OnDataLoaded(USaveGame* LoadedSaveGame, bool bIsSuccess, ESaveType InSaveType)
{
	//데이터 캐싱
	if (bIsSuccess)
	{
		SetDataInternal(InSaveType, LoadedSaveGame);
	}

	UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this);
	if (EventSystem)
	{
		EventSystem->Login.OnDataLoadComplete.Broadcast();
	}
}


void USaveManager::SaveValkyrie(UValkyrieData* InData)
{
	if (Valkyrie)
	{
		FValkyrieSaveData ValkyrieSave;
		ValkyrieSave.DataId = InData->GetDataID();

		Valkyrie->ValkyrieData.Add(InData->GetUID(), ValkyrieSave);

		if (bIsNewAccount)
		{
			//캐릭터가 새로 생성되었는데, 신규계정 상태이면 계정 생성임
			if (UGameManager* GameManager = Cast<UGameManager>(GetGameInstance()))
			{
				GameManager->SelectVakyrie(InData->GetUID());
			}

			Account->SelectedValkyrie = InData->GetUID();
			SaveInternal(ESaveType::Account);
		}

		SaveInternal(ESaveType::Valkyrie);
	}
}

void USaveManager::SaveUnit(UUnitData* InData)
{
	if (Unit)
	{
		if (Unit->OwnUnits.Contains(InData->GetUID()))
		{
			FUnitDataStruct* Data = Unit->OwnUnits.Find(InData->GetUID());
			Data->Level = InData->GetLevel();
			Data->Grade = static_cast<int32>(InData->GetCurrentGrade());
			//다른건 바뀌지 않을 데이터
		}
		else
		{
			FUnitDataStruct NewData;
			NewData.UID = InData->GetUID();
			NewData.Level = InData->GetLevel();
			NewData.UnitType = static_cast<int32>(InData->GetUnitType());
			NewData.Grade = static_cast<int32>(InData->GetCurrentGrade());

			Unit->OwnUnits.Add(InData->GetUID(), NewData);
		}
	}
	SaveInternal(ESaveType::UnitUpgrade);
}

void USaveManager::SaveUnits(TArray<FUnitDataStruct> InData)
{
	if (Unit)
	{
		for (const FUnitDataStruct&  Data : InData)
		{
			Unit->OwnUnits.Add(Data.UID, Data);
		}

		SaveInternal(ESaveType::UnitUpgrade);
	}
}

#pragma region Add save data
void USaveManager::AddSaveItem(UItemData* InItem)
{
	if (Item)
	{
		Item->AddItem(InItem);
	}
	SaveInternal(ESaveType::Item);
}

void USaveManager::RemoveSaveItem(uint64 InUID)
{
	if (Item)
	{
		Item->RemoveItem(InUID);
	}
	SaveInternal(ESaveType::Item);
}

#pragma endregion




#pragma region Set data after load saved data
void USaveManager::SetItemData()
{
	//로드된 보유 아이템 데이터를 가져옴
	if (Item)
	{
		if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
		{
			for (auto& ItemData : Item->ItemDataList)
			{
				DataManager->GetItemModule()->LoadItem(
					ItemData.Value.UID,
					ItemData.Value.DataId,
					ItemData.Value.Amount,
					ItemData.Value.EquipCharacter);
			}
		}		
	}
}

void USaveManager::SetGoodsData()
{
	UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>();

	if (DataManager)
	{
		if (Goods)
		{
			DataManager->GetGoodsModule()->Update(EGoodsType::Ticket, Goods->GetGoods(EGoodsType::Ticket));
			DataManager->GetGoodsModule()->Update(EGoodsType::Gem, Goods->GetGoods(EGoodsType::Gem));
			DataManager->GetGoodsModule()->Update(EGoodsType::Gold, Goods->GetGoods(EGoodsType::Gold));
		}
		
	}
}

void USaveManager::SetUnitData()
{
	//로드가 되었으면
	if (Unit)
	{
		if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
		{
			TArray<FUnitDataStruct> Units;
			Unit->OwnUnits.GenerateValueArray(Units);
			for (const FUnitDataStruct& UnitData : Units)
			{
				DataManager->GetUnitModule()->LoadUnit(UnitData);
			}
		}
	}
}

void USaveManager::SetAccountData()
{
	if (Account)
	{
		UGameManager* GameManager = Cast<UGameManager>(GetGameInstance());

		if (GameManager)
		{
			GameManager->SelectVakyrie(Account->SelectedValkyrie);
		}

		//TODO : 계정 데이터 모듈에 세팅필요
		//계정 레벨이라던지...
	}
}

void USaveManager::SetValkyrieData()
{
	//현재 예상 : 계정 생성 시에 DataManager에 새 캐릭터를 넣어줬음
	//발키리 로드가 계정 생성 이후에 되니까 이미 데이터 세팅이 완료되야함.
	if (Valkyrie)
	{
		UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>();

		if (DataManager)
		{
			for (auto Data : Valkyrie->ValkyrieData)
			{
				DataManager->GetValkyrieModule()->LoadData(Data.Key, Data.Value.DataId);
			}
		}

		if (bIsNewAccount)
		{
			//발키리 데이터를 로드했는데 신규 계정 생성한거면 파일 저장
			SaveInternal(ESaveType::Valkyrie);
			bIsNewAccount = false;
		}
	}
}

#pragma endregion

bool USaveManager::IsAcountExist()
{
	USaveGame* LoadData = UDataEncryptHelper::LoadGameEncrypted(ESaveType::CheckAccount);

	if (!LoadData)
		return false;

	SetDataInternal(ESaveType::CheckAccount, LoadData);

	return CheckAccount && CheckAccount->GetUserID() > 0;
}

void USaveManager::CreateAccount(FString& InNickname)
{
	if (!CheckAccount)
	{
		UE_LOG(LogTemp, Error, TEXT("계정생성 오류 : CheckAccount가 없으면 안됨"));
	}

	//임시로 고정 id 제공
	CheckAccount->SetUserID(10001);
	SaveInternal(ESaveType::CheckAccount);

	if (Account)
	{
		Account->Nickname = InNickname;

		bIsNewAccount = true;

		SaveInternal(ESaveType::Account);
	}
}

void USaveManager::LoadData(ESaveType InSaveType)
{
	USaveGame* Data = UGameSaveHelper::MakeSaveGame(InSaveType);
	if (Data)
	{
		SetDataInternal(InSaveType, Data);
	}
}

void USaveManager::SaveData(ESaveType InSaveType)
{
	SaveInternal(InSaveType);
}

uint64 USaveManager::GetNextItemUID()
{
	uint64 UID = 0;

	if (CheckAccount)
	{
		UID = CheckAccount->GetItemUID();

		SaveInternal(ESaveType::CheckAccount);
	}

	return UID;
}

uint64 USaveManager::GetNextValkyrieUID()
{
	uint64 UID = 0;

	if (CheckAccount)
	{
		UID = CheckAccount->GetValkyrieUID();

		SaveInternal(ESaveType::CheckAccount);
	}

	return UID;
}

void USaveManager::AddGoods(EGoodsType InGoodsType, int64 InAmount)
{
	if (Goods)
	{
		Goods->AddGoods(InGoodsType, InAmount);

		SaveInternal(ESaveType::Goods);		
	}
}

void USaveManager::SaveMainValkyrie(uint64 InUID)
{
	if (Account)
	{
		Account->SelectedValkyrie = InUID;

		SaveInternal(ESaveType::Account);
	}
}
