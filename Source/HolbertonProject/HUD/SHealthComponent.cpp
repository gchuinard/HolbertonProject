// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"
#include "GameFramework/Actor.h"
#include "../HolbertonProjectGameModeBase.h"
#include "../Actor/ProjectileBase.h"
#include "../Character/SCharacter.h"
#include "../Character/SIATrackerBot.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100.f;
	bIsDead = false;
}

float USHealthComponent::FtGetHealth() const
{
	return Health;
}

void USHealthComponent::FtSetHealth(float HP)
{
	Health = HP;
}

float USHealthComponent::FtGetDefaultHealth() const
{
	return DefaultHealth;
}

void USHealthComponent::FtSetDefaultHealth(float HealthMax)
{
	DefaultHealth = HealthMax;
}

bool USHealthComponent::IsFriendly(AActor *ActorA, AActor *ActorB)
{
	if (ActorA && ActorB)
	{
		ASCharacter *PlayerPawnA = nullptr;
		AProjectileBase *PlayerPawnB = nullptr;
		ASIATrackerBot *ASIATrackerBotA = nullptr;
		ASIATrackerBot *ASIATrackerBotB = nullptr;
		if (ActorA->IsA(ASIATrackerBot::StaticClass()))
		{
			ASIATrackerBotA = Cast<ASIATrackerBot>(ActorA);
		}
		else
		{
			PlayerPawnA = Cast<ASCharacter>(ActorA);
		}
		if (ActorB->IsA(ASIATrackerBot::StaticClass()))
		{
			ASIATrackerBotB = Cast<ASIATrackerBot>(ActorB);
		}
		else
		{
			PlayerPawnB = Cast<AProjectileBase>(ActorB);
		}

		if (PlayerPawnA && PlayerPawnB)
		{
			return PlayerPawnA->TeamNum == PlayerPawnB->TeamProjectile;
		}
		else if (ASIATrackerBotA && PlayerPawnB)
		{
			return ASIATrackerBotA->TeamTracker == PlayerPawnB->TeamProjectile;
		}
		else if (PlayerPawnA && ASIATrackerBotB)
		{
			return PlayerPawnA->TeamNum == ASIATrackerBotB->TeamTracker;
		}
		else if (ASIATrackerBotA && ASIATrackerBotB)
		{
			return false;
		}
	}
	return true;
}

// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor *MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::FtHandleTakeAnyDamage);
	}

	Health = DefaultHealth;
}

void USHealthComponent::FtHandleTakeAnyDamage(AActor *DamageActor, float Damage, const class UDamageType *DamageType, class AController *InstigatedBy, AActor *DamageCauser)
{
	if (Damage > 0 && !bIsDead)
	{
		if (DamageCauser != DamageActor && IsFriendly(DamageActor, DamageCauser))
		{
			Damage /= 1.3;
		}
		Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

		bIsDead = Health <= 0.0f;

		OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);

		if (bIsDead)
		{
			AHolbertonProjectGameModeBase *GameModeBase = Cast<AHolbertonProjectGameModeBase>(GetWorld()->GetAuthGameMode());
			if (GameModeBase)
			{
				GameModeBase->OnActorKilled.Broadcast(GetOwner(), DamageCauser, GetWorld()->GetFirstPlayerController());
			}
		}
	}
}
