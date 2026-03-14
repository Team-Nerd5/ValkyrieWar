// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

class IPoolData
{
public:
	virtual ~IPoolData() = default;
};

template<class T = UObject>
class TPoolData : public IPoolData, public FGCObject
{
public:
	TPoolData(TSubclassOf<class AActor> InClass);

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObjects(DataInstance);
		Collector.AddReferencedObject(SpawnClass);
	}
	virtual FString GetReferencerName() const override
	{
		return TEXT("TPoolData");
	}
	inline void AddInstance(T* Instance)
	{
		if (!IsValid(Instance))
			return;

		DataInstance.Add(Instance);
	}

	inline T* PopInstance()
	{
		if (DataInstance.IsEmpty() || DataInstance.Num() == 0)
			return nullptr;

		T* PopInstance = DataInstance.Pop(EAllowShrinking::No);
		return IsValid(PopInstance) ? PopInstance : nullptr;
	}

	inline int32 NumInstance() { return DataInstance.Num(); }
public:
	TObjectPtr<UClass> SpawnClass;

	TArray<TObjectPtr<T>> DataInstance;
};

template<class T>
inline TPoolData<T>::TPoolData(TSubclassOf<class AActor> InClass)
{
	SpawnClass = InClass.Get();
}
