// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterBase.generated.h"

UCLASS()
class MOUSEKILLER_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:

	void Move(const FInputActionValue& Value);
	void StopMove(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void JumpBite(const FInputActionValue& Value);
	void Bite(const FInputActionValue& Value);
	void PerformBiteTrace(float SphereRadius, float Damage);

	void PlannedJumpBite();
	void PlannedBite();

	void StartSprint();
	void StopSprint();

	void StopInitiated();
	void StopCompleted();

	float SlowdownFunction(float time);

	UFUNCTION()
	void OnHealthChanged(UMKHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

private:

	FVector GetSurfaceNormal();

protected:
	/* INPUT */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UInputMappingContext* InputMapping;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	class UMKInputConfigData* InputActions;


	/* COMPONENTS */

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	class UMKHealthComponent* HealthComponent;


	/* Assets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	class UAnimMontage* JumpBiteAnimation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	class UAnimMontage* BiteAnimation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	class USoundBase* HitMarkerSoundCue;

	/* Parameters*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxWalkingSpeed = 80;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxRunningSpeed = 650;

	float CurrentMaxSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float DefaultSlowdownTransitionTime = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float UrgentSlowdownTransitionTime = 0.3f;

	float SlowdownTransitionTime = 4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
	float SidewaysDragInterpolationSpeed = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float Acceleration = 400;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DeafaultAttackCooldown = 0.6f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DefaultAttackRadius = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DefaultAttackDamage = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float JumpAttackCooldown = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float JumpAttackRadius = 35.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float JumpAttackDamage = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float JumpAttackTraceDelay = 0.4f;

	FTimerHandle AttackTimer;



	FRotator MovementRotationTarget = { 0, 0, 0 };
	FVector TargetDirection = { 0, 0, 0 };

private:
	bool bSprinting = false;
	bool bWantsWalk = false;
	bool bCanAttack = true;

	void AllowAttack();
};
