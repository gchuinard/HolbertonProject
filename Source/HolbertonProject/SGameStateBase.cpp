// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameStateBase.h"
#include "Net/UnrealNetwork.h"


void ASGameStateBase::OnRep_WaveState(EWaveState OldState) 
{
    FtWaveStateChanged(WaveState, OldState);
}

void ASGameStateBase::FtSetWaveState(EWaveState NewState) 
{
        EWaveState OldState = WaveState;
        WaveState = NewState;
        OnRep_WaveState(OldState);
}

void ASGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASGameStateBase, WaveState);
}