// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/CustomGraphPins/SPulldownStructGraphPin.h"

namespace PulldownBuilder
{
	class SPulldownSelectorComboButton;

	/**
	 * A custom graph pin that applies to structures that inherit from FNativeLessPulldownStruct.
	 */
	class PULLDOWNBUILDER_API SNativeLessPulldownStructGraphPin : public SPulldownStructGraphPin
	{
	public:
		SLATE_BEGIN_ARGS(SNativeLessPulldownStructGraphPin) {}
		SLATE_END_ARGS()

		void Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj);

		// SGraphPin interface.
		virtual TSharedRef<SWidget> GetDefaultValueWidget() override;
		// End of SGraphPin interface.

	private:
		// SPulldownStructGraphPin interface.
		virtual void RefreshPulldownWidget() override;
		virtual TArray<TSharedPtr<FPulldownRow>> GenerateSelectableValues() override;
		// End of SPulldownStructGraphPin interface.
		
		// Search for the same name as the specified name from the PulldownContentsNames.
		// If not found, returns nullptr.
		TSharedPtr<FPulldownRow> FindPulldownContentsNameByName(const FName& InName) const;
		
		// Called when the value of the PulldownSourceWidget changes.
		void OnPulldownSourceChanged(TSharedPtr<FPulldownRow> SelectedItem, ESelectInfo::Type SelectInfo);
		
	private:
		// A list of values that can be set in FNativeLessPulldownStruct::PulldownSource.
		TArray<TSharedPtr<FPulldownRow>> PulldownContentsNames;

		// A widget that displays a pull-down menu based on the PulldownContentsNames.
		TSharedPtr<SPulldownSelectorComboButton> PulldownSourceWidget;
	};
}
