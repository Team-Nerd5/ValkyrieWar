// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraBoundsVolume.generated.h"


class UBoxComponent;

UCLASS()
class VALKYRIEWAR_API ACameraBoundsVolume : public AActor
{
	GENERATED_BODY()
	
public:
	ACameraBoundsVolume();

	// 영역을 표시할 박스 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera Bounds")
	TObjectPtr<UBoxComponent> BoundsBox = nullptr;

protected:
	virtual void BeginPlay() override;

public:
	// 박스 크기 가져오는 함수 (컨트롤러에서 쓸 거임)
	UBoxComponent* GetBoundsBox() const { return BoundsBox; }

};
