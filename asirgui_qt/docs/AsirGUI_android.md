# AsirGUI Android 実装計画

## 1. 概要

Android版は、Qt6.8 Quick (QML) でタッチ最適化UIを構築し、libasir.a を
Android NDK でクロスコンパイルして直接リンクする。

- **GUI**: Qt 6.8 Quick (QML) — タッチ操作に最適化
- **計算エンジン**: libasir.a (Android NDK クロスコンパイル、JNI経由)
- **数式描画**: KaTeX (Qt WebView / Android WebView)
- **LaTeX変換**: `print_tex_form()` (names.rr)
- **ターゲット**: Android 10 (API 29) 以上、arm64-v8a / x86_64

## 2. アーキテクチャ

```
┌──────────────────────────────────────────────────────┐
│                 Android アプリ                         │
│                                                       │
│  ┌─────────────────────────────────────────────────┐ │
│  │             QML UI Layer                         │ │
│  │                                                  │ │
│  │  ┌──────────────────────────────────────────┐   │ │
│  │  │  入力領域 (TextField / TextArea)          │   │ │
│  │  │  - ソフトキーボード連携                    │   │ │
│  │  │  - コマンド履歴 (スワイプ)                 │   │ │
│  │  │  - 数学記号パレット                        │   │ │
│  │  └──────────────────────────────────────────┘   │ │
│  │                                                  │ │
│  │  ┌──────────────────────────────────────────┐   │ │
│  │  │  出力領域                                 │   │ │
│  │  │  ┌────────────────────────────────────┐  │   │ │
│  │  │  │ テキスト表示 (ScrollView + Text)    │  │   │ │
│  │  │  ├────────────────────────────────────┤  │   │ │
│  │  │  │ 数式表示 (WebView + KaTeX)         │  │   │ │
│  │  │  └────────────────────────────────────┘  │   │ │
│  │  └──────────────────────────────────────────┘   │ │
│  └─────────────────────────────────────────────────┘ │
│                                                       │
│  ┌─────────────────────────────────────────────────┐ │
│  │  C++ Backend (Qt/JNI)                            │ │
│  │                                                  │ │
│  │  AsirEngine (ワーカースレッド)                    │ │
│  │  ├── libasir.a (arm64-v8a / x86_64)             │ │
│  │  ├── libgc.a                                    │ │
│  │  ├── libgmp.a                                   │ │
│  │  ├── libmpfr.a                                  │ │
│  │  └── libmpc.a                                   │ │
│  └─────────────────────────────────────────────────┘ │
└──────────────────────────────────────────────────────┘
```

## 3. ビルド環境

### 開発マシン (Linux/macOS/Windows)

```bash
# Android SDK & NDK
# Android Studio から以下をインストール:
#   - Android SDK Platform 34 (API 34)
#   - Android NDK r26 以降
#   - CMake 3.22+
#   - Android SDK Build-Tools

# 環境変数
export ANDROID_SDK_ROOT=$HOME/Android/Sdk
export ANDROID_NDK_ROOT=$ANDROID_SDK_ROOT/ndk/26.1.10909125
export PATH=$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$PATH

# Qt 6.8 for Android
# Qt Online Installer から:
#   - Qt 6.8.x / Android (arm64-v8a, x86_64)
#   - Qt WebView (WebEngine はAndroid非対応、WebView を使用)
export QT_ANDROID=$HOME/Qt/6.8.0/android_arm64_v8a
```

### クロスコンパイルツールチェイン

Android NDK のツールチェインを使用して、libasir.a と依存ライブラリを
arm64-v8a / x86_64 向けにクロスコンパイルする。

## 4. 依存ライブラリのクロスコンパイル

### 4.1 GMP (GNU Multiple Precision)

```bash
# GMP 6.3.0
wget https://gmplib.org/download/gmp/gmp-6.3.0.tar.xz
tar xf gmp-6.3.0.tar.xz
cd gmp-6.3.0

# arm64-v8a 向け
export TOOLCHAIN=$ANDROID_NDK_ROOT/toolchains/llvm/prebuilt/linux-x86_64
export TARGET=aarch64-linux-android
export API=29

./configure \
    --host=$TARGET \
    --prefix=$PWD/../android-libs/arm64-v8a \
    CC=$TOOLCHAIN/bin/${TARGET}${API}-clang \
    CXX=$TOOLCHAIN/bin/${TARGET}${API}-clang++ \
    --disable-shared --enable-static

make -j$(nproc) && make install
```

### 4.2 MPFR

```bash
cd mpfr-4.2.1
./configure \
    --host=$TARGET \
    --prefix=$PWD/../android-libs/arm64-v8a \
    --with-gmp=$PWD/../android-libs/arm64-v8a \
    CC=$TOOLCHAIN/bin/${TARGET}${API}-clang \
    --disable-shared --enable-static

make -j$(nproc) && make install
```

### 4.3 MPC

```bash
cd mpc-1.3.1
./configure \
    --host=$TARGET \
    --prefix=$PWD/../android-libs/arm64-v8a \
    --with-gmp=$PWD/../android-libs/arm64-v8a \
    --with-mpfr=$PWD/../android-libs/arm64-v8a \
    CC=$TOOLCHAIN/bin/${TARGET}${API}-clang \
    --disable-shared --enable-static

make -j$(nproc) && make install
```

### 4.4 Boehm GC

```bash
cd gc-8.2.6
./configure \
    --host=$TARGET \
    --prefix=$PWD/../android-libs/arm64-v8a \
    CC=$TOOLCHAIN/bin/${TARGET}${API}-clang \
    --disable-shared --enable-static \
    --enable-threads=posix

make -j$(nproc) && make install
```

### 4.5 libasir.a

```bash
# OpenXM_contrib2/asir2018 を Android 向けクロスコンパイル
cd OpenXM_contrib2/asir2018

# Android 向け configure
./configure \
    --host=$TARGET \
    CC=$TOOLCHAIN/bin/${TARGET}${API}-clang \
    CFLAGS="-I$PWD/../../android-libs/arm64-v8a/include" \
    LDFLAGS="-L$PWD/../../android-libs/arm64-v8a/lib" \
    --with-gmp=$PWD/../../android-libs/arm64-v8a \
    --with-mpfr=$PWD/../../android-libs/arm64-v8a \
    --disable-plot    # X11 プロット機能は無効化

make libasir
# → libasir.a が生成される
```

**注意**: asir2018 の configure / Makefile が Android NDK クロスコンパイルに
対応していない場合、Makefile の手動修正が必要になる可能性がある。
この場合、CMake ベースのビルドスクリプトを別途作成する。

## 5. プロジェクト構成

```
AsirGUI/
├── CMakeLists.txt                    # トップレベル (全プラットフォーム)
├── src/
│   ├── main.cpp                      # デスクトップ用 main
│   ├── main_android.cpp              # Android 用 main
│   ├── core/                         # 共通コア (デスクトップと共通)
│   │   ├── AsirEngine.h / .cpp
│   │   ├── LocalAsirEngine.h / .cpp
│   │   └── CommandHistory.h / .cpp
│   ├── mobile/
│   │   ├── qml/
│   │   │   ├── main.qml             # メイン画面
│   │   │   ├── InputArea.qml        # 入力領域
│   │   │   ├── OutputArea.qml       # 出力領域 (テキスト + 数式切替)
│   │   │   ├── MathView.qml         # KaTeX WebView
│   │   │   ├── HistoryDrawer.qml    # 履歴サイドドロワー
│   │   │   ├── SymbolPalette.qml    # 数学記号パレット
│   │   │   ├── SettingsPage.qml     # 設定画面
│   │   │   └── components/
│   │   │       ├── ToolbarButton.qml
│   │   │       └── DisplayModeSwitch.qml
│   │   └── MobileAsirBridge.h / .cpp # QML ↔ C++ ブリッジ
│   └── resources/
│       ├── katex/
│       ├── math_render.html
│       ├── mobile_resources.qrc
│       └── icons/
├── android/
│   ├── AndroidManifest.xml
│   ├── build.gradle
│   ├── res/
│   │   ├── drawable/                 # アイコン
│   │   └── values/
│   │       └── strings.xml
│   └── libs/                         # クロスコンパイル済みライブラリ
│       ├── arm64-v8a/
│       │   ├── libasir.a
│       │   ├── libgc.a
│       │   ├── libgmp.a
│       │   ├── libmpfr.a
│       │   └── libmpc.a
│       └── x86_64/                   # エミュレータ用
│           └── ...
└── tests/
```

## 6. 実装フェーズ

### フェーズ1: QML UI 設計

#### 6.1 メイン画面 (main.qml)

```qml
// mobile/qml/main.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    visible: true
    title: "AsirGUI"

    // Android のステータスバー・ナビゲーションバーに対応
    flags: Qt.Window

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            ToolButton {
                icon.name: "menu"
                onClicked: historyDrawer.open()
            }
            Label {
                text: "AsirGUI"
                font.bold: true
                Layout.fillWidth: true
            }
            // 表示モード切替
            DisplayModeSwitch {
                id: displaySwitch
            }
            ToolButton {
                icon.name: "settings"
                onClicked: settingsPage.open()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // 出力領域 (上部、スクロール可能)
        OutputArea {
            id: outputArea
            Layout.fillWidth: true
            Layout.fillHeight: true
            displayMode: displaySwitch.mode  // "text", "math", "split"
        }

        // 区切り線
        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#ccc"
        }

        // 数学記号パレット (折りたたみ可能)
        SymbolPalette {
            id: symbolPalette
            Layout.fillWidth: true
            visible: symbolPaletteButton.checked
            onSymbolSelected: (symbol) => inputArea.insertSymbol(symbol)
        }

        // 入力領域 (下部)
        InputArea {
            id: inputArea
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            onCommandSubmitted: (cmd) => asirBridge.execute(cmd)
        }
    }

    // 履歴サイドドロワー
    HistoryDrawer {
        id: historyDrawer
        onCommandSelected: (cmd) => inputArea.setText(cmd)
    }

    // 設定画面
    SettingsPage {
        id: settingsPage
    }

    // C++ バックエンドとの接続
    Connections {
        target: asirBridge
        function onResultReady(text, latex) {
            outputArea.appendResult(text, latex)
        }
        function onErrorOccurred(error) {
            outputArea.appendError(error)
        }
        function onEngineReady() {
            inputArea.enabled = true
        }
    }
}
```

#### 6.2 入力領域 (InputArea.qml)

```qml
// mobile/qml/InputArea.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    signal commandSubmitted(string cmd)

    function insertSymbol(symbol) {
        inputField.insert(inputField.cursorPosition, symbol)
    }
    function setText(text) {
        inputField.text = text
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8

        // テキスト入力
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true

            TextArea {
                id: inputField
                placeholderText: "Enter Asir command..."
                font.family: "monospace"
                font.pixelSize: 16
                wrapMode: TextArea.Wrap

                // Enter で実行 (Shift+Enter で改行)
                Keys.onReturnPressed: (event) => {
                    if (event.modifiers & Qt.ShiftModifier) {
                        inputField.insert(inputField.cursorPosition, "\n")
                    } else {
                        let cmd = inputField.text.trim()
                        if (cmd.length > 0) {
                            root.commandSubmitted(cmd)
                            inputField.text = ""
                        }
                    }
                }
            }
        }

        // 実行ボタン
        RoundButton {
            Layout.preferredWidth: 56
            Layout.preferredHeight: 56
            icon.name: "play_arrow"
            highlighted: true
            onClicked: {
                let cmd = inputField.text.trim()
                if (cmd.length > 0) {
                    root.commandSubmitted(cmd)
                    inputField.text = ""
                }
            }
        }
    }
}
```

#### 6.3 数学記号パレット (SymbolPalette.qml)

タッチ操作で数学記号を素早く入力するためのパレット：

```qml
// mobile/qml/SymbolPalette.qml
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    height: 48
    signal symbolSelected(string symbol)

    ScrollView {
        anchors.fill: parent
        ScrollBar.vertical.policy: ScrollBar.AlwaysOff

        Row {
            spacing: 4
            padding: 4

            // 基本演算
            Repeater {
                model: [
                    { label: "x", value: "x" },
                    { label: "y", value: "y" },
                    { label: "z", value: "z" },
                    { label: "^", value: "^" },
                    { label: "(", value: "(" },
                    { label: ")", value: ")" },
                    { label: "[", value: "[" },
                    { label: "]", value: "]" },
                    { label: ",", value: "," },
                    { label: ";", value: ";" },
                    // 関数
                    { label: "fctr", value: "fctr(" },
                    { label: "gcd", value: "gcd(" },
                    { label: "diff", value: "diff(" },
                    { label: "subst", value: "subst(" },
                    { label: "det", value: "det(" },
                    { label: "gr", value: "gr(" },
                    // 定数
                    { label: "@pi", value: "@pi" },
                    { label: "@e", value: "@e" },
                    { label: "@i", value: "@i" }
                ]
                delegate: Button {
                    text: modelData.label
                    flat: true
                    font.pixelSize: 14
                    implicitWidth: Math.max(40, contentWidth + 16)
                    implicitHeight: 40
                    onClicked: root.symbolSelected(modelData.value)
                }
            }
        }
    }
}
```

#### 6.4 KaTeX 数式表示 (MathView.qml)

```qml
// mobile/qml/MathView.qml
import QtQuick
import QtWebView  // Android では WebEngineView ではなく WebView を使用

WebView {
    id: mathWebView
    url: "qrc:/resources/math_render.html"

    function renderLatex(latex) {
        var escaped = latex.replace(/\\/g, '\\\\').replace(/'/g, "\\'").replace(/\n/g, '\\n')
        runJavaScript("renderMath('" + escaped + "')")
    }

    function clearMath() {
        runJavaScript("clearOutput()")
    }
}
```

**注意**: Android では `Qt WebEngineView` は利用不可。代わりに `Qt WebView`
(Android の WebView をラップ) を使用する。

### フェーズ2: C++ バックエンド

#### 6.5 QML ↔ C++ ブリッジ

```cpp
// mobile/MobileAsirBridge.h
#pragma once
#include <QObject>
#include <QtQml/qqmlregistration.h>
#include "core/AsirEngine.h"
#include "core/CommandHistory.h"

class MobileAsirBridge : public QObject {
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)

public:
    explicit MobileAsirBridge(QObject *parent = nullptr);

    Q_INVOKABLE void execute(const QString &command);
    Q_INVOKABLE QString previousHistory();
    Q_INVOKABLE QString nextHistory();
    Q_INVOKABLE QStringList historyList() const;
    Q_INVOKABLE void loadFile(const QString &path);

    bool isReady() const;

signals:
    void resultReady(const QString &text, const QString &latex);
    void errorOccurred(const QString &error);
    void readyChanged();

private:
    AsirEngine *m_engine;
    CommandHistory m_history;
};
```

```cpp
// mobile/MobileAsirBridge.cpp
#include "MobileAsirBridge.h"
#include "core/LocalAsirEngine.h"

MobileAsirBridge::MobileAsirBridge(QObject *parent)
    : QObject(parent)
{
    m_engine = new LocalAsirEngine(this);

    connect(m_engine, &AsirEngine::resultReady,
            this, &MobileAsirBridge::resultReady);
    connect(m_engine, &AsirEngine::errorOccurred,
            this, &MobileAsirBridge::errorOccurred);
}

void MobileAsirBridge::execute(const QString &command) {
    m_history.add(command);
    m_engine->executeString(command);
}
```

#### 6.6 Android 用 main

```cpp
// main_android.cpp
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "mobile/MobileAsirBridge.h"

int main(int argc, char *argv[]) {
    QGuiApplication app(argc, argv);
    app.setOrganizationName("OpenXM");
    app.setApplicationName("AsirGUI");

    QQmlApplicationEngine engine;

    // AsirBridge をシングルトンとして登録
    MobileAsirBridge *bridge = new MobileAsirBridge(&app);
    engine.rootContext()->setContextProperty("asirBridge", bridge);

    engine.loadFromModule("AsirGUI", "Main");

    // エンジン初期化 (QML ロード後)
    bridge->engine()->initialize();

    return app.exec();
}
```

### フェーズ3: Android 固有対応

#### 6.7 ソフトキーボード対応

```qml
// InputArea.qml に追加
TextArea {
    id: inputField
    inputMethodHints: Qt.ImhNone  // すべての文字を許可
    // ソフトキーボードが表示されたら、入力領域が見えるようにスクロール
    onActiveFocusChanged: {
        if (activeFocus) {
            root.parent.ensureVisible(root)
        }
    }
}
```

#### 6.8 ファイルアクセス

```qml
// Android の Storage Access Framework を使用
import Qt.labs.platform

FileDialog {
    id: fileDialog
    title: "Open Asir Script"
    nameFilters: ["Asir Files (*.rr *.asir)", "All Files (*)"]
    onAccepted: asirBridge.loadFile(file)
}
```

#### 6.9 names.rr のバンドル

Android APK 内の assets にバンドルし、起動時に内部ストレージにコピー：

```cpp
void LocalAsirEngine::initializeAsirContrib() {
    // assets から names.rr を内部ストレージにコピー
    QString destDir = QStandardPaths::writableLocation(
        QStandardPaths::AppDataLocation) + "/asir";
    QDir().mkpath(destDir);

    QFile::copy("assets:/asir/names.rr", destDir + "/names.rr");

    // Asir にロードパスを設定し、names.rr をロード
    QString cmd = QString("load(\"%1/names.rr\");").arg(destDir);
    asir_ox_execute_string(cmd.toUtf8().data());
}
```

## 7. CMake ビルド設定

```cmake
# Android 用 CMakeLists.txt (条件ブロック)

if(ANDROID)
    # Qt6 for Android
    find_package(Qt6 6.8 REQUIRED COMPONENTS
        Quick QuickControls2 WebView)

    # クロスコンパイル済みライブラリ
    set(ANDROID_LIBS_DIR "${CMAKE_SOURCE_DIR}/android/libs/${ANDROID_ABI}")

    find_library(LIBASIR asir PATHS ${ANDROID_LIBS_DIR} NO_DEFAULT_PATH REQUIRED)
    find_library(LIBGC gc PATHS ${ANDROID_LIBS_DIR} NO_DEFAULT_PATH REQUIRED)
    find_library(LIBGMP gmp PATHS ${ANDROID_LIBS_DIR} NO_DEFAULT_PATH REQUIRED)
    find_library(LIBMPFR mpfr PATHS ${ANDROID_LIBS_DIR} NO_DEFAULT_PATH REQUIRED)
    find_library(LIBMPC mpc PATHS ${ANDROID_LIBS_DIR} NO_DEFAULT_PATH REQUIRED)

    set(MOBILE_SOURCES
        src/main_android.cpp
        src/core/AsirEngine.cpp
        src/core/LocalAsirEngine.cpp
        src/core/CommandHistory.cpp
        src/mobile/MobileAsirBridge.cpp
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
            src/mobile/qml/components/ToolbarButton.qml
            src/mobile/qml/components/DisplayModeSwitch.qml
        RESOURCES
            src/resources/katex/katex.min.js
            src/resources/katex/katex.min.css
            src/resources/math_render.html
    )

    target_include_directories(asirgui PRIVATE
        src
        ${ANDROID_LIBS_DIR}/../../../OpenXM/include
    )

    target_link_libraries(asirgui PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::WebView
        ${LIBASIR}
        ${LIBGC}
        ${LIBGMP}
        ${LIBMPFR}
        ${LIBMPC}
        log     # Android ログ
    )

    # Android パッケージ設定
    set_target_properties(asirgui PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/android"
        QT_ANDROID_MIN_SDK_VERSION 29
        QT_ANDROID_TARGET_SDK_VERSION 34
    )
endif()
```

## 8. AndroidManifest.xml

```xml
<?xml version="1.0"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="org.openxm.asirgui"
    android:versionCode="1"
    android:versionName="1.0">

    <uses-sdk android:minSdkVersion="29" android:targetSdkVersion="34"/>

    <uses-permission android:name="android.permission.INTERNET"/>
    <uses-permission android:name="android.permission.READ_EXTERNAL_STORAGE"/>

    <application
        android:label="AsirGUI"
        android:icon="@drawable/ic_launcher"
        android:hardwareAccelerated="true"
        android:theme="@style/Theme.MaterialComponents.DayNight">

        <activity
            android:name="org.qtproject.qt.android.bindings.QtActivity"
            android:configChanges="orientation|screenSize|screenLayout|uiMode"
            android:exported="true"
            android:launchMode="singleTop"
            android:windowSoftInputMode="adjustResize">

            <intent-filter>
                <action android:name="android.intent.action.MAIN"/>
                <category android:name="android.intent.category.LAUNCHER"/>
            </intent-filter>

            <!-- .rr ファイルの関連付け -->
            <intent-filter>
                <action android:name="android.intent.action.VIEW"/>
                <category android:name="android.intent.category.DEFAULT"/>
                <data android:mimeType="*/*"
                      android:pathPattern=".*\\.rr"/>
            </intent-filter>

            <meta-data android:name="android.app.lib_name"
                       android:value="asirgui"/>
        </activity>
    </application>
</manifest>
```

## 9. ビルド手順

```bash
# Qt for Android のツールチェインで CMake ビルド
cd AsirGUI
mkdir build-android && cd build-android

# arm64-v8a 向け
$QT_ANDROID/bin/qt-cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DANDROID_ABI=arm64-v8a \
    -DANDROID_PLATFORM=android-29

cmake --build . --parallel

# APK 生成
cmake --build . --target apk

# インストール (USBデバッグ接続時)
adb install build/android-build/build/outputs/apk/release/asirgui-release.apk
```

## 10. デスクトップ版との UI 差分

| 機能 | デスクトップ (Qt Widgets) | Android (Qt Quick) |
|------|--------------------------|---------------------|
| 入力方式 | キーボード + Emacs風バインド | ソフトキーボード + 記号パレット |
| コマンド履歴 | Ctrl+P/N / ↑↓ | スワイプ / サイドドロワー |
| 出力表示 | QSplitter で分割 | タブ切替 or 縦スクロール |
| メニュー | メニューバー | ハンバーガーメニュー / ツールバー |
| フォント設定 | ダイアログ | 設定画面 (Slider) |
| ファイル操作 | QFileDialog | Android Storage Access Framework |
| テーマ | OS依存 | Material Design (Dark/Light) |

## 11. 既知の技術的課題

| 課題 | 対策 |
|------|------|
| libasir.a の Android クロスコンパイル | GMP/MPFR/GC が Android NDK 対応済み。asir 本体の configure 修正が必要な可能性 |
| Boehm GC と Android のスレッドモデル | `GC_INIT()` を JNI_OnLoad() で呼ぶ。`GC_allow_register_threads()` 設定 |
| Qt WebView の制約 | WebEngineView ほど高機能でない。KaTeX は動作するが、複雑な JS は要検証 |
| APK サイズ | libasir.a + GMP + 依存で数MB増。KaTeX は ~1MB。合計 30-50MB 想定 |
| 64bit 制限 | Google Play は arm64-v8a 必須。x86_64 はエミュレータ用 |
| ソフトキーボードと入力領域の干渉 | `windowSoftInputMode="adjustResize"` で対応 |
| names.rr と asir-contrib のパス | APK assets にバンドルし、起動時に内部ストレージに展開 |
| メモリ制限 | Android のプロセスメモリ制限。大規模計算時は警告表示 |

## 12. 配布方式

### Google Play Store

```
1. リリースビルドの署名
   keytool -genkey -v -keystore asirgui.keystore \
       -alias asirgui -keyalg RSA -keysize 2048

2. AAB (Android App Bundle) 生成
   cmake --build . --target aab

3. Google Play Console にアップロード
```

### APK 直接配布

GitHub Releases などで APK を配布。
「提供元不明のアプリ」の許可が必要。

## 13. 将来の拡張

- **タブレットモード**: 画面サイズに応じた2カラムレイアウト
- **Stylus 対応**: 手書き数式入力 (将来的に認識エンジン連携)
- **共有機能**: 計算結果を画像/LaTeX/テキストとして他アプリに共有
- **ウィジェット**: ホーム画面に計算ウィジェット配置
