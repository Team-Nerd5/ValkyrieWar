#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/Enum/CommonEnums.h"
#include "BattleDirectorSubsystem.generated.h"

class AUnitCharacter;
class AValkyrieCharacter;
class ACoreWallActor;

/**
 * BattleDirectorSubsystem
 *
 * 월드 단위 전투 오케스트레이터.
 * 팀별 유닛을 관리하고, 그리드 기반 후보 탐색을 통해
 * 타겟 예약(Engagement Slot)과 전투 매칭을 조율한다.
 *
 * 추가:
 * - 발키리(플레이어, TeamA) 1개를 월드에서 자동 캐시
 * - TeamB 유닛은 발키리를 최우선 타겟으로 시도
 * - 일정 거리 이상 멀어지면 발키리 타겟 해제
 *
 * Cleanup은 타이머가 "요청"만 수행하며,
 * 실제 실행은 안전한 지점에서 MaybeCleanup()을 통해 처리된다.
 */
UCLASS()
class VALKYRIEWAR_API UBattleDirectorSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    /** 중앙 업데이트 루프에서 1회 호출 권장 (Cleanup 실행 지점) */
    void TickCleanup();

    void RegisterUnit(AUnitCharacter* Unit);
    void UnregisterUnit(AUnitCharacter* Unit);

    void RegisterWallAnchor(ETeam Team, AActor* AnchorActor);
    void UnregisterWallAnchor(ETeam Team, AActor* AnchorActor);

    void RegisterWallCore(ETeam Team, ACoreWallActor* Core);
    void UnregisterWallCore(ETeam Team, ACoreWallActor* Core);
    ACoreWallActor* GetWallCore(ETeam Team) const;

    void UpdateReservationFor(AUnitCharacter* Unit);

    void ReleaseAllAttackersOfTarget(AUnitCharacter* Target);
    void ReleaseReservation(AUnitCharacter* Attacker, AUnitCharacter* Target);

    void NotifyUnitMoved(AUnitCharacter* Unit);

private:
    // ===============================
    // 팀 관리
    // ===============================
    TArray<TWeakObjectPtr<AUnitCharacter>>& GetTeamArray(ETeam Team);
    const TArray<TWeakObjectPtr<AUnitCharacter>>& GetTeamArrayConst(ETeam Team) const;

    AActor* GetEnemyBaseFor(const AUnitCharacter* Unit) const;

    const TArray<TWeakObjectPtr<AActor>>& GetTeamWallAnchorsConst(ETeam Team) const;
    float DistanceToNearestWallAnchorSq(const FVector& P, ETeam WallTeam) const;

    // ===============================
    // Grid (공간 분할)
    // ===============================
    FIntPoint WorldToCell2D(const FVector& P) const;

    TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>>& GetGrid(ETeam Team);
    const TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>>& GetGridConst(ETeam Team) const;

    void UpdateUnitCell(AUnitCharacter* Unit);
    void RemoveUnitFromGrid(AUnitCharacter* Unit);

    void GatherEnemyCandidates(
        AUnitCharacter* Attacker,
        TArray<AUnitCharacter*>& OutCandidates
    ) const;

    AActor* GetNearestWallAnchorTo(const FVector& From, ETeam Team) const;

    AUnitCharacter* FindBestTargetWithFreeSlot_Grid(
        AUnitCharacter* Attacker,
        float Now,
        const TArray<AUnitCharacter*>& Candidates
    ) const;

    bool TryReserve(AUnitCharacter* Attacker, AUnitCharacter* Target, float Now);

    // ===============================
    // Valkyrie (플레이어) 우선 타겟팅
    // ===============================
    void CacheValkyrieIfNeeded();
    bool IsValkyrieTargetableBy(const AUnitCharacter* Attacker) const;

    bool IsCurrentTargetValkyrie(const AUnitCharacter* Attacker) const;
    bool TryReserveValkyrieFirst(AUnitCharacter* Attacker, float Now);
    void ReleaseValkyrieReservationIfTooFar(AUnitCharacter* Attacker, float Now);

    // ===============================
    // Cleanup
    // ===============================
    void CleanupInvalidReferences();
    void MarkCleanupDirty();
    void MaybeCleanup(float Now);
    void RequestCleanupFromTimer();   // 타이머 콜백 (실제 Cleanup 실행 X)

    void NotifyTargetAssigned(AUnitCharacter* Unit, AActor* NewTarget);

    // ===============================
    // Debug
    // ===============================
    void DrawCellDebug(const FIntPoint& Cell, FColor Color, float Duration);

public:
    UFUNCTION(BlueprintCallable)
    void DrawAllGridDebug();

    // ===============================
    // Tuning
    // ===============================
    UPROPERTY(EditAnywhere, Category = "Tuning")
    float UpdateInterval = 0.25f;

    UPROPERTY(EditAnywhere, Category = "Tuning")
    float SearchRadius = 2000.f;

    UPROPERTY(EditAnywhere, Category = "Tuning")
    float NearWallDistance = 900.f;

    UPROPERTY(EditAnywhere, Category = "Tuning")
    float NoSlotGraceSeconds = 0.6f;

    // Grid
    UPROPERTY(EditAnywhere, Category = "BD|Grid")
    float GridCellSize = 1000.f;

    UPROPERTY(EditAnywhere, Category = "BD|Grid")
    int32 MaxCellRadius = 2;

    UPROPERTY(EditAnywhere, Category = "BD|Grid")
    int32 CandidateCap = 48;

    // Cleanup
    UPROPERTY(EditAnywhere, Category = "BD|Perf")
    float CleanupInterval = 1.0f;

    // Valkyrie
    UPROPERTY(EditAnywhere, Category = "Tuning|Valkyrie")
    bool bEnableValkyriePriority = true;

    /** TeamB 유닛이 발키리를 타겟팅 시도하는 최대 거리 */
    UPROPERTY(EditAnywhere, Category = "Tuning|Valkyrie")
    float ValkyrieMaxTargetDistance = 2000.f;

    /** 발키리 타겟 유지/해제 거리(이 값 초과 시 해제). Max보다 약간 크게 추천 */
    UPROPERTY(EditAnywhere, Category = "Tuning|Valkyrie")
    float ValkyrieReleaseDistance = 2100.f;

private:
    // ===============================
    // 팀 데이터
    // ===============================
    UPROPERTY()
    TArray<TWeakObjectPtr<AUnitCharacter>> TeamAUnits;

    UPROPERTY()
    TArray<TWeakObjectPtr<AUnitCharacter>> TeamBUnits;

    UPROPERTY()
    TWeakObjectPtr<AActor> TeamABase;

    UPROPERTY()
    TWeakObjectPtr<AActor> TeamBBase;

    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> TeamAWallAnchors;

    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> TeamBWallAnchors;

    UPROPERTY()
    TMap<ETeam, TWeakObjectPtr<ACoreWallActor>> TeamWallCores;

    // ===============================
    // Grid 데이터
    // ===============================
    TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>> GridA;
    TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>> GridB;

    UPROPERTY()
    TMap<TWeakObjectPtr<AUnitCharacter>, FIntPoint> UnitToCell;

    // ===============================
    // Valkyrie 캐시 / 상태
    // ===============================
    UPROPERTY()
    TWeakObjectPtr<AValkyrieCharacter> Valkyrie;

    // ===============================
    // Cleanup 상태
    // ===============================
    float LastCleanupTime = -FLT_MAX;
    bool bCleanupDirty = true;
    bool bCleanupInProgress = false;

    /** 타이머 핸들 (Cleanup 요청용) */
    FTimerHandle CleanupRequestTimerHandle;
};
