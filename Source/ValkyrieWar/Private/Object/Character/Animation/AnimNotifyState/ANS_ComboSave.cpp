// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Animation/AnimNotifyState/ANS_ComboSave.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

void UANS_ComboSave::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (AValkyrieCharacter* Character = Cast<AValkyrieCharacter>(MeshComp->GetOwner()))
		{
			Character->SetComboEnable(true);
		}
	}
}

void UANS_ComboSave::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (AValkyrieCharacter* Character = Cast<AValkyrieCharacter>(MeshComp->GetOwner()))
	{
		Character->SetComboEnable(false);
	}
}
