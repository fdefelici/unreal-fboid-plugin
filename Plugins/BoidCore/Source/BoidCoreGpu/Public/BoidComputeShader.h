#pragma once

#include "RHIResources.h"
#include "BoidComputeShaderDeclaration.h"
#include "Containers/DynamicRHIResourceArray.h"

class BOIDCOREGPU_API FBoidComputeShader
{
public:
	FBoidComputeShader();
	void Init(int BoidCount);
	void Execute(TResourceArray<BoidData_t>& Input);
	bool IsCompleted();
	const TArray<BoidData_t>& GetResult() const;
private:
	FStructuredBufferRHIRef m_BoidDataBuffer;
	FUnorderedAccessViewRHIRef m_BoidDataUAV;
	bool m_IsCompleted;
	TArray<BoidData_t> m_Result;
};