# AsirGUI macOS 実装計画

## 1. 概要

macOS版は、Qt6.8 Widgets でGUIを構築し、libasir.a をネイティブビルドして直接リンクする。
Linux版とほぼ同一のアーキテクチャだが、macOS固有のビルド設定とUI慣習への対応が必要。

- **GUI**: Qt 6.8 Widgets (macOS ネイティブ)
- **計算エンジン**: libasir.a (インプロセス、ワーカースレッド) ※macOS向けにビルド
- **数式描画**: KaTeX (QWebEngineView)
- **LaTeX変換**: `print_tex_form()` (names.rr / noro_print.rr)

## 2. アーキテクチャ

Linux版と同一構成。libasir.a を直接リンクし、ワーカースレッドで計算を実行する。

```
┌──────────────────────────────────────────────────┐
│           AsirGUI.app (macOS Bundle)              │
│                                                   │
│  ┌─────────────────────────────────────────────┐ │
│  │         Qt6.8 Widgets UI                     │ │
│  │  メニューバー (macOS ネイティブ統合)          │ │
│  │  ┌──────────┐ ┌───────────────────────────┐ │ │
│  │  │入力領域   │ │出力領域                    │ │ │
│  │  │          │ │ テキスト / KaTeX 数式      │ │ │
│  │  └──────────┘ └───────────────────────────┘ │ │
│  └─────────────────────────────────────────────┘ │
│                                                   │
│  ┌─────────────────────────────────────────────┐ │
│  │  LocalAsirEngine (ワーカースレッド)           │ │
│  │  libasir.a + libgc.a + libgmp + libmpfr     │ │
│  └─────────────────────────────────────────────┘ │
│                                                   │
│  Contents/Resources/                              │
│    katex/ (JS/CSS/Fonts)                         │
│    asir/ (names.rr, asir-contrib)                │
└──────────────────────────────────────────────────┘
```

## 3. ビルド環境

### 前提条件

```bash
# Xcode Command Line Tools
xcode-select --install

# Homebrew
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 依存ライブラリ
brew install gmp mpfr libmpc cmake ninja

# Qt 6.8
brew install qt@6
# または Qt Online Installer で 6.8 をインストール

# Boehm GC
brew install bdw-gc
```

### OpenXM / libasir.a のmacOS向けビルド

```bash
# OpenXM ソースの取得 (すでに取得済みの場合はスキップ)
git clone git@github.com:openxm-org/OpenXM.git
git clone git@github.com:openxm-org/OpenXM_contrib2.git

# macOS 向けビルド
cd OpenXM
# configure に __DARWIN__ フラグが自動設定される (configure.in で検出)
./configure
make

# 確認
ls lib/libasir.a
file lib/libasir.a
# lib/libasir.a: current ar archive random library
```

**注意**: macOS (Apple Silicon / Intel) の場合、以下に注意：
- Apple Silicon (arm64): `arch -arm64` でビルド
- Intel (x86_64): 通常ビルド
- Universal Binary が必要な場合: 両アーキテクチャでビルドし `lipo` で結合

## 4. プロジェクト構成

Linux版と共通のソースコード構成に、macOS固有のファイルを追加：

```
AsirGUI/
├── CMakeLists.txt                  # クロスプラットフォーム対応
├── src/
│   ├── main.cpp
│   ├── core/                       # Linux版と完全共通
│   │   ├── AsirEngine.h / .cpp
│   │   ├── LocalAsirEngine.h / .cpp
│   │   └── CommandHistory.h / .cpp
│   ├── desktop/                    # Linux版と完全共通
│   │   ├── MainWindow.h / .cpp
│   │   ├── InputWidget.h / .cpp
│   │   ├── OutputWidget.h / .cpp
│   │   ├── MathView.h / .cpp
│   │   └── SettingsDialog.h / .cpp
│   └── resources/
│       ├── katex/
│       ├── math_render.html
│       └── resources.qrc
├── platform/
│   └── macos/
│       ├── Info.plist              # アプリケーションバンドル設定
│       ├── asirgui.icns            # macOS アイコン
│       ├── entitlements.plist      # サンドボックス・権限設定
│       └── create_dmg.sh          # DMG インストーラ作成
└── tests/
```

## 5. macOS 固有の対応

### 5.1 メニューバー統合

macOS ではメニューバーがウィンドウ上部ではなく画面上部に表示される。
Qt は自動的にこれを処理するが、以下の調整が必要：

```cpp
// desktop/MainWindow.cpp (macOS 対応)
void MainWindow::createMenus() {
    // File メニュー
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(tr("&Open..."), this, &MainWindow::onFileOpen,
                        QKeySequence::Open);
    fileMenu->addAction(m_loggingAction);
    fileMenu->addSeparator();

#ifndef Q_OS_MACOS
    // macOS では Quit は自動的に Application メニューに配置される
    fileMenu->addAction(tr("E&xit"), qApp, &QApplication::quit,
                        QKeySequence::Quit);
#endif

    // Edit メニュー
    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(tr("&Undo"), m_input, &QPlainTextEdit::undo,
                        QKeySequence::Undo);
    editMenu->addSeparator();
    editMenu->addAction(tr("Cu&t"), m_input, &QPlainTextEdit::cut,
                        QKeySequence::Cut);
    editMenu->addAction(tr("&Copy"), m_input, &QPlainTextEdit::copy,
                        QKeySequence::Copy);
    editMenu->addAction(tr("&Paste"), m_input, &QPlainTextEdit::paste,
                        QKeySequence::Paste);

    // View メニュー
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(tr("&Font..."), this, &MainWindow::onFontSelect);

    // 表示モード切替
    QActionGroup *displayGroup = new QActionGroup(this);
    QAction *textOnly = viewMenu->addAction(tr("Text Only"));
    QAction *mathOnly = viewMenu->addAction(tr("Math Only"));
    QAction *splitView = viewMenu->addAction(tr("Split View"));
    textOnly->setCheckable(true);
    mathOnly->setCheckable(true);
    splitView->setCheckable(true);
    splitView->setChecked(true);
    displayGroup->addAction(textOnly);
    displayGroup->addAction(mathOnly);
    displayGroup->addAction(splitView);

    // Help メニュー
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("User's Manual"), this, &MainWindow::onHelp);

#ifndef Q_OS_MACOS
    helpMenu->addSeparator();
    helpMenu->addAction(tr("&About AsirGUI..."), this, &MainWindow::onAbout);
#endif
    // macOS では About は自動的に Application メニューに配置される
}
```

### 5.2 macOS 固有のキーバインド調整

macOS では Ctrl の代わりに Cmd (Meta) を使う慣習があるが、
Emacs風キーバインドでは Ctrl を使用する。両方をサポート：

```cpp
void InputWidget::keyPressEvent(QKeyEvent *event) {
    // macOS: Cmd+C はコピー、Ctrl+C はエンジン割り込み
    if (event->modifiers() & Qt::ControlModifier) {
        switch (event->key()) {
        case Qt::Key_A: moveCursor(QTextCursor::StartOfLine); return;
        case Qt::Key_E: moveCursor(QTextCursor::EndOfLine); return;
        case Qt::Key_P: showPreviousHistory(); return;
        case Qt::Key_N: showNextHistory(); return;
        case Qt::Key_K: killLine(); return;
        case Qt::Key_U: clearLine(); return;
        // ... (Linux版と共通)
        }
    }
    QPlainTextEdit::keyPressEvent(event);
}
```

### 5.3 Retina ディスプレイ対応

Qt6 は Retina (HiDPI) を自動サポートするが、KaTeX 描画で注意：

```html
<!-- math_render.html: Retina 対応 -->
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<style>
  body {
    -webkit-font-smoothing: antialiased;
    font-size: 18px;
  }
</style>
```

### 5.4 ダークモード対応

macOS のダークモードに対応：

```cpp
// desktop/MainWindow.cpp
void MainWindow::updateTheme() {
    // Qt6 は macOS のシステムテーマを自動検出
    QPalette palette = QApplication::palette();
    bool isDark = palette.color(QPalette::Window).lightness() < 128;

    // KaTeX のテーマを切り替え
    if (isDark) {
        m_mathView->page()->runJavaScript(
            "document.body.style.backgroundColor='#1e1e1e';"
            "document.body.style.color='#d4d4d4';");
    } else {
        m_mathView->page()->runJavaScript(
            "document.body.style.backgroundColor='white';"
            "document.body.style.color='black';");
    }
}
```

## 6. CMake ビルド設定

```cmake
cmake_minimum_required(VERSION 3.22)
project(AsirGUI VERSION 1.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS
    Widgets WebEngineWidgets WebChannel)

# OpenXM / libasir
set(OPENXM_HOME "$ENV{OpenXM_HOME}" CACHE PATH "OpenXM installation directory")
set(ASIR_LIBDIR "${OPENXM_HOME}/lib")
set(ASIR_INCDIR "${OPENXM_HOME}/include")

find_library(LIBASIR asir PATHS ${ASIR_LIBDIR} REQUIRED)
find_library(LIBGC gc PATHS ${ASIR_LIBDIR} REQUIRED)

# Homebrew ライブラリパス (Apple Silicon / Intel)
if(APPLE)
    if(CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
        set(HOMEBREW_PREFIX "/opt/homebrew")
    else()
        set(HOMEBREW_PREFIX "/usr/local")
    endif()
    list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX}")
    link_directories("${HOMEBREW_PREFIX}/lib")
    include_directories("${HOMEBREW_PREFIX}/include")
endif()

# ソースファイル (Linux版と共通)
set(CORE_SOURCES
    src/core/AsirEngine.cpp
    src/core/LocalAsirEngine.cpp
    src/core/CommandHistory.cpp
)

set(DESKTOP_SOURCES
    src/desktop/MainWindow.cpp
    src/desktop/InputWidget.cpp
    src/desktop/OutputWidget.cpp
    src/desktop/MathView.cpp
    src/desktop/SettingsDialog.cpp
)

add_executable(asirgui
    src/main.cpp
    ${CORE_SOURCES}
    ${DESKTOP_SOURCES}
    src/resources/resources.qrc
)

# macOS アプリケーションバンドル設定
if(APPLE)
    set_target_properties(asirgui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/platform/macos/Info.plist
        MACOSX_BUNDLE_ICON_FILE asirgui.icns
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.openxm.asirgui"
        MACOSX_BUNDLE_BUNDLE_NAME "AsirGUI"
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    )

    # アイコンファイルをバンドルに含める
    set(MACOS_ICON ${CMAKE_SOURCE_DIR}/platform/macos/asirgui.icns)
    set_source_files_properties(${MACOS_ICON} PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(asirgui PRIVATE ${MACOS_ICON})
endif()

target_include_directories(asirgui PRIVATE
    src
    ${ASIR_INCDIR}
)

target_link_libraries(asirgui PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebChannel
    ${LIBASIR}
    ${LIBGC}
    pthread
    mpfr
    mpc
    gmp
    m
)

# macOS 固有: フレームワークリンク
if(APPLE)
    target_link_libraries(asirgui PRIVATE
        "-framework Cocoa"
        "-framework CoreFoundation"
    )
endif()

install(TARGETS asirgui
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin)
```

## 7. Info.plist

```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"
    "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key>
    <string>AsirGUI</string>
    <key>CFBundleDisplayName</key>
    <string>AsirGUI</string>
    <key>CFBundleIdentifier</key>
    <string>org.openxm.asirgui</string>
    <key>CFBundleVersion</key>
    <string>1.0.0</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundleExecutable</key>
    <string>asirgui</string>
    <key>CFBundleIconFile</key>
    <string>asirgui.icns</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>LSMinimumSystemVersion</key>
    <string>12.0</string>
    <key>CFBundleDocumentTypes</key>
    <array>
        <dict>
            <key>CFBundleTypeName</key>
            <string>Asir Script</string>
            <key>CFBundleTypeExtensions</key>
            <array>
                <string>rr</string>
                <string>asir</string>
            </array>
            <key>CFBundleTypeRole</key>
            <string>Editor</string>
        </dict>
    </array>
</dict>
</plist>
```

## 8. ビルド手順

### 通常ビルド

```bash
cd AsirGUI
mkdir build && cd build

# Qt6 のパスを指定 (Homebrew の場合)
cmake .. -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) \
         -DOpenXM_HOME=/path/to/OpenXM

make -j$(sysctl -n hw.ncpu)

# アプリケーションバンドルが build/asirgui.app に生成される
open asirgui.app
```

### Universal Binary ビルド (Intel + Apple Silicon)

```bash
# arm64 ビルド
cmake .. -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" \
         -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) \
         -DOpenXM_HOME=/path/to/OpenXM
make -j$(sysctl -n hw.ncpu)

# 注意: libasir.a も Universal Binary である必要がある
# または、それぞれのアーキテクチャで個別にビルドし lipo で結合
```

## 9. 配布方式

### DMG インストーラ

```bash
#!/bin/bash
# platform/macos/create_dmg.sh

APP_NAME="AsirGUI"
BUILD_DIR="build"
DMG_NAME="${APP_NAME}.dmg"

# Qt のフレームワークをバンドルに同梱
$(brew --prefix qt@6)/bin/macdeployqt \
    ${BUILD_DIR}/${APP_NAME}.app \
    -dmg \
    -verbose=1

# libasir.a の依存ライブラリ (動的リンクの場合) を fixup
# install_name_tool で @rpath を設定

echo "Created ${DMG_NAME}"
```

### Homebrew Formula (将来)

```ruby
class Asirgui < Formula
  desc "GUI for Risa/Asir computer algebra system"
  homepage "https://www.openxm.org"
  url "https://github.com/openxm-org/AsirGUI/archive/v1.0.tar.gz"
  depends_on "qt@6"
  depends_on "gmp"
  depends_on "mpfr"
  depends_on "libmpc"
  depends_on "bdw-gc"
  # ...
end
```

## 10. Linux版との差分まとめ

| 項目 | Linux | macOS |
|------|-------|-------|
| エンジン | LocalAsirEngine (同一) | LocalAsirEngine (同一) |
| GUI コード | 共通 | 共通 (メニュー位置が macOS ネイティブに) |
| ビルドシステム | CMake (同一) | CMake + MACOSX_BUNDLE |
| 依存管理 | apt | Homebrew |
| libasir.a ビルド | 既存 | macOS向けに要ビルド |
| アイコン | PNG | ICNS |
| 配布形式 | AppImage | DMG |
| HiDPI | 標準対応 | Retina 自動対応 |
| ダークモード | 対応 | macOS ダークモード連動 |
| サンドボックス | なし | App Sandbox 対応 (配布時) |
| コード署名 | 不要 | Developer ID 署名 (配布時) |

## 11. 既知の技術的課題

| 課題 | 対策 |
|------|------|
| libasir.a の macOS ビルド | OpenXM の configure は __DARWIN__ を検出済み。ビルド実績を確認 |
| Apple Silicon 対応 | arm64 でのビルド・テストが必要。GMP等は Homebrew arm64版あり |
| Qt WebEngine の Chromium sandbox | App Sandbox と干渉する可能性。entitlements.plist で対応 |
| コード署名 / 公証 (Notarization) | 配布時に Apple Developer アカウントが必要 |
| Gatekeeper 警告 | 署名なしバイナリは「開発元が未確認」と表示される |
| Boehm GC と macOS のスレッド | GC_INIT() を main スレッドで呼び、GC_allow_register_threads() 設定 |
