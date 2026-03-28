// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "AbilitySystemComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "GameSystem/Instance/Game/DataManager.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Instance/World/ObjectPoolSubsystem.h"

#include "GameSystem/Library/GameBaseLibrary.h"

#include "GameSystem/Base/BaseGameplayAbility.h"
#include "GameSystem/Base/BaseProjectile.h"
#include "GameSystem/Base/BaseAnimInstance.h"
#include "GameSystem/State/Game/BattleGameState.h"

#include "Kismet/GameplayStatics.h"

#include "Data/Module/ItemModule.h"
#include "Data/Module/AttackModule.h"
#include "Data/Game/AttackData.h"
#include "Data/Game/SkillData.h"
#include "Data/Attribute/StatAttributeSet.h"
#include "Data/Table/GameData/ItemDataRow.h"

#include "Object/Character/Valkyrie/Animation/AnimNotifyState/ANS_ComboWindow.h"
#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"
#include "AIController.h"


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

	PrimaryActorTick.bCanEverTick = true;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	//EventSystem
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Valkyrie.OnUseAttack.AddDynamic(this, &AValkyrieCharacter::ExecuteAttack);
		EventSystem->Valkyrie.OnUseSkill.AddDynamic(this, &AValkyrieCharacter::ExecuteSkill);
	}
}

void AValkyrieCharacter::ResetCombo()
{
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
					if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
					{
						UBlendSpace* NewBS = GameManager->GetValkyrieBlendSpace(EquippedWeapon->GetWeaponType());
						if (NewBS)
							LocomotionBS = NewBS;
					}
					UpdateWeaponMesh();

					if ( FProjectileDataRow* ProjectileData = AttackData->GetProjectileData())
					{
						if (UObjectPoolSubsystem* Pool = UGameBaseLibrary::GetObjectPoolSystem(this))
						{
							Pool->InitPool<ABaseProjectile>(ProjectileData->EPoolTypes, ProjectileData->SpawnObject.LoadSynchronous(), 5);
						}
					}
				}
			}
		}
	}
}
#endif

void AValkyrieCharacter::InitHpBarWidget()
{
	BroadcastHpChanged();
}

void AValkyrieCharacter::BeginPlay()
{
	Super::BeginPlay();

	BindAttributeDelegates();
}

void AValkyrieCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//이거는 캐릭터 정보창에서 장비를 장착해서 변경되었을때, 해당 캐릭터가 로비 캐릭터일 때만 사용
//데이터 업데이트 및 로비에배치된 캐릭터 무기 변경
void AValkyrieCharacter::EquipWeapon(UItemData* InWeapon)
{
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		if(InWeapon)
			EquippedWeapon = InWeapon;
		else
			EquippedWeapon = Data->GetEquippedItem(EEquipGroup::Weapon);

		if (EquippedWeapon && EquippedWeapon->GetItemGroup() != EItemGroup::Equip)
		{
			//장비 아니면 없애버림
			EquippedWeapon = nullptr;
		}
		if (EquippedWeapon && DataManager->GetAttackModule())
		{
			AttackData = DataManager->GetAttackModule()->GetAttackData(EquippedWeapon->GetAttackID());;
		}

		if (EquippedWeapon)
		{
			if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
			{
				UBlendSpace* NewBS = GameManager->GetValkyrieBlendSpace(EquippedWeapon->GetWeaponType());
				if (NewBS)
					LocomotionBS = NewBS;

				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					if (UBaseAnimInstance* BaseInstance = Cast<UBaseAnimInstance>(AnimInstance))
					{
						BaseInstance->SetInstacne(LocomotionBS, this);
					}
				}
			}
		}
	}

	UpdateWeaponMesh();	
}

void AValkyrieCharacter::UpdateWeaponMesh()
{
	if (!EquippedWeapon) return;
	if (!AttackData) return;
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
}

void AValkyrieCharacter::BindAttributeDelegates()
{
	if (!AbilitySystemComponent || !StatAttributeSet)
	{
		return;
	}

	UnbindAttributeDelegates();

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(StatAttributeSet->GetHealthAttribute())
		.AddUObject(this, &AValkyrieCharacter::HandleHealthChanged);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(StatAttributeSet->GetMaxHealthAttribute())
		.AddUObject(this, &AValkyrieCharacter::HandleHealthChanged);
}

void AValkyrieCharacter::UnbindAttributeDelegates()
{
	if (!AbilitySystemComponent || !StatAttributeSet)
	{
		return;
	}

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(StatAttributeSet->GetHealthAttribute())
		.RemoveAll(this);

	AbilitySystemComponent
		->GetGameplayAttributeValueChangeDelegate(StatAttributeSet->GetMaxHealthAttribute())
		.RemoveAll(this);
}

void AValkyrieCharacter::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	BroadcastHpChanged();
}

void AValkyrieCharacter::BroadcastHpChanged()
{
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Valkyrie.OnValkyrieHpChanged.Broadcast(StatAttributeSet->GetHealth(), StatAttributeSet->GetMaxHealth());
	}
}

void AValkyrieCharacter::ExecuteAttack()
{
	if (!AttackData) return;

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (AnimInst == nullptr)
	{
		return;
	}

	if (!AttackMontage)
		return;

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

			//AnimInst->Montage_JumpToSection(FName("Combo1"), AttackMontage);

			TArray<FGameplayCueData> Cues = AttackData->GetCue(EGameplayCueOrder::OnExecute);
			if (Cues.Num() > 0)
			{
				for (const FGameplayCueData& Cue : Cues)
				{
					FGameplayCueParameters CueParams;
					CueParams.Location = GetActorLocation() + Cue.Offset;

					AbilitySystemComponent->ExecuteGameplayCue(Cue.Tag, CueParams);
				}
			}
		}
	}

}

void AValkyrieCharacter::OnAttackNotify()
{
	if (!AttackData || !EquippedWeapon)
		return;

	//근접 공격 시 데미지 처리 시점
	//원거리 공격 시 발사체 생성 위치
	//무기 동작을 시작하는 지점 아님 -> 필요하면 별개로 만들어야함
	if (AttackData->GetAttackType() == EAttackType::Melee)
	{
		//TODO : 타겟 탐색
		// 타겟에게 데미지
		ApplyAttack(nullptr);
	}
	else
	{
		if (UObjectPoolSubsystem* Pool = UGameBaseLibrary::GetObjectPoolSystem(this))
		{
			FVector FirePos = GetActorLocation();
			if (EquippedWeapon->IsSkeletalWeapon())
			{
				if (!SkeletalWeapon->DoesSocketExist(FName("FirePos")))
				{
					UE_LOG(LogTemp, Error, TEXT("Weapon id %d doesn't have FirePos Socket!!"), EquippedWeapon->GetTableData().DataId);
					return;
				}
				FirePos = SkeletalWeapon->GetSocketLocation(FName("FirePos"));
			}
			else
			{
				if (StaticWeapon->DoesSocketExist(FName("FirePos")))
				{
					UE_LOG(LogTemp, Error, TEXT("Weapon id %d doesn't have FirePos Socket!!"), EquippedWeapon->GetTableData().DataId);
					return;
				}
			}
			ABaseProjectile* Projectile = Pool->Get<ABaseProjectile>(AttackData->GetProjectileData()->EPoolTypes, FirePos, GetActorRotation());
			Projectile->SetTeam(GetTeamType());
			float TotalAttack = StatAttributeSet->GetAttack();

			if (EquippedWeapon)
			{
				TotalAttack += EquippedWeapon->GetStat().Attack;
			}
			Projectile->SetAttack(TotalAttack);
			Projectile->SetData(AttackData->GetAbilityTag(), AttackSpec, *AttackData->GetProjectileData(), AttackData->GetCue(EGameplayCueOrder::InProjectile));
		}
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

		TArray<FGameplayCueData> Cues = UsingSkill->GetCue(EGameplayCueOrder::OnExecute);
		if (Cues.Num() > 0)
		{
			for (const FGameplayCueData Cue : Cues)
			{
				FGameplayCueParameters CueParams;
				CueParams.Location = GetActorLocation() + Cue.Offset;

				AbilitySystemComponent->ExecuteGameplayCue(Cue.Tag, CueParams);
			}
		}
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

ETeamType AValkyrieCharacter::GetTeamType() const
{
	return ETeamType::Ally;
}

void AValkyrieCharacter::OnDeath()
{
	ABattleGameState* State = GetWorld()->GetGameState<ABattleGameState>();
	if (State)
	{
		State->ChangeState(EBattleState::Defeat);
	}
}

void AValkyrieCharacter::SetData(UValkyrieData* InData)
{
	Data = InData;

	if (!StatAttributeSet)
	{
		StatAttributeSet = NewObject<UStatAttributeSet>(this);
		AbilitySystemComponent->AddAttributeSetSubobject<UStatAttributeSet>(StatAttributeSet);
	}

	FStatValueData Stat = Data->GetStat();

	StatAttributeSet->SetAttack(Stat.Attack);
	StatAttributeSet->SetDefense(Stat.Defence);
	StatAttributeSet->SetHealth(Stat.Health);
	StatAttributeSet->SetMaxHealth(Stat.Health);

	EquippedWeapon = Data->GetEquippedItem(EEquipGroup::Weapon);

	if (EquippedWeapon)
	{
		if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
		{
			UBlendSpace* NewBS = GameManager->GetValkyrieBlendSpace(EquippedWeapon->GetWeaponType());
			if (NewBS)
			{
				LocomotionBS = NewBS;

				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					if (UBaseAnimInstance* BaseInstance = Cast<UBaseAnimInstance>(AnimInstance))
					{
						BaseInstance->SetInstacne(LocomotionBS, this);
					}					
				}
			}
		}
	}

	//기본 무기에 따른 공격/스킬 적용
	AttackData = InData->GetAttackData();
	CreateAttackAbility();
	if (AttackData)
	{
		TSoftObjectPtr<UAnimMontage> MontageData = AttackData->GetAnimMontage();
		AttackMontage = MontageData.LoadSynchronous();
	}

	//데이터 풀 타입 있는지도 체크...
	if (FProjectileDataRow* ProjectileData = AttackData->GetProjectileData())
	{
		if (UObjectPoolSubsystem* Pool = UGameBaseLibrary::GetObjectPoolSystem(this))
		{			
			Pool->InitPool<ABaseProjectile>(ProjectileData->EPoolTypes, ProjectileData->SpawnObject.LoadSynchronous(), 5);
		}
	}	

	SkillDataList = InData->GetSkillData();
	CreateSkillAbility();

	UpdateWeaponMesh();
	//BroadcastHpChanged();
}
void AValkyrieCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ResetCombo();
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}
