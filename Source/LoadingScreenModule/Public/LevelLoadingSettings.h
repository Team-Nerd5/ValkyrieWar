// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LevelLoadingSettings.generated.h"

// 프로젝트 세팅에서 로딩 화면 설정을 가능하게 해줌
UCLASS(Config = "Game", DefaultConfig, meta = (DisplayName = "Loading Screen"))
class LOADINGSCREENMODULE_API ULevelLoadingSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
    // 로딩 화면을 표시해야 하는 맵 에셋에 대한 소프트 참조 배열
    // AllowedClasses = "World" - 월드 에셋만 선택 가능
    UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (AllowedClasses = "World"))
    TArray<FSoftObjectPath> MapsWithLoadingScreens;

    // 로딩 화면의 배경 이미지에 대한 소프트 참조
    // AllowedClasses = "Texture" - 텍스처 에셋만 선택 가능
    UPROPERTY(Config, EditAnywhere, Category = "Loading Screen", meta = (AllowedClasses = "Texture"))
    FSoftObjectPath BackgroundImage;

    // 로딩 화면이 표시될 최소 지속 시간 (초 단위)
    UPROPERTY(Config, EditAnywhere, Category = "Loading Screen")
    float MinimumLoadingScreenDisplayTime = 2.0f;

};
