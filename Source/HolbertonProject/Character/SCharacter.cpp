// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "../HolbertonProject.h"
#include "../Gun/Gun.h"
#include "../Gun/ProjectileWeapon.h"
#include "../HUD/SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASCharacter::ASCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->SetRelativeLocation(FVector(0.f, 100.f, 80.f));
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->DefaultHealth = 100.f;
	HealthComp->Health = HealthComp->DefaultHealth;

	WalkingSpeed = 250.f;
	RunningSpeed = 1500.f;
	MaxStamina = 1.f;
	Stamina = MaxStamina;
	bRest = true;
	bBlockSprint = false;

	JumpCount = 1;
	JumpMax = 1;
	JumpHeight = 500.f;

	GrenadeDelay = 1.f;

	bGun = true;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
	
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}

void ASCharacter::FtMoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::FtMoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::FtSprint()
{
	bRest = false;
	if (Stamina > 0.01 && !bBlockSprint)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		Stamina = FMath::ClampAngle(Stamina - 0.02f, 0.0f, MaxStamina);
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
		bBlockSprint = true;
	}
}

void ASCharacter::FtWalk()
{
	bRest = true;
	GetCharacterMovement()->MaxWalkSpeed = WalkingSpeed;
}

void ASCharacter::FtJump()
{
	if (JumpCount <= JumpMax)
	{
		ASCharacter::LaunchCharacter(FVector(0, 0, JumpHeight), false, true);
		JumpCount++;
	}
}

void ASCharacter::Landed(const FHitResult &Hit)
{
	JumpCount = 1;
}

void ASCharacter::FtFire()
{
	Gun->FtFire();
}

// void ASCharacter::FtSwitchWeapon()
// {
// 	if (Gun)
// 	{
// 		Gun->Destroy();
// 	}
// 	if (bGun)
// 	{
// 		Gun = GetWorld()->SpawnActor<AProjectileWeapon>(ProjectileWeaponClass);
// 		bGun = false;
// 	}
// 	else
// 	{
// 		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
// 		bGun = true;
// 	}
// 	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
// }

void ASCharacter::OnHealthChanged(USHealthComponent *InHealthComp, float Health, float HealthDelta, const class UDamageType *DamageType, class AController *InstigatedBy, AActor *DamageCauser)
{
	if (Health <= 0.0f && !bDead)
	{
		bDead = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);
	}
}

void ASCharacter::FtGrenadeDelay()
{
	if (GrenadeDelay < 3)
	{
		GrenadeDelay += 1;
	}
	else
	{
		GrenadeDelay = 1;
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Stamina <= 0)
	{
		ASCharacter::FtWalk();
	}
	if (Stamina < MaxStamina && bRest)
	{
		Stamina = FMath::ClampAngle(Stamina + 0.003, 0.0f, MaxStamina);
		if (Stamina >= 0.25f)
		{
			bBlockSprint = false;
		}
	}
	if (!bDead && HealthComp->Health < HealthComp->DefaultHealth)
	{
		HealthComp->Health = FMath::ClampAngle(HealthComp->Health + 0.05, 0.0f, HealthComp->DefaultHealth);
	}
}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::FtMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::FtMoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::FtJump);

	if (!bBlockSprint)
	{
		PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::FtSprint);
		PlayerInputComponent->BindAction("Sprint", IE_Repeat, this, &ASCharacter::FtSprint);
	}
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::FtWalk);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::FtFire);

	PlayerInputComponent->BindAction("GrenadeDelay", IE_Pressed, this, &ASCharacter::FtGrenadeDelay);

	// PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &ASCharacter::FtSwitchWeapon);
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, Gun);
}