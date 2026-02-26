// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Components/Image.h"
#include "CharacterInfoEntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UCharacterInfoEntryWidget : public UBaseWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;

	// 캐릭터 정보 인벤토리 타일뷰에서 들어온 아이템을 UI에 표시하기 위한 함수
	UFUNCTION()
	void Init(UObject* InData);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Icon = nullptr;
};
