# PulldownBuilder 

![image](https://user-images.githubusercontent.com/51815450/117563463-ff75b880-b0e0-11eb-9029-4a1766e39729.png)

<!--ts-->
   * [概要](#概要)
   * [動作環境](#動作環境)
   * [インストール](#インストール)
   * [使い方](#使い方)
      * [C++を使用する場合](#C++を使用する場合)
      * [C++を使用しない場合](#C++を使用しない場合)
      * [PulldownListGenerator](#PulldownListGenerator)
      * [RowNameUpdater](#RowNameUpdater)
   * [オプション](#オプション)
   * [備考](#備考)
   * [ライセンス](#ライセンス)
   * [作者](#作者)
   * [履歴](#履歴)
<!--te-->

## 概要  

このプラグインは専用アセットを使用して簡単にプルダウン表示する構造体を作成することができます。  
例えば、データテーブルのRowNameを文字列ではなく列挙型のようなプルダウンメニューで指定することができます。  

![image](https://user-images.githubusercontent.com/51815450/117563475-20d6a480-b0e1-11eb-9f98-eaed2cc562ea.png)

## 動作環境  

対象バージョン : UE4.23 ~ UE4.26  
対象プラットフォーム :  Windows, Mac, Linux (ランタイムモジュールはプラットフォームの制限無し)   

## インストール  

プロジェクトのPluginsフォルダにPlugins/PulldownBuilderフォルダを入れてください。  

## 使い方  

### ・C++を使用する場合  

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
まずは、ランタイムモジュール([`PulldownStruct`](https://github.com/Naotsun19B/PulldownBuilder/tree/master/Plugins/PulldownBuilder/Source/PulldownStruct))に含まれる構造体を使用するため、[Project].Build.csで依存モジュールに追加します。

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
次に、ランタイムモジュール([`PulldownStruct`](https://github.com/Naotsun19B/PulldownBuilder/tree/master/Plugins/PulldownBuilder/Source/PulldownStruct))に含まれる[`FPulldownStructBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/3b829357b2c9f013357a3ee30fcdb2b5a913aaf1/Plugins/PulldownBuilder/Source/PulldownStruct/Public/PulldownStructBase.h#L16)を継承した構造体を定義します。  
プルダウンメニューで選択する文字列の他に変数を定義することができます。  
ここで定義した構造体がプルダウンメニューが表示される構造体になります。  

![image](https://user-images.githubusercontent.com/51815450/117563939-42855b00-b0e4-11eb-8fb9-bddb3e346919.png)

続いて、ビルドしてエディタを起動し、コンテンツブラウザでPulldownContentsアセットを作成します。

![image](https://user-images.githubusercontent.com/51815450/117564018-c6d7de00-b0e4-11eb-84f3-78183b6edd3b.png)

作成したアセットを開き、`Pulldown Struct Type`に先程定義した構造体を設定します。  
複数のPulldownContentsアセットに同一の構造体を設定することはできません。  

![image](https://user-images.githubusercontent.com/51815450/117564115-5aa9aa00-b0e5-11eb-8bd2-4cf5d08d203b.png)

次に、プルダウンメニューの元となるリストを構築するクラスを設定します。  
リストを構築するクラスについては下記の`PulldownListGenerator`の項目を参照してください。  

![Preview](https://user-images.githubusercontent.com/51815450/127496168-b2746a67-e729-4883-8321-7aa94c1182ec.PNG)

ここまで設定すると`Preview`でこのPulldownContentsアセットによって構築されるプルダウンメニューを確認することができます。  
あとは、変数や関数の引数などで定義した構造体を使用すると自動的にプルダウンメニューが表示されるようになります。  

![Pins](https://user-images.githubusercontent.com/51815450/127496202-3b1d0838-aee7-49e1-a4da-b9c9e18abcc9.PNG)
![Details](https://user-images.githubusercontent.com/51815450/127496205-9d6c49e1-436a-4631-aa82-609ce85574cc.PNG)

### ・C++を使用しない場合

![image](https://user-images.githubusercontent.com/51815450/117563939-42855b00-b0e4-11eb-8fb9-bddb3e346919.png)

まずは、コンテンツブラウザでPulldownContentsアセットを作成します。

![image](https://user-images.githubusercontent.com/51815450/117564115-5aa9aa00-b0e5-11eb-8bd2-4cf5d08d203b.png)

次に、プルダウンメニューの元となるリストを構築するクラスを設定します。  
リストを構築するクラスについては下記の`PulldownListGenerator`の項目を参照してください。  

![Preview](https://user-images.githubusercontent.com/51815450/127496168-b2746a67-e729-4883-8321-7aa94c1182ec.PNG)

ここまで設定すると`Preview`でこのPulldownContentsアセットによって構築されるプルダウンメニューを確認することができます。  
あとは、変数や関数の引数などで`NativeLessPulldownStruct`を使用すると自動的にプルダウンメニューが表示されるようになります。  
C++で定義したものとは違い、プルダウンメニューの元となるPulldownContentsアセットを切り替えることができます。

![NativeLessPins](https://user-images.githubusercontent.com/51815450/127496214-49997c1a-fab5-4527-b994-a1e8b36214d0.PNG)
![NativeLessDetails](https://user-images.githubusercontent.com/51815450/127496219-56d9adb5-287d-42c2-856f-7ce5673cfcf0.PNG)

### ・PulldownListGenerator  

プルダウンメニューの元となるリストを構築するクラスとして`PulldownListGenerator`があります。  
標準で以下の3つの`PulldownListGenerator`が用意されています。

|**クラス**|**機能**|**ツールチップ**|
|:---:|---|---|
|DataTablePulldownListGenerator|`SourceDataTable`に設定されたデータテーブルアセットのRowNameをプルダウンメニューに列挙します。|データテーブルの行として使用されている構造体内にFString型の"PulldownTooltip"という名前の変数があった場合、その文字列を表示します。|
|StringTablePulldownListGenerator|`SourceStringTable`に設定されたストリングテーブルアセットのKeyをプルダウンメニューに列挙します。|各項目の対応する文字列を表示します。|
|NameArrayPulldownListGenerator|`SourceNameArray`の要素をプルダウンメニューに列挙します。|各項目のValueに設定されている文字列を表示します。|

独自の`PulldownListGenerator`を作成するには、C++もしくはBPで [`UPulldownListGeneratorBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/ListGenerator/PulldownListGeneratorBase.h) を継承し、`GetPulldownRows`をオーバライドします。  
戻り値の配列がプルダウンメニューに列挙される内容になります。  

### ・RowNameUpdater  

プルダウンメニューの元となるデータに更新があった場合(例えばデータテーブルのRowNameが変更されたなど)、既に使用されている値を新しい名前に置き換える仕組みがあります。  
標準で以下のアセットが対応しています。

|**アセット**|**Updaterクラス**|
|:---:|---|
|Blueprint| [`UBlueprintUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/RowNameUpdater/BlueprintUpdater.h) |
|DataTable| [`UDataTableUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/RowNameUpdater/DataTableUpdater.h) |
|DataAsset| [`UDataAssetUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/RowNameUpdater/DataAssetUpdater.h) |

これら以外のアセットに対応するには、C++で [`URowNameUpdaterBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/RowNameUpdater/RowNameUpdaterBase.h) を継承して更新処理を実装する必要があります。

## オプション  

![Settings](https://user-images.githubusercontent.com/51815450/127496240-39f10c81-277b-40d3-8d5d-3a6ff68e1a17.PNG)

エディタの環境設定から設定できる項目は以下の通りです。

|**項目**|**説明**|
|---|---|
|Panel Size|プルダウンメニューのパネルサイズを指定します。|
|Is Select when Double Click|このフラグがtrueの場合はプルダウンメニューの項目を選ぶ時にダブルクリックをする必要があります。|
|Should Update When Source Row Name Changed|RowNameUpdaterを使ったプルダウンメニューの自動更新処理を行うかを指定します。|
|Active Row Name Updater|有効化するRowNameUpdaterのクラスを指定します。ここで設定されているRowNameUpdaterのみが更新処理を行います。|

## 備考  

・PulldownContentsアセットはエディタ限定のアセットなため、パッケージにはクックされません。

## ライセンス

[MITライセンス](https://ja.wikipedia.org/wiki/MIT_License)

## 作者

[Naotsun](https://twitter.com/Naotsun_UE)

## 履歴

- (2021/07/29) v1.3   
  プルダウンメニューウィジェットを大幅に改善し、各項目にツールチップを表示できるようにしました  

- (2021/06/02) v1.2   
  プルダウンメニューの元となるデータに更新があった場合に、既に使用されている値を新しい名前に置き換える機能を追加しました  

- (2021/05/29) v1.1   
  UserDefinedStructをPulldownContentsアセットとして使用してデータ テーブルを指定すると正しく動作しない問題を修正しました  

- (2021/05/09) v1.0   
  プラグインを公開  
