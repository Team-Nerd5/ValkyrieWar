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
	if (AActor* OwnerActor = MeshComp->GetOwner())
	{
		if (AValkyrieCharacter* Char = Cast<AValkyrieCharacter>(OwnerActor))
		{
			if (UCharacterMovementComponent* MoveComp = Char->GetCharacterMovement())
			{
				MoveComp->bOrientRotationToMovement = true;
				Char->bIsPivotLocked = false;
			}
		}
	}
}

void UValkyrieANS_LockRotation::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (AValkyrieCharacter* Character = Cast<AValkyrieCharacter>(MeshComp->GetOwner()))
		{
			FRotator SmoothRot = FMath::RInterpTo(Character->GetActorRotation(), Character->TargetPivotRotation, FrameDeltaTime, 10.0f);

			Character->SetActorRotation(SmoothRot);
		}
		
	}
}
