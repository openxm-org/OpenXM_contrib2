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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/ifplot.h,v 1.2 2000/08/21 08:31:50 noro Exp $ 
*/
#if defined(THINK_C)
#include        <QuickDraw.h>
#include        <Windows.h>
#include        <Events.h>
#include        <OSUtils.h>
#include        <ToolUtils.h>
#include        <Fonts.h>
/* #include        <Strings.h> */
#else
#include <sys/types.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/Simple.h>
#include <X11/Xaw/Dialog.h>
#include <X11/Xaw/Box.h>
#ifndef OLDX11
#include <X11/Xaw/Toggle.h>
#include <X11/Xaw/Scrollbar.h>
#else
#include <X11/Xaw/Scroll.h>
#define toggleWidgetClass commandWidgetClass
#define XtPointer pointer
#define XawDialogAddButton XtDialogAddButton
#define XawScrollbarSetThumb XtScrollBarSetThumb
#endif
#endif

#define C_IFPLOT C_PRIV
#define C_BMPLOT C_PRIV+1
#define C_CLEAR C_PRIV+2
#define C_PMFLUSH C_PRIV+3
#define C_CONPLOT C_PRIV+4
#define C_PLOTOVER C_PRIV+5
#define C_PLOT C_PRIV+6
#define C_APLOT C_PRIV+7
#define C_DRAWCIRCLE C_PRIV+8

#define MAXCANVAS 16
#define MAXGC 16
#if defined(THINK_C)
#define DEFAULTWIDTH 200
#define DEFAULTHEIGHT 200
#else
#define DEFAULTWIDTH 400
#define DEFAULTHEIGHT 400
#endif

#define MODE_IFPLOT 0
#define MODE_CONPLOT 1
#define MODE_PLOT 2

#define DIR_X 0
#define DIR_Y 1
#define DIR_Z 2

#if defined(THINK_C)
#define COPYBITS

#define POINT Point
#define XC(a) ((a).h)
#define YC(a) ((a).v)
#define DRAWPOINT(d,p,g,x,y) MoveTo(x,y); Line(0,0)
#define DRAWLINE(d,p,g,x,y,u,v) MoveTo(x,y); LineTo(u,v)
#define DRAWSTRING(d,p,g,x,y,s,l) MoveTo(x,y); DrawText(s,0,l)
#define TEXTWIDTH(f,s,l) TextWidth(s,0,l)
#define DISPLAY int
#define WINDOW WindowPtr
#define CURSOR CursHandle
#ifndef COPYBITS
#define DRAWABLE PicHandle
#else
#define DRAWABLE BitMap
#endif
#else
#define POINT XPoint
#define XC(a) ((a).x)
#define YC(a) ((a).y)
#define DRAWPOINT(d,p,g,x,y) (count_and_flush(),XDrawPoint(d,p,g,x,y))
#define DRAWLINE(d,p,g,x,y,u,v) XDrawLine(d,p,g,x,y,u,v)
#define DRAWSTRING(d,p,g,x,y,s,l) XDrawString(d,p,g,x,y,s,l)
#define TEXTWIDTH(f,s,l) XTextWidth(f,s,l)
#define DISPLAY Display
#define WINDOW Window
#define CURSOR Cursor
#define DRAWABLE Drawable
#endif

struct pa {
	int length;
	POINT *pos;
};

struct canvas {
	int index;
#if defined(THINK_C)
	Rect rect;
	WindowRecord record;
	WindowPtr window;
#ifndef COPYBITS
	PicHandle pix;
#else
	BitMap pix,wbits;
	RgnHandle rgnsav;
#endif
#else
	Widget shell,xcoord,ycoord,xdone,ydone,level,wideb,preciseb,noaxisb;
	Window window;
	Pixmap pix;
#endif
	char *wname;
	char mode;
	char wide;
	char precise;
	char noaxis;
	V vx,vy;
	P formula;
	int width,height;
	double xmin,xmax;
	double ymin,ymax;
	double zmin,zmax;
	int nzstep;
	Q qxmin,qxmax;
	Q qymin,qymax;
	struct pa *pa;
};

extern struct canvas *canvas[];
extern struct canvas *current_can;

#ifdef INET
extern VL CO;
extern int stream;
#endif

extern DISPLAY *display;
extern CURSOR normalcur,runningcur,errorcur;

#if !defined(THINK_C)
extern Window rootwin;
extern GC drawGC,dashGC,hlGC,scaleGC,clearGC,xorGC,colorGC;
extern XFontStruct *sffs;
#endif
