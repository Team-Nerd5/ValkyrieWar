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
class UBaseGameplayAbility;

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


	//공격효과 적용
	virtual void ApplyAttack(AActor* InTargetActor);
	//스킬효과 적용
	virtual void ApplySkill(int32 InSkillIndex, AActor* InTargetActor);

	//AI나 타겟 계산 후 타겟 세팅
	void UpdateTarget(AActor* InTarget);

	/*공격 애니메이션에서 데미지 시점 Notify 호출부*/
	virtual void OnAttackNotify() {}

	/*스킬 애니메이션에서 데미지 시점 Notify 호출부*/
	virtual void OnSkillNotify() {}

	// 사망 시 StatAttributeSet에서 호출
	virtual void OnDeath() {}
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/*공격 자체를 시작*/
	virtual void ExecuteAttack() {}
	/*스킬 자체를 시작*/
	virtual void ExecuteSkill(int32 InSkillIndex) {}

	void CreateAttackAbility();

	void CreateSkillAbility();

public:		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<class UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	inline UBlendSpace* GetLocomotionBS() const { return LocomotionBS; }

protected:
	UPROPERTY()
	TObjectPtr<UItemData> EquippedWeapon = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat|Data")
	TObjectPtr<UAttackData> AttackData = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<USkillData>> SkillDataList;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> SkeletalWeapon = nullptr;

	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> StaticWeapon = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> CurrentTarget = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	TObjectPtr<class UStatAttributeSet> StatAttributeSet = nullptr;

	UPROPERTY()
	int32 UsingSkillIndex = 0;

	UPROPERTY()
	FGameplayAbilitySpec AttackSpec;
	UPROPERTY()
	TArray<FGameplayAbilitySpec> SkillSpecs;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBlendSpace> LocomotionBS = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> AttackMontage = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<UAnimMontage>> SkillMontageList;
};
