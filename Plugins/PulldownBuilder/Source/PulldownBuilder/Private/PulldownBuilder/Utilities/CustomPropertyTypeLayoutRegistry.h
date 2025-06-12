// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class IPropertyTypeCustomization;
class UPulldownContents;

namespace PulldownBuilder
{
	/**
	 * An interface class that registers-unregisters the custom details panel for a struct.
	 */
	class PULLDOWNBUILDER_API ICustomPropertyTypeLayoutRegistry
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
	class TCustomPropertyTypeLayoutRegistry : public ICustomPropertyTypeLayoutRegistry
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

	/**
	 * A class that notifies the property editor module that the details panel customization has changed.
	 */
	class FCustomizationModuleChangedNotificator
	{
	public:
		// Registers a details panel customization notificator class.
		static void Register();
	
	private:
		// Unregisters a details panel customization notificator class.
		static void HandleOnEnginePreExit();
		
		// Called when all PulldownContents assets has been loaded.
		static void HandleOnAllPulldownContentsLoaded();

		// Called when registers the struct set for PulldownContents in detail customization.
		static void HandleOnDetailCustomizationRegistered(const UPulldownContents* ModifiedPulldownContents);

		// Called when unregisters the struct set for PulldownContents in detail customization.
		static void HandleOnDetailCustomizationUnregistered(const UPulldownContents* ModifiedPulldownContents);

	public:
		// Notifies the property editor module that the details panel customization has changed.
		static void NotifyCustomizationModuleChanged();

	private:
		// The handles for event called when registers-unregisters the struct set for PulldownContents in detail customization.
		static FDelegateHandle OnEnginePreExitHandle;
		static FDelegateHandle OnAllPulldownContentsLoadedHandle;
		static FDelegateHandle OnDetailCustomizationRegisteredHandle;
		static FDelegateHandle OnDetailCustomizationUnregisteredHandle;
	};
}
