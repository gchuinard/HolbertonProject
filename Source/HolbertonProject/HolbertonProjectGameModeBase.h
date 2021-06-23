// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HolbertonProjectGameModeBase.generated.h"


enum class EWaveState : uint8;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

/**
 * 
 */
UCLASS()
class HOLBERTONPROJECT_API AHolbertonProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AHolbertonProjectGameModeBase();

protected:
	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 NbrOfBotsToSpawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	int32 WaveCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
	bool bHorde;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimerBetweenWaves;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void FtSpawnNewBot();

	void FtSpawnBotTimerElapsed();

	void FtStartWave();

	void FtEndWave();

	void FtPrepareForNextWave();

	void FtCheckWaveState();

	void FtCheckAnyPlayerAlive();

	void FtGameOver();

	void FtSetWaveState(EWaveState NewState);

public:
	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable, Category = "GameMode")
	FOnActorKilled OnActorKilled;
};
