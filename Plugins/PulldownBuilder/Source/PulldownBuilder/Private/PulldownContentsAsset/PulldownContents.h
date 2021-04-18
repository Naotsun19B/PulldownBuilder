// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PulldownContents.generated.h"

/**
 * A structure for specifying the type of structure that inherits from FPulldownStructBase.
 */
USTRUCT(BlueprintInternalUseOnly)
struct FPulldownStructType
{
	GENERATED_BODY()

public:
	// StaticStruct pointer for the specified structure.
	UPROPERTY(EditAnywhere)
	UScriptStruct* SelectedStruct;

public:
	// Constructor.
	FPulldownStructType() : SelectedStruct(nullptr) {}
	FPulldownStructType(UScriptStruct* InStructType) : SelectedStruct(InStructType) {}
	FPulldownStructType(const FName& InStructTypeName)
	{
		SelectedStruct = FindObject<UScriptStruct>(ANY_PACKAGE, *InStructTypeName.ToString(), true);
	}

	// Overload oprators.
	FORCEINLINE bool operator ==(const FPulldownStructType& Other) const
	{
		return (SelectedStruct == Other.SelectedStruct);
	}

	FORCEINLINE bool operator !=(const FPulldownStructType& Other) const
	{
		return !(*this == Other);
	}

	FORCEINLINE const FName& operator *() const
	{
		if (IsValid(SelectedStruct))
		{
			return SelectedStruct->GetFName();
		}

		return NAME_None;
	}
	// End of overload oprators.

	// Define a GetTypeHash function so that it can be used as a map key.
	friend FORCEINLINE uint32 GetTypeHash(const FPulldownStructType& PulldownStructType)
	{
		return GetTypeHash(PulldownStructType.SelectedStruct);
	}
};

/**
 * A class that defines a structure that displays a pull-down menu and 
 * builds the list that is the basis of the pull-down menu.
 */
UCLASS(Abstract, Blueprintable)
class PULLDOWNBUILDER_API UPulldownContents : public UObject
{
	GENERATED_BODY()
	
public:
	// Returns a list of strings to display in the pull-down menu.
	// By default, it returns the value of "GetDisplayStringsBP".
	virtual TArray<TSharedPtr<FString>> GetDisplayStrings();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (DisplayName = "GetDisplayStrings"))
	TArray<FString> GetDisplayStringsFromBlueprint();

	// PulldownStructType getter.
	const FPulldownStructType& GetPulldownStructType() const { return PulldownStructType; }

	// UObject interface.
	virtual bool IsEditorOnly() const { return true; }
	// End of UObject interface.

private:
	// Structure that displays a pull-down menu.
	// It must be a structure that inherits from FPulldownStructBase.
	UPROPERTY(EditDefaultsOnly)
	FPulldownStructType PulldownStructType;
};
