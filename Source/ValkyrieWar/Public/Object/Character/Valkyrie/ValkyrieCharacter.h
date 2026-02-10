// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "Data/Enums.h"
#include "ValkyrieCharacter.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API AValkyrieCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AValkyrieCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	EWeaponAnimType CurrentWeaponType = EWeaponAnimType::None;

	// 무기 바꾸는 함수 (테스트용)
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetWeaponType(EWeaponAnimType InNewType);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float InDeltaTime) override;
};
