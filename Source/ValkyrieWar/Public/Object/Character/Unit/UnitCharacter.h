// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "Data/Game/UnitData.h"
#include "UnitCharacter.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API AUnitCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;

	//캐릭터 생성 시 데이터 세팅
	void SetData(UUnitData* InData);

protected:
	UPROPERTY()
	TObjectPtr<UUnitData> Data = nullptr;
};
