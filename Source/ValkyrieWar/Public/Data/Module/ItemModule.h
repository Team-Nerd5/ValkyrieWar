// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Game/ItemData.h"
#include "ItemModule.generated.h"

/**
 * 아이템 데이터 목록을 들고있고 관리하는 클래스
 * 인벤토리로 쓸 지, 인벤토리는 따로 만들어서 아이템을 관리하도록 할 지 고민...
 */
UCLASS()
class VALKYRIEWAR_API UItemModule : public UBaseModule
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGameManager* InGameManager) override;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> Table = nullptr;

	UPROPERTY()
	TMap<int32, UItemData*> ItemDataById;
};
