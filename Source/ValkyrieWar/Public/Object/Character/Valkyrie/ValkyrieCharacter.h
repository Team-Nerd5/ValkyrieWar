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

	//급선회
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement")
	bool bIsPivotLocked = false;

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
	UFUNCTION(BlueprintCallable, Category = "Attack Test")
	void Attack();

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
};
