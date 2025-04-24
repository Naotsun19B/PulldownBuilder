// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/InputMappingsPulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "GameFramework/InputSettings.h"
#include "Editor.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(InputMappingsPulldownListGenerator)
#endif

UInputMappingsPulldownListGenerator::UInputMappingsPulldownListGenerator()
	: bIncludeActionMappings(true)
	, bIncludeAxisMappings(true)
	, bIncludeSpeechMappings(false)
{
}

void UInputMappingsPulldownListGenerator::PostInitProperties()
{
	Super::PostInitProperties();

	FEditorDelegates::OnActionAxisMappingsChanged.AddUObject(this, &UInputMappingsPulldownListGenerator::HandleOnActionAxisMappingsChanged);
	
	CachePreChangeDisplayTexts();
}

void UInputMappingsPulldownListGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if ((PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UInputMappingsPulldownListGenerator, bIncludeActionMappings)) ||
		(PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UInputMappingsPulldownListGenerator, bIncludeAxisMappings)) ||
		(PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UInputMappingsPulldownListGenerator, bIncludeSpeechMappings)))
	{
		VerifyDefaultValue();
		NotifyPulldownContentsSourceChanged();
	}
}

void UInputMappingsPulldownListGenerator::BeginDestroy()
{
	FEditorDelegates::OnActionAxisMappingsChanged.RemoveAll(this);
	
	Super::BeginDestroy();
}

FPulldownRows UInputMappingsPulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	FPulldownRows PulldownRows = Super::GetPulldownRows(OuterObjects, StructInstance);

	// If the return value of the parent GetDisplayStrings is empty,
	// the list to be displayed in the pull-down menu is generated from
	// the input settings in consideration of expansion on the Blueprint side.
	if (PulldownRows.IsEmpty())
	{
		PulldownRows = GetPulldownRowsFromInputSettings();
	}

	ApplyDefaultValue(PulldownRows);
	return PulldownRows;
}

FPulldownRows UInputMappingsPulldownListGenerator::GetPulldownRowsFromInputSettings() const
{
	const auto* InputSettings = UInputSettings::GetInputSettings();
	if (!IsValid(InputSettings))
	{
		return {};
	}
	
	FPulldownRows PulldownRows;
	
	if (bIncludeActionMappings)
	{
		TArray<FName> ActionNames;
		InputSettings->GetActionNames(ActionNames);
		for (const auto& ActionName : ActionNames)
		{
			if (ActionName == NAME_None)
			{
				continue;
			}

			{
				const TArray<FInputActionSpeechMapping>& SpeechMappings = InputSettings->GetSpeechMappings();
				if (SpeechMappings.ContainsByPredicate(
					[&](const FInputActionSpeechMapping& SpeechMapping) -> bool
					{
						return (SpeechMapping.GetActionName() == ActionName);
					}
				))
				{
					continue;
				}
			}
			
			TArray<FString> InputTexts;
			{
				TArray<FInputActionKeyMapping> ActionMappings;
				InputSettings->GetActionMappingByName(ActionName, ActionMappings);
				InputTexts.Reserve(ActionMappings.Num());
				for (const auto& ActionMapping : ActionMappings)
				{
					const FText InputText = FInputChord(
						ActionMapping.Key,
						ActionMapping.bShift,
						ActionMapping.bCtrl,
						ActionMapping.bAlt,
						ActionMapping.bCmd
					).GetInputText();
					InputTexts.Add(InputText.ToString());
				}
			}

			PulldownRows.Add(
				FPulldownRow(
					ActionName.ToString(),
					FText::FromString(FString::Join(InputTexts, LINE_TERMINATOR))
				)
			);
		}
	}

	if (bIncludeAxisMappings)
	{
		TArray<FName> AxisNames;
		InputSettings->GetAxisNames(AxisNames);
		for (const auto& AxisName : AxisNames)
		{
			if (AxisName == NAME_None)
			{
				continue;
			}
			
			TArray<FString> InputTexts;
			{
				TArray<FInputAxisKeyMapping> AxisMappings;
				InputSettings->GetAxisMappingByName(AxisName, AxisMappings);
				InputTexts.Reserve(AxisMappings.Num());
				for (const auto& AxisMapping : AxisMappings)
				{
					const FString InputText = FString::Printf(
						TEXT("%s (Scale = %.2f)"),
						*AxisMapping.Key.GetDisplayName().ToString(),
						AxisMapping.Scale
					);
					InputTexts.Add(InputText);
				}
			}

			PulldownRows.Add(
				FPulldownRow(
					AxisName.ToString(),
					FText::FromString(FString::Join(InputTexts, LINE_TERMINATOR))
				)
			);
		}
	}

	if (bIncludeSpeechMappings)
	{
		const TArray<FInputActionSpeechMapping>& SpeechMappings = InputSettings->GetSpeechMappings();
		for (const auto& SpeechMapping : SpeechMappings)
		{
			if (SpeechMapping.GetActionName() == NAME_None)
			{
				continue;
			}
			
			const FString DisplayText = SpeechMapping.GetActionName().ToString();
			const FString TooltipText = FString::Printf(
				TEXT("%s (%s)"),
				*SpeechMapping.GetSpeechKeyword().ToString(),
				*SpeechMapping.GetKeyName().ToString()
			);

			PulldownRows.Add(FPulldownRow(DisplayText, FText::FromString(TooltipText)));
		}
	}
	
	return PulldownRows;
}

void UInputMappingsPulldownListGenerator::CachePreChangeDisplayTexts()
{
	const FPulldownRows& PulldownRows = GetPulldownRowsFromInputSettings();
	
	PreChangeRowNames.Reset(PulldownRows.Num());
	
	for (const auto& PulldownRow : PulldownRows)
	{
		if (!PulldownRow.IsValid())
		{
			continue;
		}

		PreChangeRowNames.Add(*PulldownRow->SelectedValue);
	}
}

void UInputMappingsPulldownListGenerator::HandleOnActionAxisMappingsChanged()
{
	OnActionAxisMappingsChanged();
	
	TArray<FName> PostChangeRowNames;
	{
		const FPulldownRows& PulldownRows = GetPulldownRowsFromInputSettings();
		PostChangeRowNames.Reserve(PulldownRows.Num());
		for (const auto& PulldownRow : PulldownRows)
		{
			if (!PulldownRow.IsValid())
			{
				continue;
			}

			PostChangeRowNames.Add(*PulldownRow->SelectedValue);
		}
	}

	if (NotifyPulldownRowChanged(PreChangeRowNames, PostChangeRowNames))
	{
		CachePreChangeDisplayTexts();
	}

	VerifyDefaultValue();
}

TArray<FName> UInputMappingsPulldownListGenerator::GetDefaultValueOptions() const
{
	const FPulldownRows PulldownRows = GetPulldownRowsFromInputSettings();
	
	TArray<FName> MappingNames;
	MappingNames.Reserve(PulldownRows.Num());
	for (const auto& PulldownRow : PulldownRows)
	{
		check(PulldownRow.IsValid());
		MappingNames.Add(*PulldownRow->SelectedValue);
	}

	return MappingNames;
}
