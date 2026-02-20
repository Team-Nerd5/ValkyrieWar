// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Instance/Game/GameManager.h"
#include "BaseModule.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class VALKYRIEWAR_API UBaseModule : public UObject
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager);

protected:
	virtual void MakeData() {}

	virtual void SendDataLoadComplete();
protected:
	UPROPERTY()
	TWeakObjectPtr<UGameManager> GameManager = nullptr;

	UPROPERTY()
	TObjectPtr<UDataTable> DataTable = nullptr;
};
