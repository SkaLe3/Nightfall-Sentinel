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
	void Look(const FInputActionValue& Value);
	void Jump(const FInputActionValue& Value);
	void JumpBite(const FInputActionValue& Value);
	void Bite(const FInputActionValue& Value);

	void PlannedJumpBite();
	void PlannedBite();

	void StartSprint();
	void StopSprint();

private:


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


	/* Assets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	class UAnimMontage* JumpBiteAnimation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations")
	class UAnimMontage* BiteAnimation;


	/* Parameters*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxWalkingSpeed = 80;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float MaxRunningSpeed = 650;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float RunToWalkTransitionSpeed = 2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float DeafaultAttackCooldown = 0.6;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	float JumpAttackCooldown = 1;

	FTimerHandle AttackTimer;

	FRotator MovementRotationTarget = { 0, 0, 0 };

private:
	bool bWantsWalk = false;
	bool bCanAttack = true;

	void AllowAttack();
};
