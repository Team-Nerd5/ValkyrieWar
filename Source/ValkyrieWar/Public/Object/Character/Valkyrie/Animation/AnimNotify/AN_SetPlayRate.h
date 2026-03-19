// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SetPlayRate.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UAN_SetPlayRate : public UAnimNotify
{
	GENERATED_BODY()
public:
	UAN_SetPlayRate();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify")
	float TargetPlayRate;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
