// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "../HolbertonProject.h"
#include "Net/UnrealNetwork.h"
#include "../Actor/ProjectileBase.h"

void AProjectileWeapon::FtFire()
{

	UE_LOG(LogTemp, Warning, TEXT("Ammo Greande = %i"), Ammo)
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

				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
				AProjectileBase *TempProjectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);

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