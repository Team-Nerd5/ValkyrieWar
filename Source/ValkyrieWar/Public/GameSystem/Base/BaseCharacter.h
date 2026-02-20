// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Interface/ObjectPool/ObjectPoolInterface.h"
#include "Data/Game/ItemData.h"
#include "Data/Game/AttackData.h"
#include "Data/Game/SkillData.h"
#include "GameplayTagContainer.h"
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
	/// TODO : 인벤토리 연결 후 DataId -> UID로 변경필요
	/// </summary>
	/// <param name="InDataId"></param>
	virtual void EquipWeapon(uint64 InEquipUID) {}

	//공격효과 적용
	virtual void ApplyAttack(AActor* InTargetActor);
	//스킬효과 적용
	virtual void ApplySkill(int32 InSkillIndex, AActor* InTargetActor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute")
	TObjectPtr<class UStatAttributeSet> StatAttribute = nullptr;

protected:
	UPROPERTY()
	TObjectPtr<UItemData> EquippedWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Data")
	TObjectPtr<UAttackData> AttackData = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<USkillData>> SkillDataList;
};
