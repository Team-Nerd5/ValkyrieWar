// Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Actor/Wall/CoreWallActor.h"
#include "AbilitySystemComponent.h"
#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "Data/Attribute/StatAttributeSet.h"

ACoreWallActor::ACoreWallActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	StatAttribute = CreateDefaultSubobject<UStatAttributeSet>(TEXT("WallStat"));
}

UAbilitySystemComponent* ACoreWallActor::GetAbilitySystemComponent() const
{
	return ASC;
}

void ACoreWallActor::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		if (UBattleDirectorSubsystem* Subsys = World->GetSubsystem<UBattleDirectorSubsystem>())
		{
			Subsys->RegisterWallCore(Team, this);

			UE_LOG(LogTemp, Warning,
				TEXT("[WallCore] RegisterWallCore Team=%d Core=%s"),
				(int32)Team,
				*GetName());
		}
	}

	// Attribute 이벤트 바인딩
	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);

		const UStatAttributeSet* WallSet = ASC->GetSet<UStatAttributeSet>();

		if (WallSet)
		{
			// 초기 HP 브로드캐스트 (UI 초기화용)
			if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
			{
				const float CurHP = WallSet->GetHealth();
				const float MaxHP = FMath::Max(1.f, WallSet->GetMaxHealth());

				if (Team == ETeam::TeamA)
				{
					EventSystem->Battle.OnAllyWallHealthChanged.Broadcast(CurHP, MaxHP);
				}
				else
				{
					EventSystem->Battle.OnEnemyWallHealthChanged.Broadcast(CurHP, MaxHP);
				}
			}

			// HP 변경 이벤트
			ASC->GetGameplayAttributeValueChangeDelegate(
				WallSet->GetHealthAttribute()
			).AddLambda([this](const FOnAttributeChangeData& Data)
				{
					if (!ASC) return;

					const UStatAttributeSet* Set = ASC->GetSet<UStatAttributeSet>();
					if (!Set) return;

					const float CurHP = Data.NewValue;
					const float MaxHP = FMath::Max(1.f, Set->GetMaxHealth());

					UE_LOG(LogTemp, Log,
						TEXT("[WallCore:%s] HP Changed! Old=%.2f New=%.2f Max=%.2f"),
						*GetName(),
						Data.OldValue,
						Data.NewValue,
						MaxHP);

					// WorldEventSystem 이벤트 브로드캐스트
					if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
					{
						if (Team == ETeam::TeamA)
						{
							EventSystem->Battle.OnAllyWallHealthChanged.Broadcast(CurHP, MaxHP);
						}
						else
						{
							EventSystem->Battle.OnEnemyWallHealthChanged.Broadcast(CurHP, MaxHP);
						}
					}

					// 파괴 로그
					if (CurHP <= 0.f)
					{
						UE_LOG(LogTemp, Warning,
							TEXT("[WallCore:%s] DESTROYED!"),
							*GetName());
					}
				});
		}
	}
}
