// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "StageModule.generated.h"

class UStageData;

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UStageModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

protected:
	virtual void MakeData() override;
private:
	UPROPERTY()
	TMap<int32, TObjectPtr<UStageData>> StageDataByChapter;
};
