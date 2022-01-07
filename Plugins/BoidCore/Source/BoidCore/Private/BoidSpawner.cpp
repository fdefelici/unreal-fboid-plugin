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

	/*
	FOnActorSpawned::FDelegate ActorSpawnedDelegate = FOnActorSpawned::FDelegate::CreateLambda([this](AActor* Instance) {
		//UE_LOG(LogTemp, Warning, TEXT("Hey %s: %d"), *Instance->GetName(), Instance->GetComponents().Num());

		//UBoidBehaviour* BoidBehav = Cast<UBoidBehaviour>(Instance->GetComponentByClass(UBoidBehaviour::StaticClass()));
		UBoidBehaviourGpu* BoidBehav = Cast<UBoidBehaviourGpu>(Instance->GetComponentByClass(UBoidBehaviourGpu::StaticClass()));
		if (!BoidBehav) return;
		BoidBehav->SetSettings(Settings);
		BoidBehav->SetSpawner(this);
		BoidBehav->_index = BoidIndex;

		#if WITH_EDITOR
		UBoidDrawComponent* BoidDraw = Cast<UBoidDrawComponent>(Instance->GetComponentByClass(UBoidDrawComponent::StaticClass()));
		if (!BoidDraw) return;
		BoidDraw->SetSettings(Settings);
		#endif
	});

	//INUTLE: BeginPlay viene comunque chiamato non appena eseguo SpawnActor e poi successivamente viene chiamato questo Delegate
	GetWorld()->AddOnActorSpawnedHandler(ActorSpawnedDelegate);
	*/

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











		/* Anche con SpawnActorDeferred i componenti che di base avrebbe l'attore, sarebbero inizializzati
		* dopo il FinishSpawningActor. Per cui i compenenti che aggiungo (il behaviour) diventrebbero Root.
		AActor* BoidInstance = GetWorld()->SpawnActorDeferred<AActor>(BoidPrefab, Transform);

		UBoidBehaviour* BoidBehav = Cast<UBoidBehaviour>(BoidInstance->GetComponentByClass(UBoidBehaviour::StaticClass()));
		BoidBehav->SetSettings(Settings);
		BoidBehav->SetSpawner(this);

		UBoidDrawComponent* BoidDraw = Cast<UBoidDrawComponent>(BoidInstance->GetComponentByClass(UBoidDrawComponent::StaticClass()));
		BoidDraw->SetSettings(Settings);

		UGameplayStatics::FinishSpawningActor(BoidInstance, Transform);
		*/
		

		Boids.Add(BoidInstance);

/* Esempio di come Aggiungere a Runtime Componenti, di cui uno solo quando sono nell'Editor

		UBoidBehaviour* Comp = NewObject<UBoidBehaviour>(BoidInstance, NAME_None, RF_Public);
		Comp->SetSettings(Settings);
		Comp->SetSpawner(this);
		Comp->RegisterComponent();
		BoidInstance->AddInstanceComponent(Comp);

#if WITH_EDITOR
		UBoidDrawComponent* DrawComp = NewObject<UBoidDrawComponent>(BoidInstance, NAME_None, RF_Public);
		DrawComp->SetSettings(Settings);
		DrawComp->RegisterComponent();
		BoidInstance->AddInstanceComponent(DrawComp);
		DrawComp->AttachToComponent(BoidInstance->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
#endif
*/
	}

	//GetWorld()->RemoveOnActorSpawnedHandler(ActorSpawnedDelegate.GetHandle());

	BoidData_t Zeroed;
	Zeroed.AlignCount = 0;
	Zeroed.FlockDirection = FVector::ZeroVector;
	Zeroed.CohesionCount = 0;
	Zeroed.FlockPosition = FVector::ZeroVector;
	Zeroed.SeparationCount = 0;
	Zeroed.FlockSeparationDirection = FVector::ZeroVector;
	Zeroed.SeparationCount = 0;

	BoidDataIO.Init(Zeroed, Settings->BoidCount);
}

DECLARE_STATS_GROUP(TEXT("BoidStatGroup"), STATGROUP_BoidStatGroup, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("Boid_Input"), STAT_MyInputStats, STATGROUP_BoidStatGroup);
DECLARE_CYCLE_STAT(TEXT("Boid_Execute"), STAT_MyExecuteStats, STATGROUP_BoidStatGroup);
DECLARE_CYCLE_STAT(TEXT("Boid_Output"), STAT_MyOutputStats, STATGROUP_BoidStatGroup);

// Called every frame
void ABoidSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!ComputeShader.IsCompleted()) {
		return;
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_MyOutputStats);
#ifdef ENABLED
		for (int Index = 0; Index < Boids.Num(); ++Index)
		{
			AActor* Actor = Boids[Index];
			/*
			UBoidBehaviourGpu* BoidBehav = Cast<UBoidBehaviourGpu>(Actor->GetComponentByClass(UBoidBehaviourGpu::StaticClass()));
			
			BoidBehav->_UpdateData(
				BoidDataIO[Index].AlignCount,
				BoidDataIO[Index].FlockDirection,
				BoidDataIO[Index].CohesionCount,
				BoidDataIO[Index].FlockPosition,
				BoidDataIO[Index].SeparationCount,
				BoidDataIO[Index].FlockSeparationDirection
			);
			*/

			//RESET BoidDataIO
			
			//BoidDataIO
			BoidDataIO[Index].Position = Actor->GetActorLocation();
			BoidDataIO[Index].Direction = Actor->GetActorForwardVector();

			//Output
			/*
			BoidDataIO[Index].AlignCount = 0;
			BoidDataIO[Index].FlockDirection = FVector::ZeroVector;
			BoidDataIO[Index].CohesionCount = 0;
			BoidDataIO[Index].FlockPosition = FVector::ZeroVector;
			BoidDataIO[Index].SeparationCount = 0;
			BoidDataIO[Index].FlockSeparationDirection = FVector::ZeroVector;
			BoidDataIO[Index].SeparationCount = 0;
			*/
		}
#endif
	}
	
	{
		SCOPE_CYCLE_COUNTER(STAT_MyExecuteStats);
		ComputeShader.Execute(BoidDataIO);
	}


	
}

void ABoidSpawner::SetMe(UBoidBehaviourGpu* Boid)
{
	int Index = Boid->_index;

	Boid->_UpdateData(
		BoidDataIO[Index].AlignCount,
		BoidDataIO[Index].FlockDirection,
		BoidDataIO[Index].CohesionCount,
		BoidDataIO[Index].FlockPosition,
		BoidDataIO[Index].SeparationCount,
		BoidDataIO[Index].FlockSeparationDirection
	);

	BoidDataIO[Index].Position = Boid->GetOwner()->GetActorLocation();
	BoidDataIO[Index].Direction = Boid->GetOwner()->GetActorForwardVector();

}


const TArray<AActor*>& ABoidSpawner::GetBoids() const
{
	return Boids;
}

