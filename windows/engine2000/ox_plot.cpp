// ox_plot.cpp : アプリケーション用クラスの機能定義を行います。
//

#include "stdafx.h"
#include "ox_plot.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" {
#include "ca.h"
#include "ifplot.h"

	extern HANDLE hStreamNotify;
	extern HANDLE hStreamNotify_Ack;
	extern DWORD MainThread;
	void Init_IO();
}
/////////////////////////////////////////////////////////////////////////////
// COx_plotApp

BEGIN_MESSAGE_MAP(COx_plotApp, CWinApp)
	//{{AFX_MSG_MAP(COx_plotApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COx_plotApp クラスの構築

COx_plotApp::COx_plotApp()
{
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
	Init_IO();
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の COx_plotApp オブジェクト

COx_plotApp theApp;

/////////////////////////////////////////////////////////////////////////////
// COx_plotApp クラスの初期化

BOOL COx_plotApp::InitInstance()
{
	AfxEnableControlContainer();

	// 標準的な初期化処理
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さく
	// したければ以下の特定の初期化ルーチンの中から不必要なもの
	// を削除してください。

#ifdef _AFXDLL
	Enable3dControls();		// 共有 DLL の中で MFC を使用する場合にはここを呼び出してください。
#else
	Enable3dControlsStatic();	// MFC と静的にリンクしている場合にはここを呼び出してください。
#endif

	// 設定が保存される下のレジストリ キーを変更します。
	// TODO: この文字列を、会社名または所属など適切なものに
	// 変更してください。
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	// メイン ウインドウを作成するとき、このコードは新しいフレーム ウインドウ オブジェクトを作成し、
	// それをアプリケーションのメイン ウインドウにセットします

	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// フレームをリソースからロードして作成します

	pFrame->m_pWindowName = "dummy window";
	pFrame->m_cansize.cx = 1;
	pFrame->m_cansize.cy = 1;
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, 
		NULL);
	MainThread = m_nThreadID;

	// メイン ウィンドウが初期化されたので、表示と更新を行います。
//	pFrame->ShowWindow(SW_SHOW);
//	pFrame->UpdateWindow();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// COx_plotApp message handlers





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

	// ClassWizard 仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
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
		// メッセージ ハンドラはありません。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ダイアログを実行するためのアプリケーション コマンド
void COx_plotApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// COx_plotApp メッセージ ハンドラ

BOOL COx_plotApp::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	CMainFrame *pFrame;
	struct canvas *can;

	switch ( pMsg->message ) {
	  case WM_APP: // copy to canvas 
	    can = canvas[pMsg->wParam];
	    if ( !can->window ) {
	      pFrame = new CMainFrame;
	      /* XXX */
	      pFrame->m_pWindowName = 
		(char *)malloc(MAX(strlen(can->wname),strlen("ox_plot"))+10);
	      sprintf(pFrame->m_pWindowName,"%s : %d",
		strlen(can->wname)?can->wname:"ox_plot",can->index);

	      pFrame->m_cansize.cx = can->width;
	      pFrame->m_cansize.cy = can->height;
	      pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);
	      can->window = (void *)pFrame;
	      pFrame->m_wndView.can = can;
	      pFrame->ShowWindow(SW_SHOW);
	      pFrame->UpdateWindow();
	      pFrame->BringWindowToTop();
	      can->hwnd = pFrame->m_wndView.m_hWnd;
	    } else
	      pFrame = (CMainFrame *)can->window;
	    pFrame->RedrawWindow();
	    break;
	  case WM_APP+1: // popup
	    can = canvas[pMsg->wParam];
	    pFrame = (CMainFrame *)can->window;
	    pFrame->ShowWindow(SW_SHOW);
	    break;
	  case WM_APP+2: // popdown
	    can = canvas[pMsg->wParam];
	    pFrame = (CMainFrame *)can->window;
	    pFrame->ShowWindow(SW_HIDE);
	    closed_canvas[pMsg->wParam] = can;
	    canvas[pMsg->wParam] = 0;
	    break;
	  default:
	    break;
	}
	return CWinApp::PreTranslateMessage(pMsg);
}

void COx_plotApp::OnAppExit() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
}

BOOL COx_plotApp::OnIdle(LONG lCount) 
{
	// TODO: この位置に固有の処理を追加するか、または基本クラスを呼び出してください
	
	return CWinApp::OnIdle(lCount);
}

void COx_plotApp::OnFilePrintSetup()
{
	// XXX: the following function is protected.
	CWinApp::OnFilePrintSetup();
}
