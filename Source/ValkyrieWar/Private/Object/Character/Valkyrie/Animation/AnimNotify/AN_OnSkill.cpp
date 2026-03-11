// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/Animation/AnimNotify/AN_OnSkill.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

void UAN_OnSkill::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	UE_LOG(LogTemp, Warning, TEXT("🔔 [OnSkillNotify] 노티파이 정상 작동!! 방아쇠 당김!!"));
	if (MeshComp && MeshComp->GetOwner())
	{
		if (AValkyrieCharacter* ValkyrieChar = Cast<AValkyrieCharacter>(MeshComp->GetOwner()))
		{
			ValkyrieChar->OnSkillNotify();
		}
	}
}

