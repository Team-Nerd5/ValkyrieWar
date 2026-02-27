// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/Range/GA_BowAttack.h"
#include "Object/Weapon/Range/Projectile/ArrowProjectile.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

#include "GameFramework/Character.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

UGA_BowAttack::UGA_BowAttack()
{
	// 이벤트 받을 때만 실행
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_BowAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}
