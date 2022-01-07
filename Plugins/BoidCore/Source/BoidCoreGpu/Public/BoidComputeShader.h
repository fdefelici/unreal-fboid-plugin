#pragma once

#include "RHIResources.h"
#include "BoidComputeShaderDeclaration.h"

class BOIDCOREGPU_API FBoidComputeShader
{
public:
	FBoidComputeShader();
	void Execute(TArray<BoidData_t>& DataIO);
	bool IsCompleted();
private:
	bool m_IsCompleted;
};