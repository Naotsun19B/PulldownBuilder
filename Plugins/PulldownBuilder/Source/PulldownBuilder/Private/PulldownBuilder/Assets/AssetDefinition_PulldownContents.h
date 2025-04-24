// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"
#if ENABLE_ASSET_DEFINITION
#include "AssetDefinitionDefault.h"
#endif

#if ENABLE_ASSET_DEFINITION
// UAssetDefinitionDefault does not define a constructor that receives an object initializer,
// so change it so that the object initializer is not passed locally to the superclass.
#undef RELAY_CONSTRUCTOR
#define RELAY_CONSTRUCTOR(TClass, TSuperClass) TClass(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : TSuperClass() {}
#endif

namespace PulldownBuilder
{
#if ENABLE_ASSET_DEFINITION
	/**
	 * A class that defines the operations that can be performed on an PulldownContents and information about the PulldownContents.
	 */
	class UAssetDefinition_PulldownContents : public UAssetDefinitionDefault
	{
		DECLARE_CASTED_CLASS_INTRINSIC_WITH_API(UAssetDefinition_PulldownContents, UAssetDefinitionDefault, CLASS_None, TEXT("/Script/PulldownBuilder"), CASTCLASS_None, PULLDOWNBUILDER_API)
		
	public:
		// UAssetDefinition interface.
		virtual FText GetAssetDisplayName() const override;
		virtual FLinearColor GetAssetColor() const override;
		virtual TSoftClassPtr<UObject> GetAssetClass() const override;
		virtual TConstArrayView<FAssetCategoryPath> GetAssetCategories() const override;
		virtual FAssetSupportResponse CanLocalize(const FAssetData& InAsset) const override;
		// End of UAssetDefinition interface.
	};
#endif
}
