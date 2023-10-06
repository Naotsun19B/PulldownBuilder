// Copyright 2021-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/DetailCustomizations/PulldownStructDetail.h"

struct FPulldownRow;

namespace PulldownBuilder
{
	/**
	 * A detail customization class that applied to structures that inherit from FPulldownStructBase.
	 */
	class PULLDOWNBUILDER_API FPreviewPulldownStructDetail : public FPulldownStructDetail
	{
	public:
		// Registers-Unregisters and instantiate this customization.
		static void Register();
		static void Unregister();
		static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	private:
		// FPulldownStructDetail interface.
		virtual TArray<TSharedPtr<FPulldownRow>> GenerateSelectableValues() override;
		virtual UPulldownContents* GetRelatedPulldownContents() const override;
		// End of FPulldownStructDetail interface.

	private:
		// The cache of type names for properties that utilize this details panel.
		static FString CachedPropertyTypeName;
	};
}
