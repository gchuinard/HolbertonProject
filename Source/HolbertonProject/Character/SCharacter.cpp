// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "../HolbertonProject.h"
#include "../Gun/Gun.h"
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

	WalkingSpeed = 400.f;
	RunningSpeed = 800.f;

	JumpCount = 1;
	JumpMax = 1;
	JumpHeight = 500.f;


}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();

	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	//GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform , TEXT("WeaponSocket"));
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