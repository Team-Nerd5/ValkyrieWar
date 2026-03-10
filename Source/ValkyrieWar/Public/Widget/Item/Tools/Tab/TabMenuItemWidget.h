// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/CommonEnums.h"
#include "TabMenuItemWidget.generated.h"

//탭 선택 시 이미지 변경용 콜백
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabItemSelected, UTabMenuItemWidget*, InSelectedTab);
/**
 *
 */
UCLASS()
class VALKYRIEWAR_API UTabMenuItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	FOnTabItemSelected OnTabItemSelected;

	FORCEINLINE bool IsTabSelected() const { return bIsOn; }

	void SetTabSelected(bool bSelected);

	void SetTab(ETabType TabType, int32 InFilterIndex, FString InTabName);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> TabButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> TabName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> ButtonImage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> NormalImage;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UTexture2D> PressedImage;

	bool bIsOn = false;

	int32 FilterIndex;

	ETabType TabType = ETabType::Inventory;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void OnTabButtonClicked();
};
