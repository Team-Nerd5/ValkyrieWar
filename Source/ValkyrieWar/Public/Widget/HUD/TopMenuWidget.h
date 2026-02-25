// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Data/Table/GameData/GoodsDataRow.h"
#include "TopMenuWidget.generated.h"

class UGoodsItemWidget;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UTopMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void SetData(FGoodsDataRow* InData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UHorizontalBox> GoodsItemRoot = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> BackButton = nullptr;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class USizeBox> BackButtonPanel = nullptr;
	//계정정보 위젯 추가...

	UPROPERTY(EditAnywhere)
	TSubclassOf<UGoodsItemWidget> GoodsItemClass;

	TMap<EGoodsType, UGoodsItemWidget*> GoodsWidgets;

protected:
	UFUNCTION()
	void OnClickBackButton();
};
