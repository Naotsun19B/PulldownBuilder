// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h"
#include "PulldownBuilder/Assets/PulldownContentsDelegates.h"
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Utilities/PulldownBuilderUtils.h"
#include "PulldownBuilder/Utilities/PulldownBuilderRedirectSettings.h"
#include "PulldownBuilder/Utilities/PulldownBuilderMessageLog.h"
#include "PulldownStruct/PulldownStructBase.h"
#include "PulldownStruct/NativeLessPulldownStruct.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Misc/UObjectToken.h"

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(RowNameUpdaterBase)
#endif

#define LOCTEXT_NAMESPACE "RowNameUpdaterBase"

void URowNameUpdaterBase::Register()
{
	OnPulldownRowRenamedHandle = PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowRenamed.AddStatic(&URowNameUpdaterBase::UpdateRowNames);
}

void URowNameUpdaterBase::Unregister()
{
	PulldownBuilder::FPulldownContentsDelegates::OnPulldownRowRenamed.Remove(OnPulldownRowRenamedHandle);
}

void URowNameUpdaterBase::UpdateRowNames(
	UPulldownContents* PulldownContents,
	const FName& PreChangeSelectedValue,
	const FName& PostChangeSelectedValue
)
{
	const auto& Settings = PulldownBuilder::GetSettings<UPulldownBuilderRedirectSettings>();
	if (!Settings.bShouldUpdateWhenSourceRowNameChanged)
	{
		return;
	}

	PulldownBuilder::FPulldownBuilderMessageLog MessageLog;
	MessageLog.Info(
		FText::Format(
			LOCTEXT("NotifyUpdateDisplayStrings", "The source row name for \"{0}\" has changed from \"{1}\" to \"{2}\"."),
			FText::FromString(PulldownContents->GetName()),
			FText::FromName(PreChangeSelectedValue),
			FText::FromName(PostChangeSelectedValue)
		)
	)->AddToken(FUObjectToken::Create(PulldownContents));
	
	// All upload processing of the class created by inheriting this class is performed.
	for (const UClass* Class : TObjectRange<UClass>())
	{
		if (!IsValid(Class) || (Class == URowNameUpdaterBase::StaticClass()) || Class->GetFName().IsNone())
		{
			continue;
		}

		if (auto* RowNameUpdater = Cast<URowNameUpdaterBase>(Class->GetDefaultObject()))
		{
			if (RowNameUpdater->ShouldUpdateProcess())
			{
				RowNameUpdater->UpdateRowNamesInternal(PulldownContents, PreChangeSelectedValue, PostChangeSelectedValue);
			}
		}
	}
}

void URowNameUpdaterBase::UpdateRowNamesInternal(
	UPulldownContents* PulldownContents,
	const FName& PreChangeSelectedValue,
	const FName& PostChangeSelectedValue
)
{
	unimplemented();
}

bool URowNameUpdaterBase::ShouldUpdateProcess() const
{
	const auto& Settings = PulldownBuilder::GetSettings<UPulldownBuilderRedirectSettings>();
	return Settings.ActiveRowNameUpdater.Contains(GetClass());
}

bool URowNameUpdaterBase::UpdateMemberVariables(
	UClass* Class,
	UPulldownContents* PulldownContents,
	const FName& PreChangeSelectedValue,
	const FName& PostChangeSelectedValue
)
{
	return UpdateMemberVariables(
		Class,
		Class->GetDefaultObject(),
		PulldownContents,
		PreChangeSelectedValue,
		PostChangeSelectedValue
	);
}

bool URowNameUpdaterBase::UpdateMemberVariables(
	const UStruct* Struct,
	void* ContainerPtr,
	UPulldownContents* PulldownContents,
	const FName& PreChangeSelectedValue,
	const FName& PostChangeSelectedValue
)
{
	bool bIsModified = false;
	
#if UE_4_25_OR_LATER
	for (FStructProperty* StructProperty : TFieldRange<FStructProperty>(Struct))
#else
	for (UStructProperty* StructProperty : TFieldRange<UStructProperty>(Struct))
#endif
	{
		if (StructProperty == nullptr)
		{
			continue;
		}

		const UScriptStruct* StaticStruct = StructProperty->Struct;
		if (PulldownBuilder::FPulldownBuilderUtils::IsPulldownStruct(StaticStruct) &&
			StaticStruct == PulldownContents->GetPulldownStructType().SelectedStruct)
		{
			if (auto* Value = StructProperty->ContainerPtrToValuePtr<FPulldownStructBase>(ContainerPtr))
			{
				if (Value->SelectedValue == PreChangeSelectedValue)
				{
					Value->SelectedValue = PostChangeSelectedValue;
					bIsModified = true;
				}
			}
		}
		else if (PulldownBuilder::FPulldownBuilderUtils::IsNativeLessPulldownStruct(StaticStruct))
		{
			if (auto* Value = StructProperty->ContainerPtrToValuePtr<FNativeLessPulldownStruct>(ContainerPtr))
			{
				if (Value->PulldownSource == PulldownContents->GetFName())
				{
					if (Value->SelectedValue == PreChangeSelectedValue)
					{
						Value->SelectedValue = PostChangeSelectedValue;
						bIsModified = true;
					}
				}
			}
		}	
	}

	return bIsModified;
}

FDelegateHandle URowNameUpdaterBase::OnPulldownRowRenamedHandle;

#undef LOCTEXT_NAMESPACE
