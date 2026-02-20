// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "CommonConfirmWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UCommonConfirmWidget : public UBaseWidget
{
	GENERATED_BODY()
	//오버로드 해서
	//확인 , 확인/취소 팝업 버튼 조절
	//누르는 키에 따라 델리게이트 같이 보내서..
	//텍스트 입력 받아서 타이틀, 내용 세팅
};
