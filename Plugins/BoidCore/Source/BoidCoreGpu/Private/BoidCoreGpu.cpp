// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoidCoreGpu.h"

void FBoidCoreGpuModule::StartupModule()
{
	FString ShaderPath = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("BoidCore/Shaders/Private"));
	AddShaderSourceDirectoryMapping("/BoidShaders", ShaderPath);
}

void FBoidCoreGpuModule::ShutdownModule()
{
	
}

IMPLEMENT_MODULE(FBoidCoreGpuModule, BoidCoreGpu)