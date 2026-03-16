// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "StatAttributeSet.generated.h"


/**
 * 
 */
#define ATTRIBUTE_ACCESSORIS(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName,PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)\

UCLASS(BlueprintType)
class VALKYRIEWAR_API UStatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UStatAttributeSet();

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
public:
	UPROPERTY(BlueprintReadOnly, Category = "Attribute|Stat")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORIS(UStatAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attribute|Stat")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORIS(UStatAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Attribute|Stat")
	FGameplayAttributeData Attack;
	ATTRIBUTE_ACCESSORIS(UStatAttributeSet, Attack)

	UPROPERTY(BlueprintReadOnly, Category = "Attribute|Stat")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORIS(UStatAttributeSet, Defense)

	UPROPERTY(BlueprintReadOnly, Category = "Attribute|Stat")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORIS(UStatAttributeSet, MoveSpeed)
};
