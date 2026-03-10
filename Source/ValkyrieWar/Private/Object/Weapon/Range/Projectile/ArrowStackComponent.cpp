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

	UGameplayStatics::ApplyDamage(GetOwner(), TotalDamage, nullptr, nullptr, UDamageType::StaticClass());

	ClearAllArrows();

	UE_LOG(LogTemp, Warning, TEXT("[%s] 유닛에게 민트 화살 폭발! 대미지: %f"), *GetOwner()->GetName(), TotalDamage);
}

void UArrowStackComponent::ClearAllArrows()
{
	for (AArrowProjectile* Arrow : StackingArrows)
	{
		if (IsValid(Arrow)) Arrow->Destroy();
	}
	StackingArrows.Empty();
}
