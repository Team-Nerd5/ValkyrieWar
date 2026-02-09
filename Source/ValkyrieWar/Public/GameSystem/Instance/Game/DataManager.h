// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Module/ItemModule.h"
#include "DataManager.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UDataManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	//void InitItem(/*Data*/);

public:
	FORCEINLINE UItemModule* GetItemModule() const { return ItemModule; }
private:
	UPROPERTY()
	TObjectPtr<UItemModule> ItemModule = nullptr;
};
