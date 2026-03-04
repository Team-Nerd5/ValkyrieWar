#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "Data/Enum/CommonEnums.h"
#include "Data/Enum/StateEnums.h"

#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

#include "CameraBoundsVolume.h"
#include "ValkyrieCharacterController.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class VALKYRIEWAR_API AValkyrieCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	AValkyrieCharacterController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "input")
	TObjectPtr<UInputAction> CameraDragAction;

	//일반공격
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	// 바운드 볼륨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Bounds")
	TObjectPtr<ACameraBoundsVolume> BoundsVolume = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control")
	TObjectPtr<USpringArmComponent> SpringArm = nullptr;
	
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
	FVector ManualViewOffset = FVector(-0, 700.0f, 1000.0f);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Control | Settings")
	FVector AutoViewOffset = FVector(-0, 700.0f, 1000.0f);
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

	//카메라 리드
	UPROPERTY(EditAnywhere, Category = "Camera|Lead")
	float LookAheaDistance = 700.0f;
	UPROPERTY(EditAnywhere, Category = "Camera|Lead")
	float VelocityLeadScale = 0.48f;
	UPROPERTY(EditAnywhere, Category = "Camera|Lead")
	float LookAheadInterSpeed = 3.0f;
	UPROPERTY(EditAnywhere, Category = "Camera|Lead")
	float MaxLeadDistance = 1200.0f;
	FVector CurrentLookAheadOffset = FVector::ZeroVector;
	UPROPERTY(EditAnywhere)
	FRotator CameraRotate = FRotator(-55, -90, 0);

	FORCEINLINE void SetBattleUI(class UBattleWidget* InWidget) { BattleUI = InWidget; }
protected:
	UPROPERTY()
	TObjectPtr<class UBattleWidget> BattleUI = nullptr;

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PlayerTick(float DeltaTime) override;

	// 입력 함수들
	void OnMove(const FInputActionValue& InValue);
	void OnMoveCompleted(const FInputActionValue& InValue);
	void OnAttackTap(const FInputActionValue& InValue);
	//void OnAttackHold(const FInputActionValue& InValue);
	void OnInputStarted();
	void OnTouchTriggered();
	void OnTouchReleased();
	//다중 터치꼬임 방지용
	ETouchIndex::Type CurrentDragTouchIndex = ETouchIndex::Touch1;
	//// 블루프린트에서 구현 UI변경이니까 에디터가 편해용
	//UFUNCTION(BlueprintImplementableEvent, Category = "Camera Control")
	//void OnControlModeChanged(EInputControlMode InNewMode);

	//우선은 BeginPlay에서 생성되도록
	void SpawnValkyrie();
	void Move(FVector2D InMoveDir);

	UFUNCTION()
	void ChageGameState(EBattleState InState);
private:
	
	FVector DragOffset = FVector::ZeroVector;
	//현재 좌표 저장
	FVector CurrentTargetViewOffset = FVector::ZeroVector;
	FVector2D PrevTouchLocation = FVector2D::ZeroVector;


	float LastInteractionTime = 0.0f;
	bool bIsInputActive = false;
	bool bIsDragging = false;
	bool bIsTouch = false;

	bool bIsReturningToCenter = false;

	
	void RefreshInteractionTime();
	void UpdateCameraPosition(float InDeltaTime);

	UPROPERTY()
	TObjectPtr<APawn> ControlledPawn = nullptr;
	UPROPERTY()
	TObjectPtr<UCameraComponent> PawnCamera = nullptr;

	public:
#if WITH_EDITOR
		/*에디터에서 값 변경 시 반영용*/
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
