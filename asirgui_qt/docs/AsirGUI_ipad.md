# AsirGUI iPadOS 実装計画

## 1. 概要

iPadOS版では、ネイティブの libasir.a が利用できないため、Emscripten で
libasir.a と依存ライブラリを WebAssembly (Wasm) にコンパイルし、
WKWebView 内で実行する。UIは Qt6.8 Quick (QML) で構築する。

- **GUI**: Qt 6.8 Quick (QML) for iOS/iPadOS
- **計算エンジン**: libasir Wasm (Emscripten でコンパイル)
- **Wasm 実行環境**: WKWebView (JavaScript ↔ Wasm)
- **数式描画**: KaTeX (同一 WKWebView 内)
- **LaTeX変換**: `print_tex_form()` (names.rr) — Wasm 内で実行
- **ターゲット**: iPadOS 16 以上、arm64

## 2. アーキテクチャ

```
┌──────────────────────────────────────────────────────┐
│              iPadOS アプリ (Qt6.8)                     │
│                                                       │
│  ┌─────────────────────────────────────────────────┐ │
│  │              QML UI Layer                        │ │
│  │  ┌──────────────────────────────────────────┐   │ │
│  │  │  入力領域 (TextField)                     │   │ │
│  │  │  - ハードウェアキーボード対応              │   │ │
│  │  │  - 数学記号パレット                        │   │ │
│  │  └──────────────────────────────────────────┘   │ │
│  │                                                  │ │
│  │  ┌──────────────────────────────────────────┐   │ │
│  │  │  出力領域                                 │   │ │
│  │  │  テキスト / KaTeX 数式 切替               │   │ │
│  │  └──────────────────────────────────────────┘   │ │
│  └──────────────────────┬──────────────────────────┘ │
│                         │ Qt WebChannel / JS Bridge   │
│  ┌──────────────────────▼──────────────────────────┐ │
│  │           WKWebView (Wasm 実行環境)              │ │
│  │                                                  │ │
│  │  ┌──────────────────────────────────────────┐   │ │
│  │  │  asir_engine.js (Emscripten生成)          │   │ │
│  │  │  ┌──────────────────────────────────┐    │   │ │
│  │  │  │  libasir.wasm                    │    │   │ │
│  │  │  │  + libgmp.wasm                   │    │   │ │
│  │  │  │  + libmpfr.wasm                  │    │   │ │
│  │  │  │  + libmpc.wasm                   │    │   │ │
│  │  │  │  + libgc.wasm                    │    │   │ │
│  │  │  └──────────────────────────────────┘    │   │ │
│  │  │                                          │   │ │
│  │  │  asir_api.js (JavaScript ラッパー)        │   │ │
│  │  │  - initAsir()                            │   │ │
│  │  │  - executeString(cmd) → Promise<result>  │   │ │
│  │  │  - getTexForm(expr) → Promise<latex>     │   │ │
│  │  │                                          │   │ │
│  │  │  KaTeX (数式レンダリング)                  │   │ │
│  │  └──────────────────────────────────────────┘   │ │
│  └─────────────────────────────────────────────────┘ │
└──────────────────────────────────────────────────────┘
```

### 通信フロー

```
1. ユーザーが QML UI でコマンド入力
2. Qt (C++) → WKWebView (JavaScript) へメッセージ送信
3. JavaScript → Wasm (libasir) でコマンド実行
4. Wasm 結果 → JavaScript → Qt (C++) へコールバック
5. 結果をテキスト表示 + KaTeX でLaTeX表示
```

## 3. Emscripten による Wasm ビルド

### 3.1 Emscripten 環境セットアップ

```bash
# Emscripten SDK のインストール
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh

# 確認
emcc --version
# emcc (Emscripten gcc/clang-like replacement) 3.x.x
```

### 3.2 依存ライブラリの Wasm ビルド

#### GMP

```bash
cd gmp-6.3.0

# Emscripten は Generic C のみサポート (アセンブリ最適化は無効)
emconfigure ./configure \
    --host=wasm32-unknown-emscripten \
    --prefix=$PWD/../wasm-libs \
    --disable-shared --enable-static \
    --disable-assembly \
    CC=emcc \
    CFLAGS="-O2"

emmake make -j$(nproc)
emmake make install
```

#### MPFR

```bash
cd mpfr-4.2.1
emconfigure ./configure \
    --host=wasm32-unknown-emscripten \
    --prefix=$PWD/../wasm-libs \
    --with-gmp=$PWD/../wasm-libs \
    --disable-shared --enable-static \
    CC=emcc

emmake make -j$(nproc)
emmake make install
```

#### MPC

```bash
cd mpc-1.3.1
emconfigure ./configure \
    --host=wasm32-unknown-emscripten \
    --prefix=$PWD/../wasm-libs \
    --with-gmp=$PWD/../wasm-libs \
    --with-mpfr=$PWD/../wasm-libs \
    --disable-shared --enable-static \
    CC=emcc

emmake make -j$(nproc)
emmake make install
```

#### Boehm GC (Wasm対応)

```bash
# Boehm GC の Wasm 対応は限定的。以下の2つのアプローチ:

# アプローチ A: Boehm GC を Wasm 向けにビルド (実験的)
cd gc-8.2.6
emconfigure ./configure \
    --host=wasm32-unknown-emscripten \
    --prefix=$PWD/../wasm-libs \
    --disable-shared --enable-static \
    --disable-threads \
    CC=emcc

# アプローチ B: Boehm GC の代替として Emscripten の malloc を使用
# libasir のソースで GC_malloc を malloc に置換するパッチが必要
# → メモリリークの可能性があるが、短時間のセッションでは実用的
```

**推奨**: まずアプローチ A を試み、失敗した場合は B に切り替える。
Emscripten の WASM メモリは自動拡張可能なので、B でも実用的。

#### libasir.a (Wasm版)

```bash
cd OpenXM_contrib2/asir2018

emconfigure ./configure \
    --host=wasm32-unknown-emscripten \
    CC=emcc \
    CFLAGS="-O2 -I$PWD/../../wasm-libs/include" \
    LDFLAGS="-L$PWD/../../wasm-libs/lib" \
    --with-gmp=$PWD/../../wasm-libs \
    --with-mpfr=$PWD/../../wasm-libs \
    --disable-plot

emmake make libasir
# → libasir.a (Wasm bitcode) が生成される
```

### 3.3 JavaScript ラッパーの生成

Emscripten で libasir.a を JavaScript から呼び出せるモジュールにリンク：

```bash
# wasm/build_wasm.sh

WASM_LIBS=$PWD/wasm-libs
ASIR_DIR=$PWD/OpenXM_contrib2/asir2018

emcc -O2 \
    -s WASM=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME="AsirModule" \
    -s EXPORTED_FUNCTIONS='["_asir_ox_init","_asir_ox_execute_string","_asir_ox_pop_string","_asir_ox_peek_cmo_string_length","_asir_ox_push_cmo","_asir_ox_pop_cmo","_asir_ox_peek_cmo_size","_asir_ox_push_cmd","_malloc","_free"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","UTF8ToString","stringToUTF8","allocateUTF8"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=67108864 \
    -s MAXIMUM_MEMORY=536870912 \
    -s NO_EXIT_RUNTIME=1 \
    -s ASYNCIFY \
    wasm/asir_wasm_wrapper.c \
    $ASIR_DIR/libasir.a \
    $WASM_LIBS/lib/libgc.a \
    $WASM_LIBS/lib/libgmp.a \
    $WASM_LIBS/lib/libmpfr.a \
    $WASM_LIBS/lib/libmpc.a \
    -lm \
    -o wasm/asir_engine.js

# 出力: asir_engine.js + asir_engine.wasm
```

### 3.4 C ラッパー (Wasm エントリポイント)

```c
// wasm/asir_wasm_wrapper.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// libasir API
extern int asir_ox_init(int byteorder);
extern void asir_ox_execute_string(char *s);
extern int asir_ox_pop_string(void *string, int limit);
extern int asir_ox_peek_cmo_string_length(void);

// Emscripten 用ダミー関数
int find_asirrc() { return 0; }

// 初期化済みフラグ
static int initialized = 0;

// 初期化
int wasm_asir_init() {
    if (!initialized) {
        asir_ox_init(1);
        initialized = 1;
    }
    return 0;
}

// コマンド実行して結果を文字列で返す
// 呼び出し元 (JS) が返されたポインタを free する
char* wasm_asir_execute(const char *command) {
    asir_ox_execute_string((char*)command);
    int len = asir_ox_peek_cmo_string_length();
    if (len <= 0) return NULL;
    char *buf = (char*)malloc(len + 1);
    asir_ox_pop_string(buf, len);
    buf[len] = '\0';
    return buf;
}
```

### 3.5 JavaScript API ラッパー

```javascript
// wasm/asir_api.js

let asirModule = null;
let asirReady = false;

async function initAsir() {
    asirModule = await AsirModule();

    // C ラッパー関数をバインド
    const wasmInit = asirModule.cwrap('wasm_asir_init', 'number', []);
    const wasmExecute = asirModule.cwrap('wasm_asir_execute', 'number', ['string']);

    wasmInit();

    // names.rr をロード (Emscripten仮想ファイルシステムに配置済み)
    wasmExecute('load("/asir/names.rr");');

    asirReady = true;

    // グローバル API を公開
    window.asirAPI = {
        execute: function(cmd) {
            if (!asirReady) return { text: "Engine not ready", latex: "" };
            const resultPtr = wasmExecute(cmd);
            const text = asirModule.UTF8ToString(resultPtr);
            asirModule._free(resultPtr);

            // LaTeX 変換
            let latex = "";
            try {
                const texCmd = '__gui_tmp=' + cmd.replace(/;$/, '') +
                               '; print_tex_form(__gui_tmp);';
                const texPtr = wasmExecute(texCmd);
                if (texPtr) {
                    latex = asirModule.UTF8ToString(texPtr);
                    asirModule._free(texPtr);
                }
            } catch(e) {
                // LaTeX変換失敗時はテキストのみ
            }

            return { text: text, latex: latex };
        },

        isReady: function() {
            return asirReady;
        }
    };

    // Qt 側に準備完了を通知
    if (window.qtBridge) {
        window.qtBridge.engineReady();
    }
}

// ページロード時に初期化開始
initAsir();
```

## 4. プロジェクト構成

```
AsirGUI/
├── CMakeLists.txt
├── src/
│   ├── main_ios.cpp                   # iPadOS 用 main
│   ├── core/
│   │   ├── AsirEngine.h / .cpp        # 共通抽象クラス
│   │   ├── WasmAsirEngine.h           # Wasm エンジン (iPadOS用)
│   │   ├── WasmAsirEngine.cpp
│   │   └── CommandHistory.h / .cpp
│   ├── mobile/
│   │   ├── qml/                       # Android版と共通 QML
│   │   │   ├── main.qml
│   │   │   ├── InputArea.qml
│   │   │   ├── OutputArea.qml
│   │   │   ├── MathView.qml
│   │   │   ├── HistoryDrawer.qml
│   │   │   ├── SymbolPalette.qml
│   │   │   └── SettingsPage.qml
│   │   └── MobileAsirBridge.h / .cpp
│   └── resources/
│       ├── katex/
│       ├── math_render.html
│       └── resources.qrc
├── wasm/                              # Wasm ビルド関連
│   ├── build_wasm.sh                  # Wasm ビルドスクリプト
│   ├── asir_wasm_wrapper.c            # C ラッパー
│   ├── asir_api.js                    # JS API ラッパー
│   ├── asir_wasm_page.html            # Wasm + KaTeX 統合ページ
│   └── output/                        # ビルド成果物
│       ├── asir_engine.js
│       └── asir_engine.wasm
├── platform/
│   └── ios/
│       ├── Info.plist
│       ├── LaunchScreen.storyboard
│       ├── Assets.xcassets/           # アプリアイコン
│       └── entitlements.plist
└── tests/
```

## 5. 実装フェーズ

### フェーズ1: Wasm エンジン

#### 5.1 WasmAsirEngine

Qt (C++) と WKWebView 内の Wasm エンジンを橋渡しするクラス：

```cpp
// core/WasmAsirEngine.h
#pragma once
#include "AsirEngine.h"
#include <QWebEngineView>
#include <QWebChannel>

class WasmAsirEngine : public AsirEngine {
    Q_OBJECT
public:
    explicit WasmAsirEngine(QObject *parent = nullptr);

    bool initialize() override;
    void shutdown() override;
    void executeString(const QString &command) override;
    bool isReady() const override;

    // WKWebView のインスタンスを取得 (QML から参照)
    QWebEngineView* webView() const { return m_webView; }

private:
    QWebEngineView *m_webView = nullptr;
    QWebChannel *m_channel = nullptr;
    bool m_ready = false;

    // JavaScript からのコールバック受信用
    class JsBridge : public QObject {
        Q_OBJECT
    public:
        using QObject::QObject;
    public slots:
        void onEngineReady();
        void onResult(const QString &text, const QString &latex);
        void onError(const QString &error);
    signals:
        void engineReady();
        void resultReady(const QString &text, const QString &latex);
        void errorOccurred(const QString &error);
    };
    JsBridge *m_jsBridge = nullptr;
};
```

```cpp
// core/WasmAsirEngine.cpp
#include "WasmAsirEngine.h"

bool WasmAsirEngine::initialize() {
    m_webView = new QWebEngineView();

    // WebChannel でJS ↔ C++ 通信
    m_channel = new QWebChannel(this);
    m_jsBridge = new JsBridge(this);
    m_channel->registerObject("qtBridge", m_jsBridge);
    m_webView->page()->setWebChannel(m_channel);

    // Wasm + KaTeX 統合ページをロード
    m_webView->setUrl(QUrl("qrc:/wasm/asir_wasm_page.html"));

    connect(m_jsBridge, &JsBridge::engineReady, this, [this]() {
        m_ready = true;
        emit engineReady();
    });
    connect(m_jsBridge, &JsBridge::resultReady,
            this, &AsirEngine::resultReady);
    connect(m_jsBridge, &JsBridge::errorOccurred,
            this, &AsirEngine::errorOccurred);

    return true;
}

void WasmAsirEngine::executeString(const QString &command) {
    QString escaped = command;
    escaped.replace("\\", "\\\\").replace("'", "\\'");
    QString js = QString(
        "try {"
        "  var r = window.asirAPI.execute('%1');"
        "  qtBridge.onResult(r.text, r.latex);"
        "} catch(e) {"
        "  qtBridge.onError(e.toString());"
        "}"
    ).arg(escaped);
    m_webView->page()->runJavaScript(js);
}
```

#### 5.2 Wasm + KaTeX 統合ページ

```html
<!-- wasm/asir_wasm_page.html -->
<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link rel="stylesheet" href="qrc:/katex/katex.min.css">
    <script src="qrc:/katex/katex.min.js"></script>
    <script src="qrc:/qtwebchannel/qwebchannel.js"></script>
    <style>
        body { font-size: 18px; padding: 10px; margin: 0; }
        .math-block { margin: 8px 0; }
        .text-block { font-family: monospace; white-space: pre-wrap; margin: 4px 0; }
        .error { color: red; }
        .loading { text-align: center; padding: 20px; color: #666; }
    </style>
</head>
<body>
    <div id="loading" class="loading">Initializing Asir engine...</div>
    <div id="output" style="display:none;"></div>

    <!-- Wasm エンジン -->
    <script src="qrc:/wasm/asir_engine.js"></script>
    <script src="qrc:/wasm/asir_api.js"></script>

    <script>
    // Qt WebChannel セットアップ
    var qtBridge = null;

    new QWebChannel(qt.webChannelTransport, function(channel) {
        qtBridge = channel.objects.qtBridge;
        window.qtBridge = qtBridge;

        // Wasm がすでに準備完了していれば通知
        if (window.asirAPI && window.asirAPI.isReady()) {
            qtBridge.onEngineReady();
            document.getElementById('loading').style.display = 'none';
            document.getElementById('output').style.display = 'block';
        }
    });

    // KaTeX レンダリング関数
    function renderMath(latex) {
        var div = document.createElement('div');
        div.className = 'math-block';
        try {
            katex.render(latex, div, {
                displayMode: true,
                throwOnError: false,
                trust: true
            });
        } catch(e) {
            div.className = 'error';
            div.textContent = 'TeX error: ' + e.message;
        }
        document.getElementById('output').appendChild(div);
        window.scrollTo(0, document.body.scrollHeight);
    }

    function appendText(text) {
        var div = document.createElement('div');
        div.className = 'text-block';
        div.textContent = text;
        document.getElementById('output').appendChild(div);
        window.scrollTo(0, document.body.scrollHeight);
    }

    function clearOutput() {
        document.getElementById('output').innerHTML = '';
    }
    </script>
</body>
</html>
```

### フェーズ2: QML UI (Android版と共通)

iPadOS 版の QML は Android 版と基本的に共通。以下の iPad 固有対応を追加：

#### 5.3 iPad 固有: マルチタスキング対応

```qml
// mobile/qml/main.qml (iPad 追加)
ApplicationWindow {
    // iPad の Split View / Slide Over に対応
    // Qt6 は自動的にウィンドウサイズ変更を処理

    // iPad の外付けキーボード対応
    Shortcut {
        sequence: StandardKey.Open
        onActivated: fileDialog.open()
    }
    Shortcut {
        sequence: "Ctrl+Return"
        onActivated: {
            let cmd = inputArea.currentText()
            if (cmd.length > 0) asirBridge.execute(cmd)
        }
    }
}
```

#### 5.4 iPad 固有: Apple Pencil 対応 (将来)

```qml
// 将来的な手書き数式入力の基盤
// Scribble (手書きテキスト入力) は TextArea で自動サポート
TextArea {
    id: inputField
    // iPadOS の Scribble は自動的に手書きをテキストに変換
}
```

### フェーズ3: WasmAsirEngine の QML 統合

iPadOS では WebEngineView の代わりに WKWebView ベースのアプローチ：

```cpp
// main_ios.cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "core/WasmAsirEngine.h"
#include "mobile/MobileAsirBridge.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);

    WasmAsirEngine *engine = new WasmAsirEngine(&app);
    MobileAsirBridge *bridge = new MobileAsirBridge(engine, &app);

    QQmlApplicationEngine qmlEngine;
    qmlEngine.rootContext()->setContextProperty("asirBridge", bridge);

    // WasmAsirEngine の WebView を QML に公開
    // (QML の WebView コンポーネントとして統合)
    qmlEngine.loadFromModule("AsirGUI", "Main");

    engine->initialize();

    return app.exec();
}
```

## 6. Emscripten 仮想ファイルシステム

names.rr と asir-contrib を Wasm の仮想ファイルシステムに埋め込む：

```bash
# ビルド時に --preload-file でファイルを埋め込み
emcc ... \
    --preload-file names.rr@/asir/names.rr \
    --preload-file noro_print.rr@/asir/noro_print.rr \
    --preload-file tex_symbols.rr@/asir/tex_symbols.rr \
    --preload-file noro_print_default.rr@/asir/noro_print_default.rr \
    -o asir_engine.js

# → asir_engine.data ファイルが生成される (仮想FS データ)
```

これにより、Wasm 内の libasir が `load("/asir/names.rr")` でファイルを読める。

## 7. CMake ビルド設定

```cmake
# iOS/iPadOS 用 CMakeLists.txt

if(IOS)
    find_package(Qt6 6.8 REQUIRED COMPONENTS
        Quick QuickControls2 WebEngineQuick WebChannel)

    set(MOBILE_SOURCES
        src/main_ios.cpp
        src/core/AsirEngine.cpp
        src/core/WasmAsirEngine.cpp
        src/core/CommandHistory.cpp
        src/mobile/MobileAsirBridge.cpp
    )

    # Wasm ビルド成果物をリソースに含める
    set(WASM_RESOURCES
        wasm/output/asir_engine.js
        wasm/output/asir_engine.wasm
        wasm/output/asir_engine.data
        wasm/asir_api.js
        wasm/asir_wasm_page.html
    )

    qt_add_executable(asirgui ${MOBILE_SOURCES})

    qt_add_qml_module(asirgui
        URI AsirGUI
        VERSION 1.0
        QML_FILES
            src/mobile/qml/main.qml
            src/mobile/qml/InputArea.qml
            src/mobile/qml/OutputArea.qml
            src/mobile/qml/MathView.qml
            src/mobile/qml/HistoryDrawer.qml
            src/mobile/qml/SymbolPalette.qml
            src/mobile/qml/SettingsPage.qml
        RESOURCES
            src/resources/katex/katex.min.js
            src/resources/katex/katex.min.css
            src/resources/math_render.html
            ${WASM_RESOURCES}
    )

    target_include_directories(asirgui PRIVATE src)

    target_link_libraries(asirgui PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::WebEngineQuick
        Qt6::WebChannel
    )

    # iOS バンドル設定
    set_target_properties(asirgui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/platform/ios/Info.plist
        MACOSX_BUNDLE_GUI_IDENTIFIER "org.openxm.asirgui"
        XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "2"  # iPad only
        XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "16.0"
    )
endif()
```

## 8. Info.plist (iPadOS)

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
    <key>LSRequiresIPhoneOS</key>
    <true/>
    <key>UIDeviceFamily</key>
    <array>
        <integer>2</integer>  <!-- iPad のみ -->
    </array>
    <key>UISupportedInterfaceOrientations~ipad</key>
    <array>
        <string>UIInterfaceOrientationPortrait</string>
        <string>UIInterfaceOrientationLandscapeLeft</string>
        <string>UIInterfaceOrientationLandscapeRight</string>
    </array>
    <key>UILaunchStoryboardName</key>
    <string>LaunchScreen</string>
    <key>UIRequiresFullScreen</key>
    <false/>  <!-- Split View 対応 -->
    <key>UISupportsDocumentBrowser</key>
    <true/>  <!-- .rr ファイルを開く -->
    <key>CFBundleDocumentTypes</key>
    <array>
        <dict>
            <key>CFBundleTypeName</key>
            <string>Asir Script</string>
            <key>LSItemContentTypes</key>
            <array>
                <string>org.openxm.asir-script</string>
            </array>
        </dict>
    </array>
    <key>UTExportedTypeDeclarations</key>
    <array>
        <dict>
            <key>UTTypeIdentifier</key>
            <string>org.openxm.asir-script</string>
            <key>UTTypeConformsTo</key>
            <array>
                <string>public.plain-text</string>
            </array>
            <key>UTTypeTagSpecification</key>
            <dict>
                <key>public.filename-extension</key>
                <array>
                    <string>rr</string>
                    <string>asir</string>
                </array>
            </dict>
        </dict>
    </array>
</dict>
</plist>
```

## 9. ビルド手順

### Wasm ビルド (開発マシン上)

```bash
# 1. 依存ライブラリの Wasm ビルド (セクション3参照)
cd AsirGUI/wasm
./build_wasm.sh

# 出力:
#   output/asir_engine.js
#   output/asir_engine.wasm
#   output/asir_engine.data
```

### iPadOS アプリビルド (macOS 上)

```bash
# macOS + Xcode 15+ が必要
cd AsirGUI
mkdir build-ios && cd build-ios

# Qt for iOS の CMake を使用
$HOME/Qt/6.8.0/ios/bin/qt-cmake .. \
    -DCMAKE_OSX_SYSROOT=iphoneos \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -GXcode

# Xcode でビルド
xcodebuild -project asirgui.xcodeproj \
    -scheme asirgui \
    -configuration Release \
    -sdk iphoneos \
    -destination 'generic/platform=iOS'

# または Xcode GUI から直接ビルド・デバッグ
open asirgui.xcodeproj
```

## 10. パフォーマンス考慮

### Wasm の制約と最適化

| 項目 | ネイティブ (Linux/Android) | Wasm (iPadOS) | 対策 |
|------|---------------------------|---------------|------|
| 計算速度 | 1x (基準) | 2-5x 遅い | O2最適化、SIMD有効化 |
| メモリ | OS制限 | 初期64MB、最大512MB | ALLOW_MEMORY_GROWTH |
| 起動時間 | 即時 | Wasm ロード 1-3秒 | スプラッシュスクリーン |
| ファイルI/O | ネイティブFS | 仮想FS (Emscripten) | preload-file |
| スレッド | pthreads | SharedArrayBuffer (制限) | シングルスレッド推奨 |

### Wasm バイナリサイズの最適化

```bash
# サイズ最適化オプション
emcc -Oz \
    -s MINIMAL_RUNTIME=0 \
    -s TEXTDECODER=2 \
    --closure 1 \
    ...

# 予想サイズ:
#   asir_engine.wasm: 3-8 MB
#   asir_engine.js:   200-500 KB
#   asir_engine.data: 100-300 KB (names.rr 等)
#   KaTeX:            ~1 MB
#   合計:             5-10 MB
```

## 11. 既知の技術的課題

| 課題 | 深刻度 | 対策 |
|------|--------|------|
| Boehm GC の Wasm 対応 | 高 | GC なしビルドを試行。malloc/free に置換するパッチ |
| libasir の Emscripten ビルド | 高 | configure/Makefile の修正が必要。不足するシステムコール対応 |
| GMP のアセンブリ最適化無効 | 中 | --disable-assembly で Generic C ビルド。速度低下あり |
| Wasm の起動遅延 | 中 | ローディング画面表示。Streaming Compilation 活用 |
| WKWebView のメモリ制限 | 中 | iPadOS のプロセスメモリ制限。大規模計算で OOM の可能性 |
| Qt WebEngine の iOS サポート | 中 | Qt6.8 時点で iOS の WebEngine は制限あり。WKWebView 直接利用も検討 |
| App Store 審査 | 低 | Wasm 実行はApp Store ガイドライン準拠 (JIT不使用) |
| ファイル共有 (iCloud/Files) | 低 | UIDocumentBrowserViewController で対応 |

## 12. 代替アーキテクチャ: 完全 Web アプリ (PWA)

Wasm ビルドが完成した場合、iPadOS 専用アプリに加えて
Progressive Web App (PWA) としても配布可能：

```
asir-web/
├── index.html            # メイン HTML
├── asir_engine.js        # Emscripten 生成
├── asir_engine.wasm      # Wasm バイナリ
├── asir_engine.data      # 仮想FS データ
├── asir_api.js           # JS API
├── katex/                # KaTeX
├── app.js                # UI ロジック
├── style.css             # スタイル
├── manifest.json         # PWA マニフェスト
└── sw.js                 # Service Worker (オフライン対応)
```

**利点**: App Store 不要、ブラウザだけで動作、全プラットフォーム対応
**欠点**: Safari の制限、ネイティブ機能制限

## 13. 配布方式

### App Store (TestFlight → 正式リリース)

```
1. Apple Developer Program 加入 (年額 $99)
2. Xcode で Archive → App Store Connect にアップロード
3. TestFlight で内部テスト
4. App Store 審査提出
```

### Ad Hoc 配布 (開発・テスト用)

```
1. Provisioning Profile 作成 (デバイスUDID登録)
2. Xcode で Ad Hoc ビルド
3. .ipa ファイルを配布
```

## 14. Android版との QML コード共有

| ファイル | 共通 | iPad固有 |
|----------|------|----------|
| main.qml | ほぼ共通 | キーボードショートカット追加 |
| InputArea.qml | 共通 | Apple Pencil / Scribble 対応 |
| OutputArea.qml | 共通 | — |
| MathView.qml | **異なる** | Wasm統合WebView vs Android WebView |
| HistoryDrawer.qml | 共通 | — |
| SymbolPalette.qml | 共通 | — |
| SettingsPage.qml | 共通 | — |

**MathView.qml の分岐**:
- Android: Qt WebView → KaTeX のみ
- iPad: Qt WebEngineView → KaTeX + Wasm エンジン統合

エンジンの違い (LocalAsirEngine vs WasmAsirEngine) は C++ 層で吸収し、
QML からは MobileAsirBridge の統一 API を通じてアクセスする。
