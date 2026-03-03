// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Actor/Wall/CoreWallActor.h"
#include "AbilitySystemComponent.h"
#include "GameSystem/Instance/World/BattleDirectorSubsystem.h"
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

	// GAS 기본 초기화(필요한 프로젝트면 활성화)
	 ASC->InitAbilityActorInfo(this, this);

	if (UWorld* World = GetWorld())
	{
		if (UBattleDirectorSubsystem* Subsys = World->GetSubsystem<UBattleDirectorSubsystem>())
		{
			Subsys->RegisterWallCore(Team, this);
			UE_LOG(LogTemp, Warning, TEXT("[WallCore] RegisterWallCore Team=%d Core=%s"), (int32)Team, *GetName());
		}
	}

	if (ASC)
	{
		const UStatAttributeSet* WallSet = ASC->GetSet<UStatAttributeSet>();
		if (WallSet)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(
				WallSet->GetHealthAttribute()
			).AddLambda([this](const FOnAttributeChangeData& Data)
				{
					UE_LOG(LogTemp, Log,
						TEXT("[WallCore:%s] HP Changed! Old=%.2f New=%.2f"),
						*GetName(),
						Data.OldValue,
						Data.NewValue);

					if (Data.NewValue <= 0.f)
					{
						UE_LOG(LogTemp, Log,
							TEXT("[WallCore:%s] DESTROYED!"),
							*GetName());
					}
				});
		}
	}
}
