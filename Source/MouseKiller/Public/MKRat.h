// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MKRat.generated.h"

UCLASS()
class MOUSEKILLER_API AMKRat : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMKRat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UFUNCTION()
	void OnHealthChanged(class UMKHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UMKHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	class UAnimMontage* HitAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	class UNiagaraSystem* BiteEffect;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundBase* MeatSoundCue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundBase* ScreamSoundCue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundBase* DeathSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "States")
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "States")
	bool bWantFlex;


};
