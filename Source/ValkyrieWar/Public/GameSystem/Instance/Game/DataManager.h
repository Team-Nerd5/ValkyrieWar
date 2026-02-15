// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "Data/Module/ItemModule.h"
#include "Data/Module/ValkyrieModule.h"
#include "Data/Module/AttackModule.h"
#include "Data/Module/SkillModule.h"
#include "Data/Module/SkillEffectModule.h"

#include "GameSystem/Base/BaseModule.h"
#include "DataManager.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UDataManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void CreateData();

public:
	FORCEINLINE UItemModule* GetItemModule() const { return ItemModule; }
	FORCEINLINE UValkyrieModule* GetValkyrieModule() const { return ValkyrieModule; }
	FORCEINLINE UAttackModule* GetAttackModule() const { return AttackModule; }
	FORCEINLINE USkillModule* GetSkillModule() const { return SkillModule; }
	FORCEINLINE USkillEffectModule* GetSkillEffectModule() const { return SkillEffectModule; }

private:
	template<typename T>
	void InitializeData(TObjectPtr<T>& InVal);
private:
	UPROPERTY()
	TObjectPtr<UItemModule> ItemModule = nullptr;
	UPROPERTY()
	TObjectPtr<UValkyrieModule> ValkyrieModule = nullptr;
	UPROPERTY()
	TObjectPtr<UAttackModule> AttackModule = nullptr;
	UPROPERTY()
	TObjectPtr<USkillModule> SkillModule = nullptr;
	UPROPERTY()
	TObjectPtr<USkillEffectModule> SkillEffectModule = nullptr;


	UPROPERTY()
	TWeakObjectPtr<UGameManager> GameManager = nullptr;
};

template<typename T>
inline void UDataManager::InitializeData(TObjectPtr<T>& InVal)
{
	if (GameManager.IsValid())
	{
		static_assert(TIsDerivedFrom<T, UBaseModule>::IsDerived, "T must inherit from UBaseModule.");

		if (!InVal)
		{
			InVal = NewObject<T>(this);
		}
		InVal->Initialize(GameManager.Get());
	}

}
