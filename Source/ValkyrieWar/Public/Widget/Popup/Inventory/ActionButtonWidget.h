// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/Button.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "Data/Game/ItemData.h"
#include "ActionButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UActionButtonWidget : public UBaseWidget
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
	// 인벤토리 타입에 따라 표시할 버튼 세팅
	void SetVisibleButton(EUIType InCurrentUIType);

	// 인벤토리에서 선택한 아이템 받는 함수
	void SetupItem(UItemData* InItemData);

	// 선택된 캐릭터 UID 받는 함수
	void SetupCharacterUID(uint64 InCharacterUID);

protected:
	// 판매 버튼 클릭시 실행
	UFUNCTION()
	void Sell();
	// 장착 버튼 클릭시 실행
	UFUNCTION()
	void Equip();
	// 장착 해제 버튼 클릭시 실행
	UFUNCTION()
	void Unequip();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Sell = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Equip = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Unequip = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Cancel = nullptr;

private:
	UPROPERTY()
	TObjectPtr<UInventorySystem> InventorySystem;
	UPROPERTY()
	TObjectPtr<UWorldEventSystem> WorldEventSystem;

	UPROPERTY()
	TObjectPtr<UItemData> CachedItemData = nullptr;
	uint64 CachedCharacterUID = 0;

};
