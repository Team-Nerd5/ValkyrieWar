// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Game/SkillEffectData.h"
#include "SkillEffectModule.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSkillEffects
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<TObjectPtr<USkillEffectData>> Effects;
};

UCLASS()
class VALKYRIEWAR_API USkillEffectModule : public UBaseModule
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGameManager* InGameManager) override;

	TArray<USkillEffectData*> GetEffects(int32 InGroupId);
protected:
	virtual void MakeData() override;

private:
	UPROPERTY()
	TMap<int32, FSkillEffects> EffectsByGroupId;
};
