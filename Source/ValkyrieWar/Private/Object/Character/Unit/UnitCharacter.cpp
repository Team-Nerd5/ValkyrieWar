// Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Character/Unit/UnitCharacter.h"
#include "Object/Unit/Component/UnitBrainComponent.h"
#include "Object/Unit/AI/Controller/UnitAIController.h"

#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "GameSystem/Base/BaseUnitSpawner.h"

#include "Data/Struct/UnitEngagementSlotData.h"
#include "Data/Attribute/StatAttributeSet.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Kismet/GameplayStatics.h"
#include "AbilitySystemBlueprintLibrary.h"

AUnitCharacter::AUnitCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	Brain = CreateDefaultSubobject<UUnitBrainComponent>(TEXT("Brain"));

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	GetMesh()->bEnableUpdateRateOptimizations = true;
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
}

void AUnitCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentHP = MaxHP;
	bDead = false;
}

void AUnitCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();    
}

void AUnitCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopStuckMonitor();
	StopCellUpdate();

	EngagementSlots.Empty();

	if (UWorld* W = GetWorld())
	{
		if (UBattleDirectorSubsystem* BD = W->GetSubsystem<UBattleDirectorSubsystem>())
		{
			BD->UnregisterUnit(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void AUnitCharacter::SetData(UUnitData* InData)
{
	Data = InData;

	StatAttributeSet->SetAttack(Data->GetStat(EStatusType::Attack));
	StatAttributeSet->SetDefense(Data->GetStat(EStatusType::Defence));
	StatAttributeSet->SetHealth(Data->GetStat(EStatusType::Health));
	StatAttributeSet->SetMaxHealth(Data->GetStat(EStatusType::Health));

	//기본 무기에 따른 공격/스킬 적용
	AttackData = InData->GetAttackData();
	CreateAttackAbility();

	SkillDataList = InData->GetSkillData();
	CreateSkillAbility();
}

void AUnitCharacter::SetOwnerSpawner(ABaseUnitSpawner* InSpawner)
{
	OwnerSpawner = InSpawner;
}

int32 AUnitCharacter::FindSlotOfAttacker(AActor* Attacker) const
{
	if (!Attacker) return INDEX_NONE;

	for (int32 i = 0; i < EngagementSlots.Num(); ++i)
	{
		if (EngagementSlots[i].Attacker.Get() == Attacker)
			return i;
	}
	return INDEX_NONE;
}

int32 AUnitCharacter::FindFirstFreeSlot() const
{
	for (int32 i = 0; i < EngagementSlots.Num(); ++i)
	{
		if (!EngagementSlots[i].Attacker.IsValid())
			return i;
	}
	return INDEX_NONE;
}

bool AUnitCharacter::HasFreeSlot() const
{
	return FindFirstFreeSlot() != INDEX_NONE;
}

bool AUnitCharacter::CanAttackNow(float Now) const
{
	// Attack Cooldown은 BT에서 Wait Task로 대체(2s ~ 3s)
	return true;
}

bool AUnitCharacter::PerformAttack(AActor* Target)
{
	if (!Target || IsDead()) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	const float Now = World->GetTimeSeconds();
	if (!CanAttackNow(Now)) return false; // 쿨타임 체크 활성화

	//이게 몽타주 등 AnimNotify에서 공격 시점에 수행해야 데미지 적용
	ApplyAttack(Target);

	// 2. 애니메이션 재생
	if (AttackMontage)
	{
		// PlayAnimMontage는 내부적으로 AnimInstance를 찾아 실행해줍니다.
		float Duration = PlayAnimMontage(AttackMontage);
		if (Duration > 0.f)
		{
			LastAttackTime = Now;

			// 공격 성공 => 정체 카운트다운 리셋(오탐 방지)
			ResetStuckCountdown(true);

			return true;
		}
	}

	// 몽타주가 없을 경우를 대비한 Fallback (즉시 공격)
	LastAttackTime = Now;

	// 몽타주 없어도 공격 성공으로 간주하니 리셋
	ResetStuckCountdown(true);

	return true;
}

//TODO : Attributte에서 사망 시 호출로 변경
void AUnitCharacter::HandleDeath()
{
	if (bDead) return;
	bDead = true;

	// 1) 예약/슬롯 정리(핵심)
	UnregisterFromBattleDirector(true);

	// 2) AI/이동 정지
	if (AUnitAIController* AIC = Cast<AUnitAIController>(GetController()))
	{
		AIC->StopMovement();
		AIC->ClearFocus(EAIFocusPriority::Gameplay);

		ClearRuntimeBlackboardState(AIC);

		if (UPathFollowingComponent* PFC = AIC->GetPathFollowingComponent())
		{
			PFC->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished);
		}

		if (UBrainComponent* BC = AIC->BrainComponent)
		{
			BC->StopLogic(TEXT("Dead"));
		}
	}

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->DisableMovement();
	}

	// 3) 충돌/피격 방지
	if (UCapsuleComponent* Cap = GetCapsuleComponent())
	{
		Cap->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 4) 일정 시간 후 "풀로 반환"
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DestroyTimerHandle);

		const float Delay = FMath::Max(0.f, DestroyDelay);

		if (Delay <= 0.f)
		{
			// 즉시 반환
			if (UObjectPoolSubsystem* Pool = GetPool())
			{
				if (MyPoolType != EPoolTypes::None)
				{
					Pool->Release<AUnitCharacter>(MyPoolType, this);
					return;
				}
			}
			// fallback
			Destroy();
			return;
		}

		World->GetTimerManager().SetTimer(
			DestroyTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this]()
				{
					if (UObjectPoolSubsystem* Pool = GetPool())
					{
						if (MyPoolType != EPoolTypes::None)
						{
							Pool->Release<AUnitCharacter>(MyPoolType, this);
							return;
						}
					}
					Destroy();
				}),
			Delay,
			false
		);
	}
	else
	{
		Destroy();
	}
}

void AUnitCharacter::OnGet_Implementation()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	ResetForReuse();
	bInPool = false;
}

void AUnitCharacter::OnRelease_Implementation()
{
	StopStuckMonitor();

	// 풀로 반환될 때마다 서브시스템 해제
	UnregisterFromBattleDirector(true);

	if (AbilitySystemComponent)
	{
		// 진행중인 능력 중단
		AbilitySystemComponent->CancelAllAbilities();

		// 남아있는 큐/이펙트 정리 (버프/디버프/쿨다운 잔재 방지)
		AbilitySystemComponent->RemoveAllGameplayCues();
		AbilitySystemComponent->RemoveActiveEffectsWithSourceTags(FGameplayTagContainer());

		AbilitySystemComponent->ClearAllAbilities();
	}

	// 타이머/이동 잔여 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearAllTimersForObject(this);
	}

	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->DisableMovement();
	}

	// 스포너 AliveCount 반영
	if (OwnerSpawner.IsValid())
	{
		OwnerSpawner->NotifyUnitReleased(this);
	}

	if (Brain)
	{
		Brain->ResetRuntimeBrainState();

		if (Brain->Team == ETeamType::Enemy)
		{
			if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
			{
				WorldEventSystem->Battle.OnManaAdd.Broadcast(20);
			}
		}
	}

	if (AUnitAIController* AIC = Cast<AUnitAIController>(GetController()))
	{
		AIC->StopMovement();
		AIC->ClearFocus(EAIFocusPriority::Gameplay);

		ClearRuntimeBlackboardState(AIC);

		if (UPathFollowingComponent* PFC = AIC->GetPathFollowingComponent())
		{
			PFC->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished);
		}

		if (UBrainComponent* BC = AIC->BrainComponent)
		{
			BC->StopLogic(TEXT("ReleasedToPool"));
		}
	}

	bInPool = true;
	OwnerSpawner = nullptr;
}

void AUnitCharacter::OnTargetAssigned_Implementation(AActor* NewTarget)
{
	if (IsDead()) return;

	// 중복 호출 방지: 같은 타깃이면 아무것도 안 함
	if (LastAssignedTarget.Get() == NewTarget)
		return;

	LastAssignedTarget = NewTarget;

	// 타깃이 있으면 Run, 없으면 Walk
	const bool bHasTarget = (NewTarget != nullptr);

	if (bRunWhenHasTarget && bHasTarget)
	{
		ApplyMoveSpeed(RunSpeed);
	}
	else
	{
		ApplyMoveSpeed(WalkSpeed);

		if (AUnitAIController* AIC = Cast<AUnitAIController>(GetController()))
		{
			// 타깃이 사라졌으면 즉시 이동 중단(발키리 캐릭터 추적 중단을 위함)
			if (!IsValid(NewTarget))
			{
				AIC->StopMovement();
			}
		}
	}
}

void AUnitCharacter::ResetStuckCountdown(bool bAlsoClearBB)
{
	StuckAccumSeconds = 0.f;

	// 필요하면 BB 플래그도 내려줌(기본 true)
	if (bAlsoClearBB)
	{
		SetNeedToEscapeBB(false);
	}
}

UBattleDirectorSubsystem* AUnitCharacter::GetBattleDirector() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetSubsystem<UBattleDirectorSubsystem>();
	}
	return nullptr;
}

void AUnitCharacter::RegisterToBattleDirector()
{
	if (bRegisteredToBattleDirector) return;

	UBattleDirectorSubsystem* Subsys = GetBattleDirector();
	if (!Subsys || !Brain) return;

	Subsys->RegisterUnit(this);
	bRegisteredToBattleDirector = true;
}

void AUnitCharacter::UnregisterFromBattleDirector(bool bReleaseAttackersIfIAmTarget)
{
	if (!bRegisteredToBattleDirector)
	{
		return;
	}

	UBattleDirectorSubsystem* Subsys = GetBattleDirector();
	if (!Subsys || !Brain)
	{
		bRegisteredToBattleDirector = false;
		return;
	}

	if (bReleaseAttackersIfIAmTarget)
	{
		Subsys->ReleaseAllAttackersOfTarget(this);
	}

	Subsys->UnregisterUnit(this);
	bRegisteredToBattleDirector = false;
}

// --------------------
// Pool helper
// --------------------

UObjectPoolSubsystem* AUnitCharacter::GetPool() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetSubsystem<UObjectPoolSubsystem>();
	}
	return nullptr;
}

// --------------------
// Reuse reset (OnGet에서만 호출)
// --------------------

void AUnitCharacter::ResetForReuse()
{
	// 상태 리셋
	CurrentHP = MaxHP;
	bDead = false;
	LastAttackTime = -1000.f;

	// 죽음 타이머 제거
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DestroyTimerHandle);
	}

	// Auto 타깃팅 기본 룰
	if (Brain && Brain->TargetingPolicy == ETargetingPolicy::Auto)
	{
		Brain->TargetingPolicy = ETargetingPolicy::NearWallEnemyThenBase;
	}

	// 슬롯 보장 + 잔여 슬롯 흔적 제거(안전)
	const int32 SlotCount = FMath::Clamp(MaxEngagementSlots, 1, 3);
	EngagementSlots.SetNum(SlotCount);
	for (int32 i = 0; i < EngagementSlots.Num(); ++i)
	{
		EngagementSlots[i].Reset();
	}

	// 이동/회전 복구
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->StopMovementImmediately();
		Move->SetMovementMode(EMovementMode::MOVE_Walking);
		Move->MaxWalkSpeed = WalkSpeed;
		Move->Activate(true);
	}

	// 충돌 복구
	if (UCapsuleComponent* Cap = GetCapsuleComponent())
	{
		Cap->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	AUnitAIController* AIC = Cast<AUnitAIController>(GetController());
	if (!AIC)
	{
		SpawnDefaultController();
		AIC = Cast<AUnitAIController>(GetController());
	}

	if (AIC)
	{
		AIC->StopMovement();
		AIC->ClearFocus(EAIFocusPriority::Gameplay);

		if (UPathFollowingComponent* PFC = AIC->GetPathFollowingComponent())
		{
			PFC->AbortMove(*this, FPathFollowingResultFlags::OwnerFinished);
		}

		ClearRuntimeBlackboardState(AIC);
		
		if (UBrainComponent* BC = AIC->BrainComponent)
		{
			BC->RestartLogic();
		}
	}

	LastAssignedTarget = nullptr;

	// BB escape 플래그/카운터 초기화
	ResetStuckCountdown(true);

	// BD 등록(활성화 시점)
	RegisterToBattleDirector();

	// 정체 모니터 시작
	StartStuckMonitor();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			CellSyncTimerHandle,
			FTimerDelegate::CreateUObject(this, &AUnitCharacter::CellSyncTick),
			FMath::Max(0.05f, CellSyncInterval),
			true
		);
	}
}

void AUnitCharacter::ApplyMoveSpeed(float NewSpeed)
{
	if (IsDead()) return;

	UCharacterMovementComponent* Move = GetCharacterMovement();
	if (!Move) return;

	// DisableMovement 상태면 굳이 바꿔도 의미가 없으니 방어
	if (Move->MovementMode == MOVE_None)
		return;

	// 중복 세팅 방지 (미세 오차 고려)
	if (FMath::IsNearlyEqual(Move->MaxWalkSpeed, NewSpeed, 0.1f))
		return;

	Move->MaxWalkSpeed = NewSpeed;
}

void AUnitCharacter::StartStuckMonitor()
{
	if (!bEnableEscapeWhenStuck) return;

	UWorld* World = GetWorld();
	if (!World) return;

	// 이미 돌고 있으면 중복 시작 방지
	if (World->GetTimerManager().IsTimerActive(StuckMonitorTimerHandle))
		return;

	World->GetTimerManager().SetTimer(
		StuckMonitorTimerHandle,
		FTimerDelegate::CreateUObject(this, &AUnitCharacter::StuckMonitorTick),
		FMath::Max(0.05f, StuckCheckInterval),
		true
	);
}

void AUnitCharacter::StopStuckMonitor()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(StuckMonitorTimerHandle);
	}
}

void AUnitCharacter::StuckMonitorTick()
{
	if (!bEnableEscapeWhenStuck) return;
	if (IsDead()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	AAIController* AIC = Cast<AAIController>(GetController());
	if (!AIC) return;

	UPathFollowingComponent* PFC = AIC->GetPathFollowingComponent();
	if (!PFC) return;

	// 1) "진짜 이동 중(Moving)"일 때만 정체 카운트다운
	if (PFC->GetStatus() != EPathFollowingStatus::Moving)
	{
		// 이동 중이 아니라면 정체 누적을 하지 않음
		StuckAccumSeconds = 0.f;
		return;
	}

	// 2) 공격이 성공하면 PerformAttack에서 리셋되니까 오탐이 크게 줄어듦.

	const float Speed2D = GetVelocity().Size2D();

	if (Speed2D <= StuckSpeedThreshold)
	{
		StuckAccumSeconds += FMath::Max(0.0f, StuckCheckInterval);
	}
	else
	{
		// 움직임이 있으면 누적 리셋 + 필요하면 플래그도 내림
		ResetStuckCountdown(true);
		return;
	}

	// 3) 타임아웃 도달 시 탈출 요청(쿨다운 포함)
	const float Now = World->GetTimeSeconds();

	const bool bCooldownOK = (Now - LastEscapeRequestTime) >= EscapeRequestCooldown;
	if (StuckAccumSeconds >= StuckTimeoutSeconds && bCooldownOK)
	{
		LastEscapeRequestTime = Now;
		StuckAccumSeconds = 0.f; // 요청 후 누적 리셋(연속 요청 방지)

		SetNeedToEscapeBB(true);
	}
}

void AUnitCharacter::SetNeedToEscapeBB(bool bValue)
{
	if (IsDead()) return;

	AAIController* AIC = Cast<AAIController>(GetController());
	if (!AIC) return;

	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB) return;

	BB->SetValueAsBool(BB_NeedToEscapeKey, bValue);
}

void AUnitCharacter::ClearRuntimeBlackboardState(AUnitAIController* AIC)
{
	if (!AIC) return;

	if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
	{
		AIC->ClearBBKeySafe(BB, TEXT("ReservedTarget"));
		AIC->ClearBBKeySafe(BB, TEXT("EnemyBase"));
		AIC->ClearBBKeySafe(BB, TEXT("MoveGoal"));
		AIC->ClearBBKeySafe(BB, TEXT("TargetLocation"));
		AIC->ClearBBKeySafe(BB, TEXT("HasTarget"));
		AIC->ClearBBKeySafe(BB, TEXT("NeedToEscape"));
	}
}

void AUnitCharacter::CellSyncTick()
{
	if (IsDead()) return;
	if (!bRegisteredToBattleDirector) return;

	UBattleDirectorSubsystem* BD = GetBattleDirector();
	if (!BD) return;

	// 셀 계산은 BD와 동일한 규칙이어야 함.
	// 가장 깔끔한 방법: BD가 "셀 키 계산 함수"를 공개하거나,
	// BD->NotifyUnitMoved 안에서 계산하게 한다.
	BD->NotifyUnitMoved(this);
}

void AUnitCharacter::StopCellUpdate()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(CellSyncTimerHandle);
	}
}

void AUnitCharacter::ExecuteAttack()
{
	if (IsDead()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const float Now = World->GetTimeSeconds();
	if (!CanAttackNow(Now)) return; // 쿨타임 체크 활성화

	//성현우[TODO] : 쿨타임 테이블 추가 및 어빌리티에 설정하는게 맞을 것 같다.
	
	// 2. 애니메이션 재생
	if (AttackMontage)
	{
		// PlayAnimMontage는 내부적으로 AnimInstance를 찾아 실행해줍니다.
		float Duration = PlayAnimMontage(AttackMontage);
		if (Duration > 0.f)
		{
			LastAttackTime = Now;

			// 공격 성공 => 정체 카운트다운 리셋(오탐 방지)
			ResetStuckCountdown(true);

			return;
		}
	}

	// 몽타주가 없을 경우를 대비한 Fallback (즉시 공격)
	LastAttackTime = Now;

	// 몽타주 없어도 공격 성공으로 간주하니 리셋
	ResetStuckCountdown(true);
}

void AUnitCharacter::ExecuteSkill(int32 InSkillIndex)
{
	//스킬 몽타주 실행
}

void AUnitCharacter::OnAttackNotify()
{
	if (CurrentTarget)
	{
		ApplyAttack(CurrentTarget);
	}
}

void AUnitCharacter::OnSkillNotify()
{
	//타겟이 있으면 타겟으로
	//ApplySkill()
}

void AUnitCharacter::OnDeath()
{
	HandleDeath();
}
