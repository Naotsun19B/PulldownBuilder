// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/UObjectIterator.h"
#include "FileHelpers.h"
#include "RowNameUpdaterBase.generated.h"

class UPulldownContents;

/**
 * A base class that replaces an already used value with a new one when the display string list of PulldownContents is updated.
 */
UCLASS(Abstract)
class PULLDOWNBUILDER_API URowNameUpdaterBase : public UObject
{
	GENERATED_BODY()

public:
	// Updates all pull-down struct that reference owner PulldownContents asset.
	static void UpdateRowNames(
		UPulldownContents* PulldownContents,
		const FName& PreChangeSelectedValue,
		const FName& PostChangeSelectedValue
	);

protected:
	// The actual update process.
	// Performs processing according to the asset type in the inherited class.
	virtual void UpdateRowNamesInternal(
		UPulldownContents* PulldownContents,
		const FName& PreChangeSelectedValue,
		const FName& PostChangeSelectedValue
	);

	// Returns whether to perform the upload process.
	virtual bool ShouldUpdateProcess() const;
	
	// Updates the FPulldownStructBase in the member variable of the specified class.
	virtual bool UpdateMemberVariables(
		UClass* Class,
		UPulldownContents* PulldownContents,
		const FName& PreChangeSelectedValue,
		const FName& PostChangeSelectedValue
	);
	virtual bool UpdateMemberVariables(
		const UStruct* Struct,
		void* ContainerPtr,
		UPulldownContents* PulldownContents,
		const FName& PreChangeSelectedValue,
		const FName& PostChangeSelectedValue
	);

	// Enumerates objects of the specified class.
	template<class TAsset>
	void EnumerateAssets(TFunction<bool(TAsset* Asset)> Predicate)
	{
		for (TAsset* Asset : TObjectRange<TAsset>())
		{
			if (!IsValid(Asset))
			{
				continue;
			}

			UPackage* Package = Asset->GetOutermost();
			if (!IsValid(Package))
			{
				continue;
			}
			
			if (Predicate(Asset))
			{
				Package->MarkPackageDirty();
				UEditorLoadingAndSavingUtils::SavePackages(
					TArray<UPackage*>{ Package },
					true
				);
			}
		}
	}
};
