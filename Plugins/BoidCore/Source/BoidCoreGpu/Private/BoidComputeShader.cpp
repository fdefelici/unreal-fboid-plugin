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

DECLARE_STATS_GROUP(TEXT("BoidShader"), STATGROUP_BoidShader, STATCAT_Advanced);

DECLARE_CYCLE_STAT(TEXT("BufferSetup"), STAT_BufferSetup, STATGROUP_BoidShader);
DECLARE_CYCLE_STAT(TEXT("Execute"), STAT_Execute, STATGROUP_BoidShader);
DECLARE_CYCLE_STAT(TEXT("BufferRead"), STAT_BufferRead, STATGROUP_BoidShader);

void FBoidComputeShader::Execute(float AlignRadius, float CohesionRadius, float SeparationRadius, TArray<BoidDataInput_t>& DataInput, TArray<BoidData_t>& DataOutput)
{
	//FRenderCommandFence Fence;
	m_IsCompleted = false;
	ENQUEUE_RENDER_COMMAND(FBoidComputeShaderCommand)(
		[this, &DataInput, &DataOutput, AlignRadius, CohesionRadius, SeparationRadius](FRHICommandListImmediate& RHICommands)
		{
			FRDGBuilder GraphBuilder(RHICommands);


			uint32 BeginTime, EndTime;

			BeginTime = FPlatformTime::Cycles();
			FRDGBufferRef BoidInputBuffer = CreateStructuredBuffer(GraphBuilder, TEXT("BoidInputBufferPass"), sizeof(BoidDataInput_t), DataInput.Num(),
				DataInput.GetData(), sizeof(BoidDataInput_t) * DataInput.Num(), ERDGInitialDataFlags::NoCopy); //Usare "No Copy" se Input non muore dopo la chiamata a Execute.
			FRDGBufferSRVRef BoidInputBufferSRV = GraphBuilder.CreateSRV(BoidInputBuffer);


			FRDGBufferRef BoidDataBuffer = GraphBuilder.CreateBuffer(FRDGBufferDesc::CreateStructuredDesc(sizeof(BoidData_t), DataOutput.Num()), TEXT("BoidOutputBufferPass"));
			FRDGBufferUAVRef BoidDataBufferUAV = GraphBuilder.CreateUAV(BoidDataBuffer);

			EndTime = FPlatformTime::Cycles();
			SET_CYCLE_COUNTER(STAT_BufferSetup, EndTime - BeginTime);

			BeginTime = FPlatformTime::Cycles();
			TShaderMapRef<FBoidComputeShaderDeclaration> ComputeShaderDecl(GetGlobalShaderMap(ERHIFeatureLevel::SM5));

		
			FBoidComputeShaderDeclaration::FParameters* PassParams = GraphBuilder.AllocParameters<FBoidComputeShaderDeclaration::FParameters>();
			PassParams->BoidDataOutput = BoidDataBufferUAV;
			PassParams->BoidDataInput = BoidInputBufferSRV;
			PassParams->BoidCount = DataInput.Num();
			PassParams->AlignRadius = AlignRadius;
			PassParams->CohesionRadius = CohesionRadius;
			PassParams->SeparationRadius = SeparationRadius;

			//https://answers.unrealengine.com/questions/978809/loading-data-tofrom-structured-buffer-compute-shad.html
			//int X = FMath::DivideAndRoundUp(BoidCount, FBoidComputeShaderDeclaration::ThreadsPerGroup.X);
			FIntVector GroupCounts = FComputeShaderUtils::GetGroupCount({ DataInput.Num(), 1, 1}, FBoidComputeShaderDeclaration::ThreadsPerGroup);

			//Execute the Compute Shader (synchrnous call)
			FComputeShaderUtils::AddPass(GraphBuilder, RDG_EVENT_NAME("BoidComputeShaderPass"),
				ComputeShaderDecl, PassParams, GroupCounts);

			//Extends life time of the Structured Buffer (otherwise destroyed at end of the Pass)
			TRefCountPtr<FRDGPooledBuffer> PooledBuffer;
			GraphBuilder.QueueBufferExtraction(BoidDataBuffer, &PooledBuffer, ERHIAccess::CPURead);

			GraphBuilder.Execute();
			EndTime = FPlatformTime::Cycles();
			SET_CYCLE_COUNTER(STAT_Execute, EndTime - BeginTime);

			// Read back data from buffer
			BeginTime = FPlatformTime::Cycles();
			FRHIStructuredBuffer* StructuredBuffer = PooledBuffer->GetStructuredBufferRHI();
			BoidData_t* data = (BoidData_t*)RHILockStructuredBuffer(StructuredBuffer, 0, sizeof(BoidData_t) * DataOutput.Num(), RLM_ReadOnly);
			FMemory::Memcpy(DataOutput.GetData(), data, sizeof(BoidData_t) * DataOutput.Num());
			RHIUnlockStructuredBuffer(StructuredBuffer);
			EndTime = FPlatformTime::Cycles();
			SET_CYCLE_COUNTER(STAT_BufferRead, EndTime - BeginTime);

			m_IsCompleted = true;
		}
	);

	//Fence.BeginFence();
	//Fence.Wait();
}