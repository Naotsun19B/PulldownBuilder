// Copyright 2021-2024 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Utilities/PulldownBuilderStyle.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleRegistry.h"
#if UE_5_00_OR_LATER
#include "Styling/SlateStyleMacros.h"
#include "Styling/CoreStyle.h"
#endif
#include "Misc/Paths.h"

namespace PulldownBuilder
{
#if !UE_5_00_OR_LATER
	namespace CoreStyleConstants
	{
		static const FVector2D Icon16x16(16.f, 16.f);
		static const FVector2D Icon64x64(64.f, 64.f);
	}
#endif
	
	FPulldownBuilderStyle::FPulldownBuilderStyle()
		: FSlateStyleSet(TEXT("PulldownContentsStyle"))
	{
	}

#if !UE_5_00_OR_LATER
#define CORE_IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( RootToCoreContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#endif
	
	void FPulldownBuilderStyle::RegisterInternal()
	{
		SetCoreContentRoot(FPaths::EngineContentDir());
		{
			FString StyleContentRoot;
			{
				const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(Global::PluginName.ToString());
				check(Plugin.IsValid());
				StyleContentRoot = FPaths::ConvertRelativePathToFull(
					Plugin->GetBaseDir() / TEXT("Resources") / TEXT("Icons")
				);
			}
			SetContentRoot(StyleContentRoot);
		}

		Set(
			TEXT("ClassIcon.PulldownContents"),
			new CORE_IMAGE_BRUSH("Editor/Slate/Icons/AssetIcons/UserDefinedEnum_16x", CoreStyleConstants::Icon16x16)
		);
		Set(
			TEXT("ClassThumbnail.PulldownContents"),
			new CORE_IMAGE_BRUSH("Editor/Slate/Icons/AssetIcons/UserDefinedEnum_64x", CoreStyleConstants::Icon64x64)
		);
	}

#if !UE_5_00_OR_LATER
#undef CORE_IMAGE_BRUSH
#endif
	
	void FPulldownBuilderStyle::Register()
	{
		Instance = MakeUnique<FPulldownBuilderStyle>();
		Instance->RegisterInternal();
		FSlateStyleRegistry::RegisterSlateStyle(*Instance);
	}

	void FPulldownBuilderStyle::Unregister()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*Instance);
		Instance.Reset();
	}

	const ISlateStyle& FPulldownBuilderStyle::Get()
	{
		check(Instance.IsValid()); // Don't call before Register is called or after Unregister is called.
		return *Instance.Get();
	}

	TUniquePtr<FPulldownBuilderStyle> FPulldownBuilderStyle::Instance = nullptr;
}
