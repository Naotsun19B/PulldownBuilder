// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

/**
 * A class that manages the slate icon used by this plugin.
 */
class PULLDOWNBUILDER_API FPulldownBuilderStyle : public FSlateStyleSet
{	
public:
	// Constructor.
	FPulldownBuilderStyle() : FSlateStyleSet(TEXT("PulldownContentsStyle"))
	{
		SetContentRoot(FPaths::ConvertRelativePathToFull(FPaths::Combine(
			FPaths::EngineContentDir(),
			TEXT("Editor"),
			TEXT("Slate"),
			TEXT("Icons"),
			TEXT("AssetIcons")
		)));

		Set(TEXT("ClassIcon.PulldownContents"), new FSlateImageBrush(
			RootToContentDir(
				TEXT("/UserDefinedEnum_16x"),
				TEXT(".png")
			), FVector2D(16.f, 16.f)
		));
		
		Set(TEXT("ClassThumbnail.PulldownContents"), new FSlateImageBrush(
			RootToContentDir(
				TEXT("/UserDefinedEnum_64x"),
				TEXT(".png")
			), FVector2D(64.f, 64.f)
		));
		
		FSlateStyleRegistry::RegisterSlateStyle(*this);
	}

	// Destructor.
	~FPulldownBuilderStyle()
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*this);
	}

	// Register-Unregister and instance getter this class.
	static void Register()
	{
		Instance = MakeShared<FPulldownBuilderStyle>();
	}
	
	static void Unregister()
	{
		Instance.Reset();
	}
	
	static FPulldownBuilderStyle& Get()
	{
		check(Instance.IsValid()); // Don't call before Register is called or after Unregister is called.
		return *Instance.Get();
	}

private:
	// An instance of this style class.
	static TSharedPtr<FPulldownBuilderStyle> Instance;
};

TSharedPtr<FPulldownBuilderStyle> FPulldownBuilderStyle::Instance = nullptr;
