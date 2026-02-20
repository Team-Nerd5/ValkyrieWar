// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Data/Game/ItemData.h"
#include "Components/Button.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "TestPopupWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UTestPopupWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	virtual void OpenUI() override;

	UFUNCTION()
	virtual void CloseUI() override;

public:
	UFUNCTION(BlueprintCallable)
	void InitMenu(UItemData* ItemData);

protected:
	UFUNCTION()
	void Sell();

	UFUNCTION()
	void Use();

	UFUNCTION()
	void Equip();

	UFUNCTION()
	void UnEquip();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Sell = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Cancel = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Use = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Equip = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_UnEquip = nullptr;

private:
	UInventorySystem* InventorySystem;

	UWorldEventSystem* WorldEventSystem;

	UItemData* CachedItemData = nullptr;
};
