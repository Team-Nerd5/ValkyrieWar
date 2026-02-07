// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameBaseLibrary.generated.h"

class UWorldEventSystem;
class UObjectPoolSubsystem;
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
	UFUNCTION(BlueprintCallable, Category = "MyGame|Utils", meta = (WorldContext = "WorldContextObject"))
	static UObjectPoolSubsystem* GetObjectPoolSystem(const UObject* WorldContextObject);


	template<typename T>
	static bool CheckClassImplements(TSubclassOf<T> InClass, UClass* InInterface);
};

template<typename T>
inline bool UGameBaseLibrary::CheckClassImplements(TSubclassOf<T> InClass, UClass* InInterface)
{
	if (!InClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[Subsystem(Spawn)] 스폰할 액터가 없습니다"));
		return false;
	}

	if (!InClass->ImplementsInterface(InInterface))
	{
		UE_LOG(LogTemp, Error, TEXT("[Subsystem(Spawn)] 스폰할 액터의 인터페이스가 없습니다"));
		return false;
	}

	return true;
}
