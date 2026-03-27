// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Actor/Wall/ProjectileWallActor.h"

#include "Components/BoxComponent.h"

AProjectileWallActor::AProjectileWallActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	SetRootComponent(CollisionComponent);

	CollisionComponent->SetCollisionProfileName(TEXT("Pawn"));

	CollisionComponent->SetGenerateOverlapEvents(true);

	// 성벽 형태
	CollisionComponent->SetBoxExtent(FVector(20.0f, 1000.0f, 100.0f));
}
