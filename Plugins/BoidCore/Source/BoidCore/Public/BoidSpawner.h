// (FDF) Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoidComputeShader.h"
#include "BoidSpawner.generated.h"

UCLASS()
class BOIDCORE_API ABoidSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABoidSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	const TArray<AActor*>& GetBoids() const;

	UPROPERTY(EditAnywhere, Category = "Spawn Config")
	TSubclassOf<AActor> BoidPrefab;

	UPROPERTY(EditAnywhere, Category = "Spawn Config")
	class UBoidSettings* Settings;

private:
	TArray<AActor*> Boids;

	FBoidComputeShader ComputeShader;
};
