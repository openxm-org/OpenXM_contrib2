// MainFrm.cpp : CMainFrame クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "asir32gui.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_HEAP,OnUpdateHeapSize)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ステータス ライン インジケータ
	ID_INDICATOR_KANA,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_HEAP,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame クラスの構築/消滅

CMainFrame *theFrame;

CMainFrame::CMainFrame()
{
	// TODO: この位置にメンバの初期化処理コードを追加してください。
	theFrame = this;	
}

CMainFrame::~CMainFrame()
{
}

extern "C" {
	extern int asirgui_kind;
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if ( asirgui_kind == ASIRGUI_MAIN ) {
		if (!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			TRACE0("Failed to create toolbar\n");
			return -1;      // 作成に失敗
		}
	
		if (!m_wndStatusBar.Create(this) ||
			!m_wndStatusBar.SetIndicators(indicators,
		  	sizeof(indicators)/sizeof(UINT)))
		{
			TRACE0("Failed to create status bar\n");
			return -1;      // 作成に失敗
		}
	
		// TODO: もしツール チップスが必要ない場合、ここを削除してください。
		m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
			CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	
		// TODO: ツール バーをドッキング可能にしない場合は以下の３行を削除
		//       してください。
		m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
		EnableDocking(CBRS_ALIGN_ANY);
		DockControlBar(&m_wndToolBar);
	}
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: この位置で CREATESTRUCT cs を修正して、Window クラスやスタイルを
	//       修正してください。
	if ( asirgui_kind == ASIRGUI_MAIN ) {
	   	cs.cx = ::GetSystemMetrics(SM_CXSCREEN)*2 / 3; 
		cs.cy = ::GetSystemMetrics(SM_CYSCREEN)*2 / 3; 
	} else {
	   	cs.cx = ::GetSystemMetrics(SM_CXSCREEN)*2 / 5; 
		cs.cy = ::GetSystemMetrics(SM_CYSCREEN)*1 / 5; 
	}

	return CFrameWnd::PreCreateWindow(cs);
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

extern "C" {
	extern int AsirHeapSize;

void show_window(int on)
{
	if ( on )
		theFrame->ShowWindow(SW_SHOW);
	else
		theFrame->ShowWindow(SW_HIDE);
}
}

void CMainFrame::OnUpdateHeapSize(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
	CString strHeap;
	strHeap.Format("HeapSize %dBytes",AsirHeapSize);
	pCmdUI->SetText(strHeap);
}
