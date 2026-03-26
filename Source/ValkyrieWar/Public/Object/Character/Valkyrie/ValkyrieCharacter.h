// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "Data/Enum/CharacterEnums.h"
#include "Data/Game/ValkyrieData.h"
#include "Engine/DataTable.h"
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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombo();
	

	virtual void OnAttackNotify() override;
	virtual void OnSkillNotify() override;
 
	virtual ETeamType GetTeamType() const override;
	virtual void OnDeath() override;

	/*
	virtual void StartAttackSequence() ; // 공격모션실행

	virtual void FaceTarget(AActor* Target) ;

	AActor* FindTarget(float SearchRadius);
	
	* 캐릭터 Spawn 후 데이터 세팅에 사용
	
	*/
	
	void SetData(UValkyrieData* InData);

public:

#if WITH_EDITOR
	void ChangeWeapon(UItemData* InEquip);
#endif

	void InitHpBarWidget();

protected:
	virtual void BeginPlay() override;

	void EquipWeapon(uint64 InValkyrieUID, uint64 InEquipUID);

	void UpdateWeaponMesh();

	void BindAttributeDelegates();
	void UnbindAttributeDelegates();

	void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	void BroadcastHpChanged();

protected: // 스킬 관련
	UFUNCTION()
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	virtual void ExecuteAttack() override;
	UFUNCTION()
	virtual void ExecuteSkill(int32 InSkillIndex) override;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	UPROPERTY()
	TObjectPtr<UValkyrieData> Data = nullptr;

	EValkyrieModeType ValkyrieMode = EValkyrieModeType::None;
};
