// (FDF) Fill out your copyright notice in the Description page of Project Settings.


#include "BoidSpawner.h"
#include "BoidBehaviour.h"
#include "BoidSettings.h"
#if WITH_EDITOR
#include "BoidDrawComponent.h"
#endif
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Engine/World.h"

#include "Containers/DynamicRHIResourceArray.h"
#include "BoidComputeShader.h"
#include "BoidComputeShaderDeclaration.h"

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

		UBoidBehaviour* BoidBehav = Cast<UBoidBehaviour>(Instance->GetComponentByClass(UBoidBehaviour::StaticClass()));
		if (!BoidBehav) return;
		BoidBehav->SetSettings(Settings);
		BoidBehav->SetSpawner(this);

		UBoidDrawComponent* BoidDraw = Cast<UBoidDrawComponent>(Instance->GetComponentByClass(UBoidDrawComponent::StaticClass()));
		if (!BoidDraw) return;
		BoidDraw->SetSettings(Settings);
		 
	});
	GetWorld()->AddOnActorSpawnedHandler(ActorSpawnedDelegate);

	for (int Index = 0; Index < Settings->BoidCount; ++Index)
	{
		FVector Position = GetActorLocation() + UKismetMathLibrary::RandomUnitVector() * Settings->SpawnRadius;
		FVector Direction = UKismetMathLibrary::RandomUnitVector();
		
		FTransform Transform(Direction.Rotation(), Position);

		AActor* BoidInstance = GetWorld()->SpawnActor<AActor>(BoidPrefab, Transform);
	
		/*
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


	ComputeShader.Init(Settings->BoidCount);
}

// Called every frame
void ABoidSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Create only at BeginPlay or CTOR
	TResourceArray<BoidData_t> Input;
	Input.SetNum(Boids.Num());
	

	//ForEach Boid update data: 
	for (int Index = 0; Index < Boids.Num(); ++Index)
	{
		AActor* Actor = Boids[Index];
		UBoidBehaviour* BoidBehav = Cast<UBoidBehaviour>(Actor->GetComponentByClass(UBoidBehaviour::StaticClass()));
		Input[Index].Position = Actor->GetActorLocation();
		Input[Index].Direction = Actor->GetActorForwardVector();
		Input[Index].FlockmatesCount = 0;
		Input[Index].AlignDirection = FVector::ZeroVector;
	}
	ComputeShader.Execute(Input);
	while (!ComputeShader.IsCompleted()) {
		//WAIT;
	}

	const TArray<BoidData_t>& Result = ComputeShader.GetResult();
	const BoidData_t& first = Result[0];
	const BoidData_t& second = Result[1];
	UE_LOG(LogTemp, Warning, TEXT("%p"), &first);
	UE_LOG(LogTemp, Warning, TEXT("%p"), &second);

}

const TArray<AActor*>& ABoidSpawner::GetBoids() const
{
	return Boids;
}

