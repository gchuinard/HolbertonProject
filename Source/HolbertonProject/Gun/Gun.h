// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class UDamageType;

UCLASS()
class HOLBERTONPROJECT_API AGun : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun();

	void FtFire();

protected:
	void FtFireEffect();

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent *GunRoot;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent *GunMesh;

	UPROPERTY(EditAnywhere)
	UParticleSystem *MuzzleFlash;
	UPROPERTY(EditAnywhere)
	USoundBase *MuzzleSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem *ImpactEffect;
	UPROPERTY(EditAnywhere)
	USoundBase *ImpactSound;

	UPROPERTY(EditAnywhere)
	UParticleSystem *ImpactBodyEffect;

	UPROPERTY(EditAnywhere)
	float MaxRange;

	UPROPERTY(EditAnywhere)
	float Damage;
};
