// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/TextBlock.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "Data/Game/ItemData.h"
#include "TestEquipmentWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UTestEquipmentWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void EquipInfo(UItemData* InItemData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UID = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DataID = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemGroup = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EquipCharacterUID = nullptr;

private:
	UWorldEventSystem* WorldEventSystem;
};
