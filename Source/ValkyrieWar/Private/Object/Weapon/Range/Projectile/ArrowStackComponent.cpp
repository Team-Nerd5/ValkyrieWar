// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/Range/Projectile/ArrowStackComponent.h"
#include "Object/Weapon/Range/Projectile/ArrowProjectile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UArrowStackComponent::UArrowStackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UArrowStackComponent::AddArrow(AArrowProjectile* InArrow)
{
	if (InArrow) StackingArrows.Add(InArrow);
}

void UArrowStackComponent::PullIt(float DamagePerArrow)
{
	if (StackingArrows.Num() <= 0) return;
	float TotalDamage = StackingArrows.Num() * DamagePerArrow;
	for (AActor* Arrow : StackingArrows)
	{
		if (Arrow)
		{
			Arrow->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform); // 화살분리
			UPrimitiveComponent* RootPrim = Cast<UPrimitiveComponent>(Arrow->GetRootComponent());
			if (RootPrim)
			{
				RootPrim->SetCollisionProfileName(TEXT("PhysicsActor"));

				RootPrim->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
				RootPrim->SetSimulatePhysics(true);

				RootPrim->SetCollisionResponseToAllChannels(ECR_Ignore);
				RootPrim->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

				RootPrim->WakeAllRigidBodies();

				FVector PullDirection = (Arrow->GetActorForwardVector() * -1.0f) + FVector(0.0f, 0.0f, 0.5f);
				PullDirection.Normalize();

				RootPrim->AddImpulse(PullDirection * PullForce, NAME_None, true);
			}
			Arrow->SetLifeSpan(0.2f);
		}
	}
	StackingArrows.Empty();

	AActor* Owner = GetOwner();
	if (Owner)
	{
		UGameplayStatics::ApplyDamage(Owner, TotalDamage, nullptr, this->GetOwner(), nullptr);
		UE_LOG(LogTemp, Warning, TEXT("촥! 화살 %d개 뽑힘! 후드득 떨어짐!"), StackingArrows.Num());
	}
}

void UArrowStackComponent::ClearAllArrows()
{
	for (AArrowProjectile* Arrow : StackingArrows)
	{
		if (IsValid(Arrow)) Arrow->Destroy();
	}
	StackingArrows.Empty();
}
