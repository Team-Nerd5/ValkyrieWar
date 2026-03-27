#include "Widget/Item/Battle/UnitSpawnUpgradeCardWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/SizeBoxSlot.h"

#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "Data/Module/UnitModule.h"

void UUnitSpawnUpgradeCardWidget::Init(int32 InUnitId)
{
	UnitId = InUnitId;

	EnsureWidgetTree();
	ApplyCardStyle(false);
	ApplyStateVisuals();

	UTexture2D* LoadedIcon = nullptr;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDataManager* DataManager = GI->GetSubsystem<UDataManager>())
		{
			if (UUnitModule* UnitModule = DataManager->GetUnitModule())
			{
				const TSoftObjectPtr<UTexture2D> IconPtr = UnitModule->GetUnitIcon(UnitId);

				if (!IconPtr.IsNull())
				{
					LoadedIcon = IconPtr.LoadSynchronous();
				}
			}
		}
	}

	if (LoadedIcon && UnitIconImage)
	{
		UnitIconImage->SetBrushFromTexture(LoadedIcon);
	}
}

void UUnitSpawnUpgradeCardWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	EnsureWidgetTree();
	ApplyCardStyle(IsDesignTime());
	ApplyStateVisuals();
}

void UUnitSpawnUpgradeCardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EnsureWidgetTree();
	ApplyCardStyle(false);
	ApplyStateVisuals();

	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddUniqueDynamic(this, &UUnitSpawnUpgradeCardWidget::HandleUpgradeButton);
	}
}

void UUnitSpawnUpgradeCardWidget::NativeDestruct()
{
	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.RemoveDynamic(this, &UUnitSpawnUpgradeCardWidget::HandleUpgradeButton);
	}

	Super::NativeDestruct();
}

void UUnitSpawnUpgradeCardWidget::HandleUpgradeButton()
{
	if (UnitId <= 0) return;

	// 비용 부족이면 클릭 무시(버튼 비활성화가 되어있어도 안전)
	if (!bAffordableCached) return;

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		// "UnitId == FamilyId" 키로 사용 중
		WorldEventSystem->Battle.OnUpgradeClicked.Broadcast(UnitId);
	}
}

void UUnitSpawnUpgradeCardWidget::SetAffordable(bool bAffordable)
{
	bAffordableCached = bAffordable;
	ApplyStateVisuals();
}

void UUnitSpawnUpgradeCardWidget::ApplyUpgradeState(int32 InLevel, int32 InCost, bool bAffordable)
{
	// BP에서 텍스트/아이콘 갱신(원하면 BP로 다시 돌려도 됨)
	// BP_SetLevel(InLevel);
	// BP_SetCost(InCost);

	if (LevelText) LevelText->SetText(FText::AsNumber(InLevel));
	if (CostText)  CostText->SetText(FText::AsNumber(InCost));

	bAffordableCached = bAffordable;
	ApplyStateVisuals();
}

void UUnitSpawnUpgradeCardWidget::EnsureWidgetTree()
{
	// WBP로 구성되어 있으면 아무 것도 하지 않음
	if (WidgetTree && WidgetTree->RootWidget) return;
	if (!WidgetTree) return;

	// ---- (Fallback) WBP가 없을 때만 간단 트리 생성 ----
	USizeBox* RootSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("RootSizeBox"));
	RootSize->SetWidthOverride(160.f);
	RootSize->SetHeightOverride(200.f);

	UOverlay* RootOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("RootOverlay"));
	RootSize->AddChild(RootOverlay);

	WidgetTree->RootWidget = RootSize;

	// CardBorder
	CardBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CardBorder"));
	{
		UOverlaySlot* OverlaySlot = RootOverlay->AddChildToOverlay(CardBorder);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
	}

	// GlowBorder (optional)
	GlowBorder = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("GlowBorder"));
	{
		UOverlaySlot* OverlaySlot = RootOverlay->AddChildToOverlay(GlowBorder);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
	}

	// Click button
	UpgradeButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("UpgradeButton"));
	{
		UOverlaySlot* OverlaySlot = RootOverlay->AddChildToOverlay(UpgradeButton);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
	}

	// Content
	UVerticalBox* VBox = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ContentVBox"));
	{
		UOverlaySlot* OverlaySlot = RootOverlay->AddChildToOverlay(VBox);
		OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
		OverlaySlot->SetVerticalAlignment(VAlign_Fill);
		OverlaySlot->SetPadding(FMargin(10.f));
	}

	IconSizeBox = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("IconSizeBox"));
	UnitIconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("UnitIconImage"));
	IconSizeBox->AddChild(UnitIconImage);
	{
		UVerticalBoxSlot* VerticalBoxSlot = VBox->AddChildToVerticalBox(IconSizeBox);
		VerticalBoxSlot->SetSize(ESlateSizeRule::Automatic);
		VerticalBoxSlot->SetHorizontalAlignment(HAlign_Center);
		VerticalBoxSlot->SetVerticalAlignment(VAlign_Center);
		VerticalBoxSlot->SetPadding(FMargin(0.f, 6.f));
	}

	LevelText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("LevelText"));
	{
		UVerticalBoxSlot* OverlaySlot = VBox->AddChildToVerticalBox(LevelText);
		OverlaySlot->SetSize(ESlateSizeRule::Automatic);
		OverlaySlot->SetHorizontalAlignment(HAlign_Left);
		OverlaySlot->SetPadding(FMargin(2.f));
	}

	CostText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CostText"));
	{
		UVerticalBoxSlot* OverlaySlot = VBox->AddChildToVerticalBox(CostText);
		OverlaySlot->SetSize(ESlateSizeRule::Automatic);
		OverlaySlot->SetHorizontalAlignment(HAlign_Left);
		OverlaySlot->SetPadding(FMargin(2.f));
	}
}

FLinearColor UUnitSpawnUpgradeCardWidget::GetBgColor() const
{
	return FLinearColor(0.09f, 0.09f, 0.14f, 1.f); // #171824
}

FLinearColor UUnitSpawnUpgradeCardWidget::GetFrameColor() const
{
	return FLinearColor(0.17f, 0.18f, 0.27f, 1.f); // #2C2F44
}

FLinearColor UUnitSpawnUpgradeCardWidget::GetTextColor() const
{
	return FLinearColor(0.91f, 0.93f, 1.00f, 1.f); // #E9ECFF
}

FLinearColor UUnitSpawnUpgradeCardWidget::GetManaColor() const
{
	return FLinearColor(0.023153f, 0.074214f, 1.00f, 1.f); // #45E6FF
}

FLinearColor UUnitSpawnUpgradeCardWidget::GetWarnColor() const
{
	return FLinearColor(1.00f, 0.30f, 0.65f, 1.f); // #FF4DA6
}

void UUnitSpawnUpgradeCardWidget::ApplyCardStyle(bool bIsDesignTime)
{
	const FLinearColor Bg = GetBgColor();
	const FLinearColor Frame = GetFrameColor();
	const FLinearColor Text = GetTextColor();
	const FLinearColor Mana = GetManaColor();

	if (OuterBorder)
	{
		OuterBorder->SetPadding(FMargin(2.f));
		OuterBorder->SetBrushColor(Frame);
	}

	if (CardBorder)
	{
		CardBorder->SetPadding(FMargin(10.f));
		CardBorder->SetBrushColor(Bg);
	}

	// 글로우는 "affordable일 때만" 보여주도록 ApplyStateVisuals에서 제어
	if (GlowBorder)
	{
		GlowBorder->SetBrushColor(FLinearColor(0.102f, 0.118f, 0.165f, 0.85f));
	}

	if (UnitIconImage && bIsDesignTime)
	{
		UnitIconImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.95f));
	}

	if (LevelText)
	{
		if (bIsDesignTime && (LevelText->GetText().IsEmptyOrWhitespace()))
		{
			LevelText->SetText(FText::FromString(TEXT("1")));
		}
		LevelText->SetColorAndOpacity(FSlateColor(Text));

		FSlateFontInfo Font = LevelText->GetFont();
		Font.Size = 18;
		LevelText->SetFont(Font);
	}

	if (CostText)
	{
		if (bIsDesignTime && (CostText->GetText().IsEmptyOrWhitespace()))
		{
			CostText->SetText(FText::FromString(TEXT("30")));
		}
		// affordable 여부에 따른 색상은 ApplyStateVisuals에서 처리
		FSlateFontInfo Font = CostText->GetFont();
		Font.Size = 16;
		CostText->SetFont(Font);
	}

	if (ManaIcon)
	{
		ManaIcon->SetColorAndOpacity(Mana);
	}
}

void UUnitSpawnUpgradeCardWidget::ApplyStateVisuals()
{
	const FLinearColor Mana = GetManaColor();
	const FLinearColor Warn = GetWarnColor();

	// 버튼 enable은 affordable만 사용
	if (UpgradeButton)
	{
		UpgradeButton->SetIsEnabled(bAffordableCached);
	}

	// 비용 텍스트 컬러: affordable이면 Mana, 아니면 Warn
	if (CostText)
	{
		CostText->SetColorAndOpacity(FSlateColor(bAffordableCached ? Mana : Warn));
	}

	// 글로우: affordable일 때만 표시
	if (GlowBorder)
	{
		GlowBorder->SetVisibility(bAffordableCached ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	// afford 불가면 살짝 흐리게
	const float Opacity = bAffordableCached ? 1.f : 0.80f;
	if (UnitIconImage) UnitIconImage->SetRenderOpacity(Opacity);
	if (LevelText)     LevelText->SetRenderOpacity(Opacity);
}
