// Fill out your copyright notice in the Description page of Project Settings.

#include "SIATrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "../HUD/SHealthComponent.h"
#include "Engine/EngineTypes.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "Engine/EngineTypes.h"

// Sets default values
ASIATrackerBot::ASIATrackerBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FtSetupMesh();
	
	FtSetupHealth();

	FtSetupSphere();

	bUseVelocityChange = false;
	MovementForce = 1000.0f;

	RequiredDistanceToTarget = 250.0f;
	bshot = false;

	ExplosionDamage = 85.0f;
	ExplosionRadius = 800.0f;

	bStartedSelfDestruction = false;
}

// Called when the game starts or when spawned
void ASIATrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NextPathPoint = GetNextPathPoint();
	}
}

void ASIATrackerBot::FtSetupMesh() 
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;
}

void ASIATrackerBot::FtSetupHealth() 
{
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->FtSetDefaultHealth(45.0f);
	HealthComp->FtSetHealth(HealthComp->FtGetDefaultHealth());
	HealthComp->OnHealthChanged.AddDynamic(this, &ASIATrackerBot::FtHandleTakeDamage);
}

void ASIATrackerBot::FtSetupSphere()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(400);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);
}

void ASIATrackerBot::FtHandleTakeDamage(USHealthComponent *InHealthComp, float Health, float HealthDelta, const class UDamageType *DamageType, class AController *InstigatedBy, AActor *DamageCauser)
{
	bshot = true;
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}
	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), BeepSound, GetActorLocation());
	}
	if (Health <= 0.f)
	{
		FtSelfDestruct();
	}
}

FVector ASIATrackerBot::GetNextPathPoint()
{
	ACharacter *PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerPawn)
	{
		UNavigationPath *NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

		if (NavPath && NavPath->PathPoints.Num() > 1)
		{
			return NavPath->PathPoints[1];
		}
		return GetActorLocation();
	}
	return {};
}

void ASIATrackerBot::FtSelfDestruct()
{
	if (!bExploded)
	{
		TArray<AActor *> IgnoredActors;

		bExploded = true;
		IgnoredActors.Add(this);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), false);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSound, GetActorLocation());
		Destroy();
	}
}

void ASIATrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 10, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ASIATrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{

		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
		}
		else
		{
			if (bshot && !bStartedSelfDestruction)
			{
				int i = 100;
				while (i > 0)
				{
					MovementForce = -500.0f;
					ForceDirection = NextPathPoint - GetActorLocation();
					ForceDirection.Normalize();
					ForceDirection *= MovementForce;
					i--;
					MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
				}
				bshot = false;
			}
			MovementForce = 1000.0f;
			ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		}
	}
}

void ASIATrackerBot::NotifyActorBeginOverlap(AActor *OtherActor)
{
	if (!bStartedSelfDestruction)
	{
		ASCharacter *PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASIATrackerBot::DamageSelf, 0.35f, true, 0.0f);
			bStartedSelfDestruction = true;
		}
	}
}