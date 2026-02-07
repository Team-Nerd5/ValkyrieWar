// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "GameFramework/Character.h"
#include "Interface/ObjectPool/ObjectPoolInterface.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "TestCharacter.generated.h"

UCLASS()
class VALKYRIEWAR_API ATestCharacter : public ABaseCharacter, public IObjectPoolInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	UFUNCTION(BlueprintCallable)
	void Despawn();

	virtual void OnGet_Implementation() override;
	virtual void OnRelease_Implementation() override;

private:
	TWeakObjectPtr<UObjectPoolSubsystem> SubSystem;

};
