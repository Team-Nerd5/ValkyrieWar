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
	virtual void Tick(float InDeltaTime) override;
	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void SetWeaponType(EWeaponAnimType InNewType);

	virtual void EquipWeapon(uint64 InEquipUID) override;

	void SetData(UValkyrieData* InData);
protected:
	virtual void BeginPlay() override;

protected:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	class USpringArmComponent* CameraBoom;

	TObjectPtr<UValkyrieData> Data = nullptr;

	FGameplayAbilitySpecHandle CurrentAttackHandle;

	virtual void PossessedBy(AController* NewController) override;
public: // 일반 공격
	UFUNCTION(BlueprintCallable, Category = "Attack Test")
	void Attack();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat|Combo")
	bool bCanSaveCombo = false; // 콤보 예약을 받을 수 있는 상태인지

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat|Combo")
	bool bIsAttackSaved = false; // 유저가 공격키를 미리 눌러서 예약했는지

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat|Combo")
	int32 ComboIndex = 0; // 현재 콤보 단계

	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void SetComboEnable(bool bEnable); // ANS_ComboSave에서 호출할 함수

	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void ExecuteCombo(FName NextSectionName); // ANS_ComboSkip에서 호출할 함수

	UFUNCTION(BlueprintCallable, Category = "Combat|Combo")
	void ResetCombo(); // 공격 끝났을때 콤보 리셋

};
