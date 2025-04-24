// Copyright 2021-2025 Naotsun. All Rights Reserved.

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
struct PULLDOWNSTRUCT_API FPulldownStructBase
{
	GENERATED_BODY()

public:
	// Constructor.
	FPulldownStructBase();
	explicit FPulldownStructBase(const FName& InSelectedValue);
	virtual ~FPulldownStructBase() = default;

	// Conversion functions.
	const FName& operator* () const;
	FString ToString() const;
	FText ToText() const;
	
	// Returns whether no valid value is selected.
    bool IsNone() const;

protected:
	// Provides a point where plugin users can extend it, because normal PostSerialize is used by SETUP_PULLDOWN_STRUCT and cannot be used.
	virtual void PostSerialize_Implementation(const FArchive& Ar);
	
#if WITH_EDITOR
	// Prepares a wrapper function that calls FArchive::MarkSearchableName, because SearchableObject is private.
	void MarkSearchableName(const FArchive& Ar);
#endif

public:
	// The variable to store the item selected in the pull-down menu.
	// Stores the value used at runtime.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pulldown")
	FName SelectedValue;
	
private:
#if WITH_EDITORONLY_DATA
	// The reference that allows you to view the asset that is the source of the Selected Value from the reference viewer in the editor environment.
	// Access only through reflection so that unnecessary variables are not visible to plugin users.
	UPROPERTY(EditAnywhere, Category = "Pulldown")
	UObject* SearchableObject;

	// Whether it has been edited at least once.
	// True if the user edits even once, even if SelectedValue is None.
	UPROPERTY(EditAnywhere, Category = "Pulldown")
	bool bIsEdited;
#endif

public:
#if WITH_EDITORONLY_DATA
	// Define the property name because can not use GET_MEMBER_NAME_CHECKED outside for private members.
	static const FName SearchableObjectPropertyName;
	static const FName IsEditedPropertyName;
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

/**
 * In the editor, the object associated with the SelectedValue is the asset that is pulled down so that the asset can be opened from the reference viewer.
 * The purpose is to open the asset from the reference viewer, so it doesn't do anything at runtime.
 */
#if WITH_EDITOR
#define PULLDOWN_STRUCT_POST_SERIALIZE \
	if (Ar.IsSaving() && !Ar.IsCooking()) \
	{ \
		MarkSearchableName(Ar); \
	}
#else
#define PULLDOWN_STRUCT_POST_SERIALIZE
#endif

/**
 * A macro defined to use basic functionality in the pull-down struct.
 * Make it possible to use the FPulldownStructBase constructor and check the SelectedValue in the reference viewer.
 */
#define SETUP_PULLDOWN_STRUCT() SETUP_PULLDOWN_STRUCT_WITH_SUPER_STRUCT(FPulldownStructBase)
#define SETUP_PULLDOWN_STRUCT_WITH_SUPER_STRUCT(SuperStructName) \
	public: \
		using SuperStructName::SuperStructName; \
		void PostSerialize(const FArchive& Ar) \
		{ \
			PostSerialize_Implementation(Ar); \
			PULLDOWN_STRUCT_POST_SERIALIZE \
		}

/**
 * A macro that allows you to check the SelectedValue in the reference viewer.
 * If there are other custom aspects, define a flag after the structure name.
 */
#define SETUP_PULLDOWN_STRUCT_OPS(StructName, ...) \
	template<> \
	struct TStructOpsTypeTraits<StructName> : public TStructOpsTypeTraitsBase2<StructName> \
	{ \
		enum \
		{ \
			WithPostSerialize = true, \
			__VA_ARGS__ \
		}; \
	};
