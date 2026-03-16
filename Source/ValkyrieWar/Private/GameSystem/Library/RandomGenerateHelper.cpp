// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Library/RandomGenerateHelper.h"
#include "Data/Enum/DataEnums.h"
#include "Kismet/KismetMathLibrary.h"

int32 URandomGenerateHelper::GetRandomValkyrie(UDataManager* InDataManager, int32 InGroupId)
{
	if (InDataManager)
	{
		TMap<int32, FGachaRandomDataRow> GachaData = InDataManager->GetGachaRandomModule()->GetRandomData(InGroupId);

		if (IsGachaDataValid(GachaData))
		{
			int32 Rand = FMath::RandHelper(10000);

			if (Rand < GachaData.FindChecked(static_cast<int32>(EGradeType::Legend)).Rate)
			{
				//5성이면 Array를 찾아서
				TArray<int32> TargetValkyries = GachaData.FindChecked(static_cast<int32>(EGradeType::Legend)).TargetValkyires;

				if (TargetValkyries.Num() == 0)
				{
					TArray<int32> AllData = InDataManager->GetValkyrieModule()->GetAllByGrade(EGradeType::Legend);
					if (AllData.Num() == 0)
						return 0;

					int32 RandomIndex = FMath::RandHelper(AllData.Num());

					return AllData[RandomIndex];
				}
				else
				{
					int32 RandomIndex = FMath::RandHelper(TargetValkyries.Num());

					return TargetValkyries[RandomIndex];
				}

			}
			else if (Rand < GachaData.FindChecked(static_cast<int32>(EGradeType::Unique)).Rate)
			{

			}
			else if (Rand < GachaData.FindChecked(static_cast<int32>(EGradeType::Rare)).Rate)
			{

			}
			else if (Rand < GachaData.FindChecked(static_cast<int32>(EGradeType::Uncommon)).Rate)
			{

			}
			else
			{

			}
		}
	}
	

	return 0;
}

bool URandomGenerateHelper::IsGachaDataValid(TMap<int32, FGachaRandomDataRow> InData)
{
	if (InData.Num() != 5)
		return false;

	int32 RateSum = 0;
	for (auto Data : InData)
	{
		RateSum += Data.Value.Rate;
	}
	if (RateSum != 10000)
	{
		UE_LOG(LogTemp, Error, TEXT("Random Rate Total is not correct!"));
		return false;
	}

	return true;
}
