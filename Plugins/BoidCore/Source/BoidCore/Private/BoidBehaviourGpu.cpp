// Fill out your copyright notice in the Description page of Project Settings.
#pragma optimize("", off)

#include "BoidBehaviourGpu.h"
#include "BoidSettings.h"
#include "BoidDirections.h"
#include "BoidSpawner.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UBoidBehaviourGpu::UBoidBehaviourGpu()
{
	PrimaryComponentTick.bCanEverTick = true;
	_index = -1;
}


void UBoidBehaviourGpu::SetSettings(UBoidSettings* SomeSettings)
{
	Settings = SomeSettings;

	CurrentSpeed = Settings->MinSpeed;
	CurrentDirection = GetOwner()->GetActorForwardVector();
	CurrentVelocity = CurrentDirection * CurrentSpeed;
}


void UBoidBehaviourGpu::SetSpawner(ABoidSpawner* ASpawner)
{
	Spawner = ASpawner;
}


// Called when the game starts
void UBoidBehaviourGpu::BeginPlay()
{
	Super::BeginPlay();

	//check(_index != -1);
	//TODO CHECK
	/*
	if (!Settings) return;
	
	CurrentSpeed = Settings->MinSpeed;
	CurrentDirection = GetOwner()->GetActorForwardVector();
	CurrentVelocity = CurrentDirection * CurrentSpeed;

	GetOwner()->SetActorRotation(CurrentDirection.Rotation());
	*/

	CurrentSpeed = 0.f;
	CurrentDirection = GetOwner()->GetActorForwardVector();
	CurrentVelocity = FVector::ZeroVector;

	m_AlignDirection = FVector::ZeroVector;
	m_CohsionDirection = FVector::ZeroVector;
	m_SeparationDirection = FVector::ZeroVector;
}

void UBoidBehaviourGpu::_UpdateData(int AlignCount, FVector FlockDirection, 
	int CohesionCount, FVector FlockPosition, 
	int SeparationCount, FVector FlockSeparationDirection) {
	if (AlignCount > 0) {
		//m_AlignDirection = AlignDirection / FlockmatesCount;
		m_AlignDirection = FlockDirection; //effettivamente la media non serve ("accorcia" solo il vettore), poiche' poi faccio il Normalize
	}

	if (CohesionCount > 0) {
		FlockPosition /= CohesionCount;
		m_CohsionDirection = FlockPosition - GetOwner()->GetActorLocation();
	}

	if (SeparationCount > 0) {
		m_SeparationDirection = FlockSeparationDirection;
	}
}


// Called every frame
void UBoidBehaviourGpu::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	//check(_index != -1);
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	

	FVector AlignForce = SteerTowards(m_AlignDirection) * Settings->AlignmentWeight;
	FVector CohesForce = SteerTowards(m_CohsionDirection) * Settings->CohesionWeight;
	FVector SeparForce = SteerTowards(m_SeparationDirection) * Settings->SeparationWeight;
	FVector ObstaForce = SteerTowards(CalculateDirectionToAvoidObstacle()) * Settings->AvoidObstacleWeight;

	FVector Acceleration = FVector::ZeroVector;
	Acceleration += AlignForce;
	Acceleration += CohesForce;
	Acceleration += SeparForce;
	Acceleration += ObstaForce;
	
	FVector NewVelocity = CurrentVelocity + Acceleration * DeltaTime;
	float NewSpeed = NewVelocity.Size();
	FVector NewDirection = NewVelocity / NewSpeed;
	GetOwner()->SetActorRotation(NewDirection.Rotation());

	CurrentSpeed = FMath::Clamp(NewSpeed, Settings->MinSpeed, Settings->MaxSpeed);
	CurrentDirection = GetOwner()->GetActorForwardVector();
	CurrentVelocity = CurrentDirection * CurrentSpeed;

	FVector NewPosition = GetOwner()->GetActorLocation() + CurrentVelocity * DeltaTime;
	GetOwner()->SetActorLocation(NewPosition);

	Spawner->SetMe(this);
}

FVector UBoidBehaviourGpu::CalculateAligmentDirection()
{
	FVector Position = GetOwner()->GetActorLocation();

	FVector Result = FVector::ZeroVector;
	int NeighboursCount = 0;

	const TArray<AActor*>& Boids = Spawner->GetBoids();

	for (const AActor* Each : Boids)
	{
		if (Each == GetOwner()) continue;
		FVector PosDiff = Position - Each->GetActorLocation(); //Vettore verso di me
		float SqrDist = PosDiff.SizeSquared();
		if (SqrDist < Settings->AlignmentRadius * Settings->AlignmentRadius)
		{
			NeighboursCount++;
			Result += Each->GetActorForwardVector(); //Sommo le direzione dei vicini
		}
	}

	if (NeighboursCount > 0)
	{
		Result /= NeighboursCount;  //media delle direzioni dei vicini
		Result.Normalize();
	}
	return Result;
}

bool UBoidBehaviourGpu::IsHeadingToObstacle()
{
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start + GetOwner()->GetActorForwardVector() * Settings->VisionDistance;

	const TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;
	return UKismetSystemLibrary::SphereTraceSingle(
		this, Start, End, Settings->ColliderRadius, UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);
}

FVector UBoidBehaviourGpu::CalculateDirectionToAvoidObstacle()
{
	if (!IsHeadingToObstacle()) 
	{
		return FVector::ZeroVector;
	}

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
	return FVector::ZeroVector;
}

FVector UBoidBehaviourGpu::SteerTowards(FVector Direction)
{
	if (Direction == FVector::ZeroVector) return FVector::ZeroVector;
	Direction.Normalize();
	FVector NewDirection = Direction * Settings->MaxSpeed - CurrentVelocity;
	return NewDirection.GetClampedToMaxSize(Settings->MaxSteeringForce);
}


FVector UBoidBehaviourGpu::CalculateCohesionDirection()
{
	FVector Position = GetOwner()->GetActorLocation();

	FVector Result = FVector::ZeroVector;
	int NeighboursCount = 0;

	const TArray<AActor*>& Boids = Spawner->GetBoids();

	for (const AActor* Each : Boids)
	{
		if (Each == GetOwner()) continue;
		FVector PosDiff = Position - Each->GetActorLocation(); //Vettore verso di me
		float SqrDist = PosDiff.SizeSquared();
		if (SqrDist < Settings->CohesionRadius * Settings->CohesionRadius)
		{
			NeighboursCount++;
			Result += Each->GetActorLocation(); //sommo le posizioni dei vicini
		}
	}

	if (NeighboursCount > 0)
	{
		Result /= NeighboursCount;  //media delle direzioni dei vicini
		Result -= Position;         //direzione da me verso il punto medio
		Result.Normalize();
	}
	return Result;
}

FVector UBoidBehaviourGpu::CalculateSeparationDirection()
{
	FVector Position = GetOwner()->GetActorLocation();

	FVector Result = FVector::ZeroVector;
	int NeighboursCount = 0;

	const TArray<AActor*>& Boids = Spawner->GetBoids();

	for (const AActor* Each : Boids)
	{
		if (Each == GetOwner()) continue;
		FVector PosDiff = Position - Each->GetActorLocation(); //Vettore verso di me
		float SqrDist = PosDiff.SizeSquared();
		if (SqrDist < Settings->SeparationRadius * Settings->SeparationRadius)
		{
			NeighboursCount++;
			Result += PosDiff / SqrDist;
		}
	}

	if (NeighboursCount > 0)
	{
		//Result /= NeighboursCount;  //media delle direzioni dei vicini
		Result.Normalize();
	}
	return Result;
}
#pragma optimize("", on)