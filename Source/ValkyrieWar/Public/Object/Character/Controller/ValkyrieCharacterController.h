// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "ValkyrieCharacterController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API AValkyrieCharacterController : public APlayerController
{
	GENERATED_BODY()
public:
	AValkyrieCharacterController();

	/** Time Threshold to know if it was a short press */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	/** FX Class that we will spawn when clicking */
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	
	//카메라 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Control")
	float CameraPanSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DragAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Camera")
	UInputAction* CameraDragAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Action")
	class UInputAction* MoveAction;

	// 조이스틱 부분 드래그 금지
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|DeadZone")
	float JoystickDeadZoneWidthRatio = 0.35f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|DeadZone")
	float JoystickDeadZoneHeightRatio = 0.4f;

protected:
	/** True if the controlled character should navigate to the mouse cursor. */
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;

	// To add mapping context
	virtual void BeginPlay();

	virtual void PlayerTick(float DeltaTime) override;

	/** Input handlers for SetDestination action. */
	void OnInputStarted();
	void OnTouchTriggered();
	void OnTouchReleased();
	void OnMove(const FInputActionValue& Value);
	bool bIsMoving = false;
private:
	FVector CachedDestination;

	bool bIsTouch; // Is it a touch device
	float FollowTime; // For how long it has been pressed
	bool bIsDragging = false;
	FVector2D PrevTouchLocation;
	FVector TargetCameraLocation;
	float LagSpeed = 10.0f;


};
