// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "PulldownStructBase.generated.h"

/**
 * A base struct of the struct displayed as a pull-down menu.
 * To create a pull-down menu struct in C++, define a struct that inherits this struct.
 * If the inherited child struct is used only internally and cannot be used from within Blueprint and
 * cannot be selected by FPulldownStructType, it is necessary to specify "NotBlueprintType" and
 * "BlueprintInternalUseOnly" in the USTRUCT of the child struct.
 */
USTRUCT(BlueprintInternalUseOnly)
struct FPulldownStructBase
{
	GENERATED_BODY()

public:
	// A variable to store the item selected in the pull-down menu.
	// Stores the value used at runtime.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FName SelectedValue;

public:
	// Constructor.
	FPulldownStructBase() : SelectedValue(NAME_None) {}
	explicit FPulldownStructBase(const FName& InSelectedValue) : SelectedValue(InSelectedValue) {}
	virtual ~FPulldownStructBase() = default;

	// Conversion functions.
	const FName& operator* () const { return SelectedValue; }
	FString ToString() const { return SelectedValue.ToString(); }
	FText ToText() const { return FText::FromName(SelectedValue); }
};

// Define a GetTypeHash function so that it can be used as a map key.
FORCEINLINE uint32 GetTypeHash(const FPulldownStructBase& PulldownStruct)
{
	return GetTypeHash(PulldownStruct.SelectedValue);
}

// A meta-struct that checks if it is a struct that inherits FPulldownStructBase.
template<class TPulldownStruct>
struct TIsPulldownStruct
{
public:
	static constexpr bool Value = (
		TIsDerivedFrom<TPulldownStruct, FPulldownStructBase>::IsDerived &&
#if UE_5_02_OR_LATER
		!std::is_same_v<TPulldownStruct, FPulldownStructBase>
#else
		!TIsSame<TPulldownStruct, FPulldownStructBase>::Value
#endif
	);
};

// Compares structs of the same type inheriting FPulldownStructBase and returns whether the values are equal.
template<class TPulldownStruct, typename TEnableIf<TIsPulldownStruct<TPulldownStruct>::Value, nullptr_t>::Type = nullptr>
FORCEINLINE_DEBUGGABLE bool operator==(const TPulldownStruct& Lhs, const TPulldownStruct& Rhs)
{
	return Lhs.SelectedValue.IsEqual(Rhs.SelectedValue, ENameCase::CaseSensitive);
}

// Compares structs of the same type inheriting FPulldownStructBase and returns whether the values are not equal.
template<class TPulldownStruct, typename TEnableIf<TIsPulldownStruct<TPulldownStruct>::Value, nullptr_t>::Type = nullptr>
FORCEINLINE_DEBUGGABLE bool operator!=(const TPulldownStruct& Lhs, const TPulldownStruct& Rhs)
{
	return !(Lhs == Rhs);
}
