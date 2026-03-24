#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"

#include "Data/Table/GameData/StageRewardDataRow.h"
#include "Data/Table/GameData/RewardDataRow.h"

#include "Data/Struct/StageRewardArray.h"
#include "Data/Struct/RewardViewData.h"
#include "StageRewardModule.generated.h"

class URewardModule;

/**
 * StageReward 데이터 관리 모듈
 * - RewardGroupId 기준으로 StageRewardDataRow 배열 관리
 * - StageReward -> Reward -> ViewData 흐름을 중계
 */
UCLASS()
class VALKYRIEWAR_API UStageRewardModule : public UBaseModule
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGameManager* InGameManager) override;

	/** RewardGroupId로 StageRewardRow 배열 복사 반환 */
	bool GetStageRewardRowsByGroupId(int32 InGroupId, TArray<FStageRewardDataRow>& OutRows) const;

	/** RewardGroupId로 StageRewardRow 배열 포인터 조회 */
	const FStageRewardArray* FindStageRewardRowsByGroupId(int32 InGroupId) const;

	/** StageRewardRow 1개를 RewardDataRow로 변환 (RewardId → RewardRow) */
	bool GetRewardRowsByStageRewardRow(const FStageRewardDataRow& InStageRewardRow, FRewardDataRow& OutData) const;

	/** RewardGroupId 기준으로 RewardDataRow 리스트 반환 */
	bool GetRewardRowsByStageRewardGroupId(int32 InGroupId, TArray<FRewardDataRow>& OutRows) const;

	/**
	 * RewardGroupId 기준으로 최종 UI용 RewardViewData 리스트 생성
	 * (StageReward → Reward → Item/Goods → ViewData)
	 */
	bool GetRewardViewDataByStageRewardGroupId(int32 InGroupId, TArray<FRewardViewData>& OutRows) const;

protected:
	virtual void MakeData() override;

private:
	/** RewardModule 접근 (RewardRow 조회 및 ViewData 생성용) */
	URewardModule* GetRewardModule() const;

	/** GoodsType을 UI용 이름 문자열로 변환 (보조 함수) */
	FString GetGoodsDisplayName(EGoodsType InGoodsType) const;

private:
	/** RewardGroupId -> StageRewardRow 배열 */
	UPROPERTY()
	TMap<int32, FStageRewardArray> StageRewardRowsByGroupId;
};
