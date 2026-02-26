#include "Object/Character/Controller/ValkyrieCharacterController.h"

#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LocalPlayer.h"

#include "GameSystem/GameMode/ValkyrieWarGameMode.h"

#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "Object/Character/Controller/CameraBoundsVolume.h"


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
	}
	else
	{
		CurrentTargetViewOffset = AutoViewOffset;
		//드래그 초기화 시키기
		DragOffset = FVector::ZeroVector;
		//자동모드에선 가상조이스틱 샷따 내리기 
		ActivateTouchInterface(nullptr);
	}
	OnControlModeChanged(CurrentControlMode);

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

void AValkyrieCharacterController::BeginPlay()
{
	Super::BeginPlay();

	// 입력 모드 설정
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);

	// 변수 초기화
	DragOffset = FVector::ZeroVector;
	bIsDragging = false;
	bIsInputActive = false;
	//수동모드일때 카메라
	ManualViewOffset = FVector(-600.0f, 0.0f, 700.0f);
	//자동모드일떄 카메라
	AutoViewOffset = FVector(-1500.0f, 0.0f, 2000.0f);

	// 시작 모드는 수동모드로
	CurrentControlMode = EInputControlMode::Manual;

	// 현재 수동모드를 좌표로 설정하고있음
	CurrentTargetViewOffset = ManualViewOffset;

	if (APawn* ControlledPawn = GetPawn())
    {
        if (UCameraComponent* CamComp = ControlledPawn->GetComponentByClass<UCameraComponent>())
        {
            CamComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
            CamComp->bUsePawnControlRotation = false;
        }
    }

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACameraBoundsVolume::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		BoundsVolume = Cast<ACameraBoundsVolume>(FoundActors[0]);
	}


	SpawnValkyrie();
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
}

void AValkyrieCharacterController::UpdateCameraPosition(float InDeltaTime)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	UCameraComponent* CamComp = ControlledPawn->GetComponentByClass<UCameraComponent>();
	if (!CamComp) return;
	float CurrentLagSpeed;
	float CurrentTime = GetWorld()->GetTimeSeconds();
	bool bTimeExpired = (CurrentTime - LastInteractionTime) > AutoCenterWaitTime;
	bool bShouldRecenter = (bIsInputActive || bTimeExpired) && !bIsDragging;

	if (CurrentControlMode == EInputControlMode::Manual && bShouldRecenter)
	{
		float Speed = bIsInputActive ? MovingCenterInterpSpeed : AutoCenterInterpSpeed;
		DragOffset = FMath::VInterpTo(DragOffset, FVector::ZeroVector, InDeltaTime, Speed);
	}

	FVector CharLoc = ControlledPawn->GetActorLocation();
	FVector CharVelocity = ControlledPawn->GetVelocity();
	CharVelocity.Z = 0.0f;

	FVector TargetLookAhead = CharVelocity * VelocityLeadScale;
	TargetLookAhead = TargetLookAhead.GetClampedToMaxSize(MaxLeadDistance);

	CurrentLookAheadOffset = FMath::VInterpTo(
		CurrentLookAheadOffset,
		TargetLookAhead,
		InDeltaTime,
		LookAheadInterSpeed
	);

	if (CurrentControlMode == EInputControlMode::Manual)
	{
		CurrentLagSpeed = ManualLagSpeed;
	}
	else
	{
		CurrentLagSpeed = AutoLagSpeed;
	}
	FVector FinalTargetLoc = CharLoc + CurrentTargetViewOffset + DragOffset + CurrentLookAheadOffset;
	FVector CurrentCamLoc = CamComp->GetComponentLocation();
	FVector NewCamLoc = FMath::VInterpTo(CurrentCamLoc, FinalTargetLoc, InDeltaTime, CurrentLagSpeed);

	
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
	FRotator LookDownRot = FRotator(-55.0f, 0.0f, 0.0f);
	CamComp->SetWorldRotation(LookDownRot);
	CamComp->SetWorldLocation(NewCamLoc);
}

void AValkyrieCharacterController::OnMove(const FInputActionValue& InValue)
{
	
	if (CurrentControlMode == EInputControlMode::Auto) return;
	
	FVector2D MovementVector = InValue.Get<FVector2D>();

	if (!MovementVector.IsNearlyZero())
	{
		bIsInputActive = true;
		RefreshInteractionTime();
	}

	if (APawn* ControlledPawn = GetPawn())
	{
		// 카메라가 아니라 컨트롤러 회전 기준으로 이동 (일반적인 방식)
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AValkyrieCharacterController::OnMoveCompleted(const FInputActionValue& InValue)
{
	bIsInputActive = false;
}

void AValkyrieCharacterController::OnAttackTap(const FInputActionValue& InValue)
{
	if (AValkyrieCharacter* ControlledChar = Cast<AValkyrieCharacter>(GetPawn()))
	{
		ControlledChar->Attack();
	}
}

void AValkyrieCharacterController::RefreshInteractionTime()
{
	LastInteractionTime = GetWorld()->GetTimeSeconds();
}

void AValkyrieCharacterController::OnInputStarted()
{
	
	if (bIsInputActive) return;

	StopMovement(); // 이동 중이면 멈춤 (필요 없으면 삭제 가능)

	float X, Y;
	bool bFoundInput = false;
	GetInputTouchState(ETouchIndex::Touch1, X, Y, bFoundInput);

	if (!bFoundInput)
	{
		if (GetMousePosition(X, Y)) bFoundInput = true;
	}

	if (bFoundInput)
	{
		if (CurrentControlMode == EInputControlMode::Manual)
		{
			// 조이스틱 영역 체크 (데드존)
			int32 ScreenWidth, ScreenHeight;
			GetViewportSize(ScreenWidth, ScreenHeight);

			float DeadZoneXLimit = ScreenWidth * JoystickDeadZoneWidthRatio;
			float DeadZoneYStart = ScreenHeight * (1.0f - JoystickDeadZoneHeightRatio);

			if (X < DeadZoneXLimit && Y > DeadZoneYStart)
			{
				bIsDragging = false;
				return; // 조이스틱 영역이면 드래그 무시
			}
		}
		PrevTouchLocation = FVector2D(X, Y);
		bIsDragging = true;
	}
}

void AValkyrieCharacterController::OnTouchTriggered()
{
	if (bIsInputActive || !bIsDragging) return;

	float X, Y;
	bool bFoundInput = false;
	GetInputTouchState(ETouchIndex::Touch1, X, Y, bFoundInput);

	if (!bFoundInput)
	{
		if (GetMousePosition(X, Y)) bFoundInput = true;
	}

	if (bFoundInput)
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
			// Y축(좌우), X축(앞뒤) - 카메라 회전에 따라 방향 맞춤
			DragOffset.Y += Delta.X * CurrentPanSpeed;
			DragOffset.X -= Delta.Y * CurrentPanSpeed;
		}
		PrevTouchLocation = CurrentTouchLocation;
	}
}

void AValkyrieCharacterController::OnTouchReleased()
{
	bIsTouch = false;
	bIsDragging = false;
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
