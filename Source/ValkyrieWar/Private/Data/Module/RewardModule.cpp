#include "Data/Module/RewardModule.h"

void URewardModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Reward);
		MakeData();
	}

	SendDataLoadComplete();
}

void URewardModule::MakeData()
{
	TableDataByDataId.Empty();

	if (!DataTable)
	{
		return;
	}

	TArray<FRewardDataRow*> AllRows;
	DataTable->GetAllRows<FRewardDataRow>(TEXT("RewardModule_Init"), AllRows);

	for (const FRewardDataRow* Row : AllRows)
	{
		if (!Row)
		{
			continue;
		}

		TableDataByDataId.Add(Row->DataId, *Row);
	}
}

bool URewardModule::GetRewardRowsByDataId(int32 InDataId, FRewardDataRow& OutData) const
{
	if (InDataId <= 0)
	{
		return false;
	}

	const FRewardDataRow* FoundReward = TableDataByDataId.Find(InDataId);
	if (!FoundReward)
	{
		return false;
	}

	OutData = *FoundReward;
	return true;
}

const FRewardDataRow* URewardModule::FindRewardByDataId(int32 InDataId) const
{
	if (InDataId <= 0)
	{
		return nullptr;
	}

	return TableDataByDataId.Find(InDataId);
}
