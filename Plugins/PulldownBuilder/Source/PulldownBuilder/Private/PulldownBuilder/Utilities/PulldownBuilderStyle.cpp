// Copyright 2021-2023 Naotsun. All Rights Reserved.

#include "PulldownBuilderStyle.h"
#include "Styling/SlateStyleRegistry.h"
#include "Styling/SlateStyleMacros.h"

namespace PulldownBuilder
{
	FPulldownBuilderStyle::FPulldownBuilderStyle()
		: FSlateStyleSet(TEXT("PulldownContentsStyle"))
	{
	}

	void FPulldownBuilderStyle::RegisterInternal()
	{
		SetCoreContentRoot(FPaths::EngineContentDir());
		SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor"));

		Set(
			TEXT("ClassIcon.PulldownContents"),
			new IMAGE_BRUSH("Slate/Icons/AssetIcons/UserDefinedEnum_16x", CoreStyleConstants::Icon16x16)
		);
		Set(
			TEXT("ClassThumbnail.PulldownContents"),
			new IMAGE_BRUSH("Slate/Icons/AssetIcons/UserDefinedEnum_64x", CoreStyleConstants::Icon64x64)
		);
	}
	
	void FPulldownBuilderStyle::Register()
	{
		Instance = MakeShared<FPulldownBuilderStyle>();
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

	TSharedPtr<FPulldownBuilderStyle> FPulldownBuilderStyle::Instance = nullptr;
}
