// Copyright 2021-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
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
	// The variable to store the item selected in the pull-down menu.
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
	
	// Returns whether no valid value is selected.
    bool IsNone() const { return SelectedValue.IsNone(); }

protected:
	// Provides a point where plugin users can extend it, because normal PostSerialize is used by SETUP_PULLDOWN_STRUCT and cannot be used.
	virtual void PostSerialize_Implementation(const FArchive& Ar) {}
	
#if WITH_EDITOR
	// Prepares a wrapper function that calls FArchive::MarkSearchableName, because SearchableObject is private.
	void MarkSearchableName(const FArchive& Ar)
	{
		Ar.MarkSearchableName(SearchableObject, SelectedValue);
	}
#endif
	
private:
#if WITH_EDITORONLY_DATA
	// The reference that allows you to view the asset that is the source of the Selected Value from the reference viewer in the editor environment.
	// Access only through reflection so that unnecessary variables are not visible to plugin users.
	UPROPERTY(VisibleAnywhere)
	UObject* SearchableObject = nullptr;
#endif

public:
#if WITH_EDITORONLY_DATA
	// Define the property name because SearchableObject is private and GET_MEMBER_NAME_CHECKED cannot be used.
	PULLDOWNSTRUCT_API static const FName SearchableObjectPropertyName;
#endif
};

// A meta-struct that checks if it is a struct that inherits FPulldownStructBase.
template<typename TPulldownStruct>
struct TIsPulldownStruct
{
public:
	static constexpr bool Value = TIsDerivedFrom<TPulldownStruct, FPulldownStructBase>::IsDerived;
};

// Compares structs of the same type inheriting FPulldownStructBase and returns whether the values are equal.
template<typename TPulldownStruct, typename TEnableIf<TIsPulldownStruct<TPulldownStruct>::Value, nullptr_t>::Type = nullptr>
FORCEINLINE_DEBUGGABLE bool operator==(const TPulldownStruct& Lhs, const TPulldownStruct& Rhs)
{
	return Lhs.SelectedValue.IsEqual(Rhs.SelectedValue, ENameCase::CaseSensitive);
}

// Compares structs of the same type inheriting FPulldownStructBase and returns whether the values are not equal.
template<typename TPulldownStruct, typename TEnableIf<TIsPulldownStruct<TPulldownStruct>::Value, nullptr_t>::Type = nullptr>
FORCEINLINE_DEBUGGABLE bool operator!=(const TPulldownStruct& Lhs, const TPulldownStruct& Rhs)
{
	return !(Lhs == Rhs);
}

// Define a GetTypeHash function so that it can be used as a map key.
template<typename TPulldownStruct, typename TEnableIf<TIsPulldownStruct<TPulldownStruct>::Value, nullptr_t>::Type = nullptr>
FORCEINLINE_DEBUGGABLE uint32 GetTypeHash(const TPulldownStruct& PulldownStruct)
{
	return GetTypeHash(PulldownStruct.SelectedValue);
}

// Returns the SelectedValue of the structs that inherits FPulldownStructBase in the type of FString.
template<typename TPulldownStruct, typename TEnableIf<TIsPulldownStruct<TPulldownStruct>::Value, nullptr_t>::Type = nullptr>
FORCEINLINE_DEBUGGABLE FString LexToString(const TPulldownStruct& PulldownStruct)
{
	return PulldownStruct.ToString();
}

// Initializes the struct that inherits from FPulldownStructBase from the SelectedValue string.
template<typename TPulldownStruct, typename TEnableIf<TIsPulldownStruct<TPulldownStruct>::Value, nullptr_t>::Type = nullptr>
FORCEINLINE_DEBUGGABLE void LexFromString(TPulldownStruct& PulldownStruct, const TCHAR* Buffer)
{
	PulldownStruct.SelectedValue = FName(Buffer);
}
