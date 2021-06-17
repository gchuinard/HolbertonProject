// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Particles/ParticleSystem.h"
#include "SIATrackerBot.generated.h"


class USHealthComponent;
class USphereComponent;

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

	void FtSetupMesh();

	void FtSetupHealth();

	void FtSetupSphere();

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent *MeshComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USHealthComponent *HealthComp;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent *SphereComp;

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
	FVector ForceDirection;

	UMaterialInstanceDynamic *MatInst;

	bool bshot;


	void FtSelfDestruct();
	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	UParticleSystem *ExplosionEffect;
	bool bExploded;
	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	float ExplosionRadius;
	UPROPERTY(EditDefaultsOnly, Category="TrackerBot")
	float ExplosionDamage;

	FTimerHandle TimerHandle_SelfDamage;
	bool bStartedSelfDestruction;

	void DamageSelf();

	UPROPERTY(EditAnywhere)
	USoundBase *ExplosionSound;
	UPROPERTY(EditAnywhere)
	USoundBase *BeepSound;

	FTimerHandle TimerHandle_RefreshPath;

	void FtRefreshPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor *OtherActor) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Team")
	uint8 TeamTracker;

};
