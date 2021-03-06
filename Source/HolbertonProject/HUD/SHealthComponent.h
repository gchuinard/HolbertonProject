// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, USHealthComponent *, HealthComp, float, Health, float, HealthDelta, const class UDamageType *, DamageType, class AController *, InstigatedBy, AActor *, DamageCauser);

UCLASS(ClassGroup = (COOP), meta = (BlueprintSpawnableComponent))
class HOLBERTONPROJECT_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USHealthComponent();

	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
	float FtGetHealth() const;
	void FtSetHealth(float HP);

	float FtGetDefaultHealth() const;
	void FtSetDefaultHealth(float HealthMax);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "HealthComponent")
	static bool IsFriendly(AActor *ActorA, AActor *ActorB);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


	UFUNCTION()
	void FtHandleTakeAnyDamage(AActor *DamageActor, float Damage, const class UDamageType *DamageType, class AController *InstigatedBy, AActor *DamageCauser);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;

	bool bIsDead;
};
