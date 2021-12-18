// (FDF) Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


class FBoidDirectionsInternal
{
public:
	FBoidDirectionsInternal();
	TArray<FVector> Directions;
};

/**
 * 
 */
class BOIDCORE_API FBoidDirections
{
public:
	static const TArray<FVector>& GetDirections()
	{
		return Internal.Directions;
	}

private:
	FBoidDirections() { }
	static FBoidDirectionsInternal Internal;
};
