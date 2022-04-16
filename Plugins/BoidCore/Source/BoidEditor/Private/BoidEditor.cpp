// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoidEditor.h"
#include "BoidDrawComponent.h"
#include "BoidSpawner.h"
#include "Editor/EditorEngine.h"
#include "EngineUtils.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Input/SCheckBox.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"
#include "Editor.h"
#include "PropertyEditorModule.h"
#include "BoidInspectorInput.h"



#define LOCTEXT_NAMESPACE "FBoidEditorModule"

static const FName TabId = FName(TEXT("BoidWindowId"));
#define TAB_TITLE "Boid Inspector"


static void ApplyDebugConfig(bool bIsDebugEnabled, EBoidIstance WhichBoid)
{
	UWorld* PIE = GEditor->PlayWorld;
	if (PIE)
	{
		for (TActorIterator<ABoidSpawner> AIt(PIE); AIt; ++AIt)
		{
			ABoidSpawner* Spawner = *AIt;
			const TArray<AActor*>& Boids = Spawner->GetBoids();
			for (int Index = 0; Index < Boids.Num(); ++Index)
			{
				AActor* EachBoid = Boids[Index];
				UActorComponent* EachComp = EachBoid->GetComponentByClass(UBoidDrawComponent::StaticClass());

				UBoidDrawComponent* EachDrawComp = Cast<UBoidDrawComponent>(EachComp);

				EachDrawComp->SetHiddenInGame(!bIsDebugEnabled);
				EachDrawComp->SetVisibleOnlySelected(EBoidIstance::Selected == WhichBoid);
			}
		}
	}
}

void FBoidEditorModule::StartupModule()
{
	Input = NewObject<UBoidInspectorInput>();
	Input->SetFlags(EObjectFlags::RF_Standalone);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabId, FOnSpawnTab::CreateRaw(this, &FBoidEditorModule::SpawnWindow))
		.SetDisplayName(LOCTEXT("MenuItem", TAB_TITLE))
		.SetTooltipText(LOCTEXT("MenuItemTooltip", "Open My Window tab"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "Launcher.TabIcon"));

	FEditorDelegates::PostPIEStarted.AddLambda([this](bool bIsSimulating) {
		ApplyDebugConfig(Input->bEnableVisualDebug, Input->InstancesToDebug);
	});
	
}

void FBoidEditorModule::ShutdownModule()
{
	//Input->SaveConfig();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabId);
	Input->MarkPendingKill();

}


TSharedRef<SDockTab> FBoidEditorModule::SpawnWindow(const FSpawnTabArgs& Args)
{
	/*
	auto IsChecked = [this]() -> ECheckBoxState { return IsCheckedStatus ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; };
	auto CheckedStateChanged = [this](ECheckBoxState NewState) 
	{ 
		IsCheckedStatus = (NewState == ECheckBoxState::Checked);
		
		
		UWorld* PIE = GEditor->PlayWorld;
		//UE_LOG(LogTemp, Warning, TEXT("PlayWorld: %p"), PIE);
		if (PIE)
		{
			for (TActorIterator<ABoidSpawner> AIt(PIE); AIt; ++AIt)
			{
				ABoidSpawner* Spawner = *AIt;
				const TArray<AActor*>& Boids = Spawner->GetBoids();
				for (int Index = 0; Index < Boids.Num(); ++Index)
				{
					AActor* EachBoid = Boids[Index];
					UActorComponent* EachComp = EachBoid->GetComponentByClass(UBoidDrawComponent::StaticClass());

					UBoidDrawComponent* EachDrawComp = Cast<UBoidDrawComponent>(EachComp);

					EachDrawComp->SetHiddenInGame(!IsCheckedStatus);
				}
			}
		}
		
	};
	*/
	

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs Args2;
	Args2.bHideSelectionTip = true;

	TSharedPtr<IDetailsView> PropertyWidget = PropertyModule.CreateDetailView(Args2);
	PropertyWidget->SetObject(Input);
	PropertyWidget->OnFinishedChangingProperties().AddLambda([this](const FPropertyChangedEvent& Event) {
		UE_LOG(LogTemp, Warning, TEXT("FATTO"));
		const FName PropName = Event.GetPropertyName();
		if (PropName == FName("bEnableVisualDebug")) 
		{
			
		}
		else if (PropName == FName("InstancesToDebug"))
		{

		}

		ApplyDebugConfig(Input->bEnableVisualDebug, Input->InstancesToDebug);
	});

	/*
	return 
		SNew(SDockTab)
			.Icon(FEditorStyle::GetBrush("Launcher.TabIcon"))
			.TabRole(ETabRole::NomadTab)
			.Label(LOCTEXT("TabTitle", TAB_TITLE))
			[
				SNew(SCheckBox)
				.ToolTipText(FText::FromString(TEXT("Show Selected")))
				.IsChecked_Lambda(IsChecked)
				.OnCheckStateChanged_Lambda(CheckedStateChanged)
			];

	*/
	return
		SNew(SDockTab)
		.Icon(FEditorStyle::GetBrush("Launcher.TabIcon"))
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("TabTitle", TAB_TITLE))
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				PropertyWidget.ToSharedRef()
			]
		];
}











#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBoidEditorModule, BoidEditor)