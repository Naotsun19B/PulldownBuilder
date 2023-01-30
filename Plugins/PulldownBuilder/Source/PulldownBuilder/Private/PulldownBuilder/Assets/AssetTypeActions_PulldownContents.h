// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

namespace PulldownBuilder
{
	/**
	 * A class that defines the operations that can be performed on an PulldownContents and
	 * information about the PulldownContents.
	 */
	class PULLDOWNBUILDER_API FAssetTypeActions_PulldownContents : public FAssetTypeActions_Base
	{
	public:
		// Register-Unregister.
		static void Register();
		static void Unregister();
	
		// FAssetTypeActions_Base interface.
		virtual FText GetName() const override;
		virtual FColor GetTypeColor() const override;
		virtual UClass* GetSupportedClass() const override;
		virtual uint32 GetCategories() override;
		virtual bool CanLocalize() const override;
		// End of FAssetTypeActions_Base interface.

	private:
		// An instance of this style class.
		static TSharedPtr<FAssetTypeActions_PulldownContents> Instance;
	};
}
