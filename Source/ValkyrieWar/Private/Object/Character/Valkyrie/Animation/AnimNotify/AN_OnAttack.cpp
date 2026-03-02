// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/Animation/AnimNotify/AN_OnAttack.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

void UAN_OnAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (ABaseCharacter* Character = Cast<ABaseCharacter>(MeshComp->GetOwner()))
		{
			Character->OnAttackNotify();
		}
	}
}
