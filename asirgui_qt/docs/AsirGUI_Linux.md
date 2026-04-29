# AsirGUI Linux 実装計画

## 1. 概要

Linux 版は、`libasir.a` を直接リンクし、Qt 6.8 Widgets で GUI を構築する最初の実装対象である。  
数式表示は `QWebEngineView` 上の KaTeX を用い、plot 系は `libasir.a` の `memory_*plot` を優先利用して Qt 側で表示する。  
実行中断は GUI から `Ctrl+C` で行い、`cputime(1);` / `cputime(0);` も GUI からそのまま使えるようにする。

- GUI: Qt 6.8 Widgets
- 計算エンジン: `libasir.a` をインプロセスで利用
- 実行方式: 専用ワーカースレッド
- 数式描画: KaTeX (`QWebEngineView`)
- LaTeX 変換: `print_tex_form()` + `names.rr`
- グラフ描画: `memory_ifplot`, `memory_conplot`, `memory_plot`, `memory_polarplot`

## 2. 目標と方針

Linux 版では、まず次を安定動作させる。

1. 通常の Asir コマンドを REPL 風 GUI で実行できること
2. 文字結果と数式結果を別表示できること
3. `ifplot/conplot/plot/polarplot` を Plot タブに表示できること
4. CLI 風の `[n]` 表示と `@n` 参照を GUI 上で再現すること

`open_canvas`, `clear_canvas`, `draw_obj`, `draw_string` は別設計が必要なため、Linux 版初期計画には含めない。

## 3. アーキテクチャ

```text
┌──────────────────────────────────────────────────────┐
│                AsirGUI (Qt 6.8 Widgets)              │
│                                                      │
│  ┌────────────────────────────────────────────────┐  │
│  │ テキスト履歴表示                               │  │
│  │ QPlainTextEdit                                 │  │
│  └────────────────────────────────────────────────┘  │
│                                                      │
│  ┌────────────────────────────────────────────────┐  │
│  │ 結果タブ                                        │  │
│  │  - Math: QWebEngineView + KaTeX               │  │
│  │  - Plot: QScrollArea + QImage                  │  │
│  └────────────────────────────────────────────────┘  │
│                                                      │
│  ┌────────────────────────────────────────────────┐  │
│  │ 入力欄                                          │  │
│  │ InputWidget (Emacs 風キーバインド)             │  │
│  └────────────────────────────────────────────────┘  │
│                                                      │
│  ┌────────────────────────────────────────────────┐  │
│  │ MainWindow                                      │  │
│  │  - メニュー                                     │  │
│  │  - 表示モード切替                               │  │
│  │  - Plot 表示設定                                │  │
│  │  - ログ保存                                     │  │
│  │  - @n 展開                                      │  │
│  └────────────────────────────────────────────────┘  │
│                                                      │
│  ┌────────────────────────────────────────────────┐  │
│  │ AsirEngine / AsirWorker                         │  │
│  │  - asir_ox_init / execute_string / pop         │  │
│  │  - print_tex_form()                             │  │
│  │  - memory_*plot → BYTEARRAY → QImage           │  │
│  └────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────┘
```

## 4. ディレクトリ構成

```text
OpenXM_contrib2/asirgui_qt/
├── README.md
├── docs/
│   ├── AsirGUI_Linux.md
│   ├── AsirGUI_Linux_impl.md
│   ├── BUILD_LINUX.md
│   └── libasir_nox.md
├── CMakeLists.txt
├── core/
│   ├── AsirEngine.h
│   ├── AppConfig.h
│   ├── AppConfig.cpp
│   ├── CommandHistory.h
│   ├── CommandHistory.cpp
│   ├── LocalAsirEngine.h
│   └── LocalAsirEngine.cpp
├── desktop/
│   ├── common/
│   │   ├── MainWindow.h
│   │   ├── MainWindow.cpp
│   │   ├── InputWidget.h
│   │   ├── InputWidget.cpp
│   │   ├── MathView.h
│   │   ├── MathView.cpp
│   │   ├── PlotView.h
│   │   └── PlotView.cpp
│   ├── linux/
│   │   └── main.cpp
│   ├── windows/
│   └── macos/
├── resources/
│   ├── math_render.html
│   ├── resources.qrc
│   └── katex/
│       ├── katex.min.js
│       ├── katex.min.css
│       └── fonts/          (woff2 × 20)
├── tests/
│   ├── test_engine.cpp
│   ├── test_history.cpp
│   └── test_input_widget.cpp
├── mobile/
└── third_party/
```

## 5. コア設計

### 5.1 AsirEngine

`AsirEngine` は Qt の signal/slot ベースの抽象インターフェースとする。

```cpp
struct AsirResult {
    QString command;
    QString text;
    QString latex;
    QString mathPlainText;
    QImage plotImage;
    QString plotSummary;
    QString timingText;
    bool isError = false;
    struct AxisInfo {
        bool valid = false;
        double xMin = 0.0; double xMax = 0.0;
        double yMin = 0.0; double yMax = 0.0;
    };
    AxisInfo axes;
};

class AsirEngine : public QObject {
    Q_OBJECT
public:
    virtual bool initialize() = 0;
    virtual void shutdown() = 0;
    virtual void executeString(const QString &command) = 0;
    virtual void interruptCurrent() = 0;
    virtual bool isReady() const = 0;

signals:
    void resultReady(const AsirResult &result);
    void errorOccurred(const QString &message);
    void engineReady();
};
```

`resultReady` は `AsirResult` 構造体 1 つで返す。構造体は `AsirEngine.h` に定義する。  
`AsirResult::isError` を UI 側でも参照し、エラー判定ロジックを `MainWindow` に重複させない。

### 5.2 LocalAsirEngine / AsirWorker

`AsirWorker` を専用 `QThread` に移し、`libasir.a` の呼び出しをそのスレッドに閉じ込める。

実行フロー:

1. `asir_ox_init()` で初期化
2. `load("names.rr");` を実行
3. 入力コマンドを正規化
4. plot 系か通常式かを判定
5. 構文エラーのように library mode で `stderr` に出るだけの診断があれば回収する
6. 必要なら `cputime()` 状態を更新し、`timingText` を構築する
7. 結果を `AsirResult` 構造体にまとめて返す

中断:

- GUI の `Ctrl+C` または `Run -> Interrupt` から `interruptCurrent()` を呼ぶ
- `asir_ox_init()` の library mode 初期化で有効になる `SIGUSR1` 経路を使い、安全に `return to toplevel` へ戻す
- `pari(...)` 実行中も同じ経路で中断し、その後のコマンド実行を継続可能にする
- 中断後は `ox_pari` を残さず終了できる状態へ戻す

キャンセル:

- `AsirWorker` は `std::atomic<bool>` によるキャンセルフラグを持つ
- GUI renderer による plot 描画ループ内でフラグを検査し、早期離脱する
- `shutdown()` 時にはキャンセル要求を送って、長時間のフォールバック描画を止められるようにする
- `QMutex` は `execute()` 全体ではなく、`asir_ox_*` 呼び出し区間だけで保持する

### 5.3 設定ファイル

実行時設定は `AppConfig` で管理する。

優先順位:

1. `asirgui` 実行ファイルと同じディレクトリの `.asirguirc`
2. `~/.config/asirgui/config`

`.asirguirc` が存在すればそれを使い、存在しなければ `~/.config/asirgui/config` を保存先として使う。

保存対象:

- `OpenXM_HOME`
- `ASIR_LIBDIR`
- `ASIR_CONTRIB_DIR`
- Plot タブの表示設定
- 入力欄フォント
- 出力欄フォント
- `Split View` における Text/結果タブの分割サイズ

### 5.4 plot 系

対象:

- `ifplot`
- `conplot`
- `plot`
- `polarplot`

基本方針:

1. 入力から plot 関数を検出する
2. 可能なら `memory_*plot(...)` を内部実行する
3. CMO の `[width,height,bytearray]` を `QImage` に復元する
4. x/y 範囲は GUI 側で保持し、軸や目盛りに使う
5. NOX `libasir.a` では `ox_plot` を起動せず、`memory_*plot` のみで完結させる
6. 失敗時のみ GUI renderer にフォールバックする

引数解釈は Asir 本体仕様に合わせる。

- `ifplot(func [,geometry] [,xrange] [,yrange])`
- `conplot(func [,geometry] [,xrange] [,yrange] [,zrange])`
- `plot(func [,geometry] [,xrange])`
- `polarplot(func [,geometry] [,thetarange])`

ここで `geometry` は `[width,height]`、範囲指定は `[x,a,b]`, `[y,a,b]`, `[t,a,b]` の形式とする。  
GUI 側では、引数の位置だけでなく内容で geometry と範囲指定を判定し、`plot(x^2-x^3,[400,400],[x,-4,4]);` のように geometry を先に書いた場合でも、ユーザが指定した範囲を保持する。

`open_canvas` 系は本計画の対象外とする。

### 5.5 `@n` 参照

CLI の `@n` をそのまま使わず、GUI 側で次を行う。

1. 実行ごとに `[n]` を付番
2. 参照可能な式は内部変数 `AsirGuiRefN` に保存
3. 後続コマンド中の `@n` を `AsirGuiRefN` に展開
4. `@@` は「直前の参照可能結果」として、最後に保存された `AsirGuiRefN` に展開

補足:

- `@@` は Asir の仕様に合わせて直前結果参照として扱う
- 文字列リテラル中の `@n` / `@@` は展開しない
- 参照可能な結果がまだ存在しない場合、`@@` はそのまま Asir へ渡す (library mode の parser は `@@` を受理しないため、その場合は `syntax error` が返る)
- `@@` の展開時は、`m_lastReferenceableId` が現在の `m_referenceableIds` 集合に含まれていることも確認し、片方だけが残るような状況でも誤った `AsirGuiRefN` を送らない

## 6. UI 設計

### 6.1 MainWindow

現在の Linux 版では、左右分離ではなく REPL 風レイアウトを採用する。

- 上部: テキスト履歴
- 中央: 結果タブ (`Math`, `Plot`)
- 下部: 入力欄

`Split View` の既定サイズは Text ペインと結果タブが 1:1 になるようにする。  
ユーザが分割バーを動かした場合、そのサイズは設定ファイルへ保存し、次回起動時に復元する。

コマンド実行後は、`Math` タブまたは `Plot` タブへ自動切替・自動スクロールした後、キャレットを Asir コマンドエディタへ戻す。

コマンド終端の表示仕様:

- 終端が「記号なし」または `;` のときは、結果を表示する
- 終端が `;;` または `$` のときは、結果を表示しない
- 非表示終端では `@n` 参照も作らない
- `@@` は最後の参照可能結果だけを参照し、非表示終端のコマンドは対象に含めない
- ただし、エラーは終端に関係なく表示する
- `cputime(1)` が有効なときの計算時間も終端に関係なく表示する (CLI 版と同じ挙動)

非表示終端では、UI 側で結果を隠すだけでなく、エンジン側でも LaTeX / Math 用プレーンテキスト生成を抑制する。  
これにより、`nd_gr(... )$` のようなケースで不要な `print_tex_form()` や Math タブ描画へ進みにくくする。

構文エラー表示:

- `yyerror()` 由来の構文エラー、`pvar.c` の `undeclared function` / `undeclared variable` / `conflicting declarations` は library mode では `stderr` にのみ出ることがある
- GUI 側でその診断を回収し、結果スタックから pop した `0` と合わせて
  `syntax error`
  `0`
  のように 2 行で Output ペインへ表示する (`0` はパース失敗時に結果スタックへ残る値)
- 診断行に含まれる `"string", near line N: ` はライブラリモード固有のプレースホルダなので除去する
- `load("foo.rr")` 経由で発生したエラーは実ファイル名・行番号を保持したまま表示する
- plot 系 (`plot(^2-1);` のような構文エラー) も同じ経路で `syntax error\n0` にそろえる
- エラー時にデバッグウィンドウへだけ出て終わる挙動は避ける

`quit;`, `bye;`, `end;` は GUI 終了コマンドとして扱い、`File -> Exit` と同じ経路で AsirGUI を閉じる。  
これらは `libasir.a` へは送らない。

### 6.2 InputWidget

複数行の「Asir コマンドエディタ」として実装する。

基本仕様:

- `Enter`: 改行
- `Shift+Enter`: 実行
- 必要時に縦横スクロールバーを表示
- `QSplitter` により入力欄の高さをマウスで調整可能
- 自動インデント
  - 開き波括弧 `{` の次の行は 4 スペース深くする
  - `}` 入力時は、その括弧自体を 1 段左へ戻して挿入する
  - `}` の直後に `Enter` を押しても、さらに左へ戻さない

キーバインド:

- `Ctrl+A`, `Ctrl+E`, `Ctrl+B`, `Ctrl+F`
- `Ctrl+D`, `Ctrl+H`, `Ctrl+K`, `Ctrl+U`, `Ctrl+W`
- `Ctrl+P`, `Ctrl+N`
- `Alt+B`, `Alt+F`, `Alt+Backspace`
- `Tab`: 補完
- 実行中の `Ctrl+C`: 計算中断ダイアログ

履歴仕様:

- エディタが空の状態で `↑` を押すと、`Ctrl+P` と同様に履歴モードへ入る
- エディタに何か入力されている通常時の `↑`, `↓` は複数行エディタのカーソル移動に使う
- `Ctrl+P` または `Ctrl+N` で履歴モードに入る
- 履歴モード中のみ `↑`, `↓` で履歴を前後に移動する
- 履歴モード中は `Ctrl+P`, `Ctrl+N` でも前後移動できる
- 履歴モードは、他のキー入力を行うと抜ける

補完仕様:

- 補完対象は「主要 Asir 関数名の固定リスト」と「現在のエディタ内容・履歴中の識別子」
- 識別子は `[A-Za-z_][A-Za-z0-9_]*` に一致するトークンを候補化する
- `Tab` で候補が 1 つなら確定し、複数なら最長共通接頭辞まで補完する

履歴展開仕様:

- `!!`: 直前の履歴を展開して実行
- `!prefix`: `prefix` で始まる直近履歴を展開して実行
- `!prefix:p`: 展開だけ行い、エディタへ戻して実行しない
- `!prefix:p` の直後は、その `prefix` に一致する履歴だけを `↑`, `↓`, `Ctrl+P`, `Ctrl+N` でたどれるようにする

### 6.3 MathView

`QWebEngineView` + KaTeX で表示する。

- 各数式ブロックの見出しは `"[n] 実行コマンド"` とする
- KaTeX が使えない場合は LaTeX 文字列でフォールバックする
- KaTeX 資産 (JS, CSS, フォント) は `resources/katex/` にローカル同梱し、`qrc` 経由で読み込む
- HTML は `resources/math_render.html` に置く
- C++ → JavaScript のパラメータ受け渡しは JSON 配列経由で行い、インジェクションを防止する
- 非 plot コマンド実行後は `Math` タブへ切り替え、最新結果まで自動スクロールする
- スクロール後はキャレットを Asir コマンドエディタへ戻す
- `pari(...)` は KaTeX に通さず、等幅フォントのプレーンテキストで表示する

Math タブ保護仕様:

- 巨大結果は KaTeX に送る前に C++ 側で判定し、必要なら Math タブでの数式レンダリングを省略する
- 高リスク関数は `nd_gr(...)`, `gr(...)`, `dp_gr(...)`, `fglm(...)`, `syz(...)`
- 高リスク関数は `LaTeX > 2000` または `Text > 4000` で自動省略する
- 一般コマンドは `LaTeX > 100000` または `Text > 200000` の極端に大きい場合だけ省略する
- 自動省略時は Math タブに次を表示する
  - 数式レンダリングを省略した旨
  - `LaTeX` / `Text` の文字数
  - 判定理由
  - 必要時のみ `強制レンダリング` ボタン
- `強制レンダリング` は、その結果 1 件だけをユーザが明示的に KaTeX 描画させる操作とする

目的:

- `nd_gr(C,vars(C),31991,0)$` のような巨大な多項式集合で Math タブがフリーズしたような状態になることを防ぐ
- Output ペインと Asir コマンドエディタを、KaTeX の重い描画とは独立に継続利用できるようにする
- `Ctrl+C` が効かない WebEngine 内 KaTeX 描画の暴走を、事前ガードで回避する

### 6.4 PlotView

`QImage` を履歴形式で表示する。

機能:

- Plot 履歴を積み上げ表示
- 見出しは `"[n] ifplot ... -> 300x300, ..."` 形式
- 前景色、背景色、線幅変更
- Fit Width / Zoom In / Zoom Out / Reset Zoom
- `Show Axes`
- `Show Axis Labels`
- plot 実行後は `Plot` タブへ切り替え、最新グラフの先頭位置まで自動スクロールする
- スクロール後はキャレットを Asir コマンドエディタへ戻す

描画仕様:

- x 軸の目盛りは x 軸の下側
- y 軸の目盛りは y 軸の左側
- 軸が中央にある場合も、その軸位置に追従

### 6.5 表示モード

View メニューから次を切り替える。

1. `Text Only`
2. `Math Only`
3. `Split View`

`Split View` では、上に text、下に結果タブを表示する。

### 6.6 Help メニュー

Help メニューには次を置く。

1. `Editor Help`
2. `User's Manual`
3. `Official Site`
4. `About`

`Editor Help` はスクロール可能なダイアログで表示し、キーバインド、`Ctrl+C` による中断、補完仕様を参照できるようにする。  
`User's Manual` と `Official Site` は既定ブラウザで開く。Linux ネイティブでは `QDesktopServices` / `xdg-open` を使い、WSL 環境では Windows 側ブラウザへ URL を確実に渡すため `powershell.exe` または `cmd.exe /c start` を優先する。

## 7. ビルド環境

### 必要パッケージ

```bash
sudo apt install qt6-base-dev qt6-webengine-dev qt6-webchannel-dev \
                 qt6-tools-dev qt6-tools-dev-tools cmake ninja-build
```

加えて、OpenXM 側がビルド済みであり、少なくとも次が利用可能であること。

- `OpenXM/lib/libasir.a`
- `OpenXM/lib/libgc.*`
- `OpenXM/lib/libgmp.*`
- `OpenXM/lib/libmpfr.*`
- `OpenXM/lib/libmpc.*`
- `OpenXM/include/...`

## 8. CMake 方針

- `Qt6 6.8` 以上を要求
- `core/` を static library として分離
- Linux エントリポイントは `desktop/linux/main.cpp`
- `resources/resources.qrc` を組み込む
- `tests/test_history.cpp`, `tests/test_engine.cpp`, `tests/test_input_widget.cpp` を CTest に登録
- 設定ファイル管理は `core/AppConfig.cpp` にまとめる

## 9. 起動時初期化

1. Linux 固有の `QWebEngine` 設定を行う
2. Qt Application を起動
3. `AsirWorker` を起動
4. `asir_ox_init()` 実行
5. `load("names.rr");` 実行
6. MainWindow を表示

## 10. テスト方針

最低限の標準確認:

- `fctr(x^4-1);`
- LaTeX 取得
- `cputime(1);` / `cputime(0);`
- `ifplot(...)`
- 連続する `ifplot(...)` と geometry 指定付き `ifplot(...)`
- `plot(x^2-x^3,[400,400],[x,-4,4]);` のように geometry を先に書いた `plot(...)`
- `polarplot(...)`
- geometry を先に書いた `polarplot(...)`
- `1+1$`, `fctr(x^4-1);;` のような非表示終端では結果を表示しないこと
- 非表示終端では `@n` 参照を作らないこと
- `@@` が直前の参照可能結果を指すこと
- `fctr(^3-1);` のような構文エラーが GUI 上で `syntax error` と `0` を表示すること
- `cyclic(5);` のような通常コマンドでは Math タブ保護が過剰に発動しないこと
- `nd_gr(C,vars(C),31991,0)$` のような高リスク関数では Math タブで省略表示と `強制レンダリング` が出ること
- `pari(factor,2^2^7+1);` 実行時に `The object at the stack top is invalid as a CMO.` が出ないこと
- `pari(factor,2^2^9+1);` の中断と、その後の `1+1;`
- `ox_launch_nox()` / `ox_shutdown()` 実行後の継続利用
- 履歴移動

標準コマンド:

```bash
ctest --test-dir OpenXM_contrib2/asirgui_qt/build --output-on-failure
```

## 11. 既知の技術的課題

| 課題 | 方針 | 状態 |
|------|------|------|
| `libasir.a` のスレッド安全性 | 専用ワーカースレッドに閉じ込める | 対応済み |
| `print_tex_form()` の不安定さ | 失敗時は文字列フォールバック | 対応済み |
| `pari(...)` の文字列結果確認時に出る不要な CMO 警告 | `asir_ox_peek_cmo_string_length()` を `asir_ox_pop_string()` と整合する実装へ修正 | 対応済み |
| KaTeX のオフライン利用 | `resources/katex/` にローカル同梱、`qrc` 経由で読み込み | 対応済み |
| 巨大な多項式集合を Math タブへ送った際のフリーズ | 高リスク関数に対する事前サイズ判定と省略表示、必要時のみ `強制レンダリング` | 対応済み |
| `QWebEngineView` の GPU 初期化失敗 | Linux 起動時に software OpenGL と `--disable-gpu` を設定 | 対応済み |
| GUI からの安全な中断 | `Ctrl+C` から library mode の `SIGUSR1` 経路へ接続し、`return to toplevel` へ戻す | 対応済み |
| `ifplot` 初回・連続実行時の余計な遅延 | NOX `memory_*plot` から `ox_plot` 起動経路を外す | 対応済み |
| `open_canvas` 系 | NOX 用メモリキャンバス API を別設計する | 未着手 |
