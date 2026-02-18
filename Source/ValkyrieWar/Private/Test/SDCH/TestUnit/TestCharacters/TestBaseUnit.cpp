#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"
#include "Test/SDCH/TestUnit/TestComponents/TestUnitBrainComponent.h"
#include "Test/SDCH/TestActors/TestUnitSpawner.h"
#include "Test/SDCH/TestGameSystem/TestInstance/TestWorld/TestBattleDirectorSubsystem.h"
#include "Test/SDCH/TestUnit/TestCharacters/TestController/TestBaseAIController.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "BrainComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"

ATestBaseUnit::ATestBaseUnit()
{
	PrimaryActorTick.bCanEverTick = false;

	Brain = CreateDefaultSubobject<UTestUnitBrainComponent>(TEXT("TestBrain"));

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->MaxWalkSpeed = 450.f;

	CurrentHP = MaxHP;
}

void ATestBaseUnit::SetOwnerSpawner(ATestUnitSpawner* InSpawner)
{
	OwnerSpawner = InSpawner;
}

void ATestBaseUnit::BeginPlay()
{
	Super::BeginPlay();

	// BeginPlay는 "최초 1회"만.
	// 풀 워밍업/미사용 상태에서 유닛이 BD에 등록되면 유령 유닛이 생길 수 있으니,
	// 여기서는 불변 초기화만.
	EngagementSlots.SetNum(FMath::Clamp(MaxEngagementSlots, 1, 3));
	CurrentHP = MaxHP;
	bDead = false;
}

void ATestBaseUnit::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Destroy 되는 예외 대비 안전장치
	UnregisterFromBattleDirector(true);

	EngagementSlots.Empty();
	Super::EndPlay(EndPlayReason);
}

// --------------------
// BattleDirector helpers
// --------------------

UTestBattleDirectorSubsystem* ATestBaseUnit::GetBattleDirector() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetSubsystem<UTestBattleDirectorSubsystem>();
	}
	return nullptr;
}

void ATestBaseUnit::RegisterToBattleDirector()
{
	if (bRegisteredToBattleDirector) return;

	UTestBattleDirectorSubsystem* Subsys = GetBattleDirector();
	if (!Subsys || !Brain) return;

	Subsys->RegisterUnit(this);
	bRegisteredToBattleDirector = true;
}

void ATestBaseUnit::UnregisterFromBattleDirector(bool bReleaseAttackersIfIAmTarget)
{
	if (!bRegisteredToBattleDirector)
	{
		return;
	}

	UTestBattleDirectorSubsystem* Subsys = GetBattleDirector();
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

UObjectPoolSubsystem* ATestBaseUnit::GetPool() const
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

void ATestBaseUnit::ResetForReuse()
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
		Move->MaxWalkSpeed = 450.f;
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

	// BD 등록(활성화 시점)
	RegisterToBattleDirector();
}

// --------------------
// Slot utils
// --------------------

int32 ATestBaseUnit::FindSlotOfAttacker(AActor* Attacker) const
{
	if (!Attacker) return INDEX_NONE;

	for (int32 i = 0; i < EngagementSlots.Num(); ++i)
	{
		if (EngagementSlots[i].Attacker.Get() == Attacker)
			return i;
	}
	return INDEX_NONE;
}

int32 ATestBaseUnit::FindFirstFreeSlot() const
{
	for (int32 i = 0; i < EngagementSlots.Num(); ++i)
	{
		if (!EngagementSlots[i].Attacker.IsValid())
			return i;
	}
	return INDEX_NONE;
}

bool ATestBaseUnit::HasFreeSlot() const
{
	return FindFirstFreeSlot() != INDEX_NONE;
}

// --------------------
// Combat
// --------------------

bool ATestBaseUnit::CanAttackNow(float Now) const
{
	return (Now - LastAttackTime) >= AttackCooldown;
}

bool ATestBaseUnit::PerformAttack(AActor* Target)
{
	if (!Target || IsDead()) return false;

	UWorld* World = GetWorld();
	if (!World) return false;

	const float Now = World->GetTimeSeconds();
	if (!CanAttackNow(Now)) return false; // 쿨타임 체크 활성화

	ApplyAttackDamage(Target);

	// 2. 애니메이션 재생
	if (AttackMontage)
	{
		// PlayAnimMontage는 내부적으로 AnimInstance를 찾아 실행해줍니다.
		float Duration = PlayAnimMontage(AttackMontage);
		if (Duration > 0.f)
		{
			LastAttackTime = Now;

			return true;
		}
	}

	// 몽타주가 없을 경우를 대비한 Fallback (즉시 공격)
	LastAttackTime = Now;

	return true;
}

void ATestBaseUnit::ApplyAttackDamage(AActor* Target)
{
	if (!Target || IsDead()) return;

	AController* InstigatorCtrl = GetController();
	UGameplayStatics::ApplyDamage(Target, AttackDamage, InstigatorCtrl, this, UDamageType::StaticClass());

	if (bDrawDebug)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), Target->GetActorLocation(), FColor::Red, false, 0.5f, 0, 1.5f);
	}
}

float ATestBaseUnit::TakeDamage(
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

void ATestBaseUnit::HandleDeath(AController* Killer, AActor* DamageCauser)
{
	if (bDead) return;
	bDead = true;

	// 1) 예약/슬롯 정리(핵심)
	UnregisterFromBattleDirector(true);

	// 2) AI/이동 정지
	if (ATestBaseAIController* AIC = Cast<ATestBaseAIController>(GetController()))
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
					Pool->Release<ATestBaseUnit>(MyPoolType, this);
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
							Pool->Release<ATestBaseUnit>(MyPoolType, this);
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

// --------------------
// Pool interface
// --------------------

void ATestBaseUnit::OnGet_Implementation()
{
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
}

void ATestBaseUnit::OnRelease_Implementation()
{
	// 풀로 반환될 때마다 서브시스템 해제
	UnregisterFromBattleDirector(true);

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

	Brain->ResetRuntimeBrainState();

	OwnerSpawner = nullptr;
}
