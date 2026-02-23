// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/HUD/LobbyWidget.h"
#include "GameSystem/Instance/Game/DataManager.h"

void ULobbyWidget::OpenUI()
{
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		//1. 탑메뉴 생성해서 세팅?
		//2. 컨텐츠 데이터 사용해서 버튼 세팅
	}

	Super::OpenUI();
}
