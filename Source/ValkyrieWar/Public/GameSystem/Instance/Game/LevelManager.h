// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/StreamableManager.h"
#include "Data/Enum/StateEnums.h"
#include "LevelManager.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ULevelManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;

public:
	UFUNCTION(BlueprintCallable, Category = "LevelManager")
	void LoadLevelAsync(TSoftObjectPtr<UWorld> InMap);

	void InitEvent();

	// 위젯에서 호출할 진척도 반환 함수 (짧으므로 FORCEINLINE 처리)
	// 위젯 NativeTick
	FORCEINLINE float GetLoadingProgress() const { return CalculateCombinedProgress(); }
private:
	// 단계별 실행 함수
	void StartDataLoading();
	void OnDataLoadCompleted();

	void StartMapLoading();
	void OnMapLoadCompleted();

	float CalculateCombinedProgress() const;


	UFUNCTION()
	void OnDataLoadComplete();

private:
	UPROPERTY()
	TSoftObjectPtr<UWorld> TargetMap;

	TSharedPtr<FStreamableHandle> MapLoadHandle;

	ELoadingState CurrentState = ELoadingState::None;
	float DataLoadProgress = 0.0f;

	int32 DataLoadTask = 0;
	float DataLoadedTask = 0;
};
