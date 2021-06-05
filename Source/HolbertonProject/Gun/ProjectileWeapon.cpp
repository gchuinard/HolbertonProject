// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "../HolbertonProject.h"
#include "Net/UnrealNetwork.h"
#include "../Actor/ProjectileBase.h"

// void AProjectileWeapon::FtFire()
// {
//     APawn *OwnerPawn = Cast<APawn>(GetOwner());
//     if (OwnerPawn)
//     {
//         AController *OwnerController = OwnerPawn->GetController();
//         if (OwnerController && ProjectileClass2)
//         {
//             FVector EyeLocation;
//             FRotator EyeRotation;

//             FVector MuzzleLocation;

//             FActorSpawnParameters SpawnParams;
//             if (MuzzleEffect && MuzzleSound)
//             {
//                 UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, GunMesh, MuzzleSocketName);
//                 UGameplayStatics::SpawnSoundAttached(MuzzleSound, GunMesh, MuzzleSocketName);
//             }
//             MuzzleLocation = GunMesh->GetSocketLocation(MuzzleSocketName);
//             SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

//             OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);
//             GetWorld()->SpawnActor<AActor>(ProjectileClass2, MuzzleLocation, EyeRotation, SpawnParams);
//         }
//     }
// }


void AProjectileWeapon::FtFire()
{
	if (!HasAuthority())
	{
		ServerFire();
	}
	if (MuzzleEffect && MuzzleSound)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, GunMesh, MuzzleSocketName);
		UGameplayStatics::SpawnSoundAttached(MuzzleSound, GunMesh, MuzzleSocketName);
	}


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