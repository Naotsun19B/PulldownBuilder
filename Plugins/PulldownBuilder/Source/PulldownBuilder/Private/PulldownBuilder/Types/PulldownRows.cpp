// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownBuilder/Types/PulldownRows.h"
#include "PulldownStruct/PulldownBuilderGlobals.h"

const FPulldownRows FPulldownRows::Empty = FPulldownRows();

FPulldownRows::FPulldownRows(const int32 NumOfReserves /*= INDEX_NONE */)
	: DefaultRowIndex(INDEX_NONE)
{
	if (NumOfReserves > 0)
	{
		Rows.Reserve(NumOfReserves + 1);
	}
	
	// Be sure to put "None" at the beginning because it may not be selected or the list may be empty.
	Rows.Insert(MakeShared<FPulldownRow>(), NoneIndex);
}

FPulldownRows::FPulldownRows(const TArray<FPulldownRow>& BlueprintValue)
	: FPulldownRows()
{
	for (const auto& PulldownRowFromBlueprint : BlueprintValue)
	{
		Add(PulldownRowFromBlueprint);
	}
}

void FPulldownRows::Add(const FPulldownRow& NewRow)
{
	Add(MakeShared<FPulldownRow>(NewRow));
}

void FPulldownRows::Add(const TSharedPtr<FPulldownRow>& NewRow)
{
	check(NewRow.IsValid());
	
	auto EqualSelectedValue = [&NewRow](const TSharedPtr<FPulldownRow>& Row) -> bool
	{
		check(Row.IsValid());
		return Row->SelectedValue.Equals(NewRow->SelectedValue);
	};
	if (Rows.ContainsByPredicate(EqualSelectedValue))
	{
		UE_LOG(LogPulldownBuilder, Warning, TEXT("The row (%s) you tried to add already exists. You can not add a PulldownRow with the same SelectedValue."), *NewRow->SelectedValue);
		return;
	}

	Rows.Add(NewRow);

	if (NewRow->bIsDefaultValue)
	{
		SetDefaultRowInternal(Rows.Num() - 1);
	}
}

void FPulldownRows::Reset()
{
	Rows.Reset();
}

TSharedPtr<FPulldownRow> FPulldownRows::FindByPredicate(const TFunction<bool(const TSharedPtr<FPulldownRow>& Row)>& Predicate) const
{
	if (const TSharedPtr<FPulldownRow>* FoundRow = Rows.FindByPredicate(Predicate))
	{
		return *FoundRow;
	}

	return nullptr;
}

int32 FPulldownRows::Num() const
{
	return Rows.Num();
}

bool FPulldownRows::IsValidIndex(const int32 Index) const
{
	return Rows.IsValidIndex(Index);
}

bool FPulldownRows::IsEmpty() const
{
	return (Num() == Empty.Num());
}

bool FPulldownRows::HasDefaultRow() const
{
	return (DefaultRowIndex != INDEX_NONE);
}

TSharedPtr<FPulldownRow> FPulldownRows::GetDefaultRow() const
{
	if (DefaultRowIndex == INDEX_NONE)
	{
		return nullptr;
	}

	check(Rows.IsValidIndex(DefaultRowIndex));
	return Rows[DefaultRowIndex];
}

void FPulldownRows::SetDefaultRow(const TFunction<bool(const TSharedRef<FPulldownRow>& Row)>& Predicate)
{
	for (int32 Index = 0; Index < Rows.Num(); Index++)
	{
		const TSharedPtr<FPulldownRow>& Row = Rows[Index];
		check(Row.IsValid());
		
		if (Predicate(Row.ToSharedRef()))
		{
			SetDefaultRowInternal(Index);
			break;
		}
	}
}

void FPulldownRows::SetNonExistentValue(const FName& SelectedValue, const FText& DisplayText)
{
	auto ContainsPredicate = [&](const TSharedPtr<FPulldownRow>& Row) -> bool
	{
		return (Row.IsValid() && (Row->SelectedValue == SelectedValue));
	};
	if (Rows.ContainsByPredicate(ContainsPredicate))
	{
		return;
	}
	
	const TSharedPtr<FPulldownRow> NonExistentValue = MakeShared<FPulldownRow>();
	check(NonExistentValue.IsValid());
	NonExistentValue->SelectedValue = SelectedValue.ToString();
	NonExistentValue->DisplayText = DisplayText;
	NonExistentValue->bIsNonExistentValue = true;
	Rows.Insert(NonExistentValue, NonExistentValueIndex);
}

void FPulldownRows::SetDefaultRowInternal(const int32 NewDefaultRowIndex)
{
	check(NewDefaultRowIndex != INDEX_NONE);
	check(IsValidIndex(NewDefaultRowIndex));
	
	if (DefaultRowIndex == INDEX_NONE)
	{
		DefaultRowIndex = NewDefaultRowIndex;
	}
	else
	{
		const TSharedPtr<FPulldownRow> OldDefaultRow = GetDefaultRow();
		DefaultRowIndex = NewDefaultRowIndex;
		const TSharedPtr<FPulldownRow> NewDefaultRow = GetDefaultRow();
		check(OldDefaultRow.IsValid() && NewDefaultRow.IsValid());
		
		UE_LOG(LogPulldownBuilder, Warning, TEXT("You added a row (%s) marked as the default row, but since there is already a default row (%s), set the default value flag for existing rows to false."), *NewDefaultRow->SelectedValue, *OldDefaultRow->SelectedValue);
	}
}

TArray<TSharedPtr<FPulldownRow>>* FPulldownRows::operator&()
{
	return &Rows;
}

TSharedPtr<FPulldownRow> FPulldownRows::operator[](const int32 Index) const
{
	return Rows[Index];
}
