// Fill out your copyright notice in the Description page of Project Settings.

#include "SHealthComponent.h"
#include "GameFramework/Actor.h"
#include "../HolbertonProjectGameModeBase.h"

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
