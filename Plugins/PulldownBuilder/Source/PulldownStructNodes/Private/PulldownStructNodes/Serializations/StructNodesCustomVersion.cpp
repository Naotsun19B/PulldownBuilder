// Copyright 2021-2026 Naotsun. All Rights Reserved.

#include "PulldownStructNodes/Serializations/StructNodesCustomVersion.h"
#include "Serialization/CustomVersion.h"

namespace PulldownBuilder
{
	// Randomly generated GUID. Do not change once shipped, otherwise saved assets will not match.
	const FGuid FStructNodesCustomVersion::GUID(0x3A7F2D91, 0x4B85E6C8, 0x9D32F4A6, 0x5E1C7B83);

	namespace StructNodesCustomVersionPrivate
	{
		// Registers the custom version with the engine at module load time so that
		// loaders / savers know about FStructNodesCustomVersion::LatestVersion.
		// Wrapped in a file-local named namespace to avoid Unity build symbol collisions
		// while complying with the no-anonymous-namespace coding rule.
		FCustomVersionRegistration GRegisterCustomVersion(
			FStructNodesCustomVersion::GUID,
			FStructNodesCustomVersion::LatestVersion,
			TEXT("PulldownStructNodesVersion")
		);
	}
}
