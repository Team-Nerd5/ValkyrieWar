// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "GameSystem/Base/BaseActor.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "TestActorSpawner.generated.h"

UCLASS()
class VALKYRIEWAR_API ATestActorSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestActorSpawner();

protected:
	virtual void BeginPlay() override;
	
protected:
	UFUNCTION(BlueprintCallable)
	void SpawnActor();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Test")
	TSubclassOf<AActor> TestActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Test")
	TSubclassOf<ABaseCharacter> TestCharacter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test")
	float Rate = 0.0f;

private:
	FTimerHandle TimerHandle;
	FTimerHandle TimerHandle2;

	TWeakObjectPtr<UObjectPoolSubsystem> SubSystem;

	// 스폰된 인스턴스를 보관
	TWeakObjectPtr<AActor> SpawnedActor = nullptr;
};
