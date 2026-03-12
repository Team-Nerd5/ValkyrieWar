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

	CacheCards();
	BindDelegates();
}

void UUnitCardListWidget::NativeDestruct()
{
	UnbindDelegates();
	Cards.Reset();

	Super::NativeDestruct();
}

void UUnitCardListWidget::CacheCards()
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
		WorldEventSystem->Battle.OnAllyUnitListReady.AddUniqueDynamic(this, &UUnitCardListWidget::SetIds);
	}

	bBound = true;
}

void UUnitCardListWidget::UnbindDelegates()
{
	if (!bBound) return;

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnBattleModeChanged.RemoveDynamic(this, &UUnitCardListWidget::OnBattleModeChanged);
		WorldEventSystem->Battle.OnUpgradeStateChanged.RemoveDynamic(this, &UUnitCardListWidget::OnUpgradeStateChanged);
		WorldEventSystem->Battle.OnAllyUnitListReady.RemoveDynamic(this, &UUnitCardListWidget::SetIds);
	}

	bBound = false;
}

void UUnitCardListWidget::SetIds(TArray<int32> InIds)
{
	if (InIds.IsEmpty() || Cards.IsEmpty()) return;

	if (UWorld* World = GetWorld())
	{
		if (USpawnUpgradeSubsystem* Sub = World->GetSubsystem<USpawnUpgradeSubsystem>())
		{
			for (int i = 0; i < InIds.Num(); i++)
			{
				if (Cards[i])
				{
					Cards[i]->Init(InIds[i]);
					Sub->EnsureFamily(InIds[i]);
				}
			}
		}
	}
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
		CacheCards();
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
