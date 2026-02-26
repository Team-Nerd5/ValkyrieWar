// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Animation/AnimNotifyState/ANS_ComboWindow.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
	

void UANS_ComboWindow::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	// *** MODIFIED: AYourCharacter를 형의 실제 캐릭터 클래스 이름으로 바꿔야 해! ***
	if (AValkyrieCharacter* Character = Cast<AValkyrieCharacter>(MeshComp->GetOwner()))
	{
		if (Character->bIsAttackSaved)
		{
			Character->ExecuteCombo(NextSectionName);
		}
	}
}
