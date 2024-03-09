// Copyright 2021-2024 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * In the editor, the object associated with the SelectedValue is the asset that is pulled down so that the asset can be opened from the reference viewer.
 * The purpose is to open the asset from the reference viewer, so it doesn't do anything at runtime.
 */
#if WITH_EDITOR
#define PULLDOWN_STRUCT_POST_SERIALIZE \
	if (Ar.IsSaving()) \
	{ \
		MarkSearchableName(Ar); \
	}
#else
#define PULLDOWN_STRUCT_POST_SERIALIZE
#endif

/**
 * A macro defined to use basic functionality in the pull-down struct.
 * Make it possible to use the FPulldownStructBase constructor and check the SelectedValue in the reference viewer.
 */
#define SETUP_PULLDOWN_STRUCT() SETUP_PULLDOWN_STRUCT_WITH_SUPER_STRUCT(FPulldownStructBase)
#define SETUP_PULLDOWN_STRUCT_WITH_SUPER_STRUCT(SuperStructName) \
	public: \
		using SuperStructName::SuperStructName; \
		void PostSerialize(const FArchive& Ar) \
		{ \
			PostSerialize_Implementation(Ar); \
			PULLDOWN_STRUCT_POST_SERIALIZE \
		}

/**
 * A macro that allows you to check the SelectedValue in the reference viewer.
 * If there are other custom aspects, define a flag after the structure name.
 */
#define SETUP_PULLDOWN_STRUCT_OPS(StructName, ...) \
	template<> \
	struct TStructOpsTypeTraits<StructName> : public TStructOpsTypeTraitsBase2<StructName> \
	{ \
		enum \
		{ \
			WithPostSerialize = true, \
			__VA_ARGS__ \
		}; \
	};
