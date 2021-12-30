// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoidComputeShaderDeclaration.h"
#include "ShaderCompilerCore.h"

bool FBoidComputeShaderDeclaration::ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) {
	return GetMaxSupportedFeatureLevel(Parameters.Platform) >= ERHIFeatureLevel::SM5;
}

void FBoidComputeShaderDeclaration::ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Parameters, OutEnvironment);

	//Setting Macro in the Shader while compiling
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_X"), ThreadsPerGroup.X);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Y"), ThreadsPerGroup.Y);
	OutEnvironment.SetDefine(TEXT("THREADGROUPSIZE_Z"), ThreadsPerGroup.Z);
}

const FIntVector FBoidComputeShaderDeclaration::ThreadsPerGroup = { 1024, 1, 1 };

// This will tell the engine to create the shader and where the shader entry point is.
//                      Shader Class                   Shader Path (Virtual)          Shader Function      Shader Type
IMPLEMENT_GLOBAL_SHADER(FBoidComputeShaderDeclaration, "/BoidShaders/BoidShader.usf", "MainComputeShader", SF_Compute);