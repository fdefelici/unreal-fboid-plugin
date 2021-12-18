// (FDF) Fill out your copyright notice in the Description page of Project Settings.


#include "BoidFactoryDialog.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "BoidFactoryInput.h"


#define LOCTEXT_NAMESPACE "SBoidFactoryDialog"

bool SBoidFactoryDialog::Open(UBoidFactoryInput* Input)
{
	//PropertyWidget->SetObject(Input);

	TSharedRef<SWindow> PickerWindow = SNew(SWindow)
		.Title(LOCTEXT("Title", "Boidize an Existing Blueprint Actor"))
		//SizingRule(ESizingRule::UserSized)
		.SizingRule(ESizingRule::Autosized)
		//.ClientSize(FVector2D(600.f, 600.f))
		.SupportsMaximize(false)
		.SupportsMinimize(false);

	TSharedRef<SBoidFactoryDialog> DialogForm = SNew(SBoidFactoryDialog)
		.ParentWindow(PickerWindow)
		.InputObject(Input);

	//DialogForm->PropertyWidget->SetObject(Input);

	PickerWindow->SetContent(DialogForm);

	GEditor->EditorAddModalWindow(PickerWindow);

	return DialogForm->bPressedOk;
}

void SBoidFactoryDialog::Construct(const FArguments& InArgs)
{
	WeakWindowPtr = InArgs._ParentWindow;
	InputObject = InArgs._InputObject;

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	FDetailsViewArgs Args;
	Args.bHideSelectionTip = true;

	PropertyWidget = PropertyModule.CreateDetailView(Args);
	PropertyWidget->SetObject(InputObject);

	ChildSlot[

		//TSharedRef<SVerticalBox> Box =
		SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			[
				PropertyWidget.ToSharedRef()
			]
		+ SVerticalBox::Slot()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(8)
			[
				SNew(SUniformGridPanel)
				.SlotPadding(FEditorStyle::GetMargin("StandardDialog.SlotPadding"))
			+ SUniformGridPanel::Slot(0, 0)
			[
				SNew(SButton)
				.Text(LOCTEXT("OkButton", "Ok"))
			.HAlign(HAlign_Center)
			.Visibility(this, &SBoidFactoryDialog::GetConfirmButtonVisibility)
			.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
			.OnClicked(this, &SBoidFactoryDialog::OnDialogConfirmed)
			.ButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
			.TextStyle(FEditorStyle::Get(), "FlatButton.DefaultTextStyle")
			]
		+ SUniformGridPanel::Slot(1, 0)
			[
				SNew(SButton)
				.Text(LOCTEXT("CancelButton", "Cancel"))
			.HAlign(HAlign_Center)
			.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
			.OnClicked(this, &SBoidFactoryDialog::OnDialogCanceled)
			.ButtonStyle(FEditorStyle::Get(), "FlatButton.Default")
			.TextStyle(FEditorStyle::Get(), "FlatButton.DefaultTextStyle")
			]
			]
	];
}


FReply SBoidFactoryDialog::OnDialogConfirmed() 
{
	bPressedOk = true;
	if (WeakWindowPtr.IsValid())
	{
		WeakWindowPtr.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}
FReply SBoidFactoryDialog::OnDialogCanceled()
{
	bPressedOk = false;
	if (WeakWindowPtr.IsValid())
	{
		WeakWindowPtr.Pin()->RequestDestroyWindow();
	}
	return FReply::Handled();
}

EVisibility SBoidFactoryDialog::GetConfirmButtonVisibility() const
{
	EVisibility ButtonVisibility = EVisibility::Hidden;
	if (InputObject->ActorBlueprint != nullptr)
	{
		ButtonVisibility = EVisibility::Visible;
	}
	return ButtonVisibility;
}

#undef LOCTEXT_NAMESPACE