// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "TestPulldown3PulldownListGenerator.h"
#include "PulldownBuilderTest/TestPulldown.h"
#include "UObject/UObjectIterator.h"

FPulldownRows UTestPulldown3PulldownListGenerator::GetPulldownRows(
	const TArray<UObject*>& OuterObjects,
	const FStructContainer& StructInstance
) const
{
	FPulldownRows PulldownRows;

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
				FPulldownRow PulldownRow(Path.GetAssetName(), FText::FromString(Path.ToString()));
				
				if (!PulldownRows.HasDefaultRow())
				{
					PulldownRow.bIsDefaultValue = true;
				}
				
				PulldownRows.Add(PulldownRow);
			}
		}
	}
	
	return PulldownRows;
}

bool UTestPulldown3PulldownListGenerator::IsEnableCustomDefaultValue() const
{
	return true;
}
