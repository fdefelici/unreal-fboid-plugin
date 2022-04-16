// (FDF) Fill out your copyright notice in the Description page of Project Settings.


#include "BoidSpawner.h"
#include "BoidBehaviour.h"
#include "BoidBehaviourGpu.h"
#include "BoidSettings.h"
#if WITH_EDITOR
#include "BoidDrawComponent.h"
#endif
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine/World.h"


// Sets default values
ABoidSpawner::ABoidSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ABoidSpawner::BeginPlay()
{
	Super::BeginPlay();
	
	Boids.Reserve(Settings->BoidCount);
	for (int BoidIndex = 0; BoidIndex < Settings->BoidCount; ++BoidIndex)
	{
		FVector Position = GetActorLocation() + UKismetMathLibrary::RandomUnitVector() * Settings->SpawnRadius;
		FVector Direction = UKismetMathLibrary::RandomUnitVector();
		
		FTransform Transform(Direction.Rotation(), Position);

		AActor* BoidInstance = GetWorld()->SpawnActor<AActor>(BoidPrefab, Transform);
		UBoidBehaviourGpu* BoidBehav = Cast<UBoidBehaviourGpu>(BoidInstance->GetComponentByClass(UBoidBehaviourGpu::StaticClass()));
		BoidBehav->SetSettings(Settings);
		BoidBehav->SetSpawner(this);
		BoidBehav->_index = BoidIndex;

#if WITH_EDITOR
		UBoidDrawComponent* BoidDraw = Cast<UBoidDrawComponent>(BoidInstance->GetComponentByClass(UBoidDrawComponent::StaticClass()));
		BoidDraw->SetSettings(Settings);
#endif

		Boids.Add(BoidInstance);
	}

	BoidDataInput_t ZeroIn;
	ZeroIn.Direction = FVector::ZeroVector;
	ZeroIn.Position = FVector::ZeroVector;


	BoidData_t Zeroed;
	Zeroed.AlignCount = 0;
	Zeroed.FlockDirection = FVector::ZeroVector;
	Zeroed.CohesionCount = 0;
	Zeroed.FlockPosition = FVector::ZeroVector;
	Zeroed.SeparationCount = 0;
	Zeroed.FlockSeparationDirection = FVector::ZeroVector;
	Zeroed.SeparationCount = 0;

	BoidDataOut.Init(Zeroed, Settings->BoidCount);
	BoidDataIn.Init(ZeroIn, Settings->BoidCount);
}

DECLARE_STATS_GROUP(TEXT("BoidStatGroup"), STATGROUP_BoidStatGroup, STATCAT_Advanced);
DECLARE_CYCLE_STAT(TEXT("Boid_Execute"), STAT_MyExecuteStats, STATGROUP_BoidStatGroup);

void ABoidSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!ComputeShader.IsCompleted()) {
		return;
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_MyExecuteStats);
		ComputeShader.Execute(Settings->AlignmentRadius, 
							  Settings->CohesionRadius, 
							  Settings->SeparationRadius, 
							  BoidDataIn, BoidDataOut);
	}	
}

//Each Boid ask for data to be udpdate. This way a for loop is no necessary in the spawner Tick
//in order to update all the boids after the Compute Shader execution.
void ABoidSpawner::UpdataDataFor(UBoidBehaviourGpu* Boid)
{
	int Index = Boid->_index;

	Boid->UpdateData(
		BoidDataOut[Index].AlignCount,
		BoidDataOut[Index].FlockDirection,
		BoidDataOut[Index].CohesionCount,
		BoidDataOut[Index].FlockPosition,
		BoidDataOut[Index].SeparationCount,
		BoidDataOut[Index].FlockSeparationDirection
	);

	BoidDataIn[Index].Position = Boid->GetOwner()->GetActorLocation();
	BoidDataIn[Index].Direction = Boid->GetOwner()->GetActorForwardVector();

}

const TArray<AActor*>& ABoidSpawner::GetBoids() const
{
	return Boids;
}

