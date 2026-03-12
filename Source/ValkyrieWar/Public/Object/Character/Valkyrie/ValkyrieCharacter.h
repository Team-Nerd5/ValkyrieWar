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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	EWeaponAnimType CurrentWeaponType = EWeaponAnimType::None;
public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetWeaponType(EWeaponAnimType InNewType);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombo();
	

	virtual void OnAttackNotify() override;
	UFUNCTION(BlueprintCallable, Category = "Combat|Skill")
	virtual void OnSkillNotify() override;
	 

	/*
	virtual void StartAttackSequence() ; // 공격모션실행

	virtual void FaceTarget(AActor* Target) ;

	AActor* FindTarget(float SearchRadius);
	
	* 캐릭터 Spawn 후 데이터 세팅에 사용
	
	*/
	
	void SetData(UValkyrieData* InData);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Equip")
	FDataTableRowHandle SelectedWeaponRow;

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Weapon|Equip")
	void EquipSelectedWeapon();

	void ChangeCombatWeapon(uint64 InEquipUID);

protected:
	virtual void BeginPlay() override;

	void EquipWeapon(uint64 InValkyrieUID, uint64 InEquipUID);

	void UpdateWeaponMesh();

	
protected: // 스킬 관련
	UFUNCTION()
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ExecuteAttack() override;
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ExecuteSkill(int32 InSkillIndex) override;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

protected:
	UPROPERTY(EditAnywhere, Category = "Combat|Combo")
	TObjectPtr<UAnimMontage> ComboMontage;

	UPROPERTY()
	TObjectPtr<class AValkyrieWeapon> CurrentWeaponActor;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AArrowProjectile> ArrowClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AValkyrieWeapon> WeaponClass;

	UPROPERTY()
	TObjectPtr<UValkyrieData> Data = nullptr;

	EValkyrieModeType ValkyrieMode = EValkyrieModeType::None;

public:// 나중에 삭제할것들
	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void BeginComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void EndComboWindow(FName NextSectionName);

	
private: // 나중에 삭제할것들
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	int32 CurrentComboCount; // 현 콤보수
	bool bCanNextCombo = false; // 콤보 이어지나
	bool bIsComboActive = false;
	bool bIsComboInputOn = false; // 타이머안에 클릭은 했냐
	bool bIsInComboWindow = false;
};
