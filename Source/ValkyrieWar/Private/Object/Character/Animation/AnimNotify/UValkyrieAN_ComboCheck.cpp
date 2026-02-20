// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Animation/AnimNotify/UValkyrieAN_ComboCheck.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

void UUValkyrieAN_ComboCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}
	AActor* Owner = MeshComp->GetOwner();
	AValkyrieCharacter* Character = Cast<AValkyrieCharacter>(Owner);
	if (Character)
	{
		Character->ContinueCombo();
	}
}
