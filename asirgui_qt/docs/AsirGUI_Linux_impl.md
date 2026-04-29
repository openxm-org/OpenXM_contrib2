# AsirGUI Linux 実装記録

この文書は、[AsirGUI_Linux.md](AsirGUI_Linux.md) に対応する Linux 版の実装結果をまとめたものである。  
2026-04-02 時点の実装済み項目、追加仕様、既知の制約を記録する。

## 1. 実装到達点

Linux 版は、日常的な Asir 実行と GUI 開発継続に使える初期版として動作している。

現在できること:

- Asir コマンド実行
- REPL 風の文字履歴表示
- `print_tex_form()` による KaTeX 表示
- `ifplot/conplot/plot/polarplot` の Plot タブ表示
- `[n]` 表示と `@n` 参照
- `load("...");` によるスクリプト読込
- ログ保存
- 入力欄と文字ペインのフォント変更
- Plot の色、線幅、軸、目盛り、ズーム設定
- 複数行入力、自動インデント、履歴展開、補完を含む入力エディタ
- `.asirguirc` / `~/.config/asirgui/config` による設定保存
- `cputime(1);` / `cputime(0);` による実行時間表示
- `Ctrl+C` による安全な計算中断

## 2. 現在の構成

```text
OpenXM_contrib2/asirgui_qt/
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
│   │   ├── InputWidget.h
│   │   ├── InputWidget.cpp
│   │   ├── MainWindow.h
│   │   ├── MainWindow.cpp
│   │   ├── MathView.h
│   │   ├── MathView.cpp
│   │   ├── PlotView.h
│   │   └── PlotView.cpp
│   └── linux/
│       └── main.cpp
├── resources/
│   ├── math_render.html
│   ├── resources.qrc
│   └── katex/
│       ├── katex.min.js
│       ├── katex.min.css
│       └── fonts/          (woff2 × 20)
└── tests/
    ├── test_engine.cpp
    ├── test_history.cpp
    └── test_input_widget.cpp
```

## 3. コア実装

### 3.1 AsirEngine

`AsirEngine` は抽象インターフェースとして実装済みである。`resultReady()` の引数は `AsirResult` 構造体にまとめ、signal の引数爆発を解消している。

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
```

`mathPlainText` は Math タブで KaTeX ではなく等幅テキストで表示したい結果用のフィールドで、`pari(...)` の結果を格納する。`latex` と `mathPlainText` のどちらが埋まっているかで `MainWindow` 側が表示経路を分岐する。

現在の主な signal:

- `resultReady(const AsirResult &result)`
- `errorOccurred(const QString &message)`
- `engineReady()`

現在の主なメソッド:

- `executeString(const QString &command)`
- `interruptCurrent()`

`AsirResult` は `AsirEngine.h` に定義し、`Q_DECLARE_METATYPE(AsirResult)` でメタ型登録を行っている。  
これにより `QueuedConnection` でワーカースレッドから GUI スレッドへ安全に構造体を渡せる。  
`MainWindow` 側の `handleResult` も構造体メンバでアクセスする。  
`timingText` は `cputime(1);` が有効なときに `0.123sec(0.456sec)` 形式の文字列を格納する。

### 3.2 LocalAsirEngine / AsirWorker

実装済み。

仕様:

- `AsirWorker` を `QThread` に移して実行
- ワーカースレッド内で `GC_INIT()` を実行
- `asir_ox_init(1)` でライブラリモード初期化
- 初期化時に `load("names.rr");` を実行
- `OpenXM_HOME`, `ASIR_LIBDIR`, `ASIR_CONTRIB_DIR` を設定

`cputime()` 対応:

- `cputime(1);` / `cputime(0);` を GUI 側で特別扱いするのではなく、Asir の `cputime` 組込みをそのまま実行する
- そのうえで worker 側が CPU 時間と経過時間を計測し、`timingText` として UI へ渡す
- `cputime(1);` 実行後は、以後の通常コマンドでも時間表示を付与する
- `cputime(0);` 実行後は時間表示を止める

キャンセル機構:

- `AsirWorker` は `std::atomic<bool> m_cancelRequested` を持つ
- `requestCancel()` で他スレッドからフラグを立てられる
- GUI renderer による plot 描画ループ (`renderFunctionPlot`, `renderImplicitPlot`, `renderPolarPlot`) 内でフラグを検査し、中断した場合は空の結果を返す
- `execute()` の冒頭で `clearCancel()` を呼んでリセットする
- `LocalAsirEngine::shutdown()` は終了前に `requestCancel()` を呼ぶ
- `QMutex` は `execute()` 全体ではなく、`asir_ox_execute_string()`, `asir_ox_pop_string()`, `asir_ox_pop_cmo()` を呼ぶ区間だけで保持する

`Ctrl+C` 中断:

- `Run -> Interrupt` と `Ctrl+C` から `interruptCurrent()` を呼ぶ
- `asir_ox_init()` の library mode 初期化により有効な `SIGUSR1` 経路を使い、計算を `return to toplevel` へ戻す
- `pari(factor,...)` 実行中の中断後でも、続けて `1+1;` を実行できることを確認済み
- GUI 終了時には `ox_pari` を残さない状態へ戻ることを確認済み

補足:

- 一時期、起動時に小さい `memory_ifplot(...)` を先行実行するウォームアップを試したが、初期化停止の原因になり得たため最終版では削除した
- 最終的な plot 高速化はウォームアップではなく、NOX `memory_*plot` が `ox_plot` を起動しない経路へ修正されたことによる

### 3.3 設定ファイル

実装済み。

設定ファイルの優先順位:

1. `asirgui` 実行ファイルと同じディレクトリの `.asirguirc`
2. `~/.config/asirgui/config`

`.asirguirc` が存在すればそちらを使い、存在しなければ `~/.config/asirgui/config` を自動生成して保存する。

現在保存している設定:

- `OpenXM_HOME`
- `ASIR_LIBDIR`
- `ASIR_CONTRIB_DIR`
- Plot 前景色
- Plot 背景色
- Plot 線幅
- Plot ズーム倍率
- `Fit Width`
- `Show Axes`
- `Show Axis Labels`
- 入力欄フォント
- 出力欄フォント
- `Split View` における Text/結果タブの分割サイズ

### 3.4 文字列結果

`asir_ox_peek_cmo_string_length()` で必要バッファサイズを取得し、その長さ分だけ `asir_ox_pop_string()` で読み出す。  
`peek` が 0 以下を返すケースもあるため、その場合は 4096 バイトのフォールバックバッファで `asir_ox_pop_string()` を試みる。  
これにより `pari(...)` のように `peek` だけでは長さが取れない結果を空文字に落とさない。

さらに最終版では、`asir2018/io/ox_asir.c` の `asir_ox_peek_cmo_string_length()` も修正した。  
従来は `valid_as_cmo()` に失敗すると `The object at the stack top is invalid as a CMO.` を `stderr` へ出していたが、`asir_ox_pop_string()` 自体はそのような値も文字列化できる。  
そのため `pari(factor,2^2^7+1);` のような正常な実行でも、長さ確認の時点で不要な警告だけがターミナルに出ることがあった。  
最終版では `peek_cmo_string_length()` も `pop_string()` と整合するように `estimate_length()` ベースで長さを返し、不要な警告を出さない。

### 3.5 LaTeX 生成

`print_tex_form()` を使っている。  
`@n` 参照のために内部で `AsirGuiRefN=(式);AsirGuiRefN;` にラップする場合があるため、LaTeX 生成時は元の式だけを抽出して `print_tex_form()` に渡す。

代入文や複雑な制御構文は、現在も LaTeX 表示対象外である。

`pari(...)` は最終版で例外扱いにした。  
`print_tex_form()` に通すと `The object at the stack top is invalid as a CMO.` が出ることがあり、また因数分解結果のような複数行・空白つき出力は KaTeX に通すと空白がつぶれて読みにくくなる。  
このため `pari(...)` は Math タブでも KaTeX ではなく、等幅フォントのプレーンテキストとして表示する。

### 3.6 エラー判定

Asir の public API だけでは CMO タグを失わずに文字列表現も同時に得る手段がないため、現在のエラー判定はエンジン側のテキスト判定で一元化している。  
`AsirResult::isError` に集約し、`MainWindow` 側の重複判定は廃止した。  
判定対象は `"return to toplevel"`, `"syntax error"`, `"undefined"`, `"error("` の行頭一致、および `"detected during parsing"` の含有である。  
従来の `"error"` 行頭一致より誤検出しにくくしている。

構文エラーの最終仕様:

- `fctr(^3-1);` のような構文エラーは、Asir の parser (`yyerror()`) が library mode でも `stderr` へ直接出す
- この場合、結果スタック側は `0` になるため、`asir_ox_pop_string()` だけでは CLI 版相当の表示を再現できない
- そのため `LocalAsirEngine` では `asir_ox_execute_string()` 実行中の `stderr` を `ScopedStderrCapture` で一時捕捉し、`extractParseDiagnostic()` で診断行を抽出する
- 抽出対象は `syntax error` / `undeclared` / `conflicting declarations` を含む行で、`pvar.c` 由来の `undeclared function`, `undeclared variable`, `conflicting declarations` も回収する
- 診断行の先頭にある `"string", near line N: ` はライブラリモード固有のプレースホルダなので除去し、`load("foo.rr")` 経由で発生した場合のファイル名・行番号はそのまま残す
- 直接コマンド実行では結果文字列を
  `syntax error`
  `0`
  に正規化して Output ペインへ返す
- 同時に `no_debug_on_error` を有効にし、エラー時にデバッグウィンドウへだけ出る挙動を抑止している
- plot 系 (`executeToCmo()` 経路) でも同じ `ScopedStderrCapture` を通し、`plot(^2-1);` のような構文エラーも `syntax error\n0` で返す

`ScopedStderrCapture` の制約:

- `dup2(fileno(m_file), fileno(stderr))` でプロセス全体の `stderr` を tmpfile に差し替える
- 実行区間は `m_mutex` で保護され、`asir_ox_execute_string()` 前後で元の `stderr` に戻す
- `stderr` の差し替えはプロセスグローバルなので、この区間に他スレッド (Qt 本体・libgc のログ等) が `stderr` に出力した場合、そのテキストも tmpfile に入る
- `extractParseDiagnostic()` は構文エラー系の行だけを抽出し、それ以外は破棄するため、他スレッドが出す情報メッセージは落ちる
- 現状 libasir / libgc はこの区間で `stderr` を使わないため実害は出ていないが、将来 `stderr` ロギングを導入する場合はこの制約を意識する必要がある

これにより GUI でも CLI 版に近い
`syntax error`
`0`
の表示になる。

### 3.7 正規表現

plot コマンドの検出に使う 3 つの `QRegularExpression` (`kDirectPlotPattern`, `kAssignmentPlotPattern`, `kWrappedPattern`) は名前空間スコープの定数として定義し、関数呼び出しのたびに再構築しないようにしている。

### 3.8 コマンド履歴の永続化

`CommandHistory` の保存形式は JSON 配列にしている。  
読み込み時は JSON 解析を試み、失敗時は 1 行 1 コマンドのプレーンテキストとして読む後方互換パスを持つ。

## 4. REPL 風 UI

### 4.1 レイアウト

当初計画の左右分離 UI は採用せず、現在は次の REPL 風構成である。

- 上部: テキスト履歴ペイン
- 中央: `Math` / `Plot` タブ
- 下部: 入力欄

`Split View` の初期比率は Text ペインと結果タブが半々になるようにしている。  
分割バーを変更した場合、そのサイズは設定ファイルへ保存し、次回起動時に復元する。

非 plot コマンドでは `Math` タブ、plot コマンドでは `Plot` タブへ自動的に切り替える。  
その後に自動スクロールを行うが、最後にキャレットは Asir コマンドエディタへ戻す。

`quit;`, `bye;`, `end;` は GUI 側で終了要求として処理する。  
当初は Asir 本体へそのまま送っていたが、library mode の `quit` 経路に入ると終了時に
`QObject::killTimer: Timers cannot be stopped from another thread`
や `internal error (SEGV)` が出ることがあったため、最終版では `File -> Exit` と同じ安全側の終了経路へ統一した。

### 4.2 コマンド番号と `@n`

実装済み。

動作:

- 実行ごとに `[n]` を表示
- 参照可能な式は `AsirGuiRefN` に保存
- 後続入力の `@n` を `AsirGuiRefN` に展開
- `@@` は最後に保存された参照可能結果へ展開
- 終端が `;;` または `$` の場合は結果を表示せず、`@n` も作らない
- 終端が「記号なし」または `;` の場合だけ、通常どおり結果表示と `@n` 作成を行う
- ただし、エラーは終端に関係なく表示する

補足:

- `@@` は GUI 側で `m_lastReferenceableId` を保持して実現している
- 展開時は `m_lastReferenceableId > 0` に加えて `m_referenceableIds.contains(m_lastReferenceableId)` も確認し、万が一どちらか片方が先にクリアされた場合でも、存在しない `AsirGuiRefN` を Asir に送らないようにしている
- 文字列リテラル中の `@n` / `@@` は展開しない
- 参照可能な結果がまだ存在しないときは `@@` をそのまま Asir に渡す。Asir の library mode の parser は `@@` を受理しないため、結果は `syntax error` になる
- 非表示終端のコマンドは参照対象に入らないため、`@@` も更新しない

自動保存対象外:

- 代入文
- `load`, `import`
- `if`, `for`, `while`
- plot 系
- `cputime`

非表示終端の最終仕様は、CLI 版 / Windows 版に合わせている。  
単に UI 側で隠すだけではなく、`AsirEngine::executeString(command, producePresentation)` に表示生成フラグを追加し、`;;` / `$` のときは `LocalAsirEngine` 側でも LaTeX / Math 用プレーンテキスト生成を抑止するようにした。  
これにより、表示しないコマンドで不要な `print_tex_form()` や Math タブ描画へ進みにくくしている。

### 4.3 InputWidget

実装済み。

現在の入力欄は、単なる 1 行入力ではなく複数行の「Asir コマンドエディタ」である。

基本仕様:

- `Enter`: 改行
- `Shift+Enter`: 実行
- 縦横スクロールバーは必要時のみ表示
- `QSplitter` により高さ変更可能
- インデントはタブ文字ではなく半角スペース 4 個
- 行末が `{` のとき、次行は 1 段深くする
- `}` 入力時は、その括弧自体を 1 段左へ戻して挿入する
- `}` の直後に `Enter` を押しても二重にデデントしない
- 実行後もキャレットは入力欄に残る

対応キーバインド:

- `Ctrl+A`, `Ctrl+E`, `Ctrl+B`, `Ctrl+F`
- `Ctrl+D`, `Ctrl+H`, `Ctrl+K`, `Ctrl+U`, `Ctrl+W`
- `Ctrl+P`, `Ctrl+N`
- `Alt+B`, `Alt+F`, `Alt+Backspace`
- `Tab`

履歴移動:

- `Ctrl+P` で履歴モードに入り、1 つ前の履歴へ移動
- `Ctrl+N` で履歴モードに入り、1 つ次の履歴へ移動
- エディタが空の状態で `↑` を押すと、`Ctrl+P` と同様に履歴モードへ入る
- 履歴モード中のみ `↑`, `↓` で履歴を移動
- 履歴モード中は `Ctrl+P`, `Ctrl+N` でも前後へ移動できる
- エディタに内容がある通常時の `↑`, `↓` は複数行エディタのカーソル移動として残している
- 履歴モードは他のキー入力で終了し、通常編集へ戻る

補完:

- `Tab` は、カーソル直前の識別子を補完対象とする
- 補完候補は次の合成集合である
  - 主要 Asir 関数・予約語の固定リスト
  - 現在のエディタ内容に含まれる識別子
  - 履歴中のコマンドに含まれる識別子
- 例:
  - 履歴に `fctr(x^4-1);` があれば、`fc` + `Tab` で `fctr` を補完できる
  - エディタに `ResultValue` が出ていれば、`Res` + `Tab` で候補になる
- 実行中の Asir エンジンからシンボル表を問い合わせる方式ではない

履歴展開:

- `!!`: 直前の履歴を展開して実行
- `!prefix`: `prefix` で始まる直近履歴を展開して実行
- `!prefix:p`: 展開だけ行い、エディタ内容を置き換えて実行しない
- `!prefix:p` の直後は、その `prefix` に一致する履歴だけを `↑`, `↓`, `Ctrl+P`, `Ctrl+N` でたどれる

### 4.4 フォント変更

計画外追加仕様として実装済み。

- `View -> Output Font...`
- `View -> Input Font...`

どちらも `QFontDialog` を使い、フォント種類とサイズを変更できる。

現在は、これらのフォント設定も設定ファイルへ保存される。

## 5. Math タブ

### 5.1 KaTeX 表示

実装済み。

表示経路は 2 つある。

KaTeX 経路 (通常コマンド):

1. C++ 側が LaTeX とタイトルを `MathView::appendLatex()` に渡す
2. `MathView` が `QJsonArray` でパラメータを JSON 配列にシリアライズし、`appendLatexFromJson(json)` を呼ぶ
3. HTML 側が JSON 配列を受け取り、KaTeX で描画する

プレーンテキスト経路 (`pari(...)` 等):

1. C++ 側がテキストとタイトルを `MathView::appendPlainText()` に渡す
2. `MathView` が同じく JSON 配列でシリアライズし、`appendPlainTextFromJson(json)` を呼ぶ
3. HTML 側が JSON 配列を受け取り、等幅フォントでそのまま表示する

どちらの経路も C++ → JavaScript のパラメータ受け渡しに JSON を使い、インジェクションを防止している。  
以前の実装では文字列連結でパラメータを埋め込んでいたが、これを廃止した。

### 5.2 Math タブ保護

実装済み。

`nd_gr(...)` のような巨大結果をそのまま KaTeX に送ると、Math タブが重くなり、最悪の場合はフリーズしたような状態になる。  
これは WebEngine 内の KaTeX 描画であり、Asir の計算中断とは別系統なので `Ctrl+C` では止められない。  
そのため最終版では、「重い結果を KaTeX に送ってから止める」のではなく、「KaTeX に送る前に止める」方針にした。

判定仕様:

- 高リスク関数:
  - `nd_gr(...)`
  - `gr(...)`
  - `dp_gr(...)`
  - `fglm(...)`
  - `syz(...)`
- 高リスク関数は `LaTeX > 2000` または `Text > 4000` で自動省略
- 一般コマンドは `LaTeX > 100000` または `Text > 200000` の極端に大きい場合だけ省略

自動省略時の Math タブ表示:

- `結果が大きすぎるため、Math タブでの数式レンダリングを省略しました。`
- `上部の Output ペインで全文を確認してください。`
- `LaTeX: ... 文字`
- `Text: ... 文字`
- `判定: ...`

自動省略だが強制可能な場合は、さらに `強制レンダリング` ボタンを表示する。  
これはその結果 1 件だけを明示的に KaTeX 描画させる操作であり、通常は押さなくても Output ペインで内容を確認できる。

この保護により、

- `cyclic(5);` のような通常コマンドはそのまま KaTeX 表示
- `nd_gr(C,vars(C),31991,0)$` のような巨大な多項式集合は省略表示

という動作にしている。

### 5.3 表示見出し

以前は各ブロックに `"KaTeX"` を表示していたが、現在は

```text
[3] fctr(x^3-1);
```

のように、コマンド番号と実行コマンドを見出しにしている。

### 5.4 KaTeX 資産

KaTeX 0.16.11 を `resources/katex/` にローカル同梱している。

構成:

- `katex.min.js` (約 275KB)
- `katex.min.css` (約 23KB)
- `fonts/` 以下に woff2 フォント 20 ファイル

`resources.qrc` に全ファイルを登録し、HTML 側は `qrc:/katex/katex.min.js` および `qrc:/katex/katex.min.css` で読み込む。  
CSS 内のフォント参照パスは `url(fonts/...)` のまま維持する。CSS 自体が `qrc:/katex/katex.min.css` にあるため、相対パスは `qrc:/katex/fonts/...` に正しく解決される。

- KaTeX 読み込み成功時: グラフィカル表示
- 失敗時: LaTeX 文字列フォールバック

非 plot コマンドで LaTeX を追加した場合は、自動的に `Math` タブへ切り替え、末尾の結果が見える位置までスクロールする。

## 6. Plot タブ

### 6.1 対応関数

実装済み:

- `ifplot`
- `conplot`
- `plot`
- `polarplot`

未対応:

- `open_canvas`
- `clear_canvas`
- `draw_obj`
- `draw_string`

### 6.2 実装方式

最初は GUI renderer による数値サンプリングのみで対応していたが、現在は `libasir.a` 側の `memory_*plot` を優先利用する構成にしている。

現在の処理:

1. plot 系コマンドを検出
2. 内部で `memory_*plot(...)` を実行
3. CMO の `[width,height,bytearray]` を取得
4. `BYTEARRAY` を `QImage` に復元
5. `PlotView` に表示

`memory_*plot` が使えない場合のみ GUI renderer へフォールバックする。  
最終版では NOX `memory_ifplot` / `memory_conplot` / `memory_plot` から `validate_ox_plot_stream()` を通さないようにし、`ox_plot` を起動しない。

引数の正規化は、当初は geometry の位置に引きずられる不具合があった。  
例えば `plot(x^2-x^3,[400,400],[x,-4,4]);` を実行すると、GUI 側が既定範囲 `[-2,2]` を勝手に補い、要約が `-2 <= x <= 2` になっていた。  
最終版では `LocalAsirEngine` の plot 正規化を見直し、Asir 本体 `plotf.c` と同様に「引数の位置」ではなく「引数の内容」で geometry と範囲指定を判定するようにした。

最終仕様:

- `plot(func,[geometry],[xrange])` と `plot(func,[xrange],[geometry])` のどちらも正しく解釈する
- `polarplot(func,[geometry],[thetarange])` と `polarplot(func,[thetarange],[geometry])` のどちらも正しく解釈する
- `ifplot/conplot` でも、既に指定されている変数範囲は保持し、不足している範囲だけ既定値を補う
- geometry は内部コマンド列の末尾に 1 回だけ付ける

これにより、geometry 先行指定の `plot(...)` / `polarplot(...)` でも summary, 軸, GUI fallback のすべてで同じ範囲が使われるようになった。

### 6.3 速度と描画経路

plot 本体の画像生成は `asir2018/plot` 系コードを通るため、計算とピクセル生成は X11 版とほぼ同じ経路である。  
ただし、軸や目盛り文字列は Qt 側の独自描画であり、X11 版のそのまま移植ではない。

`PlotView` でのピクセル走査は `QImage::scanLine()` による直接アクセスを使い、線幅 1 の場合は `QRgb` ポインタ操作のみで前景色を設定する。`pixelColor()` による 1 ピクセルごとの `QColor` 生成を避けることで、300×300 画像で約 90,000 回のオブジェクト生成コストを削減している。

当初は起動時ウォームアップで初回待ち時間を減らそうとしたが、最終版では採用していない。  
実際に効いた高速化は、NOX `memory_*plot` が `ox_plot` を起動しないように整理したことによる。  
これにより、連続する `ifplot(...)` や geometry 指定付き `ifplot(...)` の実行でも、余計な待ち時間や `ox_plot` プロセス残留が発生しなくなった。

### 6.4 見出しとテキスト要約

plot 系のテキストペイン表示は、内部実装名を出さないよう統一済みである。

例:

```text
[Plot] [1] ifplot x^3-y^2+x^2 -> 300x300, -5 <= x <= 5, -5 <= y <= 5
```

Plot タブ側の見出しも同じ情報を使い、例えば

```text
[1] ifplot x^3-y^2+x^2 -> 300x300, -5 <= x <= 5, -5 <= y <= 5
```

の形式で表示する。

plot 実行後は自動的に `Plot` タブへ切り替え、最新グラフブロックの先頭が見える位置へスクロールする。  
複数回連続で `ifplot` を実行した場合も、常に最新グラフへ追従するようにしている。

### 6.5 PlotView の設定

実装済み:

- 前景色変更
- 背景色変更
- 線の太さ変更
- Fit Width
- Zoom In
- Zoom Out
- Reset Zoom
- `Show Axes`
- `Show Axis Labels`

### 6.6 軸と目盛り

現在の仕様:

- 軸線の表示/非表示を切り替え可能
- 目盛り文字列の表示/非表示を切り替え可能
- x 軸の目盛りは x 軸の下側
- y 軸の目盛りは y 軸の左側
- 軸が中央にある場合も、軸位置に追従して配置

## 7. Linux 固有実装

`QWebEngineView` が Linux 環境で GPU コマンドバッファ初期化エラーを出すことがあったため、`desktop/linux/main.cpp` で次を設定している。

- `Qt::AA_UseSoftwareOpenGL`
- `QTWEBENGINE_CHROMIUM_FLAGS=--disable-gpu --disable-gpu-compositing`

これにより、ターミナルへ毎回出ていた GPU 関連エラーの抑止を図っている。

## 8. メニュー

現在の主なメニュー:

- `File`
  - `Open`
  - `Logging`
  - `Exit`
- `Run`
  - `Interrupt` (`Ctrl+C`)
- `View`
  - `Text Only`
  - `Math Only`
  - `Split View`
  - `Output Font...`
  - `Input Font...`
- `Plot`
  - `Foreground...`
  - `Background...`
  - `Thin / Medium / Thick`
  - `Show Axes`
  - `Show Axis Labels`
  - `Fit Width`
  - `Zoom In`
  - `Zoom Out`
  - `Reset Zoom`
- `Help`
  - `Editor Help`
  - `User's Manual`
  - `Official Site`
  - `About`

`Editor Help` は当初 `QMessageBox` で実装していたが、内容が長く起動位置によっては上部が見えないため、現在は `QDialog + QTextBrowser` に変更している。  
これにより、キーバインド、`Ctrl+C` 中断、補完説明をスクロールしながら参照できる。

`User's Manual` と `Official Site` は既定ブラウザで開く。  
Linux ネイティブでは `QDesktopServices` と `xdg-open` を使い、WSL 環境では `xdg-open` だけでは URL が正しく渡らない場合があるため、Windows 側の `powershell.exe`、次に `cmd.exe /c start` を優先して URL を起動する実装にしている。

## 9. テスト

実装済みテスト:

- `test_history.cpp`
  - 履歴前後移動
- `test_engine.cpp`
  - `LocalAsirEngine` 初期化
  - `fctr(x^4-1);`
  - LaTeX 取得
  - `cputime(1);` / `cputime(0);`
  - `pari(factor,2^2^5+1);`
  - `pari(factor,2^2^7+1);` 実行時にターミナルへ `The object at the stack top is invalid as a CMO.` が出ないこと
  - `pari(factor,2^2^8+1);`
  - `fctr(^3-1);` が `syntax error` と `0` を返し、`isError` が立つこと
  - `plot(^2-1,[x,-2,2]);` のような plot 系構文エラーも `syntax error\n0` を返し、`plotImage` が空・`isError` が立つこと
  - `1+1$`, `fctr(x^4-1);;` では結果を表示しないこと
  - `;;` / `$` のときは `@n` 参照を作らないこと
  - `cyclic(5);` が Math タブ保護で過剰に省略されないこと
  - `polarplot(...)`
  - geometry を先に書いた `polarplot(...)`
  - `ifplot(...)`
  - 連続する `ifplot(...)` と geometry 指定付き `ifplot(...)`
  - `plot(x^2-x^3,[400,400],[x,-4,4]);` のような geometry 先行の `plot(...)`
  - `nd_gr(C,vars(C),31991,0)$` が Math タブで省略表示と `強制レンダリング` になること
  - `pari(factor,2^2^9+1);` 実行中の `interruptCurrent()`
  - 中断後の `1+1;`
  - `ox_launch_nox()` / `ox_execute_string()` / `ox_pop_local()` / `ox_shutdown()`
- `test_input_widget.cpp`
  - `Ctrl+U` が行頭まで削除すること
  - `Tab` 補完
  - `!!`, `!prefix:p` の履歴展開
  - エディタが空のときだけ `↑` で履歴モードへ入ること
  - エディタに内容がある通常時の `↑` は履歴を使わないこと
  - `Ctrl+P`, `Ctrl+N` が履歴モード継続中も前後へ移動できること
  - `!prefix:p` 後に prefix 一致履歴だけをたどれること

標準確認コマンド:

```bash
ctest --test-dir OpenXM_contrib2/asirgui_qt/build --output-on-failure
```

テストカバレッジの未対応項目:

- `MainWindow::expandReferences()` の `@@` / `@n` 展開そのものは、現状 `test_engine.cpp` (engine 直叩き) では検証されていない。`submitCommand` 経由の GUI 層テストを `test_input_widget` 系に追加するのが望ましい。

## 10. 既知の制約

### 10.1 LaTeX 化

- 代入文などは現在も LaTeX 化しない
- `print_tex_form()` が失敗するケースでは文字列フォールバックする

### 10.2 open_canvas 系

未対応である。  
`ifplot` 系のような `memory_*plot` に相当する API が無いため、NOX `libasir.a` 用のメモリキャンバス API を別途設計する必要がある。

## 11. 計画との差分

### 11.1 計画どおり

- Qt Widgets ベース
- `libasir.a` 直接リンク
- ワーカースレッド実行
- `print_tex_form()` による数式表示
- `QWebEngineView` による Math タブ
- CTest による基本検証

### 11.2 計画から変更

- 左右分離 UI をやめ、REPL 風構成に変更
- plot 表示を正式に追加
- `@n` を GUI 側で代替実装
- Math ブロック見出しを `"KaTeX"` から `"[n] 実行コマンド"` に変更
- plot 要約を内部関数名ではなく `ifplot ... -> ...` 形式に統一
- `resultReady` signal を 10 引数から `AsirResult` 構造体に変更
- KaTeX を CDN からローカル同梱 (`resources/katex/`) に変更

### 11.3 計画外追加

- 入力欄フォント変更
- 文字ペインフォント変更
- Plot メニュー
- 軸表示と目盛り文字列表示
- Linux WebEngine の GPU ログ抑止設定
- 補完機能
- 履歴展開 (`!!`, `!prefix`, `!prefix:p`)
- 履歴モード付きキーバインド
- 設定ファイル管理 (`.asirguirc` / `~/.config/asirgui/config`)
- Text/結果タブ分割サイズの保存
- Math/Plot タブの自動スクロール
- `AsirWorker` のキャンセル機構 (`std::atomic<bool>`)
- コマンド履歴の JSON 形式保存
- MathView の JSON ベース JS 通信 (インジェクション防止)
- `PlotView` の `scanLine()` 最適化
- エラー判定の `startsWith` 化
- 正規表現の名前空間スコープ定数化
- 実行後にキャレットを Asir コマンドエディタへ戻す処理
- `cputime()` と `timingText`
- `Ctrl+C` による計算中断
- `ox_plot` を経由しない NOX `memory_*plot` 化による ifplot 高速化
- スクロール可能な `Editor Help`
- `User's Manual` / `Official Site` の外部ブラウザ連携
- `quit;` / `bye;` / `end;` を GUI 終了コマンドとして処理

## 12. 次の課題

1. `open_canvas` 系の方針決定
2. 設定ダイアログ導入
3. 配布用整備
