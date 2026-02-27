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
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetWeaponType(EWeaponAnimType InNewType);

	virtual void ExecuteAttack() override;
	virtual void ExecuteSkill() override;

	/*
	* 캐릭터 Spawn 후 데이터 세팅에 사용
	*/
	void SetData(UValkyrieData* InData);
protected:
	virtual void BeginPlay() override;

	void EquipWeapon(uint64 InValkyrieUID, uint64 InEquipUID);

	void UpdateWeaponMesh();

	
	virtual void OnAttackNotify() override;

	virtual void OnSkillNotify() override;

protected:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* TopDownCameraComponent;

	UPROPERTY(EditAnywhere, Category = "Combat|Combo")
	TObjectPtr<UAnimMontage> ComboMontage;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	UPROPERTY()
	TObjectPtr<UValkyrieData> Data = nullptr;

	EValkyrieModeType ValkyrieMode = EValkyrieModeType::None;

public:
	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void BeginComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void EndComboWindow(FName NextSectionName);

	AActor* FindTarget();
private:
	int32 CurrentComboCount = 0; // 현 콤보수
	bool bCanNextCombo = false; // 콤보 이어지나
	bool bIsComboActive = false;
	bool bIsComboInputOn = false; // 타이머안에 클릭은 했냐
	bool bIsInComboWindow = false;
};
