// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TestItemObject.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class VALKYRIEWAR_API UTestItemObject : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	int32 GetTestObjectAmount() { return Amount; }
	UFUNCTION(BlueprintCallable)
	void SetTestObjectAmount(int32 InNewAmount) { Amount = InNewAmount; }

	UFUNCTION(BlueprintCallable)
	int32 GetTestObjectID() { return ID; }
	UFUNCTION(BlueprintCallable)
	void SetTestObjectID(int32 InNewID) { ID = InNewID; }

private:
	int32 Amount = 0;

	int32 ID = 0;
};
