// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Weapon/ValkyrieWeapon.h"
#include "Data/Game/ItemData.h"
#include "Data/Game/AttackData.h"

AValkyrieWeapon::AValkyrieWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	// 루트 컴포넌트 생성
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	// 스켈레탈 메쉬 컴포넌트 생성
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	// 스태틱 메쉬 컴포넌트 생성
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
}

void AValkyrieWeapon::SetWeaponMesh(UItemData* InItem, UAttackData* InAttackData)
{
	if (!InItem || !InAttackData) return;

	SkeletalMesh->SetSkeletalMesh(nullptr);
	StaticMesh->SetStaticMesh(nullptr);

	// InItem(UItemData)에서 메쉬 가져오기
	// SoftObjectPtr이니까 .LoadSynchronous()를 써주거나 타입을 맞춰줘야 함
	if (InItem->IsSkeletalWeapon())
	{
		SkeletalMesh->SetSkeletalMesh(InItem->GetSkeletalMesh().LoadSynchronous());
		SkeletalMesh->SetHiddenInGame(false);
		StaticMesh->SetHiddenInGame(true);
	}
	else
	{
		StaticMesh->SetStaticMesh(InItem->GetStaticMesh().LoadSynchronous());
		StaticMesh->SetHiddenInGame(false);
		SkeletalMesh->SetHiddenInGame(true);
	}

	UE_LOG(LogTemp, Warning, TEXT("⚔️ [무기] 메쉬 교체 완료: %s"), *InItem->GetName());
}

