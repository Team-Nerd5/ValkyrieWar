#include "Widget/Item/Battle/UnitUpgradeCardWidget.h"

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

void UUnitUpgradeCardWidget::Init(int32 InUnitId)
{
	UnitId = InUnitId;

	// Init 시점에도 스타일 적용 가능(아이콘/텍스트는 BP에서 갱신)
	EnsureWidgetTree();
	ApplyCardStyle(false);
	ApplyStateVisuals();
}

void UUnitUpgradeCardWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	EnsureWidgetTree();
	ApplyCardStyle(IsDesignTime());
	ApplyStateVisuals();
}

void UUnitUpgradeCardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	EnsureWidgetTree();
	ApplyCardStyle(false);
	ApplyStateVisuals();

	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.RemoveDynamic(this, &UUnitUpgradeCardWidget::HandleUpgradeButton);
		UpgradeButton->OnClicked.AddDynamic(this, &UUnitUpgradeCardWidget::HandleUpgradeButton);
	}
}

void UUnitUpgradeCardWidget::NativeDestruct()
{
	// 위젯 소멸 시 델리게이트 정리 (중복/메모리/GC 안전)
	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.RemoveDynamic(this, &UUnitUpgradeCardWidget::HandleUpgradeButton);
	}

	Super::NativeDestruct();
}

void UUnitUpgradeCardWidget::HandleUpgradeButton()
{
	UE_LOG(LogTemp, Log, TEXT("HandleUpgradeButton : %d"), UnitId);
	if (UnitId <= 0) return;

	// MAX면 클릭 무시
	if (bIsMaxCached) return;

	// 비용 부족이면 클릭 무시(버튼 비활성화가 되어있어도 안전)
	if (!bAffordableCached) return;

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Broadcast] Sub=%p World=%s ES=%p"),
			this, *GetWorld()->GetName(), WorldEventSystem);

		// "UnitId == FamilyId" 키로 사용 중 (요구사항 그대로)
		WorldEventSystem->Battle.OnUpgradeClicked.Broadcast(UnitId);
	}
}

void UUnitUpgradeCardWidget::SetAffordable(bool bAffordable)
{
	bAffordableCached = bAffordable;
	ApplyStateVisuals();
}

void UUnitUpgradeCardWidget::SetMaxLevel(bool bIsMax)
{
	bIsMaxCached = bIsMax;
	ApplyStateVisuals();
}

void UUnitUpgradeCardWidget::ApplyUpgradeState(int32 InLevel, int32 InCost, bool bAffordable, bool bIsMax)
{
	// BP에서 텍스트/아이콘 갱신
	//BP_SetLevel(InLevel);
	//BP_SetCost(InCost);

	LevelText->SetText(FText::AsNumber(InLevel));
	CostText->SetText(FText::AsNumber(InCost));

	bAffordableCached = bAffordable;
	bIsMaxCached = bIsMax;
	ApplyStateVisuals();
}

void UUnitUpgradeCardWidget::EnsureWidgetTree()
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

	UnitIconImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("UnitIconImage"));
	{
		UVerticalBoxSlot* OverlaySlot = VBox->AddChildToVerticalBox(UnitIconImage);
		OverlaySlot->SetSize(ESlateSizeRule::Fill);
		OverlaySlot->SetHorizontalAlignment(HAlign_Center);
		OverlaySlot->SetVerticalAlignment(VAlign_Center);
		OverlaySlot->SetPadding(FMargin(0.f, 6.f));
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

FLinearColor UUnitUpgradeCardWidget::GetBgColor() const
{
	return FLinearColor(0.09f, 0.09f, 0.14f, 1.f); // #171824
}

FLinearColor UUnitUpgradeCardWidget::GetFrameColor() const
{
	return FLinearColor(0.17f, 0.18f, 0.27f, 1.f); // #2C2F44
}

FLinearColor UUnitUpgradeCardWidget::GetTextColor() const
{
	return FLinearColor(0.91f, 0.93f, 1.00f, 1.f); // #E9ECFF
}

FLinearColor UUnitUpgradeCardWidget::GetManaColor() const
{
	return FLinearColor(0.27f, 0.90f, 1.00f, 1.f); // #45E6FF
}

FLinearColor UUnitUpgradeCardWidget::GetWarnColor() const
{
	return FLinearColor(1.00f, 0.30f, 0.65f, 1.f); // #FF4DA6
}

void UUnitUpgradeCardWidget::ApplyCardStyle(bool bIsDesignTime)
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

	if (GlowBorder)
	{
		GlowBorder->SetBrushColor(FLinearColor(Mana.R, Mana.G, Mana.B, 0.14f)); // 은은한 글로우
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
		CostText->SetColorAndOpacity(FSlateColor(Mana));

		FSlateFontInfo Font = CostText->GetFont();
		Font.Size = 16;
		CostText->SetFont(Font);
	}

	if (ManaIcon)
	{
		ManaIcon->SetColorAndOpacity(Mana);
	}
}

void UUnitUpgradeCardWidget::ApplyStateVisuals()
{
	const FLinearColor Mana = GetManaColor();
	const FLinearColor Warn = GetWarnColor();

	// MAX면: 글로우 끄고, 비용 텍스트 회색/버튼 비활성
	if (bIsMaxCached)
	{
		if (GlowBorder) GlowBorder->SetVisibility(ESlateVisibility::Hidden);
		if (CostText)   CostText->SetColorAndOpacity(FSlateColor(FLinearColor(0.44f, 0.46f, 0.54f, 1.f)));
		if (UpgradeButton) UpgradeButton->SetIsEnabled(false);

		if (UnitIconImage) UnitIconImage->SetRenderOpacity(0.75f);
		if (LevelText)     LevelText->SetRenderOpacity(0.85f);
		return;
	}

	// 일반 상태
	if (UpgradeButton)
	{
		UpgradeButton->SetIsEnabled(bAffordableCached);
	}

	if (CostText)
	{
		CostText->SetColorAndOpacity(FSlateColor(bAffordableCached ? Mana : Warn));
	}

	if (GlowBorder)
	{
		GlowBorder->SetVisibility(bAffordableCached ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}

	const float Opacity = bAffordableCached ? 1.f : 0.80f;
	if (UnitIconImage) UnitIconImage->SetRenderOpacity(Opacity);
	if (LevelText)     LevelText->SetRenderOpacity(Opacity);
}
