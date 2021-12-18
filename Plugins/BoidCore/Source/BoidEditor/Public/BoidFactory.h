// (FDF) Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Factories/Factory.h"
#include "BoidFactory.generated.h"



/**
 * 
 */
UCLASS()
class BOIDEDITOR_API UBoidFactory : public UFactory
{
	GENERATED_BODY()

public:
	UBoidFactory();

	bool ShouldShowInNewMenu() const override; //default: value of CanCreateNew property.
	uint32 GetMenuCategories() const override; // default: "Miscellaneous"
	FText GetDisplayName() const override; //default: SupportedClass name
	FText GetToolTip() const override; //default: SupportedClass class documentantion
	FString GetToolTipDocumentationPage() const override { return FString(); }; //default: SupportedClass class page
	FString GetToolTipDocumentationExcerpt() const override { return FString(); }; ///default: SupportedClass class excerpt


	bool ConfigureProperties() override;
	UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

private:
	class UBoidFactoryInput* BoidInput;
	TSharedPtr<class IDetailsView> PropertyWidget;

	TSubclassOf<class AActor> SelectedClass;

	TWeakPtr<SWindow> WeakWindow;
	//TSharedPtr<UBoidFactory> MyRef;
};
