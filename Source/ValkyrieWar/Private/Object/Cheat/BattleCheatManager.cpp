// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Cheat/BattleCheatManager.h"
#include "Object/Character/Valkyrie/ValkyrieCharacter.h"
#include "GameSystem/Instance/Game/InventorySystem.h"

#if WITH_EDITOR
void UBattleCheatManager::EquipItem(int32 InDataId)
{
	//월드에서 현재 캐릭터 찾고
	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();

	if (AValkyrieCharacter* Valkyrie = Cast<AValkyrieCharacter>(Player))
	{
		if (UInventorySystem* Inventory = GetWorld()->GetGameInstance()->GetSubsystem<UInventorySystem>())
		{
			UItemData* AddedItem = Inventory->AddItem(InDataId, 1);

			Valkyrie->ChangeWeapon(AddedItem);
		}
	}
}
#endif
