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
#include "Kismet/GameplayStatics.h"
#include "Components/MKHealthComponent.h"
#include "Components/CapsuleComponent.h"


#include "DrawDebugHelpers.h"


static int32 DebugCharacterDrawing = 0;
FAutoConsoleVariableRef CVARDebugCharacterDrawing(TEXT("MK.DebugCharacter"), DebugCharacterDrawing, TEXT("Draw Debug Lines for Character"), ECVF_Cheat);

// Sets default values
ACharacterBase::ACharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	HealthComponent = CreateDefaultSubobject<UMKHealthComponent>(TEXT("HealthComponent"));

	SpringArmComponent->SetupAttachment(GetMesh());
	CameraComponent->SetupAttachment(SpringArmComponent);

	SpringArmComponent->TargetArmLength = 300;
	CurrentMaxSpeed = MaxWalkingSpeed;

}

// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent->OnHealthChanged.AddDynamic(this, &ACharacterBase::OnHealthChanged);

}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UCharacterMovementComponent* MoveComp = GetComponentByClass<UCharacterMovementComponent>();

	if (!MoveComp)
	{
		return;
	}
	if (!MoveComp->IsFalling())
	{


		if (bWantsWalk)
		{


			float Curr = MoveComp->MaxWalkSpeed - MaxWalkingSpeed;

			float SlowdownTransitionElapsed = Curr / (MaxRunningSpeed - MaxWalkingSpeed) * SlowdownTransitionTime;
			SlowdownTransitionElapsed = SlowdownTransitionTime - SlowdownTransitionElapsed;
			SlowdownTransitionElapsed += DeltaTime;

			float TransitionAlpha = SlowdownFunction(SlowdownTransitionElapsed / SlowdownTransitionTime);
			float CurrentSpeed = FMath::Lerp(MaxRunningSpeed, MaxWalkingSpeed, TransitionAlpha);
			MoveComp->MaxWalkSpeed = CurrentSpeed;
			if (CurrentSpeed - MaxWalkingSpeed <= 40.f)
			{
				bWantsWalk = false;
				MoveComp->MaxWalkSpeed = MaxWalkingSpeed;
			}

			FRotator Rotation(0, GetActorRotation().Yaw, 0);
			FVector ActorRight = Rotation.RotateVector(FVector::RightVector);
			FVector ActorUp = Rotation.RotateVector(FVector::UpVector);

			float LeftDot = FVector::DotProduct(TargetDirection, -ActorRight);
			float RightDot = FVector::DotProduct(TargetDirection, ActorRight);



			FVector CurrentVelocity = GetCharacterMovement()->Velocity;
			float Speed = CurrentVelocity.Size();
			CurrentVelocity.Normalize();


			CurrentVelocity = FMath::VInterpTo(CurrentVelocity, TargetDirection, DeltaTime, SidewaysDragInterpolationSpeed);

			GetCharacterMovement()->Velocity = CurrentVelocity * Speed;





		}
		if (bSprinting)
		{
			float NewSpeed = GetComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed;
			NewSpeed += Acceleration * DeltaTime;
			NewSpeed = FMath::Clamp(NewSpeed, 0, MaxRunningSpeed);
			GetComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed = NewSpeed;

		}

	}
	if (DebugCharacterDrawing > 0)
	{
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + GetVelocity(), FColor::Red);
		FRotator DebugVec = FRotator::ZeroRotator;
		DebugVec.Yaw = Controller->GetControlRotation().Yaw;

		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + DebugVec.Vector() * 200, FColor::Green);
	}

// 	FVector SurfaceNormal = GetSurfaceNormal();
// 	FQuat AligningQuat = FQuat::FindBetweenNormals(GetMesh()->GetUpVector(), SurfaceNormal);
// 	FRotator AligningRotation = AligningQuat.Rotator();
// 	// Get the current rotation of the mesh
// 	FRotator CurrentRotation = GetMesh()->GetComponentRotation();
// 
// 	// Combine the current rotation with the aligning rotation
// 	FRotator TargetRotation = FRotator(CurrentRotation.Pitch - AligningRotation.Pitch,
// 									CurrentRotation.Yaw,
// 									CurrentRotation.Roll - AligningRotation.Roll);
// 	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 2);

	FVector SurfaceNormal = GetSurfaceNormal();
	FQuat CurrentQuat = GetMesh()->GetComponentQuat();
	FQuat AligningQuat = FQuat::FindBetweenNormals(GetMesh()->GetUpVector(), SurfaceNormal);


	// Combine the current rotation with the aligning quaternion
	FQuat TargetQuat = AligningQuat * CurrentQuat;

	// Interpolate between the current rotation and the target rotation
	FQuat NewQuat = FQuat::Slerp(CurrentQuat, TargetQuat, DeltaTime * 4.0f);

	// Convert the interpolated quaternion back to a rotator
	FRotator NewRotation = NewQuat.Rotator();

	// Get the default rotation (initial rotation)
	FRotator DefaultRotation = FRotator::ZeroRotator; // Adjust this to your actual default rotation

	// Calculate the difference between the new rotation and the default rotation
	FRotator RotationDifference = NewRotation - DefaultRotation;

	// Clamp the pitch and roll to a maximum of 30 degrees difference from the default rotation
	RotationDifference.Pitch = FMath::Clamp(RotationDifference.Pitch, -25.0f, 25.0f);
	RotationDifference.Roll = FMath::Clamp(RotationDifference.Roll, -25.0f, 25.0f);

	// Apply the clamped difference to the default rotation to get the final rotation
	NewRotation.Pitch = DefaultRotation.Pitch + RotationDifference.Pitch;
	NewRotation.Yaw = GetMesh()->GetComponentRotation().Yaw;
	NewRotation.Roll = DefaultRotation.Roll + RotationDifference.Roll;

	// Set the new rotation to the mesh
	GetMesh()->SetWorldRotation(NewRotation);
	if (DebugCharacterDrawing)
	{
	DrawDebugLine(GetWorld(), GetMesh()->GetComponentLocation(), GetMesh()->GetComponentLocation() + SurfaceNormal * 200, FColor::Green);
	DrawDebugLine(GetWorld(), GetMesh()->GetComponentLocation(), GetMesh()->GetComponentLocation() + GetMesh()->GetUpVector() * 200, FColor::Magenta);
	DrawDebugLine(GetWorld(), GetMesh()->GetComponentLocation(), GetMesh()->GetComponentLocation() + TargetQuat.Rotator().Vector() * 200, FColor::Yellow);
	DrawDebugLine(GetWorld(), GetMesh()->GetComponentLocation(), GetMesh()->GetComponentLocation() + GetMesh()->GetComponentRotation().Vector() * 200, FColor::Blue);

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
	PEI->BindAction(InputActions->InputMove, ETriggerEvent::Completed, this, &ACharacterBase::StopMove);
	PEI->BindAction(InputActions->InputLook, ETriggerEvent::Triggered, this, &ACharacterBase::Look);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Started, this, &ACharacterBase::StartSprint);
	PEI->BindAction(InputActions->InputSprint, ETriggerEvent::Completed, this, &ACharacterBase::StopSprint);
	PEI->BindAction(InputActions->InputJumpAttack, ETriggerEvent::Started, this, &ACharacterBase::JumpBite);
	PEI->BindAction(InputActions->InputDefaultAttack, ETriggerEvent::Started, this, &ACharacterBase::Bite);
	PEI->BindAction(InputActions->InputJump, ETriggerEvent::Started, this, &ACharacterBase::Jump);
	PEI->BindAction(InputActions->InputStop, ETriggerEvent::Started, this, &ACharacterBase::StopInitiated);
	PEI->BindAction(InputActions->InputStop, ETriggerEvent::Completed, this, &ACharacterBase::StopCompleted);
}

void ACharacterBase::Move(const FInputActionValue& Value)
{
	if (Controller != nullptr)
	{
		const FVector2D MoveValue = Value.Get<FVector2D>();

		MovementRotationTarget.Yaw = FMath::RInterpTo(MovementRotationTarget, Controller->GetControlRotation(), GetWorld()->GetDeltaSeconds(), 1.6).Yaw;

		float ForwardFactor = 1.01;
		if (MoveValue.X != 0.f && MoveValue.Y != 0.f)
			ForwardFactor = 15;

		TargetDirection = FVector::ZeroVector;


		FVector DirectionForward = MovementRotationTarget.RotateVector(FVector::ForwardVector);
		FVector DirectionRight = MovementRotationTarget.RotateVector(FVector::RightVector);
		FRotator Rotation(0, GetActorRotation().Yaw, 0);
		FVector ActorDirection = Rotation.RotateVector(FVector::ForwardVector);

		DirectionForward = DirectionForward * MoveValue.Y;
		DirectionRight = DirectionRight * MoveValue.X;

		TargetDirection += DirectionForward + DirectionRight + ActorDirection * ForwardFactor;
		TargetDirection.Normalize();
		AddMovementInput(TargetDirection, MoveValue.Size());


		if (DebugCharacterDrawing > 0)
		{
			DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + DirectionForward * FMath::Abs(MoveValue.Y) * 100, FColor::Yellow);
			DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + DirectionRight * FMath::Abs(MoveValue.X) * 100, FColor::Yellow);
			DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + TargetDirection * 100, FColor::Purple);
		}
	}
}


void ACharacterBase::StopMove(const FInputActionValue& Value)
{
	bSprinting = false;
	bWantsWalk = false;
	GetComponentByClass<UCharacterMovementComponent>()->MaxWalkSpeed = MaxWalkingSpeed;
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
	if (bCanAttack && !GetCharacterMovement()->IsFalling())
	{
		bCanAttack = false;
		GetWorldTimerManager().SetTimer(AttackTimer, this, &ACharacterBase::AllowAttack, JumpAttackCooldown, false);

		FName AnimName;
		AnimName = GetVelocity().Length() > 200 ? FName(TEXT("Default")) : FName(TEXT("SlowJump"));

		PlayAnimMontage(JumpBiteAnimation, 1.f, AnimName);

		FTimerHandle TimerHandle_SphereTrace;
		GetWorldTimerManager().SetTimer(TimerHandle_SphereTrace, [this]()
										{
											PerformBiteTrace(JumpAttackRadius, JumpAttackDamage);
										}, JumpAttackTraceDelay, false);

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

		PerformBiteTrace(DefaultAttackRadius, DefaultAttackDamage);
	}
	else if (float RemainingTime = GetWorldTimerManager().GetTimerRemaining(AttackTimer); RemainingTime < 0.3)
	{
		FTimerHandle PlannedAttack;
		GetWorldTimerManager().SetTimer(PlannedAttack, this, &ACharacterBase::PlannedBite, RemainingTime, false);
	}

}

void ACharacterBase::PerformBiteTrace(float SphereRadius, float Damage)
{
	FHitResult Hit;
	const FTransform JawTranform = GetMesh()->GetSocketTransform(TEXT("Jaw"), ERelativeTransformSpace::RTS_World);
	FVector JawLocation = JawTranform.GetTranslation();
	FRotator JawRotation = JawTranform.GetRotation().Rotator();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
	FHitResult OutHit;
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);

	EDrawDebugTrace::Type DrawType = EDrawDebugTrace::None;
	if (DebugCharacterDrawing > 0)
	{
		DrawType = EDrawDebugTrace::ForDuration;
	}

	bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(),
		JawLocation,
		JawLocation + JawRotation.Vector() * 50,
		SphereRadius,
		ObjectTypes,
		false,
		IgnoredActors,
		DrawType,
		OutHit,
		true,
		FLinearColor(1, 0, 0, 1),
		FLinearColor(1, 0.5f, 0, 1),
		5);

	if (bHit)
	{
		if (DebugCharacterDrawing > 0)
		{
			DrawDebugSphere(GetWorld(), OutHit.ImpactPoint, 5, 20, FColor::Green, false, 10);
		}
		UGameplayStatics::ApplyDamage(OutHit.GetActor(), Damage, GetController(), this, UDamageType::StaticClass());

		UGameplayStatics::PlaySound2D(this, HitMarkerSoundCue);

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
	GetComponentByClass<UCharacterMovementComponent>()->MaxAcceleration = 9999999;
	bWantsWalk = false;
	bSprinting = true;

}

void ACharacterBase::StopSprint()
{
	GetComponentByClass<UCharacterMovementComponent>()->MaxAcceleration = Acceleration;
	bWantsWalk = true;
	bSprinting = false;
}

void ACharacterBase::StopInitiated()
{
	SlowdownTransitionTime = UrgentSlowdownTransitionTime;
}

void ACharacterBase::StopCompleted()
{
	SlowdownTransitionTime = DefaultSlowdownTransitionTime;
}

float ACharacterBase::SlowdownFunction(float time)
{
	//return 0.8333333*(-(time * time * time) - 0.2 * FMath::Sqrt(time)) + 1;
	//return time * (time <= 0.5f) + (1 - 2 * (1 - time) * (1 - time) * (1 - time)) * (time > 0.5f);
	return time;
}

void ACharacterBase::OnHealthChanged(UMKHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f)
	{
		Destroy();
	}
	UE_LOG(LogTemp, Warning, TEXT("Fox Health Changed: %s"), *FString::SanitizeFloat(Health));
}

FVector ACharacterBase::GetSurfaceNormal()
{
	// Get the character's location and adjust for feet height
	FVector CharacterLocation = GetActorLocation();
	FVector FeetLocation = CharacterLocation - FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	// Perform line trace
	FVector Start = FeetLocation;
	FVector End = Start - FVector(0, 0, 50.0f); // Trace 50 units downward

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, QueryParams))
	{
		if (DebugCharacterDrawing)
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2.0f);

		// Return the normal of the hit surface
		return HitResult.ImpactNormal;
	}

	// Return UpVector if no surface was hit
	return FVector::UpVector;
}

void ACharacterBase::AllowAttack()
{
	bCanAttack = true;
}

