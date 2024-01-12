// Copyright 2021-2024 Naotsun. All Rights Reserved.

#include "TestPulldown3PulldownListGenerator.h"
#include "PulldownBuilderTest/TestPulldown.h"
#include "UObject/UObjectIterator.h"

TArray<TSharedPtr<FPulldownRow>> UTestPulldown3PulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	TArray<TSharedPtr<FPulldownRow>> PulldownRows;

	if (auto* TestPulldown3 = StructInstance.Get<FTestPulldown3>())
	{
		const UClass* AssetType = TestPulldown3->AssetType;
		if (IsValid(AssetType))
		{
			for (const auto* Asset : TObjectRange<UObject>())
			{
				if (!IsValid(Asset))
				{
					continue;
				}
				
				if (Asset->GetClass() != AssetType)
				{
					continue;
				}

				if (!Asset->IsAsset())
				{
					continue;
				}

				const FSoftObjectPath Path(Asset);
				PulldownRows.Add(
					MakeShared<FPulldownRow>(
						Path.GetAssetName(),
						FText::FromString(Path.ToString())
					)
				);
			}
		}
	}
	
	return PulldownRows;
}
