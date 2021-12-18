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



#define LOCTEXT_NAMESPACE "FBoidEditorModule"

static const FName TabId = FName(TEXT("BoidWindowId"));
#define TAB_TITLE "Boid Inspector"

void FBoidEditorModule::StartupModule()
{
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TabId, FOnSpawnTab::CreateRaw(this, &FBoidEditorModule::SpawnWindow))
		.SetDisplayName(LOCTEXT("MenuItem", TAB_TITLE))
		.SetTooltipText(LOCTEXT("MenuItemTooltip", "Open My Window tab"))
		.SetGroup(WorkspaceMenu::GetMenuStructure().GetDeveloperToolsMiscCategory())
		.SetIcon(FSlateIcon(FEditorStyle::GetStyleSetName(), "Launcher.TabIcon"));

}

void FBoidEditorModule::ShutdownModule()
{
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TabId);
}


TSharedRef<SDockTab> FBoidEditorModule::SpawnWindow(const FSpawnTabArgs& Args)
{
	auto IsChecked = [this]() -> ECheckBoxState { return IsCheckedStatus ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; };
	auto CheckedStateChanged = [this](ECheckBoxState NewState) 
	{ 
		IsCheckedStatus = (NewState == ECheckBoxState::Checked); 
		
		UWorld* PIE = GEditor->PlayWorld;
		UE_LOG(LogTemp, Warning, TEXT("PlayWorld: %p"), PIE);
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
}











#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FBoidEditorModule, BoidEditor)