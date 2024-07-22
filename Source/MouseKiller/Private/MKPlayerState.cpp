// Fill out your copyright notice in the Description page of Project Settings.


#include "MKPlayerState.h"

AMKPlayerState::AMKPlayerState()
{
	CollectedTales = 0;
}

void AMKPlayerState::AddTales(int32 Amount)
{
	CollectedTales += Amount;
}

int32 AMKPlayerState::GetCollectedTales() const
{
	return CollectedTales;
}
