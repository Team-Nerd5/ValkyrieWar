// Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Character/Unit/UnitCharacter.h"
#include "Object/Unit/Component/UnitBrainComponent.h"
#include "Object/Unit/AI/Controller/UnitAIController.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "GameSystem/Base/BaseUnitSpawner.h"
#include "GameSystem/Base/BaseProjectile.h"
#include "GameSystem/Base/BaseWall.h"
#include "GameSystem/Base/BaseAnimInstance.h"

#include "Data/Struct/UnitEngagementSlotData.h"
#include "Data/Attribute/StatAttributeSet.h"
#include "Data/Struct/ComputedEnemyStat.h"

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

	InitProjectilePoolIfNeeded();
}

void AUnitCharacter::SetComputedEnemyData(UUnitData* InBaseData, const FComputedEnemyStat& InComputedStat)
{
	Data = InBaseData;
	if (!Data) return;

	// 계산된 최종 스탯 적용
	StatAttributeSet->SetAttack(InComputedStat.Attack);
	StatAttributeSet->SetDefense(InComputedStat.Defence);
	StatAttributeSet->SetHealth(InComputedStat.Health);
	StatAttributeSet->SetMaxHealth(InComputedStat.Health);

	if (LocomotionBS)
	{
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			if (UBaseAnimInstance* BaseInstance = Cast<UBaseAnimInstance>(AnimInstance))
			{
				BaseInstance->SetInstacne(LocomotionBS, this);
			}
		}
	}

	AttackData = InBaseData->GetAttackData();
	CreateAttackAbility();

	SkillDataList = InBaseData->GetSkillData();
	CreateSkillAbility();

	InitProjectilePoolIfNeeded();
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
	bDead = false;

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

bool AUnitCharacter::FireProjectileAttack()
{
	if (IsDead()) return false;
	if (!AttackData) return false;

	const FProjectileDataRow* ProjectileData = AttackData->GetProjectileData();
	if (!ProjectileData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnitCharacter] AttackType is Projectile but ProjectileData is null."));
		return false;
	}

	if (ProjectileData->EPoolTypes == EPoolTypes::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnitCharacter] Projectile PoolType is None."));
		return false;
	}

	UObjectPoolSubsystem* Pool = GetPool();
	if (!Pool)
	{
		UE_LOG(LogTemp, Error, TEXT("[UnitCharacter] ObjectPoolSubsystem is null."));
		return false;
	}

	FVector SpawnLocation;
	FRotator SpawnRotation;
	if (!GetProjectileSpawnTransform(SpawnLocation, SpawnRotation))
	{
		return false;
	}

	ABaseProjectile* Projectile = Pool->Get<ABaseProjectile>(
		ProjectileData->EPoolTypes,
		SpawnLocation,
		SpawnRotation
	);

	if (!Projectile)
	{
		UE_LOG(LogTemp, Error, TEXT("[UnitCharacter] Failed to get projectile from pool. PoolType=%d"), (int32)ProjectileData->EPoolTypes);
		return false;
	}

	Projectile->SetTeam(GetTeamType());

	Projectile->SetActorRotation(SpawnRotation);
	Projectile->SetOwner(this);
	Projectile->SetInstigator(this);

	Projectile->SetData(
		AttackData->GetAbilityTag(),
		AttackSpec,
		*ProjectileData,
		AttackData->GetCue(EGameplayCueOrder::InProjectile)
	);

	ResetStuckCountdown(true);

	return true;
}

bool AUnitCharacter::GetProjectileSpawnTransform(FVector& OutLocation, FRotator& OutRotation) const
{
	OutLocation = GetActorLocation() + GetActorForwardVector() * 70.f + FVector(0.f, 0.f, 80);
	OutRotation = GetActorRotation();

	// 현재 타깃이 있으면 타깃 방향으로 회전
	if (CurrentTarget)
	{
		const FVector Dir = (CurrentTarget->GetActorLocation() - OutLocation).GetSafeNormal();
		if (!Dir.IsNearlyZero())
		{
			OutRotation = Dir.Rotation();
		}
		return true;
	}

	// 메시에 FirePos 소켓이 있으면 우선 사용
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		static const FName FireSocketName(TEXT("FirePos"));

		if (MeshComp->DoesSocketExist(FireSocketName))
		{
			OutLocation = MeshComp->GetSocketLocation(FireSocketName);

			if (CurrentTarget)
			{
				const FVector Dir = (CurrentTarget->GetActorLocation() - OutLocation).GetSafeNormal();
				if (!Dir.IsNearlyZero())
				{
					OutRotation = Dir.Rotation();
				}
			}
			else
			{
				OutRotation = MeshComp->GetSocketRotation(FireSocketName);
			}
			return true;
		}
	}

	return true;
}

void AUnitCharacter::InitProjectilePoolIfNeeded()
{
	if (!AttackData) return;
	if (AttackData->GetAttackType() != EAttackType::Projectile) return;

	const FProjectileDataRow* ProjectileData = AttackData->GetProjectileData();
	if (!ProjectileData) return;
	if (ProjectileData->EPoolTypes == EPoolTypes::None) return;
	if (ProjectileData->SpawnObject.IsNull()) return;

	if (UObjectPoolSubsystem* Pool = GetPool())
	{
		UClass* ProjectileClass = ProjectileData->SpawnObject.LoadSynchronous();
		if (!ProjectileClass)
		{
			UE_LOG(LogTemp, Error, TEXT("[UnitCharacter] Projectile class load failed."));
			return;
		}

		// 같은 PoolType / 같은 클래스라면 내부에서 중복 방어되도록 ObjectPoolSubsystem 쪽에서 처리하는 게 제일 좋음
		Pool->InitPool<ABaseProjectile>(ProjectileData->EPoolTypes, ProjectileClass, 10);
	}
}

void AUnitCharacter::CollectSplashTargets(AActor* MainTarget, int32 SplashTargetAmount, float SplashRange, TArray<AActor*>& OutTargets) const
{
	if (!MainTarget)
	{
		return;
	}

	if (SplashTargetAmount <= 0 || SplashRange <= 0.f)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Center = MainTarget->GetActorLocation();

	DrawDebugSplashRange(Center, SplashRange, MainTarget);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(const_cast<AUnitCharacter*>(this));
	IgnoreActors.Add(MainTarget);

	TArray<AActor*> OverlapActors;

	const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		World,
		Center,
		SplashRange,
		ObjectTypes,
		AUnitCharacter::StaticClass(),
		IgnoreActors,
		OverlapActors
	);

	if (!bHit || OverlapActors.IsEmpty())
	{
		DrawDebugSplashSummary(MainTarget, SplashTargetAmount, SplashRange, 0, 0);
		return;
	}

	struct FSplashCandidate
	{
		AActor* Target = nullptr;
		float DistSq = 0.f;
	};

	TArray<FSplashCandidate> Candidates;
	Candidates.Reserve(OverlapActors.Num());

	for (AActor* Actor : OverlapActors)
	{
		if (!IsValidAttackTargetActor(Actor))
		{
			if (bDrawDebug && Actor)
			{
				DrawDebugSphere(
					World,
					Actor->GetActorLocation() + FVector(0.f, 0.f, 30.f),
					25.f,
					12,
					FColor::Red,
					false,
					2.0f
				);
			}
			continue;
		}

		const float DistSq = FVector::DistSquared(Center, Actor->GetActorLocation());
		Candidates.Add({ Actor, DistSq });
	}

	if (Candidates.IsEmpty())
	{
		DrawDebugSplashSummary(MainTarget, SplashTargetAmount, SplashRange, 0, 0);
		return;
	}

	Candidates.Sort([](const FSplashCandidate& A, const FSplashCandidate& B)
		{
			return A.DistSq < B.DistSq;
		});

	const int32 AddCount = FMath::Min(SplashTargetAmount, Candidates.Num());

	for (int32 i = 0; i < Candidates.Num(); ++i)
	{
		const bool bSelected = i < AddCount;
		AActor* TargetActor = Candidates[i].Target;

		DrawDebugSplashCandidate(TargetActor, Center, bSelected, i, Candidates[i].DistSq);

		if (bSelected)
		{
			OutTargets.Add(TargetActor);
		}
	}

	DrawDebugSplashSummary(MainTarget, SplashTargetAmount, SplashRange, Candidates.Num(), AddCount);
}

bool AUnitCharacter::IsValidAttackTargetActor(const AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("!IsValid(TargetActor)"));
		return false;
	}

	if (TargetActor == this)
	{
		return false;
	}

	// 유닛인 경우
	if (const AUnitCharacter* TargetUnit = Cast<AUnitCharacter>(TargetActor))
	{
		if (TargetUnit->IsDead())
		{
			return false;
		}

		if (TargetUnit->GetTeamType() == GetTeamType())
		{
			return false;
		}

		return true;
	}

	// 플레이어 캐릭터인 경우
	if (const AValkyrieCharacter* TargetCharacter = Cast<AValkyrieCharacter>(TargetActor))
	{
		if (GetTeamType() == ETeamType::Ally)
		{
			return false;
		}

		return true;
	}

	// 벽인 경우
	if (const ABaseWall* TargetWall = Cast<ABaseWall>(TargetActor))
	{
		if (TargetWall->GetTeamType() == GetTeamType())
		{
			return false;
		}

		return true;
	}

	return false;
}

void AUnitCharacter::ExecuteAttack()
{
	if (IsDead()) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const float Now = World->GetTimeSeconds();

	//성현우[TODO] : 쿨타임 테이블 추가 및 어빌리티에 설정하는게 맞을 것 같다.
	
	// 2. 애니메이션 재생
	if (AttackMontage)
	{
		// PlayAnimMontage는 내부적으로 AnimInstance를 찾아 실행해줍니다.
		float Duration = PlayAnimMontage(AttackMontage);
		if (Duration > 0.f)
		{
			// 공격 성공 => 정체 카운트다운 리셋(오탐 방지)
			ResetStuckCountdown(true);

			return;
		}
	}

	// 몽타주 없어도 공격 성공으로 간주하니 리셋
	ResetStuckCountdown(true);
}

void AUnitCharacter::ExecuteSkill(int32 InSkillIndex)
{
	//스킬 몽타주 실행
}

void AUnitCharacter::DrawDebugSplashRange(const FVector& Center, float Radius, AActor* MainTarget) const
{
	if (!bDrawDebug)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 스플래시 범위
	DrawDebugSphere(
		World,
		Center,
		Radius,
		24,
		FColor::Yellow,
		false,
		2.0f,
		0,
		2.0f
	);

	// 메인 타겟 강조
	if (MainTarget)
	{
		DrawDebugSphere(
			World,
			MainTarget->GetActorLocation() + FVector(0.f, 0.f, 80.f),
			45.f,
			16,
			FColor::Orange,
			false,
			2.0f,
			0,
			3.0f
		);

		DrawDebugString(
			World,
			MainTarget->GetActorLocation() + FVector(0.f, 0.f, 120.f),
			TEXT("MainTarget"),
			nullptr,
			FColor::Orange,
			2.0f,
			false
		);
	}
}

void AUnitCharacter::DrawDebugSplashCandidate(AActor* TargetActor, const FVector& Center, bool bSelected, int32 Rank, float DistSq) const
{
	if (!bDrawDebug)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World || !TargetActor)
	{
		return;
	}

	const FVector TargetLoc = TargetActor->GetActorLocation();
	const FVector DebugLoc = TargetLoc + FVector(0.f, 0.f, 50.f);
	const float Dist = FMath::Sqrt(DistSq);

	const FColor DebugColor = bSelected ? FColor::Green : FColor::Red;
	const float DebugRadius = bSelected ? 35.f : 28.f;
	const int32 Segments = bSelected ? 16 : 12;
	const float Thickness = bSelected ? 2.5f : 1.5f;

	DrawDebugSphere(
		World,
		DebugLoc,
		DebugRadius,
		Segments,
		DebugColor,
		false,
		2.0f,
		0,
		Thickness
	);

	if (bSelected)
	{
		DrawDebugLine(
			World,
			Center,
			TargetLoc,
			FColor::Blue,
			false,
			2.0f,
			0,
			1.5f
		);
	}

	DrawDebugString(
		World,
		TargetLoc + FVector(0.f, 0.f, 100.f),
		FString::Printf(TEXT("Rank:%d Dist:%.0f %s"), Rank, Dist, bSelected ? TEXT("[Selected]") : TEXT("[Rejected]")),
		nullptr,
		DebugColor,
		2.0f,
		false
	);
}

void AUnitCharacter::DrawDebugSplashSummary(AActor* MainTarget, int32 SplashTargetAmount, float SplashRange, int32 CandidateCount, int32 SelectedCount) const
{
	if (!bDrawDebug)
	{
		return;
	}

	UE_LOG(LogTemp, Log,
		TEXT("[SplashDebug] Main=%s SplashAmount=%d SplashRange=%.1f CandidateCount=%d SelectedCount=%d"),
		*GetNameSafe(MainTarget),
		SplashTargetAmount,
		SplashRange,
		CandidateCount,
		SelectedCount
	);
}

void AUnitCharacter::OnAttackNotify()
{
	if (IsDead()) return;
	if (!AttackData) return;

	switch (AttackData->GetAttackType())
	{
	case EAttackType::Melee:
	{
		TArray<AActor*> AttackTargets;
		CollectAttackTargets(AttackTargets);

		for (AActor* Target : AttackTargets)
		{
			if (!IsValidAttackTargetActor(Target))
			{
				continue;
			}

			ApplyAttack(Target);
		}
		break;
	}

	case EAttackType::Projectile:
		FireProjectileAttack();
		break;

	case EAttackType::OnTarget:
	default:
		break;
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

void AUnitCharacter::CollectAttackTargets(TArray<AActor*>& OutTargets) const
{
	OutTargets.Reset();

	if (IsDead())
	{
		return;
	}

	if (!AttackData)
	{
		return;
	}

	AActor* MainTarget = CurrentTarget;
	if (!IsValidAttackTargetActor(MainTarget))
	{
		return;
	}

	// 메인 타겟 추가
	OutTargets.Add(MainTarget);

	// 스플래시 규칙 확인
	const FTargetingDataRow& TargetingData = AttackData->GetTargetingData();

	if (TargetingData.SplashTargetAmount <= 0)
	{
		return;
	}

	if (TargetingData.SplashRange <= 0.f)
	{
		return;
	}

	// 주변 타겟 추가 수집
	CollectSplashTargets(
		MainTarget,
		TargetingData.SplashTargetAmount,
		TargetingData.SplashRange,
		OutTargets
	);
}
