// (FDF) Fill out your copyright notice in the Description page of Project Settings.
#pragma optimize("", off)

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
		, bVisibleOnlyForSelected(InComponent->bVisibleOnlyForSelected)
		, Settings(InComponent->Settings)
		, ColliderRadiusColor(InComponent->ColliderRadiusColor)
		, AlignmentRadiusColor(InComponent->AlignmentRadiusColor)
		, CohesionRadiusColor(InComponent->CohesionRadiusColor)
		, SeparationRadiusColor(InComponent->SeparationRadiusColor)
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
				DrawCircle(PDI, Center, AlignX, AlignY, ColliderRadiusColor, Settings->ColliderRadius, Smoothness, SDPG_Foreground);
				DrawCircle(PDI, Center, AlignX, AlignY, AlignmentRadiusColor, Settings->AlignmentRadius, Smoothness, SDPG_Foreground);
				DrawCircle(PDI, Center, AlignX, AlignY, CohesionRadiusColor, Settings->CohesionRadius, Smoothness, SDPG_Foreground);
				DrawCircle(PDI, Center, AlignX, AlignY, SeparationRadiusColor, Settings->SeparationRadius, Smoothness, SDPG_Foreground);

				//UKismetSystemLibrary::DrawDebugCircle(GetWorld(), Center, Settings->AlignmentRadius, Smoothness, FLinearColor::Green);
				//UKismetSystemLibrary::DrawDebugCircle(GetWorld(), Center, Settings->CohesionRadius, Smoothness, FLinearColor::Blue);
				//UKismetSystemLibrary::DrawDebugCircle(GetWorld(), Center, Settings->SeparationRadius, Smoothness, FLinearColor::Yellow);
			}
		}
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{

		const bool HasSettings = Settings != nullptr;
		const bool IsSelected = IsParentSelected() || IsIndividuallySelected();

		FPrimitiveViewRelevance Result;
		//Result.bDrawRelevance = IsShown(View) && (View->Family->EngineShowFlags.BillboardSprites);
		Result.bDrawRelevance = (!bVisibleOnlyForSelected || (bVisibleOnlyForSelected && IsSelected)) && (HasSettings && IsShown(View));
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
	bool bVisibleOnlyForSelected;
	UBoidSettings* Settings;
	FLinearColor ColliderRadiusColor;
	FLinearColor AlignmentRadiusColor;
	FLinearColor CohesionRadiusColor;
	FLinearColor SeparationRadiusColor;
};

UBoidDrawComponent::UBoidDrawComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseEditorCompositing = false;
	SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	SetGenerateOverlapEvents(false);
	bIsEditorOnly = true; //remove during UE game build
	bHiddenInGame = true; //by default, not visible on PIE

	ColliderRadiusColor = FColor::Red;
	AlignmentRadiusColor = FColor::Blue;
	CohesionRadiusColor = FColor::Green;
	SeparationRadiusColor = FColor::Black;
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

void UBoidDrawComponent::SetVisibleOnlySelected(bool bOnlySelected)
{
	bVisibleOnlyForSelected = bOnlySelected;
	MarkRenderStateDirty(); //Force SceneProxy to be re-created
}
#pragma optimize("", on)