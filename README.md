# PulldownBuilder 

![image](https://user-images.githubusercontent.com/51815450/117563463-ff75b880-b0e0-11eb-9029-4a1766e39729.png)

<!--ts-->
   * [Description](#Description)
   * [Requirement](#Requirement)
   * [Installation](#Installation)
   * [Usage](#Usage)
      * [When using C++](#When-using-C++)
      * [When don't using C++](#When-don't-using-C++)
      * [PulldownListGenerator](#PulldownListGenerator)
      * [RowNameUpdater](#RowNameUpdater)
   * [Settings](#Settings)
   * [Note](#Note)
   * [License](#License)
   * [Author](#Author)
   * [History](#History)
<!--te-->

## Description  

This plugin allows you to easily create a pull-down structure using a dedicated asset.  
For example, you can specify the Row Name of the data table in a pull-down menu like an enum instead of a string.  

![Details](https://user-images.githubusercontent.com/51815450/127496205-9d6c49e1-436a-4631-aa82-609ce85574cc.PNG)

## Requirement  

Target version : UE4.23 ~ UE4.27  
Target platform :  Windows, Mac, Linux (Runtime module has no platform restrictions)   

## Installation  

Put the Plugins/PulldownBuilder folder in your project's Plugins folder.  
Or install from the [marketplace](https://www.unrealengine.com/marketplace/en-US/product/pulldown-builder).  

## Usage  

### ・When using C++   

[/Source/PulldownBuilderTest/PulldownBuilderTest.Build.cs](https://github.com/Naotsun19B/PulldownBuilder/blob/3b829357b2c9f013357a3ee30fcdb2b5a913aaf1/Source/PulldownBuilderTest/PulldownBuilderTest.Build.cs#L13)
```
using UnrealBuildTool;

public class PulldownBuilderTest : ModuleRules
{
	public PulldownBuilderTest(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] { "PulldownStruct" });
	}
}
```
First, we will use the structure included in the runtime module([`PulldownStruct`](https://github.com/Naotsun19B/PulldownBuilder/tree/master/Plugins/PulldownBuilder/Source/PulldownStruct)), so add it to the dependent module in [Project].Build.cs.  

[/Source/PulldownBuilderTest/TestPulldown.h](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Source/PulldownBuilderTest/TestPulldown.h)
```
#pragma once

#include "CoreMinimal.h"
#include "PulldownStructBase.h"
#include "TestPulldown.generated.h"

USTRUCT(BlueprintType)
struct FTestPulldown : public FPulldownStructBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 TestInt;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString TestString;
};

USTRUCT(BlueprintType)
struct FTestPulldown2 : public FPulldownStructBase
{
	GENERATED_BODY()
};
```
Next, define a structure that inherits from [`FPulldownStructBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownStruct/Public/PulldownStruct/PulldownStructBase.h) included in the runtime module([`PulldownStruct`](https://github.com/Naotsun19B/PulldownBuilder/tree/master/Plugins/PulldownBuilder/Source/PulldownStruct)).
You can define variables in addition to the string selected in the pull-down menu.  
The structure defined here is the structure for which the pull-down menu is displayed.  

![image](https://user-images.githubusercontent.com/51815450/117563939-42855b00-b0e4-11eb-8fb9-bddb3e346919.png)

Then build, launch the editor, and create a PulldownContents asset in the Content Browser.  

![image](https://user-images.githubusercontent.com/51815450/117564018-c6d7de00-b0e4-11eb-84f3-78183b6edd3b.png)

Open the created asset and set `Pulldown Struct Type` to the structure defined earlier.    
You can't set the same structure for multiple PulldownContents assets.  

![image](https://user-images.githubusercontent.com/51815450/117564115-5aa9aa00-b0e5-11eb-8bd2-4cf5d08d203b.png)

Next, set the class that builds the list from which the pull-down menu is based.    
See the `Pulldown List Generator` section below for the classes that build lists.    

![Preview](https://user-images.githubusercontent.com/51815450/127496168-b2746a67-e729-4883-8321-7aa94c1182ec.PNG)

With the settings up to this point, you can see the pull-down menu built by this PulldownContents asset in `Preview`.  
After that, if you use a structure defined by variables or function arguments, the pull-down menu will be displayed automatically.  

![Pins](https://user-images.githubusercontent.com/51815450/127496202-3b1d0838-aee7-49e1-a4da-b9c9e18abcc9.PNG)
![Details](https://user-images.githubusercontent.com/51815450/127496205-9d6c49e1-436a-4631-aa82-609ce85574cc.PNG)

### ・When don't using C++  

![image](https://user-images.githubusercontent.com/51815450/117563939-42855b00-b0e4-11eb-8fb9-bddb3e346919.png)

First, create a PulldownContents asset in the Content Browser.  

![image](https://user-images.githubusercontent.com/51815450/117564115-5aa9aa00-b0e5-11eb-8bd2-4cf5d08d203b.png)

Next, set the class that builds the list from which the pull-down menu is based.  
See the `PulldownListGenerator` section below for the classes that build lists.   

![Preview](https://user-images.githubusercontent.com/51815450/127496168-b2746a67-e729-4883-8321-7aa94c1182ec.PNG)

With the settings up to this point, you can see the pull-down menu built by this PulldownContents asset in `Preview`.  
After that, if you use `NativeLessPulldownStruct` for variables and function arguments, the pull-down menu will be displayed automatically.  
Unlike the one defined in C++, you can switch the PulldownContents asset from which the pull-down menu is based.  

![NativeLessPins](https://user-images.githubusercontent.com/51815450/127496214-49997c1a-fab5-4527-b994-a1e8b36214d0.PNG)
![NativeLessDetails](https://user-images.githubusercontent.com/51815450/127496219-56d9adb5-287d-42c2-856f-7ce5673cfcf0.PNG)

### ・PulldownListGenerator  

There is `PulldownListGenerator` as a class that builds the list that is the basis of the pull-down menu.
The following three `PulldownListGenerator`s are provided as standard.

|**Class**|**Function**|**Tooltip**|
|:---:|---|---|
|DataTablePulldownListGenerator|List the Row Names of the data table assets set in `SourceDataTable` in the pull-down menu.|If there is a variable named "PulldownTooltip" of type FString in the structure used as the row of the Data Table, that string is displayed.|
|StringTablePulldownListGenerator|List the Keys of the string table assets set in `SourceStringTable` in the pull-down menu.|Displays the corresponding character string for each item.|
|NameArrayPulldownListGenerator|List the elements of the array in the pull-down menu under `SourceNameArray`.|Displays the character string set in the Value of each item.|

To create your own `PulldownListGenerator`, inherit the [`UPulldownListGeneratorBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Public/PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h) in C++ or BP and override the `GetDisplayStrings`.  
The return value array will be listed in the pull-down menu.  

### ・RowNameUpdater  

If the underlying data of the pull-down menu is updated (for example, the Row Name of the data table has changed), there is a mechanism to replace the already used value with the new name.  
The following assets are supported as standard.

|**Asset type**|**Updater class**|
|:---:|---|
|Blueprint| [`UBlueprintUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/PulldownBuilder/RowNameUpdaters/BlueprintUpdater.h) |
|DataTable| [`UDataTableUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/PulldownBuilder/RowNameUpdaters/DataTableUpdater.h) |
|DataAsset| [`UDataAssetUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/PulldownBuilder/RowNameUpdaters/DataAssetUpdater.h) |

In order to support assets other than these, it is necessary to inherit [`URowNameUpdaterBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Public/PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h) in C ++ and implement the update process.

## Settings  

![Settings](https://user-images.githubusercontent.com/51815450/127496240-39f10c81-277b-40d3-8d5d-3a6ff68e1a17.PNG)

The items that can be set from the editor preferences are as follows.

|**Item**|**Description**|
|---|---|
|Panel Size|Specify the panel size of the pull-down menu.|
|Is Select when Double Click|If this flag is true, you will need to double-click to select an item in the pull-down menu.|
|Should Update When Source Row Name Changed|Specifies whether to perform automatic update processing of the pull-down menu using RowNameUpdater.|
|Active Row Name Updater|Specifies the RowNameUpdater class to enable. Only the RowNameUpdater set here will perform the update process.|

## Note  

・The PulldownContents asset is an editor-only asset and will not be cooked into the package.

## License

[MIT License](https://en.wikipedia.org/wiki/MIT_License)

## Author  

[Naotsun](https://twitter.com/Naotsun_UE)

## History  

- (2021/10/17) v1.4   
  Added support for UE5  

- (2021/07/29) v1.3   
  The pull-down menu widget has been significantly improved so that tooltips can be displayed for each item  

- (2021/06/02) v1.2   
  Added the ability to replace already used values with new names when the underlying data in the pull-down menu is updated  

- (2021/05/29) v1.1   
  Fixed an issue where specifying a data table created from UserDefinedStruct as a PulldownContents asset would not work properly  

- (2021/05/09) v1.0   
  Publish plugin
