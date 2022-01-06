// (FDF) Fill out your copyright notice in the Description page of Project Settings.


#include "BoidDrawComponent.h"
#include "BoidSettings.h"
#include "SceneManagement.h"
#include "GameFramework/Actor.h"

class FBoidDrawComponentSceneProxy final : public FPrimitiveSceneProxy
{
public:
	SIZE_T GetTypeHash() const override
	{
		static size_t UniquePointer;
		return reinterpret_cast<size_t>(&UniquePointer);
	}

	FBoidDrawComponentSceneProxy(const UBoidDrawComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent)
		, Settings(InComponent->Settings)
	{
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		QUICK_SCOPE_CYCLE_COUNTER(STAT_FBoidDrawComponentSceneProxy_DrawDynamicElements);

		const FMatrix& LocalToWorldMatrix = GetLocalToWorld();
		const FVector Center = LocalToWorldMatrix.GetOrigin();
		const FVector AlignX = LocalToWorldMatrix.GetScaledAxis(EAxis::X);
		const FVector AlignY = LocalToWorldMatrix.GetScaledAxis(EAxis::Y);
		const int Smoothness = 24;
		

		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (VisibilityMap & (1 << ViewIndex))
			{
				const FSceneView* View = Views[ViewIndex];

				FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
				DrawCircle(PDI, Center, AlignX, AlignY, FLinearColor::Red, Settings->ColliderRadius, Smoothness, SDPG_Foreground);
				
				DrawCircle(PDI, Center, AlignX, AlignY, FLinearColor::Green, Settings->AlignmentRadius, Smoothness, SDPG_Foreground);
				DrawCircle(PDI, Center, AlignX, AlignY, FLinearColor::Blue, Settings->CohesionRadius, Smoothness, SDPG_Foreground);
				DrawCircle(PDI, Center, AlignX, AlignY, FLinearColor::Yellow, Settings->SeparationRadius, Smoothness, SDPG_Foreground);

				//UKismetSystemLibrary::DrawDebugCircle(GetWorld(), Center, Settings->AlignmentRadius, Smoothness, FLinearColor::Green);
				//UKismetSystemLibrary::DrawDebugCircle(GetWorld(), Center, Settings->CohesionRadius, Smoothness, FLinearColor::Blue);
				//UKismetSystemLibrary::DrawDebugCircle(GetWorld(), Center, Settings->SeparationRadius, Smoothness, FLinearColor::Yellow);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{

		const bool HasSettings = Settings != nullptr;

		FPrimitiveViewRelevance Result;
		//Result.bDrawRelevance = IsShown(View) && (View->Family->EngineShowFlags.BillboardSprites);
		Result.bDrawRelevance = HasSettings && IsShown(View);
		Result.bDynamicRelevance = true;
		Result.bShadowRelevance = false;
		Result.bEditorPrimitiveRelevance = UseEditorCompositing(View);
		Result.bRenderCustomDepth = ShouldRenderCustomDepth();
		return Result;
	}

	virtual bool CanBeOccluded() const override
	{
		return false;
	}

	virtual uint32 GetMemoryFootprint(void) const override { return sizeof * this + GetAllocatedSize(); }
	uint32 GetAllocatedSize(void) const { return FPrimitiveSceneProxy::GetAllocatedSize(); }
private:
	UBoidSettings* Settings;
};

UBoidDrawComponent::UBoidDrawComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseEditorCompositing = false;
	//bHiddenInGame = true;
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetGenerateOverlapEvents(false);

	//Se aggiunto a Editor Time a un actor, 
	// (non riesco a usare WITH_EDITOR in teoria)
	//cosi non verra' caricato nella build di Runtime
	bIsEditorOnly = true;
}


FPrimitiveSceneProxy* UBoidDrawComponent::CreateSceneProxy()
{
	return new FBoidDrawComponentSceneProxy(this);
}

void UBoidDrawComponent::SetSettings(UBoidSettings* SomeSettings)
{
	Settings = SomeSettings;
	MarkRenderStateDirty(); //Force SceneProxy to be re-created
}