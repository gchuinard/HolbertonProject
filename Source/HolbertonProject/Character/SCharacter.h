// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class AGun;
class AProjectileWeapon;
class USHealthComponent;

UCLASS()
class HOLBERTONPROJECT_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent *SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent *CameraComp;

	void FtMoveForward(float Value);
	void FtMoveRight(float Value);

	UFUNCTION()
	void FtSprint();
	UFUNCTION()
	void FtWalk();
	UPROPERTY(EditAnywhere)
	float WalkingSpeed;
	UPROPERTY(EditAnywhere)
	float RunningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Stamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bRest;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bBlockSprint;

	int numb = 3;

	UFUNCTION()
	void FtJump();
	UPROPERTY()
	int JumpCount;
	UPROPERTY()
	int JumpMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float JumpHeight;

	virtual void Landed(const FHitResult &Hit) override;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AGun> GunClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectileWeapon> ProjectileWeaponClass;
	UPROPERTY(Replicated)
	AGun *Gun;

	bool bGun;


	void FtFire();

	void FtSwitchWeapon();

	bool bZoom;
	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomFOV;
	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1f, ClampMax = 100.0f))
	float ZoomInterpSpeed;
	float DefaultFOV;

	void FtZoom();

	USHealthComponent *HealthComp;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent *InHealthComp, float Health, float HealthDelta, const class UDamageType *DamageType, class AController *InstigatedBy, AActor *DamageCauser);

public:
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDead;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;
};
