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
	if (MuzzleEffect && MuzzleSound && ShotEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, GunMesh, MuzzleSocketName);
		UGameplayStatics::SpawnEmitterAttached(ShotEffect, GunMesh, ShotSocketName);
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
			AProjectileBase *TempProjectile = GetWorld()->SpawnActor<AProjectileBase>(ProjectileClass, SpawnLocation, SpawnRotation);

			TempProjectile->SetOwner(this);
		}
		if (OwnerController)
		{

			// FVector EyeLocation;
			// FRotator EyeRotation;
			// FVector ShotDirection;

			// FHitResult Hit;
			// bool bHitSuccess;
			// AActor *HitActor;

			// FVector TraceEnd;

			// OwnerController->GetPlayerViewPoint(EyeLocation, EyeRotation);
			// ShotDirection = EyeRotation.Vector();
			// TraceEnd = EyeLocation + (EyeRotation.Vector() * MaxRange);
			// bHitSuccess = GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECollisionChannel::COLLISION_WEAPON);

			// if (bHitSuccess && ImpactEffect)
			// {
			// 	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
			// 	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);
			// 	ShotDirection = -EyeRotation.Vector();

			// 	HitActor = Hit.GetActor();
			// 	if (HitActor)
			// 	{
			// 		FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			// 		HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
			// 	}
			// }
			// if (HasAuthority())
			// {
			// 	HitScanTrace.TraceTo = TraceEnd;
			// }
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