#include "Object/Character/Controller/ValkyrieCharacterController.h"
#include "Object/Character/Controller/CameraBoundsVolume.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "Components/BoxComponent.h" 

AValkyrieCharacterController::AValkyrieCharacterController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;

	// [중요] 팀장님 파일에 Auto가 정의되어 있어야 함
	CurrentControlMode = EInputControlMode::Auto;
}

void AValkyrieCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (APawn* ControlledPawn = GetPawn())
	{
		if (USpringArmComponent* SpringArm = ControlledPawn->GetComponentByClass<USpringArmComponent>())
		{
			TargetCameraLocation = SpringArm->GetRelativeLocation();
		}
	}
}

void AValkyrieCharacterController::PlayerTick(float InDeltaTime)
{
	Super::PlayerTick(InDeltaTime);
	UpdateCameraPosition(InDeltaTime);
}

// [복구] 모드 변경 함수
void AValkyrieCharacterController::SetControlMode(EInputControlMode InNewMode)
{
	if (CurrentControlMode == InNewMode) return;
	CurrentControlMode = InNewMode;
}

void AValkyrieCharacterController::UpdateCameraPosition(float InDeltaTime)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	USpringArmComponent* SpringArm = ControlledPawn->GetComponentByClass<USpringArmComponent>();
	if (!SpringArm) return;

	// [로직 복구] Auto 모드이거나, 입력 없이 시간 지나면 중앙 복귀
	// 드래그 오프셋이 조금이라도 남아있으면 돌아가게 처리
	bool bShouldRecenter = !bIsInputActive && DragOffset.SizeSquared() > 10.0f;

	if (CurrentControlMode == EInputControlMode::Auto || bShouldRecenter)
	{
		float Speed = bIsInputActive ? MovingCenterInterpSpeed : AutoCenterInterpSpeed;
		DragOffset = FMath::VInterpTo(DragOffset, FVector::ZeroVector, InDeltaTime, Speed);
	}

	// 목표 위치 계산
	CurrentCamLoc = SpringArm->GetComponentLocation();
	FinalTargetLoc = ControlledPawn->GetActorLocation() + DragOffset;

	CurrentLagSpeed = bIsInputActive ? MovingLagSpeed : AutoLagSpeed;

	FVector NewCamLoc = FMath::VInterpTo(CurrentCamLoc, FinalTargetLoc, InDeltaTime, CurrentLagSpeed);

	// [형 기능] 바운드 볼륨
	if (BoundsVolume)
	{
		FVector Origin = BoundsVolume->GetActorLocation();
		FVector Extent = BoundsVolume->GetComponentsBoundingBox().GetExtent();

		float MinX = Origin.X - Extent.X;
		float MaxX = Origin.X + Extent.X;
		float MinY = Origin.Y - Extent.Y;
		float MaxY = Origin.Y + Extent.Y;

		NewCamLoc.X = FMath::Clamp(NewCamLoc.X, MinX, MaxX);
		NewCamLoc.Y = FMath::Clamp(NewCamLoc.Y, MinY, MaxY);
	}

	SpringArm->SetWorldLocation(NewCamLoc);
}

void AValkyrieCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AValkyrieCharacterController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AValkyrieCharacterController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AValkyrieCharacterController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AValkyrieCharacterController::OnSetDestinationReleased);

		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AValkyrieCharacterController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AValkyrieCharacterController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AValkyrieCharacterController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AValkyrieCharacterController::OnTouchReleased);
	}
}

void AValkyrieCharacterController::OnMove(const FInputActionValue& Value)
{
	// [중요] Auto 모드일 때는 이동 입력 무시하려면 아래 주석 해제
	if (CurrentControlMode == EInputControlMode::Auto) return;

	FVector2D MovementVector = Value.Get<FVector2D>();

	// 입력이 들어오면 '활동 중' 상태로 변경 (카메라 중앙 복귀 방지)
	if (!MovementVector.IsNearlyZero())
	{
		bIsInputActive = true;
		RefreshInteractionTime();
	}

	// 캐릭터 이동 처리
	if (APawn* ControlledPawn = GetPawn())
	{
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AValkyrieCharacterController::OnMoveCompleted(const FInputActionValue& Value)
{
	bIsInputActive = false;
}

void AValkyrieCharacterController::RefreshInteractionTime()
{
	LastInteractionTime = GetWorld()->GetTimeSeconds();
}

void AValkyrieCharacterController::OnInputStarted()
{
	StopMovement();

	// [복구] 터치 시작하면 Manual 모드로 전환 (내가 제어한다!)
	SetControlMode(EInputControlMode::Manual);

	float X, Y;
	GetInputTouchState(ETouchIndex::Touch1, X, Y, bIsDragging);
	if (bIsDragging)
	{
		PrevTouchLocation = FVector2D(X, Y);
	}
}

void AValkyrieCharacterController::OnTouchTriggered()
{
	// [복구] 계속 터치 중이니까 Manual 모드 유지 + 시간 갱신
	SetControlMode(EInputControlMode::Manual);
	RefreshInteractionTime();

	float X, Y;
	bool bFoundInput = false;

	GetInputTouchState(ETouchIndex::Touch1, X, Y, bFoundInput);

	if (!bFoundInput || (X == 0.0f && Y == 0.0f))
	{
		if (GetMousePosition(X, Y)) bFoundInput = true;
	}

	if (bFoundInput)
	{
		FVector2D CurrentTouchLocation = FVector2D(X, Y);

		if (!bIsDragging)
		{
			PrevTouchLocation = CurrentTouchLocation;
			bIsDragging = true;
			return;
		}

		FVector2D Delta = PrevTouchLocation - CurrentTouchLocation;

		if (Delta.SizeSquared() > 0.1f) // 손떨방
		{
			if (APawn* ControlledPawn = GetPawn())
			{
				if (USpringArmComponent* SpringArm = ControlledPawn->GetComponentByClass<USpringArmComponent>())
				{
					FRotator CameraRot = SpringArm->GetComponentRotation();
					CameraRot.Roll = 0.0f; CameraRot.Pitch = 0.0f;

					FVector CameraForward = FRotationMatrix(CameraRot).GetUnitAxis(EAxis::X);
					FVector CameraRight = FRotationMatrix(CameraRot).GetUnitAxis(EAxis::Y);

					FVector MoveDirection = (CameraForward * -Delta.Y) + (CameraRight * Delta.X);

					// 드래그 적용
					TargetCameraLocation = SpringArm->GetRelativeLocation() + (MoveDirection * CameraPanSpeed);
					DragOffset += (MoveDirection * CameraPanSpeed);
				}
			}
		}
		PrevTouchLocation = CurrentTouchLocation;
	}
}

void AValkyrieCharacterController::OnSetDestinationTriggered()
{
	FollowTime += GetWorld()->GetDeltaSeconds();

	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	else
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

	if (bHitSuccessful)
		CachedDestination = Hit.Location;

	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
}

void AValkyrieCharacterController::OnSetDestinationReleased()
{
	if (FollowTime <= ShortPressThreshold)
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}
	FollowTime = 0.f;

	// [복구] 클릭/터치 끝났으니 일정 시간 뒤 Auto 복귀를 위한 시간 체크는 Tick에서 처리될 것임
	// 필요하면 여기서 Auto로 바로 돌려도 되지만, 보통 딜레이를 줌
}

void AValkyrieCharacterController::OnTouchReleased()
{
	bIsTouch = false;
	bIsDragging = false;
}
