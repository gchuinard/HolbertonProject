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
			FVector ShotDirection;

			FVector TraceEnd;

			OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);
			ShotDirection = EyeRotation.Vector();
			TraceEnd = EyeLocation + (EyeRotation.Vector() * MaxRange);

			FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
			FRotator SpawnRotation = ProjectileSpawnPoint->GetComponentRotation();
			AProjectileBase *TempProjectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, SpawnLocation, EyeRotation);

			TempProjectile->SetOwner(this);
		}
	}
}