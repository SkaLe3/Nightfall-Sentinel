// Fill out your copyright notice in the Description page of Project Settings.


#include "MKRat.h"
#include "Components/MKHealthComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AMKRat::AMKRat()
{
	HealthComponent = CreateDefaultSubobject<UMKHealthComponent>(TEXT("HealthComponent"));

	bIsDead = false;
	bIsDead = true;
}

// Called when the game starts or when spawned
void AMKRat::BeginPlay()
{
	Super::BeginPlay();
	HealthComponent->OnHealthChanged.AddDynamic(this, &AMKRat::OnHealthChanged);

}

// Called every frame
void AMKRat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMKRat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMKRat::OnHealthChanged(UMKHealthComponent* OwningHealthComponent, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	FVector Location = GetActorLocation();
	if (Health <= 0.0f)
	{
		bIsDead = true;
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetLifeSpan(10.0f);
		UGameplayStatics::PlaySoundAtLocation(this, MeatSoundCue, Location);
		UGameplayStatics::PlaySoundAtLocation(this, DeathSoundCue, Location);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BiteEffect, Location);
		return;
	}
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BiteEffect, Location);
	UGameplayStatics::PlaySoundAtLocation(this, MeatSoundCue, Location);
	UGameplayStatics::PlaySoundAtLocation(this, ScreamSoundCue, Location);
	FName AnimName;
	AnimName = FName(TEXT("Default"));
	PlayAnimMontage(HitAnimation, 1.f, AnimName);

	UE_LOG(LogTemp, Warning, TEXT("Rat Health Changed: %s"), *FString::SanitizeFloat(Health));
}

