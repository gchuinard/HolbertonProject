// Copyright Epic Games, Inc. All Rights Reserved.


#include "HolbertonProjectGameModeBase.h"

AHolbertonProjectGameModeBase::AHolbertonProjectGameModeBase() 
{
    TimerBetweenWaves = 2.0f;
}

void AHolbertonProjectGameModeBase::StartWave() 
{
    WaveCount++;
    NbrOfBotsToSpawn = 2 * WaveCount;
    GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AHolbertonProjectGameModeBase::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
}

void AHolbertonProjectGameModeBase::EndWave() 
{
    GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

    PrepareForNextWave();
}

void AHolbertonProjectGameModeBase::PrepareForNextWave() 
{
    FTimerHandle TimerHandle_NextWaveStart;
    GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AHolbertonProjectGameModeBase::StartWave, TimerBetweenWaves, false);
}

void AHolbertonProjectGameModeBase::StartPlay() 
{
    Super::StartPlay();

    PrepareForNextWave();
}



void AHolbertonProjectGameModeBase::SpawnBotTimerElapsed() 
{
    SpawnNewBot();
    NbrOfBotsToSpawn--;
    if (NbrOfBotsToSpawn <= 0)
    {
        EndWave();
    }
}
