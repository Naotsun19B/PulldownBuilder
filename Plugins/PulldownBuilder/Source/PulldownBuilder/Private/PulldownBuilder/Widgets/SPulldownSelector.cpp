// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Widgets/SPulldownSelector.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "PulldownBuilder/CommandActions/PulldownBuilderCommands.h"
#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if UE_5_01_OR_LATER
#include "Styling/AppStyle.h"
#else
#include "EditorStyleSet.h"
#endif
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Widgets/Input/SSearchBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/SToolTip.h"

namespace PulldownBuilder
{
	void SPulldownSelector::Construct(const FArguments& InArgs)
	{
		ListItemsSource = InArgs._ListItemsSource;
		OnSelectionChanged = InArgs._OnSelectionChanged;

		CommandBindings = MakeShared<FUICommandList>();
		CommandBindings->MapAction(
			FPulldownBuilderCommands::Get().DisableDisplayText,
			FExecuteAction::CreateRaw(this, &SPulldownSelector::ToggleDisableDisplayText),
			FCanExecuteAction(),
			FGetActionCheckState::CreateStatic(&SPulldownSelector::GetDisableDisplayTextCheckState)
		);
		CommandBindings->MapAction(
			FPulldownBuilderCommands::Get().DisableTextColoring,
			FExecuteAction::CreateRaw(this, &SPulldownSelector::ToggleDisableTextColoring),
			FCanExecuteAction(),
			FGetActionCheckState::CreateStatic(&SPulldownSelector::GetDisableTextColoringCheckState)
		);
		
		FMenuBuilder ViewOptions(false, CommandBindings);
		ViewOptions.AddMenuEntry(FPulldownBuilderCommands::Get().DisableDisplayText);
		ViewOptions.AddMenuEntry(FPulldownBuilderCommands::Get().DisableTextColoring);
		
		static constexpr float MarginBetweenElements = 3.f;
		
		ChildSlot
		[
			SNew(SBox)
			.HeightOverride(InArgs._HeightOverride)
			.WidthOverride(InArgs._WidthOverride)
			.Padding(MarginBetweenElements)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.FillWidth(1.f)
					[
						SAssignNew(SearchBox, SSearchBox)
						.HintText(NSLOCTEXT("PulldownSelector", "SearchHintText", "Search"))
						.OnTextChanged(this, &SPulldownSelector::HandleOnTextChanged)
						.DelayChangeNotificationsWhileTyping(true)
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						SNew(SComboButton)
						.HasDownArrow(false)
						.ContentPadding(0.f)
						.ForegroundColor(FSlateColor::UseForeground())
						.ButtonStyle(
#if UE_5_01_OR_LATER
							FAppStyle::Get(),
#else
							FEditorStyle::Get(),
#endif
							TEXT("SimpleButton")
						)
						.AddMetaData<FTagMetaData>(TEXT("ViewOptions"))
						.MenuContent()
						[
							ViewOptions.MakeWidget()
						]
						.ButtonContent()
						[
							SNew(SImage)
							.Image(
#if UE_5_01_OR_LATER
								FAppStyle
#else
								FEditorStyle
#endif
								::GetBrush(TEXT("DetailsView.ViewOptions"))
							)
							.ColorAndOpacity(FSlateColor::UseForeground())
						]
					]
				]
				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SSpacer)
					.Size(FVector2D(MarginBetweenElements))
				]
				+ SVerticalBox::Slot()
				.FillHeight(1.0f)
				[
					SAssignNew(ListView, SListView<TSharedPtr<FPulldownRow>>)
					.SelectionMode(ESelectionMode::Single)
					.ListItemsSource(&ListItems)
					.OnGenerateRow(this, &SPulldownSelector::HandleOnGenerateRow)
					.OnMouseButtonClick(this, &SPulldownSelector::HandleOnMouseButtonClick, InArgs._bIsSelectWhenDoubleClick)
					.OnMouseButtonDoubleClick(this, &SPulldownSelector::HandleOnMouseButtonDoubleClick, InArgs._bIsSelectWhenDoubleClick)
				]
			]
		];
	}

	FReply SPulldownSelector::OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
	{
		if (CommandBindings.IsValid())
		{
			if (CommandBindings->ProcessCommandBindings(InKeyEvent))
			{
				return FReply::Handled();
			}
		}
		
		if (InKeyEvent.GetKey() == EKeys::Enter &&
			!InKeyEvent.IsAltDown() &&
			!InKeyEvent.IsCommandDown() &&
			!InKeyEvent.IsControlDown() &&
			!InKeyEvent.IsShiftDown())
		{
			const TSharedPtr<FPulldownRow> SelectedItem = GetSelectedItem();
			if (OnSelectionChanged.IsBound() && SelectedItem.IsValid())
			{
				OnSelectionChanged.Execute(SelectedItem, ESelectInfo::OnKeyPress);
				return FReply::Handled();
			}
		}

		return SCompoundWidget::OnKeyDown(MyGeometry, InKeyEvent);
	}

	bool SPulldownSelector::SupportsKeyboardFocus() const
	{
		return true;
	}

	TSharedPtr<FPulldownRow> SPulldownSelector::GetSelectedItem() const
	{
		if (ListView.IsValid())
		{
			const TArray<TSharedPtr<FPulldownRow>> SelectedItems = ListView->GetSelectedItems();
			if (SelectedItems.IsValidIndex(0))
			{
				return SelectedItems[0];
			}
		}

		return nullptr;
	}

	void SPulldownSelector::SetSelectedItem(const TSharedPtr<FPulldownRow>& NewItem)
	{
		if (ListView.IsValid() && NewItem.IsValid())
		{
			ListView->SetSelection(NewItem);
		}
	}

	void SPulldownSelector::RefreshList()
	{
		RebuildListItems();
		if (ListView.IsValid())
		{
			ListView->RebuildList();
		}
	}

	TSharedPtr<SWidget> SPulldownSelector::GetSearchBox() const
	{
		return SearchBox;
	}

	void SPulldownSelector::RebuildListItems()
	{
		if (ListItemsSource != nullptr)
		{
			ListItems = *ListItemsSource;
		}
	
		if (!FilterString.IsEmpty())
		{
			ListItems.RemoveAll([this](const TSharedPtr<FPulldownRow>& ListItem) -> bool
			{
				return !FilterRow(ListItem);
			});
		}
	}

	void SPulldownSelector::HandleOnTextChanged(const FText& InFilterString)
	{
		FilterString = InFilterString.ToString();
		RefreshList();
	}

	TSharedRef<ITableRow> SPulldownSelector::HandleOnGenerateRow(TSharedPtr<FPulldownRow> InItem, const TSharedRef<STableViewBase>& OwnerTable)
	{
		check(InItem.IsValid());

		const auto& Settings = GetSettings<UPulldownBuilderAppearanceSettings>();
		const TSharedRef<STextBlock> Row = SNew(STextBlock)
			.Text(
				Settings.bIsDisplayTextDisabled ?
				FText::FromString(InItem->SelectedValue) :
				InItem->GetDisplayText()
			)
			.HighlightText(FText::FromString(FilterString))
			.ColorAndOpacity(InItem->GetDisplayTextColor());

		if (!InItem->TooltipText.IsEmpty())
		{
			Row->SetToolTip(
				SNew(SToolTip)
				.Content()
				[
					SNew(STextBlock)
					.Text(InItem->TooltipText)
					.HighlightText(FText::FromString(FilterString))
				]
			);
		}

		return SNew(STableRow<TSharedPtr<FString>>, OwnerTable)
		[
			Row
		];
	}

	void SPulldownSelector::HandleOnMouseButtonClick(TSharedPtr<FPulldownRow> SelectedItem, const bool bIsSelectWhenDoubleClick)
	{
		if (!bIsSelectWhenDoubleClick)
		{
			OnRowItemClicked(SelectedItem);
		}
	}

	void SPulldownSelector::HandleOnMouseButtonDoubleClick(TSharedPtr<FPulldownRow> SelectedItem, const bool bIsSelectWhenDoubleClick)
	{
		if (bIsSelectWhenDoubleClick)
		{
			OnRowItemClicked(SelectedItem);
		}
	}

	void SPulldownSelector::OnRowItemClicked(TSharedPtr<FPulldownRow> SelectedItem)
	{
		if (OnSelectionChanged.IsBound())
		{
			OnSelectionChanged.Execute(SelectedItem, ESelectInfo::OnMouseClick);
		}
	}

	bool SPulldownSelector::FilterRow(TSharedPtr<FPulldownRow> InItem) const
	{
		check(InItem.IsValid());

		const auto& Settings = GetSettings<UPulldownBuilderAppearanceSettings>();
		const FString DisplayString = (
			Settings.bIsDisplayTextDisabled ?
			InItem->SelectedValue :
			InItem->GetDisplayText().ToString()
		);
		const FString TooltipString = InItem->TooltipText.ToString();
		return (
			DisplayString.Contains(FilterString) ||
			TooltipString.Contains(FilterString)
		);
	}

	void SPulldownSelector::ToggleDisableDisplayText()
	{
		auto& Settings = GetMutableSettings<UPulldownBuilderAppearanceSettings>(PULLDOWN_BUILDER_PASS_KEY);
		Settings.bIsDisplayTextDisabled = !Settings.bIsDisplayTextDisabled;

		RefreshList();
	}

	ECheckBoxState SPulldownSelector::GetDisableDisplayTextCheckState()
	{
		const auto& Settings = GetSettings<UPulldownBuilderAppearanceSettings>();
		return (Settings.bIsDisplayTextDisabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}

	void SPulldownSelector::ToggleDisableTextColoring()
	{
		auto& Settings = GetMutableSettings<UPulldownBuilderAppearanceSettings>(PULLDOWN_BUILDER_PASS_KEY);
		Settings.bIsTextColoringDisabled = !Settings.bIsTextColoringDisabled;

		RefreshList();
	}

	ECheckBoxState SPulldownSelector::GetDisableTextColoringCheckState()
	{
		const auto& Settings = GetSettings<UPulldownBuilderAppearanceSettings>();
		return (Settings.bIsTextColoringDisabled ? ECheckBoxState::Checked : ECheckBoxState::Unchecked);
	}
}
