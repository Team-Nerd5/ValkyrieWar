// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingWidget.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	// 매 프레임 UI를 업데이트하기 위해 사용
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ★ 핵심: 언리얼 에디터(UMG)에서 생성한 Progress Bar와 이름이 똑같으면 자동 연결됨
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;
};
