// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class LOADINGSCREENMODULE_API SLoadingScreen : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SLoadingScreen)
		: _BackgroundTexture(nullptr)  // 기본 생성자가 텍스처를 null로 초기화
		{
		}
		SLATE_ARGUMENT(UTexture2D*, BackgroundTexture)  // 위젯을 위한 텍스처 매개변수 선언
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	// 배경 텍스처 에셋에 대한 참조를 보유
	UTexture2D* BackgroundTexture;

	// 배경 렌더링에 사용되는 Slate 브러시를 보유
	TSharedPtr<FSlateBrush> BackgroundBrush;
};
