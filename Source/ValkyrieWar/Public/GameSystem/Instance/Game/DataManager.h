// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/Module/ItemModule.h"
#include "Data/Module/ValkyrieModule.h"
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
	FORCEINLINE UValkyrieModule* GetValkyrieModule() const { return ValkyrieModule; }
private:
	UPROPERTY()
	TObjectPtr<UItemModule> ItemModule = nullptr;
	UPROPERTY()
	TObjectPtr<UValkyrieModule> ValkyrieModule = nullptr;
};
