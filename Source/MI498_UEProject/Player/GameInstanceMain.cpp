#include "GameInstanceMain.h"

#include "MI498_UEProject/ScoringSystem/ScoringManager.h"

void UGameInstanceMain::ResetGame()
{
	UScoringManager* ScoringManager = GetSubsystem<UScoringManager>();
	ScoringManager->ResetScore();
	
	GameRestarted = true;
}

void UGameInstanceMain::Init()
{
	Super::Init();
	
	UScoringManager* ScoringManager = GetSubsystem<UScoringManager>();
	ScoringManager->SetConfig(ScoringData);
}
