/* $OpenXM: OpenXM/src/asir99/plot/ifplot.h,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
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
