#pragma once

#include "CoreMinimal.h"
#include "GameSystem/Base/BaseWall.h"
#include "ProjectileWallActor.generated.h"

class UBoxComponent;

UCLASS()
class VALKYRIEWAR_API AProjectileWallActor : public ABaseWall
{
	GENERATED_BODY()

public:
	AProjectileWallActor();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	TObjectPtr<UBoxComponent> CollisionComponent = nullptr;
};
