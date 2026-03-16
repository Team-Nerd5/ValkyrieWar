// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Game/UnitUpgradeData.h"
#include "GameSystem/Instance/Game/DataManager.h"

void UUnitUpgradeData::MakeData(const FUnitUpgradeStatDataRow InTableData, UGameManager* InGameManager)
{
	TableData = InTableData;

	if (UDataManager* DataManager = InGameManager->GetSubsystem<UDataManager>())
	{
		StatData = DataManager->GetStatGroupModule()->GetData(TableData.StatDataId);

	}
}
