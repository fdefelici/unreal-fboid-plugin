// Fill out your copyright notice in the Description page of Project Settings.


#include "BoidBehaviour.h"
#include "BoidSettings.h"
#include "BoidDirections.h"
#include "BoidSpawner.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UBoidBehaviour::UBoidBehaviour()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UBoidBehaviour::SetSettings(UBoidSettings* SomeSettings)
{
	Settings = SomeSettings;
}


void UBoidBehaviour::SetSpawner(ABoidSpawner* ASpawner)
{
	Spawner = ASpawner;
}


// Called when the game starts
void UBoidBehaviour::BeginPlay()
{
	Super::BeginPlay();
	if (!Settings) return;
	
	CurrentSpeed = Settings->MinSpeed;
	CurrentDirection = GetOwner()->GetActorForwardVector();
	CurrentVelocity = CurrentDirection * CurrentSpeed;

	GetOwner()->SetActorRotation(CurrentDirection.Rotation());
}


// Called every frame
void UBoidBehaviour::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FVector Acceleration = FVector::ZeroVector;

	FVector AlignForce = SteerTowards(CalculateAligmentDirection()) * Settings->AlignmentWeight;
	FVector CohesForce = SteerTowards(CalculateCohesionDirection()) * Settings->CohesionWeight;
	FVector SeparForce = SteerTowards(CalculateSeparationDirection()) * Settings->SeparationWeight;

	Acceleration += AlignForce;
	Acceleration += CohesForce;
	Acceleration += SeparForce;

	if (IsHeadingToObstacle())
	{
		FVector AvoidObstacleDirection = CalculateDirectionToAvoidObstacle();
		FVector AvoidObstacleForce = SteerTowards(AvoidObstacleDirection) * Settings->AvoidObstacleWeight;
		Acceleration += AvoidObstacleForce;
	}

	FVector NewVelocity = CurrentVelocity + Acceleration * DeltaTime;
	float NewSpeed = NewVelocity.Size();
	FVector NewDirection = NewVelocity / NewSpeed;
	GetOwner()->SetActorRotation(NewDirection.Rotation());

	CurrentSpeed = FMath::Clamp(NewSpeed, Settings->MinSpeed, Settings->MaxSpeed);
	CurrentDirection = GetOwner()->GetActorForwardVector();
	CurrentVelocity = CurrentDirection * CurrentSpeed;

	FVector NewPosition = GetOwner()->GetActorLocation() + CurrentVelocity * DeltaTime;
	GetOwner()->SetActorLocation(NewPosition);

}

bool UBoidBehaviour::IsHeadingToObstacle()
{
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start + GetOwner()->GetActorForwardVector() * Settings->VisionDistance;

	const TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;
	return UKismetSystemLibrary::SphereTraceSingle(
		this, Start, End, Settings->ColliderRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
}

FVector UBoidBehaviour::CalculateDirectionToAvoidObstacle()
{
	const TArray<FVector>& Rays = FBoidDirections::GetDirections();


	for (int Index = 0; Index < Rays.Num(); Index++)
	{
		FVector WorldRay = UKismetMathLibrary::TransformDirection(GetOwner()->GetTransform(), Rays[Index]);

		FVector Start = GetOwner()->GetActorLocation();
		FVector End = Start + WorldRay * Settings->VisionDistance;

		const TArray<AActor*> ActorsToIgnore;
		FHitResult HitResult;
		bool bHit = UKismetSystemLibrary::SphereTraceSingle(
			this, Start, End, Settings->ColliderRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),
			false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

		if (!bHit)
		{
			return WorldRay;
		}
	}

	return GetOwner()->GetActorForwardVector();
}

FVector UBoidBehaviour::SteerTowards(FVector Direction)
{
	Direction.Normalize();
	FVector NewDirection = Direction * Settings->MaxSpeed - CurrentVelocity;
	return NewDirection.GetClampedToMaxSize(Settings->MaxSteeringForce);
}

FVector UBoidBehaviour::CalculateAligmentDirection()
{
	FVector Position = GetOwner()->GetActorLocation();

	FVector Result = FVector::ZeroVector;
	int NeighboursCount = 0;

	const TArray<AActor*>& Boids = Spawner->GetBoids();

	for (const AActor* Each : Boids)
	{
		if (Each == GetOwner()) continue;
		FVector PosDiff = Position - Each->GetActorLocation();
		float SqrDist = PosDiff.SizeSquared();
		if (SqrDist < Settings->AlignmentRadius * Settings->AlignmentRadius)
		{
			NeighboursCount++;
			Result += Each->GetActorForwardVector();
		}
	}

	if (NeighboursCount > 0)
	{
		Result /= NeighboursCount;
		Result.Normalize();
	}
	return Result;
}

FVector UBoidBehaviour::CalculateCohesionDirection()
{
	FVector Position = GetOwner()->GetActorLocation();

	FVector Result = FVector::ZeroVector;
	int NeighboursCount = 0;

	const TArray<AActor*>& Boids = Spawner->GetBoids();

	for (const AActor* Each : Boids)
	{
		if (Each == GetOwner()) continue;
		FVector PosDiff = Position - Each->GetActorLocation();
		float SqrDist = PosDiff.SizeSquared();
		if (SqrDist < Settings->CohesionRadius * Settings->CohesionRadius)
		{
			NeighboursCount++;
			Result += Each->GetActorLocation();
		}
	}

	if (NeighboursCount > 0)
	{
		Result /= NeighboursCount;  
		Result -= Position;         
		Result.Normalize();
	}
	return Result;
}

FVector UBoidBehaviour::CalculateSeparationDirection()
{
	FVector Position = GetOwner()->GetActorLocation();

	FVector Result = FVector::ZeroVector;
	int NeighboursCount = 0;

	const TArray<AActor*>& Boids = Spawner->GetBoids();

	for (const AActor* Each : Boids)
	{
		if (Each == GetOwner()) continue;
		FVector PosDiff = Position - Each->GetActorLocation();
		float SqrDist = PosDiff.SizeSquared();
		if (SqrDist < Settings->SeparationRadius * Settings->SeparationRadius)
		{
			NeighboursCount++;
			Result += PosDiff / SqrDist;
		}
	}

	if (NeighboursCount > 0)
	{
		Result /= NeighboursCount; 
		Result.Normalize();
	}
	return Result;
}
