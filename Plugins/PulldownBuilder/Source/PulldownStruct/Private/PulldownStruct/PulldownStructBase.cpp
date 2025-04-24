// Copyright 2021-2025 Naotsun. All Rights Reserved.

#include "PulldownStruct/PulldownStructBase.h"

FPulldownStructBase::FPulldownStructBase()
	: SelectedValue(NAME_None)
#if WITH_EDITOR
	, SearchableObject(nullptr)
	, bIsEdited(false)
#endif
{
}

FPulldownStructBase::FPulldownStructBase(const FName& InSelectedValue)
	: SelectedValue(InSelectedValue)
#if WITH_EDITOR
	, SearchableObject(nullptr)
	, bIsEdited(false)
#endif
{
}

const FName& FPulldownStructBase::operator* () const
{
	return SelectedValue;
}

FString FPulldownStructBase::ToString() const
{
	return SelectedValue.ToString();
}

FText FPulldownStructBase::ToText() const
{
	return FText::FromName(SelectedValue);
}

bool FPulldownStructBase::IsNone() const
{
	return SelectedValue.IsNone();
}

void FPulldownStructBase::PostSerialize_Implementation(const FArchive& Ar)
{
}

#if WITH_EDITOR
void FPulldownStructBase::MarkSearchableName(const FArchive& Ar)
{
	Ar.MarkSearchableName(SearchableObject, SelectedValue);
}
#endif

#if WITH_EDITORONLY_DATA
const FName FPulldownStructBase::SearchableObjectPropertyName	= GET_MEMBER_NAME_CHECKED(FPulldownStructBase, SearchableObject);
const FName FPulldownStructBase::IsEditedPropertyName			= GET_MEMBER_NAME_CHECKED(FPulldownStructBase, bIsEdited);
#endif