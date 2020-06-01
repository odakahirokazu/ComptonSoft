# ComptonSoftのインストール

文: 2020-05-26 渡邊泰平
Updated: 2020-05-29 小高裕和

本文章はMacにComptonSoftをインストールする標準的な方法を解説する。
MacOS標準のシェルであるzshを使っていると仮定する。（その他のシェルを使う場合は、適宜同等の操作に読みかえること）

## 流れ

1. 開発環境を整える。
1. 必要なライブラリをHomebrew (Macのソフトウェアパッケージ管理システム) で入れる
1. Rubyのインストール
1. Geant4のインストール
1. ANLNextのインストール
1. ComptonSoftのインストール
1. ComptonSoftの例題を実行する

## 参考

https://github.com/odakahirokazu/ComptonSoft/blob/master/documentation/installation.md

## 具体的な手順

### 開発環境の整備

1. MacOSを最新版にする。(10.15.5 on 2020-05-29)
1. Xcode (Mac標準の開発環境) をApp Storeからインストールする。(11.5 on 2020-05-29)
1. Xcodeのインストールが完了したら、アプリケーションを実行し、自動的に出てくる追加インストールを行う。完了したらXcodeは終了する。
1. Command Line Toolsをインストールする。(ターミナルから次を実行)
```    
    $ xcode-select --install
```
1. Homebrewをインストールする。
https://brew.sh
を参照すること。このページに書いてあるインストールのためのコマンドをターミナルにコピペして実行すれば良い。
1. Homebrewが既に入っている場合は、情報とソフトウェアを最新にする。
```
    $ brew upgrade
```
1. X11 Window Systemを入れる。https://www.xquartz.org

### 必要なソフトウェアライブラリのインストール

homebrewを使う。

1. Homebrewの更新。
```
    $ brew upgrade
```
1. 必要なライブラリのインストール
```
    $ brew install cmake
    $ brew install boost
    $ brew install swig
    $ brew install xerces-c
    $ brew install root
    $ brew install cfitsio
```

### Rubyのインストール

Macには標準でRubyがインストールされているが、システムを汚さずに関連パッケージを入れるために、新規でインストールする。基本的にはhomebrewで入れれば良い。

1. Ruby version 2.6を入れる。
注意: 現時点では、最新版の2.7にはSWIGが対応していないため、１つ前のバージョンである2.6を入れる必要がある。
```
    $ brew install ruby@2.6
```
1. 環境変数の設定をする。.zshrcに以下の記述を追加する。
```
    export PATH="/usr/local/opt/ruby@2.6/bin:$PATH"
```
1. シェルを再起動する。（環境変数を反映させるため）
1. ParallelというRubyの並列化ユーティリティを入れる。
```
    $ gem install parallel
```

### Geant4のインストール

公式ページからソースコードを取得し、ビルドする。現時点では、10.05.p01をインストールすることを強く推奨する。（最新版10.6での正常動作は確認できていない。）

1. http://geant4.web.cern.ch/support/download_archive
のSource filesからダウンロード(ファイル名: `geant4.10.05.p01.tar.gz`)
1. Geant4をインストールするディレクトリを作る。例: Software/geant4
```
    $ mkdir -p Software/geant4
    $ cd Software/geant4
```
1. ここにソースファイルを持ってきて展開する。`<...>`は適切に置き換えること。
```
    $ mv <somewhere>/geant4.10.05.p01.tar.gz .
    $ tar xfvz geant4.10.05.p01.tar.gz
```
これでディレクトリ`geant4.10.05.p01`ができる。
1. 以下のようにコマンドで、ビルド・インストールを行う。cmakeのお勧めオプションを付けてある。`make`は`make -jN`としても良い (補足参照)。
```
    $ mkdir geant4.10.05.p01-build
    $ cd geant4.10.05.p01-build
    $ cmake ../geant4.10.05.p01 \
            -DCMAKE_INSTALL_PREFIX=../geant4.10.05.p01-install \
            -DCMAKE_PREFIX_PATH=/usr/local/opt/qt5 \
            -DGEANT4_USE_GDML=ON \
            -DGEANT4_USE_QT=ON \
            -DGEANT4_USE_OPENGL_X11=ON \
            -DGEANT4_USE_RAYTRACER_X11=ON \
            -DGEANT4_USE_NETWORKDAWN=ON
    $ make
    $ make install
```
1. データのダウンロードと展開。
データのディレクトリを作り、そこにData filesの全てのファイルをダウンロードして展開する。
```
    $ cd Software/geant4
    $ mkdir data
    $ cd data
    (ここにデータファイルを展開)
```
1. データディレクトリへのシンボリックリンクの作成。
```
    $ cd Software/geant4
    $ cd geant4.10.05.p01-install/share/Geant4-10.5.1
    $ ln -s ../../../data .
```
1. 環境変数の設定。.zshrcに以下を記述。インストール先が異なる場合は適宜読みかえること。
```
    export G4INSTALL=<somewhere>/Software/geant4/geant4.10.05.p01-install
    cd $G4INSTALL/bin
    . ./geant4.sh; export DYLD_LIBRARY_PATH=''
    cd -
```
シェルを再起動すると、全ての必要な環境変数が設定される。

#### 補足

- 並行コンパイルに対応しているので、`make`の代わりに
```
    $ make -jN (N: 並列数)
```
とするとビルド時間が短縮する。NはCPU-coreの数くらいが適当。

- ここでのディレクトリ構成は以下のようになっているはず。
```
$ pwd
<somewhere>/Software/geant4
$ ls -1
data/
geant4.10.05.p01/
geant4.10.05.p01-build/
geant4.10.05.p01-install/
geant4.10.05.p01.tar.gz
```


### ANLNextのインストール

参考: https://github.com/odakahirokazu/ANLNext#readme

1. ソフトウェアインストールの作業ディレクトリに行き、ANLNextのソースコード一式を取得する。
作業ディレクトリは適宜作成すること。
```
    $ cd <path/to/working-directory>
    $ git clone https://github.com/odakahirokazu/ANLNext.git
```
1. 以下のコマンドを実行し、ビルド・インストールする。`make`は`make -jN`としても良い (上記、Geant4の補足参照)。
```
    $ cd ANLNext
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make install
```
1. 環境変数`RUBYLIB`の設定。.zshrcに以下の記述を追加し、シェルを再起動する。
```
    export RUBYLIB=${HOME}/lib/ruby:${RUBYLIB}
```

#### 補足

- 上記の手順だと、標準のインストール先である、`$HOME`以下にインストールされ、ユーザーのホームディレクトリを汚染する。それを避けたい場合は、自分でインストール先を指定する必要がある。その場合、別途インストール先を環境変数に指定する必要がある。詳しくは公式のインストールの説明を参照。
```
    $ cmake .. -DCMAKE_INSTALL_PREFIX=<path/to/install>
```
この場合は、環境変数`RUBYLIB`の設定についても、適宜場所を変更すること。

## ComptonSoftのインストール

ANLNextとほぼ同様の手順でインストールできる。
参考: https://github.com/odakahirokazu/ComptonSoft#readme

1. ソフトウェアインストールの作業ディレクトリに行き、ComptonSoftのソースコード一式を取得する。
作業ディレクトリは適宜作成すること。ANLNextと共通でも良い。
```
    $ cd <path/to/working-directory>
    $ git clone https://github.com/odakahirokazu/ComptonSoft.git
```
1. 以下のコマンドを実行し、ビルド・インストールする。`make`は`make -jN`としても良い (上記、Geant4の補足参照)。
```
    $ cd ComptonSoft
    $ mkdir build
    $ cd build
    $ cmake .. -DCS_USE_FITSIO=ON
    $ make
    $ make install
```

#### 補足

- 上記の手順だと、標準のインストール先である、`$HOME`以下にインストールされ、ユーザーのホームディレクトリを汚染する。それを避けたい場合は、自分でインストール先を指定する必要がある。その場合、別途インストール先を環境変数に指定する必要がある。詳しくは公式のインストールの説明を参照。
```
    $ cmake .. -DCMAKE_INSTALL_PREFIX=<path/to/install>
```
この場合は、環境変数`RUBYLIB`も適切に設定する必要がある。

### ComptonSoftの例題を実行

1. ComptonSoftが正しくインストールできたらテストするために、例題を実行する。
```
    $ cd ComptonSoft
    $ cd examples/simulations/pixel_detector
    $ ./run_simulation.rb
```
これでシミュレーションが走り、出力ファイル`simulation.root`が生成さればOK。
`visualize_mass_model.rb`を実行すると、Geant4QtのWindowが開き、検出器のジオメトリを見ることができる。sessionに`exit`と打てば終了できる。

