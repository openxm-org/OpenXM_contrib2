# `OpenXM_contrib2/asir2018` の最終修正記録

## 1. この文書の目的

この文書は、`OpenXM_contrib2/asir2018` に対して最終的に残した修正だけを整理した記録である。

AsirGUI の実装を進める中で、`libasir.a` にはできる限り手を入れない方針を取ったが、最終的には次の 2 点のために `asir2018` 側の修正が必要になった。

1. AsirGUI から `ox_plot` を経由せずに `ifplot/conplot/plot/polarplot` を使えるようにする
2. AsirGUI から `Ctrl+C` による安全な計算中断を library mode で有効化する

加えて、`pari()` が内部で起動する `ox_pari` を GUI 終了時に残さないための helper と、`pari()` の正常実行時に不要な CMO 警告を出さないための修正を追加した。

本書では、

- 最終的に残した修正
- 見直しの結果、不要と判断して戻した修正
- 確認方法

をまとめる。

## 2. 最終的に残した修正の全体像

最終版で `asir2018` に残した修正は、機能別に分けると次の 4 系統である。

1. NOX ビルドでも `memory_*plot()` を使えるようにする修正
2. `asir_ox_init()` を使う library mode で `Ctrl+C -> return to toplevel` を成立させる修正
3. `ox_pari` を安全に shutdown する helper
4. `asir_ox_peek_cmo_string_length()` を `asir_ox_pop_string()` と整合させ、不要な CMO 警告を抑止する修正

それ以外の試行的な修正は、最終確認の段階でできるだけ元へ戻した。

## 3. 修正したファイル

最終的に意味のある差分が残った主なファイルは次のとおりである。

- [`Makefile.am`](../../asir2018/Makefile.am)
- [`Makefile.in`](../../asir2018/Makefile.in)
- [`builtin/ftab.c`](../../asir2018/builtin/ftab.c)
- [`builtin/parif.c`](../../asir2018/builtin/parif.c)
- [`io/ox_asir.c`](../../asir2018/io/ox_asir.c)
- [`io/tcpf.c`](../../asir2018/io/tcpf.c)
- [`parse/gc_risa.c`](../../asir2018/parse/gc_risa.c)
- [`parse/glob.c`](../../asir2018/parse/glob.c)
- [`plot/ifplot.h`](../../asir2018/plot/ifplot.h)
- [`plot/plotf.c`](../../asir2018/plot/plotf.c)
- [`plot/nox_plotp.c`](../../asir2018/plot/nox_plotp.c)
- [`plot/nox_memory.c`](../../asir2018/plot/nox_memory.c)
- [`plot/nox_stub.c`](../../asir2018/plot/nox_stub.c)

## 4. NOX ビルドで `memory_*plot()` を使うための修正

### 4.1 `NOX_PLOT_SRCS` の追加

修正箇所:

- [`Makefile.am`](../../asir2018/Makefile.am)
- [`Makefile.in`](../../asir2018/Makefile.in)

`make NOX=y NOFEP=y` のときでも、plot 計算に必要な最小ソースを `libasir.a` に含めるようにした。

追加した NOX 用ソースは次のとおりである。

- `plot/calc.c` — 既存の数値計算コード (GUI 版と共有)
- `plot/plotf.c` — 既存の `Pmemory_ifplot()` 等コマンド定義 (GUI 版と共有)
- `plot/nox_plotp.c` — `plotp.c` から X11 非依存の `memory_*` 描画関数だけを抽出したファイル
- `plot/nox_memory.c` — NOX 版の `memory_plot()` / `memory_polarplot()` 実体
- `plot/nox_stub.c` — `calc.c` が参照する X11 進捗表示関数の空実装

このとき `ASIR_NO_XPLOT` を定義し、X11 依存コードを通さずに済むようにした。

#### NOX 専用 3 ファイルの必要性

GUI 版では X11 関連のコードがすべて `plotp.c` (694 行) に集約されている。このファイルには `DISPLAY *display` を引数に取る描画関数 (`if_print`, `con_print`, `plot_print`, `draw_point` 等) と、X11 を使わないメモリビットマップ関数 (`memory_if_print`, `memory_con_print`, `memory_print`) が混在している。

NOX ビルドでは `plotp.c` を含められない (X11 ヘッダ・型が存在しないためコンパイルできない)。そこで NOX 版に必要な部分だけを次の 3 ファイルに分離した。

| ファイル | 行数 | 役割 |
|---|---|---|
| `nox_plotp.c` | 94 | `plotp.c` から `memory_if_print`, `memory_con_print`, `memory_print` を X11 非依存で抽出。ビットマップ (`BYTEARRAY`) への描画のみを行い、`DISPLAY` 型を一切使わない。 |
| `nox_memory.c` | 112 | `memory_plot()` と `memory_polarplot()` の実体。Asir コマンド引数 (`NODE arg`) を解釈し、スタックローカルの `struct canvas` を `memset` で初期化して、`calc.c` の数値計算と `nox_plotp.c` のビットマップ描画を呼び出し、結果を `[width, height, BYTEARRAY]` の `LIST` として返す。GUI 版のような X11 ウィンドウやキャンバスの管理は一切行わない。 |
| `nox_stub.c` | 25 | 共有コード `calc.c` が参照する `initmarker()`, `marker()`, `count_and_flush()`, `flush()` の空実装。GUI 版ではこれらを `plotp.c` が提供し X11 のスクロールバー更新や `XFlush()` を行うが、NOX 版では描画先がないため何もしない。`calc.c` 側にも `if(!nox)` ガードがあるため実行時には呼ばれないが、リンカがシンボルを要求するためスタブが必要になる。 |

この分離により、数値計算 (`calc.c`) → ビットマップ生成 (`nox_plotp.c`) → Asir コマンドとしての結合 (`nox_memory.c`) という流れを X11 なしで構成し、`nox_stub.c` が共有コードのリンクギャップを埋める構造になっている。

### 4.2 `plot_tab` を NOX でも登録

修正箇所:

- [`builtin/ftab.c`](../../asir2018/builtin/ftab.c)

元のコードでは `plot_tab` が `DO_PLOT` のときだけ登録されるため、NOX ビルドでは

```text
evalf : memory_ifplot undefined
```

になっていた。

これを `ASIR_NO_XPLOT` のときも `plot_tab` を登録するように変更し、NOX `libasir.a` でも `memory_ifplot` / `memory_conplot` / `memory_plot` / `memory_polarplot` を組み込み関数として使えるようにした。

### 4.3 NOX 用の型・マクロ定義

修正箇所:

- [`plot/ifplot.h`](../../asir2018/plot/ifplot.h)

NOX ビルドでは X11 の型や描画マクロがないため、既存 plot コードをそのままコンパイルできない。

そこで `ASIR_NO_XPLOT` の場合だけ、`POINT`, `Widget`, `DRAWABLE`, `GC`, `XFontStruct` などの最小ダミー定義と、空の描画マクロを与えた。

これは「NOX で描画する」ためではなく、「数値計算とビットマップ生成だけを通す」ための準備である。

### 4.4 `memory_plot()` / `memory_polarplot()` によるビットマップ返却

修正箇所:

- [`plot/nox_plotp.c`](../../asir2018/plot/nox_plotp.c)
- [`plot/nox_memory.c`](../../asir2018/plot/nox_memory.c)
- [`plot/plotf.c`](../../asir2018/plot/plotf.c)

NOX 側では、計算結果を `[width,height,bytearray]` 形式の `LIST` として返す。

Qt 側はこの `bytearray` を `QImage` に復元して描画する。

特に `polarplot` については、NOX 経路でも同じ扱いにするために `memory_polarplot()` を追加した。

### 4.5 `ox_plot` を起動しない memory 経路

修正箇所:

- [`plot/plotf.c`](../../asir2018/plot/plotf.c)

最終見直しで重要だったのはここである。

当初は `memory_ifplot()` などの memory 経路でも `validate_ox_plot_stream()` が先に呼ばれており、NOX ビルドにもかかわらず `ox_plot` が起動してしまう経路が残っていた。

その結果、

- `invalid server id`
- `ifplot : Two geometries are specified. Probably a variable is missing.`
- `ox_plot` プロセスが残る

という不具合が起きた。

最終版では、`ASIR_NO_XPLOT` の memory 経路では `validate_ox_plot_stream()` を通さず、直接

- `memory_plot(arg, ...)`
- `memory_polarplot(arg, ...)`

を呼ぶようにした。

これにより、

- AsirGUI から `ox_plot` を一切起動しない
- `ifplot` の連続実行が安定する
- 描画開始までの待ち時間が目に見えて短くなる

という効果が得られた。

## 5. library mode 中断のための最小修正

### 5.1 方針

AsirGUI は `asir_ox_init()` を使って `libasir.a` を同一プロセス内で動かしている。

そのため CLI 版のように外部端末から `SIGINT` を直接送るのではなく、GUI の worker thread に `SIGUSR1` を送り、`return to toplevel` へ安全に戻れる library mode 専用経路が必要だった。

### 5.2 `asir_ox_init()` 側の修正

修正箇所:

- [`io/ox_asir.c`](../../asir2018/io/ox_asir.c)

追加したものは次のとおりである。

- `asir_ox_lib_mode`
- `asir_ox_lib_interrupting`
- `asir_ox_lib_interrupt_handler()`

そして `asir_ox_init()` で

- `asir_ox_lib_mode = 1`
- `asir_ox_lib_interrupting = 0`
- `set_signal(SIGUSR1, asir_ox_lib_interrupt_handler)`

を行うようにした。

重要なのは、この変更を `asir_ox_init()` にだけ入れた点である。`ox_io_init()` まで library mode 扱いにする必要はないため、見直し時にその変更は元へ戻した。

### 5.3 GC 中断時の復帰

修正箇所:

- [`parse/gc_risa.c`](../../asir2018/parse/gc_risa.c)

GC 中に `SIGUSR1` を受けると `caught_intr == 2` になる。

従来はそのまま `ox_usr1_handler(SIGUSR1)` に流していたが、library mode ではこれを使うと GUI 側の復帰経路と噛み合わない。

そこで、library mode のときだけ

- `asir_ox_lib_interrupting = 1`
- `set_lasterror("return to toplevel")`
- `resetenv("return to toplevel")`

へ入るようにした。

### 5.4 `resetenv()` からの戻り値を分離

修正箇所:

- [`parse/glob.c`](../../asir2018/parse/glob.c)

`resetenv()` の最後で `asir_ox_lib_interrupting` を見て、

- 通常エラー: `LONGJMP(main_env,1)`
- library mode 中断: `LONGJMP(main_env,2)`

と分けるようにした。

これにより `asir_ox_execute_string()` / `asir_ox_push_cmd()` 側は、中断と通常エラーを区別して扱える。

結果として、AsirGUI の `Ctrl+C -> Yes` で

```text
return to toplevel
```

へ戻り、その後に `1+1;` などの次コマンドを正常に再開できるようになった。

## 6. `ox_pari` を残さないための helper

修正箇所:

- [`builtin/parif.c`](../../asir2018/builtin/parif.c)
- [`io/tcpf.c`](../../asir2018/io/tcpf.c)

`pari()` は初回呼び出し時に `ox_pari` を起動し、その後は再利用する。

AsirGUI は `libasir.a` に直接リンクしているため、GUI 終了時に `ox_pari` を明示的に落とす手段が必要だった。

最終版で残した修正は次の 2 つだけである。

- `ox_shutdown_safe(int index)`  
  Asir の式評価を経由せずに、m/c stream を C レベルで閉じる helper
- `asir_ox_shutdown_pari()`  
  `ox_pari_stream_initialized` を見て `ox_shutdown_safe()` を呼び、`ox_pari` 状態を 0 に戻す helper

AsirGUI の終了処理は、この `asir_ox_shutdown_pari()` を呼ぶだけで `ox_pari` を残さず終了できる。

## 7. `asir_ox_peek_cmo_string_length()` の見直し

対象ファイル:

- [`io/ox_asir.c`](../../asir2018/io/ox_asir.c)

AsirGUI は文字列結果を取り出す前に `asir_ox_peek_cmo_string_length()` で必要バッファ長を見積もっている。  
しかし従来の `asir_ox_peek_cmo_string_length()` は `valid_as_cmo()` を通らない値に対して

```text
The object at the stack top is invalid as a CMO.
```

を `stderr` へ出して 0 を返していた。  
一方 `asir_ox_pop_string()` は、そのような値でも `sprintexpr()` で文字列化できる。

この不一致により、`pari(factor,2^2^7+1);` のような正常な実行でも、GUI が長さを確認しただけで不要な警告がターミナルへ出ていた。

最終版では、`asir_ox_peek_cmo_string_length()` を次の方針に変更した。

- stack top が `0` なら `"0"` 相当の長さを返す
- それ以外は `estimate_length(CO,obj)` で文字列長を見積もる
- `valid_as_cmo()` 失敗を理由に `stderr` へ警告を出さない

これは `asir_ox_pop_string()` の挙動に合わせるための修正であり、CMO 変換そのものの仕様を変えるものではない。

## 8. 見直しで削除した修正

最終確認の段階で、次の修正は不要と判断して削除または元へ戻した。

- `asir_ox_interrupt_pari()` と `ox_intr_safe()`  
  最終版の中断は library mode の `SIGUSR1` 経路で安定したため、`ox_pari` 専用 interrupt helper は不要になった。
- `parif.c` の Windows 側待機ループ変更  
  Linux GUI の実装には不要だったため、元のコードに戻した。
- `ox_io_init()` の library mode 化  
  必要なのは `asir_ox_init()` のみであり、外部 `ox_asir` サーバ側の初期化まで変える必要はなかったため、元に戻した。

この整理により、`asir2018` 側の修正は

- NOX `memory_*plot`
- library mode 中断
- `ox_pari` shutdown helper
- `peek_cmo_string_length()` の整合化

の 4 系統だけに絞られた。

## 9. 確認手順

`asir2018` の修正反映は次で行う。

```bash
cd OpenXM/src/asir2018
make NOX=y NOFEP=y install
```

その後、AsirGUI を再ビルドして標準テストを実行する。

```bash
cmake --build OpenXM_contrib2/asirgui_qt/build -j4
ctest --test-dir OpenXM_contrib2/asirgui_qt/build --output-on-failure
```

今回の最終確認では、少なくとも次を通している。

- `ifplot(x^3-y^2);`
- `ifplot(x^2 + y^2 - 1, [300,300], [x,-2,2], [y,-2,2]);`
- その後の `1+1;`
- `pari(factor,2^2^9+1);` 実行中の `Ctrl+C -> Yes`
- 中断後の `1+1;`
- `pari(factor,2^2^7+1);` 実行時に `The object at the stack top is invalid as a CMO.` が出ないこと
- GUI 終了後に `ox_pari` が残らないこと
- `ox_launch_nox(0)`, `ox_execute_string(...)`, `ox_pop_local(...)`, `ox_shutdown(...)` の後に `invalid server id` にならないこと

## 10. まとめ

最終的に `asir2018` 側へ残した修正は、AsirGUI を成立させるための最小セットに絞られている。

要点は次のとおりである。

- NOX `libasir.a` でも `memory_*plot()` を使えるようにし、`ox_plot` を一切起動しないようにした
- `asir_ox_init()` を使う library mode でも `Ctrl+C -> return to toplevel` へ戻れるようにした
- `pari()` が起動した `ox_pari` を GUI 終了時に確実に shutdown できる helper だけを追加した
- `asir_ox_peek_cmo_string_length()` を `asir_ox_pop_string()` と整合する実装にし、正常な `pari()` 実行で不要な CMO 警告を出さないようにした

この修正により、AsirGUI は

- X11 依存の `libasir_X.a` を使わず
- `libasir.a` ベースのまま
- plot と interrupt を両立した GUI

として動作できるようになった。
