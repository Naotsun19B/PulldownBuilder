// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/K2Nodes/K2Node_FindActorByPulldownStruct.h"
#include "PulldownStructNodes/Utilities/PulldownStructNodeUtils.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBUilder/Assets/PulldownContents.h"
#include "PulldownBuilder/ListGenerators/ActorNamePulldownListGenerator.h"
#include "PulldownStruct/Utilities/PulldownStructFunctionLibrary.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "GameFramework/Actor.h"
#include "BlueprintNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "KismetCompiler.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Self.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(K2Node_FindActorByPulldownStruct)
#endif

#define LOCTEXT_NAMESPACE "K2Node_FindActorByPulldownStruct"

const FName UK2Node_FindActorByPulldownStruct::WorldContextObjectPinName			= TEXT("WorldContextObject");
const FName UK2Node_FindActorByPulldownStruct::PulldownStructPinName				= TEXT("PulldownStruct");
const FName UK2Node_FindActorByPulldownStruct::WorldAndActorIdentifierNamePinName	= TEXT("WorldAndActorIdentifierName");
const FName UK2Node_FindActorByPulldownStruct::ActorClassPinName					= TEXT("ActorClass");

FText UK2Node_FindActorByPulldownStruct::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("Title", "Find Actor By Pulldown Struct");
}

FText UK2Node_FindActorByPulldownStruct::GetTooltipText() const
{
	return LOCTEXT("Tooltip", "Returns the actor found from the pull-down struct constructed by the UActorNamePulldownListGenerator.");
}

FText UK2Node_FindActorByPulldownStruct::GetKeywords() const
{
	return LOCTEXT("Keywords", "Find Actor By Pulldown Struct");
}

FSlateIcon UK2Node_FindActorByPulldownStruct::GetIconAndTint(FLinearColor& OutColor) const
{
	const UFunction* Function = FindUField<UFunction>(
		UPulldownStructFunctionLibrary::StaticClass(),
		GET_FUNCTION_NAME_CHECKED(UPulldownStructFunctionLibrary, K2_FindActorByPulldownStruct)
	);
	check(IsValid(Function));
	return UK2Node_CallFunction::GetPaletteIconForFunction(Function, OutColor);
}

void UK2Node_FindActorByPulldownStruct::AllocateDefaultPins()
{
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Wildcard, PulldownStructPinName);
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Class, AActor::StaticClass(), ActorClassPinName);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Then);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Object, AActor::StaticClass(), UEdGraphSchema_K2::PN_ReturnValue);
}

FText UK2Node_FindActorByPulldownStruct::GetMenuCategory() const
{
	return LOCTEXT("MenuCategory", "Actor");
}

void UK2Node_FindActorByPulldownStruct::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* ActionKey = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(ActionKey))
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintNodeSpawner::Create(ActionKey);
		check(IsValid(NodeSpawner));
		ActionRegistrar.AddBlueprintAction(ActionKey, NodeSpawner);
	}
}

void UK2Node_FindActorByPulldownStruct::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	const UEdGraphSchema_K2* K2Schema = CompilerContext.GetSchema();
	check(IsValid(K2Schema));

	auto* FindActorByPulldownStructNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	check(IsValid(FindActorByPulldownStructNode));
	FindActorByPulldownStructNode->FunctionReference.SetExternalMember(
		GET_FUNCTION_NAME_CHECKED(UPulldownStructFunctionLibrary, K2_FindActorByPulldownStruct),
		UPulldownStructFunctionLibrary::StaticClass()
	);
	FindActorByPulldownStructNode->AllocateDefaultPins();

	{
		UEdGraphPin* SourcePin = FindPinChecked(UEdGraphSchema_K2::PN_Execute, EGPD_Input);
		UEdGraphPin* FindActorByPulldownStructNodePin = FindActorByPulldownStructNode->FindPinChecked(UEdGraphSchema_K2::PN_Execute, EGPD_Input);
		check(CompilerContext.MovePinLinksToIntermediate(*SourcePin, *FindActorByPulldownStructNodePin).CanSafeConnect());
	}
	{
		UK2Node_Self* SelfNode = CompilerContext.SpawnIntermediateNode<UK2Node_Self>(this, SourceGraph);
		check(IsValid(SelfNode));
		SelfNode->AllocateDefaultPins();
		
		UEdGraphPin* SelfNodePin = SelfNode->FindPinChecked(UEdGraphSchema_K2::PSC_Self, EGPD_Output);
		UEdGraphPin* FindActorByPulldownStructNodePin = FindActorByPulldownStructNode->FindPinChecked(WorldContextObjectPinName, EGPD_Input);
		check(K2Schema->TryCreateConnection(SelfNodePin, FindActorByPulldownStructNodePin));
	}
	{
		UEdGraphPin* PulldownStructPin = FindPinChecked(PulldownStructPinName, EGPD_Input);
		const UScriptStruct* PulldownStructPinType = Cast<UScriptStruct>(PulldownStructPin->PinType.PinSubCategoryObject.Get());
		if ((PulldownStructPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct) && IsValid(PulldownStructPinType))
		{
			UEdGraphPin* WorldAndActorIdentifierNamePin = FindActorByPulldownStructNode->FindPinChecked(WorldAndActorIdentifierNamePinName, EGPD_Input);
			check(
				PulldownBuilder::FPulldownStructNodeUtils::LinkPulldownStructPinToNamePin(
					this,
					CompilerContext,
					K2Schema,
					PulldownStructPin,
					WorldAndActorIdentifierNamePin
				)
			);
		}
	}
	{
		UEdGraphPin* SourcePin = FindPinChecked(ActorClassPinName, EGPD_Input);
		UEdGraphPin* FindActorByPulldownStructNodePin = FindActorByPulldownStructNode->FindPinChecked(ActorClassPinName, EGPD_Input);
		check(CompilerContext.MovePinLinksToIntermediate(*SourcePin, *FindActorByPulldownStructNodePin).CanSafeConnect());
	}
	{
		UEdGraphPin* SourcePin = FindPinChecked(UEdGraphSchema_K2::PN_Then, EGPD_Output);
		UEdGraphPin* FindActorByPulldownStructNodePin = FindActorByPulldownStructNode->FindPinChecked(UEdGraphSchema_K2::PN_Then, EGPD_Output);
		check(CompilerContext.MovePinLinksToIntermediate(*SourcePin, *FindActorByPulldownStructNodePin).CanSafeConnect());
	}
	{
		UEdGraphPin* SourcePin = FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);
		UEdGraphPin* FindActorByPulldownStructNodePin = FindActorByPulldownStructNode->FindPinChecked(UEdGraphSchema_K2::PN_ReturnValue, EGPD_Output);
		check(CompilerContext.MovePinLinksToIntermediate(*SourcePin, *FindActorByPulldownStructNodePin).CanSafeConnect());
	}
}

void UK2Node_FindActorByPulldownStruct::PostReconstructNode()
{
	Super::PostReconstructNode();

	RefreshPulldownStructType();
	RefreshBaseActorClass();
}

void UK2Node_FindActorByPulldownStruct::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);
	
	RefreshPulldownStructType();
	RefreshBaseActorClass();
}

bool UK2Node_FindActorByPulldownStruct::IsConnectionDisallowed(const UEdGraphPin* MyPin, const UEdGraphPin* OtherPin, FString& OutReason) const
{
	if (!ensure(MyPin != nullptr && OtherPin != nullptr))
	{
		return true;
	}

	bool bIsConnectionDisallowed = false;
	
	const UEdGraphPin* PulldownStructPin = FindPinChecked(PulldownStructPinName, EGPD_Input);
	if (MyPin == PulldownStructPin)
	{
		const UScriptStruct* OtherPinStructType = Cast<UScriptStruct>(OtherPin->PinType.PinSubCategoryObject.Get());

		auto IsPulldownStructPin = [&]() -> bool
		{
			if (OtherPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct)
			{
				return false;
			}

			return (
				PulldownBuilder::FPulldownBuilderUtils::IsPulldownStruct(OtherPinStructType) ||
				PulldownBuilder::FPulldownBuilderUtils::IsNativeLessPulldownStruct(OtherPinStructType)
			);
		};
		auto IsSupportedPulldownStruct = [&]() -> bool
		{
			const UPulldownContents* PulldownContents = PulldownBuilder::FPulldownBuilderUtils::FindPulldownContentsByStruct(OtherPinStructType);
			if (!IsValid(PulldownContents))
			{
				return false;
			}

			const auto* ActorNamePulldownListGenerator = PulldownContents->GetPulldownListGenerator<UActorNamePulldownListGenerator>();
			return IsValid(ActorNamePulldownListGenerator);
		};

		if (!IsPulldownStructPin())
		{
			OutReason = LOCTEXT("IsNotPulldownStructPinDisallowedReason", "The type of the structure specified in the argument must be a structure that inherits PulldownStructBase or NativeLessPulldownStruct.").ToString();
			bIsConnectionDisallowed = true;
		}
		else if (!IsSupportedPulldownStruct())
		{
			OutReason = LOCTEXT("IsNotSupportedPulldownStructDisallowedReason", "The type of pulldown structure specified in the argument must be set the pulldown list generator in PulldownContents as ActorNamePulldownListGenerator.").ToString();
			bIsConnectionDisallowed = true;
		}
	}
	
	return (Super::IsConnectionDisallowed(MyPin, OtherPin, OutReason) || bIsConnectionDisallowed);
}

UScriptStruct* UK2Node_FindActorByPulldownStruct::GetLinkedPulldownStructType() const
{
	const UEdGraphPin* PulldownStructPin = FindPinChecked(PulldownStructPinName, EGPD_Input);
	if (PulldownStructPin->LinkedTo.Num() == 0)
	{
		return nullptr;
	}

	UScriptStruct* PulldownStructType = nullptr;
	const TArray<UEdGraphPin*>& LinkedTo = PulldownStructPin->LinkedTo;
	for (int32 LinkIndex = 0; LinkIndex < LinkedTo.Num(); LinkIndex++)
	{
		const UEdGraphPin* LinkedPin = LinkedTo[LinkIndex];
		if (LinkedPin == nullptr)
		{
			continue;
		}
		
		UScriptStruct* LinkType = Cast<UScriptStruct>(LinkedPin->PinType.PinSubCategoryObject.Get());
		if (!IsValid(LinkType))
		{
			continue;
		}

		PulldownStructType = LinkType;
		break;
	}

	return PulldownStructType;
}

void UK2Node_FindActorByPulldownStruct::RefreshPulldownStructType()
{
	UEdGraphPin* PulldownStructPin = FindPinChecked(PulldownStructPinName, EGPD_Input);
	if (PulldownStructPin->SubPins.Num() > 0)
	{
		const UEdGraphSchema* Schema = GetSchema();
		check(IsValid(Schema));
		Schema->RecombinePin(PulldownStructPin);
	}
	
	UScriptStruct* PulldownStructType = GetLinkedPulldownStructType();
	PulldownStructPin->PinType.PinCategory = (IsValid(PulldownStructType) ? UEdGraphSchema_K2::PC_Struct : UEdGraphSchema_K2::PC_Wildcard);
	PulldownStructPin->PinType.PinSubCategoryObject = PulldownStructType;
}

void UK2Node_FindActorByPulldownStruct::RefreshBaseActorClass()
{
	auto GetBaseActorClass = [&](UClass*& BaseActorClass) -> bool
	{
		const UEdGraphPin* PulldownStructPin = FindPinChecked(PulldownStructPinName, EGPD_Input);
		if (PulldownStructPin->PinType.PinCategory != UEdGraphSchema_K2::PC_Struct)
		{
			return false;
		}
		
		const UScriptStruct* PulldownStructType = Cast<UScriptStruct>(PulldownStructPin->PinType.PinSubCategoryObject.Get());
		if (!IsValid(PulldownStructType))
		{
			return false;
		}

		const UPulldownContents* PulldownContents = PulldownBuilder::FPulldownBuilderUtils::FindPulldownContentsByStruct(PulldownStructType);
		if (!IsValid(PulldownContents))
		{
			return false;
		}

		const auto* ActorNamePulldownListGenerator = PulldownContents->GetPulldownListGenerator<UActorNamePulldownListGenerator>();
		if (!IsValid(ActorNamePulldownListGenerator))
		{
			return false;
		}

		BaseActorClass = ActorNamePulldownListGenerator->GetActorClass();
		return true;
	};
	
	UClass* BaseActorClass = nullptr;
	if (!GetBaseActorClass(BaseActorClass))
	{
		BaseActorClass = AActor::StaticClass();
	}
	
	UEdGraphPin* ActorClassPin = FindPinChecked(ActorClassPinName, EGPD_Input);
	ActorClassPin->PinType.PinSubCategoryObject = BaseActorClass;
}

#undef LOCTEXT_NAMESPACE
