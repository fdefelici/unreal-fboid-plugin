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


	FOnActorSpawned::FDelegate ActorSpawnedDelegate = FOnActorSpawned::FDelegate::CreateLambda([this](AActor* Instance) {
		UE_LOG(LogTemp, Warning, TEXT("Hey %s: %d"), *Instance->GetName(), Instance->GetComponents().Num());

		//UBoidBehaviour* BoidBehav = Cast<UBoidBehaviour>(Instance->GetComponentByClass(UBoidBehaviour::StaticClass()));
		UBoidBehaviourGpu* BoidBehav = Cast<UBoidBehaviourGpu>(Instance->GetComponentByClass(UBoidBehaviourGpu::StaticClass()));
		if (!BoidBehav) return;
		BoidBehav->SetSettings(Settings);
		BoidBehav->SetSpawner(this);

		#if WITH_EDITOR
		UBoidDrawComponent* BoidDraw = Cast<UBoidDrawComponent>(Instance->GetComponentByClass(UBoidDrawComponent::StaticClass()));
		if (!BoidDraw) return;
		BoidDraw->SetSettings(Settings);
		#endif
		 
	});
	//BeginPlay will be called after the execution of this delegate
	GetWorld()->AddOnActorSpawnedHandler(ActorSpawnedDelegate);

	for (int Index = 0; Index < Settings->BoidCount; ++Index)
	{
		FVector Position = GetActorLocation() + UKismetMathLibrary::RandomUnitVector() * Settings->SpawnRadius;
		FVector Direction = UKismetMathLibrary::RandomUnitVector();
		
		FTransform Transform(Direction.Rotation(), Position);

		AActor* BoidInstance = GetWorld()->SpawnActor<AActor>(BoidPrefab, Transform);
	
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

	GetWorld()->RemoveOnActorSpawnedHandler(ActorSpawnedDelegate.GetHandle());

	Input.SetNum(Settings->BoidCount);
	ComputeShader.Init(Settings->BoidCount);
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


	//Create only at BeginPlay or CTOR
	//static TArray<BoidData_t> Input;
	//Input.SetNum(Boids.Num());
	
	{
		SCOPE_CYCLE_COUNTER(STAT_MyInputStats);
		//ForEach Boid update data: 
		for (int Index = 0; Index < Boids.Num(); ++Index)
		{
			AActor* Actor = Boids[Index];
			//UBoidBehaviour* BoidBehav = Cast<UBoidBehaviour>(Actor->GetComponentByClass(UBoidBehaviour::StaticClass()));
		
			//Input
			Input[Index].Position = Actor->GetActorLocation();
			Input[Index].Direction = Actor->GetActorForwardVector();
		
			//Output
			Input[Index].AlignCount = 0;
			Input[Index].FlockDirection = FVector::ZeroVector;
			Input[Index].CohesionCount = 0;
			Input[Index].FlockPosition = FVector::ZeroVector;
			Input[Index].SeparationCount = 0;
			Input[Index].FlockSeparationDirection = FVector::ZeroVector;
			Input[Index].SeparationCount = 0;
		}
	}

	{
		SCOPE_CYCLE_COUNTER(STAT_MyExecuteStats);
		ComputeShader.Execute(Input);
	}


	{
		SCOPE_CYCLE_COUNTER(STAT_MyOutputStats);
		const TArray<BoidData_t>& Result = ComputeShader.GetResult();
		for (int Index = 0; Index < Boids.Num(); ++Index)
		{
			AActor* Actor = Boids[Index];
			UBoidBehaviourGpu* BoidBehav = Cast<UBoidBehaviourGpu>(Actor->GetComponentByClass(UBoidBehaviourGpu::StaticClass()));
			BoidBehav->_UpdateData(
				Result[Index].AlignCount,
				Result[Index].FlockDirection,
				Result[Index].CohesionCount,
				Result[Index].FlockPosition,
				Result[Index].SeparationCount,
				Result[Index].FlockSeparationDirection
			);
		}
	}
}

const TArray<AActor*>& ABoidSpawner::GetBoids() const
{
	return Boids;
}

