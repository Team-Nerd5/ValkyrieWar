// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/Base/BaseModule.h"

void UBaseModule::Initialize(UGameManager* InGameManager)
{
	GameManager = InGameManager;
}
