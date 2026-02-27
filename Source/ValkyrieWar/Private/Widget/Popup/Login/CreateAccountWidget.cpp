// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Popup/Login/CreateAccountWidget.h"
#include "GameSystem/Instance/Game/SaveManager.h"
#include "GameSystem/Instance/Game/UIManager.h"

#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "Components/EditableTextBox.h"
#include "Components/Button.h"

void UCreateAccountWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(false);
		ConfirmButton->OnClicked.AddDynamic(this, &UCreateAccountWidget::OnClickConfirm);
	}

	if (CancelButton)
	{
		CancelButton->OnClicked.AddDynamic(this, &UCreateAccountWidget::OnClickCancel);
	}

	if (NicknameInput)
	{
		NicknameInput->OnTextChanged.AddDynamic(this, &UCreateAccountWidget::OnNicknameChanged);
	}
}

void UCreateAccountWidget::OnClickConfirm()
{
	if (USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>())
	{
		SaveManager->GetUserId();
		SaveManager->CreateAccount(Nickname);

		if (EventSystem)
		{
			EventSystem->Login.OnLoginStateChanged.Broadcast(ELoginState::CreateAccount);
		}
	}
}

void UCreateAccountWidget::OnClickCancel()
{
	//경고 팝업...
}

void UCreateAccountWidget::OnNicknameChanged(const FText& Text)
{
	Nickname = Text.ToString();

	if (Nickname.Len() >= 2)
	{
		ConfirmButton->SetIsEnabled(true);
	}
	else
	{
		ConfirmButton->SetIsEnabled(false);
	}
}
