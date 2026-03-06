#include "Widget/Item/Battle/UnitCardListWidget.h"

#include "Widget/Item/Battle/UnitUpgradeCardWidget.h"

#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/World/SpawnUpgradeSubsystem.h"

#include "Animation/UMGSequencePlayer.h"

void UUnitCardListWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);
	bIsShown = false;

	CacheCardsIfNeeded();

	if (Card_1) Card_1->Init(210002);
	if (Card_2) Card_2->Init(210001);
	if (Card_3) Card_3->Init(103);
	if (Card_4) Card_4->Init(104);

	BindDelegates();

	// 처음 진입 시점에 현재 상태를 한번 밀어줌
	if (UWorld* World = GetWorld())
	{
		if (USpawnUpgradeSubsystem* Sub = World->GetSubsystem<USpawnUpgradeSubsystem>())
		{
			Sub->EnsureFamily(210002);
			Sub->EnsureFamily(210001);
			Sub->EnsureFamily(103);
			Sub->EnsureFamily(104);

			Sub->SyncAll();
		}
	}
}

void UUnitCardListWidget::NativeDestruct()
{
	UnbindDelegates();
	Cards.Reset();

	Super::NativeDestruct();
}

void UUnitCardListWidget::CacheCardsIfNeeded()
{
	Cards.Reset();

	if (Card_1) Cards.Add(Card_1);
	if (Card_2) Cards.Add(Card_2);
	if (Card_3) Cards.Add(Card_3);
	if (Card_4) Cards.Add(Card_4);
}

void UUnitCardListWidget::BindDelegates()
{
	if (bBound) return;

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnBattleModeChanged.AddUniqueDynamic(this, &UUnitCardListWidget::OnBattleModeChanged);

		WorldEventSystem->Battle.OnUpgradeStateChanged.AddUniqueDynamic(this, &UUnitCardListWidget::OnUpgradeStateChanged);

		bBound = true;
	}
}

void UUnitCardListWidget::UnbindDelegates()
{
	if (!bBound) return;

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnBattleModeChanged.RemoveDynamic(this, &UUnitCardListWidget::OnBattleModeChanged);
		WorldEventSystem->Battle.OnUpgradeStateChanged.RemoveDynamic(this, &UUnitCardListWidget::OnUpgradeStateChanged);
	}

	bBound = false;
}

void UUnitCardListWidget::OnBattleModeChanged(EInputControlMode InCurrentMode)
{
	SetVisibleByMode(InCurrentMode);
}

void UUnitCardListWidget::SetVisibleByMode(EInputControlMode InMode)
{
	const bool bShouldShow = (InMode == EInputControlMode::Auto);

	if (bShouldShow && !bIsShown)
	{
		ShowList(true);

		// 보여줄 때도 최신 상태 보장
		if (UWorld* World = GetWorld())
		{
			if (USpawnUpgradeSubsystem* Sub = World->GetSubsystem<USpawnUpgradeSubsystem>())
			{
				Sub->SyncAll();
			}
		}
	}
	else if (!bShouldShow && bIsShown)
	{
		HideList(true);
	}
}

void UUnitCardListWidget::OnUpgradeStateChanged(int32 FamilyId, int32 Level, int32 Cost, bool bAffordable)
{
	if (FamilyId <= 0) return;

	if (Cards.Num() == 0)
	{
		CacheCardsIfNeeded();
	}

	if (UUnitUpgradeCardWidget* Card = FindCardByFamilyId(FamilyId))
	{
		Card->ApplyUpgradeState(Level, Cost, bAffordable);
	}
}

UUnitUpgradeCardWidget* UUnitCardListWidget::FindCardByFamilyId(int32 FamilyId) const
{
	for (UUnitUpgradeCardWidget* Card : Cards)
	{
		if (Card && Card->GetUnitId() == FamilyId)
		{
			return Card;
		}
	}
	return nullptr;
}

void UUnitCardListWidget::ShowList(bool bAnimate)
{
	if (bIsShown) return;
	bIsShown = true;

	SetVisibility(ESlateVisibility::Visible);

	if (Anim_Out && IsAnimationPlaying(Anim_Out))
	{
		StopAnimation(Anim_Out);
	}

	if (bAnimate && Anim_In)
	{
		PlayAnimation(Anim_In, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);
	}
}

void UUnitCardListWidget::HideList(bool bAnimate)
{
	if (!bIsShown) return;
	bIsShown = false;

	if (Anim_In && IsAnimationPlaying(Anim_In))
	{
		StopAnimation(Anim_In);
	}

	if (bAnimate && Anim_Out)
	{
		UUMGSequencePlayer* Player = PlayAnimation(Anim_Out, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f);

		if (Player)
		{
			Player->OnSequenceFinishedPlaying().AddWeakLambda(this, [this](UUMGSequencePlayer&)
				{
					if (!bIsShown)
					{
						SetVisibility(ESlateVisibility::Collapsed);
					}
				});
		}
		else
		{
			SetVisibility(ESlateVisibility::Collapsed);
		}
		return;
	}

	SetVisibility(ESlateVisibility::Collapsed);
}
