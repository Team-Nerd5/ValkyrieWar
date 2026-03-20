#include "Data/Module/StageRewardModule.h"

#include "Data/Module/RewardModule.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/Game/DataManager.h"

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

bool UStageRewardModule::GetRewardRowsByStageRewardRow(const FStageRewardDataRow& InStageRewardRow, TArray<FRewardDataRow>& OutRows) const
{
	OutRows.Reset();

	if (InStageRewardRow.RewardId <= 0)
	{
		return false;
	}

	URewardModule* RewardModule = GetRewardModule();
	if (!RewardModule)
	{
		return false;
	}

	return RewardModule->GetRewardRowsByGroupId(InStageRewardRow.RewardId, OutRows);
}

bool UStageRewardModule::GetRewardRowsByStageRewardGroupId(int32 InGroupId, TArray<FRewardDataRow>& OutRows) const
{
	OutRows.Reset();

	if (InGroupId <= 0)
	{
		return false;
	}

	URewardModule* RewardModule = GetRewardModule();
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
		const FRewardArray* RewardArray = RewardModule->FindRewardRowsByGroupId(StageRewardRow.RewardId);
		if (!RewardArray || RewardArray->Rows.IsEmpty())
		{
			continue;
		}

		OutRows.Append(RewardArray->Rows);
	}

	return OutRows.Num() > 0;
}

URewardModule* UStageRewardModule::GetRewardModule() const
{
	if (!GameManager.IsValid())
	{
		return nullptr;
	}

	UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>();
	if (!DataManager)
	{
		return nullptr;
	}

	return DataManager->GetRewardModule();
}
