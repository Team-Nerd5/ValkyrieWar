// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "BaseModule.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UBaseModule : public UObject
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager);

protected:
	UPROPERTY()
	TWeakObjectPtr<UGameManager> GameManager = nullptr;
};
