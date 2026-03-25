// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "GachaResultWidget.generated.h"

class UWrapBox;
class UButton;
class UInventoryEntryWidget;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGachaResultWidget : public UBaseWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	virtual void OpenUI() override;

	UFUNCTION()
	void OnClickNextCharacter();

	UFUNCTION()
	void OnClickCloseGacha();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> WrapBox_Result = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWrapBox> RewardListPanel = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NextCharacterButton = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> CloseButton = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryEntryWidget> ItemSpawnClass;

	TArray<TObjectPtr<UObject>> ResultData;
public:
	void ShowGachaResults();

	void SetResultData(TArray<UObject*> InResultData);
};
