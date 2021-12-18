// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BoidSettings.generated.h"

/**
 * 
 */
UCLASS()
class BOIDCORE_API UBoidSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Spawner")
	int BoidCount;
	UPROPERTY(EditAnywhere, Category = "Spawner")
	float SpawnRadius;

	UPROPERTY(EditAnywhere, Category = "Flock")
	float MinSpeed;
	UPROPERTY(EditAnywhere, Category = "Flock")
	float MaxSpeed;
	UPROPERTY(EditAnywhere, Category = "Flock")
	float MaxSteeringForce;

	UPROPERTY(EditAnywhere, Category = "Flock/Sensor")
	float AlignmentRadius;
	UPROPERTY(EditAnywhere, Category = "Flock/Sensor")
	float CohesionRadius;
	UPROPERTY(EditAnywhere, Category = "Flock/Sensor")
	float SeparationRadius;

	UPROPERTY(EditAnywhere, Category = "Flock/SensorWeight")
	float AlignmentWeight;
	UPROPERTY(EditAnywhere, Category = "Flock/SensorWeight")
	float CohesionWeight;
	UPROPERTY(EditAnywhere, Category = "Flock/SensorWeight")
	float SeparationWeight;

	UPROPERTY(EditAnywhere, Category = "Obstacles")
	float ColliderRadius;
	UPROPERTY(EditAnywhere, Category = "Obstacles")
	float VisionDistance;
	UPROPERTY(EditAnywhere, Category = "Obstacles")
	float AvoidObstacleWeight;
};
