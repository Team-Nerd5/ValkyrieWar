#include "Widget/Popup/Battle/BattlePauseMenuWidget.h"

#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

void UBattlePauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bIsFocusable = true;

	if (Btn_Return)
	{
		Btn_Return->OnClicked.AddUniqueDynamic(this, &UBattlePauseMenuWidget::HandleReturnClicked);
	}

	if (Btn_BackToLobby)
	{
		Btn_BackToLobby->OnClicked.AddUniqueDynamic(this, &UBattlePauseMenuWidget::HandleBackToLobbyClicked);
	}
}

void UBattlePauseMenuWidget::NativeDestruct()
{
	if (Btn_Return)
	{
		Btn_Return->OnClicked.RemoveDynamic(this, &UBattlePauseMenuWidget::HandleReturnClicked);
	}

	if (Btn_BackToLobby)
	{
		Btn_BackToLobby->OnClicked.RemoveDynamic(this, &UBattlePauseMenuWidget::HandleBackToLobbyClicked);
	}

	Super::NativeDestruct();
}

void UBattlePauseMenuWidget::OpenUI()
{
	Super::OpenUI();
	// UBaseWidget::OpenUI()를 그대로 쓰면 CreateTopMenu()가 호출되므로
	// 여기서는 직접 처리하는 쪽이 더 안전함.
	//bIsOpen = true;
	//SetVisibility(ESlateVisibility::Visible);

	if (bPauseGameWhenOpened)
	{
		UGameplayStatics::SetGamePaused(this, true);
	}
	//SetKeyboardFocus();
}

void UBattlePauseMenuWidget::CloseUI()
{
	if (bPauseGameWhenOpened)
	{
		UGameplayStatics::SetGamePaused(this, false);
	}

	Super::CloseUI();
}

FReply UBattlePauseMenuWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	const FKey PressedKey = InKeyEvent.GetKey();

	if (PressedKey == EKeys::Escape)
	{
		HandleReturnClicked();
		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UBattlePauseMenuWidget::HandleReturnClicked()
{
	CloseUI();
}

void UBattlePauseMenuWidget::HandleBackToLobbyClicked()
{
	if (bPauseGameWhenOpened)
	{
		UGameplayStatics::SetGamePaused(this, false);
	}
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnBattleStateChanged.Broadcast(EBattleState::MoveToLobby);
	}
}
