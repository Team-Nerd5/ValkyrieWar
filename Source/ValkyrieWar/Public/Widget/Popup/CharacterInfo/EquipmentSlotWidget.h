// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/Image.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "EquipmentSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UEquipmentSlotWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void RefreshEquipment(uint64 InCharacterUID);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> WeaponIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> HelmetIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ArmorIcon = nullptr;

private:
	UInventorySystem* InventorySystem;
};
