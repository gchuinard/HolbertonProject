// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;
class AProjectileBase;
class ASCharacter;

USTRUCT()
struct FHitScanTraceGun
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;
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
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void FtFireAuto();

	void FtFireEffect();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

protected:

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

	UPROPERTY(EditAnywhere)
	USoundBase *EmptyAmmoSound;

	UPROPERTY(EditAnywhere)
	float MaxRange;

	UPROPERTY(EditAnywhere)
	float Damage;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTraceGun HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();


	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UCameraShakeBase> FireCamShake;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 Ammo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	int32 FullMag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bAuto;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	bool bCanFire;

	void FtReloadAmmo();

	float FtSerie();
	int32 Serie;
	bool bSerie;

	FTimerHandle TimerHandle_ReloadTime;

public:

	bool FtGetbAuto();
	void FtSetbAuto(bool Auto);

	int32 FtGetFullMag();
	void FtSetFullMag(int32 DefaultAmmo);

	int32 FtGetAmmo();
	UFUNCTION()
	void FtSetAmmo(int32 NewAmmo);

	bool FtGetbCanFire();
	void FtSetbCanFire(bool Auto);
	
	void FtReload();

	bool FtGetbSerie();
	void FtSetbSerie(bool InSerie);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Team")
	uint8 TeamGun;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin=0.0f))
	float BulletSpread;
};
