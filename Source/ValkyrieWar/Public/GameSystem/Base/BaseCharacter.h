// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/ObjectPool/ObjectPoolInterface.h"
#include "Data/Table/GameData/ItemDataRow.h"
#include "BaseCharacter.generated.h"

class UAbilitySystemComponent;

UCLASS()
class VALKYRIEWAR_API ABaseCharacter : public ACharacter, public IAbilitySystemInterface, public IObjectPoolInterface
{
	GENERATED_BODY()

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override {
		return AbilitySystemComponent;
	}
public:
	// Sets default values for this character's properties
	ABaseCharacter();

	virtual void OnGet_Implementation() override;

	virtual void OnRelease_Implementation() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	/// <summary>
	/// 무기 장착 시 공격 관련 세팅(무기에 공격테이블 id)
	/// </summary>
	/// <param name="InDataId"></param>
	virtual void EquipWeapon(int32 InDataId);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent = nullptr;

protected:
	const FItemDataRow* EquippedWeapon = nullptr;
};
