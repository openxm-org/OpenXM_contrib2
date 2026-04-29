# BUILD_LINUX

この文書は、Linux 版 `asirgui_qt` をソースからビルドし、実行するための手順をまとめたものである。

## 1. 前提

以下を前提とする。

- `OpenXM` が同じワークスペース内にある
- `OpenXM` 全体のビルドが完了している
- `OpenXM/lib/libasir.a` が存在する

このワークスペースでは、標準的な配置は次である。

```text
.
├── OpenXM
└── OpenXM_contrib2/
    └── asirgui_qt
```

## 2. Qt とビルドツールの準備

Debian / Ubuntu 系では、少なくとも次を導入する。

```bash
sudo apt install qt6-base-dev qt6-webengine-dev qt6-webchannel-dev \
qt6-tools-dev qt6-tools-dev-tools cmake ninja-build
```

加えて、通常は C/C++ コンパイラが必要である。

```bash
sudo apt install build-essential
```

## 3. OpenXM 側の前提成果物

`asirgui_qt` のビルドでは、Qt 以外に OpenXM 側の以下を利用する。

- `OpenXM/include/`
- `OpenXM/lib/libasir.a`
- `OpenXM/lib/libgc.*`
- `OpenXM/lib/libmpfr.*`
- `OpenXM/lib/libmpc.*`
- `OpenXM/lib/libgmp.*`

つまり、Qt パッケージを入れるだけでは不十分で、OpenXM が先にビルド済みである必要がある。

`asirgui_qt` では `OpenXM_contrib2/asir2018` 側にも修正が入っているため、`libasir.a` を作り直した場合は次を再実行する。

```bash
cd OpenXM/src/asir2018
make NOX=y NOFEP=y
make install
```

特に、NOX `memory_*plot` による plot 表示や GUI からの `Ctrl+C` 中断を使うには、修正済み `libasir.a` が入っていることが前提になる。

## 4. Qt バージョンの確認

実際に利用される Qt 6 のバージョンは、次で確認できる。

```bash
pkg-config --modversion Qt6Core Qt6Widgets Qt6WebEngineWidgets Qt6WebChannel
qtpaths6 --query QT_VERSION
```

## 5. CMake 設定

プロジェクトルートへ移動してから CMake を実行する。

```bash
cd ~/projects
cmake -S OpenXM_contrib2/asirgui_qt -B OpenXM_contrib2/asirgui_qt/build
```

通常は `CMakeLists.txt` が既定で `../../OpenXM` を見に行くため、同じワークスペース配置なら追加指定は不要である。

もし `OpenXM` を別の場所に置いているなら、明示的に指定する。

```bash
cmake -S OpenXM_contrib2/asirgui_qt -B OpenXM_contrib2/asirgui_qt/build \
  -DOPENXM_HOME=/path/to/OpenXM
```

## 6. ビルド

```bash
cmake --build OpenXM_contrib2/asirgui_qt/build -j4
```

生成物:

- `OpenXM_contrib2/asirgui_qt/build/asirgui`
- `OpenXM_contrib2/asirgui_qt/build/test_history`
- `OpenXM_contrib2/asirgui_qt/build/test_engine`
- `OpenXM_contrib2/asirgui_qt/build/test_input_widget`

## 7. 実行

```bash
./OpenXM_contrib2/asirgui_qt/build/asirgui
```

実装上、バイナリは CMake 設定時の `OPENXM_HOME` を既定値として埋め込むため、標準配置なら追加の環境変数なしで起動できる。

実行中の補足:

- `Ctrl+C` で現在の計算を中断できる
- `cputime(1);` を実行すると、その後のコマンドの実行時間が transcript に表示される
- `cputime(0);` で時間表示を止める

## 8. テスト

変更後の標準確認コマンドは次である。

```bash
ctest --test-dir OpenXM_contrib2/asirgui_qt/build --output-on-failure
```

## 9. 注意点

### 9.1 KaTeX 表示

KaTeX 0.16.11 (JS, CSS, woff2 フォント) を `resources/katex/` にローカル同梱しており、オフライン環境でも数式表示が動作する。  
KaTeX の読み込みに失敗した場合は LaTeX 文字列でフォールバックする。

### 9.2 `@n` 参照

CLI 版 Asir の `@n` は libasir のライブラリモードではそのまま使えないため、GUI 側で `AsirGuiRefN` を使う代替実装をしている。

### 9.3 文字列結果の取得

`asir_ox_peek_cmo_string_length()` で必要バッファサイズを取得し、その長さ分だけ `asir_ox_pop_string()` で読み出す。  
`peek` が 0 以下を返すケースもあるため、その場合は 4096 バイトのフォールバックバッファで `asir_ox_pop_string()` を試みる。
