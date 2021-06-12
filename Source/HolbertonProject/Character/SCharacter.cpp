// Fill out your copyright notice in the Description page of Project Settings.

#include "SCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "../HolbertonProject.h"
#include "../Gun/Gun.h"
#include "../Gun/ProjectileWeapon.h"
#include "../HUD/SHealthComponent.h"

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
	HealthComp->FtSetDefaultHealth(100.0f);
	HealthComp->FtSetHealth(HealthComp->FtGetDefaultHealth());

	WalkingSpeed = 250.0f;
	RunningSpeed = 1500.0f;
	MaxStamina = 1.f;
	Stamina = MaxStamina;
	bRest = true;
	bBlockSprint = false;

	JumpCount = 0;
	JumpMax = 1;
	JumpHeight = 500.0f;

	bGun = false;

	ZoomFOV = 45.0f;
	ZoomInterpSpeed = 20.0f;

	BulletLeft = -1;
	GrenadeLeft = -1;

	bAuto = false;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);

	DefaultFOV = CameraComp->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);

	if (HasAuthority())
	{
		FtSwitchWeapon();
	}
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
	if (JumpCount < JumpMax)
	{
		if (JumpSound)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, JumpSound, GetActorLocation());
		}
		ASCharacter::LaunchCharacter(FVector(0, 0, JumpHeight), false, true);
		JumpCount++;
	}
}

void ASCharacter::Landed(const FHitResult &Hit)
{
	JumpCount = 0;
}

void ASCharacter::FtPullTrigger()
{
	if (Gun && !Gun->FtGetbAuto())
	{
		Gun->FtFire();
	}
	else if (Gun && Gun->FtGetbAuto())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_FireRate, Gun, &AGun::FtFire, 0.1f, Gun->FtGetbAuto(), 0.0f);
	}
}

void ASCharacter::FtStopPullTriggerAuto()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_FireRate);
}

int32 ASCharacter::FtGetBulletLeft() 
{
	return BulletLeft;
}

void ASCharacter::FtSetBulletLeft(int32 BulletInMag) 
{
	BulletLeft = BulletInMag;
}

int32 ASCharacter::FtGetGrenadeLeft() 
{
	return GrenadeLeft;
}

void ASCharacter::FtSetGrenadeLeft(int32 GrenadeInMag) 
{
	GrenadeLeft = GrenadeInMag;
}

void ASCharacter::FtSwitchWeapon()
{
	if (Gun)
	{
		if (bGun)
		{
			BulletLeft = Gun->FtGetAmmo();
			bAuto = Gun->FtGetbAuto();
		}
		else
		{
			GrenadeLeft = Gun->FtGetAmmo();
		}
		Gun->Destroy();
	}
	if (BulletLeft == -1)
	{
		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		BulletLeft = Gun->FtGetFullMag();
		bAuto = Gun->FtGetbAuto();
		Gun->Destroy();
		Gun = GetWorld()->SpawnActor<AProjectileWeapon>(ProjectileWeaponClass);
		GrenadeLeft = Gun->FtGetFullMag();
		Gun->Destroy();
	}
	if (bGun)
	{
		Gun = GetWorld()->SpawnActor<AProjectileWeapon>(ProjectileWeaponClass);
		Gun->FtSetAmmo(GrenadeLeft);
		bGun = false;
	}
	else
	{
		Gun = GetWorld()->SpawnActor<AGun>(GunClass);
		Gun->FtSetAmmo(BulletLeft);
		Gun->FtSetbAuto(bAuto);
		bGun = true;
	}
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
	UE_LOG(LogTemp, Warning, TEXT("Ammo = %i"), Gun->FtGetAmmo());
}

void ASCharacter::FtSwitchWeaponMode()
{
	if (Gun)
	{
		if (Gun->FtGetbAuto())
		{
			Gun->FtSetbAuto(false);
		}
		else
		{
			Gun->FtSetbAuto(true);
		}
	}
}

void ASCharacter::FtZoom()
{
	if (bZoom)
	{
		bZoom = false;
	}
	else
	{
		bZoom = true;
	}
}

void ASCharacter::OnHealthChanged(USHealthComponent *InHealthComp, float Health, float HealthDelta, const class UDamageType *DamageType, class AController *InstigatedBy, AActor *DamageCauser)
{
	if (HitShake && !bDead)
	{
		if (HitSound && Health > 50.0f)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, HitSound, GetActorLocation());
		}
		else if (HeavyHitSound && Health < 50.0f)
		{
			UGameplayStatics::SpawnSoundAtLocation(this, HeavyHitSound, GetActorLocation());
		}
		GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(HitShake);
	}
	if (Health <= 0.0f && !bDead)
	{
		bDead = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (Gun)
		{
			Gun->Destroy();
		}

		DetachFromControllerPendingDestroy();

		SetLifeSpan(10.0f);
	}
}

void ASCharacter::FtReloading()
{
	if (Gun)
	{
		Gun->FtReload();
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float TargetFOV = bZoom ? ZoomFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	CameraComp->SetFieldOfView(NewFOV);

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
	if (!bDead && HealthComp->FtGetHealth() < HealthComp->FtGetDefaultHealth())
	{
		HealthComp->FtSetHealth(FMath::ClampAngle(HealthComp->FtGetHealth() + 0.05, 0.0f, HealthComp->FtGetDefaultHealth()));
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

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::FtPullTrigger);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &ASCharacter::FtStopPullTriggerAuto);

	PlayerInputComponent->BindAction("SwitchWeapon", IE_Pressed, this, &ASCharacter::FtSwitchWeapon);
	PlayerInputComponent->BindAction("SwitchWeaponMode", IE_Pressed, this, &ASCharacter::FtSwitchWeaponMode);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &ASCharacter::FtReloading);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::FtZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::FtZoom);
}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCharacter, Gun);
}