// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/Enums.h"
#include "DataEncryptHelper.generated.h"

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnSaveGameLoaded, USaveGame*, LoadedSaveGame, bool, bIsSuccess, ESaveType, InSaveType);
/**
 * 
 */

UCLASS()
class VALKYRIEWAR_API UDataEncryptHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "SaveGame|Encryption")
	static bool SaveGameEncrypted(USaveGame* SaveGameObject, ESaveType InSaveType);

	UFUNCTION(BlueprintCallable, Category = "SaveGame|Encryption")
	static USaveGame* LoadGameEncrypted(ESaveType InSaveType);

	UFUNCTION(BlueprintCallable, Category = "SaveGame|Encryption")
	static void LoadGameEncryptedAsync(FOnSaveGameLoaded OnLoaded, ESaveType InSaveType);

private:
	// 키가 유효한지 검사하는 내부 함수 (32바이트 체크)
	static bool IsKeyValid(const FString& Key);

	// 저장 경로 가져오기
	static FString GetSaveFilePath(const FString& SlotName, int32 UserIndex = 0);

	static FString GetEncryptKey();
};
