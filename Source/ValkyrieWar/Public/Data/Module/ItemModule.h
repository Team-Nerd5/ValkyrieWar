// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameSystem/Base/BaseModule.h"
#include "Data/Game/ItemData.h"
#include "ItemModule.generated.h"

/**
 * 아이템 데이터 목록을 들고있고 관리하는 클래스
 * 인벤토리는 이 데이터를 가공
 */
UCLASS()
class VALKYRIEWAR_API UItemModule : public UBaseModule
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGameManager* InGameManager) override;

	virtual void MakeData() override;

	//저장한 아이템 추가하는 함수 구현
	void AddItem(int32 InDataId, int32 Amount);

private:
	FItemDataRow* GetTableDataById(int32 InDataId);

private:
	UPROPERTY()
	TObjectPtr<UDataTable> DataTable = nullptr;

	TMap<int32, FItemDataRow*> TableDataByDataId;
};
