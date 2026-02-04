// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"

#include "Data/Event/WidgetEventData.h"
#include "Data/Event/ValkyrieEventData.h"
#include "Data/Event/UnitEventData.h"
#include "Data/Event/LoginEventData.h"

#include "WorldEventSystem.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UWorldEventSystem : public UWorldSubsystem
{
	GENERATED_BODY()
public:
	FWidgetEventData Widget;
	FValkyrieEventData Valkyrie;
	FUnitEventData Unit;
	FLoginEventData Login;
};
