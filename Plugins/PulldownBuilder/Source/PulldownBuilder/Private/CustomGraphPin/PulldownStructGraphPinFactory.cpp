// Copyright 2021 Naotsun. All Rights Reserved.

#include "CustomGraphPin/PulldownStructGraphPinFactory.h"
#include "PulldownBuilderGlobals.h"
#include "Utility/PulldownBuilderUtils.h"
#include "CustomGraphPin/SPulldownStructGraphPin.h"

TSharedPtr<FGraphPanelPinFactory> FPulldownStructGraphPinFactory::Instance = nullptr;

void FPulldownStructGraphPinFactory::Register()
{
	Instance = MakeShared<FPulldownStructGraphPinFactory>();
	FEdGraphUtilities::RegisterVisualPinFactory(Instance);
}

void FPulldownStructGraphPinFactory::Unregister()
{
	FEdGraphUtilities::UnregisterVisualPinFactory(Instance);
}

TSharedPtr<SGraphPin> FPulldownStructGraphPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	check(InPin);
	
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		if (auto* Struct = Cast<UScriptStruct>(InPin->PinType.PinSubCategoryObject))
		{
			if (FPulldownBuilderUtils::IsPulldownStruct(Struct))
			{
				return SNew(SPulldownStructGraphPin, InPin);
			}
		}
	}

	return nullptr;
}
