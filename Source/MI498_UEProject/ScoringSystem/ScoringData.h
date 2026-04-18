#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ScoringData.generated.h"

UCLASS(BlueprintType)
class MI498_UEPROJECT_API UScoringData : public UDataAsset
{
    GENERATED_BODY()

public:
    // === Core Score Values ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int BombPieceScore = 5000;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int OpenVaultScore = 10000;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int FinishLevelScore = 10000;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int VaultSecScore = 500;

    // === Enemy Scores ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int AverageEnemyKillScore = 500;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int BruteKillScore = 1000;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int SwingerKillScore = 1000;

    // === Misc Scores ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int AirtimeScore = 100;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int BreakableWallScore = 1000;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int InAirKillBonus = 100;

    // === Multipliers ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float AfterBombPlantGlobalMult = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float MidairKillModifier = 1.5f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ComboKillModifier = 0.2f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BombBarrelKillModifier = 1.5f;

    // === Timing ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float SecToUpdateOnScreenScore = 0.001f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float SecToAddAirtime = 1.0f;

    // === Rank Thresholds ===
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float CRankScore = 25000;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float BRankScore = 40000;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ARankScore = 55000;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float SRankScore = 75000;
};