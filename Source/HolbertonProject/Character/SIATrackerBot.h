// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SIATrackerBot.generated.h"


class USHealthComponent;

UCLASS()
class HOLBERTONPROJECT_API ASIATrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASIATrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent *MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent *HealthComp;

	UFUNCTION()
	void FtHandleTakeDamage(USHealthComponent *InHealthComp, float Health, float HealthDelta, const class UDamageType *DamageType, class AController *InstigatedBy, AActor *DamageCauser);

	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	bool bUseVelocityChange;
	
	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	float RequiredDistanceToTarget;

	UMaterialInstanceDynamic *MatInst;

	bool bshot;
	FVector ForceDirection;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
