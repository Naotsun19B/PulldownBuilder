// Copyright 2021-2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/UObjectIterator.h"
#include "RowNameUpdaterBase.generated.h"

class UPulldownContents;

/**
 * A base class that replaces an already used value with a new one
 * when the display string list of PulldownContents is updated.
 */
UCLASS()
class PULLDOWNBUILDER_API URowNameUpdaterBase : public UObject
{
	GENERATED_BODY()

public:
	// Update all FPulldownStructBase that reference owner PulldownContents asset.
	static void UpdateRowNames(
		UPulldownContents* PulldownContents,
		const FName& PreChangeName,
		const FName& PostChangeName
	);

protected:
	// The actual upload process. Perform processing according to the asset type in the inherited class.
	virtual void UpdateRowNamesInternal(
		UPulldownContents* PulldownContents,
		const FName& PreChangeName,
		const FName& PostChangeName
	);

	// Returns whether to perform the upload process.
	virtual bool ShouldUpdateProcess() const;
	
	// Update the FPulldownStructBase in the member variable of the specified class.
	virtual bool UpdateMemberVariables(
		UClass* Class,
		UPulldownContents* PulldownContents,
		const FName& PreChangeName,
		const FName& PostChangeName
	);
	virtual bool UpdateMemberVariables(
		const UStruct* Struct,
		void* ContainerPtr,
		UPulldownContents* PulldownContents,
		const FName& PreChangeName,
		const FName& PostChangeName
	);

	// Scans objects of the specified class.
	template<class TAsset>
	void EnumerateAssets(TFunction<bool(TAsset* Asset)> Predicate)
	{
		for (TAsset* Asset : TObjectRange<TAsset>())
		{
			if (!IsValid(Asset))
			{
				continue;
			}

			if (Predicate(Asset))
			{
				Asset->Modify();
			}
		}
	}
};
