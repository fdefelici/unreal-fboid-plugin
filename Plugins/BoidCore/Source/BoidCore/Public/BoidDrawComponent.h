// (FDF) Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PrimitiveComponent.h"
#include "Math/Color.h"
#include "BoidDrawComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = Utility,
	hidecategories = (Object, LOD, Physics, Lighting, TextureStreaming, Activation, "Components|Activation",
		Collision, Navigation, ComponentReplication, Events, HLOD, Tags, Mobile, AssetUserData),
	editinlinenew, meta = (BlueprintSpawnableComponent))
class BOIDCORE_API UBoidDrawComponent : public UPrimitiveComponent
{
	GENERATED_UCLASS_BODY()

public:
	void SetSettings(class UBoidSettings* Settings);
	void SetVisibleOnlySelected(bool bOnlySelected);
	bool bVisibleOnlyForSelected;

	UPROPERTY(EditAnywhere, Category = "Boid")
	class UBoidSettings* Settings;

	UPROPERTY(EditAnywhere, Category = "Boid")
	FLinearColor ColliderRadiusColor;
	UPROPERTY(EditAnywhere, Category = "Boid")
	FLinearColor AlignmentRadiusColor;
	UPROPERTY(EditAnywhere, Category = "Boid")
	FLinearColor CohesionRadiusColor;
	UPROPERTY(EditAnywhere, Category = "Boid")
	FLinearColor SeparationRadiusColor;

private:
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
};
