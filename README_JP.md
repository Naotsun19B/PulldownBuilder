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

![image](https://user-images.githubusercontent.com/51815450/117564161-a78d8080-b0e5-11eb-8e2c-d158b58b571e.png)

ここまで設定すると`Preview`でこのPulldownContentsアセットによって構築されるプルダウンメニューを確認することができます。  
あとは、変数や関数の引数などで定義した構造体を使用すると自動的にプルダウンメニューが表示されるようになります。  

![image](https://user-images.githubusercontent.com/51815450/117563475-20d6a480-b0e1-11eb-9f98-eaed2cc562ea.png)
![image](https://user-images.githubusercontent.com/51815450/117564229-1c60ba80-b0e6-11eb-9455-b160a5d0bef0.png)

### ・C++を使用しない場合

![image](https://user-images.githubusercontent.com/51815450/117563939-42855b00-b0e4-11eb-8fb9-bddb3e346919.png)

まずは、コンテンツブラウザでPulldownContentsアセットを作成します。

![image](https://user-images.githubusercontent.com/51815450/117564115-5aa9aa00-b0e5-11eb-8bd2-4cf5d08d203b.png)

次に、プルダウンメニューの元となるリストを構築するクラスを設定します。  
リストを構築するクラスについては下記の`PulldownListGenerator`の項目を参照してください。  

![image](https://user-images.githubusercontent.com/51815450/117564161-a78d8080-b0e5-11eb-8e2c-d158b58b571e.png)

ここまで設定すると`Preview`でこのPulldownContentsアセットによって構築されるプルダウンメニューを確認することができます。  
あとは、変数や関数の引数などで`NativeLessPulldownStruct`を使用すると自動的にプルダウンメニューが表示されるようになります。  
C++で定義したものとは違い、プルダウンメニューの元となるPulldownContentsアセットを切り替えることができます。

![image](https://user-images.githubusercontent.com/51815450/117564369-aa3ca580-b0e6-11eb-9aa0-90dede20de3b.png)
![image](https://user-images.githubusercontent.com/51815450/117564377-baed1b80-b0e6-11eb-9795-1c82e424eb58.png)
![image](https://user-images.githubusercontent.com/51815450/117564402-d0624580-b0e6-11eb-9b24-ccf7bae3467c.png)

### ・PulldownListGenerator  

プルダウンメニューの元となるリストを構築するクラスとして`PulldownListGenerator`があります。  
標準で以下の3つの`PulldownListGenerator`が用意されています。

|**クラス**|**機能**|
|:---:|---|
|DataTablePulldownListGenerator|`SourceDataTable`に設定されたデータテーブルアセットのRowNameをプルダウンメニューに列挙します。|
|StringTablePulldownListGenerator|`SourceStringTable`に設定されたストリングテーブルアセットのKeyをプルダウンメニューに列挙します。|
|NameArrayPulldownListGenerator|`SourceNameArray`に配列の要素をプルダウンメニューに列挙します。|

独自の`PulldownListGenerator`を作成するには、C++もしくはBPで`UPulldownListGeneratorBase`を継承し、`GetDisplayStrings`をオーバライドします。  
戻り値の配列がプルダウンメニューに列挙される内容になります。  

## 備考  

・PulldownContentsアセットはエディタ限定のアセットなため、パッケージにはクックされません。  

・UE4.26.2の時点ではエンジンコードに不具合があるため、UE4.26のみプルダウンメニューに検索欄を表示しないようにしています。
#### UE4.23 ~ UE4.25  
![E0Tj1waVUAQ0LBM](https://user-images.githubusercontent.com/51815450/117564840-056f9780-b0e9-11eb-806b-00db8bf125d9.png)
#### UE4.26.2  
![image](https://user-images.githubusercontent.com/51815450/117564900-6d25e280-b0e9-11eb-992c-5a63a6faa7d6.png)

## ライセンス

[MITライセンス](https://ja.wikipedia.org/wiki/MIT_License)

## 作者

[Naotsun](https://twitter.com/Naotsun_UE)

## 履歴

- (2021/05/09) v1.0   
  プラグインを公開
