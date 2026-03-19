// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/Animation/AnimNotify/AN_SetPlayRate.h"

UAN_SetPlayRate::UAN_SetPlayRate()
{
	TargetPlayRate = 1.0f;
}

void UAN_SetPlayRate::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetAnimInstance())
	{
		UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();
		UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();

		if (CurrentMontage)
		{
			AnimInstance->Montage_SetPlayRate(CurrentMontage, TargetPlayRate);
		}
	}
}
