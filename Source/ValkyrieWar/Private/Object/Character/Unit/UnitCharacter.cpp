// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Unit/UnitCharacter.h"

void AUnitCharacter::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    //하위 파츠 세팅
    USkeletalMeshComponent* LeaderMesh = GetMesh();

    TArray<USkeletalMeshComponent*> ModularParts;
    GetComponents<USkeletalMeshComponent>(ModularParts);

    for (USkeletalMeshComponent* Part : ModularParts)
    {
        if (Part == LeaderMesh) continue;

        if (Part->ComponentHasTag(TEXT("Weapon"))) continue;

        Part->SetLeaderPoseComponent(LeaderMesh);
    }
}

void AUnitCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    
}
