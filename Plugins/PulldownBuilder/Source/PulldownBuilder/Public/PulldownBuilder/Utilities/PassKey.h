// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

#if UE_5_00_OR_LATER
namespace PulldownBuilder
{
	/**
	 * A class template for generic passkey that used in the passkey idiom.
	 */
	template <class TAuthorized>
	struct TPassKey
	{
	private:
		friend TAuthorized;
		TPassKey() {}

	public:
		TPassKey(const TPassKey&) = delete;
		TPassKey& operator=(const TPassKey&) = delete;
	};

	/**
	 * A class template that determines whether a variable-length template argument contains a particular type.
	 */
	template <typename TTest, typename... TList>
	struct TContains;
	template<typename TTest, typename TFirst, typename... TRest>
	struct TContains<TTest, TFirst, TRest...> : std::conditional_t<std::is_same_v<TTest, TFirst>, std::true_type, TContains<TTest, TRest...>> {};
	template <typename TTest>
	struct TContains<TTest> : std::false_type {};

	/**
	 * A class template for passkey that allows access to multiple classes used in the passkey idiom.
	 */
	template<class... TAuthorizedList>
	struct TPassKeys
	{
	public:
		template <class TAuthorized, typename TEnableIf<TContains<TAuthorized, TAuthorizedList...>::value, std::nullptr_t>::Type = nullptr>
		TPassKeys(TPassKey<TAuthorized>) {}
		TPassKeys(const TPassKeys&) = delete;
		TPassKeys& operator=(const TPassKeys&) = delete;
	};

#if UE_5_04_OR_LATER
	template<typename T>
	using TRemoveConstVolatileReference_T = std::remove_cvref_t<T>;
#else
	template<typename T>
	using TRemoveConstVolatileReference_T = std::remove_cv_t<std::remove_reference_t<T>>;
#endif
}

/**
 * A macro that simplifies arguments when calling functions from classes that are allowed in the passkey idiom.
 * Cannot be used in static functions because the type is determined from this.
 */
#define PULLDOWN_BUILDER_PASS_KEY PulldownBuilder::TPassKey<PulldownBuilder::TRemoveConstVolatileReference_T<decltype(*this)>>()
#else
#define PULLDOWN_BUILDER_PASS_KEY
#endif
