// Fill out your copyright notice in the Description page of Project Settings.

#include "Gun.h"

#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGun::AGun()
{
	GunRoot = CreateDefaultSubobject<USceneComponent>(TEXT("GunRoot"));
	SetRootComponent(GunRoot);

	GunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	GunMesh->SetupAttachment(GunRoot);

	MaxRange = 5000.f;
	Damage = 34.f;
}

void AGun::FtFire()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, GunMesh, TEXT("MuzzleFlashSocket"));
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, GunMesh, TEXT("MuzzleFlashSocket"));

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
			bHitSuccess = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1);
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
