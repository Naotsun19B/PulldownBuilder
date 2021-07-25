// Copyright 2021 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ListGenerators/PulldownListGeneratorBase.h"
#include "NameArrayPulldownListGenerator.generated.h"

/**
 * Generate a list to be displayed in the pull-down menu from the name array.
 */
UCLASS()
class UNameArrayPulldownListGenerator : public UPulldownListGeneratorBase
{
	GENERATED_BODY()

public:
	// UObject interface.
#if BEFORE_UE_4_24
	virtual void PreEditChange(UProperty* PropertyAboutToChange) override;
#else
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
#endif
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	// End of UObject interface.
	
	// UPulldownListGeneratorBase interface.
	virtual TArray<TSharedPtr<FString>> GetDisplayStrings() const override;
	// End of UPulldownListGeneratorBase interface.

protected:
	// Name array from which the list displayed in the pull-down menu is based.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pulldown")
	TArray<FName> SourceNameArray;

	// Cache of name array before change.
	UPROPERTY(Transient)
	TArray<FName> PreChangeNameArray;
};
