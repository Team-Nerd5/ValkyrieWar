// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Library/RandomGenerateHelper.h"
#include "Data/Enum/DataEnums.h"
#include "Kismet/KismetMathLibrary.h"

int32 URandomGenerateHelper::GetRandomValkyrie(UDataManager* InDataManager, int32 InGroupId)
{
	if (InDataManager)
	{
		TMap<EGradeType, FGachaRandomDataRow> GachaData = InDataManager->GetGachaRandomModule()->GetRandomData(InGroupId);

		if (IsGachaDataValid(GachaData))
		{
			int32 Rand = FMath::RandHelper(10000);

			if (Rand < GachaData.FindChecked(EGradeType::Legend).Rate)
			{
				//5성이면 Array를 찾아서
				TArray<int32> TargetValkyries = GachaData.FindChecked(EGradeType::Legend).TargetValkyires;

				if (TargetValkyries.Num() == 0)
				{
					return GetRandomValkyrieInGrade(InDataManager, EGradeType::Legend);
				}
				else
				{
					int32 RandomIndex = FMath::RandHelper(TargetValkyries.Num());

					return TargetValkyries[RandomIndex];
				}

			}
			else if (Rand < GachaData.FindChecked(EGradeType::Unique).Rate)
			{
				return GetRandomValkyrieInGrade(InDataManager, EGradeType::Unique);
			}
			else if (Rand < GachaData.FindChecked(EGradeType::Rare).Rate)
			{
				return GetRandomValkyrieInGrade(InDataManager, EGradeType::Rare);
			}
			else if (Rand < GachaData.FindChecked(EGradeType::Uncommon).Rate)
			{
				return GetRandomValkyrieInGrade(InDataManager, EGradeType::Uncommon);
			}
			else
			{
				return GetRandomValkyrieInGrade(InDataManager, EGradeType::Common);
			}
		}
	}	

	return 0;
}

bool URandomGenerateHelper::IsGachaDataValid(TMap<EGradeType, FGachaRandomDataRow> InData)
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

int32 URandomGenerateHelper::GetRandomValkyrieInGrade(UDataManager* InDataManager, EGradeType InGrade)
{
	TArray<int32> AllData = InDataManager->GetValkyrieModule()->GetAllByGrade(EGradeType::Legend);
	if (AllData.Num() == 0)
		return 0;

	int32 RandomIndex = FMath::RandHelper(AllData.Num());

	return AllData[RandomIndex];
}
