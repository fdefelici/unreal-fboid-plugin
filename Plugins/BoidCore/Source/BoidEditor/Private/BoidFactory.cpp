// (FDF) Fill out your copyright notice in the Description page of Project Settings.


#include "BoidFactory.h"
#include "BoidFactoryInput.h"
#include "AssetTypeCategories.h"
#include "Editor/EditorEngine.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Widgets/SWindow.h"

#include "Widgets/SBoxPanel.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Layout/SUniformGridPanel.h"

#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"
#include "Kismet2/SClassPickerDialog.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "BoidBehaviour.h"
#include "Engine/SimpleConstructionScript.h"
#include "BoidFactoryDialog.h"
#include "BoidDrawComponent.h"
#include "Engine/SCS_Node.h"
#include "BoidBehaviourGpu.h"

UBoidFactory::UBoidFactory()
{
	//bCreateNew = true;
	
	//Determines the asset icon, before calling the method CreateXXX
	//(first the icon is created, then the input focus is set to the asset label to be let the user set the asset name)
	SupportedClass = UBlueprint::StaticClass();
}


bool UBoidFactory::ShouldShowInNewMenu() const
{
	return true;
}

uint32 UBoidFactory::GetMenuCategories() const
{
	return EAssetTypeCategories::Blueprint;
};

FText UBoidFactory::GetDisplayName() const
{
	return FText::FromString("Boidize Asset");
}

FText UBoidFactory::GetToolTip() const
{
	return FText::FromString("Create a Boid Asset");
}

template<typename T>
static bool HasComponent(UBlueprint* Blueprint)
{
	USimpleConstructionScript* BPScript = Blueprint->SimpleConstructionScript;
	TArray<USCS_Node*> Nodes = BPScript->GetAllNodes();
	for (auto It = Nodes.begin(); It != Nodes.end(); ++It)
	{
		USCS_Node* Each = *It;
		if (Each->ComponentClass->IsChildOf(T::StaticClass()))
		{
			return true;
		}
	}
	return false;
}

static void AttachBoidComponents(UBlueprint* BPAsset)
{
	USimpleConstructionScript* BPScript = BPAsset->SimpleConstructionScript;

	if (!HasComponent<UBoidBehaviour>(BPAsset))
	{
		//USCS_Node* Node = BPScript->CreateNode(UBoidBehaviour::StaticClass(), TEXT("Boid Behaviour"));
		USCS_Node* Node = BPScript->CreateNode(UBoidBehaviourGpu::StaticClass(), TEXT("Boid Behaviour"));
		BPScript->AddNode(Node);
		FKismetEditorUtilities::CompileBlueprint(BPAsset);
	}

	if (!HasComponent<UBoidDrawComponent>(BPAsset))
	{
		USCS_Node* Node = BPScript->CreateNode(UBoidDrawComponent::StaticClass(), TEXT("Boid Draw"));
		BPScript->AddNode(Node);
		FKismetEditorUtilities::CompileBlueprint(BPAsset);
	}
}

bool UBoidFactory::ConfigureProperties() 
{	
	
	BoidInput = NewObject<UBoidFactoryInput>();

	bool Confirmed = SBoidFactoryDialog::Open(BoidInput);

	if (Confirmed && BoidInput->bInPlace)
	{

		//Adding actor cmponent to a blueprint
		//Reference: https://forums.unrealengine.com/t/adding-components-and-modifying-properties-of-ublueprint-asset/41992/7
		//See also: FKismetEditorUtilities::CreateBlueprintUsingAsset
		//Maybe a better management when adding a node
		//NOTE: another useful source class is FCreateBlueprintFromActorDialog
		AttachBoidComponents(BoidInput->ActorBlueprint);
		
		return false;
	}
	return Confirmed;
}


UObject* UBoidFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{

	//FKismetEditorUtilities::CreateBlueprint
	//AActor* Obj = NewObject<AActor>(InParent, SelectedClass, InName, Flags);

	UBlueprint* BPAsset = DuplicateObject(BoidInput->ActorBlueprint, InParent, InName);	
	AttachBoidComponents(BPAsset);

	return BPAsset;
}
