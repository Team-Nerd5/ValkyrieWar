#include "Widget/Popup/Stage/RewardInfoCardWidget.h"

#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void URewardInfoCardWidget::NativeConstruct()
{
	Super::NativeConstruct();

	RefreshUI();
}

void URewardInfoCardWidget::SetRewardData(const FRewardViewData& InRewardData)
{
	CachedRewardData = InRewardData;
	RefreshUI();
}

void URewardInfoCardWidget::RefreshUI()
{
	//if (Text_Name)
	//{
	//	Text_Name->SetText(FText::FromString(CachedRewardData.Name));
	//}

	if (Image_RewardIcon)
	{
		UTexture2D* LoadedTexture = nullptr;

		if (!CachedRewardData.Icon.IsNull())
		{
			LoadedTexture = CachedRewardData.Icon.LoadSynchronous();
		}

		Image_RewardIcon->SetBrushFromTexture(LoadedTexture);
	}
}
