# AsirGUI Windows 実装計画

## 1. 概要

Windows版は、GUIを Windows ネイティブ (Qt6.8 Widgets) で構築し、計算エンジンは WSL2 上の
Debian で動作する ox_asir を利用する。通信には OpenXM の OX プロトコル (TCP/IP) を使用する。

- **GUI**: Qt 6.8 Widgets (Windows ネイティブビルド)
- **計算エンジン**: WSL2 Debian 上の ox_asir (libasir.a リンク済みバイナリ)
- **通信方式**: OX プロトコル over TCP/IP (localhost)、リバースモード
- **数式描画**: KaTeX (QWebEngineView)
- **LaTeX変換**: `print_tex_form()` (names.rr) をエンジン側で実行

## 2. アーキテクチャ

```
┌──── Windows ──────────────────────────────────────┐
│                                                    │
│  ┌──────────────────────────────────────────────┐ │
│  │          AsirGUI (Qt6.8 Widgets)              │ │
│  │                                               │ │
│  │  ┌──────────┐  ┌──────────────────────────┐  │ │
│  │  │ 入力領域  │  │ 出力領域                  │  │ │
│  │  │          │  │  テキスト / KaTeX 数式     │  │ │
│  │  └──────────┘  └──────────────────────────┘  │ │
│  │                                               │ │
│  │  ┌──────────────────────────────────────────┐ │ │
│  │  │ OxClient (OXプロトコル クライアント)       │ │ │
│  │  │  - TCP/IP ソケット (QTcpSocket)           │ │ │
│  │  │  - CMO シリアライズ/デシリアライズ         │ │ │
│  │  │  - リバース接続モード                      │ │ │
│  │  └────────────────┬─────────────────────────┘ │ │
│  └───────────────────│──────────────────────────┘ │
│                      │ TCP/IP (localhost)          │
│  ┌───────────────────│──────────────────────────┐ │
│  │      WSL2 (Debian)│                           │ │
│  │                   ▼                           │ │
│  │  ┌──────────────────────────────────────────┐ │ │
│  │  │ ox_asir (OXサーバーモード)                │ │ │
│  │  │  - libasir.a リンク済み                   │ │ │
│  │  │  - names.rr ロード済み                    │ │ │
│  │  │  - OXプロトコルで応答                     │ │ │
│  │  └──────────────────────────────────────────┘ │ │
│  └──────────────────────────────────────────────┘ │
└────────────────────────────────────────────────────┘
```

### 通信フロー (リバースモード)

```
1. GUI (Windows) が 2つの TCP ポートを listen
   - Control Port (制御用)
   - Server Port  (データ用)
2. GUI が wsl.exe 経由で ox_asir を起動
   wsl.exe ox_asir <host> 0 <control_port> <server_port>
3. ox_asir が GUI の listen ポートに connect (リバース接続)
4. 双方向通信開始
   GUI → ox_asir: OX_DATA (CMO) + OX_COMMAND (SM_executeStringByLocalParser)
   ox_asir → GUI: OX_DATA (結果 CMO)
```

## 3. ビルド環境

### Windows側

```powershell
# Qt 6.8 for Windows (MSVC)
# Qt Online Installer から以下をインストール:
#   - Qt 6.8.x / MSVC 2022 64-bit
#   - Qt WebEngine
#   - CMake, Ninja

# Visual Studio 2022 (Build Tools)
# C++ デスクトップ開発ワークロード

# 環境変数
$env:Qt6_DIR = "C:\Qt\6.8.0\msvc2022_64"
$env:CMAKE_PREFIX_PATH = $env:Qt6_DIR
```

### WSL2側 (Debian)

```bash
# OpenXM がビルド済みであること (既存環境)
# 実際のパスは環境に合わせて設定する
export OpenXM_HOME=/path/to/OpenXM

# ox_asir が利用可能であること
ls -la $OpenXM_HOME/bin/ox_asir
# ox_asir -> asir (シンボリックリンク)

# names.rr が利用可能であること
ls $OpenXM_HOME/src/asir-contrib/packages/src/names.rr
```

## 4. プロジェクト構成

```
AsirGUI/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── core/
│   │   ├── AsirEngine.h              # エンジン抽象クラス (共通)
│   │   ├── AsirEngine.cpp
│   │   ├── OxClient.h                # OXプロトコル クライアント
│   │   ├── OxClient.cpp
│   │   ├── OxProtocol.h              # OXメッセージ定義・シリアライズ
│   │   ├── OxProtocol.cpp
│   │   ├── CmoSerializer.h           # CMO バイナリフォーマット処理
│   │   ├── CmoSerializer.cpp
│   │   ├── OxAsirEngine.h            # OXプロトコル経由のエンジン実装
│   │   ├── OxAsirEngine.cpp
│   │   ├── CommandHistory.h
│   │   └── CommandHistory.cpp
│   ├── desktop/
│   │   ├── MainWindow.h / .cpp
│   │   ├── InputWidget.h / .cpp      # Emacs風キーバインド
│   │   ├── OutputWidget.h / .cpp
│   │   ├── MathView.h / .cpp         # KaTeX (QWebEngineView)
│   │   └── SettingsDialog.h / .cpp
│   └── resources/
│       ├── katex/
│       ├── math_render.html
│       ├── icons/
│       └── resources.qrc
├── platform/
│   └── windows/
│       ├── asirgui.rc                # Windows リソース (アイコン)
│       ├── asirgui.ico
│       ├── installer.nsi             # NSIS インストーラスクリプト
│       └── wsl_launcher.bat          # WSL2 エンジン起動スクリプト
└── tests/
    ├── test_ox_protocol.cpp
    └── test_cmo_serializer.cpp
```

## 5. 実装フェーズ

### フェーズ1: OXプロトコル クライアント実装

OXプロトコルは OpenXM の ox_toolkit に実装済み。Qt の QTcpSocket で再実装する。

#### 1.1 OXプロトコル メッセージ定義

```cpp
// core/OxProtocol.h
#pragma once
#include <QByteArray>
#include <QtEndian>

// OX メッセージタグ
namespace OxTag {
    constexpr int OX_COMMAND = 513;
    constexpr int OX_DATA    = 514;
    constexpr int OX_SYNC_BALL = 515;
}

// SM コマンド
namespace SmCommand {
    constexpr int SM_popCMO = 262;
    constexpr int SM_popString = 263;
    constexpr int SM_mathcap = 264;
    constexpr int SM_executeStringByLocalParser = 268;
    constexpr int SM_executeFunction = 269;
    constexpr int SM_shutdown = 272;
}

// CMO タグ
namespace CmoTag {
    constexpr int CMO_NULL    = 1;
    constexpr int CMO_INT32   = 2;
    constexpr int CMO_STRING  = 4;
    constexpr int CMO_LIST    = 17;
    constexpr int CMO_ZZ      = 20;
}

class OxMessage {
public:
    static QByteArray makeOxCommand(int smCommand);
    static QByteArray makeOxData(const QByteArray &cmo);
    static QByteArray makeCmoString(const QString &str);
    static QByteArray makeCmoInt32(int value);
};
```

#### 1.2 OxClient (TCP ソケット通信)

```cpp
// core/OxClient.h
#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class OxClient : public QObject {
    Q_OBJECT
public:
    // リバースモード: GUI側が listen し、ox_asir が connect してくる
    bool startListening(quint16 controlPort, quint16 serverPort);
    void sendExecuteString(const QString &command);
    void sendPopString();
    void sendPopCMO();
    void sendShutdown();

signals:
    void connected();
    void dataReceived(const QByteArray &cmo);
    void stringReceived(const QString &result);
    void disconnected();
    void errorOccurred(const QString &error);

private slots:
    void onControlConnected();
    void onServerConnected();
    void onServerReadyRead();

private:
    QTcpServer *m_controlServer = nullptr;
    QTcpServer *m_dataServer = nullptr;
    QTcpSocket *m_controlSocket = nullptr;
    QTcpSocket *m_dataSocket = nullptr;

    QByteArray readOxMessage();
    void writeOxMessage(QTcpSocket *socket, const QByteArray &message);
};
```

#### 1.3 CMO シリアライザ

```cpp
// core/CmoSerializer.h
#pragma once
#include <QByteArray>
#include <QString>
#include <QVariant>

class CmoSerializer {
public:
    // CMO バイナリ → Qt 型への変換
    static QVariant deserialize(const QByteArray &cmo);
    static QString cmoToString(const QByteArray &cmo);
    static int cmoToInt32(const QByteArray &cmo);

    // Qt 型 → CMO バイナリへの変換
    static QByteArray serializeString(const QString &str);
    static QByteArray serializeInt32(int value);
    static QByteArray serializeNull();

private:
    static int readInt32(const QByteArray &data, int offset);
};
```

### フェーズ2: WSL2 エンジン起動

#### 2.1 OxAsirEngine

```cpp
// core/OxAsirEngine.h
#pragma once
#include "AsirEngine.h"
#include "OxClient.h"
#include <QProcess>

class OxAsirEngine : public AsirEngine {
    Q_OBJECT
public:
    bool initialize() override;
    void shutdown() override;
    void executeString(const QString &command) override;
    bool isReady() const override;

private:
    void launchWslEngine(quint16 controlPort, quint16 serverPort);
    void loadNamesRr();  // print_tex_form の初期化

    OxClient *m_client = nullptr;
    QProcess *m_wslProcess = nullptr;
    bool m_ready = false;
    bool m_namesRrLoaded = false;
};
```

#### 2.2 WSL2 エンジン起動処理

```cpp
bool OxAsirEngine::initialize() {
    m_client = new OxClient(this);

    // 空きポートを自動選択
    quint16 controlPort = findFreePort();
    quint16 serverPort = findFreePort();

    // GUI側で listen 開始
    m_client->startListening(controlPort, serverPort);

    // WSL2 上で ox_asir を起動 (リバース接続モード)
    m_wslProcess = new QProcess(this);
    QString wslCmd = QString(
        "wsl.exe -d Debian -- bash -c '"
        "export OpenXM_HOME=/path/to/OpenXM; "  // 実際のパスに置き換える
        "cd $OpenXM_HOME; "
        "bin/ox_asir 127.0.0.1 0 %1 %2'"
    ).arg(controlPort).arg(serverPort);

    m_wslProcess->start("cmd.exe", {"/c", wslCmd});

    // ox_asir が connect してくるのを待つ
    connect(m_client, &OxClient::connected, this, [this]() {
        loadNamesRr();  // print_tex_form() を使えるようにする
        m_ready = true;
        emit engineReady();
    });

    return true;
}
```

#### 2.3 コマンド実行フロー

```cpp
void OxAsirEngine::executeString(const QString &command) {
    // 1. テキスト結果の取得
    m_client->sendExecuteString(command);
    m_client->sendPopString();

    // 2. LaTeX結果の取得
    if (m_namesRrLoaded) {
        // 元の式を再評価してprint_tex_formで変換
        // 注: 式の結果に対してprint_tex_formを呼ぶ
        QString texCmd = QString(
            "__gui_tmp = %1 print_tex_form(__gui_tmp);")
            .arg(command);
        m_client->sendExecuteString(texCmd);
        m_client->sendPopString();
    }
}
```

### フェーズ3: デスクトップUI (Linux版と共通)

Linux版と同一の Qt Widgets UI コードを使用。プラットフォーム差異は以下のみ：

| 項目 | Linux | Windows |
|------|-------|---------|
| エンジン | LocalAsirEngine (libasir.a直接) | OxAsirEngine (OXプロトコル/WSL2) |
| フォント | システム等幅フォント | MS Gothic / Consolas |
| パス区切り | `/` | `\` (QDir で吸収) |

```cpp
// main.cpp (Windows版)
int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

#ifdef Q_OS_WIN
    auto *engine = new OxAsirEngine();
#else
    auto *engine = new LocalAsirEngine();
#endif

    MainWindow window(engine);
    window.show();

    engine->initialize();

    return app.exec();
}
```

### フェーズ4: KaTeX 数式表示

Linux版と同一実装。QWebEngineView + KaTeX をバンドル。

## 6. OXプロトコル詳細

### メッセージフォーマット

```
OX メッセージ:
┌───────────┬────────────┬──────────────────┐
│ tag (4B)  │ serial (4B)│ body (可変長)     │
│ bigendian │ bigendian  │                   │
└───────────┴────────────┴──────────────────┘

OX_COMMAND (tag=513):
  body = SM command (4B, bigendian)

OX_DATA (tag=514):
  body = CMO binary data
```

### CMO バイナリフォーマット

```
CMO_STRING (tag=4):
┌───────────┬───────────┬──────────────────┐
│ tag=4 (4B)│ len (4B)  │ string data      │
└───────────┴───────────┴──────────────────┘

CMO_INT32 (tag=2):
┌───────────┬───────────┐
│ tag=2 (4B)│ value (4B)│
└───────────┴───────────┘

CMO_ZZ (tag=20):
┌───────────┬───────────┬──────────────────┐
│ tag=20(4B)│ sign+len  │ GMP limb data    │
│           │ (4B)      │                   │
└───────────┴───────────┴──────────────────┘
```

### 典型的な通信シーケンス

```
GUI → ox_asir:
  OX_DATA { CMO_STRING("fctr(x^10-1);") }
  OX_COMMAND { SM_executeStringByLocalParser }
  OX_COMMAND { SM_popString }

ox_asir → GUI:
  OX_DATA { CMO_STRING("[[1,1],[x-1,1],[x+1,1],...]") }
```

## 7. CMake ビルド設定 (Windows)

```cmake
cmake_minimum_required(VERSION 3.22)
project(AsirGUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 6.8 REQUIRED COMPONENTS
    Widgets WebEngineWidgets WebChannel Network)

set(CORE_SOURCES
    src/core/AsirEngine.cpp
    src/core/OxClient.cpp
    src/core/OxProtocol.cpp
    src/core/CmoSerializer.cpp
    src/core/OxAsirEngine.cpp
    src/core/CommandHistory.cpp
)

set(DESKTOP_SOURCES
    src/desktop/MainWindow.cpp
    src/desktop/InputWidget.cpp
    src/desktop/OutputWidget.cpp
    src/desktop/MathView.cpp
    src/desktop/SettingsDialog.cpp
)

add_executable(asirgui WIN32
    src/main.cpp
    ${CORE_SOURCES}
    ${DESKTOP_SOURCES}
    src/resources/resources.qrc
    platform/windows/asirgui.rc
)

target_include_directories(asirgui PRIVATE src)

target_link_libraries(asirgui PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::WebChannel
    Qt6::Network
    ws2_32          # Winsock (OxClient の TCP ソケット用)
)

# Windows 用: リソースファイル (アイコン)
# platform/windows/asirgui.rc に IDI_ICON1 ICON "asirgui.ico" を記述
```

## 8. ビルド手順

```powershell
# Visual Studio Developer Command Prompt で実行
cd AsirGUI
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 `
    -DCMAKE_PREFIX_PATH="C:\Qt\6.8.0\msvc2022_64"
cmake --build . --config Release

# 配布用パッケージ作成
windeployqt --release --no-translations Release\asirgui.exe
```

## 9. インストーラ (NSIS)

```nsis
; platform/windows/installer.nsi
!include "MUI2.nsh"

Name "AsirGUI"
OutFile "AsirGUI_Setup.exe"
InstallDir "$PROGRAMFILES64\AsirGUI"

Section "Main"
    SetOutPath $INSTDIR
    File /r "Release\*.*"        ; Qt DLL, プラグイン含む
    File "asirgui.exe"
    ; KaTeX リソースは qrc に埋め込み済み

    ; WSL2 前提条件チェック
    ; wsl.exe --status で確認
    ; Debian ディストリビューションの確認

    ; スタートメニュー
    CreateShortCut "$SMPROGRAMS\AsirGUI.lnk" "$INSTDIR\asirgui.exe"
SectionEnd
```

## 10. WSL2 セットアップガイド (ユーザー向け)

Windows版 AsirGUI を使用するための前提条件：

```
1. WSL2 が有効化されていること
   > wsl --install

2. Debian ディストリビューションがインストールされていること
   > wsl --install -d Debian

3. WSL2 Debian 内で OpenXM がビルド済みであること
   $ cd /home/user/OpenXM
   $ make

4. 環境変数 OpenXM_HOME が設定されていること
   $ export OpenXM_HOME=/home/user/OpenXM
   (~/.bashrc に追加推奨)
```

AsirGUI は初回起動時に WSL2 環境を自動検出し、設定ダイアログで
OpenXM_HOME のパスを指定できるようにする。

## 11. 既知の技術的課題

| 課題 | 対策 |
|------|------|
| WSL2 の localhost ポートフォワーディング | WSL2 は localhost を Windows と共有 (既定で有効) |
| ox_asir の起動遅延 | GUI にスプラッシュスクリーン / 接続待ちインジケータ表示 |
| WSL2 未インストール時のエラー | 初回起動時に wsl.exe --status で検出し、インストールガイド表示 |
| ファイアウォールによるブロック | localhost のみ使用するためブロックされにくい。問題時はガイド表示 |
| OXプロトコル実装の正確性 | ox_toolkit のソースコード (C) を参考に忠実に再実装 |
| WSL2 内のファイルパス変換 | `\\wsl$\Debian\...` ↔ `/home/...` の相互変換ユーティリティ |
| ox_asir プロセスの管理 | GUI終了時に SM_shutdown 送信。異常終了時はプロセスkill |

## 12. 将来の拡張

### OXプロトコルの活用

OXプロトコル経由のため、以下の拡張が自然に実現可能：

- **リモート計算サーバー**: WSL2 以外のリモートマシンに ox_asir を配置
- **複数エンジン接続**: 並列計算のために複数の ox_asir に接続
- **他の OX サーバー**: ox_sm1, ox_kan など他の数学ソフトウェアとの連携

### ネイティブ Windows ビルドへの移行

将来、libasir.a の Windows ネイティブビルドが利用可能になった場合、
`LocalAsirEngine` に切り替えることで WSL2 依存を解消できる。
エンジン抽象クラスにより、GUIコード変更なしで切替可能。
