// (FDF) Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Input/Reply.h"

/**
 * 
 */
class SBoidFactoryDialog : public SCompoundWidget
{
public:
	 static bool Open(class UBoidFactoryInput* Input);

	SLATE_BEGIN_ARGS(SBoidFactoryDialog) {}
	SLATE_ARGUMENT(TSharedPtr<class SWindow>, ParentWindow)
	SLATE_ARGUMENT(class UBoidFactoryInput*, InputObject)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FReply OnDialogConfirmed();
	FReply OnDialogCanceled();
	EVisibility GetConfirmButtonVisibility() const;


	TSharedPtr<class IDetailsView> PropertyWidget;
	TWeakPtr<class SWindow> WeakWindowPtr;
	class UBoidFactoryInput* InputObject;
	bool bPressedOk;
};
