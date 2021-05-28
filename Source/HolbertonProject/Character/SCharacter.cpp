// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "../Gun/Gun.h"

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

	WalkingSpeed = 400.f;
	RunningSpeed = 800.f;

	JumpCount = 1;
	JumpMax = 1;
	JumpHeight = 500.f;

	MaxHealth = 100.f;;
	Health = MaxHealth;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform , TEXT("WeaponSocket"));
	Gun->SetOwner(this);
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
	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}

void ASCharacter::FtWalk() 
{
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

const bool ASCharacter::IsDead() 
{
	return Health <= 0;
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::FtMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::FtMoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::FtJump);

	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::FtSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::FtWalk);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::FtFire);
}

float ASCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) 
{
	float DamageToApply;
	
	DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("Health left: %f"), Health);

	return DamageToApply;
}

