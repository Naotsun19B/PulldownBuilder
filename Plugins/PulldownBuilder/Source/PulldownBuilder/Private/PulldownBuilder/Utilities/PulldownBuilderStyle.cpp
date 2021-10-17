// Copyright 2021 Naotsun. All Rights Reserved.

#include "PulldownBuilderStyle.h"
#include "Styling/SlateStyleRegistry.h"

namespace PulldownBuilder
{
	namespace IconSize
	{
		static const FVector2D Icon16x16(16.0f, 16.0f);
		static const FVector2D Icon64x64(64.0f, 64.0f);
	}
	
	FPulldownBuilderStyle::FPulldownBuilderStyle()
		: FSlateStyleSet(TEXT("PulldownContentsStyle"))
	{
	}

#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(Instance->RootToContentDir(TEXT(RelativePath), TEXT(".png")), __VA_ARGS__)
	
	void FPulldownBuilderStyle::Register()
	{
		const FString StyleContentRoot = FPaths::ConvertRelativePathToFull(FPaths::Combine(
			FPaths::EngineContentDir(),
			TEXT("Editor"),
			TEXT("Slate"),
			TEXT("Icons"),
			TEXT("AssetIcons")
		));
		
		Instance = MakeShared<FPulldownBuilderStyle>();
		Instance->SetContentRoot(StyleContentRoot);
		Instance->SetCoreContentRoot(StyleContentRoot);
		
		Instance->Set(TEXT("ClassIcon.PulldownContents"), new IMAGE_BRUSH("UserDefinedEnum_16x", IconSize::Icon16x16));
		Instance->Set(TEXT("ClassThumbnail.PulldownContents"), new IMAGE_BRUSH("UserDefinedEnum_64x", IconSize::Icon64x64));
		
		FSlateStyleRegistry::RegisterSlateStyle(*Instance);
	}

#undef IMAGE_BRUSH

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
