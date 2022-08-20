# duvuvurkarpogeschel

MikanOS リパライン語版

## 計画書兼作業ログ

[ここ](https://docs.google.com/document/d/1wIOPxwwuKzM3AXjePmQmzAJn_1PvlHc2g7eBjgCJTak/edit#)

## ビルド方法

基本的に https://github.com/uchan-nos/mikanos-build を参考にするとよい。

```
source $HOME/osbook/devenv/buildenv.sh
```

は毎回走らせる必要があることに注意。

「apps ディレクトリにアプリ群を入れ、フォントなどのリソースをも含めたディスクイメージを作る」ための方法は、

```
APPS_DIR=cersvass RESOURCE_DIR=resource ./build.sh run
```

へと変更されている。

```
APPS_DIR=cersvass RESOURCE_DIR=resource ALPHABET=latin ./build.sh run
```

とすることにより、表示がラテンアルファベットへとすり替わる。

## 組み込みコマンド紹介

| コマンド名 | 内容 |
|------------|---------------------------|
| ksfnerfe   | ターミナルなしでアプリを開く |
| lkurf	     | 第一引数を出力 |
| lkurf &lex | 直前コマンドの終了コードを出力 |
| xel        | ファイルを開く |
| melfertal  | ファイルの一覧を表示 |
| ys         | ターミナル上の文字を全て消す |


## アプリ紹介

| アプリ名 | 内容 |
|---------|---------------------------|
| fla_leus | ファイルをページ送りで見る |
| fiurs    | マウスを追いかける目玉 |
| feucocar | ブロック崩しゲーム |
| therdaes | ファイルコピー |
| tarmzi   | 皇論之書を表示する |
| chaiss   | 線の表示デモ |
| cerxe    | 立方体の 3D 表示デモ |
| xel_aklu | 画像ビューア |
| kotielss | 点の表示デモ |
| mili     | 時間が経つのを待つ（単位は sinestususn）|
| mejfert  | ファイル内の特定バイトの個数を求める |
| lurfa_kr | テキストビューア |
| banmis   | バンミスを表示する |
| selsurle | マウスでお絵かき | 
| iska	   | OSをバグらせようと企む |
| ekcanaxm | 逆ポーランド記法電卓 |

なお、現状、iska kr_sysital とすると OS が本当にバグります。
これは MikanOS day30f のバグです。

<hr>

# mikanos
MikanOS: An educational operating system running on legacy free architecture.

## Files

- MikanLoaderPkg
    - The MikanOS loader as a UEFI Application
- kernel
    - The MikanOS kernel
- resource/__jpnipa.ttf
    - IPA gothic font file
- IPA_Font_License_Agreement_v1.0.txt
    - License agreement for IPA fonts

