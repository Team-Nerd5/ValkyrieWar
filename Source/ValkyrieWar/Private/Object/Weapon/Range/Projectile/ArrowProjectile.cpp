// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/Range/Projectile/ArrowProjectile.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PointLightComponent.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"


// Sets default values
AArrowProjectile::AArrowProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);

	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Overlap);

	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	RootComponent = CollisionComp;

	// 스태틱매쉬 부착
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileMesh->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	// 화살 날리기 기초 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	//속도 설정
	ProjectileMovement->InitialSpeed = 5000.0f;
	ProjectileMovement->MaxSpeed = 5000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true; //날라가는 방향으로 화살 머리꺽기?? 이게 필요한가 일단 저장
	ProjectileMovement->ProjectileGravityScale = 0.0f;//투사체에 적용 되는 중력 설정인데 설정해줄까 일단은 고민중

	TrailNiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailNiagaraComp"));
	TrailNiagaraComp->SetupAttachment(ProjectileMesh); // 화살메쉬에 부착
	TrailNiagaraComp->SetAutoActivate(true); //테어나자마자

	InitialLifeSpan = 3.0f; // 수명
}

void AArrowProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (CollisionComp)
	{
		CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AArrowProjectile::OnOverlap);
	}
}

void AArrowProjectile::Destroyed()
{
	Super::Destroyed();
}

void AArrowProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString ActorName = OtherActor ? OtherActor->GetName() : TEXT("Unknown");
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner()) return;

	ProjectileMovement->StopMovementImmediately();
	ProjectileMovement->Deactivate();

	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FName TargetBone = SweepResult.BoneName;
	if (TargetBone == NAME_None && OtherComp)
	{
		FVector OutClosestPoint;
		// 뼈 이름이 없으면 메쉬의 소켓이나 기본 위치라도 잡기 위해 시도
		OtherComp->GetClosestPointOnCollision(GetActorLocation(), OutClosestPoint, NAME_None);
	}

	FAttachmentTransformRules AttachRules(EAttachmentRule::KeepWorld, true);
	AttachToComponent(OtherComp, AttachRules, TargetBone);

	ProjectileMesh->SetHiddenInGame(false);
	if (UMaterialInstanceDynamic* DynMeaterial = ProjectileMesh->CreateAndSetMaterialInstanceDynamic(0))
	{
		DynMeaterial->SetScalarParameterValue(TEXT("EmissivePower"), 50.0f);
	}

	SetLifeSpan(8.0f);// 화살을 맞았을떈 수명 을 조금 더 늘려서 스킬발동에 원할함을 줌
}


