// BattleDirectorSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/Enum/CommonEnums.h"
#include "BattleDirectorSubsystem.generated.h"

class AUnitCharacter;

UCLASS()
class VALKYRIEWAR_API UBattleDirectorSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    void RegisterUnit(AUnitCharacter* Unit);
    void UnregisterUnit(AUnitCharacter* Unit);

    void RegisterWallAnchor(ETeam Team, AActor* AnchorActor);
    void UnregisterWallAnchor(ETeam Team, AActor* AnchorActor);

    void UpdateReservationFor(AUnitCharacter* Unit);

    void ReleaseAllAttackersOfTarget(AUnitCharacter* Target);
    void ReleaseReservation(AUnitCharacter* Attacker, AUnitCharacter* Target);

    void NotifyUnitMoved(AUnitCharacter* Unit);

private:
    // ===== 기존 =====
    TArray<TWeakObjectPtr<AUnitCharacter>>& GetTeamArray(ETeam Team);
    const TArray<TWeakObjectPtr<AUnitCharacter>>& GetTeamArrayConst(ETeam Team) const;

    AActor* GetEnemyBaseFor(const AUnitCharacter* Unit) const;

    const TArray<TWeakObjectPtr<AActor>>& GetTeamWallAnchorsConst(ETeam Team) const;

    // (기존 의미 유지용 - 지금은 최적화 버전에서 거의 안 씀)
    float DistanceToNearestWallAnchorSq(const FVector& P, ETeam WallTeam) const;

    // ===== 버전3(그리드) 추가 =====
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

    // 후보 선택 (TargetingPolicy 반영) - 이제 "그리드 후보"만 대상으로 평가
    AUnitCharacter* FindBestTargetWithFreeSlot_Grid(
        AUnitCharacter* Attacker,
        float Now,
        const TArray<AUnitCharacter*>& Candidates
    ) const;

    bool TryReserve(AUnitCharacter* Attacker, AUnitCharacter* Target, float Now);
    void CleanupInvalidReferences();

    // 청소를 유닛마다 하지 말고 주기적으로
    void MarkCleanupDirty();
    void MaybeCleanup(float Now);

    void NotifyTargetAssigned(AUnitCharacter* Unit, AActor* NewTarget);

public:
    // 매칭 갱신 주기(현재 BTService가 0.25s, 여기 값은 문서용)
    UPROPERTY(EditAnywhere, Category = "Tuning")
    float UpdateInterval = 0.25f;

    UPROPERTY(EditAnywhere, Category = "Tuning")
    float SearchRadius = 2000.f;

    UPROPERTY(EditAnywhere, Category = "Tuning")
    float NearWallDistance = 900.f;

    UPROPERTY(EditAnywhere, Category = "Tuning")
    float NoSlotGraceSeconds = 0.6f;

    // ===== 버전3 튜닝 =====
    // 그리드 셀 크기(보통 SearchRadius의 0.5~1.0배 권장)
    UPROPERTY(EditAnywhere, Category = "BD|Grid")
    float GridCellSize = 1000.f;

    // 후보를 모을 때 셀 반경 상한 (너무 크게 잡으면 버전3 의미가 약해짐)
    UPROPERTY(EditAnywhere, Category = "BD|Grid")
    int32 MaxCellRadius = 2;

    // 후보 상한(G cap). 이 값이 사실상 G를 상수로 “봉인”해줌
    UPROPERTY(EditAnywhere, Category = "BD|Grid")
    int32 CandidateCap = 48;

    // ===== 청소 주기 =====
    UPROPERTY(EditAnywhere, Category = "BD|Perf")
    float CleanupInterval = 1.0f;

private:
    // 팀별 유닛 목록(유지: 디버그/폴백/일반 관리)
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

    // ===== 그리드 저장소 =====
    // TeamA / TeamB
    TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>> GridA;
    TMap<FIntPoint, TArray<TWeakObjectPtr<AUnitCharacter>>> GridB;

    // 유닛 -> 현재 셀(역인덱스)
    UPROPERTY()
    TMap<TWeakObjectPtr<AUnitCharacter>, FIntPoint> UnitToCell;

    float LastCleanupTime = -FLT_MAX;
    bool bCleanupDirty = true;
};
