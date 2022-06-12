# PulldownBuilder 

![Plugin](https://user-images.githubusercontent.com/51815450/173223798-f60374a1-1d14-4ac2-93c1-bdbab9fe5278.PNG)

<!--ts-->
   * [Description](#Description)
   * [Requirement](#Requirement)
   * [Installation](#Installation)
   * [Usage](#Usage)
      * [When using C++](#when-using-c)
      * [When don't using C++](#when-dont-using-c)
      * [PulldownListGenerator](#PulldownListGenerator)
      * [RowNameUpdater](#RowNameUpdater)
   * [Nodes](#Nodes)
   * [Settings](#Settings)
   * [Note](#Note)
   * [License](#License)
   * [Author](#Author)
   * [History](#History)
<!--te-->

## Description  

This plugin allows you to easily create a pull-down structure using a dedicated asset.  
For example, you can specify the Row Name of the data table in a pull-down menu like an enum instead of a string.  

![PulldownDetails](https://user-images.githubusercontent.com/51815450/173223818-c8297c9c-6a0d-4e4a-938d-a15f55df9c49.PNG)

## Requirement  

Target version : UE4.23 ~ 5.0  
Target platform :  Windows, Mac, Linux (Runtime module has no platform restrictions)   

## Installation  

Put the Plugins/PulldownBuilder folder in your project's Plugins folder.  
Or install from the [marketplace](https://www.unrealengine.com/marketplace/en-US/product/pulldown-builder).  
If the feature is not available after installing the plugin, it is possible that the plugin has not been enabled, so please check if the plugin is enabled from Edit > Plugins.  

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

![CreatePulldownContents](https://user-images.githubusercontent.com/51815450/173223842-a5356544-b7ee-4979-9864-36986ee358ec.PNG)

Then build, launch the editor, and create a PulldownContents asset in the Content Browser.  

![PulldownStructType](https://user-images.githubusercontent.com/51815450/173223858-12fca014-5fb8-44b1-bbce-1d07c363edde.PNG)

Open the created asset and set `Pulldown Struct Type` to the structure defined earlier.    
You can't set the same structure for multiple PulldownContents assets.  

![PulldownListGenerator](https://user-images.githubusercontent.com/51815450/173223874-23a8d64b-545d-4fb4-8b89-b163a93df5fc.PNG)

Next, set the class that builds the list from which the pull-down menu is based.    
See the `Pulldown List Generator` section below for the classes that build lists.    

![Preview](https://user-images.githubusercontent.com/51815450/173223882-b88eba54-db31-4282-8b57-aa49f4ac4c1b.PNG)

With the settings up to this point, you can see the pull-down menu built by this PulldownContents asset in `Preview`.  
After that, if you use a structure defined by variables or function arguments, the pull-down menu will be displayed automatically.  

![PulldownPin](https://user-images.githubusercontent.com/51815450/173223894-288dc7c3-ca0b-47dc-b0c9-97e7999f6460.PNG)
![PulldownDetails](https://user-images.githubusercontent.com/51815450/173223818-c8297c9c-6a0d-4e4a-938d-a15f55df9c49.PNG)

### ・When don't using C++  

![CreatePulldownContents](https://user-images.githubusercontent.com/51815450/173223842-a5356544-b7ee-4979-9864-36986ee358ec.PNG)

First, create a PulldownContents asset in the Content Browser.  

![PulldownListGenerator](https://user-images.githubusercontent.com/51815450/173223874-23a8d64b-545d-4fb4-8b89-b163a93df5fc.PNG)

Next, set the class that builds the list from which the pull-down menu is based.  
See the `PulldownListGenerator` section below for the classes that build lists.   

![Preview](https://user-images.githubusercontent.com/51815450/173223882-b88eba54-db31-4282-8b57-aa49f4ac4c1b.PNG)

With the settings up to this point, you can see the pull-down menu built by this PulldownContents asset in `Preview`.  
After that, if you use `NativeLessPulldownStruct` for variables and function arguments, the pull-down menu will be displayed automatically.  
Unlike the one defined in C++, you can switch the PulldownContents asset from which the pull-down menu is based.  

![NativeLessPin](https://user-images.githubusercontent.com/51815450/173223970-d3d8e7e5-3f1c-4d9f-9293-71cdc35ebbe7.PNG)
![NativeLessDetails](https://user-images.githubusercontent.com/51815450/173223982-c93bd1dc-8470-4aca-906c-61617450cc00.PNG)

### ・PulldownListGenerator  

There is `PulldownListGenerator` as a class that builds the list that is the basis of the pull-down menu.
The following three `PulldownListGenerator`s are provided as standard.

|             **Class**              | **Function**                                                                                            | **Tooltip**                                                                                                                                  |
|:----------------------------------:|---------------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------------------------------------------------------|
|   DataTablePulldownListGenerator   | List the Row Names of the data table assets set in `SourceDataTable` in the pull-down menu.             | If there is a variable named "PulldownTooltip" of type FString in the structure used as the row of the Data Table, that string is displayed. |
|  StringTablePulldownListGenerator  | List the Keys of the string table assets set in `SourceStringTable` in the pull-down menu.              | Displays the corresponding character string for each item.                                                                                   |
|   NameArrayPulldownListGenerator   | List the elements of the array in the pull-down menu under `SourceNameArray`.                           | Displays the character string set in the Value of each item.                                                                                 |
| InputMappingsPulldownListGenerator | List the elements of the input mapping set in the input of the project settings in the pull-down menu.  | Displays the name of the button corresponding to the input name.                                                                             |

To create your own `PulldownListGenerator`, inherit the [`UPulldownListGeneratorBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Public/PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h) in C++ or BP and override the `GetDisplayStrings`.  
The return value array will be listed in the pull-down menu.  

### ・RowNameUpdater  

If the underlying data of the pull-down menu is updated (for example, the Row Name of the data table has changed), there is a mechanism to replace the already used value with the new name.  
The following assets are supported as standard.

| **Asset type** | **Updater class**                                                                                                                                                                            |
|:--------------:|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
|   Blueprint    | [`UBlueprintUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/PulldownBuilder/RowNameUpdaters/BlueprintUpdater.h)   |
|   DataTable    | [`UDataTableUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/PulldownBuilder/RowNameUpdaters/DataTableUpdater.h)   |
|   DataAsset    | [`UDataAssetUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/PulldownBuilder/RowNameUpdaters/DataAssetUpdater.h)   |

In order to support assets other than these, it is necessary to inherit [`URowNameUpdaterBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Public/PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h) in C ++ and implement the update process.

### ・Nodes

![Nodes](https://user-images.githubusercontent.com/51815450/173224288-acde9271-fff3-40eb-bd4a-47529f90a62a.PNG)

Nodes are available that compare pull-down structures of the same type.

## Settings  

![EditorPreferences](https://user-images.githubusercontent.com/51815450/173224011-f82601a7-77e8-45fb-b74a-31ca17464163.PNG)

The items that can be set from the editor preferences are as follows.

| **Section** | **Item**                                   | **Description**                                                                                                 |
|-------------|--------------------------------------------|-----------------------------------------------------------------------------------------------------------------|
| Appearance  | Panel Size                                 | Specify the panel size of the pull-down menu.                                                                   |
|             | Is Select when Double Click                | If this flag is true, you will need to double-click to select an item in the pull-down menu.                    |
|             | Should Inline Display When Single Property | If this flag is true, automatically inline a single property pull-down structure.                               |
| Redirect    | Should Update When Source Row Name Changed | Specifies whether to perform automatic update processing of the pull-down menu using RowNameUpdater.            |
|             | Active Row Name Updater                    | Specifies the RowNameUpdater class to enable. Only the RowNameUpdater set here will perform the update process. |

## Note  

・The PulldownContents asset is an editor-only asset and will not be cooked into the package.

## License

[MIT License](https://en.wikipedia.org/wiki/MIT_License)

## Author  

[Naotsun](https://twitter.com/Naotsun_UE)

## History  

- (2022/06/12) v1.6   
  Added comparison blueprint node between pull-down structures  
  The settings related to appearance are saved for each individual, and the settings related to redirect processing are saved in the project share
  Added `PulldownListGenerator` to generate a pulldown list from the input settings

- (2022/04/06) v1.5   
  Fixed errors when building with strict includes

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
