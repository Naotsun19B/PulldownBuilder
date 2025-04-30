// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilderTest/PulldownBuilderTestFunctionLibrary.h"

void UPulldownBuilderTestFunctionLibrary::Log(const FText& Message)
{
#if WITH_EDITOR
	static const TArray<FColor, TInlineAllocator<3>> MessageColors =
	{
		FColor::Green, FColor::Red, FColor::Cyan
	};
	static uint64 Index = 0;
	GEngine->AddOnScreenDebugMessage(
		Index,
		20.f,
		MessageColors[Index % 3],
		Message.ToString()
	);
	Index++;

	DEFINE_LOG_CATEGORY_STATIC(LogPulldownBuilderTest, Log, All);
	UE_LOG(LogPulldownBuilderTest, Log, TEXT("%s"), *Message.ToString());
#else
	static const FString Filename = (FPaths::ProjectDir() / TEXT("PulldownBuilderTestLog.txt"));
	static bool bDoOnce = false;
	TArray<FString> Lines;
	if (FPaths::FileExists(Filename))
	{
		FFileHelper::LoadFileToStringArray(Lines, *Filename);
		
		if (!bDoOnce)
		{
			Lines.Empty();
			bDoOnce = true;
		}
	}
	Lines.Add(Message.ToString());
	FFileHelper::SaveStringArrayToFile(Lines, *Filename, FFileHelper::EEncodingOptions::ForceUTF8);
#endif
}
