// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Components/Button.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "Data/Game/ItemData.h"
#include "EquipButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UEquipButtonWidget : public UBaseWidget
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
	// 선택한 아이템 받는 함수
	UFUNCTION()
	void SetupEquipItem(UItemData* InItemData);

protected:
	// 장착 버튼 클릭시 실행
	UFUNCTION()
	void Equip();
	// 장착 해제 버튼 클릭시 실행
	UFUNCTION()
	void Unequip();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Equip = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Unequip = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Btn_Cancel = nullptr;

private:
	UInventorySystem* InventorySystem;

	UWorldEventSystem* WorldEventSystem;

	UItemData* CachedItemData = nullptr;

	const uint64 TempCharacterUID = 1001001;	// 테스트용 캐릭터UID(케릭터 선택창 구현시 삭제 예정)
};
