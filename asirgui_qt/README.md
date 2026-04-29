# asirgui_qt

`asirgui_qt` は、OpenXM / Risa/Asir のための Qt 6 ベース GUI プロジェクトである。  
`libasir.a` を直接リンクして計算エンジンを組み込み、デスクトップ版とモバイル版の両方を視野に入れた構成で整理している。

## 目的

- Linux / Windows / macOS 向けの Qt Widgets ベース GUI を整備する
- Android / iPadOS 向け展開を見据えて、共通ロジックを分離する
- Asir の文字表示と数式表示を両立する

## 現在の実装状況

現時点で実装済みなのは Linux 版である。

- `libasir.a` を直接リンクするローカル実行エンジン (`AsirResult` 構造体による結果返却)
- REPL 風のテキスト表示
- KaTeX による数式表示 (ローカル同梱、JSON ベース JS 通信)
- `ifplot` / `conplot` / `plot` / `polarplot` の Plot タブ表示
- Plot 設定 (前景色、背景色、線幅、軸、目盛り、ズーム)
- コマンド履歴 (JSON 形式保存)
- `@n` 形式の結果参照
- ファイル読込
- ログ保存
- 上部ペイン / 入力ペインのフォント変更
- Emacs 風キーバインド、補完、履歴展開を備えた入力エディタ
- `cputime(1);` / `cputime(0);` による実行時間表示
- `Ctrl+C` による安全な計算中断
- `quit;` / `bye;` / `end;` による GUI 終了
- `.asirguirc` / `~/.config/asirgui/config` による設定保存

## ディレクトリ構成

```text
OpenXM_contrib2/asirgui_qt/
├── README.md
├── docs/
├── CMakeLists.txt
├── desktop/
│   ├── common/
│   ├── linux/
│   ├── windows/
│   └── macos/
├── mobile/
│   ├── common/
│   ├── android/
│   └── ipados/
├── core/
├── resources/
│   └── katex/          (KaTeX JS, CSS, woff2 フォント)
├── tests/
└── third_party/
```

主な役割:

- `docs/`
  - 実装計画、実装記録、ビルド手順
- `core/`
  - Asir エンジン、履歴などの共通ロジック
- `desktop/common/`
  - デスクトップ版共通 UI 部品
- `desktop/linux/`
  - Linux 版エントリポイント
- `resources/`
  - 数式表示 HTML、KaTeX 資産 (JS, CSS, フォント)、Qt resource
- `tests/`
  - CTest から実行するテスト

## 文書

- Linux 実装計画: [docs/AsirGUI_Linux.md](docs/AsirGUI_Linux.md)
- Linux 実装記録: [docs/AsirGUI_Linux_impl.md](docs/AsirGUI_Linux_impl.md)
- Linux ビルド手順: [docs/BUILD_LINUX.md](docs/BUILD_LINUX.md)

## Linux 版のビルド

概要だけ書くと、次の通り。

```bash
cmake -S OpenXM_contrib2/asirgui_qt -B OpenXM_contrib2/asirgui_qt/build
cmake --build OpenXM_contrib2/asirgui_qt/build -j4
```

詳細は [docs/BUILD_LINUX.md](docs/BUILD_LINUX.md) を参照。
