// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseCharacter.h"
#include "Data/Enum/CharacterEnums.h"
#include "Data/Game/ValkyrieData.h"
#include "Engine/DataTable.h"
#include "ValkyrieCharacter.generated.h"

class AValkyrieAIController;

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API AValkyrieCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AValkyrieCharacter();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombo();
	

	virtual void OnAttackNotify() override;
	virtual void OnSkillNotify() override;
 
	virtual ETeamType GetTeamType() const override;
	virtual void OnDeath() override;

	/*
	virtual void StartAttackSequence() ; // 공격모션실행

	virtual void FaceTarget(AActor* Target) ;

	AActor* FindTarget(float SearchRadius);
	
	* 캐릭터 Spawn 후 데이터 세팅에 사용
	
	*/
	
	void SetData(UValkyrieData* InData);

	// 자동 전투 On/Off
	void EnableAutoBattle();
	void DisableAutoBattle();

	inline float GetAttackRange() { return AttackRange; }

public:

#if WITH_EDITOR
	void ChangeWeapon(UItemData* InEquip);
#endif

	void InitValkyrieWidget();

	void EquipWeapon(UItemData* InWeapon);
protected:
	virtual void BeginPlay() override;

	void UpdateWeaponMesh();

	void BindAttributeDelegates();
	void UnbindAttributeDelegates();

	void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	void BroadcastHpChanged();
	void BroadcastSkillListReady();

	// 컨트롤러 전환 함수
	void SwitchToAIController();
	void SwitchToPlayerController();

	
protected: // 타겟팅
	AActor* FindBestMeleeMainTarget(float InRange) const;
	void CollectAttackTargets(TArray<AActor*>& OutTargets) const;
	void CollectSkillTargets(int32 InSkillIndex, TArray<AActor*>& OutTargets) const;
	void CollectSplashTargets(
		AActor* MainTarget,
		int32 SplashTargetAmount,
		float SplashRange,
		TArray<AActor*>& OutTargets
	) const;

	bool IsValidAttackTargetActor(const AActor* TargetActor) const;

protected: // 스킬 관련
	UFUNCTION()
	void OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	virtual void ExecuteAttack() override;
	UFUNCTION()
	virtual void ExecuteSkill(int32 InSkillIndex) override;

	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void HandleControlModeChanged(EInputControlMode NewMode);

	float GetCurrentWorldTime() const;
	void InitSkillCooldowns();
	float GetSkillCooldownDuration(int32 InSkillIndex) const;
	void MarkSkillUsed(int32 InSkillIndex);
	bool IsSkillOnCooldown(int32 InSkillIndex) const;
	float GetSkillRemainingCooldown(int32 InSkillIndex) const;

	virtual void TryUseSkillOrAttack() override;
	int32 FindUsableSkillIndex() const;
	bool CanUseSkillIndex(int32 InSkillIndex) const;

protected:
	UPROPERTY()
	TObjectPtr<UValkyrieData> Data = nullptr;

	EValkyrieModeType ValkyrieMode = EValkyrieModeType::None;

	// 이전 플레이어 컨트롤러 저장
	UPROPERTY()
	TWeakObjectPtr<APlayerController> CachedPlayerController;

	// 현재 AI 컨트롤러
	UPROPERTY()
	TObjectPtr<AValkyrieAIController> CachedAIController = nullptr;

	// 자동 전투 상태
	bool bIsAutoBattle = false;

	// 각 스킬의 "다음 사용 가능 시간"
	UPROPERTY()
	TArray<float> SkillNextUsableTime;

private:
	float AttackRange = 0.f;
};
