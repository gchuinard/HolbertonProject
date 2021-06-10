// Fill out your copyright notice in the Description page of Project Settings.

#include "ProjectileBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "../HolbertonProject.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileMovement->InitialSpeed = MovementSpeed;
	ProjectileMovement->MaxSpeed = MovementSpeed;
	InitialLifeSpan = 3.0f;

	ParticleTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle Trail"));
	ParticleTrail->SetupAttachment(RootComponent);

	bRifle = true;

	SetReplicates(true);
	bNetLoadOnClient = true;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
}

void AProjectileBase::OnHit(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	if (!HasAuthority())
	{
		ServerOnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
	}

	AActor *MyOwner = GetOwner();

	if (MyOwner)
	{
		if (OtherActor && OtherActor != this && OtherActor != MyOwner)
		{
			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);
			QueryParams.bReturnPhysicalMaterial = true;

			if (bRifle)
			{
				UGameplayStatics::ApplyDamage(OtherActor, Damage, MyOwner->GetInstigatorController(), this, DamageType);
			}
			else
			{
				UGameplayStatics::ApplyRadialDamage(GetWorld(), Damage, GetActorLocation(), DamageRadius, DamageType, TArray<AActor *>(), this, MyOwner->GetInstigatorController(), false, ECollisionChannel::ECC_Visibility);
			}

			EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());
			FtImpactEffect(SurfaceType);
			Destroy();
		}
	}
}

void AProjectileBase::FtImpactEffect(EPhysicalSurface SurfaceType)
{
	UParticleSystem *SelectedEffect = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
	case SURFACE_FLESHSOFTLOW:
	case SURFACE_FLESHSOFTHIGHT:
		SelectedEffect = FleshHitParticle;
		break;
	default:
		SelectedEffect = DefaultHitParticle;
		break;
	}

	if (SelectedEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, SelectedEffect, GetActorLocation());
	}
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
	GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitShake);
}

void AProjectileBase::ServerOnHit_Implementation(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}

bool AProjectileBase::ServerOnHit_Validate(UPrimitiveComponent *HitComp, AActor *OtherActor, UPrimitiveComponent *OtherComp, FVector NormalImpulse, const FHitResult &Hit)
{
	return true;
}