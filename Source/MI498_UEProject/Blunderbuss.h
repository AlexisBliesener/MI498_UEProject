// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "WeaponInterface.h"
#include "GameFramework/Actor.h"
#include "Blunderbuss.generated.h"

class APlayerController;

UCLASS(Blueprintable)
class MI498_UEPROJECT_API ABlunderbuss : public AWeaponBase
{
	GENERATED_BODY()
	
public:	
	virtual void Fire(APlayerController* PlayerController) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int KnockbackForce = 500;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int CameraRecoil = -5;
	
	
protected:

public:	

};
