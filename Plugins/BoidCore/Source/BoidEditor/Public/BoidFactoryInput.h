// (FDF) Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BoidFactoryInput.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BOIDEDITOR_API UBoidFactoryInput : public UObject
{
	GENERATED_BODY()
public:
	/* Actor Blueprint to be 'boidized' */
	//UPROPERTY(EditAnywhere, meta = (AllowedClasses="AActor", ExactClass = "true"))
	UPROPERTY(EditAnywhere)
	class UBlueprint* ActorBlueprint;

	/* Select this option if you want to update the choosen blueprint or make a clone otherwise */
	UPROPERTY(EditAnywhere)
	bool bInPlace;
	/*
	UPROPERTY(EditAnywhere)
	TSubclassOf<AActor> Actor2;
	*/
};
