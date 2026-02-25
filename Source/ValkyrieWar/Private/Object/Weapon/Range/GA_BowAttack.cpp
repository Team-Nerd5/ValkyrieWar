// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/Range/GA_BowAttack.h"
#include "Object/Weapon/Range/Projectile/ArrowProjectile.h"
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

	ACharacter* Char = Cast<ACharacter>(ActorInfo->AvatarActor.Get());
	if (!Char || !ProjectileClass)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	//가까운 적 찾기
	FVector MyLocation = Char->GetActorLocation();
	AActor* ClosestEnemy = nullptr;
	float MinDistance = AutoAimRadius;

	// 싹싹뒤지기
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), FoundActors);

	for (AActor* Enemy : FoundActors)
	{
		if (Enemy == Char) continue;

		float Dist = FVector::Dist(MyLocation, Enemy->GetActorLocation());
		if(Dist < MinDistance)
		{
			MinDistance = Dist;
			ClosestEnemy = Enemy;
		}
	}

	if (ClosestEnemy)
	{
		FVector Dir = ClosestEnemy->GetActorLocation() - MyLocation;
		Dir.Z = 0.0f;
		Char->SetActorRotation(Dir.Rotation());
	}

	// 발샷~!
	FVector SpawnPos = MyLocation + (Char->GetActorForwardVector() * 100.0f);
	GetWorld()->SpawnActor<AArrowProjectile>(ProjectileClass, SpawnPos, Char->GetActorRotation());

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
