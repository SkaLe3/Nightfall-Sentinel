 // Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MKHealthComponent.h"
#include "MKGameMode.h"

// Sets default values for this component's properties
UMKHealthComponent::UMKHealthComponent()
{
	DefaultHealth = 100.0f;
	bIsDead = false;
}


// Called when the game starts
void UMKHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UMKHealthComponent::HandleTakeAnyDamage);
	}
	Health = DefaultHealth;
	
}


void UMKHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* Damagetype, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}
	
	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);
	bIsDead = Health <= 0.0f;
	OnHealthChanged.Broadcast(this, Health, Damage, Damagetype, InstigatedBy, DamageCauser);

	if (bIsDead)
	{
		AMKGameMode* GM = Cast<AMKGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}
}

// Called every frame
void UMKHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UMKHealthComponent::GetHealth() const
{
	return Health;
}

