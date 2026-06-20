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

const FName UK2Node_MakeLiteral_PulldownStruct::InputPinName(TEXT("Value"));

UK2Node_MakeLiteral_PulldownStruct::UK2Node_MakeLiteral_PulldownStruct()
	: PulldownStruct(nullptr)
	, SelectedValue(NAME_None)
{
}

void UK2Node_MakeLiteral_PulldownStruct::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	// Older versions of this node had no input pin; the SelectedValue lived in a UPROPERTY
	// edited from the details panel. If we loaded such a node, reconstruct so AllocateDefaultPins
	// creates the new input pin -- the migration into the pin's default-value string runs there.
	if (HasLegacyDefaultValues() && (GetInputPin() == nullptr))
	{
		ReconstructNode();
	}
#endif
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
		// Input pin: same pull-down struct type. FPulldownStructGraphPinFactory recognises this and
		// attaches the pulldown UI to the pin's default-value widget so the user picks the value on the node itself.
		CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, PulldownStruct, InputPinName);

		// Output pin: returns the constructed literal.
		CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Struct, PulldownStruct, UEdGraphSchema_K2::PN_ReturnValue);

#if WITH_EDITOR
		// Migrate legacy SelectedValue / PulldownSource UPROPERTY data onto the freshly-created input pin.
		// No-op for nodes that never carried legacy data.
		MigrateLegacyDefaultValuesToInputPin();
#endif
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

	UEdGraphPin* InputPin = GetInputPin();
	UEdGraphPin* OutputPin = GetOutputPin();
	if ((InputPin == nullptr) || (OutputPin == nullptr))
	{
		BreakAllNodeLinks();
		return;
	}

	const UEdGraphSchema_K2* K2Schema = CompilerContext.GetSchema();
	check(IsValid(K2Schema));

	if (InputPin->LinkedTo.Num() > 0)
	{
		// Upstream is connected to the input pin: route the upstream output pin directly to whatever
		// the OutputPin was connected to, removing this node from the expanded graph entirely.
		UEdGraphPin* UpstreamPin = InputPin->LinkedTo[0];
		CompilerContext.MovePinLinksToIntermediate(*OutputPin, *UpstreamPin);
	}
	else
	{
		// No upstream link: the input pin's struct-literal default value (e.g. "(SelectedValue=Red)")
		// is the value we want. Spawn an intermediate MakeStruct node, copy the selected fields from
		// the literal string onto its sub-pins, and forward its output pin to whatever the OutputPin
		// was connected to.
		auto* MakeStructNode = CompilerContext.SpawnIntermediateNode<UK2Node_MakeStruct>(this, SourceGraph);
		check(IsValid(MakeStructNode));
		MakeStructNode->StructType = PulldownStruct;
		MakeStructNode->AllocateDefaultPins();
		MakeStructNode->bMadeAfterOverridePinRemoval = true;

		if (!ApplyInputPinDefaultsToMakeStructNode(CompilerContext, K2Schema, MakeStructNode, InputPin))
		{
			CompilerContext.MessageLog.Error(
				TEXT("[MakeLiteral PulldownStruct] Failed to apply default values to the intermediate MakeStruct node. @@"),
				this
			);
		}

		UEdGraphPin* MakeStructOutputPin = MakeStructNode->FindPinChecked(PulldownStruct->GetFName(), EGPD_Output);
		CompilerContext.MovePinLinksToIntermediate(*OutputPin, *MakeStructOutputPin);
	}

	BreakAllNodeLinks();
}

void UK2Node_MakeLiteral_PulldownStruct::PreloadRequiredAssets()
{
	if (IsValid(PulldownStruct))
	{
		PreloadObject(PulldownStruct);
	}
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

UEdGraphPin* UK2Node_MakeLiteral_PulldownStruct::GetInputPin() const
{
	return FindPin(InputPinName, EGPD_Input);
}

UEdGraphPin* UK2Node_MakeLiteral_PulldownStruct::GetOutputPin() const
{
	return FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);
}

bool UK2Node_MakeLiteral_PulldownStruct::ApplyInputPinDefaultsToMakeStructNode(
	FKismetCompilerContext& CompilerContext,
	const UEdGraphSchema_K2* K2Schema,
	UK2Node_MakeStruct* MakeStructNode,
	const UEdGraphPin* InputPin
) const
{
#if WITH_EDITOR
	check(IsValid(K2Schema) && IsValid(MakeStructNode) && (InputPin != nullptr));

	UEdGraphPin* SelectedValueSubPin = MakeStructNode->FindPin(
		GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
		EGPD_Input
	);
	if (SelectedValueSubPin == nullptr)
	{
		return false;
	}

	const TSharedPtr<FString> SelectedValueString = PulldownBuilder::FPulldownBuilderUtils::StructStringToMemberValue(
		InputPin->DefaultValue,
		GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue)
	);
	if (SelectedValueString.IsValid())
	{
		K2Schema->TrySetDefaultValue(*SelectedValueSubPin, *SelectedValueString);
	}

	return true;
#else
	return false;
#endif
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

bool UK2Node_MakeLiteral_PulldownStruct::HasLegacyDefaultValues() const
{
	return !SelectedValue.IsNone();
}

void UK2Node_MakeLiteral_PulldownStruct::ApplyLegacyDefaultValuesToString(FString& InOutDefaultValueString) const
{
#if WITH_EDITOR
	if (SelectedValue.IsNone())
	{
		return;
	}

	const TSharedPtr<FString> Result = PulldownBuilder::FPulldownBuilderUtils::MemberValueToStructString(
		InOutDefaultValueString,
		GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue),
		SelectedValue.ToString()
	);
	if (Result.IsValid())
	{
		InOutDefaultValueString = *Result;
	}
#endif
}

void UK2Node_MakeLiteral_PulldownStruct::ClearLegacyDefaultValues()
{
	SelectedValue = NAME_None;
}

void UK2Node_MakeLiteral_PulldownStruct::MigrateLegacyDefaultValuesToInputPin()
{
#if WITH_EDITOR
	if (!HasLegacyDefaultValues())
	{
		return;
	}
	if (!IsValid(PulldownStruct))
	{
		return;
	}

	UEdGraphPin* InputPin = GetInputPin();
	if (InputPin == nullptr)
	{
		return;
	}

	// Build the struct literal string and overwrite the relevant fields from the legacy UPROPERTY data.
	FString DefaultValueString = PulldownBuilder::FPulldownBuilderUtils::GenerateStructDefaultValueString(PulldownStruct);
	ApplyLegacyDefaultValuesToString(DefaultValueString);

	InputPin->DefaultValue = DefaultValueString;
	InputPin->AutogeneratedDefaultValue = PulldownBuilder::FPulldownBuilderUtils::GetStructDefaultValueString(InputPin->DefaultValue, InputPin);

	// Don't migrate again on subsequent loads or pin reconstructions.
	ClearLegacyDefaultValues();
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
