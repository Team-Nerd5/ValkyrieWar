// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Animation/AnimNotify/AN_SendGamePlayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAN_SendGamePlayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		FGameplayEventData Payload;
		Payload.Instigator = MeshComp->GetOwner();

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, Payload);
	}

}
