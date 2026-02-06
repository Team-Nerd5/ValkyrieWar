#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "ValkyrieCharacterController.generated.h"

UCLASS()
class VALKYRIEWAR_API AValkyrieCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	AValkyrieCharacterController();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* CameraDragAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	float CameraPanSpeed = 1.0f; // 드래그 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	float AutoCenterWaitTime = 2.0f; // 복귀 대기 시간

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	float AutoCenterInterpSpeed = 2.0f; // 복귀 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	float MovingCenterInterpSpeed = 5.0f; // 이동 중 복귀 속도

	// 조이스틱 데드존
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|DeadZone")
	float JoystickDeadZoneWidthRatio = 0.35f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input|DeadZone")
	float JoystickDeadZoneHeightRatio = 0.4f;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

	// 입력 함수들
	void OnMove(const FInputActionValue& Value);
	void OnMoveCompleted(const FInputActionValue& Value);
	void OnInputStarted();
	void OnTouchTriggered();
	void OnTouchReleased();

private:
	
	FVector DragOffset;

	
	FVector DefaultViewOffset;


	float LastInteractionTime = 0.0f;
	bool bIsInputActive = false;
	bool bIsDragging = false;
	bool bIsTouch = false;
	FVector2D PrevTouchLocation;

	
	void RefreshInteractionTime();

	void UpdateCameraPosition(float DeltaTime);
};
