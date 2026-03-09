// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/ItemModule.h"

void UItemModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Item);
		MakeData();
	}

	SendDataLoadComplete();
}

void UItemModule::MakeData()
{
	//데이터 탐색 속도를 위해 Map으로 메모리에 올려둠
	if (DataTable)
	{
		TArray<FItemDataRow*> AllRows;
		DataTable->GetAllRows<FItemDataRow>(TEXT("ItemModule_Init"), AllRows);

		for (FItemDataRow* Item : AllRows)
		{
			if (!Item) continue;
			TableDataByDataId.Add(Item->DataId, *Item);
		}
	}
	if (DataTable == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("❌ [ItemModule] 데이터 테이블 에셋이 연결되지 않았습니다!!!"));
	}
}

void UItemModule::SetList()
{
	OwnItemList.Empty();

	OwnItems.GenerateValueArray(OwnItemList);
}

uint64 UItemModule::GetExistItemUID(int32 InDataId)
{
	for(UItemData* Item : OwnItemList)
	{
		if (Item && Item->GetTableData().DataId == InDataId)
			return Item->GetUID();
	}

	return 0;
}

void UItemModule::LoadItem(uint64 InUID, int32 InDataId, int32 InAmount)
{
	FItemDataRow TableData = GetTableDataById(InDataId);

	if (TableData.DataId > 0)
	{
		UItemData* NewItem = NewObject<UItemData>(this);

		NewItem->Initialize(InUID, InAmount, TableData);

		OwnItems.Add(InUID, NewItem);
		SetList();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%d Item Table Data is not Exist!"), InDataId);
	}
}

//아이템류는 중복저장하면 안됨(장비는 가능)
void UItemModule::AddItem(int32 InDataId, int32 InAmount)
{
	FItemDataRow TableData = GetTableDataById(InDataId);
		
	if (uint64 ItemUID = GetExistItemUID(InDataId) > 0)
	{
		UItemData* Item = GetItem(ItemUID);
		if(Item->GetItemGroup() == EItemGroup::Equip)
		{
			AddNewItem(TableData, InAmount);
		}
		else
		{
			Item->AddAmount(InAmount);
			if (Item->GetAmount() <= 0)
			{
				OwnItems.Remove(ItemUID);
				SetList();
			}
		}
	}
	else
	{
		AddNewItem(TableData, InAmount);
	}	
}

//장비 판매 시 이 함수 호출
void UItemModule::AddItemAmount(uint64 InUID, int32 InAmount)
{
	UItemData* TargetItem = GetItem(InUID);

	if (!TargetItem) return;

	TargetItem->AddAmount(InAmount);

	if (TargetItem->GetAmount() <= 0)
	{
		OwnItems.Remove(InUID);
		SetList();
	}
}
void UItemModule::AddNewItem(FItemDataRow InTableData, int32 InAmount)
{
	if (InTableData.DataId > 0 && GameManager.IsValid())
	{
		UItemData* NewItem = NewObject<UItemData>(this);
		uint64 UID = GameManager->GetItemUID();

		NewItem->Initialize(UID, InAmount, InTableData);

		OwnItems.Add(UID, NewItem);
		SetList();
	}
}

FItemDataRow UItemModule::GetTableDataById(int32 InDataId)
{
	if (TableDataByDataId.Contains(InDataId))
	{
		return *TableDataByDataId.Find(InDataId);
	}

	return FItemDataRow();
}
