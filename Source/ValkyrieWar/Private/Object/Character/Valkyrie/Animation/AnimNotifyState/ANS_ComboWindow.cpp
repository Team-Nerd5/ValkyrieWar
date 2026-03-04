// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/Animation/AnimNotifyState/ANS_ComboWindow.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
	

void UANS_ComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AValkyrieCharacter* Character = Cast<AValkyrieCharacter>(MeshComp->GetOwner()))
		{
			Character->BeginComboWindow();
		}
	}
}

void UANS_ComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AValkyrieCharacter* Character = Cast<AValkyrieCharacter>(MeshComp->GetOwner()))
		{
			Character->EndComboWindow(NextSectionName);
		}
	}
}
