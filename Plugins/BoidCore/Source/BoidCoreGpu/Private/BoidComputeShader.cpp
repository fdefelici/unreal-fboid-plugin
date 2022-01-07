#include "BoidComputeShader.h"
#include "BoidComputeShaderDeclaration.h"
#include "RenderGraphUtils.h"

FBoidComputeShader::FBoidComputeShader()
{
	m_IsCompleted = true;
}

bool FBoidComputeShader::IsCompleted()
{
	return m_IsCompleted;
}

DECLARE_STATS_GROUP(TEXT("ProvaGroup"), STATGROUP_ProvaGroup, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("Prova"), STAT_MyProva, STATGROUP_ProvaGroup);
DECLARE_CYCLE_STAT(TEXT("Prova2"), STAT_MyProva2, STATGROUP_ProvaGroup);
DECLARE_CYCLE_STAT(TEXT("Prova3"), STAT_MyProva3, STATGROUP_ProvaGroup);

void FBoidComputeShader::Execute(TArray<BoidDataInput_t>& DataIO, TArray<BoidData_t>& DataOutput)
{
	//FRenderCommandFence Fence;
	m_IsCompleted = false;
	ENQUEUE_RENDER_COMMAND(FBoidComputeShaderCommand)(
		[this, &DataIO, &DataOutput](FRHICommandListImmediate& RHICommands)
		{
			FRDGBuilder GraphBuilder(RHICommands);


			uint32 BeginTime, EndTime;

			BeginTime = FPlatformTime::Cycles();
			FRDGBufferRef BoidInputBuffer = CreateStructuredBuffer(GraphBuilder, TEXT("BoidInputBufferPass"), sizeof(BoidDataInput_t), DataIO.Num(),
				DataIO.GetData(), sizeof(BoidDataInput_t) * DataIO.Num(), ERDGInitialDataFlags::NoCopy); //Usare "No Copy" se Input non muore dopo la chiamata a Execute.
			FRDGBufferSRVRef BoidInputBufferSRV = GraphBuilder.CreateSRV(BoidInputBuffer);


			FRDGBufferRef BoidDataBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(BoidData_t), DataOutput.Num()), TEXT("BoidOutputBufferPass"));
			FRDGBufferUAVRef BoidDataBufferUAV = GraphBuilder.CreateUAV(BoidDataBuffer);

			EndTime = FPlatformTime::Cycles();
			SET_CYCLE_COUNTER(STAT_MyProva, EndTime - BeginTime);

			BeginTime = FPlatformTime::Cycles();
			TShaderMapRef<FBoidComputeShaderDeclaration> ComputeShaderDecl(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

		
			FBoidComputeShaderDeclaration::FParameters* PassParams = GraphBuilder.AllocParameters<FBoidComputeShaderDeclaration::FParameters>();
			PassParams->BoidData = BoidDataBufferUAV;
			PassParams->BoidDataInput = BoidInputBufferSRV;
			PassParams->BoidCount = DataIO.Num();
			PassParams->AlignRadius = 300.f;
			PassParams->CohesionRadius = 300.f;
			PassParams->SeparationRadius = 100.f;

			//https://answers.unrealengine.com/questions/978809/loading-data-tofrom-structured-buffer-compute-shad.html
			//int X = FMath::DivideAndRoundUp(BoidCount, FBoidComputeShaderDeclaration::ThreadsPerGroup.X);
			FIntVector GroupCounts = FComputeShaderUtils::GetGroupCount({ DataIO.Num(), 1, 1}, FBoidComputeShaderDeclaration::ThreadsPerGroup);

			//Execute the Compute Shader (synchrnous call)
			FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("BoidComputeShaderPass"),
				ComputeShaderDecl, PassParams, GroupCounts);

			//Extends life time of the Structured Buffer (otherwise destroyed at end of the Pass)
			TRefCountPtr<FRDGPooledBuffer> PooledBuffer;
			GraphBuilder.QueueBufferExtraction(BoidDataBuffer, &PooledBuffer, ERHIAccess::CPURead);

			GraphBuilder.Execute();
			EndTime = FPlatformTime::Cycles();
			SET_CYCLE_COUNTER(STAT_MyProva2, EndTime - BeginTime);

			// Read back data from buffer
			BeginTime = FPlatformTime::Cycles();
			FRHIStructuredBuffer* StructuredBuffer = PooledBuffer->GetStructuredBufferRHI();
			BoidData_t* data = (BoidData_t*)RHILockStructuredBuffer(StructuredBuffer, 0, sizeof(BoidData_t) * DataOutput.Num(), RLM_ReadOnly);
			FMemory::Memcpy(DataOutput.GetData(), data, sizeof(BoidData_t) * DataOutput.Num());
			RHIUnlockStructuredBuffer(StructuredBuffer);
			EndTime = FPlatformTime::Cycles();
			SET_CYCLE_COUNTER(STAT_MyProva3, EndTime - BeginTime);

			m_IsCompleted = true;
		}
	);

	//Fence.BeginFence();
	//Fence.Wait();
}