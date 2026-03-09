// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "Widget/Popup/Inventory/ActionButtonWidget.h"
#include "Widget/Popup/Inventory/EquipmentSlotWidget.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "Data/Game/ItemData.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UInventoryWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void OpenUI() override;
	UFUNCTION(BlueprintCallable)
	virtual void CloseUI() override;

protected:
	void InitItemList();

	UFUNCTION()
	void OnItemClicked(UObject* InItemData);
	// 보유중인 캐릭터 타일뷰 클릭 했을 때 실행될 함수
	//UFUNCTION()
	//void OnCharacterClicked(UObject* InCharacterData);

	UFUNCTION()
	void OnClickClose();

private:

	void SortInventory();

protected:
	//UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemListWidget> ItemListWidget = nullptr;

	UPROPERTY(EditAnywhere)
	TMap<EInventoryFilterType, FString> InventoryTabNameData;

private:

	UPROPERTY()
	TArray<TObjectPtr<UItemData>> OriginItems;

	EInventoryFilterType CurrentFilterType = EInventoryFilterType::All;

};
