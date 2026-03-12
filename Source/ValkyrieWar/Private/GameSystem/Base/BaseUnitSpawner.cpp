#include "GameSystem/Base/BaseUnitSpawner.h"

#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"
#include "GameSystem/Instance/World/SpawnUpgradeSubsystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/Game/DataManager.h"
#include "Data/Module/UnitModule.h"
#include "Object/Character/Unit/UnitCharacter.h"

ABaseUnitSpawner::ABaseUnitSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABaseUnitSpawner::SetSpawnUnitData(int32 InDataId)
{
	PoolEntry.UnitDataId = InDataId;

	if (UUnitModule* UnitModule = GetGameInstance()->GetSubsystem<UDataManager>()->GetUnitModule())
	{
		PoolEntry.UnitClass = UnitModule->GetSpawnUnitClass(InDataId);
	}
	else
	{
		PoolEntry.UnitClass = nullptr;
	}
}

void ABaseUnitSpawner::BeginPlay()
{
	Super::BeginPlay();

	// 보험용 Cleanup
	if (CleanupInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			CleanupHandle,
			this,
			&ABaseUnitSpawner::HandleCleanupTick,
			CleanupInterval,
			true
		);
	}

	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnSpawnUnitDataReady.AddUniqueDynamic(this, &ABaseUnitSpawner::RequestUnitDataToSpawn);
		if (Team == ETeamType::Ally)
		{
			// SpawnUpgradeSubsystem에 바인딩 (승인된 업그레이드만 받음)
			WorldEventSystem->Battle.OnSpawnLevelUpgraded.AddUniqueDynamic(this, &ABaseUnitSpawner::HandleSpawnLevelUpgraded);
		}
		else
		{
			// 현재 테스트용으로 구현
			// TODO: 적 병종별로, 스테이지별로 동적 세팅되도록 수정
			SetSpawnCount(2);

		}
	}
}

void ABaseUnitSpawner::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	StopSpawning();
	GetWorldTimerManager().ClearTimer(CleanupHandle);

	if (Team == ETeamType::Ally)
	{
		if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
		{
			WorldEventSystem->Battle.OnSpawnUnitDataReady.RemoveDynamic(this, &ABaseUnitSpawner::RequestUnitDataToSpawn);
			WorldEventSystem->Battle.OnSpawnLevelUpgraded.RemoveDynamic(this, &ABaseUnitSpawner::HandleSpawnLevelUpgraded);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void ABaseUnitSpawner::StartSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTickHandle);

	if (!TryInitPool()) return;

	BP_OnSpawningStarted();

	const float Interval = FMath::Max(0.01f, SpawnInterval);
	GetWorldTimerManager().SetTimer(
		SpawnTickHandle,
		this,
		&ABaseUnitSpawner::HandleSpawnTick,
		Interval,
		true
	);
}

void ABaseUnitSpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTickHandle);

	// 전투 종료 시 모두 회수
	if (UObjectPoolSubsystem* Pool = GetPool())
	{
		for (TWeakObjectPtr<AUnitCharacter>& W : AliveUnits)
		{
			if (AUnitCharacter* U = W.Get())
			{
				if (U->IsInPool()) continue;

				if (U->GetMyPoolType() != EPoolTypes::None)
				{
					Pool->Release<AUnitCharacter>(U->GetMyPoolType(), U);
				}
				else
				{
					U->Destroy();
				}
			}
		}
	}

	AliveUnits.Reset();
}

void ABaseUnitSpawner::SetSpawnCount(int32 InSpawnCount)
{
	SpawnCount = FMath::Max(0, InSpawnCount);
}

void ABaseUnitSpawner::NotifyUnitReleased(AUnitCharacter* Unit)
{
	UnregisterAlive(Unit);
}

bool ABaseUnitSpawner::TryInitPool()
{
	if (bPoolInited) return true;

	UObjectPoolSubsystem* Pool = GetPool();
	if (!Pool)
	{
		UE_LOG(LogTemp, Error, TEXT("[UnitSpawner] Pool subsystem not found"));
		return false;
	}

	if (PoolEntry.PoolType == EPoolTypes::None || !PoolEntry.UnitClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[UnitSpawner] Invalid PoolEntry (PoolType/UnitClass)"));
		return false;
	}

	Pool->InitPool<AUnitCharacter>(PoolEntry.PoolType, PoolEntry.UnitClass, PoolEntry.ReserveSize);
	bPoolInited = true;
	return true;
}

void ABaseUnitSpawner::HandleSpawnTick()
{
	if (SpawnCount <= 0) return;
	if (!TryInitPool()) return;

	CompactAliveUnits();

	int32 Budget = FMath::Min(SpawnCount, MaxSpawnPerTick);
	while (Budget-- > 0)
	{
		SpawnOne();
	}
}

void ABaseUnitSpawner::SpawnOne()
{
	UObjectPoolSubsystem* Pool = GetPool();
	if (!Pool) return;

	// 확장 포인트: 나중에 Subsystem 붙이면 여기 결과만 바뀌게 만들기
	const int32 DataId = ResolveSpawnDataId();
	const TSubclassOf<AUnitCharacter> SpawnCls = ResolveSpawnClass(DataId);
	if (!SpawnCls)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UnitSpawner] SpawnClass is null (DataId=%d)"), DataId);
		return;
	}

	const FTransform SpawnTM = MakeSpawnTransform();

	AUnitCharacter* Unit = Pool->Get<AUnitCharacter>(
		PoolEntry.PoolType,
		SpawnCls,
		SpawnTM.GetLocation(),
		SpawnTM.Rotator()
	);

	if (!Unit) return;

	Unit->SetOwnerSpawner(this);
	Unit->SetPoolType(PoolEntry.PoolType);

	// 테스트용: UnitDataId에 해당하는 데이터 주입
	if (UUnitData* Data = ResolveUnitDataObject(DataId))
	{
		Unit->SetData(Data);
	}

	RegisterAlive(Unit);
	BP_OnUnitSpawned(Unit, PoolEntry.PoolType);
}

UUnitData* ABaseUnitSpawner::ResolveUnitDataObject(int32 DataId) const
{
	if (DataId <= 0) return nullptr;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDataManager* DataManager = GI->GetSubsystem<UDataManager>())
		{
			if (DataManager->GetUnitModule())
			{
				return DataManager->GetUnitModule()->GetUnitDataById(DataId);
			}
		}
	}
	return nullptr;
}

int32 ABaseUnitSpawner::ResolveSpawnDataId() const
{
	// 현재 1차 구현: 스포너에 셋팅된 테스트 DataId 그대로 사용
	return PoolEntry.UnitDataId;
}

TSubclassOf<AUnitCharacter> ABaseUnitSpawner::ResolveSpawnClass(int32 /*ResolvedDataId*/) const
{
	// 현재 1차 구현: 스포너에 셋팅된 테스트 클래스 그대로 사용
	return PoolEntry.UnitClass;
}

void ABaseUnitSpawner::HandleCleanupTick()
{
	CompactAliveUnits();
}

void ABaseUnitSpawner::HandleUnitDestroyed(AActor* DestroyedActor)
{
	UnregisterAlive(DestroyedActor);
}

UObjectPoolSubsystem* ABaseUnitSpawner::GetPool() const
{
	if (UWorld* World = GetWorld())
	{
		return World->GetSubsystem<UObjectPoolSubsystem>();
	}
	return nullptr;
}

void ABaseUnitSpawner::RequestUnitDataToSpawn()
{
	if (UWorld* World = GetWorld())
	{
		if (USpawnUpgradeSubsystem* SpawnUpgradeSubsystem = World->GetSubsystem<USpawnUpgradeSubsystem>())
		{
			SpawnUpgradeSubsystem->RequestDataId(this);

			if (PoolEntry.UnitDataId <= 0 || !PoolEntry.UnitClass)
			{
				return;
			}

			if (bAutoStart)
			{
				StartSpawning();
			}
		}
	}
}

void ABaseUnitSpawner::HandleSpawnLevelUpgraded(int32 InFamilyId, int32 OldLevel, int32 NewLevel)
{
	if (InFamilyId != PoolEntry.UnitDataId) return;

	// 규칙: Lv == SpawnCount
	SetSpawnCount(NewLevel);
}

FTransform ABaseUnitSpawner::MakeSpawnTransform() const
{
	const FTransform BaseTM = SpawnPointActor ? SpawnPointActor->GetActorTransform() : GetActorTransform();

	if (SpawnHalfExtent.X <= 0.f && SpawnHalfExtent.Y <= 0.f)
	{
		return BaseTM;
	}

	const float DX = FMath::FRandRange(-SpawnHalfExtent.X, SpawnHalfExtent.X);
	const float DY = FMath::FRandRange(-SpawnHalfExtent.Y, SpawnHalfExtent.Y);

	const FVector Forward = BaseTM.GetRotation().GetForwardVector();
	const FVector Right = BaseTM.GetRotation().GetRightVector();

	FVector Loc = BaseTM.GetLocation();
	Loc += Forward * DX;
	Loc += Right * DY;

	FTransform Out = BaseTM;
	Out.SetLocation(Loc);
	return Out;
}

void ABaseUnitSpawner::RegisterAlive(AUnitCharacter* Unit)
{
	if (!Unit) return;

	for (const TWeakObjectPtr<AUnitCharacter>& W : AliveUnits)
	{
		if (W.Get() == Unit) return;
	}

	AliveUnits.Add(Unit);

	Unit->OnDestroyed.RemoveAll(this);
	Unit->OnDestroyed.AddDynamic(this, &ABaseUnitSpawner::HandleUnitDestroyed);
}

void ABaseUnitSpawner::UnregisterAlive(AActor* UnitActor)
{
	if (!UnitActor) return;

	for (int32 i = AliveUnits.Num() - 1; i >= 0; --i)
	{
		AUnitCharacter* U = AliveUnits[i].Get();
		if (!U || U == UnitActor)
		{
			AliveUnits.RemoveAtSwap(i);
		}
	}
}

void ABaseUnitSpawner::CompactAliveUnits()
{
	for (int32 i = AliveUnits.Num() - 1; i >= 0; --i)
	{
		AUnitCharacter* Unit = AliveUnits[i].Get();
		if (!Unit || !IsValid(Unit) || Unit->IsInPool())
		{
			AliveUnits.RemoveAtSwap(i);
		}
	}
}
