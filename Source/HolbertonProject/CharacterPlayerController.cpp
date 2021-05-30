// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterPlayerController.h"
#include "Blueprint/UserWidget.h"

void ACharacterPlayerController::BeginPlay() 
{
    Super::BeginPlay();

    UUserWidget *HUD = CreateWidget(this, HUDClass);
    if (HUD != nullptr)
    {
        HUD->AddToViewport();
    }
    UE_LOG(LogTemp, Warning, TEXT("NULLWidget"));
}
