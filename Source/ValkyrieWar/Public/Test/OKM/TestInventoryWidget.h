// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "Data/Game/ItemData.h"
#include "TestInventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UTestInventoryWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	virtual void OpenUI() override;
	virtual void CloseUI() override;

	void FilterReset();
	void FilterWeapon();
	void FilterArmor();
	void FilterHelmet();
	void FIlterGrowth();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FilterResetButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FilterWeaponButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FilterArmorButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FilterHelmetButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> FilterGrowthButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> TileView = nullptr;

private:
	UInventorySystem* InventorySystem;

	TArray<UItemData*> CachedItemList;
};
