// Fill out your copyright notice in the Description page of Project Settings.

#include "SIATrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "../HUD/SHealthComponent.h"
#include "Engine/EngineTypes.h"

// Sets default values
ASIATrackerBot::ASIATrackerBot()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASIATrackerBot::FtHandleTakeDamage);

	bUseVelocityChange = false;
	MovementForce = 1000;

	RequiredDistanceToTarget = 250;
	bshot = false;
}

// Called when the game starts or when spawned
void ASIATrackerBot::BeginPlay()
{
	Super::BeginPlay();

	NextPathPoint = GetNextPathPoint();
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
		UE_LOG(LogTemp, Log, TEXT("Health %f"), Health);
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}
}

FVector ASIATrackerBot::GetNextPathPoint()
{
	ACharacter *PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath *NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}
	return GetActorLocation();
}

// Called every frame
void ASIATrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if (DistanceToTarget <= RequiredDistanceToTarget)
	{
		NextPathPoint = GetNextPathPoint();
	}
	else
	{
		if (bshot)
		{
			int i = 100;
			while (i > 0)
				{
					MovementForce = -1000;
					ForceDirection = NextPathPoint - GetActorLocation();
					ForceDirection.Normalize();
					ForceDirection *= MovementForce;
					i--;
					MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
				}
				bshot = false;
		}
		MovementForce = 1000;
		ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();
		ForceDirection *= MovementForce;

		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
	}
}