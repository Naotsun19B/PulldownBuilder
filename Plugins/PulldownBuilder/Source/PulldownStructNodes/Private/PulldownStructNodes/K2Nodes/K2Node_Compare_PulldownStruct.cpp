// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_Compare_PulldownStruct.h"
#include "PulldownStructNodes/Utilities/PulldownStructNodeUtils.h"
#if WITH_EDITOR
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#endif
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Kismet/KismetMathLibrary.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFieldNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_Compare_PulldownStruct)
#endif

#define LOCTEXT_NAMESPACE "K2Node_Compare_PulldownStruct"

const FName UK2Node_Compare_PulldownStruct::LhsPinName				= TEXT("Lhs");
const FName UK2Node_Compare_PulldownStruct::RhsPinName				= TEXT("Rhs");
const FName UK2Node_Compare_PulldownStruct::CompareNodeLhsPinName	= TEXT("A");
const FName UK2Node_Compare_PulldownStruct::CompareNodeRhsPinName	= TEXT("B");

UK2Node_Compare_PulldownStruct::UK2Node_Compare_PulldownStruct()
	: PulldownStruct(nullptr)
	, bStrictComparison(true)
{
}

FText UK2Node_Compare_PulldownStruct::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (CachedNodeTitle.IsOutOfDate(this))
	{
		if (IsValid(PulldownStruct))
		{
			CachedNodeTitle.SetCachedText(
				FText::Format(
					LOCTEXT("TitleFormat", "{0} ({1})"),
					GetCompareMethodName(),
					PulldownStruct->GetDisplayNameText()
				),
				this
			);
		}
		else
		{
			CachedNodeTitle.SetCachedText(
				FText::Format(
					LOCTEXT("Title", "{0} (Unknown Pulldown Struct)"),
					GetCompareMethodName()
				),
				this
			);
		}
	}
		
	return CachedNodeTitle;
}

FText UK2Node_Compare_PulldownStruct::GetTooltipText() const
{
	if (CachedNodeTooltip.IsOutOfDate(this))
	{
		CachedNodeTitle.SetCachedText(
			FText::Format(
				LOCTEXT("TooltipFormat", "Compare the values of SelectedValue in {0} and return if they are {1}."),
				FText::FromString(GetNameSafe(PulldownStruct)),
				FText::FromString(FName::NameToDisplayString(GetCompareMethodName().ToString(), false).ToLower())
			),
			this
		);
	}
	
	return CachedNodeTooltip;
}

FText UK2Node_Compare_PulldownStruct::GetKeywords() const
{
	return GetCompareMethodOperator();
}

FSlateIcon UK2Node_Compare_PulldownStruct::GetIconAndTint(FLinearColor& OutColor) const
{
	const UFunction* Function = FindUField<UFunction>(UKismetMathLibrary::StaticClass(), GetFunctionName());
	check(IsValid(Function));
	return UK2Node_CallFunction::GetPaletteIconForFunction(Function, OutColor);
}

void UK2Node_Compare_PulldownStruct::AllocateDefaultPins()
{
	if (IsValid(PulldownStruct))
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, PulldownStruct, LhsPinName);
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, PulldownStruct, RhsPinName);
	}
	else
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, LhsPinName);
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, RhsPinName);
	}
	
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Boolean, UEdGraphSchema_K2::PN_ReturnValue);
}

bool UK2Node_Compare_PulldownStruct::IsNodePure() const
{
	return true;
}

bool UK2Node_Compare_PulldownStruct::ShouldDrawCompact() const
{
	return true;
}

FText UK2Node_Compare_PulldownStruct::GetCompactNodeTitle() const
{
	return GetCompareMethodOperator();
}

FText UK2Node_Compare_PulldownStruct::GetMenuCategory() const
{
	if (CachedNodeCategory.IsOutOfDate(this))
	{
		CachedNodeCategory.SetCachedText(
			FText::Format(
				LOCTEXT("MenuCategoryFormat", "{0}|Operators|Pulldown Struct|{1}"),
				FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Utilities),
				GetCompareMethodName()
			),
			this
		);
	}
	
	return CachedNodeCategory;
}

void UK2Node_Compare_PulldownStruct::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	ActionRegistrar.RegisterStructActions(
		FBlueprintActionDatabaseRegistrar::FMakeStructSpawnerDelegate::CreateUObject(
			this, &UK2Node_Compare_PulldownStruct::HandleOnMakeStructSpawner
		)
	);
}

void UK2Node_Compare_PulldownStruct::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	if (IsValid(PulldownStruct))
	{
		const UEdGraphSchema_K2* K2Schema = CompilerContext.GetSchema();
		check(IsValid(K2Schema));
		
		auto* SelectedValueCompareNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		check(IsValid(SelectedValueCompareNode));
		SelectedValueCompareNode->FunctionReference.SetExternalMember(
			GetFunctionName(),
			UKismetMathLibrary::StaticClass()
		);
		SelectedValueCompareNode->AllocateDefaultPins();

		UK2Node_CallFunction* PulldownSourceCompareNode = nullptr;
		if (ShouldStrictComparison())
		{
			PulldownSourceCompareNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
			check(IsValid(PulldownSourceCompareNode));
			PulldownSourceCompareNode->FunctionReference.SetExternalMember(
				GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, EqualEqual_NameName),
				UKismetMathLibrary::StaticClass()
			);
			PulldownSourceCompareNode->AllocateDefaultPins();
		}

		auto LinkSourcePinToCompareNodePin = [&](const FName& SourcePinName, const FName& CompareNodePinName)
		{
			UEdGraphPin* SourcePin = FindPinChecked(SourcePinName, EGPD_Input);
			UEdGraphPin* SelectedValueCompareNodePin = SelectedValueCompareNode->FindPinChecked(CompareNodePinName, EGPD_Input);
			UEdGraphPin* PulldownSourceCompareNodePin = nullptr;
			if (IsValid(PulldownSourceCompareNode))
			{
				PulldownSourceCompareNodePin = PulldownSourceCompareNode->FindPinChecked(CompareNodePinName, EGPD_Input);
			}
			
			check(
				PulldownBuilder::FPulldownStructNodeUtils::LinkPulldownStructPinToNamePin(
					this,
					CompilerContext,
					K2Schema,
					SourcePin,
					SelectedValueCompareNodePin,
					PulldownSourceCompareNodePin
				)
			);
		};

		LinkSourcePinToCompareNodePin(LhsPinName, CompareNodeLhsPinName);
		LinkSourcePinToCompareNodePin(RhsPinName, CompareNodeRhsPinName);

		UEdGraphPin* SourceReturnValuePin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);
		if (IsValid(PulldownSourceCompareNode))
		{
			auto* BoolAndNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
			check(IsValid(BoolAndNode));
			BoolAndNode->FunctionReference.SetExternalMember(
				GET_FUNCTION_NAME_CHECKED(UKismetMathLibrary, BooleanAND),
				UKismetMathLibrary::StaticClass()
			);
			BoolAndNode->AllocateDefaultPins();
			
			{
				UEdGraphPin* PulldownSourceReturnValuePin = PulldownSourceCompareNode->GetReturnValuePin();
				UEdGraphPin* BoolAndLhsPin = BoolAndNode->FindPinChecked(CompareNodeLhsPinName, EGPD_Input);
				check(K2Schema->TryCreateConnection(PulldownSourceReturnValuePin, BoolAndLhsPin));
			}
			{
				UEdGraphPin* SelectedValueReturnValuePin = SelectedValueCompareNode->GetReturnValuePin();
				UEdGraphPin* BoolAndRhsPin = BoolAndNode->FindPinChecked(CompareNodeRhsPinName, EGPD_Input);
				check(K2Schema->TryCreateConnection(SelectedValueReturnValuePin, BoolAndRhsPin));
			}
			{
				UEdGraphPin* BoolAndReturnValue = BoolAndNode->GetReturnValuePin();
				CompilerContext.MovePinLinksToIntermediate(*SourceReturnValuePin, *BoolAndReturnValue);
			}
		}
		else
		{
			UEdGraphPin* CompareNodeReturnValuePin = SelectedValueCompareNode->GetReturnValuePin();
			CompilerContext.MovePinLinksToIntermediate(*SourceReturnValuePin, *CompareNodeReturnValuePin);
		}
	}
	
	BreakAllNodeLinks();
}

void UK2Node_Compare_PulldownStruct::PreloadRequiredAssets()
{
	if (IsValid(PulldownStruct))
	{
		PreloadObject(PulldownStruct);
	}
}

bool UK2Node_Compare_PulldownStruct::ShouldShowNodeProperties() const
{
#if WITH_EDITOR
	return PulldownBuilder::FPulldownBuilderUtils::IsNativeLessPulldownStruct(PulldownStruct);
#else
	return false;
#endif
}

UScriptStruct* UK2Node_Compare_PulldownStruct::GetPulldownStruct() const
{
	return PulldownStruct;
}

void UK2Node_Compare_PulldownStruct::SetPulldownStruct(UScriptStruct* NewPulldownStruct)
{
	PulldownStruct = NewPulldownStruct;

	ReconstructNode();
}

bool UK2Node_Compare_PulldownStruct::ShouldStrictComparison() const
{
	return (ShouldShowNodeProperties() && bStrictComparison);
}

void UK2Node_Compare_PulldownStruct::SetShouldStrictComparison(const bool bNewState)
{
	bStrictComparison = bNewState;
}

UBlueprintNodeSpawner* UK2Node_Compare_PulldownStruct::HandleOnMakeStructSpawner(const UScriptStruct* Struct) const
{
#if WITH_EDITOR
	if (!PulldownBuilder::FPulldownBuilderUtils::IsPulldownStruct(Struct, false))
	{
		return nullptr;
	}
#endif
				
	UBlueprintFieldNodeSpawner* NodeSpawner = UBlueprintFieldNodeSpawner::Create(GetClass(), Struct);
	if (!IsValid(NodeSpawner))
	{
		return nullptr;
	}

	struct FNodeFieldSetter
	{
	public:
#if UE_4_25_OR_LATER
		static void SetNodeField(UEdGraphNode* NewNode, FFieldVariant Field)
#else
		static void SetNodeField(UEdGraphNode* NewNode, const UField* Field, const TWeakObjectPtr<UScriptStruct> WeakStruct)
#endif
		{
#if UE_4_25_OR_LATER
			if (auto* Struct = Field.Get<UScriptStruct>())
#else
			if (auto* Struct = WeakStruct.Get())
#endif
			{
				if (auto* CastedNode = Cast<UK2Node_Compare_PulldownStruct>(NewNode))
				{
					CastedNode->PulldownStruct = Struct;
					CastedNode->CachedNodeTitle.Clear();
					CastedNode->CachedNodeTooltip.Clear();
				}
			}
		}
	};
	
	NodeSpawner->SetNodeFieldDelegate = UBlueprintFieldNodeSpawner::FSetNodeFieldDelegate::CreateStatic(
		&FNodeFieldSetter::SetNodeField
#if !UE_4_25_OR_LATER
		, TWeakObjectPtr<UScriptStruct>(Struct)
#endif
	);

	return NodeSpawner;
}

#undef LOCTEXT_NAMESPACE
