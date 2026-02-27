#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/Enum/CommonEnums.h"
#include "BattleDirectorSubsystem.generated.h"

class AUnitCharacter;

/**
 * BattleDirectorSubsystem
 *
 * 월드 단위 전투 오케스트레이터.
 * 팀별 유닛을 관리하고, 그리드 기반 후보 탐색을 통해
 * 타겟 예약(Engagement Slot)과 전투 매칭을 조율한다.
 *
 * - 팀/유닛 등록 및 해제 관리
 * - 공간 분할(Grid) 기반 적 후보 수집
 * - 타겟 선택 및 Engagement Slot 예약 처리
 * - 주기적 Cleanup을 통한 데이터 무결성 유지
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

    // ===============================
    // Grid 데이터
    // ===============================
    TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>> GridA;
    TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>> GridB;

    UPROPERTY()
    TMap<TWeakObjectPtr<AUnitCharacter>, FIntPoint> UnitToCell;

    // ===============================
    // Cleanup 상태
    // ===============================
    float LastCleanupTime = -FLT_MAX;
    bool bCleanupDirty = true;
    bool bCleanupInProgress = false;

    /** 타이머 핸들 (Cleanup 요청용) */
    FTimerHandle CleanupRequestTimerHandle;
};
