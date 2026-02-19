// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ValkyrieANS_LockRotation.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieANS_LockRotation : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UValkyrieANS_LockRotation();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)override;

private:
	float CachedGroundFriction;
	float CachedBrakingDeceleration;
};
