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
		ValkyrieChar->InitValkyrieWidget();
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

bool AValkyrieCharacterController::CanMoveToDirection(const FVector& WorldDirection) const
{
	if (!ControlledPawn.IsValid() || !BoundsVolume || !BoundsVolume->GetBoundsBox())
	{
		return true;
	}

	const FVector Direction2D = FVector(WorldDirection.X, WorldDirection.Y, 0.0f).GetSafeNormal();
	if (Direction2D.IsNearlyZero())
	{
		return true;
	}

	// "조금 앞" 위치를 가정해서 바운드 체크
	// 너무 크면 부자연스럽고, 너무 작으면 경계에서 미세하게 떨릴 수 있으니 적당한 값 사용
	const float PredictDistance = 30.0f;

	const FVector PredictedPawnLoc =
		ControlledPawn->GetActorLocation() + (Direction2D * PredictDistance);

	// 카메라가 실제로 따라갈 기준 위치
	const FVector PredictedBaseCamLoc =
		PredictedPawnLoc + CurrentTargetViewOffset + CurrentLookAheadOffset + DragOffset;

	const FVector Origin = BoundsVolume->GetActorLocation();
	const FVector Extent = BoundsVolume->GetBoundsBox()->GetScaledBoxExtent();

	const bool bInsideX =
		PredictedBaseCamLoc.X >= (Origin.X - Extent.X) &&
		PredictedBaseCamLoc.X <= (Origin.X + Extent.X);

	const bool bInsideY =
		PredictedBaseCamLoc.Y >= (Origin.Y - Extent.Y) &&
		PredictedBaseCamLoc.Y <= (Origin.Y + Extent.Y);

	return bInsideX && bInsideY;
}

void AValkyrieCharacterController::StopBlockedMovement(const FVector& BlockedDirection) const
{
	AValkyrieCharacter* Valkyrie = Cast<AValkyrieCharacter>(ControlledPawn.Get());
	if (!Valkyrie)
	{
		return;
	}

	UCharacterMovementComponent* MoveComp = Valkyrie->GetCharacterMovement();
	if (!MoveComp)
	{
		return;
	}

	const FVector Direction2D = FVector(BlockedDirection.X, BlockedDirection.Y, 0.0f).GetSafeNormal();
	if (Direction2D.IsNearlyZero())
	{
		return;
	}

	FVector Velocity = MoveComp->Velocity;
	const float Dot = FVector::DotProduct(Velocity, Direction2D);

	// 막힌 방향으로 가고 있는 속도만 제거
	if (Dot > 0.0f)
	{
		Velocity -= Direction2D * Dot;
		MoveComp->Velocity = Velocity;
	}
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

	const FVector BaseCamLoc = TargetPawn->GetActorLocation() + CurrentTargetViewOffset + CurrentLookAheadOffset;

	FVector DesiredCamLoc = BaseCamLoc + DragOffset;

	if (BoundsVolume && BoundsVolume->GetBoundsBox())
	{
		const FVector Origin = BoundsVolume->GetActorLocation();
		const FVector Extent = BoundsVolume->GetBoundsBox()->GetScaledBoxExtent();

		const float PaddingX = 0.0f;
		const float PaddingY = 0.0f;

		DesiredCamLoc.X = FMath::Clamp(DesiredCamLoc.X, Origin.X - Extent.X + PaddingX, Origin.X + Extent.X - PaddingX);
		DesiredCamLoc.Y = FMath::Clamp(DesiredCamLoc.Y, Origin.Y - Extent.Y + PaddingY, Origin.Y + Extent.Y - PaddingY);

		// Clamp된 결과를 기준으로 DragOffset 자체를 다시 맞춤
		DragOffset = DesiredCamLoc - BaseCamLoc;
	}

	const FVector CurrentCamLoc = FollowCamera->GetActorLocation();
	const float LagSpeed = (CachedControlMode == EInputControlMode::Manual) ? ManualLagSpeed : AutoLagSpeed;

	const FVector NewCamLoc = FMath::VInterpTo(CurrentCamLoc, DesiredCamLoc, InDeltaTime, LagSpeed);

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
	if (!ControlledPawn.IsValid())
	{
		return;
	}

	const FRotator YawRotation(0, CameraRotate.Yaw, 0);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const float ForwardValue = -InMoveDir.Y;
	const float RightValue = InMoveDir.X;

	// Forward 축 처리
	if (!FMath::IsNearlyZero(ForwardValue))
	{
		const FVector MoveDir = ForwardDirection * FMath::Sign(ForwardValue);

		if (CanMoveToDirection(MoveDir))
		{
			ControlledPawn->AddMovementInput(ForwardDirection, ForwardValue);
		}
		else
		{
			StopBlockedMovement(MoveDir);
		}
	}

	// Right 축 처리
	if (!FMath::IsNearlyZero(RightValue))
	{
		const FVector MoveDir = RightDirection * FMath::Sign(RightValue);

		if (CanMoveToDirection(MoveDir))
		{
			ControlledPawn->AddMovementInput(RightDirection, RightValue);
		}
		else
		{
			StopBlockedMovement(MoveDir);
		}
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
