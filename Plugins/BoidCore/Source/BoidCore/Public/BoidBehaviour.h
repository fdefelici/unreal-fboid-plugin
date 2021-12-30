// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoidBehaviour.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOIDCORE_API UBoidBehaviour : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBoidBehaviour();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void SetSettings(class UBoidSettings* Settings);
	void SetSpawner(class ABoidSpawner* Spawner);

private:
	bool IsHeadingToObstacle();
	FVector CalculateDirectionToAvoidObstacle();
	FVector SteerTowards(FVector Direction);

	FVector CalculateAligmentDirection();
	FVector CalculateCohesionDirection();
	FVector CalculateSeparationDirection();


	class UBoidSettings* Settings;
	class ABoidSpawner* Spawner;

public:
	FVector CurrentDirection;
	float CurrentSpeed;
	FVector CurrentVelocity;
};
