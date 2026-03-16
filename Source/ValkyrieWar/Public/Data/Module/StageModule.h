// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseModule.h"
#include "StageModule.generated.h"

class UStageData;
class UGameManager;
struct FStageEntryData;

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UStageModule : public UBaseModule
{
	GENERATED_BODY()
public:
	virtual void Initialize(UGameManager* InGameManager) override;

	bool HasChapter(int32 InChapter) const;
	void GetAllChapters(TArray<int32>& OutChapters) const;
	int32 GetMinChapter() const;
	int32 GetMaxChapter() const;

	// UI는 이 함수만 통해서 챕터/스테이지 정보를 가져감
	void GetStageListViewDataByChapter(int32 InChapter, TArray<FStageEntryData>& OutStageList) const;

	FORCEINLINE int32 MakeStageCode(int32 InChapter, int32 InStage)
	{
		return InChapter * StageCodeMultiplier + InStage;
	}

	FORCEINLINE void SetSelectedStage(int32 InCode) { SelectedStageCode = InCode; }

	FORCEINLINE int32 GetSelectedStageCode() { return SelectedStageCode; }

	FORCEINLINE int32 GetChapterFromStageCode(int32 InStageId)
	{
		return InStageId / StageCodeMultiplier;
	}

	FORCEINLINE int32 GetStageFromStageCode(int32 InStageId)
	{
		return InStageId % StageCodeMultiplier;
	}

protected:
	virtual void MakeData() override;
private:
	UPROPERTY()
	TMap<int32, TObjectPtr<UStageData>> StageDataByChapter;

	UPROPERTY()
	int32 SelectedStageCode = 0;

	static constexpr int32 StageCodeMultiplier = 10000;
};
