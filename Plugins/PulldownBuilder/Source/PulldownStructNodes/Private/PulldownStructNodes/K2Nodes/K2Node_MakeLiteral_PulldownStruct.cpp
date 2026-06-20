// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_MakeLiteral_PulldownStruct.h"
#if WITH_EDITOR
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#endif
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFieldNodeSpawner.h"
#include "EditorCategoryUtils.h"
#include "KismetCompiler.h"
#include "K2Node_MakeStruct.h"
#include "EdGraphSchema_K2.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_MakeLiteral_PulldownStruct)
#endif

#define LOCTEXT_NAMESPACE "K2Node_MakeLiteral_PulldownStruct"

UK2Node_MakeLiteral_PulldownStruct::UK2Node_MakeLiteral_PulldownStruct()
	: PulldownStruct(nullptr)
	, SelectedValue(NAME_None)
{
}

FText UK2Node_MakeLiteral_PulldownStruct::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (CachedNodeTitle.IsOutOfDate(this))
	{
		if (IsValid(PulldownStruct))
		{
			CachedNodeTitle.SetCachedText(
				FText::Format(
					LOCTEXT("TitleFormat", "Make Literal {0}"),
					PulldownStruct->GetDisplayNameText()
				),
				this
			);
		}
		else
		{
			CachedNodeTitle.SetCachedText(
				LOCTEXT("Title", "Make Literal (Unknown Pulldown Struct)"),
				this
			);
		}
	}

	return CachedNodeTitle;
}

FText UK2Node_MakeLiteral_PulldownStruct::GetTooltipText() const
{
	if (CachedNodeTooltip.IsOutOfDate(this))
	{
		if (IsValid(PulldownStruct))
		{
			CachedNodeTooltip.SetCachedText(
				FText::Format(
					LOCTEXT("TooltipFormat", "Construct a literal value of {0} with the chosen SelectedValue."),
					PulldownStruct->GetDisplayNameText()
				),
				this
			);
		}
		else
		{
			CachedNodeTooltip.SetCachedText(
				LOCTEXT("Tooltip", "Construct a literal value of a pull-down struct with the chosen SelectedValue."),
				this
			);
		}
	}

	return CachedNodeTooltip;
}

FText UK2Node_MakeLiteral_PulldownStruct::GetKeywords() const
{
	return LOCTEXT("Keywords", "Make Literal Pulldown Struct");
}

void UK2Node_MakeLiteral_PulldownStruct::AllocateDefaultPins()
{
	if (IsValid(PulldownStruct))
	{
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, PulldownStruct, UEdGraphSchema_K2::PN_ReturnValue);
	}
	else
	{
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, UEdGraphSchema_K2::PN_ReturnValue);
	}
}

bool UK2Node_MakeLiteral_PulldownStruct::IsNodePure() const
{
	return true;
}

FText UK2Node_MakeLiteral_PulldownStruct::GetMenuCategory() const
{
	if (CachedNodeCategory.IsOutOfDate(this))
	{
		CachedNodeCategory.SetCachedText(
			FText::Format(
				LOCTEXT("MenuCategoryFormat", "{0}|Pulldown Struct|Make Literal"),
				FEditorCategoryUtils::GetCommonCategory(FCommonEditorCategory::Utilities)
			),
			this
		);
	}

	return CachedNodeCategory;
}

void UK2Node_MakeLiteral_PulldownStruct::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	ActionRegistrar.RegisterStructActions(
		FBlueprintActionDatabaseRegistrar::FMakeStructSpawnerDelegate::CreateUObject(
			this, &UK2Node_MakeLiteral_PulldownStruct::HandleOnMakeStructSpawner
		)
	);
}

void UK2Node_MakeLiteral_PulldownStruct::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	if (!IsValid(PulldownStruct))
	{
		BreakAllNodeLinks();
		return;
	}

	const UEdGraphSchema_K2* K2Schema = CompilerContext.GetSchema();
	check(IsValid(K2Schema));

	auto* MakeStructNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeStruct>(this, SourceGraph);
	check(IsValid(MakeStructNode));
	MakeStructNode->StructType = PulldownStruct;
	MakeStructNode->AllocateDefaultPins();
	MakeStructNode->bMadeAfterOverridePinRemoval = true;

	if (!ApplyDefaultsToMakeStructNode(CompilerContext, K2Schema, MakeStructNode))
	{
		CompilerContext.MessageLog.Error(
			TEXT("[MakeLiteral PulldownStruct] Failed to apply default values to the intermediate MakeStruct node. @@"),
			this
		);
	}

	UEdGraphPin* SourceOutputPin = GetOutputPin();
	UEdGraphPin* MakeStructOutputPin = MakeStructNode->FindPinChecked(PulldownStruct->GetFName(), EGPD_Output);
	check(CompilerContext.MovePinLinksToIntermediate(*SourceOutputPin, *MakeStructOutputPin).CanSafeConnect());

	BreakAllNodeLinks();
}

void UK2Node_MakeLiteral_PulldownStruct::PreloadRequiredAssets()
{
	if (IsValid(PulldownStruct))
	{
		PreloadObject(PulldownStruct);
	}
}

bool UK2Node_MakeLiteral_PulldownStruct::ShouldShowNodeProperties() const
{
	return true;
}

UScriptStruct* UK2Node_MakeLiteral_PulldownStruct::GetPulldownStruct() const
{
	return PulldownStruct;
}

void UK2Node_MakeLiteral_PulldownStruct::SetPulldownStruct(UScriptStruct* NewPulldownStruct)
{
	PulldownStruct = NewPulldownStruct;

	ReconstructNode();
}

const FName& UK2Node_MakeLiteral_PulldownStruct::GetSelectedValue() const
{
	return SelectedValue;
}

void UK2Node_MakeLiteral_PulldownStruct::SetSelectedValue(const FName& InSelectedValue)
{
	SelectedValue = InSelectedValue;
}

UEdGraphPin* UK2Node_MakeLiteral_PulldownStruct::GetOutputPin() const
{
	return FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);
}

bool UK2Node_MakeLiteral_PulldownStruct::ApplyDefaultsToMakeStructNode(
	FKismetCompilerContext& CompilerContext,
	const UEdGraphSchema_K2* K2Schema,
	UK2Node_MakeStruct* MakeStructNode
) const
{
	check(IsValid(K2Schema) && IsValid(MakeStructNode));

	UEdGraphPin* SelectedValuePin = MakeStructNode->FindPin(
		GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
		EGPD_Input
	);
	if (SelectedValuePin == nullptr)
	{
		return false;
	}

	K2Schema->TrySetDefaultValue(*SelectedValuePin, SelectedValue.ToString());
	return true;
}

bool UK2Node_MakeLiteral_PulldownStruct::IsTargetStruct(const UScriptStruct* Struct) const
{
#if WITH_EDITOR
	// Base node handles only non-NativeLess pull-down structs.
	// The NativeLess specialization registers itself separately.
	return PulldownBuilder::FPulldownBuilderUtils::IsPulldownStruct(Struct, true);
#else
	return false;
#endif
}

UBlueprintNodeSpawner* UK2Node_MakeLiteral_PulldownStruct::HandleOnMakeStructSpawner(const UScriptStruct* Struct) const
{
	if (!IsTargetStruct(Struct))
	{
		return nullptr;
	}

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
				if (auto* CastedNode = Cast<UK2Node_MakeLiteral_PulldownStruct>(NewNode))
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
