// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/TileView.h"
#include "Components/Button.h"
#include "Widget/Popup/CharacterInfo/EquipButtonWidget.h"
#include "Widget/Popup/CharacterInfo/EquipmentSlotWidget.h"
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
	
	// 현재 필터링된 캐릭터 장비 인벤토리 업데이트 함수
	UFUNCTION()
	void UpdateFilteredInventory();
	// 선택된 캐릭터가 장착한 장비칸 UI업데이트 함수
	UFUNCTION()
	void UpdateEquipmentForUID(uint64 InCharacterUID);

	// 장비 인벤토리 타일뷰 클릭했을 때 실행될 함수
	UFUNCTION()
	void EquipmentTileItemClicked(UObject* InItemData);
	// 캐릭터 목록 타일뷰 클릭했을 때 실행될 함수
	//UFUNCTION()
	//void CharacterTileItemClicked(UObject* InItemData);

public:
	// 선택한 캐릭터 UID저장용(아직 사용x)
	FORCEINLINE void SetSelectedCharacterUID(uint64 InCharacterUID) { SelectedCharacterUID = InCharacterUID; }

protected:
	// 장비 인벤토리
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTileView> EquipmentTileView = nullptr;
	// 캐릭터 목록
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<UTileView> CharacterTileView = nullptr;

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

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEquipButtonWidget> EquipButtonWidget = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEquipmentSlotWidget> EquipmentSlotWidget = nullptr;

private:
	UInventorySystem* InventorySystem;

	UWorldEventSystem* WorldEventSystem;

	UPROPERTY()
	TArray<UItemData*> CachedItemList;

	EEquipGroup CurrentEquipGroup = EEquipGroup::None;

	// 현재 선택한 캐릭터 UID 저장용
	uint64 SelectedCharacterUID = 0;
};
