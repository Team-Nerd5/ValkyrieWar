// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "CreateAccountWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UCreateAccountWidget : public UBaseWidget
{
	GENERATED_BODY()
	//닉네임 생성용 위젯
	//TextInput, 확인/취소 버튼
	//입력 두글자 이상 받아야 확인 활성화
	//취소 누르면 경고팝업(취소하면 게임이 종료됩니다)
	//확인 누르면 확인 팝업(닉네임 ---으로 진행합니까? 이런정도)
	//확인팝업도 확인되면-> 계정 생성 및 저장
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UEditableTextBox> NicknameInput;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> ConfirmButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> CancelButton;

	FString Nickname = TEXT("");

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnClickConfirm();

	UFUNCTION()
	void OnClickCancel();

	UFUNCTION()
	void OnNicknameChanged(const FText& Text);
};
