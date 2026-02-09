// Fill out your copyright notice in the Description page of Project Settings.


#include "Test/SDCH/TestUnit/TestCharacters/TestBaseUnit.h"
#include "Test/SDCH/TestUnit/TestComponents/TestUnitBrainComponent.h"
#include "Test/SDCH/TestGameSystem/TestInstance/TestWorld/TestBattleDirectorSubsystem.h"
#include "Components/CapsuleComponent.h"
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

void ATestBaseUnit::BeginPlay()
{
    Super::BeginPlay();

    CurrentHP = MaxHP;
    bDead = false;

    // Auto 타깃팅: 근/원거리 타입에 따라 기본 룰셋 부여
    if (Brain && Brain->TargetingPolicy == ETargetingPolicy::Auto)
    {
        // 현재 요구사항: 근거리/원거리 모두 "성벽에서 가까운 적 > 적기지"
        Brain->TargetingPolicy = ETargetingPolicy::NearWallEnemyThenBase;
    }

    EngagementSlots.SetNum(FMath::Clamp(MaxEngagementSlots, 1, 3));
}

void ATestBaseUnit::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    EngagementSlots.Empty();
    Super::EndPlay(EndPlayReason);
}

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
    //if (!CanAttackNow(Now)) return false;

    const float Dist = FVector::Dist(GetActorLocation(), Target->GetActorLocation());
    if (Dist > AttackRange) return false;

    // 공격 발동 기록
    LastAttackTime = Now;

    // ApplyDamage -> Target의 TakeDamage로 들어감
    AController* InstigatorCtrl = GetController();
    UGameplayStatics::ApplyDamage(Target, AttackDamage, InstigatorCtrl, this, UDamageType::StaticClass());

    // (선택) 디버그
    if (bDrawDebug)
    {
        DrawDebugLine(World, GetActorLocation(), Target->GetActorLocation(), FColor::Red, false, 0.1f, 0, 1.5f);
    }

    return true;
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
    if (UWorld* World = GetWorld())
    {
        if (UTestBattleDirectorSubsystem* Subsys = World->GetSubsystem<UTestBattleDirectorSubsystem>())
        {
            // 내가 타깃으로서 수용하고 있던 공격자들을 모두 해제
            Subsys->ReleaseAllAttackersOfTarget(this);

            // 내가 공격자로서 누군가를 예약하고 있었다면 Unregister 과정에서 해제되도록 처리
            Subsys->UnregisterUnit(this);
        }
    }

    // 2) AI/이동 정지
    if (AAIController* AIC = Cast<AAIController>(GetController()))
    {
        AIC->StopMovement();
        AIC->UnPossess();
    }
    GetCharacterMovement()->StopMovementImmediately();
    GetCharacterMovement()->DisableMovement();

    // 3) 충돌/피격 방지
    if (UCapsuleComponent* Cap = GetCapsuleComponent())
    {
        Cap->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }
    // Mesh는 시각적으로 남겨두되, 필요하면 충돌 끄기
    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    }

    // 4) 사망 연출(지금은 간단히, 추후 몽타주/랙돌로 확장)
    // MeshComp->SetSimulatePhysics(true);

    // 5) 일정 시간 후 제거
    if (DestroyDelay <= 0.f)
    {
        Destroy();
        return;
    }

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            DestroyTimerHandle,
            this,
            &AActor::K2_DestroyActor,
            DestroyDelay,
            false
        );
    }
}
