// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Animation/AnimNotifyState/ValkyrieANS_LockRotation.h"
#include "GameFramework/Character.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UValkyrieANS_LockRotation::UValkyrieANS_LockRotation()
{
	NotifyColor = FColor(153, 50, 255, 255); // 있어서 한번 써봄 ㅋㅋ
}

void UValkyrieANS_LockRotation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		AValkyrieCharacter* Character = Cast<AValkyrieCharacter>(MeshComp->GetOwner());

		if (Character && Character->GetCharacterMovement())
		{
			UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();

			Character->bIsPivotLocked = true;
			MoveComp->bOrientRotationToMovement = false;

			CachedGroundFriction = MoveComp->GroundFriction;
			CachedBrakingDeceleration = MoveComp->BrakingDecelerationWalking;

			MoveComp->GroundFriction = 0.0f;
			MoveComp->BrakingDecelerationWalking = 300.0f;
		}
	}
}

void UValkyrieANS_LockRotation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		AValkyrieCharacter* Character = Cast<AValkyrieCharacter>(MeshComp->GetOwner());

		if (Character && Character->GetCharacterMovement())
		{
			UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement();
			FVector CurrentInputDirection = MoveComp->GetCurrentAcceleration();

			if (!CurrentInputDirection.IsNearlyZero())
			{
				Character->SetActorRotation(CurrentInputDirection.Rotation());
			}

			Character->bIsPivotLocked = false;
			MoveComp->bOrientRotationToMovement = true;

			MoveComp->GroundFriction = CachedGroundFriction;
			MoveComp->BrakingDecelerationWalking = CachedBrakingDeceleration;

		}
	}
}
