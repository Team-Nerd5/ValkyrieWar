// Fill out your copyright notice in the Description page of Project Settings.


#include "SLoadingScreen.h"
#include "SlateOptMacros.h"
#include "SlateExtras.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SLoadingScreen::Construct(const FArguments& InArgs)
{
	// 인자로부터 텍스처 저장
	BackgroundTexture = InArgs._BackgroundTexture;

	// 배경 렌더링을 위한 브러시 생성
	BackgroundBrush = MakeShareable(new FSlateBrush());
	BackgroundBrush->SetResourceObject(BackgroundTexture);

	// 위젯 계층 구조 생성 시작
	// ChildSlot은 우리가 구성하는 위젯의 루트 슬롯임
	ChildSlot
		[
			// 여러 요소를 쌓을 수 있는 오버레이 위젯
			SNew(SOverlay)
				// 배경 이미지를 위한 첫 번째 슬롯을 오버레이에 추가
				+ SOverlay::Slot()
				.HAlign(HAlign_Fill)  // 공간을 채우도록 가로로 늘림
				.VAlign(VAlign_Fill)  // 공간을 채우도록 세로로 늘림
				[
					// 배경 브러시를 사용하여 이미지 위젯 생성
					SNew(SImage)
						.Image(BackgroundTexture ? BackgroundBrush.Get() : nullptr)
						.ColorAndOpacity(FLinearColor::White)
				]

				// 로딩 인디케이터를 위한 두 번째 슬롯을 오버레이에 추가
				+ SOverlay::Slot()
				.HAlign(HAlign_Center)	// 중간 정렬
				.VAlign(VAlign_Center)	// 중간 정렬
				.Padding(16.0f)         // 16 단위의 패딩 추가
				[
					// 원형 스로버(로딩 스피너) 위젯 생성 - 더 원형 모양을 위해 SCircularThrobber 사용
					SNew(SCircularThrobber)
						.Visibility(EVisibility::HitTestInvisible)    // 보이지만 클릭을 차단하지 않음
						.Radius(50.0f)           // 원의 반지름 설정
						.Period(1.0f)            // 애니메이션 주기 (초)
						.NumPieces(8)            // 회전 조각 수
				]
				
		];
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
