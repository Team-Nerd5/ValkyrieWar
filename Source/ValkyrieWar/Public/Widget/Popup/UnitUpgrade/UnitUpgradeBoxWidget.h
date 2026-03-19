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

public:
	UFUNCTION()
	void Init(int32 InUnitId);

	UFUNCTION()
	void UpdateUpgradeInfo(int32 InUnitId);

	UFUNCTION()
	FORCEINLINE int32 GetUnitDataId() { return UnitDataId; }
protected:
	UFUNCTION()
	void OnUpgradeUnit();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UnitType = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> UnitIcon = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UnitLevel = nullptr;
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
	TObjectPtr<UTextBlock> NextLevel_Cost = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_UpgradeUnit = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ButtonText = nullptr;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setting|Color")
	FLinearColor ButtonBackGroundColor = FLinearColor(0.0f, 0.0f, 0.0f);
private:
	UPROPERTY()
	TObjectPtr<UUnitData> CachedUnitData = nullptr;

	int32 UnitDataId = 0;

	int32 MaxLevel = 10;
};
