// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameFramework/Actor.h"
#include "GameSystem/Library/GameBaseLibrary.h"
#include "Data/Pool/PoolTypes.h"
#include "Data/Enums.h"
#include "Interface/ObjectPool/ObjectPoolInterface.h"
#include "ObjectPoolSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class VALKYRIEWAR_API UObjectPoolSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
public:
	template<typename T>
	void InitPool(EPoolTypes InType, TSubclassOf<AActor> InClass, int32 InMaxSize);

	template<typename T>
	T* Get(EPoolTypes InType, TSubclassOf<AActor> InClass, FVector InLocation, FRotator InRotation);

	template<typename T>
	void Release(EPoolTypes InType, T* InActor);

private:
	// 풀을 생성하는 함수
	template<typename T>
	void CreateObjectPool(EPoolTypes InType, TSubclassOf<AActor> InClass);

	// 풀을 가져오는 함수
	template<typename T>
	TPoolData<T>* GetObjectPool(EPoolTypes InType, TSubclassOf<AActor> InClass);

private:
	TMap<EPoolTypes, TUniquePtr<IPoolData>> ObjectPoolMap;

};


template<typename T>
inline void UObjectPoolSubsystem::InitPool(EPoolTypes InType, TSubclassOf<AActor> InClass, int32 InMaxSize)
{
#pragma region 유효성 검사
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[Subsystem(Spawn)] 월드가 없습니다"));
		return;
	}
	if (!UGameBaseLibrary::CheckClassImplements(InClass, UObjectPoolInterface::StaticClass()))
	{
		return;
	}
#pragma endregion

	CreateObjectPool<T>(InType, InClass);
	TPoolData<T>* PoolData = GetObjectPool<T>(InType, InClass);

	if (PoolData)
	{
		PoolData->DataInstance.Reserve(InMaxSize);

		UE_LOG(LogTemp, Warning, TEXT("[InitPool] %s Pool Reserved. Num: %d, Max(Capacity): %d, Memory: %d Bytes"),
			*InClass->GetName(), PoolData->DataInstance.Num(), PoolData->DataInstance.Max(), PoolData->DataInstance.GetAllocatedSize());
	}
}

template<typename T>
inline T* UObjectPoolSubsystem::Get(EPoolTypes InType, TSubclassOf<AActor> InClass, FVector InLocation, FRotator InRotation)
{
#pragma region 유효성 검사
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[Subsystem(Spawn)] 월드가 없습니다"));
		return nullptr;
	}
	if (!UGameBaseLibrary::CheckClassImplements(InClass, UObjectPoolInterface::StaticClass()))
	{
		return nullptr;
	}
#pragma endregion

	TPoolData<T>* PoolData = GetObjectPool<T>(InType, InClass);

	T* SpawnedActor = nullptr;	
	if (PoolData && PoolData->NumInstance() > 0)
	{
		SpawnedActor = PoolData->PopInstance();
		if (SpawnedActor)
		{
			if (SpawnedActor && SpawnedActor->GetClass()->ImplementsInterface(UObjectPoolInterface::StaticClass()))
			{
				// 인터페이스 콜
				IObjectPoolInterface::Execute_OnGet(SpawnedActor);
				// Spawn 액터 세팅
				SpawnedActor->SetActorHiddenInGame(false);
				SpawnedActor->SetActorEnableCollision(true);
				SpawnedActor->SetActorTickEnabled(true);
				SpawnedActor->SetActorLocation(InLocation);
				SpawnedActor->SetActorRotation(InRotation);
				UE_LOG(LogTemp, Log, TEXT("[Subsystem(Spawn)] 풀에서 재사용"));
			}
		}
	}

	if (!SpawnedActor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = nullptr;
		SpawnParams.ObjectFlags = RF_Transient;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnedActor = World->SpawnActor<T>(
			InClass,
			InLocation,
			InRotation,
			SpawnParams
		);

		if (SpawnedActor && SpawnedActor->GetClass()->ImplementsInterface(UObjectPoolInterface::StaticClass()))
		{
			UE_LOG(LogTemp, Log, TEXT("[Subsystem(Spawn)] 새로 생성"));
			IObjectPoolInterface::Execute_OnGet(SpawnedActor);

			SpawnedActor->SetActorHiddenInGame(false);
			SpawnedActor->SetActorEnableCollision(true);
			SpawnedActor->SetActorTickEnabled(true);
			SpawnedActor->SetActorLocation(InLocation);
			SpawnedActor->SetActorRotation(InRotation);
		}
#if WITH_EDITOR
		if (SpawnedActor)
			SpawnedActor->SetFolderPath(FName("Pool"));
#endif
	}
	UE_LOG(LogTemp, Warning, TEXT("[InitPool] %s Pool Reserved. Num: %d, Max(Capacity): %d, Memory: %d Bytes"),
		*InClass->GetName(), PoolData->DataInstance.Num(), PoolData->DataInstance.Max(), PoolData->DataInstance.GetAllocatedSize());
	return SpawnedActor;
}

template<typename T>
inline void UObjectPoolSubsystem::Release(EPoolTypes InType, T* InActor)
{
#pragma region 유효성 검사
	if (!InActor)
	{
		UE_LOG(LogTemp, Error, TEXT("[Subsystem(Return)] 스폰할 액터가 없습니다"));
		return;
	}
	TPoolData<T>* PoolData = GetObjectPool<T>(InType, InActor->GetClass());
	if (!PoolData)
	{
		UE_LOG(LogTemp, Error, TEXT("[Subsystem(Return)] PoolData가 없습니다"));
		InActor->Destroy();
		return;		
	}
#pragma endregion

	UE_LOG(LogTemp, Log, TEXT("[Subsystem(Return)] 풀로 돌아감"));
	if (InActor && InActor->GetClass()->ImplementsInterface(UObjectPoolInterface::StaticClass()))
	{
		IObjectPoolInterface::Execute_OnRelease(InActor);
		// Return 액터 세팅
		InActor->SetActorHiddenInGame(true);
		InActor->SetActorEnableCollision(false);
		InActor->SetActorTickEnabled(false);
		InActor->SetActorLocation(FVector::ZeroVector);

		if (PoolData && !PoolData->DataInstance.Contains(InActor))
		{
			PoolData->AddInstance(InActor);
		}
	}
}

template<typename T>
inline void UObjectPoolSubsystem::CreateObjectPool(EPoolTypes InType, TSubclassOf<AActor> InClass)
{
	if (!InClass)
		return;

	if (!ObjectPoolMap.Contains(InType))
		ObjectPoolMap.Add(InType, MakeUnique<TPoolData<T>>());

	return;
}

template<typename T>
inline TPoolData<T>* UObjectPoolSubsystem::GetObjectPool(EPoolTypes InType, TSubclassOf<AActor> InClass)
{
	if (!InClass)
		return nullptr;
	if (!ObjectPoolMap.Contains(InType))
		return nullptr;

	return static_cast<TPoolData<T>*>(ObjectPoolMap[InType].Get());
}

