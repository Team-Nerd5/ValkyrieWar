// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Struct/StatValueData.h"
#include "ValkyrieStatWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieStatWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> AttackValueText = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> DefenceValueText = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> HealthValueText = nullptr;

public:
	void SetStatValue(FStatValueData InCharacterStat, FStatValueData InWeaponStat);
};
