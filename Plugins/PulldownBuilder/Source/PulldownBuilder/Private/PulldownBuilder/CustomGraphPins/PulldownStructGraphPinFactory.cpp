﻿// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/CustomGraphPins/PulldownStructGraphPinFactory.h"
#include "PulldownBuilder/CustomGraphPins/SPulldownStructGraphPin.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "EdGraphSchema_K2.h"

namespace PulldownBuilder
{
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
			if (const auto* Struct = Cast<UScriptStruct>(InPin->PinType.PinSubCategoryObject))
			{
				if (FPulldownBuilderUtils::IsPulldownStruct(Struct))
				{
					// Since the DefaultValue of the pin when it is created is empty,
					// the default structure string is generated and set.
					if (InPin->DefaultValue.IsEmpty())
					{
						InPin->DefaultValue = FPulldownBuilderUtils::GenerateStructDefaultValueString(Struct);
					}
				
					return SNew(SPulldownStructGraphPin, InPin);
				}
			}
		}

		return nullptr;
	}
}
