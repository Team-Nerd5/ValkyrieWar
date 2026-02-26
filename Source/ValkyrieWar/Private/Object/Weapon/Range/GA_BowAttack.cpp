// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/Range/GA_BowAttack.h"
#include "Object/Weapon/Range/Projectile/ArrowProjectile.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UGA_BowAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	UGameplayAbility::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbility(Handle, ActorInfo, ActivationInfo);
}

void UGA_BowAttack::FireArrow()
{
	ACharacter* Char = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!Char || !ProjectileClass){return;}

	// 가까운 적 찾기
	FVector MyLocation = Char->GetActorLocation();
	AActor* ClosestEnemy = nullptr;
	float MinDistance = AutoAimRadius;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), FoundActors);

	for (AActor* Enemy : FoundActors)
	{
		if (Enemy == Char) continue;

		float Dist = FVector::Dist(MyLocation, Enemy->GetActorLocation());
		if (Dist < MinDistance)
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
}
