// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ValkyrieListWidget.generated.h"

class UValkyrieData;
/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UValkyrieListWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetData(TArray<UValkyrieData*> InValkyries);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UListView> ValkyrieListView = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<UValkyrieData>> CachedValkyries;
};
