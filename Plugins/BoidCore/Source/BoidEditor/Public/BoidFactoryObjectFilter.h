// (FDF) Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DetailsViewObjectFilter.h"

/**
 * 
 */
class BOIDEDITOR_API BoidFactoryObjectFilter : public FDetailsViewObjectFilter
{
public:
	BoidFactoryObjectFilter();
	~BoidFactoryObjectFilter();
	TArray<FDetailsViewObjectRoot> FilterObjects(const TArray<UObject*>& SourceObjects) override;
};
