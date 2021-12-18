// (FDF) Fill out your copyright notice in the Description page of Project Settings.


#include "BoidDirections.h"

FBoidDirectionsInternal::FBoidDirectionsInternal()
{
    int NumViewDirections = 60;
    Directions.SetNum(NumViewDirections);

    float GoldenRatio = (1.f + FMath::Sqrt(5.f)) / 2.f;
    float AngleIncrement = PI * 2.f * GoldenRatio;
    for (int Index = 0; Index < NumViewDirections; Index++)
    {
        float Step = (float)Index / NumViewDirections;
        float ZenithAngle = FMath::Acos(1.f - 2.f * Step);
        float AzimuthAngle = AngleIncrement * Index;

        float y = FMath::Sin(ZenithAngle) * FMath::Cos(AzimuthAngle);
        float z = FMath::Sin(ZenithAngle) * FMath::Sin(AzimuthAngle);
        float x = FMath::Cos(ZenithAngle);
        Directions[Index] = FVector(x, y, z);
    }
}

FBoidDirectionsInternal FBoidDirections::Internal;