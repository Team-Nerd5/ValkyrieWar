#include "Data/Module/RewardModule.h"

#include "GameSystem/Instance/Game/GameManager.h"
#include "GameSystem/Instance/Game/DataManager.h"

#include "Data/Module/ItemModule.h"
#include "Data/Module/GoodsModule.h"

#include "Data/Table/GameData/ItemDataRow.h"
#include "Data/Table/GameData/GoodsDataRow.h"

void URewardModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		DataTable = GameManager->GetGameData(ETableDataType::Reward);
		MakeData();
	}

	SendDataLoadComplete();
}

void URewardModule::MakeData()
{
	TableDataByDataId.Empty();

	if (!DataTable)
	{
		return;
	}

	TArray<FRewardDataRow*> AllRows;
	DataTable->GetAllRows<FRewardDataRow>(TEXT("RewardModule_Init"), AllRows);

	for (const FRewardDataRow* Row : AllRows)
	{
		if (!Row)
		{
			continue;
		}

		TableDataByDataId.Add(Row->DataId, *Row);
	}
}

bool URewardModule::GetRewardRowsByDataId(int32 InDataId, FRewardDataRow& OutData) const
{
	if (InDataId <= 0)
	{
		return false;
	}

	const FRewardDataRow* FoundReward = TableDataByDataId.Find(InDataId);
	if (!FoundReward)
	{
		return false;
	}

	OutData = *FoundReward;
	return true;
}

const FRewardDataRow URewardModule::FindRewardByDataId(int32 InDataId) const
{
	if (TableDataByDataId.Contains(InDataId))
		return TableDataByDataId.FindChecked(InDataId);
	else
		return FRewardDataRow();
}

/**
 * RewardDataRow를 UI 표시용 RewardViewData로 변환
 *
 * @param InRewardRow   Reward 테이블의 원본 데이터
 * @param InRewardRate  StageReward에서 넘어온 확률 (만분율)
 * @param OutViewData   최종 UI 표시용 데이터 (아이콘, 이름 등 포함)
 *
 * @return true  : 정상 변환 성공
 * @return false : 데이터 누락 또는 타입 미지원
 */
bool URewardModule::MakeRewardViewData(
	const FRewardDataRow& InRewardRow,
	int32 InRewardRate,
	FRewardViewData& OutViewData) const
{
	// 출력 데이터 초기화
	OutViewData = FRewardViewData();

	// 공통 데이터 세팅 (Reward 테이블 기반)
	OutViewData.RewardType = InRewardRow.RewardType;
	OutViewData.RewardId = InRewardRow.DataId;
	OutViewData.RewardDataId = InRewardRow.RewardDataId;
	OutViewData.Amount = InRewardRow.Amount;
	OutViewData.RewardRate = InRewardRate;

	// RewardType에 따라 실제 데이터 해석
	switch (InRewardRow.RewardType)
	{
	case ERewardType::Goods:
	{
		// GoodsModule 접근
		TObjectPtr<UGoodsModule> GoodsModule = GetGoodsModule();
		if (!GoodsModule)
		{
			UE_LOG(LogTemp, Warning, TEXT("[RewardModule] GoodsModule is null"));
			return false;
		}

		// RewardDataId를 GoodsType으로 해석
		const EGoodsType GoodsType = static_cast<EGoodsType>(InRewardRow.RewardDataId);

		// Goods 테이블에서 데이터 조회
		const FGoodsDataRow GoodsRow = GoodsModule->GetTableData(GoodsType);

		// 유효성 체크
		if (GoodsRow.GoodsType == EGoodsType::None)
		{
			UE_LOG(LogTemp, Warning, TEXT("[RewardModule] Goods row not found. RewardDataId=%d"), InRewardRow.RewardDataId);
			return false;
		}

		// UI 표시용 이름/아이콘 설정
		OutViewData.Name = GetGoodsDisplayName(GoodsRow.GoodsType);
		OutViewData.Icon = GoodsRow.Icon;

		return true;
	}

	case ERewardType::Item:
	{
		// ItemModule 접근
		TObjectPtr<UItemModule> ItemModule = GetItemModule();
		if (!ItemModule)
		{
			UE_LOG(LogTemp, Warning, TEXT("[RewardModule] ItemModule is null"));
			return false;
		}

		// Item 테이블에서 데이터 조회
		const FItemDataRow ItemRow = ItemModule->GetTableDataById(InRewardRow.RewardDataId);

		// 유효성 체크
		if (ItemRow.DataId <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[RewardModule] Item row not found. RewardDataId=%d"), InRewardRow.RewardDataId);
			return false;
		}

		// UI 표시용 이름/아이콘 설정
		OutViewData.Name = ItemRow.Name;
		OutViewData.Icon = ItemRow.Icon;

		return true;
	}

	case ERewardType::Character:
	{
		// 현재 미구현 타입
		return false;
	}

	case ERewardType::None:
	default:
		break;
	}

	// 잘못된 타입
	UE_LOG(LogTemp, Warning, TEXT("[RewardModule] Invalid RewardType. RewardId=%d"), InRewardRow.DataId);
	return false;
}

TObjectPtr<UItemModule> URewardModule::GetItemModule() const
{
	if (!GameManager.IsValid())
	{
		return nullptr;
	}

	if (UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>())
	{
		return DataManager->GetItemModule();
	}

	return nullptr;
}

TObjectPtr<UGoodsModule> URewardModule::GetGoodsModule() const
{
	if (!GameManager.IsValid())
	{
		return nullptr;
	}

	if (UDataManager* DataManager = GameManager->GetSubsystem<UDataManager>())
	{
		return DataManager->GetGoodsModule();
	}

	return nullptr;
}

FString URewardModule::GetGoodsDisplayName(EGoodsType InGoodsType) const
{
	switch (InGoodsType)
	{
	case EGoodsType::Ticket:
		return TEXT("티켓");
	case EGoodsType::Gem:
		return TEXT("젬");
	case EGoodsType::Gold:
		return TEXT("골드");
	case EGoodsType::None:
	default:
		return TEXT("없음");
	}
}
