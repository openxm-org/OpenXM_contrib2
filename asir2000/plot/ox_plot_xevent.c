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
 * e-mail at risa-admin@flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/ox_plot_xevent.c,v 1.1.1.1 1999/12/03 07:39:13 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"
#include "cursor.h"

#ifdef ABS
#undef ABS
#define ABS(a) ((a)>0?(a):-(a))
#endif

static char *dname;
static int remotes;
static int depth,scrn;

extern jmp_buf ox_env;
static Widget toplevel;
static XtAppContext app_con;

static int busy;

static struct PlotResources {
	Pixel ForePixel,BackPixel,DashPixel;
	char *ForeName,*BackName,*DashName;
	Boolean Reverse;
} PlotResources;

#define forePixel PlotResources.ForePixel
#define backPixel PlotResources.BackPixel
#define dashPixel PlotResources.DashPixel
#define foreName PlotResources.ForeName
#define backName PlotResources.BackName
#define dashName PlotResources.DashName
#define reverse PlotResources.Reverse

Cursor create_cursor();

#define blackPixel	BlackPixel(display,scrn)
#define whitePixel	WhitePixel(display,scrn)

#define LABELWIDTH 150

process_xevent() {
	XEvent ev;

	while ( XPending(display) ) {
		XtAppNextEvent(app_con,&ev);
		XtDispatchEvent(&ev);
	}
}

/* event handlers */

static POINT spos,cpos;

void press(w,can,ev)
Widget w;
struct canvas *can;
XButtonEvent *ev;
{
	POINT p;

	switch ( ev->button ) {
		case Button1:
			XC(spos) = ev->x; YC(spos) = ev->y; cpos = spos; break;
		case Button3:
			XC(p) = ev->x; YC(p) = ev->y; draw_coord(can,&p); break;
		default:
			break;
	}
}

void motion(w,can,ev)
Widget w;
struct canvas *can;
XMotionEvent *ev;
{

	POINT o,p;

	if ( ev->state & Button1Mask ) {
		o = cpos; XC(cpos) = ev->x; YC(cpos) = ev->y; 
		draw_frame(can->window,spos,o,cpos);
	} else if ( ev->state & Button3Mask ) {
		XC(p) = ev->x; YC(p) = ev->y;
		draw_coord(can,p);
	}
}

void release(w,can,ev)
Widget w;
struct canvas *can;
XButtonEvent *ev;
{
	POINT e;

	switch ( ev->button ) {
		case Button1:
			e.x = ev->x; e.y = ev->y;
			draw_frame0(can->window,spos,e);
			if ( !busy ) {
				if ( can->mode == MODE_PLOT )
					plot_resize(can,spos,e);
				else
					ifplot_resize(can,spos,e);
			}
			break;
		default:
			break;
	}
}

void structure(w,can,ev)
Widget w;
struct canvas *can;
XEvent *ev;
{
	switch ( ev->xany.type ) {
		case Expose: 
				if ( !ev->xexpose.count )
					redraw_canvas(can);
			break;
		case ConfigureNotify:
			redraw_canvas(can); break;
		default:
			break;
	}
}

static int lindex;

void lpress(w,can,ev)
Widget w;
struct canvas *can;
XButtonEvent *ev;
{
	lindex = (can->height-ev->y)/(can->height/can->nzstep);
	draw_level(can,lindex,hlGC);
}

void jumpproc(w,can,percent)
Widget w;
struct canvas *can;
float *percent;
{
	int index;

	index = can->nzstep * (1.0-*percent);
	if ( index == lindex )
		return;
	if ( lindex >= 0 )
		draw_level(can,lindex,drawGC);
	lindex = index;
	draw_level(can,lindex,hlGC); 
}

void jumpproc_m(w,can,percent)
Widget w;
struct canvas *can;
float *percent;
{
	int index;

	index = can->nzstep * (1.0-*percent);
	if ( index != lindex ) {
		draw_level(can,lindex,drawGC);
		draw_level(can,lindex,hlGC); 
	}
}

void lrelease(w,can,ev)
Widget w;
struct canvas *can;
XButtonEvent *ev;
{
	draw_level(can,lindex,drawGC); lindex = -1;
}

void lrelease_m(w,can,ev)
Widget w;
struct canvas *can;
XButtonEvent *ev;
{
	lindex = -1;
}

draw_level(can,index,gc)
struct canvas *can;
int index;
GC gc;
{
	Pixmap pix;
	struct pa *pa;
	int i,len;
	POINT *p;
	Arg arg[2];
	char buf[BUFSIZ];

	if ( busy || can->wide || index < 0 || index > can->nzstep )
		return;
	pix = can->pix; pa = &can->pa[index]; len = pa->length; p = pa->pos;
	for ( i = 0; i < len; i++ )
		XDrawPoint(display,pix,gc,p[i].x,p[i].y);
	sprintf(buf,"level:%g",can->zmin+(can->zmax-can->zmin)*index/can->nzstep);
	XtSetArg(arg[0],XtNlabel,buf); XtSetArg(arg[1],XtNwidth,LABELWIDTH);
	XtSetValues(can->level,arg,2);
	copy_to_canvas(can);
}

draw_frame(window,spos,opos,epos)
Window window;
POINT spos,opos,epos;
{
	if ( XC(opos) != XC(epos) || YC(opos) != YC(epos) )
		draw_frame0(window,spos,opos);
	draw_frame0(window,spos,epos);
}

draw_frame0(window,spos,epos)
Window window;
POINT spos,epos;
{
	int ulx,uly,w,h;

	ulx = MIN(XC(spos),XC(epos)); uly = MIN(YC(spos),YC(epos));
	w = ABS(XC(spos)-XC(epos)); h = ABS(YC(spos)-YC(epos));
	if ( !w || !h )
		return;
	XDrawRectangle(display,window,xorGC,ulx,uly,w,h);
	XFlush(display);
}

draw_coord(can,pos)
struct canvas *can;
POINT pos;
{
	char buf[BUFSIZ];
	Arg arg[2];
	double x,y,xmin,ymax,dx,dy;

	if ( can->wide ) {
		dx = 10*(can->xmax-can->xmin); dy = 10*(can->ymax-can->ymin);
		xmin = (can->xmax+can->xmin-dx)/2;
		ymax = (can->ymax+can->ymin+dy)/2;
	} else {
		dx = can->xmax-can->xmin; dy = can->ymax-can->ymin;
		xmin = can->xmin; ymax = can->ymax;
	}
	x = xmin+XC(pos)*dx/can->width;
	y = ymax-YC(pos)*dy/can->height;
	sprintf(buf,"%s:%g",can->vx?can->vx->name:"horiz",x);
	XtSetArg(arg[0],XtNlabel,buf); XtSetArg(arg[1],XtNwidth,LABELWIDTH); 
	XtSetValues(can->xcoord,arg,2);
	sprintf(buf,"%s:%g",can->vy?can->vy->name:"vert",y);
	XtSetArg(arg[0],XtNlabel,buf); XtSetArg(arg[1],XtNwidth,LABELWIDTH); 
	XtSetValues(can->ycoord,arg,2);
}

redraw_canvas(can)
struct canvas *can;
{
	if ( can->wide )
		draw_wideframe(can);
	else
		copy_to_canvas(can);
}

search_canvas()
{
	int i;

	for ( i = 0; i < MAXCANVAS; i++ )
		if ( !canvas[i] ) {
			canvas[i] = (struct canvas *)MALLOC(sizeof(struct canvas));
			canvas[i]->index = i; return i;
		}
}

search_active_canvas()
{
	int i;

	for ( i = 0; i < MAXCANVAS; i++ )
		if ( canvas[i] )
			return i;
	return -1;
}

void destroy_canvas(w,can,calldata)
Widget w;
struct canvas *can;
XtPointer calldata;
{
	XtPopdown(can->shell); 
	XtDestroyWidget(can->shell);
	XFlush(display);
	if ( can == current_can ) {
		reset_busy(can); current_can = 0;
	}
	canvas[can->index] = 0;
}

void precise_canvas(w,can,calldata)
Widget w;
struct canvas *can;
XtPointer calldata;
{
	if ( can->precise )
		can->precise = 0;
	else
		can->precise = 1;
}

void wide_canvas(w,can,calldata)
Widget w;
struct canvas *can;
XtPointer calldata;
{
	if ( can->wide ) {
		can->wide = 0; copy_to_canvas(can); 
	} else {
		can->wide = 1; draw_wideframe(can);
	}
}

void noaxis_canvas(w,can,calldata)
Widget w;
struct canvas *can;
XtPointer calldata;
{
	if ( can->noaxis )
		can->noaxis = 0;
	else
		can->noaxis = 1;
	if ( can->wide )
		draw_wideframe(can);
	else
		copy_to_canvas(can); 
}

toggle_button(w,flag)
Widget w;
int flag;
{
	Arg arg[2];

	if ( flag ) {
		XtSetArg(arg[0],XtNforeground,backPixel);
		XtSetArg(arg[1],XtNbackground,forePixel);
	} else {
		XtSetArg(arg[0],XtNforeground,forePixel);
		XtSetArg(arg[1],XtNbackground,backPixel);
	}
	XtSetValues(w,arg,2); XFlush(display);
}

draw_wideframe(can)
struct canvas *can;
{
	struct canvas fakecan;
	double xmin,xmax,ymin,ymax,xmid,ymid,dx,dy;
	POINT s,e;

	fakecan = *can;
	dx = 10*(can->xmax-can->xmin); dy = 10*(can->ymax-can->ymin);
	xmid = (can->xmax+can->xmin)/2; ymid = (can->ymax+can->ymin)/2;

	fakecan.xmin = xmid-dx/2; fakecan.xmax = xmid+dx/2;
	fakecan.ymin = ymid-dy/2; fakecan.ymax = ymid+dy/2;

	XFillRectangle(display,can->window,clearGC,
		0,0,can->width,can->height);
	pline(display,&fakecan,can->window);
	XC(s) = can->width*9/20; YC(s) = can->height*9/20;
	XC(e) = can->width*11/20; YC(e) = can->height*11/20;
	draw_frame0(can->window,s,e);
}

create_popup(parent,name,str,shell,dialog)
Widget parent;
char *name,*str;
Widget *shell,*dialog;
{
	Arg arg[3];
	Position x,y;

	XtTranslateCoords(parent,0,0,&x,&y);
	XtSetArg(arg[0],XtNx,x); XtSetArg(arg[1],XtNy,y);
	*shell = XtCreatePopupShell(name,transientShellWidgetClass,parent,arg,2);
	XtSetArg(arg[0],XtNlabel,str);
	*dialog = XtCreateManagedWidget("dialog",dialogWidgetClass,*shell,arg,1);
}

warning(can,s)
struct canvas *can;
char *s;
{
	void popdown_warning();
	Widget warnshell,warndialog;
	Position x,y;
	Arg arg[3];

	if ( !can->shell )
		return;
	create_popup(can->shell,"warning",s,&warnshell,&warndialog);
	XawDialogAddButton(warndialog,"dismiss",popdown_warning,warnshell);
	XtPopup(warnshell,XtGrabNone);
}

void popdown_warning(w,client,call) 
Widget w;
XtPointer client,call;
{
	XtPopdown(client); XtDestroyWidget(client);
}

void show_formula(w,can,calldata)
Widget w;
struct canvas *can;
XtPointer calldata;
{
	void popdown_formula();
	Widget fshell,fdialog;
	char buf[BUFSIZ];

	soutput_init(buf); sprintexpr(CO,(Obj)can->formula);
	create_popup(can->shell,"formula",buf,&fshell,&fdialog);
	XawDialogAddButton(fdialog,"dismiss",popdown_formula,w);
	XtSetSensitive(w,False); XtPopup(fshell,XtGrabNone);
}

void popdown_formula(w,fbutton,call) 
Widget w,fbutton;
XtPointer call;
{
	Widget shell = XtParent(XtParent(w));
	XtPopdown(shell); XtDestroyWidget(shell);
	XtSetSensitive(fbutton,True);
}

#define NormalSelection ButtonPressMask|ButtonReleaseMask|Button1MotionMask|Button3MotionMask|StructureNotifyMask| ExposureMask

create_canvas(can)
struct canvas *can;
{
	XEvent event;
	Widget box,frame,commands,
		coords,quit,wide,precise,canvas,formula;
	Window window;
	Pixmap pix;
	int i,width,height;
	Arg arg[6];
	char buf[BUFSIZ];

	width = can->width; height = can->height;

	sprintf(buf,"%s : %d/%d", can->wname?can->wname:"Plot",
		remotes,can->index);
	XtSetArg(arg[0],XtNiconName,buf);
	can->shell =
		XtCreatePopupShell("shell",topLevelShellWidgetClass,toplevel,arg,1);

	XtSetArg(arg[0],XtNhSpace,0);
	XtSetArg(arg[1],XtNvSpace,0);
	XtSetArg(arg[2],XtNborderWidth,0);
	box = XtCreateManagedWidget("box",boxWidgetClass,can->shell,arg,3);

	frame = XtCreateManagedWidget("form",formWidgetClass,box,NULL,0);

	XtSetArg(arg[0],XtNorientation,XtorientHorizontal);
	XtSetArg(arg[1],XtNborderWidth,0);
	commands = XtCreateManagedWidget("commands",boxWidgetClass,frame,arg,2);

	quit = XtCreateManagedWidget("quit",commandWidgetClass,commands,NULL,0);
	XtAddCallback(quit,XtNcallback,destroy_canvas,can);
	can->wideb = wide = 
		XtCreateManagedWidget("wide",toggleWidgetClass,commands,NULL,0);
	XtAddCallback(wide,XtNcallback,wide_canvas,can);
	can->preciseb = precise = 
		XtCreateManagedWidget("precise",toggleWidgetClass,commands,NULL,0);
	XtAddCallback(precise,XtNcallback,precise_canvas,can);
	formula = 
		XtCreateManagedWidget("formula",commandWidgetClass,commands,NULL,0);
	XtAddCallback(formula,XtNcallback,show_formula,can);
	can->noaxisb =
		XtCreateManagedWidget("noaxis",toggleWidgetClass,commands,NULL,0);
	XtAddCallback(can->noaxisb,XtNcallback,noaxis_canvas,can);

	XtSetArg(arg[0],XtNfromVert,commands);
	XtSetArg(arg[1],XtNwidth,width); 
	XtSetArg(arg[2],XtNheight,height);
	canvas = XtCreateManagedWidget("canvas",simpleWidgetClass,frame,arg,3);

	XtSetArg(arg[0],XtNfromVert,canvas);
	XtSetArg(arg[1],XtNheight,5); 
	XtSetArg(arg[2],XtNwidth,width);
	XtSetArg(arg[3],XtNorientation,XtorientHorizontal);
	XtSetArg(arg[4],XtNsensitive,False);
	can->xdone =
		XtCreateManagedWidget("xdone",scrollbarWidgetClass,frame,arg,5);

	XtSetArg(arg[0],XtNfromHoriz,canvas);
	XtSetArg(arg[1],XtNfromVert,commands);
	XtSetArg(arg[2],XtNwidth,5);
	XtSetArg(arg[3],XtNheight,height);
	XtSetArg(arg[4],XtNorientation,XtorientVertical);
	XtSetArg(arg[5],XtNsensitive,False);
	can->ydone = 
		XtCreateManagedWidget("ydone",scrollbarWidgetClass,frame,arg,6);

	XtSetArg(arg[0],XtNfromVert,can->xdone);
	XtSetArg(arg[1],XtNorientation,XtorientHorizontal);
	XtSetArg(arg[2],XtNborderWidth,0);
	coords = XtCreateManagedWidget("coords",boxWidgetClass,frame,arg,3);

	XtSetArg(arg[0],XtNwidth,LABELWIDTH);
	can->xcoord = XtCreateManagedWidget("xcoord",labelWidgetClass,coords,arg,1);
	XtSetArg(arg[0],XtNwidth,LABELWIDTH);
	can->ycoord = XtCreateManagedWidget("ycoord",labelWidgetClass,coords,arg,1);

	XtAddEventHandler(canvas,ButtonPressMask,False,press,can);
	XtAddEventHandler(canvas,ButtonReleaseMask,False,release,can);
	XtAddEventHandler(canvas,Button1MotionMask,False,motion,can);
	XtAddEventHandler(canvas,Button3MotionMask,False,motion,can);
	XtAddEventHandler(canvas,StructureNotifyMask,False,structure,can);
	XtAddEventHandler(canvas,ExposureMask,False,structure,can);

	if ( can->mode == MODE_CONPLOT ) {
		Widget scale;

		XtSetArg(arg[0],XtNwidth,LABELWIDTH);
		can->level = XtCreateManagedWidget("level",labelWidgetClass,
			commands,arg,1);

		XtSetArg(arg[0],XtNsensitive,True); 
		XtSetValues(can->ydone,arg,1);
		if ( depth >= 2 ) {
			XtAddCallback(can->ydone,XtNjumpProc,jumpproc,can);
			XtAddEventHandler(can->ydone,ButtonReleaseMask,False,lrelease,can);
		} else {
			XtAddCallback(can->ydone,XtNjumpProc,jumpproc_m,can);
			XtAddEventHandler(can->ydone,ButtonReleaseMask,False,lrelease_m,can);
		}
	}
	if ( can->mode != MODE_IFPLOT || !qpcheck((Obj)can->formula) )
		XtSetSensitive(precise,False);
	XtPopup(can->shell,XtGrabNone);

	window = can->window = XtWindow(canvas);
	pix = can->pix = XCreatePixmap(display,window,width,height,depth);
	XFillRectangle(display,pix,clearGC,0,0,width,height);
	XDefineCursor(display,window,normalcur);
	XFlush(display);
	current_can = can;
}

alloc_pixmap(can)
struct canvas *can;
{
	can->pix = XCreatePixmap(display,can->window,
		can->width,can->height,depth);
	XFillRectangle(display,can->pix,clearGC,0,0,can->width,can->height);
}

static XrmOptionDescRec options[] = {
{"-reverse","*reverse",XrmoptionNoArg,"on"},
{"-fg","*foreground",XrmoptionSepArg,NULL},
{"-bg","*background",XrmoptionSepArg,NULL},
};

#define offset(name) XtOffset(struct PlotResources *,name)

static XtResource resources[] = {
{"reverse","Reverse",XtRBoolean,sizeof(Boolean),
	offset(Reverse),XtRBoolean,&reverse},
{"foreground","Foreground",XtRString,sizeof(char *),
	offset(ForeName),XtRString,NULL},
{"foreground","Foreground",XtRPixel,sizeof(Pixel),
	offset(ForePixel),XtRPixel,(XtPointer)&forePixel},
{"background","Background",XtRString,sizeof(char *),
	offset(BackName),XtRString,NULL},
{"background","Background",XtRPixel,sizeof(Pixel),
	offset(BackPixel),XtRPixel,(XtPointer)&backPixel},
{"dash","Dash",XtRString,sizeof(char *),
	offset(DashName),XtRString,NULL},
{"dash","Dash",XtRPixel,sizeof(Pixel),
	offset(DashPixel),XtRPixel,(XtPointer)&dashPixel},
};

init_plot_display(argc,argv)
int argc;
char **argv;
{
	int ac;
	char **av;
	unsigned int tmp;

	for ( ac = argc, av = argv; ac; ac--, av++ )
		if ( index(*av,':') )
			dname = *av;
	XtToolkitInitialize();
	app_con = XtCreateApplicationContext();
	display = XtOpenDisplay(app_con,dname,"plot","Plot",
		options,XtNumber(options),&argc,argv);
	if ( !display ) {
		fprintf(stderr,"Can't open display\n");
		exit(1);
	}
	toplevel = XtAppCreateShell(0,"Plot",applicationShellWidgetClass,
		display,0,0);

	forePixel = blackPixel; backPixel = whitePixel;
	dashPixel = blackPixel;
	XtGetApplicationResources(toplevel,&PlotResources,
		resources,XtNumber(resources),NULL,0);
	display = XtDisplay(toplevel);
	scrn = DefaultScreen(display);
	depth = DefaultDepth(display,scrn);
	rootwin = RootWindow(display,scrn);

	if ( reverse ) {
		tmp = forePixel; forePixel = backPixel; backPixel = tmp;
	}

	create_gc();
	create_font();
	create_cursors();
}

static char *scalefont = "*-8-80-*";

create_font() {
	Font sfid;

	sfid = XLoadFont(display,scalefont);
	sffs = XQueryFont(display,sfid);
	XSetFont(display,scaleGC,sfid);
}

create_gc() {
	static XColor color = {0,0x0,0x0,0x0,DoRed|DoGreen|DoBlue,0};
	int i,b,step;

	drawGC = XCreateGC(display,rootwin,0,NULL);
	dashGC = XCreateGC(display,rootwin,0,NULL);
	hlGC = XCreateGC(display,rootwin,0,NULL);
	clearGC = XCreateGC(display,rootwin,0,NULL);
	scaleGC = XCreateGC(display,rootwin,0,NULL);
	xorGC = XCreateGC(display,rootwin,0,NULL);
	colorGC = XCreateGC(display,rootwin,0,NULL);
	XCopyGC(display,DefaultGC(display,scrn),(1L<<(GCLastBit+1))-1,drawGC);
	XCopyGC(display,DefaultGC(display,scrn),(1L<<(GCLastBit+1))-1,dashGC);
	XCopyGC(display,DefaultGC(display,scrn),(1L<<(GCLastBit+1))-1,clearGC);
	XCopyGC(display,DefaultGC(display,scrn),(1L<<(GCLastBit+1))-1,scaleGC);
	XCopyGC(display,DefaultGC(display,scrn),(1L<<(GCLastBit+1))-1,xorGC);
	XCopyGC(display,DefaultGC(display,scrn),(1L<<(GCLastBit+1))-1,colorGC);
	XSetForeground(display,drawGC,forePixel);
	XSetForeground(display,scaleGC,forePixel);
	XSetForeground(display,clearGC,backPixel);
	XSetForeground(display,xorGC,forePixel^backPixel);
	XSetFunction(display,xorGC,GXxor);
	XSetForeground(display,dashGC,dashPixel);
	XSetLineAttributes(display,dashGC,1,LineOnOffDash,CapButt,JoinRound);  

	color.red = color.green = color.blue = 0xffff/2;
	XAllocColor(display,DefaultColormap(display,scrn),&color);
	XSetForeground(display,hlGC,color.pixel);
	color.red = 0xffff; color.green = color.blue = 0;
	XAllocColor(display,DefaultColormap(display,scrn),&color);
	XSetForeground(display,colorGC,color.pixel);
}

create_cursors() {
	static XColor fg = {0, 0x0, 0x0, 0x0,DoRed|DoGreen|DoBlue,0};
	static XColor bg = {0, 0xffff, 0xffff, 0xffff,DoRed|DoGreen|DoBlue,0};

	XAllocColor(display,DefaultColormap(display,scrn),&fg);
	XAllocColor(display,DefaultColormap(display,scrn),&bg);
	normalcur = create_cursor(h_bits,h_m_bits,h_width,h_height,
		h_x_hot,h_y_hot,&fg,&bg);
	runningcur = create_cursor(ht_bits,ht_m_bits,ht_width,ht_height,
		ht_width/2,ht_height/2,&fg,&bg);
	errorcur = create_cursor(m_bits,m_m_bits,m_width,m_height,
		m_width/2,m_height/2,&fg,&bg);
}

Cursor create_cursor(image,mask,width,height,xhot,yhot,fg,bg)
char *image,*mask;
int width,height,xhot,yhot;
XColor *fg,*bg;
{
	Pixmap ipix,mpix;

	ipix = XCreateBitmapFromData(display,rootwin,image,width,height);
	mpix = XCreateBitmapFromData(display,rootwin,mask,width,height);
	return XCreatePixmapCursor(display,ipix,mpix,fg,bg,xhot,yhot);
}

copy_to_canvas(can)
struct canvas *can;
{
	if ( display ) {
		XCopyArea(display,can->pix,can->window,
			drawGC,0,0,can->width,can->height,0,0);
		pline(display,can,can->window);
		XFlush(display);
	}
}

copy_subimage(subcan,can,pos)
struct canvas *subcan,*can;
XPoint pos;
{
	if ( display ) {
		XCopyArea(display,subcan->pix,can->pix,
			drawGC,0,0,subcan->width,subcan->height,pos.x,pos.y);
		XFlush(display);
	}
}

#include <signal.h>
#include <fcntl.h>

set_selection() {
	if ( current_can ) {
		XSelectInput(display,current_can->window,0);
		XFlush(display);
	}
}

reset_selection() {
	if ( current_can ) {
		XSelectInput(display,current_can->window,NormalSelection);
		XFlush(display);
	}
}

set_busy(can)
struct canvas *can;
{
	busy = 1;
	XtSetSensitive(can->wideb,False);
	XtSetSensitive(can->preciseb,False);
	XtSetSensitive(can->noaxisb,False);
	XFlush(display);
}

reset_busy(can)
struct canvas *can;
{
	busy = 0;
	if ( can->window ) {
		XtSetSensitive(can->wideb,True);
		XtSetSensitive(can->noaxisb,True);
		if ( can->mode == MODE_IFPLOT && qpcheck((Obj)can->formula) )
			XtSetSensitive(can->preciseb,True);
		XFlush(display);
	}
}

reset_current_computation()
{
	if ( current_can ) {
		reset_selection(); reset_busy(current_can);
		define_cursor(current_can->window,normalcur);
	}
}
