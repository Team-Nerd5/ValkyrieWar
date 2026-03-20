#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/RewardDataRow.h"
#include "Data/Struct/RewardArray.h"
#include "RewardModule.generated.h"

UCLASS()
class VALKYRIEWAR_API URewardModule : public UBaseModule
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGameManager* InGameManager) override;

	bool GetRewardRowsByGroupId(int32 InGroupId, TArray<FRewardDataRow>& OutRows) const;
	const FRewardArray* FindRewardRowsByGroupId(int32 InGroupId) const;

protected:
	virtual void MakeData() override;

private:
	UPROPERTY()
	TMap<int32, FRewardArray> RewardRowsByGroupId;
};
