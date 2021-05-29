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
   * [Note](#Note)
   * [License](#License)
   * [Author](#Author)
   * [History](#History)
<!--te-->

## Description  

This plugin allows you to easily create a pull-down structure using a dedicated asset.  
For example, you can specify the Row Name of the data table in a pull-down menu like an enum instead of a string.  

![image](https://user-images.githubusercontent.com/51815450/117563475-20d6a480-b0e1-11eb-9f98-eaed2cc562ea.png)

## Requirement  

Target version : UE4.23 ~ UE4.26  
Target platform :  Windows, Mac, Linux (Runtime module has no platform restrictions)   

## Installation  

Put the Plugins/PulldownBuilder folder in your project's Plugins folder.  

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
Next, define a structure that inherits from [`FPulldownStructBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/3b829357b2c9f013357a3ee30fcdb2b5a913aaf1/Plugins/PulldownBuilder/Source/PulldownStruct/Public/PulldownStructBase.h#L16) included in the runtime module([`PulldownStruct`](https://github.com/Naotsun19B/PulldownBuilder/tree/master/Plugins/PulldownBuilder/Source/PulldownStruct)).
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

![image](https://user-images.githubusercontent.com/51815450/117564161-a78d8080-b0e5-11eb-8e2c-d158b58b571e.png)

With the settings up to this point, you can see the pull-down menu built by this PulldownContents asset in `Preview`.  
After that, if you use a structure defined by variables or function arguments, the pull-down menu will be displayed automatically.  

![image](https://user-images.githubusercontent.com/51815450/117563475-20d6a480-b0e1-11eb-9f98-eaed2cc562ea.png)
![image](https://user-images.githubusercontent.com/51815450/117564229-1c60ba80-b0e6-11eb-9455-b160a5d0bef0.png)

### ・When don't using C++  

![image](https://user-images.githubusercontent.com/51815450/117563939-42855b00-b0e4-11eb-8fb9-bddb3e346919.png)

First, create a PulldownContents asset in the Content Browser.  

![image](https://user-images.githubusercontent.com/51815450/117564115-5aa9aa00-b0e5-11eb-8bd2-4cf5d08d203b.png)

Next, set the class that builds the list from which the pull-down menu is based.  
See the `Pulldown List Generator` section below for the classes that build lists.   

![image](https://user-images.githubusercontent.com/51815450/117564161-a78d8080-b0e5-11eb-8e2c-d158b58b571e.png)

With the settings up to this point, you can see the pull-down menu built by this PulldownContents asset in `Preview`.  
After that, if you use `NativeLessPulldownStruct` for variables and function arguments, the pull-down menu will be displayed automatically.  
Unlike the one defined in C++, you can switch the PulldownContents asset from which the pull-down menu is based.  

![image](https://user-images.githubusercontent.com/51815450/117564369-aa3ca580-b0e6-11eb-9aa0-90dede20de3b.png)
![image](https://user-images.githubusercontent.com/51815450/117564377-baed1b80-b0e6-11eb-9795-1c82e424eb58.png)
![image](https://user-images.githubusercontent.com/51815450/117564402-d0624580-b0e6-11eb-9b24-ccf7bae3467c.png)

### ・PulldownListGenerator  

There is `PulldownListGenerator` as a class that builds the list that is the basis of the pull-down menu.
The following three `PulldownListGenerator`s are provided as standard.

|**Class**|**Function**|
|:---:|---|
|DataTablePulldownListGenerator|List the Row Names of the data table assets set in `SourceDataTable` in the pull-down menu.|
|StringTablePulldownListGenerator|List the Keys of the string table assets set in `SourceStringTable` in the pull-down menu.|
|NameArrayPulldownListGenerator|List the elements of the array in the pull-down menu under `SourceNameArray`.|

To create your own `PulldownListGenerator`, inherit the` UPulldownListGeneratorBase` in C++ or BP and override the `GetDisplayStrings`.  
The return value array will be listed in the pull-down menu.  

## Note  

・The PulldownContents asset is an editor-only asset and will not be cooked into the package.

・At UE4.26.2, there is a problem with the engine code, so only UE4.26 does not display the search field in the pull-down menu.
#### UE4.23 ~ UE4.25  
![E0Tj1waVUAQ0LBM](https://user-images.githubusercontent.com/51815450/117564840-056f9780-b0e9-11eb-806b-00db8bf125d9.png)
#### UE4.26.2  
![image](https://user-images.githubusercontent.com/51815450/117564900-6d25e280-b0e9-11eb-992c-5a63a6faa7d6.png)

## License

[MIT License](https://en.wikipedia.org/wiki/MIT_License)

## Author  

[Naotsun](https://twitter.com/Naotsun_UE)

## History  

- (2021/05/29) v1.1   
  Fixed an issue where specifying a data table created from UserDefinedStruct as a PulldownContents asset would not work properly  

- (2021/05/09) v1.0   
  Publish plugin
