// Copyright 2021 Naotsun. All Rights Reserved.

#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Utilities/PulldownBuilderSettings.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"

void URowNameUpdaterBase::UpdateRowNames(
	UPulldownContents* PulldownContents,
	const FName& PreChangeName,
	const FName& PostChangeName
)
{
	// All upload processing of the class created by inheriting this class is performed.
	for (TObjectIterator<UClass> Itr; Itr; ++Itr)
	{
		UClass* Class = *Itr;
		if (!IsValid(Class) || Class == URowNameUpdaterBase::StaticClass())
		{
			continue;
		}

		if (auto* RowNameUpdater = Cast<URowNameUpdaterBase>(Class->GetDefaultObject()))
		{
			if (RowNameUpdater->ShouldUpdateProcess())
			{
				RowNameUpdater->UpdateRowNamesInternal(PulldownContents, PreChangeName, PostChangeName);
			}
		}
	}
}

void URowNameUpdaterBase::UpdateRowNamesInternal(
	UPulldownContents* PulldownContents,
	const FName& PreChangeName,
	const FName& PostChangeName
)
{
	unimplemented();
}

bool URowNameUpdaterBase::ShouldUpdateProcess() const
{
	auto* Settings = GetDefault<UPulldownBuilderSettings>();
	check(Settings);

	return Settings->ActiveRowNameUpdater.Contains(GetClass());
}

bool URowNameUpdaterBase::UpdateMemberVariables(
	UClass* Class,
	UPulldownContents* PulldownContents,
	const FName& PreChangeName,
	const FName& PostChangeName
)
{
	return UpdateMemberVariables(
		Class,
		Class->GetDefaultObject(),
		PulldownContents,
		PreChangeName,
		PostChangeName
	);
}

bool URowNameUpdaterBase::UpdateMemberVariables(
	const UStruct* Struct,
	void* ContainerPtr,
	UPulldownContents* PulldownContents,
	const FName& PreChangeName,
	const FName& PostChangeName
)
{
	bool bIsModified = false;
	
#if BEFORE_UE_4_24
	for (TFieldIterator<UStructProperty> PropertyItr(Struct); PropertyItr; ++PropertyItr)
	{
		UStructProperty* StructProperty = *PropertyItr;
#else
	for (TFieldIterator<FStructProperty> PropertyItr(Struct); PropertyItr; ++PropertyItr)
	{
		FStructProperty* StructProperty = *PropertyItr;
#endif 
		if (StructProperty == nullptr)
		{
			continue;
		}

		const UScriptStruct* StaticStruct = StructProperty->Struct;
		if (FPulldownBuilderUtils::IsPulldownStruct(StaticStruct) &&
			StaticStruct == PulldownContents->GetPulldownStructType().SelectedStruct)
		{
			if (auto* Value = StructProperty->ContainerPtrToValuePtr<FPulldownStructBase>(ContainerPtr))
			{
				if (Value->SelectedValue == PreChangeName)
				{
					Value->SelectedValue = PostChangeName;
					bIsModified = true;
				}
			}
		}
		else if (FPulldownBuilderUtils::IsNativeLessPulldownStruct(StaticStruct))
		{
			if (auto* Value = StructProperty->ContainerPtrToValuePtr<FNativeLessPulldownStruct>(ContainerPtr))
			{
				if (Value->PulldownSource == PulldownContents->GetFName())
				{
					if (Value->SelectedValue == PreChangeName)
					{
						Value->SelectedValue = PostChangeName;
						bIsModified = true;
					}
				}
			}
		}	
	}

	return bIsModified;
}
