// Copyright 2021-2025 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PulldownListGeneratorBase.h"
#include "ActorNamePulldownListGenerator.generated.h"

class AActor;

/**
 * A generator class that generates a list displayed in the pull-down menu from actors placed in the currently open world.
 * SelectedValue is in the following format: WorldName::ActorIdentifierName
 */
UCLASS()
class PULLDOWNBUILDER_API UActorNamePulldownListGenerator : public UPulldownListGeneratorBase
{
	GENERATED_BODY()

public:
	// Constructor.
	UActorNamePulldownListGenerator();
	
	// UPulldownListGeneratorBase interface.
	virtual FPulldownRows GetPulldownRows(
		const TArray<UObject*>& OuterObjects,
		const FStructContainer& StructInstance
	) const override;
	virtual bool IsEnableCustomDefaultValue_Implementation() const override;
	// End of UPulldownListGeneratorBase interface.

protected:
	// Returns whether the specified actor is listed in the pull-down menu.
	UFUNCTION(BlueprintNativeEvent, Category = "Pulldown")
	bool FilterActor(const AActor* TestActor) const;

public:
	// Returns the class of the actor to be listed in the pull-down menu.
	UClass* GetActorClass() const;
	
protected:
	// The class of the actor to be listed in the pull-down menu.
	UPROPERTY(EditAnywhere, Category = "Pulldown")
	TSubclassOf<AActor> ActorClass;

	// Whether classes of actors that inherit from the class specified in ActorClass are also listed in the pull-down menu.
	UPROPERTY(EditAnywhere, Category = "Pulldown")
	bool bIncludeInheritedActorClasses;

	// The list of classes of actors that inherit from Actor Class but do not explicitly list them in the pull-down menu.
	UPROPERTY(EditAnywhere, Category = "Pulldown", meta = (EditCondition = "bIncludeInheritedActorClasses"))
	TArray<TSubclassOf<AActor>> ActorClassesToExclude;

	// The list of interfaces that the actors listed in the pull-down menu must implement.
	UPROPERTY(EditAnywhere, Category = "Pulldown")
	TArray<TSubclassOf<UInterface>> RequiredInterfaces;
};
