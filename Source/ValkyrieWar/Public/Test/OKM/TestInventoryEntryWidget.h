// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/TextBlock.h"
#include "TestInventoryEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UTestInventoryEntryWidget : public UBaseWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	UFUNCTION()
	void Init(UObject* InData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> UID = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DataID = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ItemGroup = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Amount = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> EquipCharacterUID = nullptr;

};
