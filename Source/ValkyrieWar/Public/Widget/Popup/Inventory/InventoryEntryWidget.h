// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Data/Game/ItemData.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "InventoryEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UInventoryEntryWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	// 인벤토리 타일뷰에서 들어온 아이템을 UI에 표시하기 위한 함수
	UFUNCTION()
	void Init(UItemData* InData);

	void SetData(UItemData* ItemData);
	void SetData(UValkyrieData* InValkyrieData);
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	// 아이템의 양 업데이트 할 때 바인드할 함수
	UFUNCTION()
	void OnAmountChanged(uint64 InUID);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Amount = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Icon = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> BackGround = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SelectImage = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> AmountPanel = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EquippingCharacterIcon = nullptr;	// 캐릭터가 장착한 아이템을 표시하기 위한 캐릭터아이콘

private:
	UPROPERTY()
	TObjectPtr<UItemData> CachedItemData = nullptr;

	bool IsSelected = false;
};
