#include "Object/Character/Controller/ValkyrieCharacterController.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h" // 카메라 컴포넌트 헤더 필수!
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"

AValkyrieCharacterController::AValkyrieCharacterController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// 기본값 설정
	CameraPanSpeed = 1.0f;
	AutoCenterWaitTime = 2.0f;
	AutoCenterInterpSpeed = 2.0f;
	MovingCenterInterpSpeed = 5.0f;
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

	// ★ [카메라 기본 위치 강제 설정]
	// 뒤로 600, 위로 900 (쿼터뷰 정석)
	DefaultViewOffset = FVector(-600.0f, 0.0f, 900.0f);

	UE_LOG(LogTemp, Warning, TEXT("카메라 강제 위치 설정 완료! :: %s"), *DefaultViewOffset.ToString());
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
	}
}

void AValkyrieCharacterController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// ★ 매 프레임 카메라 위치 갱신 (이게 없으면 안 움직임!)
	UpdateCameraPosition(DeltaTime);
}

void AValkyrieCharacterController::UpdateCameraPosition(float DeltaTime)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	// 캐릭터에 붙어있는 카메라 찾기 (스프링암 아님!)
	UCameraComponent* CamComp = ControlledPawn->GetComponentByClass<UCameraComponent>();
	if (!CamComp) return;

	// 1. 복귀 로직 (손 떼면 드래그 거리 줄이기)
	float CurrentTime = GetWorld()->GetTimeSeconds();
	bool bTimeExpired = (CurrentTime - LastInteractionTime) > AutoCenterWaitTime;
	bool bShouldRecenter = (bIsInputActive || bTimeExpired) && !bIsDragging;

	if (bShouldRecenter)
	{
		float Speed = bIsInputActive ? MovingCenterInterpSpeed : AutoCenterInterpSpeed;
		DragOffset = FMath::VInterpTo(DragOffset, FVector::ZeroVector, DeltaTime, Speed);
	}

	// 2. 목표 위치 계산
	FVector CharLoc = ControlledPawn->GetActorLocation();

	// 최종 목표 = 캐릭터 + 기본 거리 + 드래그 오프셋
	FVector FinalTargetLoc = CharLoc + DefaultViewOffset + DragOffset;

	// 3. 부드럽게 이동 (Lag 효과)
	FVector CurrentCamLoc = CamComp->GetComponentLocation();
	FVector NewCamLoc = FMath::VInterpTo(CurrentCamLoc, FinalTargetLoc, DeltaTime, 10.0f); // 10.0f는 반응 속도

	// 4. ★ 강제 적용 (월드 좌표)
	CamComp->SetWorldLocation(NewCamLoc);

	// 5. ★ 회전도 강제 적용! (하늘 보는 거 방지)
	// 무조건 아래로 55도 내려다보기 (쿼터뷰 각도)
	FRotator LookDownRot = FRotator(-55.0f, 0.0f, 0.0f);
	CamComp->SetWorldRotation(LookDownRot);
}

void AValkyrieCharacterController::OnMove(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

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
			// ★ 드래그 적용
			// Y축(좌우), X축(앞뒤) - 카메라 회전에 따라 방향 맞춤
			DragOffset.Y += Delta.X * CameraPanSpeed;
			DragOffset.X -= Delta.Y * CameraPanSpeed;
		}
		PrevTouchLocation = CurrentTouchLocation;
	}
}

void AValkyrieCharacterController::OnTouchReleased()
{
	bIsTouch = false;
	bIsDragging = false;
}
