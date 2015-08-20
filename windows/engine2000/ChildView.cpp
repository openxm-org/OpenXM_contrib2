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
#include "if_canvas.h"

struct pa {
	int length;
	POINT *pos;
};

typedef struct RealVect {
	int len;
	int body[1];
} RealVect;

#define EXP10(a) pow(10.0,a)

extern struct canvas *current_can;
extern POINT start_point,end_point;

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
	if ( canvas_is_interactive(can) ) {
		canvas_bitblt(can,dc.m_hDC,0,0);
	} else {
		if ( !canvas_wide(can) ) {
			canvas_bitblt(can,dc.m_hDC,0,0);
			pline(0,can,dc.m_hDC);
		} else
			draw_wideframe(can,dc.m_hDC);
	}
	BringWindowToTop();
}

void CChildView::OnPrint(CDC &dc) 
{
	DOCINFO docinfo;
//	NODE n;
	int width,height,ratio,x,y,step;
	int ch,cw,len,i;

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

	cw = canvas_width(can);
	ch = canvas_height(can);

	if ( canvas_is_interactive(can) ) {

		// We want to associate a rectangle of a fixed size to
		// one bitmap pixel
		// if cw/ch > width/height
		// then match the widths, else match the height
		if ( cw*height > ch*width )
			ratio = width/cw;
		else
			ratio = height/ch;

		// set the logical src bitmap size 
		dc.SetWindowExt(cw*ratio,ch*ratio);

		// set the viewport size and the origine in printer pixel
		dc.SetViewportOrg(width/18,height/18);
		dc.SetViewportExt(width,height);

		step = (ratio+4)/5;
		RealVect **p = (RealVect **)canvas_history(can, &len);
		for(i=0; i<len; i++) {
			RealVect *rv = p[i];
			if ( rv->len == 2 ) {
//				dc.SetPixel(rv->body[0],rv->body[1],0);
				x = rv->body[0]*ratio;
				y = rv->body[1]*ratio;
//				dc.FillRect(CRect(x,y,x+1,y+1),&brush);
//				dc.FillRect(CRect(x-1,y-1,x+1,y+1),&brush);
				dc.FillRect(CRect(x,y,x+step,y+step),&brush);
			} else if ( rv->len == 4 ) {
				dc.MoveTo(rv->body[0]*ratio,rv->body[1]*ratio);
				dc.LineTo(rv->body[2]*ratio,rv->body[3]*ratio);
			}
		}
	} else {
		// set the logical src bitmap size 
		dc.SetWindowExt(cw,ch);

		// set the viewport size and the origine in printer pixel
		dc.SetViewportOrg(width/18,height/18);
		dc.SetViewportExt(width,height);

		if ( canvas_is_plot(can) )
			dc.Polyline(canvas_pa(can)->pos,canvas_pa(can)->length);
		else
			::StretchBlt(dc.m_hDC,0,0,cw,ch,canvas_pix(can),0,0,cw,ch,SRCCOPY);
		PrintAxis(dc);
	}
	dc.EndPage();
	dc.EndDoc();
}

void CChildView::PrintAxis(CDC &dc)
{
#define D 5
#define DEFAULTWIDTH 400
#define DEFAULTHEIGHT 400

	double w,w1,e,n;
	int x0,y0,x,y,xadj,yadj;
	char buf[BUFSIZ];
	double cxmin,cxmax,cymin,cymax;
	int cw,ch;

	cxmin = canvas_xmin(can);
	cxmax = canvas_xmax(can);
	cymin = canvas_ymin(can);
	cymax = canvas_ymax(can);
	cw = canvas_width(can);
	ch = canvas_height(can);

	/* XXX : should be cleaned up */
	if ( canvas_noaxis(can) || (canvas_is_plot(can) && !canvas_pa(can)) 
		 || canvas_is_interactive(can) )
		return;

	xadj = yadj = 0;
	if ( (cxmin < 0) && (cxmax > 0) ) {
		x0 = (int)((cw)*(-cxmin/(cxmax-cxmin)));
		dc.MoveTo(x0,0); dc.LineTo(x0,ch);
	} else if ( cxmin >= 0 )
		x0 = 0;
	else
		x0 = cw-D;
	if ( (cymin < 0) && (cymax > 0) ) {
		y0 = (int)((ch)*(cymax/(cymax-cymin)));
		dc.MoveTo(0,y0); dc.LineTo(cw,y0);
	} else if ( cymin >= 0 )
		y0 = ch;
	else
		y0 = D;
	w = cxmax-cxmin; 
	w1 = w * DEFAULTWIDTH/cw;
	e = adjust_scale(EXP10(floor(log10(w1))),w1);
	for ( n = ceil(cxmin/e); n*e<= cxmax; n++ ) {
		x = (int)(cw*(n*e-cxmin)/w);
		dc.MoveTo(x,y0); dc.LineTo(x,y0-D);
		sprintf(buf,"%g",n*e);
		dc.TextOut(x+2,y0+2,buf,strlen(buf));
	}
	w = cymax-cymin;
	w1 = w * DEFAULTHEIGHT/ch;
	e = adjust_scale(EXP10(floor(log10(w1))),w1);
	for ( n = ceil(cymin/e); n*e<= cymax; n++ ) {
		y = (int)(ch*(1-(n*e-cymin)/w));
		dc.MoveTo(x0,y); dc.LineTo(x0+D,y);
		sprintf(buf,"%g",n*e);
		if ( cxmax <= 0 ) {
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
	canvas_toggle_noaxis(can);
	RedrawWindow();
}

void CChildView::OnUpdateOptNoaxis(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(canvas_noaxis(can));	
}

void CChildView::OnOptPrecise() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	canvas_toggle_precise(can);
}

void CChildView::OnUpdateOptPrecise(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(canvas_precise(can));	
}

void CChildView::OnOptWide() 
{
	// TODO: この位置にコマンド ハンドラ用のコードを追加してください
	canvas_toggle_wide(can);
	RedrawWindow();
}

void CChildView::OnUpdateOptWide(CCmdUI* pCmdUI) 
{
	// TODO: この位置に command update UI ハンドラ用のコードを追加してください
	pCmdUI->SetCheck(canvas_wide(can));	
}
