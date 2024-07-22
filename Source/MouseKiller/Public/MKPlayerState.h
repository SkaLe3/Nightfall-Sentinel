// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MKPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MOUSEKILLER_API AMKPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AMKPlayerState();

public:

	UFUNCTION(BlueprintCallable, Category = "Items")
	void AddTales(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Items")
	int32 GetCollectedTales() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Items")
	int32 CollectedTales;
};
