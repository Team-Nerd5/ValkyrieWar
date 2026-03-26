#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Table/GameData/RewardDataRow.h"
#include "Data/Struct/RewardViewData.h"
#include "RewardModule.generated.h"

class UItemModule;
class UGoodsModule;
class UTexture2D;

/**
 * Reward 데이터 해석 모듈
 * - RewardId -> RewardDataRow 조회
 * - RewardType(Item/Goods)에 따라 실제 데이터(Item/Goods)로 변환
 * - UI에서 사용할 RewardViewData 생성
 */
UCLASS()
class VALKYRIEWAR_API URewardModule : public UBaseModule
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGameManager* InGameManager) override;

	/** RewardId(DataId)로 RewardDataRow 1개 조회 (복사 반환) */
	bool GetRewardRowsByDataId(int32 InDataId, FRewardDataRow& OutData) const;

	/** RewardId(DataId)로 RewardDataRow 조회 (읽기 전용) */
	const FRewardDataRow FindRewardByDataId(int32 InDataId) const;

	/**
	 * RewardDataRow + 확률(RewardRate) 기반으로
	 * UI 표시용 RewardViewData 생성
	 */
	bool MakeRewardViewData(
		const FRewardDataRow& InRewardRow,
		int32 InRewardRate,
		FRewardViewData& OutViewData) const;

protected:
	virtual void MakeData() override;

private:
	/** ItemModule 접근 (아이템 데이터 조회용) */
	TObjectPtr<UItemModule> GetItemModule() const;

	/** GoodsModule 접근 (재화 데이터 조회용) */
	TObjectPtr<UGoodsModule> GetGoodsModule() const;

	/** GoodsType을 UI용 이름 문자열로 변환 */
	FString GetGoodsDisplayName(EGoodsType InGoodsType) const;

private:
	/** RewardId(DataId) -> RewardDataRow 캐싱 */
	UPROPERTY()
	TMap<int32, FRewardDataRow> TableDataByDataId;
};
