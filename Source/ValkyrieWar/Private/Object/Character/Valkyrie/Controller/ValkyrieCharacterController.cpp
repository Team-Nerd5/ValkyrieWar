#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"

#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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

	AutoCenterWaitTime = 3.0f;
	AutoCenterInterpSpeed = 2.0f;
	MovingCenterInterpSpeed = 5.0f;

	DragOffset = FVector::ZeroVector;
	bIsDragging = false;
	bIsInputActive = false;
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

	SpawnValkyrie();

	ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		PawnCamera = ControlledPawn->GetComponentByClass<UCameraComponent>();
		SpringArm = ControlledPawn->GetComponentByClass<USpringArmComponent>();
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
		// 수동모드시에 조이스틱 주기
		ActivateTouchInterface(MyTouchInterface);

		if (BattleUI) BattleUI->SetJoyPadVisibility(true);

		if (PawnCamera && SpringArm)
		{
			SpringArm->SetWorldRotation(CameraRotate);

			PawnCamera->AttachToComponent(SpringArm, FAttachmentTransformRules::KeepWorldTransform, USpringArmComponent::SocketName);
			PawnCamera->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
			bIsReturningToCenter = true;
			RefreshInteractionTime();
		}
	}
	else
	{
		CurrentTargetViewOffset = AutoViewOffset;
		//드래그 초기화 시키기
		DragOffset = FVector::ZeroVector;
		//자동모드에선 가상조이스틱 샷따 내리기 
		ActivateTouchInterface(nullptr);

		if (BattleUI) BattleUI->SetJoyPadVisibility(false);

		if (PawnCamera)
		{
			PawnCamera->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		}
	}

	//OnControlModeChanged(CurrentControlMode);

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
}



void AValkyrieCharacterController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// 이동 바인딩
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AValkyrieCharacterController::OnMove);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AValkyrieCharacterController::OnMoveCompleted);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &AValkyrieCharacterController::OnMoveCompleted);
		}
		// 드래그 바인딩
		if (CameraDragAction)
		{
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Started, this, &AValkyrieCharacterController::OnInputStarted);
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Triggered, this, &AValkyrieCharacterController::OnTouchTriggered);
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Completed, this, &AValkyrieCharacterController::OnTouchReleased);
			EnhancedInputComponent->BindAction(CameraDragAction, ETriggerEvent::Canceled, this, &AValkyrieCharacterController::OnTouchReleased);
		}
		// 공격 바인딩
		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AValkyrieCharacterController::OnAttackTap); // 약공격
		}
	}
}

void AValkyrieCharacterController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	UpdateCameraPosition(DeltaTime);

	//위젯 조이패드 이동
	if (BattleUI)
	{
		Move(BattleUI->GetJoyPadAxis());
	}
}

void AValkyrieCharacterController::UpdateCameraPosition(float InDeltaTime)
{
	if (!PawnCamera || !ControlledPawn) return;

	float CurrentTime = GetWorld()->GetTimeSeconds();

	bool bTimeExpired = (CurrentTime - LastInteractionTime) > AutoCenterWaitTime;
	bool bShouldRecenter = (!bIsDragging && (bIsInputActive || bTimeExpired || bIsReturningToCenter));

	if (bShouldRecenter)
	{
		float Speed = (bIsInputActive || bIsReturningToCenter) ? MovingCenterInterpSpeed : AutoCenterInterpSpeed;
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
	FVector TotalOffset = DragOffset + CurrentLookAheadOffset;

	CurrentLookAheadOffset = FMath::VInterpTo(CurrentLookAheadOffset, TargetLookAhead, InDeltaTime, LookAheadInterSpeed);

	if (CurrentControlMode == EInputControlMode::Manual)
	{
		if (SpringArm && PawnCamera)
		{
			FVector TargetLocalOffset = CameraRotate.UnrotateVector(TotalOffset);
			FVector CurrentLocalOffset = PawnCamera->GetRelativeLocation();
			FVector SmoothedOffset = FMath::VInterpTo(CurrentLocalOffset, TargetLocalOffset, InDeltaTime, ManualLagSpeed);

			PawnCamera->SetRelativeLocation(SmoothedOffset);
		}
		return; 
	}

	
	float CurrentLagSpeed = AutoLagSpeed; 

	FVector FinalTargetLoc = CharLoc + AutoViewOffset + DragOffset + CurrentLookAheadOffset;
	FVector CurrentCamLoc = PawnCamera->GetComponentLocation();
	FVector NewCamLoc = FMath::VInterpTo(CurrentCamLoc, FinalTargetLoc, InDeltaTime, CurrentLagSpeed);

	// 바운드 박스 제한 로직
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

	PawnCamera->SetWorldRotation(CameraRotate);
	PawnCamera->SetWorldLocation(NewCamLoc);
}
#if WITH_EDITOR
void AValkyrieCharacterController::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PawnCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("Valkyrie Camera connection lost"));
		return;
	}

	FVector CharLoc = ControlledPawn->GetActorLocation();
	FVector FinalTargetLoc = CharLoc + CurrentTargetViewOffset + DragOffset + CurrentLookAheadOffset;
	PawnCamera->SetWorldRotation(CameraRotate);
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
	bIsReturningToCenter = true;
	RefreshInteractionTime();
	Move(MovementVector);
}

void AValkyrieCharacterController::OnMoveCompleted(const FInputActionValue& InValue)
{
	bIsInputActive = false;
}

void AValkyrieCharacterController::OnAttackTap(const FInputActionValue& InValue)
{
	if (AValkyrieCharacter* ControlledChar = Cast<AValkyrieCharacter>(GetPawn()))
	{
		ControlledChar->ExecuteAttack();
	}
	DragOffset = FVector::ZeroVector;
	bIsReturningToCenter = true;
	RefreshInteractionTime();
}

void AValkyrieCharacterController::RefreshInteractionTime()
{
	LastInteractionTime = GetWorld()->GetTimeSeconds();
}

void AValkyrieCharacterController::OnInputStarted()
{
	

	float X, Y;
	bool bFoundInput = false;

	int32 ViewportSizeX, ViewportSizeY;
	GetViewportSize(ViewportSizeX, ViewportSizeY);
	float ScreenThreshold = ViewportSizeX * 0.4f;

	for (uint8 i = 0; i < 10; ++i)
	{
		bool bIsPressed;
		GetInputTouchState((ETouchIndex::Type)i, X, Y, bIsPressed);
		if (bIsPressed)
		{
			// UI가 터치를 먹었다면 컨트롤러까지 안 옴! 여기까지 왔다는 건 카메라 회전이란 뜻!
			if (bIsPressed && X > ScreenThreshold)
			{
				bFoundInput = true;
				CurrentDragTouchIndex = (ETouchIndex::Type)i;
				break;
			}
		}
	}
	// 마우스로 테스트용
	if (!bFoundInput && GetMousePosition(X, Y))
	{
		if (IsInputKeyDown(EKeys::LeftMouseButton) && GetMousePosition(X, Y))
		{
			if (X > ScreenThreshold)
			{
				bFoundInput = true;
				CurrentDragTouchIndex = (ETouchIndex::Type)255;
			}
		}
	}

	if (bFoundInput)
	{
		PrevTouchLocation = FVector2D(X, Y);
		bIsDragging = true;

		bIsReturningToCenter = false;
		RefreshInteractionTime();
	}
	

}

void AValkyrieCharacterController::OnTouchTriggered()
{
	if (!bIsDragging)
	{
		OnInputStarted();
		if (!bIsDragging) return;
		return; 
	}

	float X, Y;
	bool bIsPressed = false;

	if (CurrentDragTouchIndex == (ETouchIndex::Type)255)
	{
		bIsPressed = IsInputKeyDown(EKeys::LeftMouseButton);
		GetMousePosition(X, Y);
	}
	else
	{
		GetInputTouchState(CurrentDragTouchIndex, X, Y, bIsPressed);
	}

	if (!bIsPressed)
	{
		bIsDragging = false;
		RefreshInteractionTime();
		return;
	}

	if (bIsPressed)
	{
		RefreshInteractionTime();
		FVector2D CurrentTouchLocation = FVector2D(X, Y);
		FVector2D Delta = PrevTouchLocation - CurrentTouchLocation;

		// 너무 큰 델타(튀는 값) 무시
		if (Delta.SizeSquared() > 10000.0f)
		{
			PrevTouchLocation = CurrentTouchLocation;
			return;
		}

		// 손떨림 방지
		if (Delta.SizeSquared() > 1.0f)
		{
			float CurrentPanSpeed = (CurrentControlMode == EInputControlMode::Manual) ? ManualPanSpeed : AutoPanSpeed;
			// 카메라 드래그 
			FVector CamForward = PawnCamera->GetForwardVector();
			FVector CamRight = PawnCamera->GetRightVector();

			FVector FlatForward = FVector(CamForward.X, CamForward.Y, 0.0f).GetSafeNormal();
			FVector FlatRight = FVector(CamRight.X, CamRight.Y, 0.0f).GetSafeNormal();

			DragOffset += FlatRight * (Delta.X * CurrentPanSpeed);
			DragOffset -= FlatForward * (Delta.Y * CurrentPanSpeed);
		}
		PrevTouchLocation = CurrentTouchLocation;
	}
}

void AValkyrieCharacterController::OnTouchReleased()
{
	bIsTouch = false;
	bIsDragging = false;

	RefreshInteractionTime();
}

void AValkyrieCharacterController::SpawnValkyrie()
{
	if (UGameManager* GameManager = GetGameInstance<UGameManager>())
	{
		UValkyrieData* Selected = GameManager->GetSelectedValkyrie();

		if (Selected)
		{
			AValkyrieWarGameMode* GameMode = GetWorld()->GetAuthGameMode<AValkyrieWarGameMode>();
			if (GameMode)
			{
				if (auto SpawnClass = Selected->GetSpawnClass())
				{
					AValkyrieCharacter* Valkyrie = GameMode->SpawnValkyrie(this, SpawnClass);
					if (Valkyrie)
					{
						Valkyrie->SetData(Selected);
					}
				}				
			}
		}
	}
}

void AValkyrieCharacterController::Move(FVector2D InMoveDir)
{
	if (ControlledPawn && !InMoveDir.IsNearlyZero())
	{
		// 카메라가 아니라 컨트롤러 회전 기준으로 이동 (일반적인 방식)
		//const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, CameraRotate.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, -InMoveDir.Y);
		ControlledPawn->AddMovementInput(RightDirection, InMoveDir.X);

		if (!bIsInputActive)
		{
			bIsDragging = false;
			bIsReturningToCenter = true;
		}
		bIsInputActive = true;
		RefreshInteractionTime();
	}
	else if (InMoveDir.IsNearlyZero() && BattleUI && BattleUI->GetJoyPadAxis().IsNearlyZero())
	{
		bIsInputActive = false;
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
