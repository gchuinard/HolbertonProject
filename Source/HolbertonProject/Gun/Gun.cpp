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

	SetReplicates(true);
}

void AGun::FtFire()
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

			FVector SpawnLocation = ProjectileSpawnPoint->GetComponentLocation();
			AProjectileBase *TempProjectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, SpawnLocation, EyeRotation);

			TempProjectile->SetOwner(this);
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
			PlayerController->ClientPlayCameraShake(FireCamShake);
		}
	}
}

void AGun::OnRep_HitScanTrace()
{
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