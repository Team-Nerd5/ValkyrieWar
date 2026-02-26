// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/Range/GA_BowAttack.h"
#include "Object/Weapon/Range/Projectile/ArrowProjectile.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

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
	AValkyrieCharacter* ValChar = Cast<AValkyrieCharacter>(GetAvatarActorFromActorInfo());
	if (!ValChar || !ProjectileClass){return;}

	// 가까운 적 찾기
	FVector MyLocation = ValChar->GetActorLocation();
	AActor* ClosestEnemy = nullptr;
	float MinDistance = AutoAimRadius;

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACharacter::StaticClass(), FoundActors);

	for (AActor* Enemy : FoundActors)
	{
		if (Enemy == ValChar) continue;

		float Dist = FVector::Dist(MyLocation, Enemy->GetActorLocation());
		if (Dist < MinDistance)
		{
			MinDistance = Dist;
			ClosestEnemy = Enemy;
		}
	}
	FRotator SpawnRot = ValChar->GetActorForwardVector().Rotation();
	if (ClosestEnemy)
	{
		FVector Dir = ClosestEnemy->GetActorLocation() - MyLocation;
		Dir.Z = 0.0f;
		SpawnRot = Dir.Rotation();
		ValChar->SetActorRotation(SpawnRot);
	}

	// 발샷~!
	FVector SpawnPos = MyLocation + (ValChar->GetActorForwardVector() * 100.0f);
	

	USkeletalMeshComponent* CharMesh = ValChar->GetMesh();
	if (CharMesh && CharMesh->DoesSocketExist(FName("ArrowSocket")))
	{
		SpawnPos = CharMesh->GetSocketLocation(FName("ArrowSocket"));
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = ValChar;

	FTransform SpawnTransform(SpawnRot, SpawnPos);
	GetWorld()->SpawnActor<AArrowProjectile>(ProjectileClass, SpawnTransform, SpawnParams);
}
