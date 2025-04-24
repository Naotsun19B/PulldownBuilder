// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownBuilder/Types/PulldownRow.h"

/**
 * A struct of the data that appears in the list in the pull-down menu.
 */
struct PULLDOWNBUILDER_API FPulldownRows
{
public:
	// An empty instance containing only None.
	static const FPulldownRows Empty;
	
public:
	// Constructor.
	FPulldownRows();
	explicit FPulldownRows(const TArray<FPulldownRow>& BlueprintValue);

	// Adds a new item to the end of the array, possibly reallocating the whole array to fit.
	void Add(const FPulldownRow& NewRow);
	void Add(const TSharedPtr<FPulldownRow>& NewRow);

	// Empties the array. It calls the destructors on held items if needed.
	void Reset();

	// Finds an element which matches a predicate functor.
	TSharedPtr<FPulldownRow> FindByPredicate(const TFunction<bool(const TSharedPtr<FPulldownRow>& Row)>& Predicate) const;

	// Returns number of elements in array.
	int32 Num() const;

	// Tests if index is valid, i.e. greater than or equal to zero, and less than the number of elements in the array.
	bool IsValidIndex(const int32 Index) const;

	// Returns whether there is only one None row held.
	bool IsEmpty() const;

	// Returns whether the default pull-down row of the pull-down list that this struct constitutes exists.
	bool HasDefaultRow() const;

	// Returns the default pull-down row of the pull-down list that this struct constitutes.
	TSharedPtr<FPulldownRow> GetDefaultRow() const;

	// Sets the default pull-down row for the pull-down list that this struct constitutes.
	// Marks row that match the predicate as default values.
	void SetDefaultRow(const TFunction<bool(const TSharedRef<FPulldownRow>& Row)>& Predicate);

private:
	// Internal implementation of SetDefaultRow.
	void SetDefaultRowInternal(const int32 NewDefaultRowIndex);

public:
	// Returns a pointer to an array used internally by this struct.
	TArray<TSharedPtr<FPulldownRow>>* operator&();
	
	// Operator overloading to make it possible to be treated as an array.
	TSharedPtr<FPulldownRow> operator[](const int32 Index) const;
	
	/**
	 * DO NOT USE DIRECTLY
	 * STL-like iterators to enable range-based for loop support.
	 */
	using FRangedForIteratorType				= TArray<TSharedPtr<FPulldownRow>>::RangedForIteratorType;
	using FRangedForConstIteratorType		= TArray<TSharedPtr<FPulldownRow>>::RangedForConstIteratorType;
	using FRangedForReverseIteratorType		= TArray<TSharedPtr<FPulldownRow>>::RangedForReverseIteratorType;
	using FRangedForConstReverseIteratorType	= TArray<TSharedPtr<FPulldownRow>>::RangedForConstReverseIteratorType;
#if TARRAY_RANGED_FOR_CHECKS
	FORCEINLINE FRangedForIteratorType             begin ()       { return FRangedForIteratorType            (Num(), Rows.GetData()); }
	FORCEINLINE FRangedForConstIteratorType        begin () const { return FRangedForConstIteratorType       (Num(), Rows.GetData()); }
	FORCEINLINE FRangedForIteratorType             end   ()       { return FRangedForIteratorType            (Num(), Rows.GetData() + Num()); }
	FORCEINLINE FRangedForConstIteratorType        end   () const { return FRangedForConstIteratorType       (Num(), Rows.GetData() + Num()); }
	FORCEINLINE FRangedForReverseIteratorType      rbegin()       { return FRangedForReverseIteratorType     (Num(), Rows.GetData() + Num()); }
	FORCEINLINE FRangedForConstReverseIteratorType rbegin() const { return FRangedForConstReverseIteratorType(Num(), Rows.GetData() + Num()); }
	FORCEINLINE FRangedForReverseIteratorType      rend  ()       { return FRangedForReverseIteratorType     (Num(), Rows.GetData()); }
	FORCEINLINE FRangedForConstReverseIteratorType rend  () const { return FRangedForConstReverseIteratorType(Num(), Rows.GetData()); }
#else
	FORCEINLINE FRangedForIteratorType             begin ()       { return                                    Rows.GetData(); }
	FORCEINLINE FRangedForConstIteratorType        begin () const { return                                    Rows.GetData(); }
	FORCEINLINE FRangedForIteratorType             end   ()       { return                                    Rows.GetData() + Num(); }
	FORCEINLINE FRangedForConstIteratorType        end   () const { return                                    Rows.GetData() + Num(); }
	FORCEINLINE FRangedForReverseIteratorType      rbegin()       { return FRangedForReverseIteratorType     (Rows.GetData() + Num()); }
	FORCEINLINE FRangedForConstReverseIteratorType rbegin() const { return FRangedForConstReverseIteratorType(Rows.GetData() + Num()); }
	FORCEINLINE FRangedForReverseIteratorType      rend  ()       { return FRangedForReverseIteratorType     (Rows.GetData()); }
	FORCEINLINE FRangedForConstReverseIteratorType rend  () const { return FRangedForConstReverseIteratorType(Rows.GetData()); }
#endif
	
private:
	// A list of pull-down rows that make up the list of pull-down struct.
	TArray<TSharedPtr<FPulldownRow>> Rows;

	// The default pull-down row index.
	int32 DefaultRowIndex;
};
