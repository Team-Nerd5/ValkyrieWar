// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameBaseLibrary.generated.h"

class UWorldEventSystem;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UGameBaseLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "MyGame|Utils", meta = (WorldContext = "WorldContextObject"))
	static UWorldEventSystem* GetWorldEventSystem(const UObject* WorldContextObject);
};
