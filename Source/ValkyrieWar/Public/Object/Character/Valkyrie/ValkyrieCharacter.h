// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "Data/Enum/CharacterEnums.h"
#include "Data/Game/ValkyrieData.h"
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

	UPROPERTY(EditAnywhere, Category = "Combat") // 에디터에서 추가함
	TMap<EWeaponAnimType, TObjectPtr<UAnimMontage>> WeaponMontageMap;


public:

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetWeaponType(EWeaponAnimType InNewType);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombo();
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void ExecuteAttack() override;

	virtual void ExecuteSkill() override;

	virtual void OnAttackNotify() override;

	virtual void OnSkillNotify() override;

	/*
	virtual void StartAttackSequence() ; // 공격모션실행

	virtual void FaceTarget(AActor* Target) ;

	AActor* FindTarget(float SearchRadius);
	
	* 캐릭터 Spawn 후 데이터 세팅에 사용
	
	*/
	
	void SetData(UValkyrieData* InData);

	
protected:
	virtual void BeginPlay() override;

	void EquipWeapon(uint64 InValkyrieUID, uint64 InEquipUID);

	void UpdateWeaponMesh();

	UPROPERTY()
	TObjectPtr<class AValkyrieWeapon> CurrentWeaponActor;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AArrowProjectile> ArrowClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class AValkyrieWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, BlueprintReadWrite, Category = "Combat")
	float DetectionRange = 1200.0f; // 색적 범위

protected:
	UPROPERTY(EditAnywhere, Category = "Combat|Combo")
	TObjectPtr<UAnimMontage> ComboMontage;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UPROPERTY()
	TObjectPtr<UValkyrieData> Data = nullptr;

	EValkyrieModeType ValkyrieMode = EValkyrieModeType::None;

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void BeginComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void EndComboWindow(FName NextSectionName);

	
private:
	int32 CurrentComboCount = 0; // 현 콤보수
	bool bCanNextCombo = false; // 콤보 이어지나
	bool bIsComboActive = false;
	bool bIsComboInputOn = false; // 타이머안에 클릭은 했냐
	bool bIsInComboWindow = false;
	bool bIsAutoChasing; // 색적중임?
};
