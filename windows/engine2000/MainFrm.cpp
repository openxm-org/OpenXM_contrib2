// MainFrm.cpp : CMainFrame クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "ox_plot.h"

#include "MainFrm.h"

#include "gmpxx.h"
extern "C" {
#include "ca.h"
#include "ifplot.h"
}

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_SETUP, OnFilePrintSetup)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_PROGRESS,OnUpdateProgress)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケータ
	ID_INDICATOR_PROGRESS,
//	ID_INDICATOR_KANA,
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};
/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの構築/消滅

CMainFrame::CMainFrame()
{
	// TODO: この位置にメンバの初期化処理コードを追加してください。
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// フレームのクライアント領域全体を占めるビューを作成します。
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // 作成に失敗
	}

	m_nProgress = 100;

	UINT nID,nStyle;
	int cxWidth;

	m_wndStatusBar.GetPaneInfo(0,nID,nStyle,cxWidth);
	cxWidth = 0;
	m_wndStatusBar.SetPaneInfo(0,nID,nStyle,cxWidth);

	m_wndStatusBar.GetPaneInfo(1,nID,nStyle,cxWidth);
	cxWidth = 150;
	m_wndStatusBar.SetPaneInfo(1,nID,nStyle,cxWidth);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: この位置で CREATESTRUCT cs を修正して、Window クラスやスタイルを
	//       修正してください。

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.lpszName = m_pWindowName;
	cs.cx = m_cansize.cx+20; /* XXX */
	cs.cy = m_cansize.cy+80; /* XXX */
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの診断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame メッセージ ハンドラ
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// ビュー ウィンドウにフォーカスを与えます。
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// ビューに最初にコマンドを処理する機会を与えます。
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 処理されなかった場合にはデフォルトの処理を行います。
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnClose() 
{
	// TODO: この位置にメッセージ ハンドラ用のコードを追加するかまたはデフォルトの処理を呼び出してください
	m_wndView.DestroyCanvas();	
	CFrameWnd::OnClose();
}

void CMainFrame::OnAppExit() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	OnClose();	
}

void CMainFrame::OnUpdateProgress(CCmdUI *pCmdUI)
{
	int mode;
	CString strProgress;

	if ( !m_wndView.can )
		return;
	switch ( mode = m_wndView.can->mode ) {
		case MODE_IFPLOT: case MODE_CONPLOT:
			pCmdUI->Enable();
			strProgress.Format("%s%d%%",m_wndView.can->prefix,m_wndView.can->percentage);
			pCmdUI->SetText(strProgress);
			break;
		default:
			break;
	}
}

void CMainFrame::OnFilePrintSetup()
{
	theApp.OnFilePrintSetup();
}

void CMainFrame::OnFilePrint()
{
	CDC dc;
	CPrintDialog pd(TRUE);
	int ret;

	ret = theApp.DoPrintDialog(&pd);
	if ( ret == IDOK ) {
		theApp.CreatePrinterDC(dc);
		m_wndView.OnPrint(dc);
	}
}
