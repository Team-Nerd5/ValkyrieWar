#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/StageRewardDataRow.h"
#include "Data/Table/GameData/RewardDataRow.h"
#include "Data/Struct/StageRewardArray.h"
#include "StageRewardModule.generated.h"

class URewardModule;
struct FRewardArray;

UCLASS()
class VALKYRIEWAR_API UStageRewardModule : public UBaseModule
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGameManager* InGameManager) override;

	bool GetStageRewardRowsByGroupId(int32 InGroupId, TArray<FStageRewardDataRow>& OutRows) const;
	const FStageRewardArray* FindStageRewardRowsByGroupId(int32 InGroupId) const;

	bool GetRewardRowsByStageRewardRow(const FStageRewardDataRow& InStageRewardRow, FRewardDataRow& OutData) const;
	bool GetRewardRowsByStageRewardGroupId(int32 InGroupId, TArray<FRewardDataRow>& OutRows) const;

protected:
	virtual void MakeData() override;

private:
	//제발좀 원시포인터...
	URewardModule* GetRewardModule() const;

private:
	UPROPERTY()
	TMap<int32, FStageRewardArray> StageRewardRowsByGroupId;
};
