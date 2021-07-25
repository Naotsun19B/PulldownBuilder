// Copyright 2021 Naotsun. All Rights Reserved.

#include "Widgets/SSearchableTextComboBox.h"
#include "DetailLayoutBuilder.h"

void SSearchableTextComboBox::Construct(const FArguments& InArgs)
{
	// If want to inherit the SSearchableComboBox and create custom combo box with search function,
	// bind the function that generates the row widget to the OnGenerateWidget and
	// specify the widget to be displayed in the Content closed state.
	SSearchableComboBox::Construct(
		SSearchableComboBox::FArguments()
		.OptionsSource(InArgs._OptionsSource)
		.OnSelectionChanged(InArgs._OnSelectionChanged)
		.OnComboBoxOpening(InArgs._OnComboBoxOpening)
		.InitiallySelectedItem(InArgs._InitiallySelectedItem)
		.OnGenerateWidget_Raw(this, &SSearchableTextComboBox::OnGenerateWidget)
		.Content()
		[
			SNew(STextBlock)
			// The function to bind to Text must be const function,
			// but SSearchableComboBox::GetSelectedItem is not const function,
			// so use the lambda function.
			.Text_Lambda([this]() -> FText
			{
				const TSharedPtr<FString>& SelectedItem = GetSelectedItem();
				if (SelectedItem.IsValid())
				{
					return FText::FromString(*SelectedItem);
				}
				return FText::FromName(FName(NAME_None));
			})
			.Font(IDetailLayoutBuilder::GetDetailFont())
		]
#if DISABLE_SEARCH_FIELD
		// At version 4.26.2, there is a problem that the pull-down menu is closed
		// when using the search field, so hide the search field in version 4.26.
		.SearchVisibility(EVisibility::Collapsed)
#endif
	);
}

TSharedRef<SWidget> SSearchableTextComboBox::OnGenerateWidget(TSharedPtr<FString> InDisplayString)
{
	check(InDisplayString);
	
	return
		SNew(STextBlock)
		.Text(FText::FromString(*InDisplayString))
		.Font(IDetailLayoutBuilder::GetDetailFont());
}
