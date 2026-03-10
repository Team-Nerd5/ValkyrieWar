// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/Range/Projectile/ArrowProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AArrowProjectile::AArrowProjectile()
{
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	CollisionComp->OnComponentHit.AddDynamic(this, &AArrowProjectile::OnProjectileHit);
	RootComponent = CollisionComp;

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

	InitialLifeSpan = 3.0f; // 수명

}

void AArrowProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		// UGameplayStatics::ApplyDamage(OtherActor, 10.f, ...);
		CollisionComp->SetSimulatePhysics(false);
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ProjectileMovement->StopMovementImmediately();
		ProjectileMovement->Deactivate();

		FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, true);
		FVector StickLocation = Hit.Location + (GetActorForwardVector() * 10.f);
		SetActorLocation(StickLocation);
		AttachToComponent(OtherComp, AttachmentRules, Hit.BoneName);

		SetLifeSpan(10.0f);
	}

}


