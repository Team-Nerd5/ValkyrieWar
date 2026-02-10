#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Data/Enums.h"
#include "CameraBoundsVolume.h"
#include "ValkyrieCharacterController.generated.h"


UCLASS()
class VALKYRIEWAR_API AValkyrieCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	AValkyrieCharacterController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> CameraDragAction;

	// 바운드 볼륨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Bounds")
	TObjectPtr<ACameraBoundsVolume> BoundsVolume = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	float AutoCenterWaitTime = 2.0f; // 복귀 대기 시간

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	float AutoCenterInterpSpeed = 2.0f; // 복귀 속도

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	float MovingCenterInterpSpeed = 5.0f; // 이동 중 복귀 속도

	UFUNCTION(BlueprintCallable, Category = "Camera Control")
	void SetControlMode(EInputControlMode NewMode);

	UFUNCTION(BlueprintCallable, Category = "Camera Control")
	void ToggleControlMode();

	// 수동, 자동 모드 변환
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control | Settings")
	FVector ManualViewOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control | Settings")
	FVector AutoViewOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control | Settings")
	float ManualLagSpeed = 5.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control | Settings")
	float AutoLagSpeed = 2.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control | Settings")
	float ManualPanSpeed = 1.0f; // 수동 모드 드래그 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control | Settings")
	float AutoPanSpeed = 1.0f; // 자동모드

	//상태확인,변경
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Control | State")
	EInputControlMode CurrentControlMode = EInputControlMode::Manual; // 현재

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UTouchInterface> MyTouchInterface; // 터치 인터페이스 킬거임? 끌꺼임?

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
	void OnMove(const FInputActionValue& InValue);
	void OnMoveCompleted(const FInputActionValue& InValue);
	void OnInputStarted();
	void OnTouchTriggered();
	void OnTouchReleased();
	// 블루프린트에서 구현 UI변경이니까 에디터가 편해용
	UFUNCTION(BlueprintImplementableEvent, Category = "Camera Control")
	void OnControlModeChanged(EInputControlMode InNewMode);

private:
	
	FVector DragOffset = FVector::ZeroVector;
	//현재 좌표 저장
	FVector CurrentTargetViewOffset = FVector::ZeroVector;
	FVector2D PrevTouchLocation = FVector2D::ZeroVector;


	float LastInteractionTime = 0.0f;
	bool bIsInputActive = false;
	bool bIsDragging = false;
	bool bIsTouch = false;

	
	void RefreshInteractionTime();
	void UpdateCameraPosition(float InDeltaTime);
};
