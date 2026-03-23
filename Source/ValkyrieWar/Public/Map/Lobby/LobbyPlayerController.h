// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Data/Enum/StateEnums.h"
#include "Data/Enum/DataEnums.h"
#include "LobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	ALobbyPlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	void ChangeGameState(ELobbyState InState);

	void SetActorCamera(FName InLevelName);

	UFUNCTION()
	void LoadLobbyLevel();

	UFUNCTION()
	void LoadGachaLevel(int32 InAmount, int32 InGachaGroupId);

	UFUNCTION()
	void OnLobbyLevelLoaded();

	UFUNCTION()
	void OnGachaLevelLoaded();
	UFUNCTION()
	void OnGachaLevelShown();
	UFUNCTION()
	void OnLobbyLevelShown();

	//가챠
	void SetGachaResult(int32 InAmount, int32 InGachaGroupId);

	int32 GetMasteryItemAmount(EGradeType InGrade);

	void ShowGachaCharacter();

	UFUNCTION()
	void ShowNextGacha();

private:
	int32 CurrentGachaIndex = 0;

	UPROPERTY()
	TArray<TObjectPtr<UObject>> GachaResultData;

	UPROPERTY()
	TObjectPtr<class UGachaResultWidget> GachaResultWidget = nullptr;
};
