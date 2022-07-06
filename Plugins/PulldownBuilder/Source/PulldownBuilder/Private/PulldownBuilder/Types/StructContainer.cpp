// Copyright 2021-2022 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Types/StructContainer.h"

FStructContainer::FStructContainer()
	: ScriptStruct(nullptr)
	, StructMemory(nullptr)
{
}

FStructContainer::FStructContainer(const UScriptStruct* InScriptStruct, const uint8* InStructMemory)
	: FStructContainer()
{
	InitializeAs(InScriptStruct, InStructMemory);
}

FStructContainer::FStructContainer(const FStructContainer& Other)
	: FStructContainer()
{
	InitializeAs(Other.GetScriptStruct(), Other.GetMemory());
}

FStructContainer::FStructContainer(FStructContainer&& Other) noexcept
	: FStructContainer(Other.GetScriptStruct(), Other.GetMemory())
{
	Other.SetStructData(nullptr,nullptr);
}

FStructContainer::~FStructContainer()
{
	Reset();
}

bool FStructContainer::Identical(const FStructContainer* Other, uint32 PortFlags) const
{
	if (Other == nullptr || ScriptStruct == Other->GetScriptStruct())
	{
		return false;
	}

	if (::IsValid(ScriptStruct))
	{
		return ScriptStruct->CompareScriptStruct(GetMemory(), Other->GetMemory(), PortFlags);
	}

	return true;
}

#if !BEFORE_UE_4_27
void FStructContainer::AddStructReferencedObjects(FReferenceCollector& ReferenceCollector)
{
	if (::IsValid(ScriptStruct))
	{
		ReferenceCollector.AddReferencedObjects(ScriptStruct, GetMutableMemory());
	}
}
#endif

void FStructContainer::GetPreloadDependencies(TArray<UObject*>& Dependencies)
{
	if (::IsValid(ScriptStruct))
	{
		Dependencies.Add(const_cast<UScriptStruct*>(ScriptStruct));
	}
}

void FStructContainer::InitializeAs(const UScriptStruct* InScriptStruct, const uint8* InStructMemory)
{
	Reset();

	if (!::IsValid(InScriptStruct))
	{
		return;
	}

	const int32 RequiredSize = FMath::Max(InScriptStruct->GetStructureSize(), 1);
	const uint8* Memory = static_cast<uint8*>(FMemory::Malloc(RequiredSize));
	SetStructData(InScriptStruct, Memory);

	InScriptStruct->InitializeStruct(GetMutableMemory());

	if (InStructMemory != nullptr)
	{
		InScriptStruct->CopyScriptStruct(GetMutableMemory(), InStructMemory);
	}
}

bool FStructContainer::IsValid() const
{
	return (
		::IsValid(GetScriptStruct()) &&
		GetMemory() != nullptr
	);
}

void FStructContainer::Reset()
{
	if (uint8* MutableMemory = GetMutableMemory())
	{
		DestroyScriptStruct();
		FMemory::Free(MutableMemory);
	}
	
	StructMemory = nullptr;
	ScriptStruct = nullptr;
}

const UScriptStruct* FStructContainer::GetScriptStruct() const
{
	return ScriptStruct;
}

const uint8* FStructContainer::GetMemory() const
{
	return StructMemory;
}

uint8* FStructContainer::GetMutableMemory() const
{
	return const_cast<uint8*>(StructMemory);
}

UScriptStruct* FStructContainer::ReinitializeAs(const UScriptStruct* InScriptStruct)
{
	UScriptStruct* NonConstStruct = const_cast<UScriptStruct*>(GetScriptStruct());

	if (InScriptStruct != NonConstStruct)
	{
		InitializeAs(InScriptStruct, nullptr);
		NonConstStruct = const_cast<UScriptStruct*>(GetScriptStruct());
	}

	return NonConstStruct;
}

void FStructContainer::DestroyScriptStruct() const
{
	check(StructMemory != nullptr);
	if (::IsValid(ScriptStruct))
	{
		ScriptStruct->DestroyStruct(GetMutableMemory());
	}
}

void FStructContainer::SetStructData(const UScriptStruct* InScriptStruct, const uint8* InStructMemory)
{
	ScriptStruct = InScriptStruct;
	StructMemory = InStructMemory;
}

bool UStructContainerFunctionLibrary::Get_StructContainer(const FStructContainer& Target, int32& StructData)
{
	unimplemented();
	return false;
}

bool UStructContainerFunctionLibrary::GenericGet_StructContainer(
	const FStructContainer& Target,
	const UScriptStruct* StructType,
	void* StructRawData
)
{
	if(!Target.IsValid() || !IsValid(StructType) || Target.GetScriptStruct() != StructType)
	{
		StructType->ClearScriptStruct(StructRawData);
		return false;
	}
		
	StructType->CopyScriptStruct(StructRawData, Target.GetMemory());
	return true;
}
