#pragma once

#include "RHIResources.h"
#include "BoidComputeShaderDeclaration.h"

class BOIDCOREGPU_API FBoidComputeShader
{
public:
	FBoidComputeShader();
	void Init(int BoidCount);
	void Execute(TArray<BoidData_t>& Input);
	const TArray<BoidData_t>& GetResult() const;
	bool IsCompleted();
private:
	TArray<BoidData_t> m_Result;
	bool m_IsCompleted;
};