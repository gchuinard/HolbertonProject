// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "../HolbertonProject.h"
#include "Net/UnrealNetwork.h"
#include "UObject/Object.h"
#include "GameFramework/Character.h"
#include "../Character/SCharacter.h"
#include "../Actor/ProjectileBase.h"

// Sets default values
AGun::AGun()
{
	GunRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GunRoot"));
	SetRootComponent(GunRoot);

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(GunRoot);

	MuzzleSocketName = TEXT("MuzzleSocket");

	MaxRange = 5000.f;
	Damage = 34.f;

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(GunMesh);

	FullMag = 30;
	Ammo = FullMag;
	bAuto = false;
	bCanFire = true;

	BulletSpread = 1.0f;

	SetReplicates(true);
}

void AGun::FtFireAuto()
{
	if (!bSerie)
	{
		Serie = 0;
		bSerie = true;
	}
	Serie++;
	FtFire();
}

void AGun::FtFire()
{
	if (Ammo > 0 && bCanFire)
	{
		if (!HasAuthority())
		{
			ServerFire();
		}

		APawn *OwnerPawn = Cast<APawn>(GetOwner());

		if (OwnerPawn)
		{
			AController *OwnerController = OwnerPawn->GetController();

			if (OwnerController && ProjectileClass)
			{
				FHitResult Hit;
				AActor *HitActor;

				FVector EyeLocation;
				FRotator EyeRotation;
				OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);

				FCollisionQueryParams QueryParams;
				QueryParams.AddIgnoredActor(OwnerPawn);
				QueryParams.AddIgnoredActor(this);
				QueryParams.AddIgnoredActor(OwnerPawn->GetInstigatorController());
				QueryParams.bReturnPhysicalMaterial = true;

				FVector ShotDirection = EyeRotation.Vector();
				FVector TraceEnd = EyeLocation + (ShotDirection * 15000);
				if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
				{
					HitActor = Hit.GetActor();
				}
				FVector MuzzleLocation = GunMesh->GetSocketLocation(MuzzleSocketName);

				if (bAuto)
				{
					BulletSpread = FtSerie();
				}
				float HalfRad = FMath::DegreesToRadians(BulletSpread);
				ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);
				EyeRotation = ShotDirection.Rotation();

				//FActorSpawnParameters SpawnParams;
				//SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
				FtFireEffect();
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
		APlayerController *PlayerController = Cast<APlayerController>(MyOwner->GetController());
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

bool AGun::FtGetbSerie()
{
	return bSerie;
}

void AGun::FtSetbSerie(bool InSerie)
{
	Serie = InSerie;
}

void AGun::FtReloadAmmo()
{
	Ammo = FtGetFullMag();
	bCanFire = true;
}

float AGun::FtSerie()
{
	switch (Serie)
	{
	case 0:
	case 1:
	case 2:
		return 1.5f;
	case 3:
	case 4:
		return 2.5f;
	case 5:
	case 6:
		return 3.0f;
	case 7:
	case 8:
		return 4.0f;
	case 9:
	case 10:
		return 5.0f;
	default:
		return 6.0f;
	}
	return 1.0f;
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