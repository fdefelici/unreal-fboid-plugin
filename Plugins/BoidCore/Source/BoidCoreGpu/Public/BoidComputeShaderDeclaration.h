// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

typedef struct BoidData_t
{
	FVector FlockDirection;
	FVector FlockPosition;
	FVector FlockSeparationDirection;
	int AlignCount;
	int CohesionCount;
	int SeparationCount;
} BoidData_t;

typedef struct BoidDataInput_t
{
	FVector Position;
	FVector Direction;
} BoidDataInput_t;


class FBoidComputeShaderDeclaration : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FBoidComputeShaderDeclaration);
	SHADER_USE_PARAMETER_STRUCT(FBoidComputeShaderDeclaration, FGlobalShader);
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<BoidDataInput_t>, BoidDataInput)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<BoidData_t>, BoidDataOutput)
		SHADER_PARAMETER(int, BoidCount)
		SHADER_PARAMETER(float, AlignRadius)
		SHADER_PARAMETER(float, CohesionRadius)
		SHADER_PARAMETER(float, SeparationRadius)
	END_SHADER_PARAMETER_STRUCT()

public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters);

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

	static const FIntVector ThreadsPerGroup;
};