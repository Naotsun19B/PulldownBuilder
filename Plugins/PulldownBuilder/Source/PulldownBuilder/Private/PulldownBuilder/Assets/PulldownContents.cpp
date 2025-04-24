// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Assets/PulldownContents.h"
#include "PulldownBuilder/Assets/PulldownContentsLoader.h"
#include "PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructDetail.h"
#include "PulldownBuilder/Utilities/PulldownBuilderMessageLog.h"
#include "PulldownBuilder/Utilities/PulldownBuilderAppearanceSettings.h"
#include "PulldownBuilder/Types/PulldownRow.h"
#include "Editor.h"
#include "Subsystems/AssetEditorSubsystem.h"
#if UE_5_04_OR_LATER
#include "UObject/AssetRegistryTagsContext.h"
#endif

#if UE_5_01_OR_LATER
#include UE_INLINE_GENERATED_CPP_BY_NAME(PulldownContents)
#endif

#define LOCTEXT_NAMESPACE "PulldownContents"

const FName UPulldownContents::RegisteredStructTypeTag = TEXT("RegisteredStructType");
const FName UPulldownContents::GeneratorClassTag = TEXT("GeneratorClass");
const FName UPulldownContents::SourceAssetTag = TEXT("SourceAsset");

UPulldownContents::UPulldownContents()
	: PulldownListGenerator(nullptr)
	, bOverridePanelSize(false)
	, PanelSizeOverride(UPulldownBuilderAppearanceSettings::DefaultPanelSize)
	, PreChangePulldownListGenerator(nullptr)
{
}

bool UPulldownContents::IsEditorOnly() const
{
	return true;
}

void UPulldownContents::PostLoad()
{
	Super::PostLoad();

	if (IsValid(GEditor))
	{
		if (auto* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			AssetEditorSubsystem->OnAssetEditorOpened().AddUObject(this, &UPulldownContents::HandleOnAssetEditorOpened);
		}
	}
	
	FCoreUObjectDelegates::OnPackageReloaded.AddUObject(this, &UPulldownContents::HandleOnPackageReloaded);
	
	RegisterDetailCustomization();
}

#if UE_4_25_OR_LATER
void UPulldownContents::PreEditChange(FProperty* PropertyAboutToChange)
#else
void UPulldownContents::PreEditChange(UProperty* PropertyAboutToChange)
#endif
{
	Super::PreEditChange(PropertyAboutToChange);

	if (PropertyAboutToChange == nullptr)
	{
		return;
	}

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UPulldownContents, PulldownStructType) ||
		PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(FPulldownStructType, SelectedStruct))
	{
		UnregisterDetailCustomization();
	}

	if (PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UPulldownContents, PulldownListGenerator))
	{
		PreChangePulldownListGenerator = PulldownListGenerator;
	}
}

void UPulldownContents::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPulldownContents, PulldownStructType))
	{
		RegisterDetailCustomization();
		ModifyPulldownListGenerator();
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPulldownContents, PulldownListGenerator))
	{
		ModifyPulldownListGenerator();
	}
}

void UPulldownContents::BeginDestroy()
{
	UnregisterDetailCustomization();

	FCoreUObjectDelegates::OnPackageReloaded.RemoveAll(this);

	if (IsValid(GEditor))
	{
		if (auto* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			AssetEditorSubsystem->OnAssetEditorOpened().RemoveAll(this);
		}
	}
	
	Super::BeginDestroy();
}

void UPulldownContents::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);

	// Prevent the same struct from being registered when duplicated.
	PulldownStructType = FPulldownStructType();
}

#if UE_5_04_OR_LATER
void UPulldownContents::GetAssetRegistryTags(FAssetRegistryTagsContext Context) const
#else
void UPulldownContents::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
#endif
{
	Super::GetAssetRegistryTags(
#if UE_5_04_OR_LATER
		Context
#else
		OutTags
#endif
	);

	auto AddTag = [&](
		const FName& Name,
		const FString& Value,
		const FAssetRegistryTag::ETagType& Type = FAssetRegistryTag::TT_Alphabetical,
		const uint32 DisplayFlags = FAssetRegistryTag::TD_None
	)
	{
		const FAssetRegistryTag Tag(Name, Value, Type, DisplayFlags);
#if UE_5_04_OR_LATER
		Context.AddTag(Tag);
#else
		OutTags.Add(Tag);
#endif	
	};
	
	// Added the name of the struct registered in this asset to AssetRegistryTags.
	AddTag(RegisteredStructTypeTag, (*PulldownStructType).ToString());

	// Added the PulldownListGenerator class name set for this asset to AssetRegistryTags.
	AddTag(GeneratorClassTag, GetPulldownListGeneratorClassName());

	// Added the name of the underlying asset of the PulldownListGenerator to AssetRegistryTags.
	FString SourceAssetName = LexToString(NAME_None);
	if (IsValid(PulldownListGenerator))
	{
		if (PulldownListGenerator->HasSourceAsset())
		{
			SourceAssetName = PulldownListGenerator->GetSourceAssetName();
		}
	}
	AddTag(SourceAssetTag, SourceAssetName);
}

const FPulldownStructType& UPulldownContents::GetPulldownStructType() const
{
	return PulldownStructType;
}

FPulldownRows UPulldownContents::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	FPulldownRows PulldownRows;
	
	if (IsValid(PulldownListGenerator))
	{
		PulldownRows = PulldownListGenerator->GetPulldownRows(OuterObjects, StructInstance);
	}

	return PulldownRows;
}

FString UPulldownContents::GetPulldownListGeneratorClassName() const
{
	FName GeneratorClassName = NAME_None;
	if (IsValid(PulldownListGenerator))
	{
		if (const UClass* Class = PulldownListGenerator->GetClass())
		{
			GeneratorClassName = Class->GetFName();
		}
	}

	return GeneratorClassName.ToString();
}

FString UPulldownContents::GetTooltip() const
{
	FString Tooltip = FString::Printf(
		TEXT("%s : %s\n%s : %s"),
		*RegisteredStructTypeTag.ToString(), *FName(*PulldownStructType).ToString(),
		*GeneratorClassTag.ToString(), *GetPulldownListGeneratorClassName()
	);
	
	if (IsValid(PulldownListGenerator))
	{
		if (PulldownListGenerator->HasSourceAsset())
		{
			Tooltip += FString::Printf(
				TEXT("\n%s : %s"),
				*SourceAssetTag.ToString(), *PulldownListGenerator->GetSourceAssetName()
			);
		}
	}

	return Tooltip;
}

TOptional<FVector2D> UPulldownContents::GetIndividualPanelSize() const
{
	if (bOverridePanelSize)
	{
		return PanelSizeOverride;
	}

	return {};
}

void UPulldownContents::RegisterDetailCustomization()
{
	// The default object does not perform registration processing.
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	// If the Pulldown Struct Type is empty, the process will be skipped.
	if (!PulldownStructType.IsValid())
	{
		return;
	}

	PulldownBuilder::FPulldownStructDetail::Register(PulldownStructType);
	UE_LOG(LogPulldownBuilder, Log, TEXT("[%s] %s has been registered with detail customization."), *GetName(), *FName(*PulldownStructType).ToString());
}

void UPulldownContents::UnregisterDetailCustomization()
{
	// The default object does not perform registration processing.
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	// If the Pulldown Struct Type is empty, the process will be skipped.
	if (!PulldownStructType.IsValid())
	{
		return;
	}

	PulldownBuilder::FPulldownStructDetail::Unregister(PulldownStructType);
	UE_LOG(LogPulldownBuilder, Log, TEXT("[%s] %s has been unregistered from detail customization."), *GetName(), *FName(*PulldownStructType).ToString());
}

void UPulldownContents::ModifyPulldownListGenerator()
{
	if (IsValid(PulldownListGenerator))
	{
		const TArray<UScriptStruct*> FilterPulldownStructTypes = PulldownListGenerator->GetFilterPulldownStructTypes();
		if (FilterPulldownStructTypes.Num() > 0 &&
			IsValid(PulldownStructType.SelectedStruct) &&
			!FilterPulldownStructTypes.Contains(PulldownStructType.SelectedStruct))
		{
			FString FilterPulldownStructTypeNames;
			for (const auto* FilterPulldownStructType : FilterPulldownStructTypes)
			{
				if (!IsValid(FilterPulldownStructType))
				{
					continue;
				}

				FilterPulldownStructTypeNames += FString::Printf(TEXT("%s, "), *FilterPulldownStructType->GetName());
			}
			if (FilterPulldownStructTypeNames.IsEmpty())
			{
				FilterPulldownStructTypeNames = TEXT("None");
			}
			else
			{
				FilterPulldownStructTypeNames = FilterPulldownStructTypeNames.Mid(0, FilterPulldownStructTypeNames.Len() - 2);
			}
			
			PulldownBuilder::FPulldownBuilderMessageLog MessageLog;
			MessageLog.Warning(
				FText::Format(
					LOCTEXT("FailedChangePulldownListGenerator", "{0} cannot be used for {1}.\nThe struct types that can be used are as follows:\n{2}"),
					FText::FromString(GetNameSafe(PulldownListGenerator->GetClass())),
					FText::FromString(GetNameSafe(PulldownStructType.SelectedStruct)),
					FText::FromString(FilterPulldownStructTypeNames)
				)
			);

			PulldownListGenerator = PreChangePulldownListGenerator;
		}

		if (PreChangePulldownListGenerator != PulldownListGenerator)
		{
			PulldownBuilder::FPulldownContentsLoader::OnPulldownContentsSourceChanged.Broadcast(this);
		}
	}
	
	PreChangePulldownListGenerator = nullptr;
}

void UPulldownContents::HandleOnAssetEditorOpened(UObject* OpenedAsset)
{
	if (OpenedAsset->GetName() == GetName())
	{
		ModifyPulldownListGenerator();
	}
}

void UPulldownContents::HandleOnPackageReloaded(EPackageReloadPhase ReloadPhase, FPackageReloadedEvent* ReloadedEvent)
{
	if (ReloadPhase == EPackageReloadPhase::PostBatchPostGC)
	{
		UnregisterDetailCustomization();
		RegisterDetailCustomization();
	}
}

#undef LOCTEXT_NAMESPACE
