/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 *
 * $OpenXM: OpenXM_contrib2/windows/engine2000/ox_plot_win.c,v 1.5 2002/10/02 09:33:32 noro Exp $
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"

#ifdef ABS
#undef ABS
#define ABS(a) ((a)>0?(a):-(a))
#endif

static char *dname;
static int remotes;
static int depth,scrn;

extern jmp_buf ox_env;

static int busy;

HBRUSH ClearBrush,DrawBrush;

#define LABELWIDTH 150

int search_canvas()
{
	int i;

	for ( i = 0; i < MAXCANVAS; i++ )
		if ( !canvas[i] ) {
			canvas[i] = (struct canvas *)MALLOC(sizeof(struct canvas));
			canvas[i]->index = i; return i;
		}
}

int search_active_canvas()
{
	int i;

	for ( i = 0; i < MAXCANVAS; i++ )
		if ( canvas[i] )
			return i;
	return -1;
}

volatile DWORD MainThread;
volatile int canvas_created;
extern HANDLE hMainThreadReady,hCanvasCreated;

void create_canvas(struct canvas *can)
{
	alloc_pixmap(can);
	can->real_can = can;
	WaitForSingleObject(hMainThreadReady,(DWORD)-1);
	ResetEvent(hCanvasCreated);
	PostThreadMessage(MainThread,WM_APP,can->index,0);
	WaitForSingleObject(hCanvasCreated,(DWORD)-1);
}

void destroy_canvas(struct canvas *can)
{
	if ( can == current_can ) {
		reset_busy(can); current_can = 0;
	}
	can->window = 0;
	DeleteObject(can->pix);
	closed_canvas[can->index] = can;
	canvas[can->index] = 0;
}

void clear_pixmap(struct canvas *can)
{
	RECT rect;

	create_brushes();
	rect.left = 0; rect.top = 0;
	rect.right = can->width; rect.bottom = can->height;
	FillRect(can->pix,&rect,ClearBrush);
}

void alloc_pixmap(struct canvas *can)
{
	HDC pix;
	HBITMAP bm;
	int bpp,np;
	BITMAP bmobj;
	void *bits;

	can->pix = pix = CreateCompatibleDC(NULL);
	bpp = GetDeviceCaps(pix,BITSPIXEL);
	np = GetDeviceCaps(pix,PLANES);

// XXX: CreateCompatibleBitmap() creates a monochrome bitmap
//	bm = CreateCompatibleBitmap(pix,can->width,can->height);
	bits = (void *)calloc(((bpp*can->width+31)/32)*can->height*np,4);
	bm = CreateBitmap(can->width,can->height,np,bpp,bits);

	SelectObject(pix,bm);
	clear_pixmap(can);
	current_can = can;
}

void copy_to_canvas(struct canvas *can)
{
	PostThreadMessage(MainThread,WM_APP,can->index,0);
}

void popup_canvas(int index)
{
	create_canvas(canvas[index]);
}

void copy_subimage(struct canvas *subcan,struct canvas *can,POINT pos)
{
	BitBlt(can->pix,pos.x,pos.y,subcan->width,subcan->height,subcan->pix,0,0,SRCCOPY);
}

void draw_wideframe(struct canvas *can,DRAWABLE d)
{
	struct canvas fakecan;
	double xmid,ymid,dx,dy;
	POINT s,e;
	RECT rect;

	fakecan = *can;
	dx = 10*(can->xmax-can->xmin); dy = 10*(can->ymax-can->ymin);
	xmid = (can->xmax+can->xmin)/2; ymid = (can->ymax+can->ymin)/2;

	fakecan.xmin = xmid-dx/2; fakecan.xmax = xmid+dx/2;
	fakecan.ymin = ymid-dy/2; fakecan.ymax = ymid+dy/2;

	rect.left = 0; rect.top = 0;
	rect.right = can->width; rect.bottom = can->height;
	create_brushes();
	FillRect(d,&rect,ClearBrush);
	pline(display,&fakecan,d);
	XC(s) = can->width*9/20; YC(s) = can->height*9/20;
	XC(e) = can->width*11/20; YC(e) = can->height*11/20;
	draw_frame0(d,s,e);
}

void create_brushes()
{
	if ( !ClearBrush )
		ClearBrush = CreateSolidBrush(0xffffff);
	if ( !DrawBrush )
		DrawBrush = CreateSolidBrush(0);
}

void draw_frame0(DRAWABLE d,POINT spos,POINT epos)
{
	RECT rect;
	int ulx,uly,w,h;

	ulx = MIN(XC(spos),XC(epos)); uly = MIN(YC(spos),YC(epos));
	w = ABS(XC(spos)-XC(epos)); h = ABS(YC(spos)-YC(epos));
	if ( !w || !h )
		return;
	rect.left = spos.x; rect.top = spos.y;
	rect.right = epos.x; rect.bottom = epos.y;
	FrameRect(d,&rect,DrawBrush);
}
