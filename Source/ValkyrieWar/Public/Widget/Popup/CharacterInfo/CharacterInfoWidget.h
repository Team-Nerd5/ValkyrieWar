// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "CharacterInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UCharacterInfoWidget : public UBaseWidget
{
	GENERATED_BODY()
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	virtual void OpenUI() override;
	virtual void CloseUI() override;
};
