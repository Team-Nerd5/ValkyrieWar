// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Gacha/GachaResultWidget.h"
#include "Widget/Item/Gacha/GachaResultItemWidget.h"

#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "Components/PanelWidget.h"
#include "Components/Button.h"

void UGachaResultWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (NextCharacterButton)
		NextCharacterButton->OnClicked.AddDynamic(this, &UGachaResultWidget::OnClickNextCharacter);

	if (CloseButton)
		CloseButton->OnClicked.AddDynamic(this, &UGachaResultWidget::OnClickCloseGacha);
}

void UGachaResultWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (NextCharacterButton)
		NextCharacterButton->OnClicked.RemoveDynamic(this, &UGachaResultWidget::OnClickNextCharacter);

	if (CloseButton)
		CloseButton->OnClicked.RemoveDynamic(this, &UGachaResultWidget::OnClickCloseGacha);
}

void UGachaResultWidget::OpenUI()
{
	UIType = EUIType::PopupGachaResult;

	Super::OpenUI();
}

void UGachaResultWidget::OnClickNextCharacter()
{
	if (EventSystem)
	{
		EventSystem->Lobby.OnShowNextGacha.Broadcast();
	}
}

void UGachaResultWidget::OnClickCloseGacha()
{
	if (EventSystem)
	{
		EventSystem->Lobby.OnLoadLobby.Broadcast();
	}
}

void UGachaResultWidget::ShowGachaResults()
{
	//음...패널을 두개로 나눌 경우
	//데이터 개수로 판단해서 처리하면 될 듯함

	if (WrapBox_Result)
	{
		//WrapBox_Result->ClearChildren();
		WrapBox_Result->SetVisibility(ESlateVisibility::Visible);

		//자식 세팅
		//아이템이나 캐릭터가 나옴. 이미지, 개수(1개 초과만 표기), 등급에 따른 배경색 변경 처리
	}

	if (NextCharacterButton)
		NextCharacterButton->SetVisibility(ESlateVisibility::Hidden);

}

void UGachaResultWidget::SetResultData(TArray<UObject*> InResultData)
{
	ResultData = InResultData;

	//결과 패널을 꺼줌
	if (WrapBox_Result)
		WrapBox_Result->SetVisibility(ESlateVisibility::Hidden);
		
	//스킵 버튼을 켜줌
	if (NextCharacterButton)
		NextCharacterButton->SetVisibility(ESlateVisibility::Visible);
	
}
