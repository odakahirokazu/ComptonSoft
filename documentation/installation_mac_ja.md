# ComptonSoftのインストール

- 文: 2020-05-26 渡邊泰平
- Updated: 2020-05-29 小高裕和
- Updated: 2020-07-01 小高裕和
- Updated: 2021-04-07 小高裕和
- Updated: 2022-05-24 小高裕和
- Updated: 2023-04-07 小高裕和

本文章はMacにComptonSoftをインストールする標準的な方法を解説する。
MacOS標準のシェルであるzshを使っていると仮定する。（その他のシェルを使う場合は、適宜同等の操作に読みかえること）

## 流れ

1. 開発環境を整える。
2. 必要なライブラリをHomebrew (Macのソフトウェアパッケージ管理システム) で入れる
3. Rubyのインストール
4. Geant4のインストール
5. ANLNextのインストール
6. ComptonSoftのインストール
7. ComptonSoftの例題を実行する

## 参考

https://github.com/odakahirokazu/ComptonSoft/blob/master/documentation/installation.md

## 具体的な手順

### 開発環境の整備

1. MacOSを最新版にする。(13.3 on 2023-04-07)
2. Xcode (Mac標準の開発環境) をApp Storeからインストールする。(14.3 on 2022-05-27)
3. Xcodeのインストールが完了したら、アプリケーションを実行し、自動的に出てくる追加インストールを行う。完了したらXcodeは終了する。
4. Command Line Toolsをインストールする。(ターミナルから次を実行)
```    
    $ xcode-select --install
```
5. Homebrewをインストールする。
https://brew.sh
を参照すること。このページに書いてあるインストールのためのコマンドをターミナルにコピペして実行すれば良い。
6. Homebrewが既に入っている場合は、情報とソフトウェアを最新にする。
```
    $ brew upgrade
```
7. X11 Window Systemを入れる。https://www.xquartz.org

### 必要なソフトウェアライブラリのインストール

homebrewを使う。

1. Homebrewの更新。
```
    $ brew upgrade
```
2. 必要なライブラリのインストール
```
    $ brew install cmake
    $ brew install boost
    $ brew install swig
    $ brew install xerces-c
    $ brew install root
    $ brew install cfitsio
    $ brew install qt@5
```

#### 補足
- Intel Macではhomebrewのインストール先は`/usr/local`だが、新しいAppleのCPUを搭載したMacでは`/opt/homebrew`となるので注意。


### Rubyのインストール

Macには標準でRubyがインストールされているが、システムを汚さずに関連パッケージを入れるために、新規でインストールする。基本的にはhomebrewで入れれば良い。

1. Rubyを入れる。
```
    $ brew install ruby
```
2. 環境変数の設定をする。.zshrcに以下の記述を追加する。
```
    export PATH="/usr/local/opt/ruby/bin:$PATH"
```
3. シェルを再起動する。（環境変数を反映させるため）
4. ParallelというRubyの並列化ユーティリティを入れる。
```
    $ gem install parallel
```

### Geant4のインストール

公式ページからソースコードを取得し、ビルドする。

1. http://geant4.web.cern.ch/support/download_archive
のSource filesからダウンロード(ファイル名: `geant4-v11.1.1.tar.bz2`)
2. Geant4をインストールするディレクトリを作る。例: Software/geant4
```
    $ mkdir -p Software/geant4
    $ cd Software/geant4
```
3. ここにソースファイルを持ってきて展開する。`<...>`は適切に置き換えること。
```
    $ mv <somewhere>/geant4-v11.1.1.tar.gz .
    $ tar xfvj geant4-v11.1.1.tar.gz
```
これでディレクトリ`geant4-v11.1.1`ができる。
4. 以下のようにコマンドで、ビルド・インストールを行う。cmakeのお勧めオプションを付けてある。`make`は`make -jN`としても良い (補足参照)。
```
    $ mkdir geant4-v11.1.1-build
    $ cd geant4-v11.1.1-build
    $ cmake ../geant4-v11.1.1 \
            -DCMAKE_INSTALL_PREFIX=../geant4-v11.1.1-install \
            -DCMAKE_PREFIX_PATH=/usr/local/opt/qt5 \
            -DGEANT4_USE_GDML=ON \
            -DGEANT4_USE_QT=ON \
            -DGEANT4_USE_OPENGL_X11=ON \
            -DGEANT4_USE_RAYTRACER_X11=ON
    $ make
    $ make install
```
5. データのダウンロードと展開。
データのディレクトリを作り、そこにData filesの全てのファイルをダウンロードして展開する。
```
    $ cd Software/geant4
    $ mkdir data
    $ cd data
    (ここにデータファイルを展開)
```
6. データディレクトリへのシンボリックリンクの作成。
```
    $ cd Software/geant4
    $ cd geant4-v11.1.1-install/share/Geant4-11.1.1
    $ ln -s ../../../data .
```
7. 環境変数の設定。.zshrcに以下を記述。インストール先が異なる場合は適宜読みかえること。
```
    export G4INSTALL=<somewhere>/Software/geant4/geant4-v11.1.1-install
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
geant4-v11.1.1/
geant4-v11.1.1-build/
geant4-v11.1.1-install/
geant4-v11.1.1.tar.bz2
```


### ANLNextのインストール

参考: https://github.com/odakahirokazu/ANLNext#readme

1. ソフトウェアインストールの作業ディレクトリに行き、ANLNextのソースコード一式を取得する。
作業ディレクトリは適宜作成すること。
```
    $ cd <path/to/working-directory>
    $ git clone https://github.com/odakahirokazu/ANLNext.git
```
2. 以下のコマンドを実行し、ビルド・インストールする。`make`は`make -jN`としても良い (上記、Geant4の補足参照)。
```
    $ cd ANLNext
    $ mkdir build
    $ cd build
    $ cmake ..
    $ make
    $ make install
```
3. 環境変数`RUBYLIB`の設定。.zshrcに以下の記述を追加し、シェルを再起動する。
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
2. 以下のコマンドを実行し、ビルド・インストールする。`make`は`make -jN`としても良い (上記、Geant4の補足参照)。
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
