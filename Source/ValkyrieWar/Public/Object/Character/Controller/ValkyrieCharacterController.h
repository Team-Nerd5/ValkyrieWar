#pragma once

#include "CoreMinimal.h"
#include "Templates/SubclassOf.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "Data/Enums.h"
#include "CameraBoundsVolume.h"
#include "ValkyrieCharacterController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
class ACameraBoundsVolume;

// [삭제됨] enum 정의는 팀장님 파일에 있으니까 여기서 지움!

UCLASS()
class VALKYRIEWAR_API AValkyrieCharacterController : public APlayerController
{
	GENERATED_BODY()
public:
	AValkyrieCharacterController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	float ShortPressThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UNiagaraSystem* FXCursor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetDestinationClickAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SetDestinationTouchAction;

	// 카메라 이동속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Control")
	float CameraPanSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DragAction;

	// [형 기능] 바운드 볼륨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Bounds")
	ACameraBoundsVolume* BoundsVolume;

	// [복구] 카메라 복귀 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Control")
	float MovingCenterInterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Control")
	float AutoCenterInterpSpeed = 2.0f;

protected:
	uint32 bMoveToMouseCursor : 1;

	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	virtual void PlayerTick(float InDeltaTime) override;

	void OnInputStarted();
	void OnSetDestinationTriggered();
	void OnSetDestinationReleased();
	void OnTouchTriggered();
	void OnTouchReleased();

	void OnMove(const FInputActionValue& Value);
	void OnMoveCompleted(const FInputActionValue& Value);

	// [복구] 모드 변경 함수 (Enum 타입은 팀장님 거 씀)
	void SetControlMode(EInputControlMode InNewMode);

private:
	FVector CachedDestination;
	bool bIsTouch;
	float FollowTime;
	bool bIsDragging = false;
	FVector2D PrevTouchLocation;
	FVector TargetCameraLocation;

	// 드래그 및 모드 변수
	FVector DragOffset;
	FVector CurrentCamLoc;
	FVector FinalTargetLoc;

	float CurrentLagSpeed;
	float MovingLagSpeed = 10.0f;
	float AutoLagSpeed = 2.0f;

	bool bIsInputActive = false;
	float LastInteractionTime;

	// [복구] 현재 모드 상태
	EInputControlMode CurrentControlMode;

	void UpdateCameraPosition(float InDeltaTime);
	void RefreshInteractionTime();
};
