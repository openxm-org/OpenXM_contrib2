// ChildView.cpp : CChildView クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "ox_plot.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <math.h>

extern "C" {
#include "ca.h"
#include "ifplot.h"

#if defined(sun) && !defined(__svr4__)
#define EXP10(a) exp10(a)
#else
#define EXP10(a) pow(10.0,a)
#endif

	HANDLE hResizeNotify,hResizeNotify_Ack;
	void destroy_canvas(struct canvas *);
	void pline(int *,struct canvas *, HDC);
	void draw_wideframe(struct canvas *, HDC);
	double adjust_scale(double,double);
}
/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
	can = 0;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_OPT_NOAXIS, OnOptNoaxis)
	ON_UPDATE_COMMAND_UI(ID_OPT_NOAXIS, OnUpdateOptNoaxis)
	ON_COMMAND(ID_OPT_PRECISE, OnOptPrecise)
	ON_UPDATE_COMMAND_UI(ID_OPT_PRECISE, OnUpdateOptPrecise)
	ON_COMMAND(ID_OPT_WIDE, OnOptWide)
	ON_UPDATE_COMMAND_UI(ID_OPT_WIDE, OnUpdateOptWide)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView メッセージ ハンドラ

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);
	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト
	
	// TODO: メッセージ ハンドラのコードをここに追加してください。
	
	// 描画のために CWnd::OnPaint を呼び出してはいけません。
	if ( can->mode == MODE_INTERACTIVE ) {
		::BitBlt(dc.m_hDC,0,0,can->width,can->height,can->pix,0,0,SRCCOPY);
	} else {
		if ( !can->wide ) {
			::BitBlt(dc.m_hDC,0,0,can->width,can->height,can->pix,0,0,SRCCOPY);
			pline(0,can,dc.m_hDC);
		} else
			draw_wideframe(can,dc.m_hDC);
	}
	BringWindowToTop();
}

void CChildView::OnPrint(CDC &dc) 
{
	DOCINFO docinfo;
	NODE n;
	int width,height,ratio,x,y;

	memset(&docinfo,0,sizeof(DOCINFO));
	docinfo.cbSize = sizeof(DOCINFO);
	docinfo.lpszDocName = "test";
	docinfo.lpszOutput = 0;

	// black pen
	CPen pen(PS_SOLID,0,(COLORREF)0);
	dc.SelectObject(&pen);

	// black brush
	CBrush brush((COLORREF)0);
	dc.SelectObject(&brush);

	// setup
	// isotropic mode 
	dc.SetMapMode(MM_ISOTROPIC);

	// the page size in device pixel
	width = dc.GetDeviceCaps(HORZRES)*9/10;
	height = dc.GetDeviceCaps(VERTRES)*9/10;

	dc.StartDoc(&docinfo);
	dc.StartPage();

	if ( can->mode == MODE_INTERACTIVE ) {
		// We want to associate precisely one printer pixel to
		// one bitmap pixel
		// if can->width/can->height > width/height
		// then match the widths, else match the height
		if ( can->width*height > can->height*width )
			ratio = width/can->width;
		else
			ratio = height/can->height;

		// set the logical src bitmap size 
		dc.SetWindowExt(can->width*ratio,can->height*ratio);

		// set the viewport size and the origine in printer pixel
		dc.SetViewportOrg(width/18,height/18);
		dc.SetViewportExt(width,height);

		for ( n = can->history; n; n = NEXT(n) ) {
			RealVect *rv = (RealVect *)n->body;
			if ( rv->len == 2 ) {
//				dc.SetPixel(rv->body[0],rv->body[1],0);
				x = rv->body[0]*ratio;
				y = rv->body[1]*ratio;
//				dc.FillRect(CRect(x,y,x+1,y+1),&brush);
				dc.FillRect(CRect(x-1,y-1,x+1,y+1),&brush);
			} else if ( rv->len == 4 ) {
				dc.MoveTo(rv->body[0]*ratio,rv->body[1]*ratio);
				dc.LineTo(rv->body[2]*ratio,rv->body[3]*ratio);
			}
		}
	} else {
		// set the logical src bitmap size 
		dc.SetWindowExt(can->width,can->height);

		// set the viewport size and the origine in printer pixel
		dc.SetViewportOrg(width/18,height/18);
		dc.SetViewportExt(width,height);

		if ( can->mode == MODE_PLOT )
			dc.Polyline(can->pa[0].pos,can->pa[0].length);
		else
		::StretchBlt(dc.m_hDC,0,0,can->width,can->height,can->pix,
			0,0,can->width,can->height,SRCCOPY);
		PrintAxis(dc);
	}
	dc.EndPage();
	dc.EndDoc();
}

void CChildView::PrintAxis(CDC &dc)
{
#define D 5

	double w,w1,k,e,n;
	int x0,y0,x,y,xadj,yadj;
	char buf[BUFSIZ];

	/* XXX : should be cleaned up */
	if ( can->noaxis || (can->mode == MODE_PLOT && !can->pa) )
		return;
	if ( can->mode == MODE_INTERACTIVE )
		return;

	xadj = yadj = 0;
	if ( (can->xmin < 0) && (can->xmax > 0) ) {
		x0 = (int)((can->width)*(-can->xmin/(can->xmax-can->xmin)));
		dc.MoveTo(x0,0); dc.LineTo(x0,can->height);
	} else if ( can->xmin >= 0 )
		x0 = 0;
	else
		x0 = can->width-D;
	if ( (can->ymin < 0) && (can->ymax > 0) ) {
		y0 = (int)((can->height)*(can->ymax/(can->ymax-can->ymin)));
		dc.MoveTo(0,y0); dc.LineTo(can->width,y0);
	} else if ( can->ymin >= 0 )
		y0 = can->height;
	else
		y0 = D;
	w = can->xmax-can->xmin; 
	w1 = w * DEFAULTWIDTH/can->width;
	e = adjust_scale(EXP10(floor(log10(w1))),w1);
	for ( n = ceil(can->xmin/e); n*e<= can->xmax; n++ ) {
		x = (int)can->width*(n*e-can->xmin)/w;
		dc.MoveTo(x,y0); dc.LineTo(x,y0-D);
		sprintf(buf,"%g",n*e);
		dc.TextOut(x+2,y0+2,buf,strlen(buf));
	}
	w = can->ymax-can->ymin;
	w1 = w * DEFAULTHEIGHT/can->height;
	e = adjust_scale(EXP10(floor(log10(w1))),w1);
	for ( n = ceil(can->ymin/e); n*e<= can->ymax; n++ ) {
		y = (int)can->height*(1-(n*e-can->ymin)/w);
		dc.MoveTo(x0,y); dc.LineTo(x0+D,y);
		sprintf(buf,"%g",n*e);
		if ( can->xmax <= 0 ) {
			xadj = dc.GetOutputTextExtent(buf,strlen(buf)).cx;
		}
		dc.TextOut(x0+2-xadj,y+2,buf,strlen(buf));
	}
}

static CPoint start,prev;

void CChildView::OnLButtonDown(UINT nFlags, CPoint point) {
	start = point;
	prev = point;
	SetCapture();
	CWnd::OnLButtonDown(nFlags,point);	
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point) {
	if ( nFlags & MK_LBUTTON ) {
		CDC *pDC = GetDC();
		CRect prevrect(start,prev),rect(start,point);
		CSize size(1,1);
	
		pDC->DrawDragRect(&rect,size,&prevrect,size);
		ReleaseDC(pDC);
		prev = point;
	}
	CWnd::OnMouseMove(nFlags,point);	
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point) {
	CDC *pDC = GetDC();
	CRect rect(start,point);
	CSize size(1,1);

	pDC->DrawDragRect(&rect,size,0,size);
	ReleaseDC(pDC);
	ReleaseCapture();
	start_point = start; end_point = point;
	current_can = can;
	SetEvent(hResizeNotify);
	WaitForSingleObject(hResizeNotify_Ack,(DWORD)-1);

	CWnd::OnLButtonUp(nFlags,point);	
}

void CChildView::DestroyCanvas() {
	destroy_canvas(can);
}

void CChildView::OnOptNoaxis() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	can->noaxis = !can->noaxis;	
	RedrawWindow();
}

void CChildView::OnUpdateOptNoaxis(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(can->noaxis);	
}

void CChildView::OnOptPrecise() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	can->precise = !can->precise;	
}

void CChildView::OnUpdateOptPrecise(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(can->precise);	
}

void CChildView::OnOptWide() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	can->wide = !can->wide;	
	RedrawWindow();
}

void CChildView::OnUpdateOptWide(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(can->wide);	
}
