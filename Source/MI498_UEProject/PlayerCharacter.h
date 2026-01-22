#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class MI498_UEPROJECT_API APlayerCharacter : public ACharacter
{
public:
	void ToggleSprint();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CUSTOM Player|Movement" );
	int MaxWalkSpeed = 400;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CUSTOM Player|Movement");
	int MaxSprintSpeed = 800;

protected:

private:	
	bool bIsSprinting = false;
	
	GENERATED_BODY()

};
