#include "BoidComputeShader.h"
#include "BoidComputeShaderDeclaration.h"
#include "RenderGraphUtils.h"

FBoidComputeShader::FBoidComputeShader()
{
	/*
	FStructuredBufferRHIRef m_BoidDataBuffer;
	FShaderResourceViewRHIRef m_BoidDataSRV;

	FStructuredBufferRHIRef m_OuputBuffer;
	FUnorderedAccessViewRHIRef m_OuputUAV;
	*/
	//int BoidCount;
	
}

void FBoidComputeShader::Init(int BoidCount)
{
	m_Result.SetNum(BoidCount);
}

bool FBoidComputeShader::IsCompleted()
{
	return m_IsCompleted;
}

const TArray<BoidData_t>& FBoidComputeShader::GetResult() const
{
	return m_Result;
}

void FBoidComputeShader::Execute(TResourceArray<BoidData_t>& Input)
{
	m_IsCompleted = false;

	ENQUEUE_RENDER_COMMAND(FBoidComputeShaderCommand)(
		[this, Input](FRHICommandListImmediate& RHICommands)
		{
			int BoidCount = Input.Num();
			if (m_BoidDataUAV.IsValid())
			{
				m_BoidDataUAV.SafeRelease();
			}

			FRHIResourceCreateInfo CreateInfo;
			CreateInfo.ResourceArray = (FResourceArrayInterface*)&Input;
			m_BoidDataBuffer = RHICreateStructuredBuffer(sizeof(BoidData_t), sizeof(BoidData_t) * BoidCount, BUF_UnorderedAccess | BUF_ShaderResource, CreateInfo);
			m_BoidDataUAV = RHICreateUnorderedAccessView(m_BoidDataBuffer, false, false);


			TShaderMapRef<FBoidComputeShaderDeclaration> ComputeShaderDecl(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

			FBoidComputeShaderDeclaration::FParameters PassParams;
			PassParams.BoidData = m_BoidDataUAV;
			PassParams.BoidCount = BoidCount;
			PassParams.AlignRadius = 300.f;
			PassParams.CohesionRadius = 300.f;
			PassParams.SeparationRadius = 100.f;

			//Eventualmente: FComputeShaderUtils::GetGroupCount ??? Gia fa il calcolo a livello di FIntVector
			//https://answers.unrealengine.com/questions/978809/loading-data-tofrom-structured-buffer-compute-shad.html
			int X = FMath::DivideAndRoundUp(BoidCount, FBoidComputeShaderDeclaration::ThreadsPerGroup.X);

			FIntVector GroupCounts = FIntVector(X, 1, 1);
			
			//Execute the Compute Shader (synchrnous call)
			FComputeShaderUtils::Dispatch(RHICommands, ComputeShaderDecl, PassParams, GroupCounts);

			// Read back data from buffer (Blocking: Maybe better adding a copy-data Pass using RDG (Render Graph)?!?
			BoidData_t* data = (BoidData_t*)RHILockStructuredBuffer(m_BoidDataBuffer, 0, sizeof(BoidData_t) * m_Result.Num(), RLM_ReadOnly);
			FMemory::Memcpy(m_Result.GetData(), data, sizeof(BoidData_t) * m_Result.Num());
			RHIUnlockStructuredBuffer(m_BoidDataBuffer);

			m_IsCompleted = true;
		}
	);
}