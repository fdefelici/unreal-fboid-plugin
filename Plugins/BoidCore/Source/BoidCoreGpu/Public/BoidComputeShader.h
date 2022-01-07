#pragma once

#include "RHIResources.h"
#include "BoidComputeShaderDeclaration.h"

class BOIDCOREGPU_API FBoidComputeShader
{
public:
	FBoidComputeShader();
	void Execute(TArray<BoidDataInput_t>& DataIO, TArray<BoidData_t>& DataOut);
	bool IsCompleted();
private:
	bool m_IsCompleted;
};