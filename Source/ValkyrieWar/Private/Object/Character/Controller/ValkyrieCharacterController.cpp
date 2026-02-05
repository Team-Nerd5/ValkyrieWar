// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Controller/ValkyrieCharacterController.h"
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

AValkyrieCharacterController::AValkyrieCharacterController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
	FollowTime = 0.f;
}
void AValkyrieCharacterController::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);

	SetInputMode(InputMode);
	if (APawn* ControlledPawn = GetPawn())
	{
		if (USpringArmComponent* SpringArm = ControlledPawn->GetComponentByClass<USpringArmComponent>())
		{
			TargetCameraLocation = SpringArm->GetRelativeLocation();
		}
	}
}

void AValkyrieCharacterController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	if (APawn* ControlledPawn = GetPawn())
	{
		if (USpringArmComponent* SpringArm = ControlledPawn->GetComponentByClass<USpringArmComponent>())
		{
			FVector CurrentLoc = SpringArm->GetRelativeLocation();
			FVector SmoothLoc = FMath::VInterpTo(CurrentLoc, TargetCameraLocation, DeltaTime, LagSpeed);
			SpringArm->SetRelativeLocation(SmoothLoc);
		}
	}
}

void AValkyrieCharacterController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 1);
	}

	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AValkyrieCharacterController::OnMove);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AValkyrieCharacterController::OnMove);
		}
		if (CameraDragAction)
		{
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Started, this, &AValkyrieCharacterController::OnInputStarted);
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Triggered, this, &AValkyrieCharacterController::OnTouchTriggered);
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Completed, this, &AValkyrieCharacterController::OnTouchReleased);
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Canceled, this, &AValkyrieCharacterController::OnTouchReleased);
		}
		
	
		UE_LOG(LogTemp, Warning, TEXT("모든 액션 바인딩 완료!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("인핸스드 인풋 컴포넌트를 찾을 수 없음!"));
	}
}

void AValkyrieCharacterController::OnInputStarted()
{

	if (bIsMoving) return;
	StopMovement();

	float X, Y;
	//데드존
	bool bFoundInput = false;
	GetInputTouchState(ETouchIndex::Touch1, X, Y, bFoundInput);
	if (!bFoundInput)
	{
		if (GetMousePosition(X, Y)) bFoundInput = true;
	}
	if (bFoundInput)
	{
		int32 ScreenWidth, ScreenHeight;
		GetViewportSize(ScreenWidth, ScreenHeight);

		float DeadZoneXLimit = ScreenWidth * JoystickDeadZoneWidthRatio;
		float DeadZoneYStart = ScreenHeight * (1.0f - JoystickDeadZoneHeightRatio);

		if (X < DeadZoneXLimit && Y > DeadZoneYStart)
		{
			bIsDragging = false;
			return;
		}
		PrevTouchLocation = FVector2D(X, Y);
		bIsDragging = true;
	}
}

void AValkyrieCharacterController::OnTouchTriggered()
{
	if (bIsMoving || !bIsDragging)
	{
		return;
	}
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
		FVector2D Delta = PrevTouchLocation - CurrentTouchLocation;

		if (Delta.SizeSquared() > 10000.0f)
		{
			PrevTouchLocation = CurrentTouchLocation;
			return;
		}

		if (Delta.SizeSquared() > 1.0f) // 손떨방 수치를 더 낮춰봄
		{
			if (APawn* ControlledPawn = GetPawn())
			{
				if (USpringArmComponent* SpringArm = ControlledPawn->GetComponentByClass<USpringArmComponent>())
				{
					FRotator CameraRot = SpringArm->GetComponentRotation();
					CameraRot.Roll = 0.0f; CameraRot.Pitch = 0.0f;

					FVector CameraForward = FRotationMatrix(CameraRot).GetUnitAxis(EAxis::X);
					FVector CameraRight = FRotationMatrix(CameraRot).GetUnitAxis(EAxis::Y);

					FVector WorldDirection = (CameraForward * -Delta.Y) + (CameraRight * Delta.X);
					FRotator ActorRot = ControlledPawn->GetActorRotation();
					FVector LocalDirection = ActorRot.UnrotateVector(WorldDirection);
					TargetCameraLocation += (LocalDirection * CameraPanSpeed);


				}
			}
		}
		PrevTouchLocation = CurrentTouchLocation;
	}
	
}

void AValkyrieCharacterController::OnTouchReleased()
{
	bIsTouch = false;
	bIsDragging = false;
}

void AValkyrieCharacterController::OnMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	bIsMoving = MovementVector.SizeSquared() > 0.01f;

	if (bIsMoving)
	{
		bIsDragging = false;
	}
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
