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
					SpringArm->AddWorldOffset(MoveDirection * CameraPanSpeed);
				}
			}
		}
		PrevTouchLocation = CurrentTouchLocation;
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
