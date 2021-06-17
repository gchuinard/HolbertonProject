// Copyright Epic Games, Inc. All Rights Reserved.

#include "HolbertonProjectGameModeBase.h"
#include "HUD/SHealthComponent.h"
#include "TimerManager.h"
#include "EngineUtils.h"
#include "SGameStateBase.h"
#include "SPlayerState.h"

AHolbertonProjectGameModeBase::AHolbertonProjectGameModeBase()
{
    TimerBetweenWaves = 2.0f;

    GameStateClass = ASGameStateBase::StaticClass();
    PlayerStateClass = ASPlayerState::StaticClass();

    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.TickInterval = 2.0f;
}

void AHolbertonProjectGameModeBase::FtStartWave()
{
    WaveCount++;
    NbrOfBotsToSpawn = 2 * WaveCount;
    GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AHolbertonProjectGameModeBase::FtSpawnBotTimerElapsed, 0.1f, true, 10.0f);

    FtSetWaveState(EWaveState::WaveInProgress);
}

void AHolbertonProjectGameModeBase::FtEndWave()
{
    GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

    FtSetWaveState(EWaveState::WaitingToComplete);
}

void AHolbertonProjectGameModeBase::FtPrepareForNextWave()
{
    GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AHolbertonProjectGameModeBase::FtStartWave, TimerBetweenWaves, false);

    FtSetWaveState(EWaveState::WaitingToStart);
}

void AHolbertonProjectGameModeBase::FtCheckWaveState()
{
    bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

    if (NbrOfBotsToSpawn <= 0 && !bIsPreparingForWave)
    {
        bool bIsAnyBotAlive = false;
        for (TActorIterator<APawn> It(GetWorld()); It; ++It)
        {
            APawn *TestPawn = *It;
            if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
            {
                continue;
            }

            USHealthComponent *HealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
            if (HealthComp && HealthComp->FtGetHealth() > 0.0f)
            {
                bIsAnyBotAlive = true;
                break;
            }
        }
        if (!bIsAnyBotAlive)
        {
           FtSetWaveState(EWaveState::WaveComplete);

            FtPrepareForNextWave();
        }
    }
}

void AHolbertonProjectGameModeBase::FtCheckAnyPlayerAlive()
{
    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        APlayerController *PlayerController = It->Get();

        if (PlayerController && PlayerController->GetPawn())
        {
            APawn *MyPawn = PlayerController->GetPawn();
            USHealthComponent *HealthComp = Cast<USHealthComponent>(MyPawn->GetComponentByClass(USHealthComponent::StaticClass()));
            if (ensure(HealthComp) && HealthComp->FtGetHealth() > 0.0f)
            {
                return;
            }
        }
    }
    FtGameOver();
}

void AHolbertonProjectGameModeBase::FtGameOver()
{
    FtEndWave();

    FtSetWaveState(EWaveState::GameOver);
}

void AHolbertonProjectGameModeBase::FtSetWaveState(EWaveState NewState) 
{
    ASGameStateBase *GameStateBase = GetGameState<ASGameStateBase>();

    if (ensure(GameStateBase))
    {
        GameStateBase->FtSetWaveState(NewState);
    }
}

void AHolbertonProjectGameModeBase::StartPlay()
{
    Super::StartPlay();
    FtPrepareForNextWave();
}

void AHolbertonProjectGameModeBase::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    FtCheckWaveState();
    FtCheckAnyPlayerAlive();
}

void AHolbertonProjectGameModeBase::FtSpawnBotTimerElapsed()
{
    FtSpawnNewBot();
    NbrOfBotsToSpawn--;
    if (NbrOfBotsToSpawn <= 0)
    {
        FtEndWave();
    }
}
