// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "../HolbertonProject.h"
#include "Net/UnrealNetwork.h"
#include "UObject/Object.h"
#include "../Actor/ProjectileBase.h"

// Sets default values
AGun::AGun()
{
	GunRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GunRoot"));
	SetRootComponent(GunRoot);

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(GunRoot);

	MuzzleSocketName = TEXT("MuzzleSocket");
	ShotSocketName = TEXT("ShotSocket");

	MaxRange = 5000.f;
	Damage = 34.f;

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(GunMesh);

	FullMag = 30;
	Ammo = FullMag;
	bAuto = false;
	bCanFire = true;

	SetReplicates(true);
}

void AGun::FtFire()
{
	UE_LOG(LogTemp, Warning, TEXT("Ammo Rifle = %i"), Ammo)
	if (Ammo > 0 && bCanFire)
	{
		if (!HasAuthority())
		{
			ServerFire();
		}
		
		FtFireEffect();
		APawn *OwnerPawn = Cast<APawn>(GetOwner());
		if (OwnerPawn)
		{
			AController *OwnerController = OwnerPawn->GetController();

			if (OwnerController && ProjectileClass)
			{
				FVector EyeLocation;
				FRotator EyeRotation;

				OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);

				FVector MuzzleLocation = GunMesh->GetSocketLocation(MuzzleSocketName);

				FVector ShotDirection = EyeRotation.Vector();

				//FActorSpawnParameters SpawnParams;
				//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
				AProjectileBase *TempProjectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, MuzzleLocation, EyeRotation);

				TempProjectile->SetOwner(this);
				Ammo--;
			}
		}
	}
	else 
	{
		if (EmptyAmmoSound)
		{
			UGameplayStatics::SpawnSoundAttached(EmptyAmmoSound, GunMesh, MuzzleSocketName);
		}
	}
}

void AGun::FtFireEffect()
{
	if (MuzzleEffect && MuzzleSound)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, GunMesh, MuzzleSocketName);
		UGameplayStatics::SpawnSoundAttached(MuzzleSound, GunMesh, MuzzleSocketName);
	}
	

	APawn *MyOwner = Cast<APawn>(GetOwner());

	if (MyOwner)
	{
		APlayerController* PlayerController = Cast<APlayerController>(MyOwner->GetController());
		if (PlayerController)
		{
			PlayerController->ClientStartCameraShake(FireCamShake);
		}
	}
}

void AGun::OnRep_HitScanTrace()
{
	
}

bool AGun::FtGetbCanFire() 
{
	return bCanFire;
}

void AGun::FtSetbCanFire(bool CanFire) 
{
	bCanFire = CanFire;
}

void AGun::FtReload() 
{
	bCanFire = false;
	GetWorldTimerManager().SetTimer(TimerHandle_ReloadTime, this, &AGun::FtReloadAmmo, 3.0f, false, 3.0f);
}

void AGun::FtReloadAmmo() 
{
	Ammo = FtGetFullMag();
	bCanFire = true;
}

bool AGun::FtGetbAuto() 
{
	return bAuto;
}

void AGun::FtSetbAuto(bool Auto) 
{
	bAuto = Auto;
}

int32 AGun::FtGetAmmo() 
{
	return Ammo;
}

void AGun::FtSetAmmo(int32 NewAmmo) 
{
	Ammo = NewAmmo;
	bCanFire = true;
}

int32 AGun::FtGetFullMag() 
{
	return FullMag;
}

void AGun::FtSetFullMag(int32 DefaultAmmo) 
{
	FullMag = DefaultAmmo;
}

void AGun::ServerFire_Implementation()
{
	FtFire();
}

bool AGun::ServerFire_Validate()
{
	return true;
}

void AGun::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AGun, HitScanTrace, COND_SkipOwner);
}