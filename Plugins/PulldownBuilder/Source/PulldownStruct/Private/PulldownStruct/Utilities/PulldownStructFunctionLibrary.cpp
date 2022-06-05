// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownStruct/Utilities/PulldownStructFunctionLibrary.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

bool UPulldownStructFunctionLibrary::Equal_PulldownStruct(const FPulldownStructBase& Lhs, const FPulldownStructBase& Rhs)
{
	unimplemented();
	return false;
}

bool UPulldownStructFunctionLibrary::GenericEqual_PulldownStruct(
	const UScriptStruct* LhsType,
	const void* LhsValue,
	const UScriptStruct* RhsType,
	const void* RhsValue
)
{
	FName LhsSelectedValue;
	if (!TryGetSelectedValue(LhsSelectedValue, LhsType, LhsValue))
	{
		UE_LOG(LogPulldownBuilder, Warning, TEXT("An invalid pull-down structure was specified for lhs. (%s)"), *GetNameSafe(LhsType));
		return false;
	}

	FName RhsSelectedValue;
	if (!TryGetSelectedValue(RhsSelectedValue, RhsType, RhsValue))
	{
		UE_LOG(LogPulldownBuilder, Warning, TEXT("An invalid pull-down structure was specified for rhs. (%s)"), *GetNameSafe(RhsType));
		return false;
	}

	return (LhsSelectedValue == RhsSelectedValue);
}

bool UPulldownStructFunctionLibrary::TryGetSelectedValue(FName& SelectedValue, const UScriptStruct* Type, const void* Value)
{
#if BEFORE_UE_4_24
	for (const auto* NameProperty : TFieldRange<UNameProperty>(Type))
#else
	for (const auto* NameProperty : TFieldRange<FNameProperty>(Type))
#endif
	{
		if (NameProperty == nullptr)
		{
			continue;
		}

		if (NameProperty->GetFName() != GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SelectedValue))
		{
			continue;
		}

		if (const FName* SelectedValuePtr = NameProperty->GetPropertyValuePtr_InContainer(Value))
		{
			SelectedValue = *SelectedValuePtr;
			return true;
		}
	}

	return false;
}
