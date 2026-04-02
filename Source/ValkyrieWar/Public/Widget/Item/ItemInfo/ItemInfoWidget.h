// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Data/Game/ItemData.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

#include "ItemInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UItemInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(int32 InItemUID, UWidget* ContextWidget);

private:
	void EquipItemInfo(UItemData* InItemData);

	void SetvisibleInfo(EItemGroup InItemGroup);

	void SetPosition(int32 InItemUID, UWidget* ContextWidget);

protected:
	// 텍스트 상위 박스
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> NameBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> SellPriceBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> AmountBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> EquippedCharacterBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> EquipGroupBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> HealthBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> AttackBox = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> DefenceBox = nullptr;


	// 공통으로 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemName = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemSellPrice = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemAmount = nullptr;

	// 장비일 경우 표시
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemEquippedCharacter = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemEquipGroup = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemStat_Health = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemStat_Attack = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemStat_Defence = nullptr;

private:
	UPROPERTY()
	TWeakObjectPtr<class UCanvasPanelSlot> CachedCanvasSlot = nullptr;
};
