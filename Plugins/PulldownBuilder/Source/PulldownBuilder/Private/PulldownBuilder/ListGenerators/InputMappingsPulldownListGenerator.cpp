// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/InputMappingsPulldownListGenerator.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "UObject/UObjectThreadContext.h"
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

#if UE_4_25_OR_LATER
bool UInputMappingsPulldownListGenerator::CanEditChange(const FProperty* InProperty) const
#else
bool UInputMappingsPulldownListGenerator::CanEditChange(const UProperty* InProperty) const
#endif
{
	bool bCanEditChange = true;
#if UE_5_07_OR_LATER
#if UE_4_25_OR_LATER
	if (InProperty != nullptr)
#else
	if (IsValid(InProperty))
#endif
	{
		if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UInputMappingsPulldownListGenerator, bIncludeSpeechMappings))
		{
			bCanEditChange = false;
		}
	}
#endif
	
	return (UObject::CanEditChange(InProperty) && bCanEditChange);
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
	return GetPulldownRowsFromInputSettings();
}

FPulldownRows UInputMappingsPulldownListGenerator::GetPulldownRowsFromInputSettings() const
{
	const auto* InputSettings = UInputSettings::GetInputSettings();
	if (!IsValid(InputSettings))
	{
		return FPulldownRows::Empty;
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

#if !UE_5_07_OR_LATER
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
#endif
			
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

#if !UE_5_07_OR_LATER
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
#endif
	
	return PulldownRows;
}

void UInputMappingsPulldownListGenerator::CachePreChangeDisplayTexts()
{
	const FPulldownRows& PulldownRows = GetPulldownRowsFromInputSettings();
	
	PreChangeSelectedValues.Reset(PulldownRows.Num());
	
	for (const auto& PulldownRow : PulldownRows)
	{
		if (!PulldownRow.IsValid())
		{
			continue;
		}

		PreChangeSelectedValues.Add(*PulldownRow->SelectedValue);
	}
}

void UInputMappingsPulldownListGenerator::HandleOnActionAxisMappingsChanged()
{
	// Blueprint functions are not available during routing post load.
	if (!FUObjectThreadContext::Get().IsRoutingPostLoad)
	{
		OnActionAxisMappingsChanged();
	}
	
	TArray<FName> PostChangeSelectedValues;
	{
		const FPulldownRows& PulldownRows = GetPulldownRowsFromInputSettings();
		PostChangeSelectedValues.Reserve(PulldownRows.Num());
		for (const auto& PulldownRow : PulldownRows)
		{
			if (!PulldownRow.IsValid())
			{
				continue;
			}

			PostChangeSelectedValues.Add(*PulldownRow->SelectedValue);
		}
	}

	if (NotifyPulldownRowChanged(PreChangeSelectedValues, PostChangeSelectedValues))
	{
		CachePreChangeDisplayTexts();
	}

	VerifyDefaultValue();
}

TArray<FName> UInputMappingsPulldownListGenerator::GetDefaultValueOptions_Implementation() const
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
