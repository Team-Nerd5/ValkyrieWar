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

	bool GetRewardRowsByDataId(int32 InDataId, FRewardDataRow& OutData) const;
	const FRewardDataRow* FindRewardByDataId(int32 InDataId) const;

protected:
	virtual void MakeData() override;

private:
	UPROPERTY()
	TMap<int32, FRewardDataRow> TableDataByDataId;
};
