// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "CharacterInfoWidget.generated.h"

class UItemData;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UCharacterInfoWidget : public UBaseWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnInitialized() override;

	void InitItemList();

	UFUNCTION()
	void OnItemClicked(UObject* InItemData);

	UFUNCTION()
	void OnInventoryUpdate();

	UFUNCTION()
	void OnTabMenuChanged(int32 InSelectedTab);

public:
	virtual void OpenUI() override;
	virtual void CloseUI() override;
protected:
	//인벤토리
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UItemListWidget> ItemListWidget = nullptr;

	UPROPERTY(EditAnywhere)
	TMap<ECharacterInfoFilterType, FString> InventoryTabNameData;
private:

	UPROPERTY()
	TArray<TObjectPtr<UItemData>> OriginItems;

	ECharacterInfoFilterType CurrentFilterType = ECharacterInfoFilterType::All;

};
