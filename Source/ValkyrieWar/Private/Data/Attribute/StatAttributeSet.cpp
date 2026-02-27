// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/Attribute/StatAttributeSet.h"

UStatAttributeSet::UStatAttributeSet()
{
	InitAttack(100.0f);
	InitDefense(10.0f);
	InitHealth(1000.0f);
	InitMaxHealth(1000.0f);
	InitMoveSpeed(500.0f);
}

void UStatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UStatAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeBaseChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMoveSpeedAttribute())
	{
		//값 변경 될 때
	}
	if (Attribute == GetHealthAttribute())
	{
		AActor* TargetActor = GetOwningActor();
		//데미지 표기 / UI 표기 등...
		
	}
}

void UStatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
