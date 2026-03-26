// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/DataEnums.h"
#include "InfoEquipItemWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UInfoEquipItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitEquip(EEquipGroup InEquipGroup, TSoftObjectPtr<UTexture2D> InIcon);
	void SetEquip(TSoftObjectPtr<UTexture2D> InIcon);

	UFUNCTION()
	void UnEquipItem();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> TypeIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ItemIcon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> UnEquipButton = nullptr;

	EEquipGroup EquipGroup = EEquipGroup::None;

};
