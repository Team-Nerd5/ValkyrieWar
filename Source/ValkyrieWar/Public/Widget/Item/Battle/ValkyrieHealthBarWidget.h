// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValkyrieHealthBarWidget.generated.h"

class UProgressBar;
class UTextBlock;
class AValkyrieCharacter;

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void RefreshHp(float CurrentHp, float MaxHp);

protected:
	UPROPERTY()
	TObjectPtr<AValkyrieCharacter> ValkyrieCharacter = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_HP = nullptr;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_HP = nullptr;
};
