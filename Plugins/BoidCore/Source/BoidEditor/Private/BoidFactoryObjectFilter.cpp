// (FDF) Fill out your copyright notice in the Description page of Project Settings.


#include "BoidFactoryObjectFilter.h"

BoidFactoryObjectFilter::BoidFactoryObjectFilter()
{
}

BoidFactoryObjectFilter::~BoidFactoryObjectFilter()
{
}

TArray<FDetailsViewObjectRoot> BoidFactoryObjectFilter::FilterObjects(const TArray<UObject*>& SourceObjects)
{
	TArray<FDetailsViewObjectRoot> Roots;
	/*
	if (bAllowMultipleRoots)
	{
		Roots.Reserve(SourceObjects.Num());
		for (UObject* Object : SourceObjects)
		{
			Roots.Emplace(Object);
		}
	}
	else
	{
		Roots.Emplace(SourceObjects);
	}
	*/
	Roots.Emplace(SourceObjects);
	return Roots;
}