// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/Button.h"

#include "Data/Module/UnitUpgradeStatModule.h"
#include "Data/Module/UnitModule.h"

#include "UnitUpgradeBoxWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UUnitUpgradeBoxWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	void Init(TObjectPtr<UUnitData> InUnitData);

	// UI 정보 업데이트
	void UpdateUpgradeInfo();

protected:
	// 버튼 업테이트 테스트 함수
	UFUNCTION()
	void OnTestGoodsChangedAmount(EGoodsType InGoodsType, uint64 InAmount);
	
	// 재화의 변화가 있을 때 바인딩 할 함수(버튼 활성화 여부를 체크)
	UFUNCTION()
	void OnGoodsChangedAmount();
	// 업그레이드 버튼 바인딩 함수
	UFUNCTION()
	void OnUpgradeUnit();

private:
	// 재화의 상태에 따라 버튼 활성화/비활성화 설정 함수
	UFUNCTION()
	void CheckEnoughCost();
	// 버튼 활성화/비활성화 설정 함수
	UFUNCTION()
	void SetEnableButton(bool IsActive);

protected:
	// 유닛 정보
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UnitType = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> UnitIcon = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UnitLevel = nullptr;

	// 유닛의 현재 스텟
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentLevel_Attack = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentLevel_Health = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentLevel_Defence = nullptr;

	// 다음 레벨에 오를 스텟
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NextLevel_Attack = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NextLevel_Health = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NextLevel_Defence = nullptr;

	// 다음 레벨업 할 때 사용될 재화
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> NextLevel_CostTypeIcon = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NextLevel_Cost = nullptr;

	// 업그레이드 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_UpgradeUnit = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonText = nullptr;

protected:
	// 버튼 비활성화 색상설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting|Color")
	FLinearColor ButtonDisableColor = FLinearColor(0.0f, 0.0f, 0.0f);
private:
	UPROPERTY()
	TObjectPtr<UUnitData> CachedUnitData = nullptr;
	UPROPERTY()
	int32 CachedUnitDataId = 0;
	UPROPERTY()
	TWeakObjectPtr<UUnitUpgradeData> NextLevelData = nullptr;
};
