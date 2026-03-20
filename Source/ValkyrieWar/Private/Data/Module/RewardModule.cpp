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
	RewardRowsByGroupId.Empty();

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

		FRewardArray& RewardArray = RewardRowsByGroupId.FindOrAdd(Row->GroupId);
		RewardArray.Rows.Add(*Row);
	}
}

bool URewardModule::GetRewardRowsByGroupId(int32 InGroupId, TArray<FRewardDataRow>& OutRows) const
{
	OutRows.Reset();

	if (InGroupId <= 0)
	{
		return false;
	}

	const FRewardArray* FoundArray = RewardRowsByGroupId.Find(InGroupId);
	if (!FoundArray || FoundArray->Rows.IsEmpty())
	{
		return false;
	}

	OutRows = FoundArray->Rows;
	return true;
}

const FRewardArray* URewardModule::FindRewardRowsByGroupId(int32 InGroupId) const
{
	if (InGroupId <= 0)
	{
		return nullptr;
	}

	return RewardRowsByGroupId.Find(InGroupId);
}
