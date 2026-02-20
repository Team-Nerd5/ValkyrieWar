// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Animation/AnimNotify/ValkyrieAN_ResetComobo.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

void UValkyrieAN_ResetComobo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (AValkyrieCharacter* Char = Cast<AValkyrieCharacter>(MeshComp->GetOwner()))
		{
			Char->ResetCombo();
		}
	}
}
