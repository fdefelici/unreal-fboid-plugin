// (FDF) Fill out your copyright notice in the Description page of Project Settings.


#include "BoidFactory.h"
#include "BoidFactoryInput.h"
#include "AssetTypeCategories.h"
#include "Editor/EditorEngine.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Widgets/SWindow.h"

#include "BoidFactoryObjectFilter.h"
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

class FBoidFactoryFilter : public IClassViewerFilter
{
public:
	TSet< const UClass* > AllowedChildOfClasses;

	TSet< const UClass*> DisallowedChildOfClasses;

	bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		/*
		if (InClass->IsChildOf<AActor>()) return true;
		return false;
		*/
		if (DisallowedChildOfClasses.Num() == 0 && AllowedChildOfClasses.Num() == 0)
		{
			return true;
		}
		return (InFilterFuncs->IfInChildOfClassesSet(AllowedChildOfClasses, InClass) != EFilterReturn::Failed)
			&& (InFilterFuncs->IfInChildOfClassesSet(DisallowedChildOfClasses, InClass) == EFilterReturn::Failed);
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		if (DisallowedChildOfClasses.Num() == 0 && AllowedChildOfClasses.Num() == 0)
		{
			return true;
		}

		return InFilterFuncs->IfInChildOfClassesSet(AllowedChildOfClasses, InUnloadedClassData) != EFilterReturn::Failed
			&& (InFilterFuncs->IfInChildOfClassesSet(DisallowedChildOfClasses, InUnloadedClassData) == EFilterReturn::Failed);;
	}
};



UBoidFactory::UBoidFactory()
{
	//bCreateNew = true;
	
	//Determina l'icona dell'asset prima che venga chiamato il metodo CreateXXX
	// Ovvero viene creata l'icona e poi c'e' il focus sul la label dell'asset per cambiargli nome.
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

		//QUESTO RIESCE AD AGGIUNGERE UN ACTOR COMPONENT ALLA BLUEPRINT
		//Reference: https://forums.unrealengine.com/t/adding-components-and-modifying-properties-of-ublueprint-asset/41992/7
		//Vedi anche: FKismetEditorUtilities::CreateBlueprintUsingAsset
		// Forse ha una migliore gestione nell'aggiunta di un nodo 
		//NOTA: Altra classe utile da Vedere e' FCreateBlueprintFromActorDialog
		AttachBoidComponents(BoidInput->ActorBlueprint);
		
		return false;
	}
	return Confirmed;


	/*
	FClassViewerInitializationOptions Options;
	Options.bIsActorsOnly = true;

	TSharedPtr<FBoidFactoryFilter> Filter = MakeShareable(new FBoidFactoryFilter);
	//Filter->AllowedChildOfClasses.Add(AActor::StaticClass());

	Options.ClassFilter = Filter;


	const FText TitleText = NSLOCTEXT("EditorFactories", "CreateBlueprintOptions", "Pick Parent Class");
	UClass* ChosenClass = NULL;
	const bool bPressedOk = SClassPickerDialog::PickClass(TitleText, Options, ChosenClass, AActor::StaticClass());
	if (bPressedOk)
	{	
		//Nel caso seleziono una classe Blueprint, sara ritornata la "_C" (ovvero la classe autogenerata)
		SelectedClass = ChosenClass;
	}

	return bPressedOk;
	*/
}


UObject* UBoidFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{

	//FKismetEditorUtilities::CreateBlueprint

	//AActor* Obj = NewObject<AActor>(InParent, SelectedClass, InName, Flags);

	UBlueprint* BPAsset = DuplicateObject(BoidInput->ActorBlueprint, InParent, InName);	
	AttachBoidComponents(BPAsset);

	return BPAsset;
}
