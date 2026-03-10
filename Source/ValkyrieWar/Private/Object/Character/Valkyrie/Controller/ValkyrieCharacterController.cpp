#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"

#include "GameFramework/Pawn.h"
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

#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "Object/Character/Valkyrie/Controller/CameraBoundsVolume.h"

#include "Widget/HUD/BattleWidget.h"


AValkyrieCharacterController::AValkyrieCharacterController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// 기본값 설정
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

	UE_LOG(LogTemp, Warning, TEXT("유닛 생성됨"));
}

void AValkyrieCharacterController::BeginPlay()
{
	Super::BeginPlay();

	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Battle.OnBattleStateChanged.AddDynamic(this, &AValkyrieCharacterController::ChageGameState);
	}
	ChageGameState(EBattleState::Init);

	// 입력 모드 설정
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
	CurrentControlMode = EInputControlMode::Auto;
	SetControlMode(EInputControlMode::Manual);

	ChageGameState(EBattleState::Play);
}

void AValkyrieCharacterController::SetControlMode(EInputControlMode InNewMode)
{
	if (CurrentControlMode == InNewMode) return;

	CurrentControlMode = InNewMode;

	if (CurrentControlMode == EInputControlMode::Manual)
	{
		CurrentTargetViewOffset = ManualViewOffset;
		ActivateTouchInterface(MyTouchInterface);

		if (BattleUI) BattleUI->SetJoyPadVisibility(true);
	}
	else
	{
		CurrentTargetViewOffset = AutoViewOffset;
		DragOffset = FVector::ZeroVector;
		ActivateTouchInterface(nullptr);

		if (BattleUI) BattleUI->SetJoyPadVisibility(false);
	}

	StopMovement();
	bIsDragging = false;
	bIsInputActive = false;
}

void AValkyrieCharacterController::ToggleControlMode()
{
	if (CurrentControlMode == EInputControlMode::Manual)
	{
		SetControlMode(EInputControlMode::Auto);
	}
	else
	{
		SetControlMode(EInputControlMode::Manual);
	}

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnBattleModeChanged.Broadcast(CurrentControlMode);
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

		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AValkyrieCharacterController::OnAttackTap);
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

	FollowCamera = GetWorld()->SpawnActor<ACameraActor>(ACameraActor::StaticClass());

	SetViewTargetWithBlend(FollowCamera.Get());
}

void AValkyrieCharacterController::UpdateCameraPosition(float InDeltaTime)
{
	if (!FollowCamera.IsValid() || !ControlledPawn) return;

	bool bIsJoypadActive = BattleUI && !BattleUI->GetJoyPadAxis().IsNearlyZero();
	bool bIsMovingInput = bIsInputActive || bIsJoypadActive;

	bool bIsAttackingOrSkill = false;
	if (AValkyrieCharacter* ValkyrieChar = Cast<AValkyrieCharacter>(ControlledPawn))
	{
		if (UAnimInstance* AnimInst = ValkyrieChar->GetMesh()->GetAnimInstance())
		{
			bIsAttackingOrSkill = AnimInst->IsAnyMontagePlaying();
		}
	}

	// 카메라 복귀 스위치 판별
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
		float Speed = (bIsMovingInput || bIsAttackingOrSkill) ? MovingCenterInterpSpeed : AutoCenterInterpSpeed;
		DragOffset = FMath::VInterpTo(DragOffset, FVector::ZeroVector, InDeltaTime, Speed);

		if (DragOffset.IsNearlyZero(1.0f))
		{
			DragOffset = FVector::ZeroVector;
			bIsReturningToCenter = false;
		}
	}

	FVector CharLoc = ControlledPawn->GetActorLocation();
	FVector CharVelocity = ControlledPawn->GetVelocity();
	CharVelocity.Z = 0.0f;

	FVector TargetLookAhead = CharVelocity * VelocityLeadScale;
	TargetLookAhead = TargetLookAhead.GetClampedToMaxSize(MaxLeadDistance);

	CurrentLookAheadOffset = FMath::VInterpTo(CurrentLookAheadOffset, TargetLookAhead, InDeltaTime, LookAheadInterSpeed);


	FVector TotalOffset = DragOffset + CurrentLookAheadOffset;

	FVector CameraForward = CameraRotate.Vector();
	FVector TargetCamLoc = ControlledPawn->GetActorLocation() + AutoViewOffset + TotalOffset;

	if (CurrentControlMode == EInputControlMode::Manual)
	{
		// 절대 월드 좌표로 적용
		FollowCamera->SetActorRotation(CameraRotate);
		FollowCamera->SetActorLocation(TargetCamLoc);

		return;
	}

	float CurrentLagSpeed = AutoLagSpeed;

	// 3. 현재 카메라 위치에서 목표 위치로 부드럽게 보간 (Lag 효과)
	FVector CurrentCamLoc = FollowCamera->GetActorLocation();
	FVector NewCamLoc = FMath::VInterpTo(CurrentCamLoc, TargetCamLoc, InDeltaTime, CurrentLagSpeed);

	// 4. 바운드 박스 영역 제한
	if (BoundsVolume && BoundsVolume->GetBoundsBox())
	{
		FVector Origin = BoundsVolume->GetActorLocation();
		FVector Extent = BoundsVolume->GetBoundsBox()->GetScaledBoxExtent();

		float MinX = Origin.X - Extent.X;
		float MaxX = Origin.X + Extent.X;
		float MinY = Origin.Y - Extent.Y;
		float MaxY = Origin.Y + Extent.Y;

		NewCamLoc.X = FMath::Clamp(NewCamLoc.X, MinX, MaxX);
		NewCamLoc.Y = FMath::Clamp(NewCamLoc.Y, MinY, MaxY);
	}

	// 5. 최종 월드 위치 및 회전 적용
	FollowCamera->SetActorRotation(CameraRotate);
	FollowCamera->SetActorLocation(NewCamLoc);
}

#if WITH_EDITOR
void AValkyrieCharacterController::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!FollowCamera.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("Valkyrie Camera connection lost"));
		return;
	}

	FVector CharLoc = ControlledPawn->GetActorLocation();
	FVector FinalTargetLoc = CharLoc;
	FollowCamera->SetActorRotation(CameraRotate);
}
#endif

void AValkyrieCharacterController::OnMove(const FInputActionValue& InValue)
{
	if (CurrentControlMode == EInputControlMode::Auto) return;

	FVector2D MovementVector = InValue.Get<FVector2D>();

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

void AValkyrieCharacterController::OnAttackTap(const FInputActionValue& InValue)
{
	RequestAttack();
}

void AValkyrieCharacterController::RequestAttack()
{
	if (AValkyrieCharacter* ControlledChar = Cast<AValkyrieCharacter>(GetPawn()))
	{
		ControlledChar->ExecuteAttack();
	}
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
		bool bIsPressed;
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
	if (!bIsDragging) return;

	bool bIsJoypadActive = BattleUI && !BattleUI->GetJoyPadAxis().IsNearlyZero();
	if (bIsInputActive || bIsJoypadActive)
	{
		float X, Y; bool bDummy;
		GetInputTouchState(CurrentDragTouchIndex, X, Y, bDummy);
		if (!bDummy && GetMousePosition(X, Y)) {}

		PrevTouchLocation = FVector2D(X, Y);
		return;
	}

	float X, Y; bool bDummy;
	GetInputTouchState(CurrentDragTouchIndex, X, Y, bDummy);
	if (!bDummy && GetMousePosition(X, Y)) {}

	RefreshInteractionTime();
	FVector2D CurrentTouchLocation = FVector2D(X, Y);
	FVector2D Delta = PrevTouchLocation - CurrentTouchLocation;

	if (Delta.SizeSquared() > 10000.0f) { PrevTouchLocation = CurrentTouchLocation; return; }

	if (Delta.SizeSquared() > 1.0f)
	{
		float CurrentPanSpeed = (CurrentControlMode == EInputControlMode::Manual) ? ManualPanSpeed : AutoPanSpeed;
		FVector CamForward = FollowCamera->GetActorForwardVector();
		FVector CamRight = FollowCamera->GetActorRightVector();
		FVector FlatForward = FVector(CamForward.X, CamForward.Y, 0.0f).GetSafeNormal();
		FVector FlatRight = FVector(CamRight.X, CamRight.Y, 0.0f).GetSafeNormal();

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

	FVector WorldInputDirection = (ForwardDirection * -InMoveDir.Y) + (RightDirection * InMoveDir.X);
	WorldInputDirection.Z = 0.0f;

	if (ControlledPawn)
	{
		if (AValkyrieCharacter* ValkyrieChar = Cast<AValkyrieCharacter>(ControlledPawn))
		{
			if (UAnimInstance* AnimInst = ValkyrieChar->GetMesh()->GetAnimInstance())
			{
				if (AnimInst->IsAnyMontagePlaying())
				{
					if (!WorldInputDirection.IsNearlyZero())
					{
						FRotator TargetRotation = WorldInputDirection.Rotation();
						ValkyrieChar->SetActorRotation(TargetRotation);
					}
					return;
				}
			}
		}
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
