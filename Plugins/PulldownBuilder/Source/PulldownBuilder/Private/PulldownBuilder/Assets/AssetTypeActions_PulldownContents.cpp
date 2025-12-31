// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Assets/AssetTypeActions_PulldownContents.h"
#if ENABLE_ASSET_TYPE_ACTIONS
#include "PulldownBuilder/Assets/PulldownContents.h"
#include "AssetToolsModule.h"
#endif

#define LOCTEXT_NAMESPACE "AssetTypeActions_PulldownContents"

namespace PulldownBuilder
{
#if ENABLE_ASSET_TYPE_ACTIONS
	TSharedPtr<FAssetTypeActions_PulldownContents> FAssetTypeActions_PulldownContents::Instance = nullptr;

	void FAssetTypeActions_PulldownContents::Register()
	{
		Instance = MakeShared<FAssetTypeActions_PulldownContents>();
		FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(Instance.ToSharedRef());
	}

	void FAssetTypeActions_PulldownContents::Unregister()
	{
		if (Instance.IsValid() && FAssetToolsModule::IsModuleLoaded())
		{
			FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(Instance.ToSharedRef());
			Instance.Reset();
		}
	}

	FText FAssetTypeActions_PulldownContents::GetName() const
	{
		return LOCTEXT("AssetName", "Pulldown Contents");
	}

	FColor FAssetTypeActions_PulldownContents::GetTypeColor() const
	{
		return FColor(103, 206, 218);
	}

	UClass* FAssetTypeActions_PulldownContents::GetSupportedClass() const
	{
		return UPulldownContents::StaticClass();
	}

	uint32 FAssetTypeActions_PulldownContents::GetCategories()
	{
		return EAssetTypeCategories::Misc;
	}

	bool FAssetTypeActions_PulldownContents::CanLocalize() const
	{
		return false;
	}
#endif
}

#undef LOCTEXT_NAMESPACE
