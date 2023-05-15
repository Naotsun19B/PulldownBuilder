// Copyright 2022 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilderNotificationSeverity.generated.h"

/**
 * An enum to decide from which severity to notify.
 */
UENUM()
enum class EPulldownBuilderNotificationSeverity : uint8
{
	Info,
	Warning,
	Error,
	
	Ignore,
};
