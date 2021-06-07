// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HolbertonProjectGameModeBase.generated.h"

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

	int32 NbrOfBotsToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimerBetweenWaves;

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	void StartWave();

	void EndWave();

	void PrepareForNextWave();

public:
	virtual void StartPlay() override;

};
