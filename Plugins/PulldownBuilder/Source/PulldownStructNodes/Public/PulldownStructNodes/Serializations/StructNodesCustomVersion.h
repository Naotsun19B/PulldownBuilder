// Copyright 2021-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"

namespace PulldownBuilder
{
	/**
	 * Custom serialization version for the PulldownStructNodes module.
	 * Used by UK2Node_SwitchPulldownStruct / UK2Node_SwitchNativeLessPulldownStruct to keep
	 * binary compatibility with assets saved before the runtime field serialization was fixed.
	 */
	struct PULLDOWNSTRUCTNODES_API FStructNodesCustomVersion
	{
	public:
		enum Type
		{
			// State before this custom version was introduced. Legacy assets are tagged with this value.
			BeforeCustomVersionWasAdded = 0,

			// Runtime fields (SelectedValues / DisplayTexts / PulldownContents) are now serialized.
			// Before this version, the guard condition was inverted (&&) and the fields were never written.
			AddedSerializeOfRuntimeFields,

			// Marker for the value placed immediately after the last valid version.
			VersionPlusOne,

			// The latest version. Used when registering with the engine's custom version registry.
			LatestVersion = VersionPlusOne - 1
		};

		// The GUID that uniquely identifies this custom version in the engine's custom version registry.
		static const FGuid GUID;

	private:
		FStructNodesCustomVersion() = default;
	};
}
