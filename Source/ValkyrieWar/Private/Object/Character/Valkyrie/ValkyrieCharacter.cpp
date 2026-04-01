// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "Object/Character/Valkyrie/Controller/ValkyrieAIController.h"
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
#include "GameSystem/State/Player/ValkyriePlayerState.h"

#include "Kismet/GameplayStatics.h"

#include "Data/Module/ItemModule.h"
#include "Data/Module/AttackModule.h"
#include "Data/Game/AttackData.h"
#include "Data/Game/SkillData.h"
#include "Data/Attribute/StatAttributeSet.h"
#include "Data/Table/GameData/ItemDataRow.h"

#include "Object/Character/Valkyrie/Animation/AnimNotifyState/ANS_ComboWindow.h"
#include "Object/Character/Valkyrie/Controller/ValkyrieCharacterController.h"
#include "Object/Character/Unit/UnitCharacter.h"
#include "GameSystem/Base/BaseWall.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"
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

	PrimaryActorTick.bCanEverTick = false;
	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = AValkyrieAIController::StaticClass();

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

	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Battle.OnBattleModeChanged.AddDynamic(this, &AValkyrieCharacter::HandleControlModeChanged);
		if (AController* ValkyrieController = GetController())
		{
			if (AValkyriePlayerState* PS = ValkyrieController->GetPlayerState<AValkyriePlayerState>())
			{
				HandleControlModeChanged(PS->GetControlMode());
			}
		}
	}
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
			SkeletalWeapon->SetRelativeLocation(Data->GetCurrentWeaponOffset().LocationtionOffset);
			SkeletalWeapon->SetRelativeRotation(Data->GetCurrentWeaponOffset().RotateOffset);
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
			StaticWeapon->SetRelativeLocation(Data->GetCurrentWeaponOffset().LocationtionOffset);
			StaticWeapon->SetRelativeRotation(Data->GetCurrentWeaponOffset().RotateOffset);
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

void AValkyrieCharacter::SwitchToAIController()
{
	AController* CurrentController = GetController();

	if (AValkyrieCharacterController* PC = Cast<AValkyrieCharacterController>(CurrentController))
	{
		CachedPlayerController = PC;
	}

	if (!CachedAIController)
	{
		UClass* ControllerClassToSpawn = AIControllerClass;
		if (!ControllerClassToSpawn)
		{
			ControllerClassToSpawn = AValkyrieAIController::StaticClass();
		}

		CachedAIController = Cast<AValkyrieAIController>(
			GetWorld()->SpawnActor<AController>(ControllerClassToSpawn)
		);
	}

	if (CachedAIController)
	{
		if (CurrentController)
		{
			CurrentController->UnPossess();
		}

		CachedAIController->Possess(this);
	}
}

void AValkyrieCharacter::SwitchToPlayerController()
{
	if (!CachedPlayerController.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Valkyrie] No cached PlayerController"));
		return;
	}

	AController* CurrentController = GetController();

	if (CurrentController)
	{
		CurrentController->UnPossess();
	}

	// 플레이어가 다시 점유
	CachedPlayerController->Possess(this);
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

			FVector InputDir = GetCharacterMovement()->GetLastInputVector();
			if (!InputDir.IsNearlyZero())
			{
				SetActorRotation(InputDir.Rotation());
			}

			FOnMontageEnded EndDelegate;
			EndDelegate.BindUObject(this, &AValkyrieCharacter::OnAttackMontageEnded);
			AnimInst->Montage_Play(AttackMontage);
			AnimInst->Montage_SetEndDelegate(EndDelegate, AttackMontage);

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
	{
		return;
	}

	switch (AttackData->GetAttackType())
	{
	case EAttackType::Melee:
	{
		TArray<AActor*> AttackTargets;
		CollectAttackTargets(AttackTargets);

		for (AActor* Target : AttackTargets)
		{
			if (!IsValidAttackTargetActor(Target))
			{
				continue;
			}

			ApplyAttack(Target);
		}
		break;
	}

	case EAttackType::Projectile:
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
}

void AValkyrieCharacter::ExecuteSkill(int32 InSkillIndex)
{
	if (!CanUseSkillIndex(InSkillIndex))
	{
		return;
	}

	if (!SkillDataList.IsValidIndex(InSkillIndex))
	{
		return;
	}

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	if (!AnimInst)
	{
		return;
	}

	UsingSkillIndex = InSkillIndex;

	USkillData* UsingSkill = SkillDataList[UsingSkillIndex];
	if (!UsingSkill)
	{
		return;
	}

	UAnimMontage* SkillMontage = UsingSkill->GetMontage().LoadSynchronous();
	if (!SkillMontage)
	{
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &AValkyrieCharacter::OnSkillMontageEnded);

	AnimInst->Montage_Play(SkillMontage);
	AnimInst->Montage_SetEndDelegate(EndDelegate, SkillMontage);

	TArray<FGameplayCueData> Cues = UsingSkill->GetCue(EGameplayCueOrder::OnExecute);
	for (const FGameplayCueData& Cue : Cues)
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation() + Cue.Offset;
		AbilitySystemComponent->ExecuteGameplayCue(Cue.Tag, CueParams);
	}

	MarkSkillUsed(InSkillIndex);
}

AActor* AValkyrieCharacter::FindBestMeleeMainTarget(float InRange) const
{
	UWorld* World = GetWorld();
	if (!World || InRange <= 0.f)
	{
		return nullptr;
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(const_cast<AValkyrieCharacter*>(this));

	TArray<AActor*> OverlapActors;
	const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		World,
		GetActorLocation(),
		InRange,
		ObjectTypes,
		AActor::StaticClass(),
		IgnoreActors,
		OverlapActors
	);

	if (!bHit)
	{
		return nullptr;
	}

	AActor* BestTarget = nullptr;
	float BestScore = -FLT_MAX;

	const FVector MyLocation = GetActorLocation();
	const FVector Forward = GetActorForwardVector();

	for (AActor* Actor : OverlapActors)
	{
		if (!IsValidAttackTargetActor(Actor))
		{
			continue;
		}

		const FVector ToTarget = Actor->GetActorLocation() - MyLocation;
		const float DistSq = ToTarget.SizeSquared2D();
		const FVector Dir = ToTarget.GetSafeNormal2D();
		const float Dot = FVector::DotProduct(Forward, Dir);

		// 전방 우선 + 가까울수록 가산점
		const float Score = Dot * 100000.f - DistSq;

		if (Score > BestScore)
		{
			BestScore = Score;
			BestTarget = Actor;
		}
	}

	return BestTarget;
}

void AValkyrieCharacter::CollectAttackTargets(TArray<AActor*>& OutTargets) const
{
	OutTargets.Reset();

	if (!AttackData)
	{
		return;
	}

	AActor* MainTarget = FindBestMeleeMainTarget(AttackData->GetAttackRange());
	if (!IsValidAttackTargetActor(MainTarget))
	{
		return;
	}

	OutTargets.Add(MainTarget);

	const FTargetingDataRow& TargetingData = AttackData->GetTargetingData();

	if (TargetingData.SplashTargetAmount > 0 && TargetingData.SplashRange > 0.f)
	{
		CollectSplashTargets(
			MainTarget,
			TargetingData.SplashTargetAmount,
			TargetingData.SplashRange,
			OutTargets
		);
	}
}

void AValkyrieCharacter::CollectSkillTargets(int32 InSkillIndex, TArray<AActor*>& OutTargets) const
{
	OutTargets.Reset();

	if (!SkillDataList.IsValidIndex(InSkillIndex))
	{
		return;
	}

	USkillData* SkillData = SkillDataList[InSkillIndex];
	if (!SkillData)
	{
		return;
	}

	AActor* MainTarget = FindBestMeleeMainTarget(SkillData->GetAttackRange());
	if (!IsValidAttackTargetActor(MainTarget))
	{
		return;
	}

	OutTargets.Add(MainTarget);

	const FTargetingDataRow& TargetingData = SkillData->GetTargetingData();

	if (TargetingData.SplashTargetAmount > 0 && TargetingData.SplashRange > 0.f)
	{
		CollectSplashTargets(
			MainTarget,
			TargetingData.SplashTargetAmount,
			TargetingData.SplashRange,
			OutTargets
		);
	}
}

void AValkyrieCharacter::CollectSplashTargets(
	AActor* MainTarget,
	int32 SplashTargetAmount,
	float SplashRange,
	TArray<AActor*>& OutTargets
) const
{
	if (!MainTarget || SplashTargetAmount <= 0 || SplashRange <= 0.f)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(const_cast<AValkyrieCharacter*>(this));
	IgnoreActors.Add(MainTarget);

	TArray<AActor*> OverlapActors;
	const bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		World,
		MainTarget->GetActorLocation(),
		SplashRange,
		ObjectTypes,
		AActor::StaticClass(),
		IgnoreActors,
		OverlapActors
	);

	if (!bHit || OverlapActors.IsEmpty())
	{
		return;
	}

	struct FSplashCandidate
	{
		AActor* Target = nullptr;
		float DistSq = 0.f;
	};

	TArray<FSplashCandidate> Candidates;
	for (AActor* Actor : OverlapActors)
	{
		if (!IsValidAttackTargetActor(Actor))
		{
			continue;
		}

		FSplashCandidate NewCandidate;
		NewCandidate.Target = Actor;
		NewCandidate.DistSq = FVector::DistSquared(MainTarget->GetActorLocation(), Actor->GetActorLocation());

		Candidates.Add(NewCandidate);
	}

	Candidates.Sort([](const FSplashCandidate& A, const FSplashCandidate& B)
		{
			return A.DistSq < B.DistSq;
		});

	const int32 AddCount = FMath::Min(SplashTargetAmount, Candidates.Num());
	for (int32 i = 0; i < AddCount; ++i)
	{
		OutTargets.Add(Candidates[i].Target);
	}
}

bool AValkyrieCharacter::IsValidAttackTargetActor(const AActor* TargetActor) const
{
	if (!IsValid(TargetActor))
	{
		return false;
	}

	if (TargetActor == this)
	{
		return false;
	}

	if (const AUnitCharacter* TargetUnit = Cast<AUnitCharacter>(TargetActor))
	{
		if (TargetUnit->IsDead())
		{
			return false;
		}

		if (TargetUnit->GetTeamType() == GetTeamType())
		{
			return false;
		}

		return true;
	}

	if (const ABaseWall* TargetWall = Cast<ABaseWall>(TargetActor))
	{
		if (TargetWall->GetTeamType() == GetTeamType())
		{
			return false;
		}

		return true;
	}

	return false;
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
	if (!SkillDataList.IsValidIndex(UsingSkillIndex))
	{
		return;
	}

	USkillData* UsingSkill = SkillDataList[UsingSkillIndex];
	if (!UsingSkill)
	{
		return;
	}

	if (UsingSkill->GetAttackType() == EAttackType::Melee)
	{
		TArray<AActor*> SkillTargets;
		CollectSkillTargets(UsingSkillIndex, SkillTargets);

		for (AActor* Target : SkillTargets)
		{
			if (!IsValidAttackTargetActor(Target))
			{
				continue;
			}

			ApplySkill(UsingSkillIndex, Target);
		}

		UsingSkillIndex = 0;
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
			ABaseProjectile* Projectile = Pool->Get<ABaseProjectile>(SkillDataList[UsingSkillIndex]->GetProjectileData()->EPoolTypes, FirePos, GetActorRotation());
			Projectile->SetTeam(GetTeamType());
			float TotalAttack = StatAttributeSet->GetAttack();

			if (EquippedWeapon)
			{
				TotalAttack += EquippedWeapon->GetStat().Attack;
			}
			Projectile->SetAttack(TotalAttack);
			Projectile->SetData(SkillDataList[UsingSkillIndex]->GetAbilityTag(), AttackSpec, *SkillDataList[UsingSkillIndex]->GetProjectileData(), SkillDataList[UsingSkillIndex]->GetCue(EGameplayCueOrder::InProjectile));
		}
	}
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
		AttackRange = AttackData->GetAttackRange();
		UE_LOG(LogTemp, Warning, TEXT("Attack Cool"));
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
	InitSkillCooldowns();

	UpdateWeaponMesh();
	//BroadcastHpChanged();
}

void AValkyrieCharacter::EnableAutoBattle()
{
	if (bIsAutoBattle)
	{
		return;
	}

	bIsAutoBattle = true;

	SwitchToAIController();

	UE_LOG(LogTemp, Warning, TEXT("[Valkyrie] Auto Battle ENABLED"));
}

void AValkyrieCharacter::DisableAutoBattle()
{
	if (!bIsAutoBattle)
	{
		return;
	}

	bIsAutoBattle = false;

	SwitchToPlayerController();

	UE_LOG(LogTemp, Warning, TEXT("[Valkyrie] Auto Battle DISABLED"));
}

void AValkyrieCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ResetCombo();
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void AValkyrieCharacter::HandleControlModeChanged(EInputControlMode NewMode)
{
	if (NewMode == EInputControlMode::Auto)
	{
		EnableAutoBattle();
	}
	else
	{
		DisableAutoBattle();
	}
}

float AValkyrieCharacter::GetCurrentWorldTime() const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return 0.0f;
	}

	return World->GetTimeSeconds();
}

void AValkyrieCharacter::InitSkillCooldowns()
{
	SkillNextUsableTime.Empty();
	SkillNextUsableTime.Init(0.0f, SkillDataList.Num());
}

float AValkyrieCharacter::GetSkillCooldownDuration(int32 InSkillIndex) const
{
	if (!SkillDataList.IsValidIndex(InSkillIndex))
	{
		return 0.0f;
	}

	USkillData* SkillData = SkillDataList[InSkillIndex];
	if (!SkillData)
	{
		return 0.0f;
	}

	return SkillData->GetCooldownDuration();
}

void AValkyrieCharacter::MarkSkillUsed(int32 InSkillIndex)
{
	if (!SkillNextUsableTime.IsValidIndex(InSkillIndex))
	{
		return;
	}

	const float Cooldown = GetSkillCooldownDuration(InSkillIndex);
	const float CurrentTime = GetCurrentWorldTime();

	SkillNextUsableTime[InSkillIndex] = CurrentTime + Cooldown;

	UE_LOG(LogTemp, Log, TEXT("[Valkyrie] Skill %d used. Cooldown=%.2f NextUsableTime=%.2f"),
		InSkillIndex,
		Cooldown,
		SkillNextUsableTime[InSkillIndex]);
}

bool AValkyrieCharacter::IsSkillOnCooldown(int32 InSkillIndex) const
{
	if (!SkillNextUsableTime.IsValidIndex(InSkillIndex))
	{
		return false;
	}

	return GetCurrentWorldTime() < SkillNextUsableTime[InSkillIndex];
}

float AValkyrieCharacter::GetSkillRemainingCooldown(int32 InSkillIndex) const
{
	if (!SkillNextUsableTime.IsValidIndex(InSkillIndex))
	{
		return 0.0f;
	}

	return FMath::Max(0.0f, SkillNextUsableTime[InSkillIndex] - GetCurrentWorldTime());
}

void AValkyrieCharacter::TryUseSkillOrAttack()
{
	const int32 SkillIndex = FindUsableSkillIndex();
	if (SkillIndex != INDEX_NONE)
	{
		ExecuteSkill(SkillIndex);
		return;
	}

	ExecuteAttack();
}

int32 AValkyrieCharacter::FindUsableSkillIndex() const
{
	for (int32 i = 0; i < SkillDataList.Num(); ++i)
	{
		if (CanUseSkillIndex(i))
		{
			return i;
		}
	}

	return INDEX_NONE;
}

bool AValkyrieCharacter::CanUseSkillIndex(int32 InSkillIndex) const
{
	if (!SkillDataList.IsValidIndex(InSkillIndex))
	{
		return false;
	}

	USkillData* SkillData = SkillDataList[InSkillIndex];
	if (!SkillData)
	{
		return false;
	}

	if (IsSkillOnCooldown(InSkillIndex))
	{
		return false;
	}

	UAnimInstance* AnimInst = GetMesh() ? GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInst)
	{
		return false;
	}

	if (AnimInst->IsAnyMontagePlaying())
	{
		return false;
	}

	return true;
}
