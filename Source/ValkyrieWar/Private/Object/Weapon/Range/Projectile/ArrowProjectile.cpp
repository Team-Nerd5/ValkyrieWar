// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/Range/Projectile/ArrowProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AArrowProjectile::AArrowProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// 스태틱매쉬 부착
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	RootComponent = ProjectileMesh;
	// 화살 날리기 기초 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	//속도 설정
	ProjectileMovement->InitialSpeed = 5000.0f;
	ProjectileMovement->MaxSpeed = 5000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true; //날라가는 방향으로 화살 머리꺽기?? 이게 필요한가 일단 저장
	ProjectileMovement->ProjectileGravityScale = 0.0f;//투사체에 적용 되는 중력 설정인데 설정해줄까 일단은 고민중

	InitialLifeSpan = 3.0f; // 안맞고 날라갔을 때 수명
}


