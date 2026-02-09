// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Controller/CameraBoundsVolume.h"
#include "Components/BoxComponent.h"

// Sets default values
ACameraBoundsVolume::ACameraBoundsVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	// 박스 컴포넌트 생성 및 루트로 설정
	BoundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundsBox"));
	RootComponent = BoundsBox;

	BoundsBox->SetLineThickness(2.0f);
	BoundsBox->ShapeColor = FColor::Red;

	BoundsBox->SetCollisionProfileName(TEXT("NoCollision"));
}

void ACameraBoundsVolume::BeginPlay()
{
	Super::BeginPlay();
}

