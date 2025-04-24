// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_ConvertPulldownStructToString.h"
#include "PulldownStructNodes/Utilities/PulldownStructNodeUtils.h"
#if WITH_EDITOR
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#endif
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Kismet/KismetStringLibrary.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFieldNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_ConvertPulldownStructToString)
#endif

#define LOCTEXT_NAMESPACE "K2Node_ToString_PulldownStruct"

const FName UK2Node_ConvertPulldownStructToString::PulldownStructPinName = TEXT("PulldownStruct");

FText UK2Node_ConvertPulldownStructToString::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (CachedNodeTitle.IsOutOfDate(this))
	{
		if (IsValid(PulldownStruct))
		{
			CachedNodeTitle.SetCachedText(
				FText::Format(
					LOCTEXT("NodeTitleFormat", "To String ({0})"),
					PulldownStruct->GetDisplayNameText()
				),
				this
			);
		}
		else
		{
			CachedNodeTitle.SetCachedText(
				LOCTEXT("NodeTitle", "To String (Unknown Pulldown Struct)"),
				this
			);
		}
	}
		
	return CachedNodeTitle;
}

FText UK2Node_ConvertPulldownStructToString::GetTooltipText() const
{
	if (CachedNodeTooltip.IsOutOfDate(this))
	{
		CachedNodeTitle.SetCachedText(
			FText::Format(
				LOCTEXT("NodeTooltipFormat", "Convert the values of SelectedValue in {0} and return as string."),
				PulldownStruct->GetDisplayNameText()
			),
			this
		);
	}
	
	return CachedNodeTooltip;
}

void UK2Node_ConvertPulldownStructToString::AllocateDefaultPins()
{
	if (IsValid(PulldownStruct))
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, PulldownStruct, PulldownStructPinName);
	}
	else
	{
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, PulldownStructPinName);
	}
	
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_String, UEdGraphSchema_K2::PN_ReturnValue);
}

bool UK2Node_ConvertPulldownStructToString::IsNodePure() const
{
	return true;
}

bool UK2Node_ConvertPulldownStructToString::ShouldDrawCompact() const
{
	return true;
}

FText UK2Node_ConvertPulldownStructToString::GetCompactNodeTitle() const
{
	return LOCTEXT("CompactNodeTitle", "\x2022");
}

FText UK2Node_ConvertPulldownStructToString::GetMenuCategory() const
{
	if (CachedNodeCategory.IsOutOfDate(this))
	{
		CachedNodeCategory.SetCachedText(
			FText::Format(
				LOCTEXT("MenuCategoryFormat", "{0}|Operators|Pulldown Struct|To String"),
				FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Utilities)
			),
			this
		);
	}
	
	return CachedNodeCategory;
}

void UK2Node_ConvertPulldownStructToString::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	ActionRegistrar.RegisterStructActions(
		FBlueprintActionDatabaseRegistrar::FMakeStructSpawnerDelegate::CreateUObject(
			this, &UK2Node_ConvertPulldownStructToString::HandleOnMakeStructSpawner
		)
	);
}

void UK2Node_ConvertPulldownStructToString::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	if (IsValid(PulldownStruct))
	{
		const UEdGraphSchema_K2* K2Schema = CompilerContext.GetSchema();
		check(IsValid(K2Schema));
		
		auto* NameToStringValueCompareNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
		check(IsValid(NameToStringValueCompareNode));
		NameToStringValueCompareNode->FunctionReference.SetExternalMember(
			GET_FUNCTION_NAME_STRING_CHECKED(UKismetStringLibrary, Conv_NameToString),
			UKismetStringLibrary::StaticClass()
		);
		NameToStringValueCompareNode->AllocateDefaultPins();

		UEdGraphPin* PulldownStructPin = FindPinChecked(PulldownStructPinName, EGPD_Input);
		UEdGraphPin* SelectedValueCompareNodePin = NameToStringValueCompareNode->FindPinChecked(TEXT("InName"), EGPD_Input);
		check(
			PulldownBuilder::FPulldownStructNodeUtils::LinkPulldownStructPinToNamePin(
				this,
				CompilerContext,
				K2Schema,
				PulldownStructPin,
				SelectedValueCompareNodePin
			)
		);

		{
			UEdGraphPin* SourceReturnValuePin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);
			UEdGraphPin* CompareNodeReturnValuePin = NameToStringValueCompareNode->GetReturnValuePin();
			CompilerContext.MovePinLinksToIntermediate(*SourceReturnValuePin, *CompareNodeReturnValuePin);
		}
	}
	
	BreakAllNodeLinks();
}

void UK2Node_ConvertPulldownStructToString::PreloadRequiredAssets()
{
	if (IsValid(PulldownStruct))
	{
		PreloadObject(PulldownStruct);
	}
}

UScriptStruct* UK2Node_ConvertPulldownStructToString::GetPulldownStruct() const
{
	return PulldownStruct;
}

void UK2Node_ConvertPulldownStructToString::SetPulldownStruct(UScriptStruct* NewPulldownStruct)
{
	PulldownStruct = NewPulldownStruct;

	ReconstructNode();
}

UBlueprintNodeSpawner* UK2Node_ConvertPulldownStructToString::HandleOnMakeStructSpawner(const UScriptStruct* Struct) const
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
				if (auto* CastedNode = Cast<UK2Node_ConvertPulldownStructToString>(NewNode))
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
