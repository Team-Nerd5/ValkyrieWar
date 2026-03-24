// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Data/Enum/DataEnums.h"
#include "ValkyrieListItemWidget.generated.h"

class UImage;
class UTextBlock;
class UValkyrieData;

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieListItemWidget : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeOnItemSelectionChanged(bool bIsSelected) override;

	void SetStar(EGradeType InGrade);

public:
	void SetData(UValkyrieData* InValkyrieData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ValkyrieImage = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star1 = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star2 = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star3 = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star4 = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Star5 = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ValkyrieNameText = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> SelectedFrame = nullptr;


	UPROPERTY()
	TObjectPtr<UValkyrieData> CachedData = nullptr;


};
