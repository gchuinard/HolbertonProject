// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"
#include "../HolbertonProject.h"

// Sets default values
AGun::AGun()
{
	GunRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GunRoot"));
	SetRootComponent(GunRoot);

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(GunRoot);

	MuzzleSocketName = TEXT("MuzzleFlashSocket");

	MaxRange = 5000.f;
	Damage = 34.f;
}

void AGun::FtFire()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GunMesh, MuzzleSocketName);
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, GunMesh, MuzzleSocketName);

	// if (Bullet)
	// {
	// 	FVector MuzzleLocation = GunMesh->GetSocketLocation(MuzzleSocketName);
	// 	UParticuleSystemComponent *BulletComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Bullet, MuzzleLocation);
	// 	if (BulletComp)
	// 	{
	// 		BulletComp->SetVectorParameter("Bullet")
	// 	}

	// }

	APawn *OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn)
	{
		AController *OwnerController = OwnerPawn->GetController();
		if (OwnerController)
		{
			FVector Location;
			FRotator Rotation;
			FHitResult Hit;
			bool bHitSuccess;
			FVector ShotDirection;
			AActor *HitActor;
			FVector End;

			OwnerController->GetPlayerViewPoint(Location, Rotation);
			End = Location + (Rotation.Vector() * MaxRange);
			bHitSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::COLLISION_WEAPON);
			if (bHitSuccess)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
				ShotDirection = -Rotation.Vector();

				HitActor = Hit.GetActor();
				if (HitActor)
				{
					FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
					HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
				}
				else
				{
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactBodyEffect, Hit.Location, ShotDirection.Rotation());
				}
			}
		}
	}
}

void AGun::FtFireEffect()
{
}
