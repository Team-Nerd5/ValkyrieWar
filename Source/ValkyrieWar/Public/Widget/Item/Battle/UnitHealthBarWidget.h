#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitHealthBarWidget.generated.h"

class UProgressBar;

UCLASS()
class VALKYRIEWAR_API UUnitHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetTeamColor(const FLinearColor& InColor);

	void SetHp(float CurrentHp, float MaxHp);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_HP = nullptr;
};
