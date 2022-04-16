// (FDF) Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BoidInspectorInput.generated.h"

UENUM()
enum EBoidIstance
{
	All     UMETA(DisplayName = "All Boids"),
	Selected      UMETA(DisplayName = "Only Selected"),
};

/**
 * 
 */
UCLASS(Config = EditorPerProjectUserSettings)
class BOIDEDITOR_API UBoidInspectorInput : public UObject
{
	GENERATED_BODY()
public:
	/*
	UBoidInspectorInput() {
		FString IsDefaultObj = HasAnyFlags(EObjectFlags::RF_ClassDefaultObject) ? "true" : "false";
		UE_LOG(LogTemp, Warning,
			TEXT("UObject '%s' (IsDefault: %s) created at [%p]"),
			*this->GetName(), *IsDefaultObj, this);
	}

	~UBoidInspectorInput() {
		UE_LOG(LogTemp, Warning, TEXT("UObject deleted at [%p]"), this);
	}
	*/
	
	//Simulating Auto-Saving
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override {
		Super::PostEditChangeProperty(PropertyChangedEvent);
		SaveConfig();
	}
	
	/* Enable Visual Debugging (e.g. sensor radius) */
	UPROPERTY(EditAnywhere, Config, Category = "Debug")
	bool bEnableVisualDebug;
	/* Enable Visual Debugging (e.g. sensor radius) */
	UPROPERTY(EditAnywhere, Config, Category = "Debug")
	TEnumAsByte<EBoidIstance> InstancesToDebug;
};
