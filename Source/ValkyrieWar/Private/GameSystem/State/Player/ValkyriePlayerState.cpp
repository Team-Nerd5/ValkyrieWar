// Fill out your copyright notice in the Description page of Project Settings.


#include "GameSystem/State/Player/ValkyriePlayerState.h"
#include "GameSystem/Instance/World/WorldEventSystem.h"
#include "GameSystem/Library/GameBaseLibrary.h"

AValkyriePlayerState::AValkyriePlayerState()
{
	CurrentControlMode = EInputControlMode::Manual;
}

void AValkyriePlayerState::SetControlMode(EInputControlMode NewMode)
{
	if (CurrentControlMode == NewMode)
	{
		return;
	}

	CurrentControlMode = NewMode;
	if (UWorldEventSystem* WorldEventSystem = UGameBaseLibrary::GetWorldEventSystem(this))
	{
		WorldEventSystem->Battle.OnBattleModeChanged.Broadcast(CurrentControlMode);
	}
}

void AValkyriePlayerState::ToggleControlMode()
{
	const EInputControlMode NewMode =
		(CurrentControlMode == EInputControlMode::Manual)
		? EInputControlMode::Auto
		: EInputControlMode::Manual;

	SetControlMode(NewMode);
}
