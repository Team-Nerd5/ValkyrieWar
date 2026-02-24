// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "Data/Game/ItemData.h"
#include "CharacterInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UCharacterInfoWidget : public UBaseWidget
{
	GENERATED_BODY()
	// 케릭터 정보창 위젯
	// 장비 장착 및 해제 기능
	// 장비에 관련된 아이템만 확인가능
	// 캐릭터 정보창에서 캐릭터 선택창도 필요한가?
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void OpenUI() override;
	UFUNCTION(BlueprintCallable)
	virtual void CloseUI() override;

protected:
	UFUNCTION()
	void FilterReset();
	UFUNCTION()
	void FilterWeapon();
	UFUNCTION()
	void FilterArmor();
	UFUNCTION()
	void FilterHelmet();

	UFUNCTION()
	void ItemClicked(UObject* InItemData);

	UFUNCTION()
	void UpdateEquipmentUi(uint64 InCharacterUID, EEquipGroup InEquipGroup);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> TileView = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Close = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_FilterReset = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_FilterWeapon = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_FilterArmor = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_FilterHelmet = nullptr;

private:
	UInventorySystem* InventorySystem;

	UWorldEventSystem* EventSystem;

	UPROPERTY()
	TArray<UItemData*> CachedItemList;

	// 현재 선택한 캐릭터 UID 저장용
	uint64 CurrentCharacterUID = 0;
};
