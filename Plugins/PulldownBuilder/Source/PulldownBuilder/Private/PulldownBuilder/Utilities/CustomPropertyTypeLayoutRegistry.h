// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IPropertyTypeCustomization;

namespace PulldownBuilder
{
	/**
	 * An interface class that registers-unregisters the custom details panel for a struct.
	 */
	struct PULLDOWNBUILDER_API ICustomPropertyTypeLayoutRegistry
	{
	public:
		// Constructor.
		explicit ICustomPropertyTypeLayoutRegistry(const FString& InPropertyTypeName);

		// Destructor.
		virtual ~ICustomPropertyTypeLayoutRegistry();

		// Returns the cache of type name that utilize this details panel.
		const FString& GetPropertyTypeName() const;

	protected:
		// Returns an instance of the class in the custom details panel.
		virtual TSharedRef<IPropertyTypeCustomization> MakeInstance() = 0;

	protected:
		// The cache of type name that utilize this details panel.
		FString PropertyTypeName;
	};

	/**
	 * A class template that registers-unregisters the custom details panel for a struct.
	 */
	template<typename TStructType, class TPropertyTypeCustomization>
	struct TCustomPropertyTypeLayoutRegistry : public ICustomPropertyTypeLayoutRegistry
	{
		static_assert(TIsDerivedFrom<TPropertyTypeCustomization, IPropertyTypeCustomization>::IsDerived, "This implementation wasn't tested for a filter that isn't a child of IPropertyTypeCustomization.");
		
	public:
		// Constructor.
		TCustomPropertyTypeLayoutRegistry()
			: ICustomPropertyTypeLayoutRegistry(GetNameSafe(TStructType::StaticStruct()))
		{
		}

	protected:
		// ICustomPropertyTypeLayoutRegistry interface.
		virtual TSharedRef<IPropertyTypeCustomization> MakeInstance() override
		{
			return MakeShared<TPropertyTypeCustomization>();
		}
		// End of ICustomPropertyTypeLayoutRegistry interface.
	};
}
