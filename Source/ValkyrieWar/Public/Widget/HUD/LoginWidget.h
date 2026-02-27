// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ULoginWidget : public UBaseWidget
{
	GENERATED_BODY()

private:
	//게임 시작 가능여부 체크
	bool bIsReadyToStart = false;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> StartText = nullptr;

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnTouchStarted(const FGeometry& InGeometry, const FPointerEvent& InTouchEvent) override;

	virtual void NativeConstruct() override;

	//시작 터치가 되면 실행
	void OnStartGame();

	//시작 터치 가능 처리
	UFUNCTION()
	void OnReadyToStart();
public:
	virtual void OpenUI() override;
};
