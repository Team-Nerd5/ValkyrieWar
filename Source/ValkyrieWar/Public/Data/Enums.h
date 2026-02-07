// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EUIType : uint8
{
	//HUD
	Login,
	Lobby,
	Battle,

	//Popup
	
};

UENUM(BlueprintType)
enum class EMapType : uint8
{
	Login,
	Lobby,
	Battle,
	Loading,
};

UENUM(BlueprintType)
enum  class EInputControlMode : uint8
{
	Manual UMETA(DisplayName = "Manual Mode"),
	Auto UMETA(DisplayName = "Auto Mode")
};
