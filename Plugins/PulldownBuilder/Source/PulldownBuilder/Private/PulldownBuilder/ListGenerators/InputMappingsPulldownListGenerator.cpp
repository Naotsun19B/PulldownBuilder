﻿// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/ListGenerators/InputMappingsPulldownListGenerator.h"
#include "GameFramework/InputSettings.h"

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

void UInputMappingsPulldownListGenerator::BeginDestroy()
{
	FEditorDelegates::OnActionAxisMappingsChanged.RemoveAll(this);
	
	Super::BeginDestroy();
}

TArray<TSharedPtr<FPulldownRow>> UInputMappingsPulldownListGenerator::GetPulldownRows() const
{
	const auto* InputSettings = UInputSettings::GetInputSettings();
	if (!IsValid(InputSettings))
	{
		return {};
	}
	
	TArray<TSharedPtr<FPulldownRow>> PulldownRows;
	
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
				MakeShared<FPulldownRow>(
					ActionName.ToString(),
					FString::Join(InputTexts, LINE_TERMINATOR)
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
				MakeShared<FPulldownRow>(
					AxisName.ToString(),
					FString::Join(InputTexts, LINE_TERMINATOR)
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

			PulldownRows.Add(MakeShared<FPulldownRow>(DisplayText, TooltipText));
		}
	}
	
	return PulldownRows;
}

void UInputMappingsPulldownListGenerator::CachePreChangeDisplayTexts()
{
	const TArray<TSharedPtr<FPulldownRow>>& PulldownRows = GetPulldownRows();
	
	PreChangeDisplayTexts.Reset(PulldownRows.Num());
	
	for (const auto& PulldownRow : PulldownRows)
	{
		if (!PulldownRow.IsValid())
		{
			continue;
		}

		PreChangeDisplayTexts.Add(*PulldownRow->DisplayText.ToString());
	}
}

void UInputMappingsPulldownListGenerator::HandleOnActionAxisMappingsChanged()
{
	TArray<FName> PostChangeDisplayTexts;
	{
		const TArray<TSharedPtr<FPulldownRow>>& PulldownRows = GetPulldownRows();
		
		PostChangeDisplayTexts.Reset(PulldownRows.Num());

		for (const auto& PulldownRow : PulldownRows)
		{
			if (!PulldownRow.IsValid())
			{
				continue;
			}

			PostChangeDisplayTexts.Add(*PulldownRow->DisplayText.ToString());
		}
	}

	if (PreChangeDisplayTexts.Num() != PostChangeDisplayTexts.Num())
	{
		return;
	}

	for (int32 Index = 0; Index < PostChangeDisplayTexts.Num(); Index++)
	{
		const FName PreChangeName = PreChangeDisplayTexts[Index];
		const FName PostChangeName = PostChangeDisplayTexts[Index];
		if (PreChangeName != PostChangeName)
		{
			UpdateDisplayStrings(PreChangeName, PostChangeName);
		}
	}
	
	CachePreChangeDisplayTexts();
}
