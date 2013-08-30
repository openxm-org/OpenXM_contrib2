// asir32gui.cpp : アプリケーション用クラスの機能定義を行います。
//

#include "stdafx.h"
#include "asir32gui.h"

#include "MainFrm.h"
#include "asir32guiDoc.h"
#include "Asir32guiView.h"
#include "FatalDialog.h"
#include <winnls.h>
#include <string.h>
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" {
	extern HANDLE hNotify;
	extern HANDLE hNotify_Ack;
	extern int asirgui_kind;
	BOOL Init_IO(char *);
	BOOL get_rootdir(char *,int,char *);
	void read_and_insert();
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiApp

BEGIN_MESSAGE_MAP(CAsir32guiApp, CWinApp)
	//{{AFX_MSG_MAP(CAsir32guiApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// 標準のファイル基本ドキュメント コマンド
	// 標準の印刷セットアップ コマンド
//	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiApp クラスの構築

CAsir32guiApp::CAsir32guiApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
	char errmsg[BUFSIZ];

	if ( Init_IO(errmsg) == FALSE ) {
		CFatalDialog fatalDlg;
		fatalDlg.m_errmsg = errmsg;
		fatalDlg.DoModal();
		exit(0);
	}
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の CAsir32guiApp オブジェクト

CAsir32guiApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CAsir32guiApp クラスの初期化

BOOL CAsir32guiApp::InitInstance()
{
	// 標準的な初期化処理
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さく
	// したければ以下の特定の初期化ルーチンの中から不必要なもの
	// を削除してください。

    char lang[BUFSIZ];
    char dll[BUFSIZ];
    char root[BUFSIZ];
    char errmsg[BUFSIZ];
    HINSTANCE hRes;
    get_rootdir(root,sizeof(root),errmsg);
    if(GetLocaleInfo(GetUserDefaultLCID(), LOCALE_SISO639LANGNAME, lang, BUFSIZ)) {
        sprintf(dll, "%s\\bin\\%s.dll", root, lang);
        hRes = LoadLibrary(dll);
        if(hRes) {
            AfxSetResourceHandle(hRes);
        }
    }

#ifdef _AFXDLL
	Enable3dControls();			// 共有 DLL の中で MFC を使用する場合にはここをコールしてください。 
#else
	Enable3dControlsStatic();	// MFC と静的にリンクしている場合にはここをコールしてください。
#endif

	LoadStdProfileSettings();  // 標準の INI ファイルのオプションをローﾄﾞします (MRU を含む)
	// アプリケーション用のドキュメント テンプレートを登録します。ドキュメント テンプレート
	// はドキュメント、フレーム ウィンドウとビューを結合するために機能します。

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		asirgui_kind==ASIRGUI_MAIN?IDR_MAINFRAME:IDR_DEBUGFRAME,
		RUNTIME_CLASS(CAsir32guiDoc),
		RUNTIME_CLASS(CMainFrame),       // メイン SDI フレーム ウィンドウ
		RUNTIME_CLASS(CAsir32guiView));
	AddDocTemplate(pDocTemplate);

	// DDE、file open など標準のシェル コマンドのコマンドラインを解析します。
	CCommandLineInfo cmdInfo;
//	ParseCommandLine(cmdInfo);

	// コマンドラインでディスパッチ コマンドを指定します。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

  	// ClassWizard は仮想関数を生成しオーバーライドします。
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// メッセージ ハンドラはありません。
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// メッセージ ハンドラがありません。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ダイアログを実行するためのアプリケーション コマンド
void CAsir32guiApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAsir32guiApp コマンド

BOOL CAsir32guiApp::OnIdle(LONG lCount) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	
	if ( CWinApp::OnIdle(lCount) )
		return TRUE;
	while ( 1 ) {
		int retcode = ::MsgWaitForMultipleObjects(1,&hNotify,FALSE,-1,QS_ALLINPUT);
		retcode -= WAIT_OBJECT_0;
		if ( retcode == 0 ) {
			ResetEvent(hNotify);
			SetEvent(hNotify_Ack);
			read_and_insert();
		} else
			break;
	}
	return TRUE;
}
