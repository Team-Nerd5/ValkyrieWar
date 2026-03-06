#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/Enum/CommonEnums.h"
#include "WallHealthBarWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class VALKYRIEWAR_API UWallHealthBarWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "WallHP")
    void Init();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UProgressBar> HealthBar;

    // 퍼센트 표시용 텍스트(UMG에 같은 이름으로 TextBlock 추가)
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> PercentText;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallHP")
    ETeam Team = ETeam::TeamA;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "WallHP")
    float MaxHealth = 100.f;

private:
    bool bBound = false;

    void BindDelegates();
    void UnbindDelegates();
    void ApplyHealth(float InNewHealth);

    UFUNCTION()
    void HandleAllyWallHealthChanged(float InCurrentHealth, float InMaxHealth);

    UFUNCTION()
    void HandleEnemyWallHealthChanged(float InCurrentHealth, float InMaxHealth);
};
