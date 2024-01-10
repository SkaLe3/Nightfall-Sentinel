// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"
#include "MKInputConfigData.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

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
}

void ACharacterBase::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);

		// Forward/Backward direction
		if (MoveValue.Y != 0.f)
		{
			// Get forward vector
			FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
			FRotator Rotation(0, GetActorRotation().Yaw, 0);
			FVector ActorDirection = Rotation.RotateVector(FVector::ForwardVector);

			Direction = MoveValue.Y * Direction + ActorDirection * 1.01;
			Direction.Normalize();
			AddMovementInput(Direction, FMath::Abs(MoveValue.Y));
		}

		// Right/Left direction
		if (MoveValue.X != 0.f)
		{
			// Get right vector
			FVector Direction = MovementRotation.RotateVector(FVector::RightVector);
			FRotator Rotation(0, GetActorRotation().Yaw, 0);
			FVector ActorDirection = Rotation.RotateVector(FVector::ForwardVector);

			Direction = MoveValue.X * Direction + ActorDirection * 1.01;
			Direction.Normalize();
			AddMovementInput(Direction, FMath::Abs(MoveValue.X));
		}

// 		if (MoveValue.X != 0.f || MoveValue.Y != 0.f)
// 		{
// 
// 			FVector DirectionForward = MovementRotation.RotateVector(FVector::ForwardVector);
// 			FVector DirectionRight = MovementRotation.RotateVector(FVector::RightVector);
// 			FVector Direction = DirectionForward * MoveValue.Y + DirectionRight * MoveValue.X;
// 			Direction.Normalize();
// 			FRotator ActorRotation(0, GetActorRotation().Yaw, 0);
// 			FVector ActorDirection = ActorRotation.RotateVector(FVector::ForwardVector);
// 
// 			Direction = MoveValue.X * Direction + ActorDirection * 1.01;
// 			Direction.Normalize();
// 			AddMovementInput(Direction, FMath::Abs(MoveValue.X) + FMath::Abs(MoveValue.Y));
// 		}

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

void ACharacterBase::StartSprint()
{
	GetComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed = MaxRunningSpeed;
}

void ACharacterBase::StopSprint()
{
	GetComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed = MaxWalkingSpeed;
}

