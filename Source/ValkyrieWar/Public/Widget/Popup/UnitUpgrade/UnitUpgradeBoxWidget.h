// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/TextBlock.h"
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

public:
	UFUNCTION()
	void Init(int32 InUnitId);

	UFUNCTION()
	void UpdateUpgradeInfo(int32 InUnitId);
protected:
	UFUNCTION()
	void OnUpgradeUnit();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UnitType = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentLevel_Attack = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentLevel_Health = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CurrentLevel_Defence = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NextLevel_Attack = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NextLevel_Health = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NextLevel_Defence = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_UpgradeUnit = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UUnitUpgradeStatModule> UnitUpgradeStatModule = nullptr;
	UPROPERTY()
	TObjectPtr<UUnitModule> UnitModule = nullptr;
	UPROPERTY()
	TObjectPtr<UUnitData> CachedUnitData = nullptr;

	int32 UnitDataId = 0;

	// StatGroupId를 어떻게 써야할지 이해가 되지 않아서 임시로 사용
	int32 StatGroupId1 = 670001;
	int32 StatGroupId2 = 670002;
	int32 StatGroupId3 = 670003;
};
