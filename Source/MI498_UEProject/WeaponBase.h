#pragma once

#include "CoreMinimal.h"
#include "WeaponInterface.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class MI498_UEPROJECT_API AWeaponBase : public AActor,  public IWeaponInterface
{
public:	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly) 
	int Range = 1000;

protected:
	virtual void Fire(APlayerController* PlayerController) override;

private:	
	GENERATED_BODY()

};
