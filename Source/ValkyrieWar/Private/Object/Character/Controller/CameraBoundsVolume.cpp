// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Controller/CameraBoundsVolume.h"
ACameraBoundsVolume::ACameraBoundsVolume()
{
	PrimaryActorTick.bCanEverTick = false; // 가만히 있는 놈이라 틱 필요 없음

	BoundsBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BoundsBox"));
	RootComponent = BoundsBox;

	// 게임 중에는 안 부딪히게 설정
	BoundsBox->SetCollisionProfileName(TEXT("NoCollision"));

	// 에디터에서는 보여줘
	BoundsBox->SetHiddenInGame(true);
}
