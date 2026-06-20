// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_MakeLiteral_NativeLessPulldownStruct.h"
#if WITH_EDITOR
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#endif
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "EdGraphSchema_K2.h"
#include "K2Node_MakeStruct.h"

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

bool UK2Node_MakeLiteral_NativeLessPulldownStruct::ApplyDefaultsToMakeStructNode(
	FKismetCompilerContext& CompilerContext,
	const UEdGraphSchema_K2* K2Schema,
	UK2Node_MakeStruct* MakeStructNode
) const
{
	if (!Super::ApplyDefaultsToMakeStructNode(CompilerContext, K2Schema, MakeStructNode))
	{
		return false;
	}

	UEdGraphPin* PulldownSourcePin = MakeStructNode->FindPin(
		GET_MEMBER_NAME_CHECKED(FNativeLessPulldownStruct, PulldownSource),
		EGPD_Input
	);
	if (PulldownSourcePin == nullptr)
	{
		return false;
	}

	K2Schema->TrySetDefaultValue(*PulldownSourcePin, PulldownSource.ToString());
	return true;
}

bool UK2Node_MakeLiteral_NativeLessPulldownStruct::IsTargetStruct(const UScriptStruct* Struct) const
{
#if WITH_EDITOR
	return PulldownBuilder::FPulldownBuilderUtils::IsNativeLessPulldownStruct(Struct);
#else
	return false;
#endif
}

const FName& UK2Node_MakeLiteral_NativeLessPulldownStruct::GetPulldownSource() const
{
	return PulldownSource;
}

void UK2Node_MakeLiteral_NativeLessPulldownStruct::SetPulldownSource(const FName& InPulldownSource)
{
	PulldownSource = InPulldownSource;
}

#undef LOCTEXT_NAMESPACE
