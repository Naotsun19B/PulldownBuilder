# PulldownBuilder 

![Plugin](https://user-images.githubusercontent.com/51815450/173223798-f60374a1-1d14-4ac2-93c1-bdbab9fe5278.PNG)

<!--ts-->
   * [概要](#概要)
   * [動作環境](#動作環境)
   * [インストール](#インストール)
   * [使い方](#使い方)
      * [C++を使用する場合](#C++を使用する場合)
      * [C++を使用しない場合](#C++を使用しない場合)
      * [PulldownListGenerator](#PulldownListGenerator)
      * [RowNameUpdater](#RowNameUpdater)
      * [ノード](#ノード)
   * [オプション](#オプション)
   * [備考](#備考)
   * [ライセンス](#ライセンス)
   * [作者](#作者)
   * [履歴](#履歴)
<!--te-->

## 概要  

このプラグインは専用アセットを使用して簡単にプルダウン表示する構造体を作成することができます。  
例えば、データテーブルのRowNameを文字列ではなく列挙型のようなプルダウンメニューで指定することができます。  

![PulldownDetails](https://user-images.githubusercontent.com/51815450/173223818-c8297c9c-6a0d-4e4a-938d-a15f55df9c49.PNG)

## 動作環境  

対象バージョン : UE4.24 ~ 5.3  
対象プラットフォーム :  Windows, Mac, Linux (ランタイムモジュールはプラットフォームの制限無し)   

## インストール  

プロジェクトのPluginsフォルダにPlugins/PulldownBuilderフォルダを入れてください。  
または、[マーケットプレイス](https://www.unrealengine.com/marketplace/ja/product/pulldown-builder)からインストールしてください。  
プラグインのインストール後に機能が使用できない場合は、プラグインが有効化されていない可能性がありますので、編集 > プラグイン からプラグインの有効のチェックが入っているかをご確認ください。

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
次に、ランタイムモジュール([`PulldownStruct`](https://github.com/Naotsun19B/PulldownBuilder/tree/master/Plugins/PulldownBuilder/Source/PulldownStruct))に含まれる[`FPulldownStructBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownStruct/Public/PulldownStruct/PulldownStructBase.h)を継承した構造体を定義します。  
プルダウンメニューで選択する文字列の他に変数を定義することができます。  
ここで定義した構造体がプルダウンメニューが表示される構造体になります。  

![CreatePulldownContents](https://user-images.githubusercontent.com/51815450/173223842-a5356544-b7ee-4979-9864-36986ee358ec.PNG)

続いて、ビルドしてエディタを起動し、コンテンツブラウザでPulldownContentsアセットを作成します。

![PulldownStructType](https://user-images.githubusercontent.com/51815450/173223858-12fca014-5fb8-44b1-bbce-1d07c363edde.PNG)

作成したアセットを開き、`Pulldown Struct Type`に先程定義した構造体を設定します。  
複数のPulldownContentsアセットに同一の構造体を設定することはできません。  

![PulldownListGenerator](https://user-images.githubusercontent.com/51815450/173223874-23a8d64b-545d-4fb4-8b89-b163a93df5fc.PNG)

次に、プルダウンメニューの元となるリストを構築するクラスを設定します。  
リストを構築するクラスについては下記の`PulldownListGenerator`の項目を参照してください。  

![Preview](https://user-images.githubusercontent.com/51815450/173223882-b88eba54-db31-4282-8b57-aa49f4ac4c1b.PNG)

ここまで設定すると`Preview`でこのPulldownContentsアセットによって構築されるプルダウンメニューを確認することができます。  
あとは、変数や関数の引数などで定義した構造体を使用すると自動的にプルダウンメニューが表示されるようになります。  

![PulldownPin](https://user-images.githubusercontent.com/51815450/173223894-288dc7c3-ca0b-47dc-b0c9-97e7999f6460.PNG)
![PulldownDetails](https://user-images.githubusercontent.com/51815450/173223818-c8297c9c-6a0d-4e4a-938d-a15f55df9c49.PNG)

### ・C++を使用しない場合

![CreatePulldownContents](https://user-images.githubusercontent.com/51815450/173223842-a5356544-b7ee-4979-9864-36986ee358ec.PNG)

まずは、コンテンツブラウザでPulldownContentsアセットを作成します。

![PulldownListGenerator](https://user-images.githubusercontent.com/51815450/173223874-23a8d64b-545d-4fb4-8b89-b163a93df5fc.PNG)

次に、プルダウンメニューの元となるリストを構築するクラスを設定します。  
リストを構築するクラスについては下記の`PulldownListGenerator`の項目を参照してください。  

![Preview](https://user-images.githubusercontent.com/51815450/173223882-b88eba54-db31-4282-8b57-aa49f4ac4c1b.PNG)

ここまで設定すると`Preview`でこのPulldownContentsアセットによって構築されるプルダウンメニューを確認することができます。  
あとは、変数や関数の引数などで`NativeLessPulldownStruct`を使用すると自動的にプルダウンメニューが表示されるようになります。  
C++で定義したものとは違い、プルダウンメニューの元となるPulldownContentsアセットを切り替えることができます。

![NativeLessPin](https://user-images.githubusercontent.com/51815450/173223970-d3d8e7e5-3f1c-4d9f-9293-71cdc35ebbe7.PNG)
![NativeLessDetails](https://user-images.githubusercontent.com/51815450/173223982-c93bd1dc-8470-4aca-906c-61617450cc00.PNG)

### ・PulldownListGenerator  

プルダウンメニューの元となるリストを構築するクラスとして`PulldownListGenerator`があります。  
標準で以下の3つの`PulldownListGenerator`が用意されています。

|              **クラス**               | **機能**                                                     | **ツールチップ**                                                                   |
|:----------------------------------:|------------------------------------------------------------|------------------------------------------------------------------------------|
|   DataTablePulldownListGenerator   | `SourceDataTable`に設定されたデータテーブルアセットのRowNameをプルダウンメニューに列挙します。 | データテーブルの行として使用されている構造体内にFString型の"PulldownTooltip"という名前の変数があった場合、その文字列を表示します。 |
|  StringTablePulldownListGenerator  | `SourceStringTable`に設定されたストリングテーブルアセットのKeyをプルダウンメニューに列挙します。 | 各項目の対応する文字列を表示します。                                                           |
|   NameArrayPulldownListGenerator   | `SourceNameArray`の要素をプルダウンメニューに列挙します。                      | 各項目のValueに設定されている文字列を表示します。                                                  |
| InputMappingsPulldownListGenerator | プロジェクト設定の入力で設定されている入力のマッピングの要素をプルダウンメニューに列挙します。            | 入力名に対応したボタンの名前を表示します。                                                        |

独自の`PulldownListGenerator`を作成するには、C++もしくはBPで [`UPulldownListGeneratorBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Public/PulldownBuilder/ListGenerators/PulldownListGeneratorBase.h) を継承し、`GetPulldownRows`をオーバライドします。  
戻り値の配列がプルダウンメニューに列挙される内容になります。  
引数に編集中の変数を持つオブジェクトと編集中の変数が渡されるため、それを元に生成するリストの内容を変更することができます。  

![BP_TestPulldown4PulldownListGenerator-GetPulldownRowsFromBlueprint](https://user-images.githubusercontent.com/51815450/177553308-a277f778-67d1-41aa-8495-2cba434ed423.png)

https://user-images.githubusercontent.com/51815450/177554749-425e7a4a-a17b-4202-be00-2c5b24244a73.mp4

### ・RowNameUpdater  

プルダウンメニューの元となるデータに更新があった場合(例えばデータテーブルのRowNameが変更されたなど)、既に使用されている値を新しい名前に置き換える仕組みがあります。  
標準で以下のアセットが対応しています。

| **アセット**  | **Updaterクラス**                                                                                                                                                                              |
|:---------:|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Blueprint | [`UBlueprintUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/PulldownBuilder/RowNameUpdaters/BlueprintUpdater.h)  |
| DataTable | [`UDataTableUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/PulldownBuilder/RowNameUpdaters/DataTableUpdater.h)  |
| DataAsset | [`UDataAssetUpdater`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Private/PulldownBuilder/RowNameUpdaters/DataAssetUpdater.h)  |

これら以外のアセットに対応するには、C++で [`URowNameUpdaterBase`](https://github.com/Naotsun19B/PulldownBuilder/blob/master/Plugins/PulldownBuilder/Source/PulldownBuilder/Public/PulldownBuilder/RowNameUpdaters/RowNameUpdaterBase.h) を継承して更新処理を実装する必要があります。

### ・ノード  

![CompareNodes](https://user-images.githubusercontent.com/51815450/223067854-f48de6a1-9766-4bdb-ad28-6944a9c86913.PNG)

同じ型のプルダウン構造体同士を比較するノードが利用できます。  
比較対象が`NativeLessPulldownStruct`の場合、ノードの詳細パネルから`Strict Comparison`というプロパティで比較時に`NativeLessPulldownStruct`の`Pulldown Source`も比較するかどうかを設定できます。  

![SwitchNodes](https://user-images.githubusercontent.com/51815450/223068042-7e42c339-aa07-4c57-bda1-9f8fe0d4665a.PNG)

プルダウン構造体のプルダウンメニューに表示される項目のスイッチノードが利用できます。  
対象のプルダウン構造体が`NativeLessPulldownStruct`の場合、ノードの詳細パネルから`Pulldown Contents`というプロパティでノードのピンの項目の元となるアセットを選択できます。  
また、対象のプルダウン構造体が`NativeLessPulldownStruct`の場合で、いずれの項目でもない値が渡された場合はデフォルトピンに出力されます。  

## オプション  

![EditorPreferences](https://user-images.githubusercontent.com/51815450/173224011-f82601a7-77e8-45fb-b74a-31ca17464163.PNG)

エディタの環境設定から設定できる項目は以下の通りです。

| **セクション**  | **項目**                                     | **説明**                                                              |
|------------|--------------------------------------------|---------------------------------------------------------------------|
| Appearance | Panel Size                                 | プルダウンメニューのパネルサイズを指定します。                                             |
|            | Is Select when Double Click                | このフラグがtrueの場合、プルダウンメニューの項目を選ぶ時にダブルクリックをする必要があります。                   |
|            | Should Inline Display When Single Property | このフラグがtrueの場合、プロパティが一つだけのプルダウン構造体をインライン表示します。                       |
|            | Notification Severity                      | このプラグインによって発行される通知のメッセージログにフォーカスする重大度を指定します。                        |
| Redirect   | Should Update When Source Row Name Changed | RowNameUpdaterを使ったプルダウンメニューの自動更新処理を行うかを指定します。                       |
|            | Active Row Name Updater                    | 有効化するRowNameUpdaterのクラスを指定します。ここで設定されているRowNameUpdaterのみが更新処理を行います。 |

## 備考  

・PulldownContentsアセットはエディタ限定のアセットなため、パッケージにはクックされません。

## ライセンス

[MITライセンス](https://ja.wikipedia.org/wiki/MIT_License)

## 作者

[Naotsun](https://twitter.com/Naotsun_UE)

## 履歴

- (2023/09/07) v2.3  
  UE5.3に対応しました  
  ランタイムで使用される値とエディタ上のプルダウンメニューに表示されるデータを別々に設定できるようにしました  
  プルダウン構造体向けの`LexFromString`関数を追加しました

- (2023/06/09) v2.2  
  プルダウン構造体の拡張詳細パネルの登録が行われない不具合を修正しました  
  エディタ起動時にレベルブループリントに配置されたノードで不要な更新が行われる不具合を修正しました  
  バージョン分離マクロに再定義の対策を追加しました  
  プルダウン構造体向けの`LexToString`関数を追加しました  

- (2023/05/16) v2.1  
  エディタ終了時にクラッシュする不具合を修正しました  
  プルダウン構造体が利用可能かどうかを示すマクロを追加しました  
  UE5.2に対応しました  

- (2023/03/06) v2.0  
  `NativeLessPulldownStruct`用の比較ノードを追加しました  
  プルダウン構造体用のスイッチノードを追加しました  

- (2023/02/01) v1.9  
  エディタ起動時にクラッシュする不具合を修正しました  
  ジェネリックの不等式ノードがプルダウン構造体の不等式ノードに変換されてしまう不具合を修正しました  
  グラフピンのデフォルト値が変更された時に表示が更新されない不具合を修正しました  
  グラフピンのコンテキストメニューからデフォルト値をコピーまたはペーストできるようになりました  
  グラフピンまたは詳細パネルのコンテキストメニューから変数のプルダウン構造体の元となるPulldownContentsアセットを開けるようになりました  

- (2022/11/08) v1.8  
  エディタ言語が英語以外の場合にノードを検索するとクラッシュする問題を修正しました  
  エディタ起動時にピンの値が無効なデータになる不具合を修正  
  プルダウンメニューの検索欄が正常に動作しない不具合を修正  
  UE5.1に対応しました

- (2022/07/07) v1.7  
  `PulldownListGenerator`の`GetPulldownRows`の引数に編集中の変数を持つオブジェクトと編集中の変数を渡すようにしました  

- (2022/06/12) v1.6   
  プルダウン構造体同士の比較ブループリントノードを追加しました  
  外観に関わる設定は個人ごとに、リダイレクト処理に関する設定はプロジェクト共有で保存されるようにしました  
  入力の設定からプルダウンのリストを生成する`PulldownListGenerator`を追加しました  

- (2022/04/06) v1.5   
  Strict Includesでビルドした際に発生するエラーを解消しました

- (2021/10/17) v1.4   
  UE5に対応しました  

- (2021/07/29) v1.3   
  プルダウンメニューウィジェットを大幅に改善し、各項目にツールチップを表示できるようにしました  

- (2021/06/02) v1.2   
  プルダウンメニューの元となるデータに更新があった場合に、既に使用されている値を新しい名前に置き換える機能を追加しました  

- (2021/05/29) v1.1   
  UserDefinedStructをPulldownContentsアセットとして使用してデータテーブルを指定すると正しく動作しない問題を修正しました  

- (2021/05/09) v1.0   
  プラグインを公開  
