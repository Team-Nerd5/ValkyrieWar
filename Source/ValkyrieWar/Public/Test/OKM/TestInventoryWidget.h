// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "Data/Game/ItemData.h"

#include "Test/OKM/TestItemObject.h"

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
	UFUNCTION(BlueprintCallable)
	virtual void OpenUI() override;
	UFUNCTION(BlueprintCallable)
	virtual void CloseUI() override;

	UFUNCTION()
	void FilterReset();
	UFUNCTION()
	void FilterWeapon();
	UFUNCTION()
	void FilterArmor();
	UFUNCTION()
	void FilterHelmet();
	UFUNCTION()
	void FIlterGrowth();
	
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> TileView = nullptr;

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

private:
	UInventorySystem* InventorySystem;

	TArray<UItemData*> CachedItemList;

	// 테스트용
public:
	UFUNCTION(BlueprintCallable)
	void InitTestList();
	UFUNCTION(BlueprintCallable)
	void AddTestList(UObject* InItemObject);

private:
	TArray<UTestItemObject*> TestList;
};
