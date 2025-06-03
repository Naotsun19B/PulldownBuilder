// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_SwitchPulldownStruct.h"
#include "PulldownStructNodes/Utilities/PulldownStructNodeUtils.h"
#if WITH_EDITOR
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Assets/PulldownContentsDelegates.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Types/StructContainer.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#endif
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintFieldNodeSpawner.h"
#include "KismetCompiler.h"
#include "K2Node_SwitchName.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_SwitchPulldownStruct)
#endif

#define LOCTEXT_NAMESPACE "K2Node_SwitchPulldownStruct"

UK2Node_SwitchPulldownStruct::UK2Node_SwitchPulldownStruct()
	: PulldownStruct(nullptr)
{
	bHasDefaultPin = false;
}

void UK2Node_SwitchPulldownStruct::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITOR
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownContentsLoaded.AddUObject(this, &UK2Node_SwitchPulldownStruct::HandleOnPulldownContentsLoaded);
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowAdded.AddUObject(this, &UK2Node_SwitchPulldownStruct::HandleOnPulldownRowAdded);
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowRemoved.AddUObject(this, &UK2Node_SwitchPulldownStruct::HandleOnPulldownRowRemoved);
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowRenamed.AddUObject(this, &UK2Node_SwitchPulldownStruct::HandleOnPulldownRowRenamed);
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownContentsSourceChanged.AddUObject(this, &UK2Node_SwitchPulldownStruct::HandleOnPulldownContentsSourceChanged);
#endif
}

void UK2Node_SwitchPulldownStruct::BeginDestroy()
{
#if WITH_EDITOR
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownContentsSourceChanged.RemoveAll(this);
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowRenamed.RemoveAll(this);
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowRemoved.RemoveAll(this);
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowAdded.RemoveAll(this);
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownContentsLoaded.RemoveAll(this);
#endif
	
	Super::BeginDestroy();
}

void UK2Node_SwitchPulldownStruct::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	// If the timing is too early, PulldownContents is not loaded, and even if it is reinitialized after loading,
	// the connected pin will be cut off, so save the selected values.
	if (Ar.IsSaving() && Ar.IsLoading())
	{
		Ar << SelectedValues;
#if WITH_EDITOR
		Ar << DisplayTexts;
#endif
	}
}

FText UK2Node_SwitchPulldownStruct::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (IsValid(PulldownStruct))
	{
		if (CachedNodeTitle.IsOutOfDate(this))
		{
			CachedNodeTitle.SetCachedText(
				FText::Format(
					LOCTEXT("TitleFormat", "Switch on {0}"),
					FText::FromString(PulldownStruct->GetName())
				),
				this
			);
		}
		
		return CachedNodeTitle;
	}
	
	return LOCTEXT("Title", "Switch on Unknown Pulldown Struct");
}

FText UK2Node_SwitchPulldownStruct::GetTooltipText() const
{
	if (IsValid(PulldownStruct))
	{
		if (CachedNodeTooltip.IsOutOfDate(this))
		{
			CachedNodeTooltip.SetCachedText(
				FText::Format(
					LOCTEXT("TooltipFormat", "Branch processing for each value that can be set for {0}."),
					FText::FromString(PulldownStruct->GetName())
				),
				this
			);
		}
	
		return CachedNodeTooltip;
	}

	return LOCTEXT("Tooltip", "Branch processing for each value that can be set for FPulldownStructBase::SelectedValue.");
}

void UK2Node_SwitchPulldownStruct::ReconstructNode()
{
	SelectedValues.Reset();
	Super::ReconstructNode();
}

FText UK2Node_SwitchPulldownStruct::GetMenuCategory() const
{
	if (CachedNodeCategory.IsOutOfDate(this))
	{
		CachedNodeCategory.SetCachedText(
			FText::Format(
				LOCTEXT("MenuCategoryFormat", "{0}|Pulldown Struct"),
				Super::GetMenuCategory()
			),
			this
		);
	}
	
	return CachedNodeCategory;
}

void UK2Node_SwitchPulldownStruct::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	ActionRegistrar.RegisterStructActions(
		FBlueprintActionDatabaseRegistrar::FMakeStructSpawnerDelegate::CreateUObject(
			this, &UK2Node_SwitchPulldownStruct::HandleOnMakeStructSpawner
		)
	);
}

void UK2Node_SwitchPulldownStruct::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	const UEdGraphSchema_K2* K2Schema = CompilerContext.GetSchema();
	check(IsValid(K2Schema));
	
	auto* SwitchNameNode = CompilerContext.SpawnIntermediateNode<UK2Node_SwitchName>(this, SourceGraph);
	check(IsValid(SwitchNameNode));
	SwitchNameNode->PinNames = SelectedValues;
	SwitchNameNode->AllocateDefaultPins();

	{
		UEdGraphPin* ThisNodeExecPin = GetExecPin();
		UEdGraphPin* SwitchNameNodeExecPin = SwitchNameNode->GetExecPin();
		CompilerContext.MovePinLinksToIntermediate(*ThisNodeExecPin, *SwitchNameNodeExecPin);
	}
	
	{
		UEdGraphPin* ThisNodeSelectionPin = GetSelectionPin();
		UEdGraphPin* SwitchNameNodeSelectionPin = SwitchNameNode->GetSelectionPin();
		check(
			PulldownBuilder::FPulldownStructNodeUtils::LinkPulldownStructPinToNamePin(
				this,
				CompilerContext,
				K2Schema,
				ThisNodeSelectionPin,
				SwitchNameNodeSelectionPin
			)
		);
	}

	for (const FName& SelectedValue : SelectedValues)
	{
		UEdGraphPin* ThisNodeSelectedValuePin = FindPinChecked(SelectedValue, EGPD_Output);
		UEdGraphPin* SwitchNameNodeSelectedValuePin = SwitchNameNode->FindPinChecked(SelectedValue, EGPD_Output);
		CompilerContext.MovePinLinksToIntermediate(*ThisNodeSelectedValuePin, *SwitchNameNodeSelectedValuePin);
	}

	if (bHasDefaultPin)
	{
		UEdGraphPin* ThisNodeDefaultPin = GetDefaultPin();
		UEdGraphPin* SwitchNameNodeDefaultPin = SwitchNameNode->GetDefaultPin();
		CompilerContext.MovePinLinksToIntermediate(*ThisNodeDefaultPin, *SwitchNameNodeDefaultPin);
	}
	
	BreakAllNodeLinks();
}

UK2Node::ERedirectType UK2Node_SwitchPulldownStruct::DoPinsMatchForReconstruction(
	const UEdGraphPin* NewPin,
	int32 NewPinIndex,
	const UEdGraphPin* OldPin,
	int32 OldPinIndex
) const
{
	ERedirectType ReturnValue = Super::DoPinsMatchForReconstruction(NewPin, NewPinIndex, OldPin, OldPinIndex);
#if WITH_EDITOR
	if (ReturnValue == ERedirectType_None && IsValid(PulldownStruct) && OldPinIndex > 2 && NewPinIndex > 2)
	{
		auto FindSelectedValue = [](const UEdGraphPin* Pin, const int32 PinIndex) -> TSharedPtr<FPulldownRow>
		{
			FStructContainer StructContainer;
			if (PulldownBuilder::FPulldownBuilderUtils::GenerateStructContainerFromPin(Pin, StructContainer))
			{
				const FPulldownRows& PulldownRows = PulldownBuilder::FPulldownBuilderUtils::GetPulldownRowsFromStruct(
					StructContainer.GetScriptStruct(),
					TArray<UObject*>{ PulldownBuilder::FPulldownBuilderUtils::GetOuterAssetFromPin(Pin) },
					StructContainer
				);
				if (PulldownRows.IsValidIndex(PinIndex))
				{
					return PulldownRows[PinIndex];
				}
			}

			return nullptr;
		};
		
		const TSharedPtr<FPulldownRow> OldSelectedValue = FindSelectedValue(OldPin, OldPinIndex);
		const TSharedPtr<FPulldownRow> NewSelectedValue = FindSelectedValue(NewPin, NewPinIndex);
		if (OldSelectedValue.IsValid() && NewSelectedValue.IsValid())
		{
			if (OldSelectedValue->DisplayText.EqualTo(NewSelectedValue->DisplayText))
			{
				if (OldSelectedValue->DisplayText.EqualTo(FText::FromName(NAME_None)))
				{
					ReturnValue = ERedirectType_Name;
				}
			}
		}
	}
#endif
	
	return ReturnValue;
}

FNodeHandlingFunctor* UK2Node_SwitchPulldownStruct::CreateNodeHandler(FKismetCompilerContext& CompilerContext) const
{
	// Instead of using the switch node's standard comparison mechanism,
	// define the comparison inside the ExpandNode function.
	return nullptr;
}

bool UK2Node_SwitchPulldownStruct::CanEverRemoveExecutionPin() const
{
	return true;
}

bool UK2Node_SwitchPulldownStruct::CanUserEditPinAdvancedViewFlag() const
{
	return true;
}

void UK2Node_SwitchPulldownStruct::PreloadRequiredAssets()
{
	if (IsValid(PulldownStruct))
	{
		PreloadObject(PulldownStruct);
	}
}

FEdGraphPinType UK2Node_SwitchPulldownStruct::GetPinType() const
{
	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = PulldownStruct;
	return PinType; 
}

void UK2Node_SwitchPulldownStruct::AddPinToSwitchNode()
{
	using FAdditionalConditions = TFunction<bool(const UEdGraphPin& Pin)>;
	
	auto MakeFirstAdvancedViewPinVisible = [&](const FAdditionalConditions& AdditionalConditions) -> bool
	{
		for (UEdGraphPin* Pin : Pins)
		{
			if (Pin == nullptr)
			{
				continue;
			}

			if (Pin->bAdvancedView && AdditionalConditions(*Pin))
			{
				Pin->Modify();
				Pin->bAdvancedView = false;
				return true;
			}
		}

		return false;
	};

	// First try to restore unconnected pin, since connected one is always visible.
	static TArray<FAdditionalConditions, TInlineAllocator<2>> AdditionalConditions {
		[](const UEdGraphPin& Pin){ return (Pin.LinkedTo.Num() == 0); },
		[](const UEdGraphPin& Pin){ return true; }
	};
	for (const auto& AdditionalCondition : AdditionalConditions)
	{
		if (MakeFirstAdvancedViewPinVisible(AdditionalCondition))
		{
			break;
		}
	}
}

void UK2Node_SwitchPulldownStruct::RemovePinFromSwitchNode(UEdGraphPin* TargetPin)
{
	if (TargetPin == nullptr)
	{
		return;
	}

	if (!TargetPin->bAdvancedView)
	{
		TargetPin->Modify();
		TargetPin->bAdvancedView = true;
	}

	TargetPin->BreakAllPinLinks();
}

void UK2Node_SwitchPulldownStruct::CreateSelectionPin()
{
	UEdGraphPin* Pin = CreatePin(
		EGPD_Input,
		UEdGraphSchema_K2::PC_Struct,
		PulldownStruct,
		GetSelectionPinName()
	);
	check(Pin != nullptr);
	
	if (auto* K2Schema = GetDefault<UEdGraphSchema_K2>())
	{
		K2Schema->SetPinAutogeneratedDefaultValueBasedOnType(Pin);
	}
}

void UK2Node_SwitchPulldownStruct::CreateCasePins()
{
	FillSelectedValues();
	
	const bool bShouldUseAdvancedView = (SelectedValues.Num() > 5);
	if (bShouldUseAdvancedView && (ENodeAdvancedPins::NoPins == AdvancedPinDisplay))
	{
		AdvancedPinDisplay = ENodeAdvancedPins::Hidden;
	}
	
	for (int32 Index = 0; Index < SelectedValues.Num(); Index++)
	{
		UEdGraphPin* NewPin = CreatePin(
			EGPD_Output,
			UEdGraphSchema_K2::PC_Exec,
			SelectedValues[Index]
		);
		if (NewPin == nullptr)
		{
			continue;
		}

#if WITH_EDITOR
		if (DisplayTexts.IsValidIndex(Index))
		{
			NewPin->PinFriendlyName = DisplayTexts[Index];
		}
#endif
		
		if (bShouldUseAdvancedView && (Index > 2))
		{
			NewPin->bAdvancedView = true;
		}
	}
}

void UK2Node_SwitchPulldownStruct::CreateFunctionPin()
{
	// Don't make the function pin to avoid errors, as it doesn't make use of the Switch node's standard comparison mechanism.
}

void UK2Node_SwitchPulldownStruct::RemovePin(UEdGraphPin* TargetPin)
{
	// Don't support removing pins from this switch node.
}

FName UK2Node_SwitchPulldownStruct::GetPinNameGivenIndex(int32 Index) const
{
	if (SelectedValues.IsValidIndex(Index))
	{
		return SelectedValues[Index];
	}
	
	return NAME_None;
}

UScriptStruct* UK2Node_SwitchPulldownStruct::GetPulldownStruct() const
{
	return PulldownStruct;
}

void UK2Node_SwitchPulldownStruct::SetPulldownStruct(UScriptStruct* NewPulldownStruct)
{
	PulldownStruct = NewPulldownStruct;

	ReconstructNode();
}

void UK2Node_SwitchPulldownStruct::FillSelectedValues()
{
#if WITH_EDITOR
	if (SelectedValues.Num() > 0)
	{
		return;
	}
	
	const UEdGraphPin* SelectionPin = GetSelectionPin();
	if (SelectionPin == nullptr)
	{
		return;
	}

	const auto* Struct = Cast<UScriptStruct>(SelectionPin->PinType.PinSubCategoryObject);
	if (!IsValid(Struct))
	{
		return;
	}
	
	// If the timing is too early and the PulldownContents have not been loaded, the serialized value will be used as is.
	if (!PulldownBuilder::FPulldownBuilderUtils::IsRegisteredPulldownStruct(Struct))
	{
		return;
	}
	
	FStructContainer StructContainer;
    if (!PulldownBuilder::FPulldownBuilderUtils::GenerateStructContainerFromPin(SelectionPin, StructContainer))
    {
    	return;
    }

	const FPulldownRows& PulldownRows = PulldownBuilder::FPulldownBuilderUtils::GetPulldownRowsFromStruct(
		StructContainer.GetScriptStruct(),
		TArray<UObject*>{ PulldownBuilder::FPulldownBuilderUtils::GetOuterAssetFromPin(SelectionPin) },
		StructContainer
	);

	DisplayTexts.Reset(PulldownRows.Num());

	for (const auto& PulldownRow : PulldownRows)
	{
		if (!PulldownRow.IsValid())
		{
			continue;
		}

		SelectedValues.Add(*PulldownRow->SelectedValue);
		DisplayTexts.Add(PulldownRow->GetDisplayText());
	}
#endif
}

#if WITH_EDITOR
void UK2Node_SwitchPulldownStruct::HandleOnPulldownContentsLoaded(const UPulldownContents* LoadedPulldownContents)
{
	if (!NeedToReconstructNode(LoadedPulldownContents))
	{
		return;
	}
	
	// Nodes that load quickly, such as level blueprints, use serialized selected values, so they need to be rebuilt when the asset is loaded.
	ReconstructNode();
}

void UK2Node_SwitchPulldownStruct::HandleOnPulldownRowAdded(UPulldownContents* ModifiedPulldownContents, const FName& AddedSelectedValue)
{
	HandleOnPulldownContentsLoaded(ModifiedPulldownContents);
}

void UK2Node_SwitchPulldownStruct::HandleOnPulldownRowRemoved(UPulldownContents* ModifiedPulldownContents, const FName& RemovedSelectedValue)
{
	HandleOnPulldownContentsLoaded(ModifiedPulldownContents);
}

void UK2Node_SwitchPulldownStruct::HandleOnPulldownRowRenamed(
	UPulldownContents* ModifiedPulldownContents,
	const FName& PreChangeSelectedValue,
	const FName& PostChangeSelectedValue
)
{
	if (!NeedToReconstructNode(ModifiedPulldownContents))
	{
		return;
	}

	TArray<UEdGraphPin*> LinkedPins;
	if (const UEdGraphPin* OldNamePin = FindPin(PreChangeSelectedValue, EGPD_Output))
	{
		LinkedPins = OldNamePin->LinkedTo;
	}

	ReconstructNode();

	if (UEdGraphPin* NewNamePin = FindPin(PostChangeSelectedValue, EGPD_Output))
	{
		const UEdGraphSchema_K2* K2Schema = GetDefault<UEdGraphSchema_K2>();
		check(IsValid(K2Schema));
		
		for (auto* LinkedPin : LinkedPins)
		{
			if (LinkedPin == nullptr)
			{
				continue;
			}

			K2Schema->TryCreateConnection(NewNamePin, LinkedPin);
		}
	}
}

void UK2Node_SwitchPulldownStruct::HandleOnPulldownContentsSourceChanged(UPulldownContents* ModifiedPulldownContents)
{
	HandleOnPulldownContentsLoaded(ModifiedPulldownContents);
}

bool UK2Node_SwitchPulldownStruct::NeedToReconstructNode(const UPulldownContents* PulldownContents) const
{
	const UPulldownContents* TargetPulldownContents = PulldownBuilder::FPulldownBuilderUtils::FindPulldownContentsByStruct(PulldownStruct);
	if (!IsValid(PulldownContents) || !IsValid(TargetPulldownContents))
	{
		return false;
	}

	if (PulldownContents != TargetPulldownContents)
	{
		return false;
	}

	const UEdGraphPin* SelectionPin = GetSelectionPin();
	if (SelectionPin == nullptr)
	{
		return false;
	}

	FStructContainer StructContainer;
	if (!PulldownBuilder::FPulldownBuilderUtils::GenerateStructContainerFromPin(SelectionPin, StructContainer))
	{
		return false;
	}

	const FPulldownRows& PulldownRows = PulldownBuilder::FPulldownBuilderUtils::GetPulldownRowsFromStruct(
		StructContainer.GetScriptStruct(),
		TArray<UObject*>{ PulldownBuilder::FPulldownBuilderUtils::GetOuterAssetFromPin(SelectionPin) },
		StructContainer
	);

	const int32 NumOfPulldownRows = PulldownRows.Num();
	if (NumOfPulldownRows != SelectedValues.Num())
	{
		return true;
	}

	for (int32 Index = 0; Index < NumOfPulldownRows; Index++)
	{
		const TSharedPtr<FPulldownRow> PulldownRow = PulldownRows[Index];
		if (!PulldownRow.IsValid())
		{
			return true;
		}

		const FString DisplayString = PulldownRow->SelectedValue;
		const FString SelectedValueString = SelectedValues[Index].ToString();
		if (!DisplayString.Equals(SelectedValueString))
		{
			return true;
		}
	}

	return false;
}
#endif

UBlueprintNodeSpawner* UK2Node_SwitchPulldownStruct::HandleOnMakeStructSpawner(const UScriptStruct* Struct) const
{
	if (!PulldownBuilder::FPulldownBuilderUtils::IsPulldownStruct(Struct))
	{
		return nullptr;
	}

	const UPulldownContents* PulldownContents = PulldownBuilder::FPulldownBuilderUtils::FindPulldownContentsByStruct(Struct);
	if (!IsValid(PulldownContents))
	{
		return nullptr;
	}
	if (!PulldownContents->SupportsSwitchNode())
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
				if (auto* CastedNode = Cast<UK2Node_SwitchPulldownStruct>(NewNode))
				{
					CastedNode->PulldownStruct = Struct;
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
