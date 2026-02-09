// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "CameraBoundsVolume.generated.h"

UCLASS()
class VALKYRIEWAR_API ACameraBoundsVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACameraBoundsVolume();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bounds")
	UBoxComponent* BoundsBox;

};
