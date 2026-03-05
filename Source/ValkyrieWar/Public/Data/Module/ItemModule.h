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
		
	/// <summary>
	/// 저장 파일에서 아이템 로드할 때 사용
	/// </summary>
	/// <param name="InUID"></param>
	/// <param name="InDataId"></param>
	/// <param name="InAmount"></param>
	void LoadItem(uint64 InUID, int32 InDataId, int32 InAmount);

	/// <summary>
	/// 아이템 새로 추가(획득)
	/// </summary>
	/// <param name="InDataId"></param>
	/// <param name="InAmount"></param>
	void AddItem(int32 InDataId, int32 InAmount = 1);

	/// <summary>
	/// 아이템 개수 변경(추가, 삭제)
	/// </summary>
	/// <param name="InUID"></param>
	/// <param name="InAmount"></param>
	void AddItemAmount(uint64 InUID, int32 InAmount);


	FORCEINLINE TArray<UItemData*> GetItems() { return OwnItemList; }
	UItemData* GetItem(uint64 InUID)
	{
		if (OwnItems.Contains(InUID))
			return *OwnItems.Find(InUID);
		else
			return nullptr;
	}

	FItemDataRow GetTableDataById(int32 InDataId);
protected:
	virtual void MakeData() override;

	void SetList();
private:
	UPROPERTY()
	TMap<uint64, TObjectPtr<UItemData>> OwnItems;
	UPROPERTY()
	TMap<int32, FItemDataRow> TableDataByDataId;

	UPROPERTY()
	TArray<TObjectPtr<UItemData>> OwnItemList;
};
