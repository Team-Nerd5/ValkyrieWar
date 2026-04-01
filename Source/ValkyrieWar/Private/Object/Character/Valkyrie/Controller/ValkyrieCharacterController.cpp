#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraActor.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"

#include "GameSystem/GameMode/ValkyrieWarGameMode.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/State/Game/BattleGameState.h"
#include "GameSystem/State/Player/ValkyriePlayerState.h"

#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "Object/Character/Valkyrie/Controller/CameraBoundsVolume.h"
#include "Object/Cheat/BattleCheatManager.h"
#include "Widget/HUD/BattleWidget.h"

AValkyrieCharacterController::AValkyrieCharacterController()
{
	bAutoManageActiveCameraTarget = false;

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	ManualPanSpeed = 1.0f;
	AutoPanSpeed = 1.5f;

	ManualLagSpeed = 6.0f;
	AutoLagSpeed = 3.5f;

	AutoCenterWaitTime = 2.0f;
	AutoCenterInterpSpeed = 2.0f;
	MovingCenterInterpSpeed = 5.0f;

	DragOffset = FVector::ZeroVector;
	bIsDragging = false;
	bIsInputActive = false;

	CheatClass = UBattleCheatManager::StaticClass();
}

void AValkyrieCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (!FollowCamera.IsValid())
	{
		FollowCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass());
	}

	if (FollowCamera.IsValid())
	{
		SetViewTarget(FollowCamera.Get());
	}

	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Battle.OnBattleStateChanged.AddDynamic(this, &AValkyrieCharacterController::ChageGameState);
	}
	ChageGameState(EBattleState::Init);

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraBoundsVolume::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		BoundsVolume = Cast<ACameraBoundsVolume>(FoundActors[0]);
	}

	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Battle.OnBattleModeChanged.AddDynamic(this, &AValkyrieCharacterController::HandleControlModeChanged);

		if (AValkyriePlayerState* PS = GetValkyriePlayerState())
		{
			HandleControlModeChanged(PS->GetControlMode());
		}
		else
		{
			HandleControlModeChanged(EInputControlMode::Manual);
		}
	}

	ChageGameState(EBattleState::Play);

	if (AValkyrieCharacter* ValkyrieChar = Cast<AValkyrieCharacter>(ControlledPawn))
	{
		ValkyrieChar->InitHpBarWidget();
	}
}

AValkyriePlayerState* AValkyrieCharacterController::GetValkyriePlayerState() const
{
	return GetPlayerState<AValkyriePlayerState>();
}

void AValkyrieCharacterController::HandleControlModeChanged(EInputControlMode NewMode)
{
	CachedControlMode = NewMode;

	if (CachedControlMode == EInputControlMode::Manual)
	{
		CurrentTargetViewOffset = ManualViewOffset;
		ActivateTouchInterface(MyTouchInterface);

		if (BattleUI)
		{
			BattleUI->SetJoyPadVisibility(true);
		}
	}
	else
	{
		CurrentTargetViewOffset = AutoViewOffset;
		DragOffset = FVector::ZeroVector;
		ActivateTouchInterface(nullptr);

		if (BattleUI)
		{
			BattleUI->SetJoyPadVisibility(false);
		}
	}

	StopMovement();
	bIsDragging = false;
	bIsInputActive = false;
}

void AValkyrieCharacterController::SetCameraTargetPawn(APawn* InPawn)
{
	CameraTargetPawn = InPawn;
}

void AValkyrieCharacterController::ToggleControlMode()
{
	if (AValkyriePlayerState* PS = GetValkyriePlayerState())
	{
		PS->ToggleControlMode();
	}
}

void AValkyrieCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AValkyrieCharacterController::OnMove);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AValkyrieCharacterController::OnMoveCompleted);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AValkyrieCharacterController::OnMoveCompleted);
		}

		if (CameraDragAction)
		{
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Started, this, &AValkyrieCharacterController::OnInputStarted);
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Triggered, this, &AValkyrieCharacterController::OnTouchTriggered);
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Completed, this, &AValkyrieCharacterController::OnTouchReleased);
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Canceled, this, &AValkyrieCharacterController::OnTouchReleased);
		}
	}
}
void AValkyrieCharacterController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	UpdateCameraPosition(DeltaTime);

	if (BattleUI)
	{
		Move(BattleUI->GetJoyPadAxis());
	}
}

void AValkyrieCharacterController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	ControlledPawn = aPawn;
	SetCameraTargetPawn(aPawn);

	if (FollowCamera.IsValid())
	{
		SetViewTarget(FollowCamera.Get());
	}
}

void AValkyrieCharacterController::UpdateCameraPosition(float InDeltaTime)
{
	APawn* TargetPawn = CameraTargetPawn.Get();

	if (!FollowCamera.IsValid() || !TargetPawn)
	{
		return;
	}

	const bool bIsJoypadActive = BattleUI && !BattleUI->GetJoyPadAxis().IsNearlyZero();
	const bool bIsMovingInput = bIsInputActive || bIsJoypadActive;

	bool bIsAttackingOrSkill = false;
	if (AValkyrieCharacter* ValkyrieChar = Cast<AValkyrieCharacter>(TargetPawn))
	{
		if (UAnimInstance* AnimInst = ValkyrieChar->GetMesh()->GetAnimInstance())
		{
			bIsAttackingOrSkill = AnimInst->IsAnyMontagePlaying();
		}
	}

	if (bIsMovingInput || bIsAttackingOrSkill)
	{
		bIsReturningToCenter = true;
		RefreshInteractionTime();
	}
	else if (bIsDragging)
	{
		bIsReturningToCenter = false;
	}
	else if ((GetWorld()->GetTimeSeconds() - LastInteractionTime) > AutoCenterWaitTime)
	{
		bIsReturningToCenter = true;
	}

	if (bIsReturningToCenter)
	{
		const float Speed = (bIsMovingInput || bIsAttackingOrSkill) ? MovingCenterInterpSpeed : AutoCenterInterpSpeed;
		DragOffset = FMath::VInterpTo(DragOffset, FVector::ZeroVector, InDeltaTime, Speed);

		if (DragOffset.IsNearlyZero(1.0f))
		{
			DragOffset = FVector::ZeroVector;
			bIsReturningToCenter = false;
		}
	}

	FVector CharVelocity = TargetPawn->GetVelocity();
	CharVelocity.Z = 0.0f;

	FVector TargetLookAhead = CharVelocity * VelocityLeadScale;
	TargetLookAhead = TargetLookAhead.GetClampedToMaxSize(MaxLeadDistance);

	CurrentLookAheadOffset = FMath::VInterpTo(CurrentLookAheadOffset, TargetLookAhead, InDeltaTime, LookAheadInterSpeed);

	const FVector TotalOffset = DragOffset + CurrentLookAheadOffset;
	const FVector TargetCamLoc = TargetPawn->GetActorLocation() + CurrentTargetViewOffset + TotalOffset;

	const FVector CurrentCamLoc = FollowCamera->GetActorLocation();
	const float LagSpeed = (CachedControlMode == EInputControlMode::Manual) ? ManualLagSpeed : AutoLagSpeed;
	FVector NewCamLoc = FMath::VInterpTo(CurrentCamLoc, TargetCamLoc, InDeltaTime, LagSpeed);

	if (BoundsVolume && BoundsVolume->GetBoundsBox())
	{
		const FVector Origin = BoundsVolume->GetActorLocation();
		const FVector Extent = BoundsVolume->GetBoundsBox()->GetScaledBoxExtent();

		NewCamLoc.X = FMath::Clamp(NewCamLoc.X, Origin.X - Extent.X, Origin.X + Extent.X);
		NewCamLoc.Y = FMath::Clamp(NewCamLoc.Y, Origin.Y - Extent.Y, Origin.Y + Extent.Y);
	}

	FollowCamera->SetActorRotation(CameraRotate);
	FollowCamera->SetActorLocation(NewCamLoc);
}

#if WITH_EDITOR
void AValkyrieCharacterController::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (!FollowCamera.IsValid() || !ControlledPawn.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Valkyrie Camera connection lost"));
		return;
	}

	const FVector CharLoc = ControlledPawn->GetActorLocation();
	const FVector FinalTargetLoc = CharLoc + CurrentTargetViewOffset;
	FollowCamera->SetActorRotation(CameraRotate);
	FollowCamera->SetActorLocation(FinalTargetLoc);
}
#endif

void AValkyrieCharacterController::OnMove(const FInputActionValue& InValue)
{
	if (CachedControlMode == EInputControlMode::Auto)
	{
		return;
	}

	const FVector2D MovementVector = InValue.Get<FVector2D>();

	if (!MovementVector.IsNearlyZero())
	{
		bIsInputActive = true;
		RefreshInteractionTime();
	}

	Move(MovementVector);
}

void AValkyrieCharacterController::OnMoveCompleted(const FInputActionValue& InValue)
{
	bIsInputActive = false;
}

void AValkyrieCharacterController::RefreshInteractionTime()
{
	LastInteractionTime = GetWorld()->GetTimeSeconds();
}

void AValkyrieCharacterController::OnInputStarted()
{
	float X = 0.0f;
	float Y = 0.0f;
	bool bFoundInput = false;

	for (uint8 i = 0; i < 10; ++i)
	{
		bool bIsPressed = false;
		GetInputTouchState((ETouchIndex::Type)i, X, Y, bIsPressed);
		if (bIsPressed)
		{
			bFoundInput = true;
			CurrentDragTouchIndex = (ETouchIndex::Type)i;
			break;
		}
	}

	if (!bFoundInput && GetMousePosition(X, Y))
	{
		bFoundInput = true;
		CurrentDragTouchIndex = ETouchIndex::Touch1;
	}

	if (bFoundInput)
	{
		PrevTouchLocation = FVector2D(X, Y);
		bIsDragging = true;
	}
}

void AValkyrieCharacterController::OnTouchTriggered()
{
	if (!bIsDragging || !FollowCamera.IsValid())
	{
		return;
	}

	const bool bIsJoypadActive = BattleUI && !BattleUI->GetJoyPadAxis().IsNearlyZero();
	if (bIsInputActive || bIsJoypadActive)
	{
		float X = 0.0f;
		float Y = 0.0f;
		bool bDummy = false;
		GetInputTouchState(CurrentDragTouchIndex, X, Y, bDummy);
		if (!bDummy)
		{
			GetMousePosition(X, Y);
		}

		PrevTouchLocation = FVector2D(X, Y);
		return;
	}

	float X = 0.0f;
	float Y = 0.0f;
	bool bDummy = false;
	GetInputTouchState(CurrentDragTouchIndex, X, Y, bDummy);
	if (!bDummy)
	{
		GetMousePosition(X, Y);
	}

	RefreshInteractionTime();

	const FVector2D CurrentTouchLocation(X, Y);
	const FVector2D Delta = PrevTouchLocation - CurrentTouchLocation;

	if (Delta.SizeSquared() > 10000.0f)
	{
		PrevTouchLocation = CurrentTouchLocation;
		return;
	}

	if (Delta.SizeSquared() > 1.0f)
	{
		const float CurrentPanSpeed =
			(CachedControlMode == EInputControlMode::Manual)
			? ManualPanSpeed
			: AutoPanSpeed;

		const FVector CamForward = FollowCamera->GetActorForwardVector();
		const FVector CamRight = FollowCamera->GetActorRightVector();
		const FVector FlatForward = FVector(CamForward.X, CamForward.Y, 0.0f).GetSafeNormal();
		const FVector FlatRight = FVector(CamRight.X, CamRight.Y, 0.0f).GetSafeNormal();

		DragOffset += FlatRight * (Delta.X * CurrentPanSpeed);
		DragOffset -= FlatForward * (Delta.Y * CurrentPanSpeed);
	}

	PrevTouchLocation = CurrentTouchLocation;
}

void AValkyrieCharacterController::OnTouchReleased()
{
	bIsTouch = false;
	bIsDragging = false;
	RefreshInteractionTime();
}

void AValkyrieCharacterController::Move(FVector2D InMoveDir)
{
	const FRotator YawRotation(0, CameraRotate.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (ControlledPawn.IsValid())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, -InMoveDir.Y);
		ControlledPawn->AddMovementInput(RightDirection, InMoveDir.X);
	}
}

void AValkyrieCharacterController::ChageGameState(EBattleState InState)
{
	ABattleGameState* State = GetWorld()->GetGameState<ABattleGameState>();
	if (State)
	{
		State->ChangeState(InState);
	}
}
