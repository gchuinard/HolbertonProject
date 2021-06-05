// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class AProjectileBase;

USTRUCT()
struct FHitScanTrace
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FVector_NetQuantize TraceFrom;
	UPROPERTY()
	FVector_NetQuantize TraceTo;
};

UCLASS()
class HOLBERTONPROJECT_API AGun : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGun();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void FtFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

protected:
	// UFUNCTION(BlueprintCallable, Category = "Weapon")
	// void FtFireEffect();

	UPROPERTY(VisibleAnywhere)
	USceneComponent *GunRoot;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Component")
	USkeletalMeshComponent *GunMesh;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem *MuzzleEffect;
	UPROPERTY(EditAnywhere)
	USoundBase *MuzzleSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* ProjectileSpawnPoint;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Type", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AProjectileBase> ProjectileClass;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName ShotSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem *ShotEffect;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem *ImpactEffect;
	UPROPERTY(EditAnywhere)
	USoundBase *ImpactSound;

	UPROPERTY(EditAnywhere)
	float MaxRange;

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();
};
