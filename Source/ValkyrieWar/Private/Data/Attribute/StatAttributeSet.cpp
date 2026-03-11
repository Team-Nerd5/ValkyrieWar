// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Attribute/StatAttributeSet.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "Object/Actor/Wall/CoreWallActor.h"
#include "GameSystem/Library/GameBaseLibrary.h"

UStatAttributeSet::UStatAttributeSet()
{
	InitAttack(100.0f);
	InitDefense(10.0f);
	InitHealth(1000.0f);
	InitMaxHealth(1000.0f);
	InitMoveSpeed(500.0f);
}

void UStatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UStatAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMoveSpeedAttribute())
	{
		//값 변경 될 때
	}
	if (Attribute == GetHealthAttribute())
	{
		AActor* TargetActor = GetOwningActor();
		//데미지 표기 / UI 표기 등...
		if (GetHealth() <= 0.0f)
		{
			//테스트 제거
			UE_LOG(LogTemp, Log, TEXT("%s is dead"), *TargetActor->GetName());

			// 성벽과 유닛에 동일한 어트리뷰트를 사용.
			if (ACoreWallActor* Wall = Cast<ACoreWallActor>(TargetActor)) // 성벽 파괴 시
			{
				if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
				{
					if (Wall->GetTeam() == ETeamType::Ally)
					{
						EventSystem->Battle.OnBattleStateChanged.Broadcast(EBattleState::Defeat);
					}
					else
					{
						EventSystem->Battle.OnBattleStateChanged.Broadcast(EBattleState::Win);
					}
				}
			}
			else if (ABaseCharacter* Character = Cast<ABaseCharacter>(TargetActor)) // 유닛 사망 시
			{
				Character->OnDeath();
			}			
		}
	}
}

void UStatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
