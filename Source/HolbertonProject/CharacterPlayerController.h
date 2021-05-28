// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CharacterPlayerController.generated.h"

/**
 * 
 */
class UUserWidget;

UCLASS()
class HOLBERTONPROJECT_API ACharacterPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HUDClass;
};
