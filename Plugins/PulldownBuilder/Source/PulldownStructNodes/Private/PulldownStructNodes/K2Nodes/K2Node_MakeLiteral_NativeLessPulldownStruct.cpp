// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_MakeLiteral_NativeLessPulldownStruct.h"
#if WITH_EDITOR
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#endif
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "K2Node_MakeStruct.h"
#include "EdGraphSchema_K2.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_MakeLiteral_NativeLessPulldownStruct)
#endif

#define LOCTEXT_NAMESPACE "K2Node_MakeLiteral_NativeLessPulldownStruct"

UK2Node_MakeLiteral_NativeLessPulldownStruct::UK2Node_MakeLiteral_NativeLessPulldownStruct()
	: PulldownSource(NAME_None)
{
	PulldownStruct = FNativeLessPulldownStruct::StaticStruct();
}

FText UK2Node_MakeLiteral_NativeLessPulldownStruct::GetTooltipText() const
{
	return LOCTEXT(
		"Tooltip",
		"Construct a literal value of FNativeLessPulldownStruct with the chosen PulldownSource and SelectedValue."
	);
}

bool UK2Node_MakeLiteral_NativeLessPulldownStruct::ApplyInputPinDefaultsToMakeStructNode(
	FKismetCompilerContext& CompilerContext,
	const UEdGraphSchema_K2* K2Schema,
	UK2Node_MakeStruct* MakeStructNode,
	const UEdGraphPin* InputPin
) const
{
#if WITH_EDITOR
	if (!Super::ApplyInputPinDefaultsToMakeStructNode(CompilerContext, K2Schema, MakeStructNode, InputPin))
	{
		return false;
	}

	UEdGraphPin* PulldownSourceSubPin = MakeStructNode->FindPin(
		GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource),
		EGPD_Input
	);
	if (PulldownSourceSubPin == nullptr)
	{
		return false;
	}

	const TSharedPtr<FString> PulldownSourceString = PulldownBuilder::FPulldownBuilderUtils::StructStringToMemberValue(
		InputPin->DefaultValue,
		GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource)
	);
	if (PulldownSourceString.IsValid())
	{
		K2Schema->TrySetDefaultValue(*PulldownSourceSubPin, *PulldownSourceString);
	}

	return true;
#else
	return false;
#endif
}

bool UK2Node_MakeLiteral_NativeLessPulldownStruct::IsTargetStruct(const UScriptStruct* Struct) const
{
#if WITH_EDITOR
	return PulldownBuilder::FPulldownBuilderUtils::IsNativeLessPulldownStruct(Struct);
#else
	return false;
#endif
}

bool UK2Node_MakeLiteral_NativeLessPulldownStruct::HasLegacyDefaultValues() const
{
	return (Super::HasLegacyDefaultValues() || !PulldownSource.IsNone());
}

void UK2Node_MakeLiteral_NativeLessPulldownStruct::ApplyLegacyDefaultValuesToString(FString& InOutDefaultValueString) const
{
#if WITH_EDITOR
	Super::ApplyLegacyDefaultValuesToString(InOutDefaultValueString);

	if (PulldownSource.IsNone())
	{
		return;
	}

	const TSharedPtr<FString> Result = PulldownBuilder::FPulldownBuilderUtils::MemberValueToStructString(
		InOutDefaultValueString,
		GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource),
		PulldownSource.ToString()
	);
	if (Result.IsValid())
	{
		InOutDefaultValueString = *Result;
	}
#endif
}

void UK2Node_MakeLiteral_NativeLessPulldownStruct::ClearLegacyDefaultValues()
{
	Super::ClearLegacyDefaultValues();

	PulldownSource = NAME_None;
}

#undef LOCTEXT_NAMESPACE
