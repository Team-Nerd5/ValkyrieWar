// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Unit/UnitCharacter.h"

void AUnitCharacter::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    //하위 파츠 세팅
    USkeletalMeshComponent* LeaderMesh = GetMesh();

    TArray<USkeletalMeshComponent*> ModularParts;
    GetComponents<USkeletalMeshComponent>(ModularParts);

    for (USkeletalMeshComponent* Part : ModularParts)
    {
        if (Part == LeaderMesh) continue;

        Part->SetLeaderPoseComponent(LeaderMesh);
    }
}
