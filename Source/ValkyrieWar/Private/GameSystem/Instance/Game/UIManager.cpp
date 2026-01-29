// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Instance/Game/UIManager.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void UUIManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UUIManager::Deinitialize()
{
	CloseAllPopupUI();

	//모든 캐시된 위젯 정리
	//TODO 함수형으로 전환
	for (auto& Pair : CachedWidgets)
	{
		if (Pair.Value && Pair.Value->IsInViewport())
		{
			Pair.Value->RemoveFromParent();
		}
	}

	CachedWidgets.Empty();
	PersistentUIMap.Empty();
	PopupUIStack.Empty();

	Super::Deinitialize();
}

int32 UUIManager::CalculateZOrder(UBaseWidget* Widget) const
{
	if (!Widget)
	{
		return 0;
	}

	// Persistent 타입의 경우 ZOrder 낮게
	if (Widget->UILayer == EUILayer::PERSISTENT)
	{
		return Widget->ZOrder;
	}
	// Popup 타입의 경우 ZOrder 높게 (100 + 스택 깊이)
	else
	{
		return 100 + PopupUIStack.Num();
	}
}

void UUIManager::NotifyInputModeChange()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		return;
	}

	if (HasOpenPopupUI())
	{
		// Popup이 있을 때는 UI Only 모드
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(PopupUIStack.Last()->TakeWidget());
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}
	else
	{
		// Popup이 없을 때는 Game Only 모드
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}
}

void UUIManager::CloseUIInternal(UBaseWidget* Widget)
{
	if (!Widget || !Widget->IsOpen())
	{
		return;
	}

	if (Widget->UILayer == EUILayer::PERSISTENT)
	{
		Widget->CloseUI();
		Widget->RemoveFromParent();

		TSubclassOf<UBaseWidget> WidgetClass = Widget->GetClass();
		PersistentUIMap.Remove(WidgetClass);
	}
	else // Popup
	{
		int32 Index = PopupUIStack.Find(Widget);
		if (Index != INDEX_NONE)
		{
			PopupUIStack.RemoveAt(Index);
			Widget->CloseUI();
			Widget->RemoveFromParent();

			RefreshTopPopupUI();
			NotifyInputModeChange();
		}
	}
}

void UUIManager::CloseUI(UBaseWidget* Widget)
{
	CloseUIInternal(Widget);
}

void UUIManager::CloseTopPopupUI()
{
	if (PopupUIStack.Num() == 0)
	{
		return;
	}

	UBaseWidget* TopWidget = PopupUIStack.Pop();
	TopWidget->CloseUI();
	TopWidget->RemoveFromParent();

	RefreshTopPopupUI();
	NotifyInputModeChange();
}

void UUIManager::CloseAllPopupUI()
{
	while (PopupUIStack.Num() > 0)
	{
		CloseTopPopupUI();
	}
}

void UUIManager::RefreshTopPopupUI()
{
	// 스택에 팝업이 남아있다면 새로운 Top에게 포커스 전달
	if (PopupUIStack.Num() > 0)
	{
		PopupUIStack.Last()->OnFocusGained();
	}
}

void UUIManager::ResetAllUIStates()
{
	//캐싱된 인스턴스들 상태 초기화
	for (auto& pair : CachedWidgets)
	{
		if (pair.Value)
		{
			if (pair.Value->IsOpen())
			{
				pair.Value->CloseUI();
			}

			if (pair.Value->IsInViewport())
			{
				pair.Value->RemoveFromParent();
			}
		}
	}

	//관련 컨테이너 비우기
	PersistentUIMap.Empty();
	PopupUIStack.Empty();
	CachedWidgets.Empty();

	//입력모드 초기화
	APlayerController* pc = GetWorld()->GetFirstPlayerController();
	if (pc)
	{
		pc->SetInputMode(FInputModeGameOnly());
		pc->SetShowMouseCursor(false);
	}
}