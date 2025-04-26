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

Target version : UE4.27 ~ 5.5  
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
	SETUP_PULLDOWN_STRUCT()

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
	SETUP_PULLDOWN_STRUCT()
};
```
Next, define a structure that inherits from [`FPulldownStructBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownStruct/Public/PulldownStruct/PulldownStructBase.h) included in the runtime module([`PulldownStruct`](https://github.com/Naotsun19B/PulldownBuilder/tree/master/Plugins/PulldownBuilder/Source/PulldownStruct)).
You can define variables in addition to the string selected in the pull-down menu.  
The structure defined here is the structure for which the pull-down menu is displayed.  
By using `SETUP_PULLDOWN_STRUCT` added in version 2.4, you can now see which asset uses which pulldown structure and which value in the reference viewer.  
Please note that by resaving assets created with the previous version, the contents of the reference viewer will be updated.  

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

To create your own `PulldownListGenerator`, inherit the [`UPulldownListGeneratorBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Public/PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h) in C++ or BP and override the `GetPulldownRows`.  
The return value array will be listed in the pull-down menu.  
Since the object with the variable being edited and the variable being edited are passed as arguments, the contents of the list generated based on it can be changed.  

![BP_TestPulldown4PulldownListGenerator-GetPulldownRowsFromBlueprint](https://user-images.githubusercontent.com/51815450/177553308-a277f778-67d1-41aa-8495-2cba434ed423.png)

https://user-images.githubusercontent.com/51815450/177554749-425e7a4a-a17b-4202-be00-2c5b24244a73.mp4

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

![CompareNodes](https://user-images.githubusercontent.com/51815450/223067854-f48de6a1-9766-4bdb-ad28-6944a9c86913.PNG)

Nodes is available that compares pull-down structures of the same type.  
If the comparison target is `NativeLessPulldownStruct`, you can set whether to also compare `Pulldown Source` of `NativeLessPulldownStruct` when comparing with `Strict Comparison` property from node details panel.  

![SwitchNodes](https://user-images.githubusercontent.com/51815450/223068042-7e42c339-aa07-4c57-bda1-9f8fe0d4665a.PNG)

Switch nodes is available that consists of the items that appear in the pull-down menu of the pull-down structure.  
If the target pulldown structure is `NativeLessPulldownStruct`, the property `Pulldown Contents` from the node details panel allows you to select the asset that the node's pin items are based on.    
Also, if the target pulldown structure is `Native Less Pulldown Struct` and a value that is not one of the items is passed, it will be output to the default pin.  

## Settings  

![EditorPreferences](https://user-images.githubusercontent.com/51815450/173224011-f82601a7-77e8-45fb-b74a-31ca17464163.PNG)

The items that can be set from the editor preferences are as follows.

| **Section** | **Item**                                   | **Description**                                                                                                 |
|-------------|--------------------------------------------|-----------------------------------------------------------------------------------------------------------------|
| Appearance  | Panel Size                                 | Specifies the panel size of the pull-down menu.                                                                 |
|             | Is Select when Double Click                | If this flag is true, you will need to double-click to select an item in the pull-down menu.                    |
|             | Should Inline Display When Single Property | If this flag is true, automatically inline a single property pull-down structure.                               |
|             | Notification Severity                      | Specifies the severity to focus the message log of notifications issued by this plugin.                         |
| Redirect    | Should Update When Source Row Name Changed | Specifies whether to perform automatic update processing of the pull-down menu using RowNameUpdater.            |
|             | Active Row Name Updater                    | Specifies the RowNameUpdater class to enable. Only the RowNameUpdater set here will perform the update process. |

## Note  

・The PulldownContents asset is an editor-only asset and will not be cooked into the package.  
・In UE4.27 and earlier, the reset to default value function cannot be implemented when inline displaying, so `Should Inline Display When Single Property` will be disabled.

## License

[MIT License](https://en.wikipedia.org/wiki/MIT_License)

## Author  

[Naotsun](https://twitter.com/Naotsun_UE)

## History  

- (2025/04/26) v2.6  
  Support before UE4.26 has been discontinued  
  Added a function that allows you to set default values for pull-down structures in `PulldownListGenerator`  
  Changed to apply the default value set by the user when reset to default value is performed on the details panel or graph pin  

- (2024/12/26) v2.5  
  Added support for UE5.5  
  Fixed a bug that caused a crash in the sequence editor when `SETUP_PULLDOWN_STRUCT` was used  

- (2024/04/24) v2.4  
  Added support for UE5.4  
  Added a node that converts the pulldown structure to a string with the value set in `Selected Value`  
  Fixed a bug that caused an error on K 2 Node during packaging  
  Changed the pulldown content asset to be loaded asynchronously when the editor starts  
  It is now possible to specify the vertical and horizontal sizes of the pull-down menu individually in the pull-down content asset  
  Fixed a bug that caused a crash when using a pulldown structure in the sequencer key property  
  Fixed a bug where the value could not be saved correctly if the default value of the pulldown structure pin contained special characters  
  Added a feature that allows you to check which asset uses which pulldown structure and which value in the reference viewer  

- (2023/09/07) v2.3  
  Added support for UE5.3  
  Added the ability to set the values used at runtime and the data displayed in the pull-down menu on the editor separately  
  Added `LexFromString` function for pulldown structure

- (2023/06/09) v2.2  
  Fixed a bug that the details customization of the pull-down structure was not registered  
  Fixed a bug that caused nodes placed in level blueprints to update unnecessarily when the editor was launched  
  Added redefinition countermeasures to version-separated macros  
  Added `LexToString` function for pulldown structure

- (2023/05/16) v2.1  
  Fixed a bug that crashes when exiting the editor    
  Added macro to indicate if pulldown structure is available  
  Added support for UE5.2  

- (2023/03/06) v2.0  
  Added compare nodes for `NativeLessPulldownStruct`  
  Added switch nodes for pull-down structure  

- (2023/02/01) v1.9  
  Fixed a bug that crashes when starting the editor  
  Fixed a bug where generic not equal node was converted to pull-down structure not equal node  
  Fixed a bug that the display was not updated when the default value of the graph pin was changed  
  Default values can now be copied or pasted from the graph pin context menu  
  You can now open the PulldownContents asset underlying the variable pull-down structure from the context menu of the graph pin or details panel  

- (2022/11/08) v1.8  
  Fixed a crash when searching for a node when the editor language is other than English  
  Fixed a bug that pin value becomes invalid data  
  Fixed a bug that the search field did not work properly  
  Added support for UE5.1  

- (2022/07/07) v1.7  
  Changed to pass the object with the variable being edited and the variable being edited to the argument of `GetPulldownRows` of `PulldownListGenerator`  

- (2022/06/12) v1.6   
  Added comparison blueprint node between pull-down structures  
  The settings related to appearance are saved for each individual, and the settings related to redirect processing are saved in the project share  
  Added `PulldownListGenerator` to generate a pull-down list from the input settings

- (2022/04/06) v1.5   
  Fixed errors when building with strict includes

- (2021/10/17) v1.4   
  Added support for UE5.0  

- (2021/07/29) v1.3   
  The pull-down menu widget has been significantly improved so that tooltips can be displayed for each item  

- (2021/06/02) v1.2   
  Added the ability to replace already used values with new names when the underlying data in the pull-down menu is updated  

- (2021/05/29) v1.1   
  Fixed an issue where specifying a data table created from UserDefinedStruct as a PulldownContents asset would not work properly  

- (2021/05/09) v1.0   
  Publish plugin
