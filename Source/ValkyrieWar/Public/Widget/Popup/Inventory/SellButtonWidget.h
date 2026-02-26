// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/Button.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "Data/Game/ItemData.h"
#include "SellButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API USellButtonWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void OpenUI() override;
	UFUNCTION(BlueprintCallable)
	virtual void CloseUI() override;

public:
	// 인벤토리에서 선택한 아이템 받는 함수
	void SetupSellItem(UItemData* InItemData);

protected:
	// 판매 버튼 클릭시 실행
	UFUNCTION()
	void Sell();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Sell = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Cancel = nullptr;

private:
	UInventorySystem* InventorySystem;

	UWorldEventSystem* WorldEventSystem;

	UItemData* CachedItemData = nullptr;
};
