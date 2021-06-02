﻿// Copyright 2021 Naotsun. All Rights Reserved.

#include "CustomGraphPin/NativeLessPulldownStructGraphPinFactory.h"
#include "CustomGraphPin/SNativeLessPulldownStructGraphPin.h"
#include "Utility/PulldownBuilderUtils.h"
#include "EdGraphSchema_K2.h"

TSharedPtr<FGraphPanelPinFactory> FNativeLessPulldownStructGraphPinFactory::Instance = nullptr;

void FNativeLessPulldownStructGraphPinFactory::Register()
{
	Instance = MakeShared<FNativeLessPulldownStructGraphPinFactory>();
	FEdGraphUtilities::RegisterVisualPinFactory(Instance);
}

void FNativeLessPulldownStructGraphPinFactory::Unregister()
{
	FEdGraphUtilities::UnregisterVisualPinFactory(Instance);
}

TSharedPtr<SGraphPin> FNativeLessPulldownStructGraphPinFactory::CreatePin(UEdGraphPin* InPin) const
{
	check(InPin);
	
	if (InPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Struct)
	{
		if (auto* Struct = Cast<UScriptStruct>(InPin->PinType.PinSubCategoryObject))
		{
			if (FPulldownBuilderUtils::IsNativeLessPulldownStruct(Struct))
			{
				// Since the DefaultValue of the pin when it is created is empty,
				// the default structure string is generated and set.
				if (InPin->DefaultValue.IsEmpty())
				{
					InPin->DefaultValue = FPulldownBuilderUtils::GenerateStructDefaultValueString(Struct);
				}
			
				return SNew(SNativeLessPulldownStructGraphPin, InPin);
			}
		}
	}

	return nullptr;
}
