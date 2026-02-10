// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Library/GameSaveHelper.h"
#include "Object/SaveGame/ValkyrieSaveGame.h"
#include "Object/SaveGame/AccountSaveGame.h"
#include "Object/SaveGame/CheckAccountSaveGame.h"
#include "Object/SaveGame/GachaSaveGame.h"
#include "Object/SaveGame/GoodsSaveGame.h"
#include "Object/SaveGame/ItemSaveGame.h"
#include "Object/SaveGame/StageSaveGame.h"
#include "Object/SaveGame/UnitUpgradeSaveGame.h"

USaveGame* UGameSaveHelper::MakeSaveGame(ESaveType InType)
{
	switch (InType)
	{
	case ESaveType::Valkyrie:
		return NewObject<UValkyrieSaveGame>();
	case ESaveType::Account:
		return NewObject<UAccountSaveGame>();
	case ESaveType::CheckAccount:
		return NewObject<UCheckAccountSaveGame>();
	case ESaveType::Gacha:
		return NewObject<UGachaSaveGame>();
	case ESaveType::Goods:
		return NewObject<UGoodsSaveGame>();
	case ESaveType::Item:
		return NewObject<UItemSaveGame>();
	case ESaveType::Stage:
		return NewObject<UStageSaveGame>();
	case ESaveType::UnitUpgrade:
		return NewObject<UUnitUpgradeSaveGame>();
	}

	return nullptr;
}
