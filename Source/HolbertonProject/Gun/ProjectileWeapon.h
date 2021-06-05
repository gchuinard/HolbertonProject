// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "ProjectileWeapon.generated.h"

class UParticleSystem;

/**
 * 
 */
UCLASS()
class HOLBERTONPROJECT_API AProjectileWeapon : public AGun
{
	GENERATED_BODY()

	protected:

	virtual void FtFire() override;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon2")
	TSubclassOf<AActor>	ProjectileClass2;
};
