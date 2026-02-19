// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Loading/LoadingWidget.h"
#include "GameSystem/Instance/Game/LevelManager.h"
#include "Components/ProgressBar.h"

void ULoadingWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ProgressBar)
	{
		// GameInstance를 통해 LevelManager 서브시스템을 찾아 진척도를 가져옴
		if (UGameInstance* GI = GetGameInstance())
		{
			if (ULevelManager* LevelManager = GI->GetSubsystem<ULevelManager>())
			{
				// 서브시스템에서 계산한 진척도(0.0 ~ 1.0)를 프로그레스 바에 적용
				ProgressBar->SetPercent(LevelManager->GetLoadingProgress());
			}
		}
	}
}
