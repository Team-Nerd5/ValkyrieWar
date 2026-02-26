// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/Range/Projectile/ArrowProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AArrowProjectile::AArrowProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	USceneComponent* RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootScene;
	// 스태틱매쉬 부착
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	// 화살 날리기 기초 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	ProjectileMovement->UpdatedComponent = RootComponent;
	//속도 설정
	ProjectileMovement->InitialSpeed = 5000.0f;
	ProjectileMovement->MaxSpeed = 5000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true; //날라가는 방향으로 화살 머리꺽기?? 이게 필요한가 일단 저장
	ProjectileMovement->ProjectileGravityScale = 0.0f;//투사체에 적용 되는 중력 설정인데 설정해줄까 일단은 고민중

	InitialLifeSpan = 0.0f; // 안맞고 날라갔을 때 수명

	UE_LOG(LogTemp, Warning, TEXT("간다아아앗!"));
}
void AArrowProjectile::BeginPlay()
{
	Super::BeginPlay();
	// 실제 게임 세상에 배치됐을 때 위치 찍어보기
	UE_LOG(LogTemp, Warning, TEXT("🚀 [화살] BeginPlay 실행됨! 좌표: %s"), *GetActorLocation().ToString());
}


