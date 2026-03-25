// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/GoodsModule.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

void UGoodsModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Goods);
		MakeData();
	}

	SendDataLoadComplete();
}

FGoodsDataRow UGoodsModule::GetTableDataById(int32 InDataId)
{
	return TableDataById.FindChecked(InDataId);
}

FGoodsDataRow UGoodsModule::GetTableData(EGoodsType InKey)
{
	return TableDataByType.FindChecked(InKey);
}
void UGoodsModule::Update(EGoodsType InType, uint64 InAmount)
{
	uint64* Amount = GoodsAmount.Find(InType);
	if (Amount)
	{
		*Amount = InAmount;
	}
	else
	{
		GoodsAmount.Add(InType, InAmount);
	}
}
void UGoodsModule::Add(EGoodsType InType, int64 InAmount)
{
	if (InAmount < 0 && !IsEnough(InType, -InAmount))
	{
		//빼야하는데 부족함..
		return;
	}

	uint64* Amount = GoodsAmount.Find(InType);
	if (Amount)
	{
		*Amount += InAmount;
	}
	else
	{
		if(InAmount > 0)
			GoodsAmount.Add(InType, InAmount);
	}

	if (USaveManager* SaveManager = GameManager->GetSubsystem<USaveManager>())
	{
		SaveManager->AddGoods(InType, InAmount);
	}

	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Widget.OnGoodsUpdate.Broadcast(InType, Amount ? *Amount : 0);
	}
}

bool UGoodsModule::IsEnough(EGoodsType InType, uint64 InCheckAmount)
{
	uint64* Amount = GoodsAmount.Find(InType);
	if (Amount)
	{
		return *Amount >= InCheckAmount;
	}
	
	return false;
}

void UGoodsModule::MakeData()
{
	if (DataTable)
	{
		TArray<FGoodsDataRow*> AllRows;
		DataTable->GetAllRows<FGoodsDataRow>(TEXT("GoodsModule_Init"), AllRows);

		for (FGoodsDataRow* Item : AllRows)
		{
			if (!Item) continue;

			TableDataById.Add(Item->DataId, *Item);
			TableDataByType.Add(Item->GoodsType, *Item);
		}
	}
}
