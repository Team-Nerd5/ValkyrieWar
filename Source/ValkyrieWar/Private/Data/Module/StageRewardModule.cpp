#include "Data/Module/StageRewardModule.h"

#include "Data/Module/RewardModule.h"
#include "Data/Module/ItemModule.h"
#include "Data/Module/GoodsModule.h"

#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/Game/DataManager.h"

#include "Data/Table/GameData/ItemDataRow.h"
#include "Data/Table/GameData/GoodsDataRow.h"

void UStageRewardModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::StageReward);
		MakeData();
	}

	SendDataLoadComplete();
}

void UStageRewardModule::MakeData()
{
	StageRewardRowsByGroupId.Empty();

	if (!DataTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] DataTable is null"));
		return;
	}

	TArray<FStageRewardDataRow*> AllRows;
	DataTable->GetAllRows<FStageRewardDataRow>(TEXT("StageRewardModule_Init"), AllRows);

	for (const FStageRewardDataRow* Row : AllRows)
	{
		if (!Row)
		{
			continue;
		}

		FStageRewardArray& RewardArray = StageRewardRowsByGroupId.FindOrAdd(Row->GroupId);
		RewardArray.Rows.Add(*Row);
	}

	UE_LOG(LogTemp, Log, TEXT("[StageRewardModule] Loaded Group Count = %d"), StageRewardRowsByGroupId.Num());
}

bool UStageRewardModule::GetStageRewardRowsByGroupId(int32 InGroupId, TArray<FStageRewardDataRow>& OutRows) const
{
	OutRows.Reset();

	if (InGroupId <= 0)
	{
		return false;
	}

	const FStageRewardArray* FoundArray = StageRewardRowsByGroupId.Find(InGroupId);
	if (!FoundArray || FoundArray->Rows.IsEmpty())
	{
		return false;
	}

	OutRows = FoundArray->Rows;
	return true;
}

const FStageRewardArray* UStageRewardModule::FindStageRewardRowsByGroupId(int32 InGroupId) const
{
	if (InGroupId <= 0)
	{
		return nullptr;
	}

	return StageRewardRowsByGroupId.Find(InGroupId);
}

bool UStageRewardModule::GetRewardRowByStageRewardRow(const FStageRewardDataRow& InStageRewardRow, FRewardDataRow& OutData) const
{
	if (InStageRewardRow.RewardId <= 0)
	{
		return false;
	}

	const TObjectPtr<URewardModule> RewardModule = GetRewardModule();
	if (!RewardModule)
	{
		return false;
	}

	return RewardModule->GetRewardRowById(InStageRewardRow.RewardId, OutData);
}

bool UStageRewardModule::GetRewardRowsByStageRewardGroupId(int32 InGroupId, TArray<FRewardDataRow>& OutRows) const
{
	OutRows.Reset();

	if (InGroupId <= 0)
	{
		return false;
	}

	const TObjectPtr<URewardModule> RewardModule = GetRewardModule();
	if (!RewardModule)
	{
		return false;
	}

	const FStageRewardArray* StageRewardArray = StageRewardRowsByGroupId.Find(InGroupId);
	if (!StageRewardArray || StageRewardArray->Rows.IsEmpty())
	{
		return false;
	}

	for (const FStageRewardDataRow& StageRewardRow : StageRewardArray->Rows)
	{
		const FRewardDataRow* RewardRow = RewardModule->FindRewardRowById(StageRewardRow.RewardId);
		if (!RewardRow)
		{
			UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] Reward not found. RewardId=%d"), StageRewardRow.RewardId);
			continue;
		}

		OutRows.Add(*RewardRow);
	}

	return OutRows.Num() > 0;
}

bool UStageRewardModule::GetRewardViewDataByStageRewardGroupId(int32 InGroupId, TArray<FRewardViewData>& OutRows) const
{
	OutRows.Reset();

	if (InGroupId <= 0)
	{
		return false;
	}

	const FStageRewardArray* StageRewardArray = FindStageRewardRowsByGroupId(InGroupId);
	if (!StageRewardArray || StageRewardArray->Rows.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] StageReward group not found. GroupId=%d"), InGroupId);
		return false;
	}

	const TObjectPtr<URewardModule> RewardModule = GetRewardModule();
	if (!RewardModule)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] RewardModule is null"));
		return false;
	}

	if (!GameManager.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] GameManager is invalid"));
		return false;
	}

	const TObjectPtr<UDataManager> DataManager = GameManager->GetSubsystem<UDataManager>();
	if (!DataManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] DataManager is null"));
		return false;
	}

	const TObjectPtr<UItemModule> ItemModule = DataManager->GetItemModule();
	const TObjectPtr<UGoodsModule> GoodsModule = DataManager->GetGoodsModule();

	for (const FStageRewardDataRow& StageRewardRow : StageRewardArray->Rows)
	{
		const FRewardDataRow* RewardRow = RewardModule->FindRewardRowById(StageRewardRow.RewardId);
		if (!RewardRow)
		{
			UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] RewardRow not found. RewardId=%d"), StageRewardRow.RewardId);
			continue;
		}

		FRewardViewData ViewData;
		ViewData.RewardType = RewardRow->RewardType;
		ViewData.RewardId = RewardRow->DataId;
		ViewData.RewardDataId = RewardRow->RewardDataId;
		ViewData.Amount = RewardRow->Amount;
		ViewData.RewardRate = StageRewardRow.RewardRate;

		switch (RewardRow->RewardType)
		{
		case ERewardType::Item:
		{
			if (!ItemModule)
			{
				UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] ItemModule is null"));
				continue;
			}

			const FItemDataRow ItemRow = ItemModule->GetTableDataById(RewardRow->RewardDataId);
			if (ItemRow.DataId <= 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] Invalid ItemData. RewardDataId=%d"), RewardRow->RewardDataId);
				continue;
			}

			ViewData.Name = ItemRow.Name;
			ViewData.Icon = ItemRow.Icon;
			break;
		}

		case ERewardType::Goods:
		{
			if (!GoodsModule)
			{
				UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] GoodsModule is null"));
				continue;
			}

			const EGoodsType GoodsType = static_cast<EGoodsType>(RewardRow->RewardDataId);
			const FGoodsDataRow GoodsRow = GoodsModule->GetTableData(GoodsType);

			ViewData.Name = GetGoodsDisplayName(GoodsRow.GoodsType);
			ViewData.Icon = GoodsRow.Icon;
			break;
		}

		case ERewardType::Character:
			UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] Character reward is not supported yet. RewardId=%d"), RewardRow->DataId);
			continue;

		case ERewardType::None:
		default:
			UE_LOG(LogTemp, Warning, TEXT("[StageRewardModule] Invalid RewardType. RewardId=%d"), RewardRow->DataId);
			continue;
		}

		OutRows.Add(ViewData);
	}

	return OutRows.Num() > 0;
}

TObjectPtr<URewardModule> UStageRewardModule::GetRewardModule() const
{
	if (!GameManager.IsValid())
	{
		return nullptr;
	}

	const TObjectPtr<UDataManager> DataManager = GameManager->GetSubsystem<UDataManager>();
	if (!DataManager)
	{
		return nullptr;
	}

	return DataManager->GetRewardModule();
}

FString UStageRewardModule::GetGoodsDisplayName(EGoodsType InGoodsType) const
{
	switch (InGoodsType)
	{
	case EGoodsType::Ticket:
		return TEXT("티켓");
	case EGoodsType::Gem:
		return TEXT("젬");
	case EGoodsType::Gold:
		return TEXT("골드");
	case EGoodsType::None:
	default:
		return TEXT("없음");
	}
}
