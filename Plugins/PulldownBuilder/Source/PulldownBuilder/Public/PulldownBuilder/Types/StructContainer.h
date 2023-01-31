// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "StructContainer.generated.h"

/**
 * A container struct for putting different types of structures and passing them to the blueprint.
 * A simplified version of FInstancedStruct included in the StructUtils module for UE5.0 and above.
 */
USTRUCT(BlueprintType, BlueprintInternalUseOnly)
struct PULLDOWNBUILDER_API FStructContainer
{
	GENERATED_BODY()

public:
	// Constructor.
	FStructContainer();
	FStructContainer(const UScriptStruct* InScriptStruct, const uint8* InStructMemory);
	FStructContainer(const FStructContainer& Other);
	FStructContainer(FStructContainer&& Other) noexcept;

	// Destructor.
	~FStructContainer();

	// Copy operator.
	FStructContainer& operator=(const FStructContainer& Other)
	{
		if (this != &Other)
		{
			InitializeAs(Other.GetScriptStruct(), Other.GetMemory());
		}
		return *this;
	}

	// Move operator.
	FStructContainer& operator=(FStructContainer&& Other) noexcept
	{
		if (this != &Other)
		{
			Reset();

			SetStructData(Other.GetScriptStruct(), Other.GetMemory());
			Other.SetStructData(nullptr,nullptr);
		}
		return *this;
	}

	// Comparison operators.
	bool operator==(const FStructContainer& Other) const
	{
		return Identical(&Other, PPF_None);
	}
	bool operator!=(const FStructContainer& Other) const
	{
		return !Identical(&Other, PPF_None);
	}
	template <typename OtherType>
	bool operator==(const OtherType& Other) const
	{
		if ((GetScriptStruct() != Other.GetScriptStruct()) || (GetMemory() != Other.GetMemory()))
		{
			return false;
		}
		return true;
	}
	template <typename OtherType>
	bool operator!=(const OtherType& Other) const
	{
		return !operator==(Other);
	}
	
	// TStructOpsTypeTraits interface.
	bool Identical(const FStructContainer* Other, uint32 PortFlags) const;
#if !BEFORE_UE_4_27
	void AddStructReferencedObjects(FReferenceCollector& ReferenceCollector);
#endif
	void GetPreloadDependencies(TArray<UObject*>& Dependencies);
	// End of TStructOpsTypeTraits interface.

	// Initializes from struct type and optional data.
	void InitializeAs(const UScriptStruct* InScriptStruct, const uint8* InStructMemory = nullptr);
	
	// Returns whether the data of the struct stored in the container is valid.
	bool IsValid() const;

	// Empty the data stored in this container.
	void Reset();
	
	// Returns the type of the struct stored in this container.
	const UScriptStruct* GetScriptStruct() const;

	// Returns the raw data of the struct stored in this container.
	const uint8* GetMemory() const;
	uint8* GetMutableMemory() const;
	
	// Cast the data stored in the container to TStruct and return it.
	template<typename TStruct>
	const TStruct* Get() const
	{
		const UScriptStruct* Struct = GetScriptStruct();
		const uint8* Memory = GetMemory();
		if (::IsValid(Struct) && Memory != nullptr)
		{
			if (Struct->IsChildOf(TStruct::StaticStruct()))
			{
				return reinterpret_cast<const TStruct*>(Memory);
			}
		}
		return nullptr;
	}

protected:
	// Initializes for new struct type (does nothing if same type) and returns mutable struct.
	UScriptStruct* ReinitializeAs(const UScriptStruct* InScriptStruct);

	// Discard the struct data stored in this container.
	void DestroyScriptStruct() const;

	// Set new values for struct types and values.
	void SetStructData(const UScriptStruct* InScriptStruct, const uint8* InStructMemory);

protected:
	// The type of struct stored in this container.
	const UScriptStruct* ScriptStruct;

	// The raw data of the struct stored in this container.
	const uint8* StructMemory;
};

template<>
struct TStructOpsTypeTraits<FStructContainer> : public TStructOpsTypeTraitsBase2<FStructContainer>
{
	enum
	{
		WithIdentical = true,
#if !BEFORE_UE_4_27
		WithAddStructReferencedObjects = true,
#endif
		WithGetPreloadDependencies = true,
	};
};
