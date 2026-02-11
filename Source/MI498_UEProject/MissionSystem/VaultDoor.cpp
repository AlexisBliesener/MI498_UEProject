#include "VaultDoor.h"

void AVaultDoor::VaultDoorInteract()
{
	/// Broadcast the door interact event
	OnVaultDoorInteract.Broadcast();
}
