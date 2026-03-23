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
		EventSystem->Lobby.OnLobbyStateChanged.AddDynamic(this, &ALobbyPlayerController::ChageGameState);
		EventSystem->Lobby.OnLoadLobby.AddDynamic(this, &ALobbyPlayerController::LoadLobbyLevel);
		EventSystem->Lobby.OnLoadGacha.AddDynamic(this, &ALobbyPlayerController::LoadGachaLevel);
		EventSystem->Lobby.OnShowNextGacha.AddDynamic(this, &ALobbyPlayerController::ShowNextGacha);

	}	
	ChageGameState(ELobbyState::Init);
}

void ALobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (UWorldEventSystem* EventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		EventSystem->Lobby.OnLobbyStateChanged.RemoveDynamic(this, &ALobbyPlayerController::ChageGameState);
		EventSystem->Lobby.OnLoadLobby.RemoveDynamic(this, &ALobbyPlayerController::LoadLobbyLevel);
		EventSystem->Lobby.OnLoadGacha.RemoveDynamic(this, &ALobbyPlayerController::LoadGachaLevel);
		EventSystem->Lobby.OnShowNextGacha.RemoveDynamic(this, &ALobbyPlayerController::ShowNextGacha);
	}

}

void ALobbyPlayerController::ChageGameState(ELobbyState InState)
{
	if (ALobbyGameState* State = GetWorld()->GetGameState<ALobbyGameState>())
	{
		State->ChangeState(InState);
	}
}

void ALobbyPlayerController::SetActorCamera(FName InLevelName)
{
	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), InLevelName);

	if (StreamingLevel && StreamingLevel->IsLevelLoaded())
	{
		// 2. 스트리밍 객체에서 실제 로드된 ULevel 메모리를 가져옵니다.
		ULevel* LoadedLevel = StreamingLevel->GetLoadedLevel();

		if (LoadedLevel)
		{
			for (AActor* Actor : LoadedLevel->Actors)
			{
				// 1. 배열의 해당 인덱스가 비어있지 않은지 반드시 먼저 확인합니다.
				if (Actor != nullptr)
				{
					// 2. 그 다음 클래스를 확인하고 캐스팅합니다.
					if (Actor->IsA(ACameraActor::StaticClass()))
					{
						ACameraActor* MyCam = Cast<ACameraActor>(Actor);
						// MyCam 사용

						SetViewTargetWithBlend(MyCam, 0.0f);
						break;
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
		// 비동기 로드 시작
		UGameplayStatics::LoadStreamLevel(GetWorld(), FName("Lobby"), true, false, FLatentActionInfo());
	}
}

void ALobbyPlayerController::LoadGachaLevel(int32 InAmount, int32 InGachaGroupId)
{
	CurrentGachaIndex = 0;
	GachaResultWidget = nullptr;

	if (USaveManager* SaveManager = GetGameInstance()->GetSubsystem<USaveManager>())
	{
		//우선은 급해서 그냥 차감.. 테이블로 해야할지는 고민..
		SaveManager->AddGoods(EGoodsType::Gem, InAmount * 100);
	}

	SetGachaResult(InAmount, InGachaGroupId);

	//미리 뽑기는 완료함

	// 스트리밍 레벨 객체를 먼저 가져옵니다.
	ULevelStreaming* StreamingLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("GachaMap"));

	if (StreamingLevel)
	{
		// 레벨 로드가 완료되었을 때 실행될 함수를 델리게이트에 연결합니다.
		StreamingLevel->OnLevelLoaded.AddDynamic(this, &ALobbyPlayerController::OnGachaLevelLoaded);
		StreamingLevel->OnLevelShown.AddDynamic(this, &ALobbyPlayerController::OnGachaLevelShown);

		// 비동기 로드 시작
		UGameplayStatics::LoadStreamLevel(GetWorld(), FName("GachaMap"), true, false, FLatentActionInfo());
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

void ALobbyPlayerController::OnGachaLevelLoaded()
{
	if (ULevelStreaming* LobbyLevel = UGameplayStatics::GetStreamingLevel(GetWorld(), FName("Lobby")))
	{
		UGameplayStatics::UnloadStreamLevel(GetWorld(), FName("Lobby"), FLatentActionInfo(), false);
	}	
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
		if (GachaResultWidget = UIManager->OpenUI<UGachaResultWidget>(EUIType::PopupGachaResult))
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
							NewItem->MakeData(MasteryItem);
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
