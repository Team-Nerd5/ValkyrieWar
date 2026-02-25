// Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Character/Unit/UnitCharacter.h"
#include "Object/Unit/Component/UnitBrainComponent.h"
#include "Object/AI/Controller/UnitAIController.h"
#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "GameSystem/Base/BaseUnitSpawner.h"
#include "Data/Struct/UnitEngagementSlotData.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "Components/CapsuleComponent.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

AUnitCharacter::AUnitCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	Brain = CreateDefaultSubobject<UUnitBrainComponent>(TEXT("Brain"));

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AUnitCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

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

	// Destroy 되는 예외 대비 안전장치
	UnregisterFromBattleDirector(true);

	EngagementSlots.Empty();
	Super::EndPlay(EndPlayReason);
}

void AUnitCharacter::SetData(UUnitData* InData)
{
	Data = InData;

	//기본 무기에 따른 공격/스킬 적용
	AttackData = InData->GetAttackData();
	SkillDataList = InData->GetSkillData();
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

void AUnitCharacter::ApplyAttack(AActor* InTargetActor)
{
	ApplyAttackDamage(InTargetActor);
	ApplyAttackEffects(InTargetActor);

	Super::ApplyAttack(InTargetActor);
}

void AUnitCharacter::ApplyAttackDamage(AActor* Target)
{
	if (!Target || IsDead()) return;

	//AController* InstigatorCtrl = GetController();
	//UGameplayStatics::ApplyDamage(Target, AttackDamage, InstigatorCtrl, this, UDamageType::StaticClass());

	if (bDrawDebug)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Red, false, 0.5f, 0, 1.5f);
	}
}

float AUnitCharacter::TakeDamage(
	float DamageAmount,
	const FDamageEvent& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCauser
)
{
	if (IsDead()) return 0.f;

	const float Applied = FMath::Max(0.f, DamageAmount);
	if (Applied <= 0.f) return 0.f;

	CurrentHP = FMath::Clamp(CurrentHP - Applied, 0.f, MaxHP);

	if (bDrawDebug)
	{
		UE_LOG(LogTemp, Log, TEXT("[%s] TakeDamage: %.1f  (HP: %.1f / %.1f)"),
			*GetName(), Applied, CurrentHP, MaxHP);
	}

	if (CurrentHP <= 0.f && !bDead)
	{
		HandleDeath(EventInstigator, DamageCauser);
	}

	return Applied;
}

void AUnitCharacter::HandleDeath(AController* Killer, AActor* DamageCauser)
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

		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			AIC->ClearBBKeySafe(BB, TEXT("ReservedTarget"));
			AIC->ClearBBKeySafe(BB, TEXT("EnemyBase"));
			AIC->ClearBBKeySafe(BB, TEXT("MoveGoal"));
			AIC->ClearBBKeySafe(BB, TEXT("TargetLocation"));
			AIC->ClearBBKeySafe(BB, TEXT("HasTarget"));
		}

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

	// 풀에서 꺼내질 때마다(재사용 포함) 초기화 + 서브시스템 등록
	ResetForReuse();

	// AI 재가동
	if (!GetController())
	{
		// 혹시라도 컨트롤러가 없는 예외 케이스
		SpawnDefaultController();
	}

	if (AAIController* AIC = Cast<AAIController>(GetController()))
	{
		// BT 사용 중이면 RestartLogic로 충분한 경우가 많음
		if (UBrainComponent* BrainComp = AIC->BrainComponent)
		{
			BrainComp->RestartLogic();
		}
	}

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

	if (Brain) Brain->ResetRuntimeBrainState();

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

	// 컨트롤러가 UnPossess 되었을 수 있으니 보장
	if (!GetController())
	{
		SpawnDefaultController();
	}

	LastAssignedTarget = nullptr;

	// BB escape 플래그/카운터 초기화
	ResetStuckCountdown(true);

	// BD 등록(활성화 시점)
	RegisterToBattleDirector();

	// 정체 모니터 시작
	StartStuckMonitor();
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

void AUnitCharacter::ApplyAttackEffects(AActor* TargetActor)
{
	if (!TargetActor || IsDead()) return;
	if (!AbilitySystemComponent) return;

	UAbilitySystemComponent* TargetASC =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!TargetASC) return;

	// ============================================
	// 1. 더미 테스트 이펙트 먼저 적용
	// ============================================
	//if (DebugTestEffect)
	//{
	//	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	//	Context.AddSourceObject(this);

	//	FGameplayEffectSpecHandle SpecHandle =
	//		AbilitySystemComponent->MakeOutgoingSpec(DebugTestEffect, DebugTestEffectLevel, Context);

	//	if (SpecHandle.IsValid())
	//	{
	//		FActiveGameplayEffectHandle Handle =
	//			AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
	//				*SpecHandle.Data.Get(), TargetASC);

	//		UE_LOG(LogTemp, Warning, TEXT("[DEBUG TEST EFFECT] Applied = %s"),
	//			Handle.IsValid() ? TEXT("TRUE") : TEXT("FALSE"));
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("[DEBUG TEST EFFECT] Spec invalid"));
	//	}
	//}

	// ============================================
	// 기존 데이터 기반 이펙트 적용 로직
	// ============================================

	if (!AttackData)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATTACK EFFECT] No AttackData!"));
		return;
	}

	const TArray<USkillEffectData*> Effects = AttackData->GetEffectList();

	if (Effects.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("[ATTACK EFFECT] No Effects!"));
		return;
	}

	FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	for (USkillEffectData* EffectData : Effects)
	{
		if (!EffectData) continue;

		UGameplayEffect* GEDef = BuildGameplayEffect(EffectData);
		if (!GEDef)
		{
			UE_LOG(LogTemp, Warning, TEXT("[ATTACK EFFECT] BuildGameplayEffect FAILED. Unit=%s Target=%s"),
				*GetName(), *GetNameSafe(TargetActor));
			continue;
		}

		FGameplayEffectSpec Spec(GEDef, Context, 1.0f);

		// 적용 결과 핸들로 성공/실패 판별
		FActiveGameplayEffectHandle Handle =
			AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(Spec, TargetASC);

		// 로그(성공/실패 + 어떤 이펙트인지)
		const bool bApplied = Handle.IsValid();

		// (가능한 정보들) 클래스명 / DurationPolicy / Period / GrantedTags / ModifierCount
		const FString GEName = GEDef->GetClass() ? GEDef->GetClass()->GetName() : TEXT("None");
		const float Period = GEDef->Period.GetValueAtLevel(1.0f);
		const int32 ModCount = GEDef->Modifiers.Num();

		FGameplayTagContainer GrantedTags =
			GEDef->InheritableOwnedTagsContainer.Added;

		UE_LOG(LogTemp, Log,
			TEXT("[ATTACK EFFECT] Applied | Unit=%s Target=%s | GE=%s | DurationPolicy=%d Period=%.2f Mods=%d Tags=%s"),
			*GetName(),
			*GetNameSafe(TargetActor),
			*GEName,
			(int32)GEDef->DurationPolicy,
			Period,
			ModCount,
			*GrantedTags.ToString()
		);
	}
}

UGameplayEffect* AUnitCharacter::BuildGameplayEffect(USkillEffectData* EffectData) const
{
	if (!EffectData) return nullptr;

	UGameplayEffect* GE = NewObject<UGameplayEffect>(GetTransientPackage(), NAME_None, RF_Transient);

	if (!GE) return nullptr;

	GE->DurationPolicy = EffectData->GetDurationPolicy();

	if (GE->DurationPolicy == EGameplayEffectDurationType::HasDuration)
	{
		GE->DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(EffectData->GetDuration()));
	}

	// Periodic (주기 적용)
	const float Period = EffectData->GetPeriod();
	if (Period > 0.f)
	{
		GE->Period = FScalableFloat(Period);
	}

	// Granted Tags
	{
		FInheritedTagContainer GamePlayTags;
		GamePlayTags.Added = EffectData->GetGrantedTags();
		GE->InheritableOwnedTagsContainer = GamePlayTags;
	}

	// GameplayCue
	if (EffectData->GetCueTag().IsValid())
	{
		FGameplayEffectCue Cue;
		Cue.GameplayCueTags.AddTag(EffectData->GetCueTag());
		GE->GameplayCues.Add(Cue);
	}

	// Modifier (Attribute 변화)
	// - Value를 단순 add/mul/override 등 Op로 적용
	// - TargetAttribute를 수정
	{
		const FGameplayAttribute TargetAttr = EffectData->GetTargetAttribute();
		if (TargetAttr.IsValid())
		{
			FGameplayModifierInfo Mod;
			Mod.Attribute = TargetAttr;
			Mod.ModifierOp = EffectData->GetOp();

			// 값 적용 방식: 기본은 고정 값
			// bUseSourceAttribute 같은 “소스 스탯 기반 계산”은 여기서 확장 가능
			const float Value = EffectData->GetApplyValue();
			Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Value));

			GE->Modifiers.Add(Mod);
		}
	}

	return GE;
}
