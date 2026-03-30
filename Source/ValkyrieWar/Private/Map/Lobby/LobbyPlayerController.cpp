// Fill out your copyright notice in the Description page of Project Settings.


#include "Map/Lobby/LobbyPlayerController.h"

#include "GameSystem/Instance/Game/UIManager.h"
#include "GameSystem/Instance/Game/SaveManager.h"
#include "GameSystem/Instance/Game/InventorySystem.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"

#include "GameSystem/GameMode/LobbyGameMode.h"

#include "GameSystem/State/Game/LobbyGameState.h"

#include "Data/Enum/DataEnums.h"
#include "Data/Game/ItemData.h"

#include "GameSystem/Library/GameBaseLibrary.h"
#include "GameSystem/Library/RandomGenerateHelper.h"
#include "GameSystem/Library/Data/GameDataFactory.h"

#include "Kismet/GameplayStatics.h"

#include "Widget/HUD/LobbyWidget.h"
#include "Widget/Popup/Gacha/GachaResultWidget.h"
#include "Widget/Popup/Gacha/GachaWidget.h"
#include "Widget/Loading/LoadingWidget.h"
#include "Widget/Popup/CharacterInfo/CharacterInfoWidget.h"
#include "Widget/Popup/Stage/StageListPanelWidget.h"

#include "Object/Character/Valkyrie/ValkyrieCharacter.h"

#include "Object/Cheat/LobbyCheatManager.h"

#include "Camera/CameraActor.h"

ALobbyPlayerController::ALobbyPlayerController()
{
	CheatClass = ULobbyCheatManager::StaticClass();
	bAutoManageActiveCameraTarget = false;
}

void ALobbyPlayerController::BeginPlay()
{
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Lobby.OnLobbyStateChanged.AddDynamic(this, &ALobbyPlayerController::ChangeGameState);
		EventSystem->Lobby.OnLoadLobby.AddDynamic(this, &ALobbyPlayerController::LoadLobbyLevel);
		EventSystem->Lobby.OnLoadGacha.AddDynamic(this, &ALobbyPlayerController::LoadGachaLevel);
		EventSystem->Lobby.OnShowNextGacha.AddDynamic(this, &ALobbyPlayerController::ShowNextGacha);
		EventSystem->Widget.OnCharacterInfoWidgetOpened.AddDynamic(this, &ALobbyPlayerController::StartCharacterInfoCamMove);
		EventSystem->Widget.OnCharacterInfoWidgetClosed.AddDynamic(this, &ALobbyPlayerController::StartLobbyCamMove);
		EventSystem->Widget.OnValkyrieSelected.AddDynamic(this, &ALobbyPlayerController::OnValkyrieChanged);
		EventSystem->Widget.OnUnEquipItem.AddDynamic(this, &ALobbyPlayerController::OnUnEquipItem);
		EventSystem->Widget.OnEquipItem.AddDynamic(this, &ALobbyPlayerController::OnEquipItem);
	}	
	ChangeGameState(ELobbyState::Init);
}

void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Lobby.OnLobbyStateChanged.RemoveDynamic(this, &ALobbyPlayerController::ChangeGameState);
		EventSystem->Lobby.OnLoadLobby.RemoveDynamic(this, &ALobbyPlayerController::LoadLobbyLevel);
		EventSystem->Lobby.OnLoadGacha.RemoveDynamic(this, &ALobbyPlayerController::LoadGachaLevel);
		EventSystem->Lobby.OnShowNextGacha.RemoveDynamic(this, &ALobbyPlayerController::ShowNextGacha);
		EventSystem->Widget.OnCharacterInfoWidgetOpened.RemoveDynamic(this, &ALobbyPlayerController::StartCharacterInfoCamMove);
		EventSystem->Widget.OnCharacterInfoWidgetClosed.RemoveDynamic(this, &ALobbyPlayerController::StartLobbyCamMove);
		EventSystem->Widget.OnValkyrieSelected.RemoveDynamic(this, &ALobbyPlayerController::OnValkyrieChanged);
		EventSystem->Widget.OnUnEquipItem.RemoveDynamic(this, &ALobbyPlayerController::OnUnEquipItem);
		EventSystem->Widget.OnEquipItem.RemoveDynamic(this, &ALobbyPlayerController::OnEquipItem);
	}

}

void ALobbyPlayerController::ChangeGameState(ELobbyState InState)
{
	if (ALobbyGameState* State = GetWorld()->GetGameState<ALobbyGameState>())
	{
		State->ChangeState(InState);
	}
}

void ALobbyPlayerController::SetActorCamera(FName InLevelName)
{
	CurrentCamera = nullptr;

	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), InLevelName);

	if (StreamingLevel && StreamingLevel->IsLevelLoaded())
	{
		// 2. 스트리밍 객체에서 실제 로드된 ULevel 메모리를 가져옵니다.
		ULevel* LoadedLevel = StreamingLevel->GetLoadedLevel();

		if (LoadedLevel)
		{
			for (AActor* Actor : LoadedLevel->Actors)
			{
				if (Actor != nullptr)
				{
					if (Actor->IsA(ACameraActor::StaticClass()))
					{
						if (Actor->ActorHasTag("CharacterInfo"))
						{
							CharacterInfoCamera = Cast<ACameraActor>(Actor);
						}
						else
						{
							CurrentCamera = Cast<ACameraActor>(Actor);
							// MyCam 사용

							SetViewTargetWithBlend(CurrentCamera, 0.0f);
						}
					}
				}
			}
		}
	}

	bShowMouseCursor = true;
}

void ALobbyPlayerController::LoadLobbyLevel()
{
	// 스트리밍 레벨 객체를 먼저 가져옵니다.
	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("Lobby"));

	if (StreamingLevel)
	{
		// 레벨 로드가 완료되었을 때 실행될 함수를 델리게이트에 연결합니다.
		StreamingLevel->OnLevelLoaded.AddUniqueDynamic(this, &ALobbyPlayerController::OnLobbyLevelLoaded);
		StreamingLevel->OnLevelShown.AddUniqueDynamic(this, &ALobbyPlayerController::OnLobbyLevelShown);
		// 비동기 로드 시작
		UGameplayStatics::LoadStreamLevel(GetWorld(), FName("Lobby"), true, false, FLatentActionInfo());
	}
}

void ALobbyPlayerController::OnLobbyLevelLoaded()
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->CloseTopPopupUI();
		UIManager->OpenUI<ULobbyWidget>(EUIType::Lobby);
	}

	if (ULevelStreaming* GachaLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("GachaMap")))
	{
		UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("GachaMap"), FLatentActionInfo(), false);
	}
	SetActorCamera(FName("Lobby"));
}

void ALobbyPlayerController::OnLobbyLevelShown()
{
	ShowLobbyCharacter();

	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		ALobbyGameState* State = GetWorld()->GetGameState<ALobbyGameState>();
		if (State->GetState() == ELobbyState::Gacha)
		{
			UIManager->OpenUI<UGachaWidget>(EUIType::PopupGacha);
		}
		else
		{
			if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
			{
				int32 NextStage = DataManager->GetStageInfoModule()->GetNextStageDataId();
				if (NextStage > 0)
				{
					UStageListPanelWidget* StageWidget = UIManager->OpenUI<UStageListPanelWidget>(EUIType::PopupStageList);
					StageWidget->OpenStageList(NextStage);
				}
			}
		}
	}

	ChangeGameState(ELobbyState::Ready);
}

void ALobbyPlayerController::ShowLobbyCharacter()
{
	if (UGameManager* GameManager = GetWorld()->GetGameInstance<UGameManager>())
	{
		if (UValkyrieData* Valkyrie = GameManager->GetSelectedValkyrie())
		{
			AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
			if (ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GameMode))
			{
				LobbyGameMode->SpawnValkyire(Valkyrie, this, TEXT("LobbyStart"));
			}
		}		
	}
}

void ALobbyPlayerController::ChangeLobbyCharacter(UValkyrieData* InNewValkyrie)
{
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	if (ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GameMode))
	{
		LobbyGameMode->SpawnValkyire(InNewValkyrie, this, TEXT("LobbyStart"));
	}
}

void ALobbyPlayerController::LoadGachaLevel(int32 InAmount, int32 InGachaGroupId)
{
	CurrentGachaIndex = 0;
	GachaResultWidget = nullptr;
	GachaResultData.Empty();

	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		//우선은 급해서 그냥 차감.. 테이블로 해야할지는 고민..
		DataManager->GetGoodsModule()->Add(EGoodsType::Gem, -InAmount * 100);
	}

	SetGachaResult(InAmount, InGachaGroupId);

	//미리 뽑기는 완료함

	// 스트리밍 레벨 객체를 먼저 가져옵니다.
	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("GachaMap"));

	if (StreamingLevel)
	{
		// 레벨 로드가 완료되었을 때 실행될 함수를 델리게이트에 연결합니다.
		StreamingLevel->OnLevelLoaded.AddUniqueDynamic(this, &ALobbyPlayerController::OnGachaLevelLoaded);
		StreamingLevel->OnLevelShown.AddUniqueDynamic(this, &ALobbyPlayerController::OnGachaLevelShown);

		// 비동기 로드 시작
		UGameplayStatics::LoadStreamLevel(GetWorld(), FName("GachaMap"), true, false, FLatentActionInfo());
	}
}

void ALobbyPlayerController::OnGachaLevelLoaded()
{
	if (ULevelStreaming* LobbyLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("Lobby")))
	{
		UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("Lobby"), FLatentActionInfo(), false);
	}

	ChangeGameState(ELobbyState::Gacha);

	SetActorCamera(FName("GachaMap"));
}

void ALobbyPlayerController::OnGachaLevelShown()
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		//가챠 UI를 꺼줌
		UIManager->CloseTopPopupUI();
		//로비 HUD 꺼줌
		UIManager->CloseUI<ULobbyWidget>(EUIType::Lobby);

		//미리 띄우고...스킵 버튼 누르면 이벤트 처리
		GachaResultWidget = UIManager->OpenUI<UGachaResultWidget>(EUIType::PopupGachaResult);

		if(GachaResultWidget)
		{
			GachaResultWidget->SetResultData(GachaResultData);
		}
	}

	ShowGachaCharacter();
}

void ALobbyPlayerController::SetGachaResult(int32 InAmount, int32 InGachaGroupId)
{
	if (UDataManager* DataManager = GetGameInstance()->GetSubsystem<UDataManager>())
	{
		for (int32 i = 0; i < InAmount; i++)
		{
			int32 GeneratedData = URandomGenerateHelper::GetRandomValkyrie(DataManager, InGachaGroupId);

			if (DataManager->GetValkyrieModule()->HasValkyrie(GeneratedData))
			{
				//아이템 데이터 생성
				FValkyrieDataRow ValkyrieTableData = DataManager->GetValkyrieModule()->GetTableData(GeneratedData);
				FItemDataRow BaseWeapon = DataManager->GetItemModule()->GetTableDataById(ValkyrieTableData.BaseWeaponId);
				if (BaseWeapon.DataId > 0)
				{
					FItemDataRow MasteryItem = DataManager->GetItemModule()->GetMasteryItem(BaseWeapon.WeaponType);
					if (ValkyrieTableData.DataId > 0 && BaseWeapon.DataId > 0)
					{
						if (UInventorySystem* Inventory = GetGameInstance()->GetSubsystem<UInventorySystem>())
						{
							int32 ItemAmount = GetMasteryItemAmount(ValkyrieTableData.BaseGrade);

							UItemData* NewItem = NewObject<UItemData>(this);
							NewItem->MakeData(MasteryItem, GetGameInstance<UGameManager>());
							NewItem->AddAmount(ItemAmount);
							GachaResultData.Add(NewItem);

							//인벤토리에 추가
							Inventory->AddItem(MasteryItem.DataId, ItemAmount);
						}
					}
				}				
			}
			else
			{
				//발키리 데이터 생성
				UValkyrieData* NewValkyrie = UGameDataFactory::GenerateValkyrie(GeneratedData, GetGameInstance());
				//캐릭터 모듈에도 추가... 저장도
				GachaResultData.Add(NewValkyrie);
			}
		}
	}
}

int32 ALobbyPlayerController::GetMasteryItemAmount(EGradeType InGrade)
{
	return static_cast<int32>(InGrade) + 1;
}

void ALobbyPlayerController::ShowGachaCharacter()
{
	UObject* CurrentData = GachaResultData[CurrentGachaIndex];

	if (UValkyrieData* Valkyrie = Cast<UValkyrieData>(CurrentData))
	{
		//캐릭터 연출 시작(추후 추가)
		
		//우선은 생성만
		AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
		if (ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GameMode))
		{
			LobbyGameMode->SpawnValkyire(Valkyrie, this, TEXT("GachaStart"));
		}
	}
	else
	{
		ShowNextGacha();
	}
}

void ALobbyPlayerController::ShowNextGacha()
{
	//현재 연출 종료 필요(추후 추가 시)
	if(CurrentGachaIndex < GachaResultData.Num())
		CurrentGachaIndex++;


	if (CurrentGachaIndex == GachaResultData.Num())
	{
		//결과 위젯에서 결과 띄워줌
		if (GachaResultWidget)
		{
			GachaResultWidget->ShowGachaResults();
		}
	}
	else
	{
		ShowGachaCharacter();
	}
}

void ALobbyPlayerController::StartCharacterInfoCamMove()
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->CloseUI<ULobbyWidget>(EUIType::Lobby);
	}

	if (CharacterInfoCamera && CurrentCamera)
	{
		SetViewTargetWithBlend(CharacterInfoCamera, 1.0f, VTBlend_Cubic);
		GetWorldTimerManager().SetTimer(CameraBlendTimerHandle, this, &ALobbyPlayerController::OnMovedCharacterInfo, 1.0f, false);
	}
}

void ALobbyPlayerController::OnMovedCharacterInfo()
{
	//UI켜줌
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->OpenUI<UCharacterInfoWidget>(EUIType::PopupCharacterInfo);
	}
}

void ALobbyPlayerController::StartLobbyCamMove()
{
	if (CharacterInfoCamera && CurrentCamera)
	{
		SetViewTargetWithBlend(CurrentCamera, 1.0f, VTBlend_Cubic);
		GetWorldTimerManager().SetTimer(CameraBlendTimerHandle, this, &ALobbyPlayerController::OnMovedLobby, 1.0f, false);
	}
}

void ALobbyPlayerController::OnMovedLobby()
{
	if (UUIManager* UIManager = GetGameInstance()->GetSubsystem<UUIManager>())
	{
		UIManager->OpenUI<ULobbyWidget>(EUIType::Lobby);
	}
}

//캐릭터 정보창에서 변경함
void ALobbyPlayerController::OnValkyrieChanged(UValkyrieData* InNewValkyrie)
{
	ChangeLobbyCharacter(InNewValkyrie);
}

void ALobbyPlayerController::OnEquipItem(UItemData* InItem)
{
	//스폰된 캐릭터 찾아서 변경
	if (APawn* Spawned = GetPawn())
	{
		if (AValkyrieCharacter* Valkyrie = Cast<AValkyrieCharacter>(Spawned))
		{
			Valkyrie->EquipWeapon(InItem);
		}
	}	
}

void ALobbyPlayerController::OnUnEquipItem()
{
	if (APawn* Spawned = GetPawn())
	{
		if (AValkyrieCharacter* Valkyrie = Cast<AValkyrieCharacter>(Spawned))
		{
			Valkyrie->EquipWeapon(nullptr);
		}
	}
}
