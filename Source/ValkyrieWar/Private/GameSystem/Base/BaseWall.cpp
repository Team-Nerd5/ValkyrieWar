#include "GameSystem/Base/BaseWall.h"
#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
#include "Object/Actor/Wall/CoreWallActor.h"
#include "AbilitySystemComponent.h"

ABaseWall::ABaseWall()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseWall::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		if (UBattleDirectorSubsystem* Subsys = World->GetSubsystem<UBattleDirectorSubsystem>())
		{
			Subsys->RegisterWallAnchor(Team, this);

			// 코어 캐싱 (없으면 나중에 GetAbilitySystemComponent에서 다시 찾아도 됨)
			CachedCore = Subsys->GetWallCore(Team);
		}
	}
}

void ABaseWall::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UBattleDirectorSubsystem* Subsys = World->GetSubsystem<UBattleDirectorSubsystem>())
		{
			Subsys->UnregisterWallAnchor(Team, this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

UAbilitySystemComponent* ABaseWall::GetAbilitySystemComponent() const
{
	// 캐시된 코어 우선
	if (CachedCore.IsValid())
	{
		return CachedCore->GetAbilitySystemComponent();
	}
	// 캐시가 비었으면 서브시스템에서 재조회(레벨 배치 순서 문제 대비)
	if (UWorld* World = GetWorld())
	{
		if (UBattleDirectorSubsystem* Subsys = World->GetSubsystem<UBattleDirectorSubsystem>())
		{
			if (ACoreWallActor* Core = Subsys->GetWallCore(Team)) return Core->GetAbilitySystemComponent();
		}
	}

	// 코어가 없으면 GAS 타겟으로 동작 불가
	return nullptr;
}
