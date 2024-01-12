// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "MKInputConfigData.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	SpringArmComponent->SetupAttachment(GetMesh());
	CameraComponent->SetupAttachment(SpringArmComponent);

	SpringArmComponent->TargetArmLength = 300;

}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWantsWalk)
	{
		float CurrentSpeed =  GetComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed;
		CurrentSpeed = FMath::FInterpTo(CurrentSpeed, MaxWalkingSpeed, DeltaTime, RunToWalkTransitionSpeed);
		GetComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed = CurrentSpeed;
		if (CurrentSpeed - MaxWalkingSpeed <= 40.f)
		{
			bWantsWalk = false;
			GetComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed = MaxWalkingSpeed;
		}
	}

}

// Called to bind functionality to input
void ACharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PC = Cast<APlayerController>(GetController());

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &ACharacterBase::Move);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &ACharacterBase::Look);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Started, this, &ACharacterBase::StartSprint);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Completed, this, &ACharacterBase::StopSprint);
	PEI->BindAction(InputActions->InputJumpAttack, ETriggerEvent::Started, this, &ACharacterBase::JumpBite);
	PEI->BindAction(InputActions->InputDefaultAttack, ETriggerEvent::Started, this, &ACharacterBase::Bite);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Started, this, &ACharacterBase::Jump);
}

void ACharacterBase::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();

		MovementRotationTarget.Yaw = FMath::RInterpTo(MovementRotationTarget, Controller->GetControlRotation(), GetWorld()->GetDeltaSeconds(), 1.6).Yaw;

		float multiplier = 1.01;
		if (MoveValue.X != 0.f && MoveValue.Y != 0.f)
			multiplier = 1.5;

		// Forward/Backward direction
		if (MoveValue.Y != 0.f)
		{
			// Get forward vector
			FVector Direction = MovementRotationTarget.RotateVector(FVector::ForwardVector);
			FRotator Rotation(0, GetActorRotation().Yaw, 0);
			FVector ActorDirection = Rotation.RotateVector(FVector::ForwardVector);

			Direction = Direction * MoveValue.Y + ActorDirection * multiplier;
			Direction.Normalize();
			AddMovementInput(Direction, FMath::Abs(MoveValue.Y));
		}

		// Right/Left direction
		if (MoveValue.X != 0.f)
		{
			// Get right vector
			FVector Direction = MovementRotationTarget.RotateVector(FVector::RightVector);
			FRotator Rotation(0, GetActorRotation().Yaw, 0);
			FVector ActorDirection = Rotation.RotateVector(FVector::ForwardVector);

			Direction = Direction * MoveValue.X + ActorDirection * multiplier;
			Direction.Normalize();
			AddMovementInput(Direction, FMath::Abs(MoveValue.X));	
		}	
	}
}

void ACharacterBase::Look(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D LookValue = Value.Get<FVector2D>();

		if (LookValue.X != 0.f)
		{
			AddControllerYawInput(LookValue.X);
		}

		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(LookValue.Y);
		}
	}
}

void ACharacterBase::Jump(const FInputActionValue& Value)
{
	if (GetVelocity().Length() >= 10)
	{
		ACharacter::Jump();
	}
}

void ACharacterBase::JumpBite(const FInputActionValue& Value)
{
	if (bCanAttack)
	{
		bCanAttack = false;
		GetWorldTimerManager().SetTimer(AttackTimer, this, &ACharacterBase::AllowAttack, JumpAttackCooldown, false);

		FName AnimName;
		AnimName = GetVelocity().Length() > 200 ? FName(TEXT("Default")) : FName(TEXT("SlowJump"));

		PlayAnimMontage(JumpBiteAnimation, 1.f, AnimName);
	}
	else if (float RemainingTime = GetWorldTimerManager().GetTimerRemaining(AttackTimer); RemainingTime < 0.3)
	{
		FTimerHandle PlannedAttack;
		GetWorldTimerManager().SetTimer(PlannedAttack, this, &ACharacterBase::PlannedJumpBite, RemainingTime, false);
	}
}

void ACharacterBase::Bite(const FInputActionValue& Value)
{
	if (bCanAttack)
	{
		bCanAttack = false;
		GetWorldTimerManager().SetTimer(AttackTimer, this, &ACharacterBase::AllowAttack, DeafaultAttackCooldown, false);

		FName AnimName;
		AnimName = GetVelocity().Length() > 10 ? FName(TEXT("Run")) : FName(TEXT("Default"));

		PlayAnimMontage(BiteAnimation, 1.0f, AnimName);
	}
	else if (float RemainingTime = GetWorldTimerManager().GetTimerRemaining(AttackTimer); RemainingTime < 0.3)
	{
		FTimerHandle PlannedAttack;
		GetWorldTimerManager().SetTimer(PlannedAttack, this, &ACharacterBase::PlannedBite, RemainingTime, false);
	}

}

void ACharacterBase::PlannedJumpBite()
{
	bCanAttack = true;
	JumpBite(1);
}

void ACharacterBase::PlannedBite()
{
	bCanAttack = true;
	Bite(1);
}

void ACharacterBase::StartSprint()
{
	GetComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed = MaxRunningSpeed;
	bWantsWalk = false;
}

void ACharacterBase::StopSprint()
{
	bWantsWalk = true;
}

void ACharacterBase::AllowAttack()
{
	bCanAttack = true;
}

