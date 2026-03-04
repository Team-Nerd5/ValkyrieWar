// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/Range/ValkyrieBow.h"
#include "Object/Weapon/Range/Projectile/ArrowProjectile.h"

void AValkyrieBow::ExecuteWeaponAction(AActor* InOwner, TSubclassOf<AActor> ProjectileClass)
{
	if (!InOwner || !ArrowClass) return;

	// 캐릭터 앞 100 지점
	FVector SpawnLocation = InOwner->GetActorLocation() + (InOwner->GetActorForwardVector() * 100.0f) + FVector(0.f, 0.f, 20.f);
	FRotator SpawnRotation = InOwner->GetActorRotation();

	FActorSpawnParameters Params;
	Params.Owner = InOwner;
	Params.Instigator = Cast<APawn>(InOwner);

	GetWorld()->SpawnActor<AActor>(ArrowClass, SpawnLocation, SpawnRotation, Params);
	UE_LOG(LogTemp, Warning, TEXT("🏹 [활] 화살 발사!!"));
}
