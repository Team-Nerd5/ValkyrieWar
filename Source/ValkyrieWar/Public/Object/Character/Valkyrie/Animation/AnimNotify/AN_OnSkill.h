// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_OnSkill.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UAN_OnSkill : public UAnimNotify
{
	GENERATED_BODY()
	void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference);
};
