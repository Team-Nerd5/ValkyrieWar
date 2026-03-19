#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseActor.h"
#include "Data/Enum/CommonEnums.h"
#include "AbilitySystemInterface.h"
#include "BaseWall.generated.h"

class UAbilitySystemComponent;
class ACoreWallActor;

UCLASS()
class VALKYRIEWAR_API ABaseWall : public ABaseActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ABaseWall();

	// GAS가 이 액터에서 ASC를 물어보면 "코어의 ASC"를 내준다
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	inline ETeamType GetTeamType() const { return Team; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<ACoreWallActor> CachedCore;

	UPROPERTY(EditAnywhere, Category = "Team")
	ETeamType Team = ETeamType::Ally;
};
