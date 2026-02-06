#pragma once

#include "CoreMinimal.h"

struct IPoolData
{
	virtual ~IPoolData() {};
};

template<typename T>
struct TPoolData : public IPoolData
{
	TArray<TObjectPtr<T>> DataInstance;

	// 인스턴스 추가
	inline void AddInstance(T* Instance)
	{
		if (!IsValid(Instance))
			return;

		DataInstance.Add(Instance);
	}

	inline UClass* GetInstanceClass() { return T::StaticClass(); }

	inline T* PopInstance()
	{
		if (DataInstance.IsEmpty() || DataInstance.Num() == 0)
			return nullptr;
		T* PopInstance = DataInstance.Pop();
		return IsValid(PopInstance) ? PopInstance : nullptr;		
	}

	// 인스턴스 수 확인
	inline int32 NumInstance() { return DataInstance.Num(); }
};
