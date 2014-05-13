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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/ifplot.h,v 1.17 2014/05/12 16:54:41 saito Exp $ 
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
#define DEFAULTPOLARSTEP 400
#define DEFAULTWIDTH 400
#define DEFAULTHEIGHT 400

#define IFPLOT "ifplot"
#define CONPLOT "conplot"
#define PLOT "plot"
#define INTERACTIVE "interactive"
#define POLARPLOT "polarplot"
#define PLOTOVER "plotover"
#define IFPLOTD "ifplotD"
#define IFPLOTQ "ifplotQ"
#define IFPLOTB "ifplotB"
#define INEQN "ineqn"
#define INEQND "ineqnD"
#define INEQNQ "ineqnQ"
#define INEQNB "ineqnB"
#define INEQNAND "ineqnand"
#define INEQNDAND "ineqnDand"
#define INEQNQAND "ineqnQand"
#define INEQNBAND "ineqnBand"
#define INEQNOR "ineqnor"
#define INEQNDOR "ineqnDor"
#define INEQNQOR "ineqnQor"
#define INEQNBOR "ineqnBor"
#define INEQNXOR "ineqnxor"
#define INEQNDXOR "ineqnDxor"
#define INEQNQXOR "ineqnQxor"
#define INEQNBXOR "ineqnBxor"
#define CONPLOTD "conplotD"
#define CONPLOTQ "conplotQ"
#define CONPLOTB "conplotB"
#define ITVIFPLOT "itvifplot"
#define PLOTOVERD "plotoverD"
#define PLOTOVERQ "plotoverQ"
#define PLOTOVERB "plotoverB"
#define MEMORY_IFPLOT "memory_ifplot"
#define MEMORY_CONPLOT "memory_conplot"
#define MEMORY_PLOT "memory_plot"
#define ARRAYPLOT "arrayplot"
#define OPEN_CANVAS "open_canvas"
#define DRAWCIRCLE "drawcircle"
#define DRAW_OBJ "draw_objc"
#define DRAW_STRING "draw_string"
#define OBJ_CP "obj_cp"
#define CLEAR_CANVAS "clear_canvas"

/*
#define IFPLOT 0
#define CONPLOT 1
#define PLOT 2
#define INTERACTIVE 3
#define POLARPLOT 4
#define PLOTOVER 5
#define IFPLOTD 6
#define IFPLOTQ 7
#define IFPLOTB 8
#define INEQND 9
#define INEQNQ 10
#define INEQNB 11
#define INEQNANDD 12
#define INEQNANDQ 13
#define INEQNANDB 14
#define INEQNORD 15
#define INEQNORQ 16
#define INEQNORB 17
#define INEQNXORD 18
#define INEQNXORQ 19
#define INEQNXORB 20
#define CONPLOTD 21
#define CONPLOTQ 22
#define CONPLOTB 23
#define ITVIFPLOT 24
#define PLOTOVERD 25
#define PLOTOVERQ 26
#define PLOTOVERB 27
#define MEMORY_IFPLOT 28
#define MEMORY_CONPLOT 29
#define MEMORY_PLOT 30
#define ARRAYPLOT 31
#define OPEN_CANVAS 32
#define DRAWCIRCLE 33
#define DRAW_OBJ 34
#define DRAW_STRING 35
#define OBJ_CP 36
#define CLEAR_CANVAS 37
*/

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
#define MKRVECT5(v,x,y,z,u,w)\
((v)=(RealVect *)MALLOC_ATOMIC(sizeof(RealVect)+4*sizeof(int)),\
 (v)->len=4,(v)->body[0]=(x),(v)->body[1]=(y),(v)->body[2]=(z),(v)->body[3]=(u),(v)->body[4]=(w))

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
	double vmin,vmax;
	int nzstep;
	Q qxmin,qxmax;
	Q qymin,qymax;
	struct pa *pa;
	/* to register the history in the interactive mode */
	NODE history;
	int color;
	int division;
};

extern struct canvas *canvas[];
extern struct canvas *closed_canvas[];
extern struct canvas *current_can;
extern char* pfn[];

#if defined(INTERVAL)
extern Pixel BackPixel;
extern int Itvplot;
#endif

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

struct xcolorForPS {
	unsigned long pixel;
	double r,g,b;
	int print;
};

/* prototypes */
int modeNO(char *);
void calc(double **,struct canvas *,int);
double usubstrp(P,double);
void qcalc(char **,struct canvas *);
void calcb(double **,struct canvas *,int);
void calcq(double **,struct canvas *,int);
void polarcalc(struct canvas *);
void sturmseq(VL,P,VECT *);
void seproot(VECT,int,int,int *);
int numch(VECT,Q,Q);
void usubstqp(P,Q,Q *);
void plotcalc(struct canvas *);
int open_canvas(NODE);
int plot(NODE,char *);
int ifplotold(NODE);
int polarplot(NODE);
int conplotD(NODE);
int memory_plot(NODE,LIST *);
int plotover(NODE);
int plotoverD(NODE);
int drawcircle(NODE);
int draw_obj(NODE);
int draw_string(NODE);
int clear_canvas(NODE);
void popup_canvas(int);
int arrayplot(NODE);
int ineqnover(NODE);
int ineqnand(NODE);
int ineqnor(NODE);
int ineqnxor(NODE);
int ifplotOP(NODE,int);
int ifplotNG(NODE,int);
int polarplotNG(NODE);
void ineqnmain(struct canvas *, int, int);
void conplotmainD(struct canvas *);
void obj_op(struct canvas *, struct canvas *, int);
void area_print(DISPLAY *,double **, struct canvas *, int);
void memory_print(struct canvas *,BYTEARRAY *);
void over_print(DISPLAY *,double **,struct canvas *,int);
void polar_print(DISPLAY *,struct canvas *);
unsigned long GetColor(DISPLAY *,char *);
void Pox_pop_local(NODE,Obj *);
void Pox_pop_cmo(NODE,Obj *);

#if defined(INTERVAL)
int itvifplot(NODE);
//int itvplot1(NODE);
//int itvplot2(NODE);
//int itvplot3(NODE);
//int itvplot4(NODE);
int objcp(NODE);
int ineqn(NODE);
void obj_cp(struct canvas *,struct canvas *,int);
void itvcalc(double **, struct canvas *, int);
void itvplotmain(struct canvas *, int);
void itvplotmain1(struct canvas *);
#endif
void ifplot_resize(struct canvas *,POINT,POINT);
void plot_resize(struct canvas *,POINT,POINT);
void ifplotmainOld(struct canvas *);
void ifplotmain(struct canvas *);
void ifplotmains(struct canvas *);
void qifplotmain(struct canvas *);
void if_print(DISPLAY *,double **,struct canvas *,int);
void if_printOld(DISPLAY *,double **,struct canvas *);
void memory_if_print(double **,struct canvas *,BYTEARRAY *);
void con_print(DISPLAY *,double **,struct canvas *);
void memory_con_print(double **,struct canvas *,BYTEARRAY *);
void qif_print(DISPLAY *,char **,struct canvas *);
void plot_print(DISPLAY *,struct canvas *);
void set_drawcolor(unsigned int);
void draw_character_string(DISPLAY *,struct canvas *,int,int,char *,int);
void draw_point(DISPLAY *,struct canvas *,int,int,int);
void draw_line(DISPLAY *,struct canvas *,int,int,int,int,int);
void pline(DISPLAY *,struct canvas *,DRAWABLE);
double adjust_scale(double,double);
void initmarker(struct canvas *,char *);
void marker(struct canvas *,int,int);
void define_cursor(WINDOW,CURSOR);
void count_and_flush();
void flush();
int search_canvas();
void create_canvas(struct canvas *can);
void copy_to_canvas(struct canvas *can);
void set_selection();
void reset_selection();
void set_busy();
void reset_busy();
void clear_pixmap(struct canvas *can);
void alloc_pixmap(struct canvas *can);
void copy_subimage(struct canvas *subcan,struct canvas *can,POINT pos);
int init_plot_display(int,char **);
void reset_current_computation();
void process_xevent();

#if defined(VISUAL)
int search_active_canvas();
void destroy_canvas(struct canvas *can);
void draw_wideframe(struct canvas *can,DRAWABLE d);
void create_brushes();
void draw_frame0(DRAWABLE d,POINT spos,POINT epos);

#endif
