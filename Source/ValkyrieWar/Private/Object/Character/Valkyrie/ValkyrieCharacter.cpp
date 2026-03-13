// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "Object/Weapon/Range/Projectile/ArrowStackComponent.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystemComponent.h"

#include "Components/DecalComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "GameSystem/Library/GameBaseLibrary.h"

#include "GameSystem/Base/BaseGameplayAbility.h"

#include "Kismet/GameplayStatics.h"

#include "Data/Module/ItemModule.h"
#include "Data/Module/AttackModule.h"
#include "Data/Game/AttackData.h"
#include "Data/Game/SkillData.h"
#include "Data/Attribute/StatAttributeSet.h"
#include "Data/Table/GameData/ItemDataRow.h"


#include "Object/Character/Valkyrie/Animation/AnimNotifyState/ANS_ComboWindow.h"
#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"
#include "Object/Weapon/Range/ValkyrieBow.h"

AValkyrieCharacter::AValkyrieCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	CurrentComboCount = 0;
	PrimaryActorTick.bCanEverTick = true;

	//EventSystem
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Valkyrie.OnUseAttack.AddDynamic(this, &AValkyrieCharacter::ExecuteAttack);
		EventSystem->Valkyrie.OnUseSkill.AddDynamic(this, &AValkyrieCharacter::ExecuteSkill);
	}
}

void AValkyrieCharacter::SetWeaponType(EWeaponAnimType InNewType)
{
	CurrentWeaponType = InNewType;
}

void AValkyrieCharacter::ResetCombo()
{
	CurrentComboCount = 0;
	bIsComboActive = false;
	bIsComboInputOn = false;
	bCanNextCombo = false;
	bIsInComboWindow = false;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

#if WITH_EDITOR
//테스트용 장비 변경 코드
void AValkyrieCharacter::ChangeWeapon(UItemData* InEquip)
{
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		if (InEquip)
		{
			EquippedWeapon = InEquip;
			if (UAttackModule* AttackModule = DataManager->GetAttackModule())
			{
				AttackData = AttackModule->GetAttackData(EquippedWeapon->GetAttackID());

				if (IsValid(AttackData))
				{
					TSoftClassPtr<UAnimInstance> AnimClass = AttackData->GetAnimInstance();
					if (AnimClass.IsValid())
					{
						UClass* NewAnimClass = AnimClass.LoadSynchronous();
					}

					UpdateWeaponMesh();
				}
			}
		}		
	}
}
#endif

void AValkyrieCharacter::BeginPlay()
{
	Super::BeginPlay();

	
}
void AValkyrieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//이거는 캐릭터 정보창에서 장비를 장착해서 변경되었을때, 해당 캐릭터가 로비 캐릭터일 때만 사용
//데이터 업데이트 및 로비에배치된 캐릭터 무기 변경
void AValkyrieCharacter::EquipWeapon(uint64 InValkyrieUID, uint64 InEquipUID)
{
	if (InValkyrieUID != Data->GetUID())
	{
		//장착한 캐릭터가 배치된 캐릭터가 아님
		return;
	}
	//TODO : Inventory에서 Get 해야함. 수정필요
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		if (UItemModule* ItemModule = DataManager->GetItemModule())
		{
			//장착한 장비
			EquippedWeapon = ItemModule->GetItem(InEquipUID);

			if (EquippedWeapon && EquippedWeapon->GetItemGroup() != EItemGroup::Equip)
			{
				//장비 아니면 없애버림
				EquippedWeapon = nullptr;
			}
			if (EquippedWeapon && DataManager->GetAttackModule())
			{
				AttackData = DataManager->GetAttackModule()->GetAttackData(EquippedWeapon->GetAttackID());;
			}
		}
	}

	UpdateWeaponMesh();	
}

void AValkyrieCharacter::UpdateWeaponMesh()
{
	if (!EquippedWeapon)
		return;

	if (EquippedWeapon->IsSkeletalWeapon())
	{
		if (StaticWeapon)
		{
			StaticWeapon->SetStaticMesh(nullptr);
		}
		if (SkeletalWeapon)
		{
			TSoftObjectPtr<USkeletalMesh> WeaponMeshData = EquippedWeapon->GetSkeletalMesh();
			SkeletalWeapon->SetSkeletalMeshAsset(WeaponMeshData.LoadSynchronous());

			FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
			SkeletalWeapon->AttachToComponent(GetMesh(), AttachRules, EquippedWeapon->GetTableData().SocketName);
			SkeletalWeapon->SetRelativeLocation(AttackData->GetLocationOffset());
			SkeletalWeapon->SetRelativeRotation(AttackData->GetRotatinOffset());
		}
	}
	else
	{
		if (SkeletalWeapon)
		{
			SkeletalWeapon->SetSkeletalMeshAsset(nullptr);
		}
		if (StaticWeapon)
		{
			TSoftObjectPtr<UStaticMesh> WeaponMeshData = EquippedWeapon->GetStaticMesh();
			StaticWeapon->SetStaticMesh(WeaponMeshData.LoadSynchronous());

			FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, true);
			StaticWeapon->AttachToComponent(GetMesh(), AttachRules, EquippedWeapon->GetTableData().SocketName);
			StaticWeapon->SetRelativeLocation(AttackData->GetLocationOffset());
			StaticWeapon->SetRelativeRotation(AttackData->GetRotatinOffset());
		}
	}

	//이건 또 뭐냐...
	//TArray<USceneComponent*> AttachedComponents;
	//GetMesh()->GetChildrenComponents(true, AttachedComponents);
	//for (USceneComponent* Child : AttachedComponents)
	//{
	//	if (Child->ComponentHasTag(TEXT("EquippedWeaponMesh")))
	//	{
	//		Child->DestroyComponent();
	//	}
	//}
	//if (WeaponDataPtr->IsSkeletalWeapon()) // 스켈레탈 메시
	//{
	//	if (USkeletalMesh* SkelMesh = WeaponDataPtr->GetSkeletalMesh().LoadSynchronous())
	//	{
	//		// NewObject..??
	//		USkeletalMeshComponent* NewComp = NewObject<USkeletalMeshComponent>(this);
	//		NewComp->RegisterComponent();
	//		NewComp->SetSkeletalMesh(SkelMesh);
	//		NewComp->ComponentTags.Add(TEXT("EquippedWeaponMesh")); 
	//		NewComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

	//		NewComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
	//		NewComp->RegisterComponent();

	//		// 데이터 테이블 오프셋 적용
	//		NewComp->SetRelativeLocation(AttackDataPtr->GetLocationOffset());
	//		NewComp->SetRelativeRotation(AttackDataPtr->GetRotatinOffset());
	//	}
	//}
	//else // 스테틱
	//{
	//	if (UStaticMesh* StaticMesh = WeaponDataPtr->GetStaticMesh().LoadSynchronous())
	//	{
	//		UStaticMeshComponent* NewComp = NewObject<UStaticMeshComponent>(this);
	//		NewComp->RegisterComponent();
	//		NewComp->SetStaticMesh(StaticMesh);
	//		NewComp->ComponentTags.Add(TEXT("EquippedWeaponMesh"));
	//		NewComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); 

	//		NewComp->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("WeaponSocket"));
	//		NewComp->RegisterComponent();

	//		// 데이터 테이블 오프셋 적용
	//		NewComp->SetRelativeLocation(AttackDataPtr->GetLocationOffset());
	//		NewComp->SetRelativeRotation(AttackDataPtr->GetRotatinOffset());
	//	}
	//}
	
}

void AValkyrieCharacter::ExecuteAttack()
{
	if (!AttackData) return;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	TSoftObjectPtr<UAnimMontage> MontageData = AttackData->GetAnimMontage();
	if (!MontageData.IsValid())
		return;

	UAnimMontage* AttackMontage = MontageData.LoadSynchronous();

	if (AnimInst && AttackMontage)
	{
		if (!AnimInst->Montage_IsPlaying(AttackMontage))
		{
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->bOrientRotationToMovement = false;

			// 마우스/입력 방향으로 고개는 돌려주기
			FVector InputDir = GetCharacterMovement()->GetLastInputVector();
			if (!InputDir.IsNearlyZero()) { SetActorRotation(InputDir.Rotation()); }

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AValkyrieCharacter::OnAttackMontageEnded);
			AnimInst->Montage_Play(AttackMontage);
			AnimInst->Montage_SetEndDelegate(EndDelegate, AttackMontage);

			AnimInst->Montage_JumpToSection(FName("Combo1"), AttackMontage);

			CurrentComboCount = 1;
			bIsComboActive = true;
			bCanNextCombo = false;
			bIsComboInputOn = false;
		}
	}
	else
	{
		if (bIsComboActive && bCanNextCombo)
		{
			bIsComboInputOn = true;
		}
	}
}

void AValkyrieCharacter::OnAttackNotify()
{
	if (!AttackData)
		return;

	//근접 공격 시 데미지 처리 시점
	//원거리 공격 시 발사체 생성 위치
	//무기 동작을 시작하는 지점 아님 -> 필요하면 별개로 만들어야함
	if (AttackData->GetAttackType() == EAttackType::Melee)
	{
		//타겟에게 데미지
		ApplyAttack(nullptr);
	}
	else
	{
		//Projectile 생성 및 현재 Ability 주입
	}
	

	if (CurrentWeaponActor)
	{
		//사용 불가(오류나는 형태임)
		//CurrentWeaponActor->ExecuteWeaponAction(this, *ArrowClass);
	}
}

void AValkyrieCharacter::ExecuteSkill(int32 InSkillIndex)
{
	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst && AnimInst->IsAnyMontagePlaying()) return; // 중복실행 막기

	if (InSkillIndex >= SkillDataList.Num() || InSkillIndex < 0)
	{
		//스킬데이터에 없는 스킬
		return;
	}

	UsingSkillIndex = InSkillIndex;

	USkillData* UsingSkill = SkillDataList[UsingSkillIndex];
	UAnimMontage* SkillMontage = UsingSkill->GetMontage().LoadSynchronous();

	if (SkillMontage && AnimInst)
	{
		//GetCharacterMovement()->MaxWalkSpeed = 0.0f;
		//GetCharacterMovement()->StopMovementImmediately();

		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AValkyrieCharacter::OnSkillMontageEnded);

		AnimInst->Montage_Play(SkillMontage);
		AnimInst->Montage_SetEndDelegate(EndDelegate, SkillMontage);
	}
}

void AValkyrieCharacter::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
	ResetCombo();
	UE_LOG(LogTemp, Warning, TEXT("스킬 종료! 이제 다시 움직일 수 있음."));
}

void AValkyrieCharacter::OnSkillNotify()
{	
	//근접 공격 시 데미지 처리
	//원거리 공격 시 발사체 생성 위치
	//무기 동작을 시작하는 지점 아님
	//TODO : 원거리는 발사체 생성해서 어빌리티 넘겨줘야함
	if (SkillDataList[UsingSkillIndex]->GetAttackType() == EAttackType::Melee)
	{
		ApplySkill(UsingSkillIndex, nullptr);

		UsingSkillIndex = 0;
	}
	else
	{

	}

	//TArray<AActor*> FoundEnemies;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AUnitCharacter::StaticClass(), FoundEnemies);

	//for (AActor* Actor : FoundEnemies)
	//{
	//	if (Actor == this) continue;
	//	if (UArrowStackComponent* StackComp = Actor->FindComponentByClass<UArrowStackComponent>())
	//	{
	//		if (StackComp->StackingArrows.Num() > 0)
	//		{
	//			StackComp->PullIt(CachedSkillDamage);
	//		}
	//	}
	//}
}

void AValkyrieCharacter::SetData(UValkyrieData* InData)
{
	Data = InData;

	StatAttribute->SetAttack(Data->GetStat(EStatusType::Attack));
	StatAttribute->SetDefense(Data->GetStat(EStatusType::Defence));
	StatAttribute->SetHealth(Data->GetStat(EStatusType::Health));
	StatAttribute->SetMaxHealth(Data->GetStat(EStatusType::Health));

	//기본 무기에 따른 공격/스킬 적용
	AttackData = InData->GetAttackData();
	CreateAttackAbility();

	SkillDataList = InData->GetSkillData();
	CreateSkillAbility();

	//캐릭터 블루프린트 생성 후 무기 세팅
	//장비 음..
}
void AValkyrieCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ResetCombo();
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}
void AValkyrieCharacter::BeginComboWindow()
{
	bIsInComboWindow = true;
	bCanNextCombo = true;
}

void AValkyrieCharacter::EndComboWindow(FName NextSectionName)
{
	if (!AttackData) return;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	auto MotageData = AttackData->GetAnimMontage();

	if (!MotageData.IsValid())
		return;

	UAnimMontage* AttackMontage = MotageData.LoadSynchronous();

	if (!AnimInst || !AttackMontage) return;
	if (bIsComboInputOn)
	{
		bIsComboInputOn = false;
		CurrentComboCount++;
		FVector InputDir = GetCharacterMovement()->GetLastInputVector();
		if (!InputDir.IsNearlyZero())
		{
			SetActorRotation(InputDir.Rotation());
		}
		if (CurrentComboCount > 1)
		{
			GetCharacterMovement()->MaxWalkSpeed = 0.f;
			GetCharacterMovement()->StopMovementImmediately();
			GetCharacterMovement()->bOrientRotationToMovement = false;
		}

		AnimInst->Montage_JumpToSection(NextSectionName, AttackMontage);

		if (NextSectionName == FName("Finish"))
		{
			bIsComboActive = false;
		}
	}
	// 예약된 입력이 없으면 콤보 종료
	else
	{
		CurrentComboCount = 0;
		bIsComboActive = false;
		ResetCombo();
	}
}

//AActor* AValkyrieCharacter::FindTarget()
//{
//
//	return nullptr;
//}
