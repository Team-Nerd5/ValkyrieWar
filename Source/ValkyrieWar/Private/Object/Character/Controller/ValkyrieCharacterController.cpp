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

	if (APawn* ControlledPawn = GetPawn())
	{
		if (USpringArmComponent* SpringArm = ControlledPawn->GetComponentByClass<USpringArmComponent>())
		{
			TargetCameraLocation = SpringArm->GetRelativeLocation();

			TargetZoomLength = SpringArm->TargetArmLength;// 줌
		}
	}
}

void AValkyrieCharacterController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	GEngine->AddOnScreenDebugMessage(2, 0.1f, FColor::Cyan, FString::Printf(TEXT("Target: %s"), *TargetCameraLocation.ToString()));
	if (APawn* ControlledPawn = GetPawn())
	{
		// 캐릭터의 SpringArm을 찾아서 부드럽게 이동
		if (USpringArmComponent* SpringArm = ControlledPawn->GetComponentByClass<USpringArmComponent>())
		{
			FVector CurrentLoc = SpringArm->GetRelativeLocation();

			// 현재 위치에서 목표 위치로 DeltaTime 동안 LagSpeed 속도로 보간
			FVector SmoothLoc = FMath::VInterpTo(CurrentLoc, TargetCameraLocation, DeltaTime, LagSpeed);

			SpringArm->SetRelativeLocation(SmoothLoc);

			SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, TargetZoomLength, DeltaTime, ZoomSpeed);
		}
	}
}

void AValkyrieCharacterController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Started, this, &AValkyrieCharacterController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Triggered, this, &AValkyrieCharacterController::OnSetDestinationTriggered);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &AValkyrieCharacterController::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &AValkyrieCharacterController::OnSetDestinationReleased);

		// Setup touch input events
		UE_LOG(LogTemp, Warning, TEXT("✅ [Setup] DragAction 바인딩 성공!"));
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Started, this, &AValkyrieCharacterController::OnInputStarted);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Triggered, this, &AValkyrieCharacterController::OnTouchTriggered);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Completed, this, &AValkyrieCharacterController::OnTouchReleased);
		EnhancedInputComponent->BindAction(SetDestinationTouchAction, ETriggerEvent::Canceled, this, &AValkyrieCharacterController::OnTouchReleased);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AValkyrieCharacterController::OnInputStarted()
{
	StopMovement();

	float X, Y;

	GetInputTouchState(ETouchIndex::Touch1, X, Y, bIsDragging);

	if (bIsDragging)
	{
		PrevTouchLocation = FVector2D(X, Y);
	}
}

void AValkyrieCharacterController::OnTouchTriggered()
{

	float X, Y;
	bool bFoundInput = false;

	GetInputTouchState(ETouchIndex::Touch1, X, Y, bFoundInput);

	
	if (!bFoundInput || (X == 0.0f && Y == 0.0f))
	{
		if (GetMousePosition(X, Y))
		{
			bFoundInput = true;
		}
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

					FVector MoveDirection = (CameraForward * -Delta.Y) + (CameraRight * Delta.X); // 여기서 반전
					TargetCameraLocation = SpringArm->GetRelativeLocation() + (MoveDirection * CameraPanSpeed);
				}
			}
		}
		PrevTouchLocation = CurrentTouchLocation;
	}
	float T1X, T1Y, T2X, T2Y;
	bool bTouch1, bTouch2;

	// 1. 첫 번째 터치(마우스 클릭) 상태 가져오기
	GetInputTouchState(ETouchIndex::Touch1, T1X, T1Y, bTouch1);
	if (!bTouch1) GetMousePosition(T1X, T1Y); // 마우스 백업

	// 2. [디버그용] Ctrl 키를 누르면 현재 위치에 Touch1을 고정!
	static FVector2D FixedTouch1Pos;
	static bool bIsTouch1Fixed = false;

	if (IsInputKeyDown(EKeys::LeftControl))
	{
		if (!bIsTouch1Fixed)
		{
			FixedTouch1Pos = FVector2D(T1X, T1Y); // 키 누르는 순간 좌표 박제
			bIsTouch1Fixed = true;
			if (GEngine) GEngine->AddOnScreenDebugMessage(4, 2.f, FColor::Red, TEXT("📌 Touch1 Fixed!"));
		}

		// 고정된 좌표와 현재 마우스 좌표를 두 손가락으로 간주
		T1X = FixedTouch1Pos.X;
		T1Y = FixedTouch1Pos.Y;
		GetMousePosition(T2X, T2Y); // 현재 마우스 위치를 Touch2로
		bTouch1 = true;
		bTouch2 = true;
	}
	else
	{
		bIsTouch1Fixed = false;
		GetInputTouchState(ETouchIndex::Touch2, T2X, T2Y, bTouch2);
	}

	// 3. 줌 로직 (bTouch1 && bTouch2 일 때만 실행)
	if (bTouch1 && bTouch2)
	{
		FVector2D Pos1(T1X, T1Y);
		FVector2D Pos2(T2X, T2Y);
		float CurrentDistance = FVector2D::Distance(Pos1, Pos2);

		if (!bIsPinching)
		{
			PreviousPinchDistance = CurrentDistance;
			bIsPinching = true;
		}
		else
		{
			float DistanceDelta = CurrentDistance - PreviousPinchDistance;
			TargetZoomLength -= (DistanceDelta * PinchSenSitivity);
			TargetZoomLength = FMath::Clamp(TargetZoomLength, MinZoomLength, MaxZoomLength);
			PreviousPinchDistance = CurrentDistance;
		}
	}
	else
	{
		bIsPinching = false;
	}
}

// Triggered every frame when the input is held down
void AValkyrieCharacterController::OnSetDestinationTriggered()
{
	// We flag that the input is being pressed
	FollowTime += GetWorld()->GetDeltaSeconds();

	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;
	bool bHitSuccessful = false;
	if (bIsTouch)
	{
		bHitSuccessful = GetHitResultUnderFinger(ETouchIndex::Touch1, ECollisionChannel::ECC_Visibility, true, Hit);
	}
	else
	{
		bHitSuccessful = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);
	}

	// If we hit a surface, cache the location
	if (bHitSuccessful)
	{
		CachedDestination = Hit.Location;
	}

	// Move towards mouse pointer or touch
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn != nullptr)
	{
		FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
		ControlledPawn->AddMovementInput(WorldDirection, 1.0, false);
	}
	float WheelValue = GetInputAxisValue(TEXT("MouseWheel")); // IMC에 등록된 이름

	if (FMath::Abs(WheelValue) > 0.1f)
	{
		// 휠 굴리는 만큼 목표 줌 거리 조절
		TargetZoomLength -= (WheelValue * 100.f);
		TargetZoomLength = FMath::Clamp(TargetZoomLength, MinZoomLength, MaxZoomLength);

		UE_LOG(LogTemp, Warning, TEXT("🎡 Wheel Zoom! Target: %.1f"), TargetZoomLength);
	}
}

void AValkyrieCharacterController::OnSetDestinationReleased()
{
	// If it was a short press
	if (FollowTime <= ShortPressThreshold)
	{
		// We move there and spawn some particles
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CachedDestination, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	}

	FollowTime = 0.f;
}

// Triggered every frame when the input is held down


void AValkyrieCharacterController::OnTouchReleased()
{
	bIsTouch = false;
	bIsDragging = false;
}
