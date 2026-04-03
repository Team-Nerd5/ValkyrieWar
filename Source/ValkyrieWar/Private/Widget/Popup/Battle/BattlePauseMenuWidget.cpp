#include "Widget/Popup/Battle/BattlePauseMenuWidget.h"

#include "Components/Button.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/Game/UIManager.h"

void UBattlePauseMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//SetIsFocusable(true);

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

	if (bPauseGameWhenOpened)
	{
		UGameplayStatics::SetGamePaused(this, true);
	}
}

void UBattlePauseMenuWidget::CloseUI()
{
	if (bPauseGameWhenOpened)
	{
		UGameplayStatics::SetGamePaused(this, false);
	}

	Super::CloseUI();
}

void UBattlePauseMenuWidget::HandleReturnClicked()
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->CloseTopPopupUI();
	}
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
