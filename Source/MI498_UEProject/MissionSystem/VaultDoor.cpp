#include "VaultDoor.h"

void AVaultDoor::VaultDoorInteract()
{
	OnVaultDoorInteract.Broadcast();
	Destroy();
}
