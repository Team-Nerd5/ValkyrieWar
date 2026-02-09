// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Module/ItemModule.h"

void UItemModule::Initialize(UGameManager* InGameManager)
{
	Super::Initialize(InGameManager);

	if (GameManager.IsValid())
	{
		Table = GameManager->GetGameData(ETableDataType::Item);

		if (Table)
		{
			//데이터 변환해서 자료형으로 보관..
			//여기서 그냥 배열로만 들고있고
			//인벤토리 따로 구현해서 관리해도 괜찮을 듯..?
			//여기로 업데이트 하고 저장하는쪽으로 보내는 용도로 사용...
			// 캐릭터도 비슷하게 사용가능은 할듯
			// 나머지는 따로 안만들고 가능하긴 한데..			
		}
	}
}
