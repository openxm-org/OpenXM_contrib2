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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/ifplot.h,v 1.7 2000/12/05 01:24:59 noro Exp $ 
*/
#if defined(VISUAL)
/* for Visual C++ */
#include <windows.h>
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

#define MAXCANVAS 64
#define MAXGC 16
#define DEFAULTWIDTH 400
#define DEFAULTHEIGHT 400

#define MODE_IFPLOT 0
#define MODE_CONPLOT 1
#define MODE_PLOT 2
#define MODE_INTERACTIVE 3

#define DIR_X 0
#define DIR_Y 1
#define DIR_Z 2

#if defined(VISUAL)
/* for Visual C++ */
#define XC(a) ((a).x)
#define YC(a) ((a).y)
#define DISPLAY int
#define WINDOW void *
#define CURSOR int
#define DRAWABLE HDC

/* XXX : use GC argument as the color */
#define DRAWPOINT(d,p,g,x,y) SetPixel(p,x,y,0) 

#define DRAWLINE(d,p,g,x,y,u,v) MoveToEx(p,x,y,&oldpos); LineTo(p,u,v)
#define DRAWSTRING(d,p,g,x,y,s,l) TextOut(p,x,y,s,l)
/* #define TEXTWIDTH(f,s,l) */
#else
/* for UNIX */
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

typedef struct RealVect {
	int len;
	int body[1];
} RealVect;

#define MKRVECT2(v,x,y)\
((v)=(RealVect *)MALLOC_ATOMIC(sizeof(RealVect)+sizeof(int)),\
 (v)->len=2,(v)->body[0]=(x),(v)->body[1]=(y))
#define MKRVECT3(v,x,y,z)\
((v)=(RealVect *)MALLOC_ATOMIC(sizeof(RealVect)+2*sizeof(int)),\
 (v)->len=3,(v)->body[0]=(x),(v)->body[1]=(y),(v)->body[2]=(z))
#define MKRVECT4(v,x,y,z,u)\
((v)=(RealVect *)MALLOC_ATOMIC(sizeof(RealVect)+3*sizeof(int)),\
 (v)->len=4,(v)->body[0]=(x),(v)->body[1]=(y),(v)->body[2]=(z),(v)->body[3]=(u))

#define RV_POINT 1
#define RV_LINE 2
#define RV_ARC 3

struct canvas {
	int index;
#if defined(VISUAL)
/* for Visual C++ */
	void *window; /* pointer to CMainFrame */
	HWND hwnd; /* handle to the canvas window */
	HDC pix; /* shadow DC on memory */
	char *prefix;
	int percentage;
	struct canvas *real_can;
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
	/* to register the history in the interactive mode */
	NODE history;
};

extern struct canvas *canvas[];
extern struct canvas *current_can;

extern VL CO;
extern int stream;

extern DISPLAY *display;
extern CURSOR normalcur,runningcur,errorcur;

#if defined(VISUAL)
extern POINT start_point,end_point;
extern SIZE cansize;
#else
extern Window rootwin;
extern GC drawGC,dashGC,hlGC,scaleGC,clearGC,xorGC,colorGC,cdrawGC;
extern XFontStruct *sffs;
#endif

/* prototypes */
void calc(double **tab,struct canvas *can,int nox);
double usubstrp(P p,double r);
void qcalc(char **tab,struct canvas *can);
void sturmseq(VL vl,P p,VECT *rp);
void seproot(VECT s,int min,int max,int *ar);
int numch(VECT s,Q n,Q a0);
void usubstqp(P p,Q r,Q *v);
void plotcalc(struct canvas *can);
int open_canvas(NODE arg);
int plot(NODE arg);
int memory_plot(NODE arg,LIST *bytes);
int plotover(NODE arg);
int drawcircle(NODE arg);
int draw_obj(NODE arg);
int clear_canvas(NODE arg);
int arrayplot(NODE arg);
void ifplot_resize(struct canvas *can,POINT spos,POINT epos);
void plot_resize(struct canvas *can,POINT spos,POINT epos);
void ifplotmain(struct canvas *can);
void qifplotmain(struct canvas *can);
void if_print(DISPLAY *display,double **tab,struct canvas *can);
void memory_if_print(double **tab,struct canvas *can,BYTEARRAY *bytes);
void con_print(DISPLAY *display,double **tab,struct canvas *can);
void memory_con_print(double **tab,struct canvas *can,BYTEARRAY *bytes);
void qif_print(DISPLAY *display,char **tab,struct canvas *can);
void plot_print(DISPLAY *display,struct canvas *can);
void draw_point(DISPLAY *display,struct canvas *can,int x,int y,int color);
void draw_line(DISPLAY *display,struct canvas *can,int x,int y,int u,int v,int color);
void pline(DISPLAY *display,struct canvas *can,DRAWABLE d);
double adjust_scale(double e,double w);
void initmarker(struct canvas *can,char *message);
void marker(struct canvas *can,int dir,int p);
void define_cursor(WINDOW w,CURSOR cur);
void count_and_flush();
void flush();

#if defined(VISUAL)
int search_canvas();
int search_active_canvas();
void create_canvas(struct canvas *can);
void destroy_canvas(struct canvas *can);
void clear_pixmap(struct canvas *can);
void alloc_pixmap(struct canvas *can);
void copy_to_canvas(struct canvas *can);
void copy_subimage(struct canvas *subcan,struct canvas *can,POINT pos);
void draw_wideframe(struct canvas *can,DRAWABLE d);
void create_brushes();
void draw_frame0(DRAWABLE d,POINT spos,POINT epos);

void reset_current_computation();
void set_selection();
void reset_selection();
void set_busy();
void reset_busy();
#endif
