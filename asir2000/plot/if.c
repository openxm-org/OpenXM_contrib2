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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/if.c,v 1.24 2013/12/19 05:48:24 saito Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"

#if defined(INTERVAL)
// Time message and func
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>

static struct oEGT ltime;
static double r0;
double get_rtime();
#if defined(ITV_TIME_CHECK)
void tstart()
{
	get_eg(&ltime);
	r0=get_rtime();
}

void tstop(struct canvas *can)
{
	struct oEGT egt1;
	double e, g, r;
	char ts[100];
	void popdown_warning();
	Widget warnshell,warndialog;

	get_eg(&egt1);
	e=egt1.exectime - ltime.exectime;
	g=egt1.gctime - ltime.gctime;
	r=get_rtime() - r0;
	sprintf(ts,"(%8.6f + gc %8.6f) total %8.6f \n",e,g,r);
	create_popup(can->shell,"Message",&ts,&warnshell,&warndialog);
	XawDialogAddButton(warndialog,"OK",popdown_warning,warnshell);
	XtPopup(warnshell,XtGrabNone);
	SetWM_Proto(warnshell);
}
#else
#define tstart()
#define tstop(a)
#endif
#endif

extern JMP_BUF ox_env;

int open_canvas(NODE arg){
	int id;
	struct canvas *can;
	LIST wsize;
	STRING wname;

	wsize=(LIST)ARG0(arg);
	wname=(STRING)ARG1(arg);
	can=canvas[id=search_canvas()];
	can->mode=MODE_INTERACTIVE;
	if(!wsize){
		can->width=DEFAULTWIDTH;
		can->height=DEFAULTHEIGHT;
	} else {
		can->width=QTOS((Q)BDY(BDY(wsize)));
		can->height=QTOS((Q)BDY(NEXT(BDY(wsize))));
	}
	if(wname)can->wname=BDY(wname);
	else can->wname="";
	create_canvas(can);
	return id;
}


int plot(NODE arg,int fn){
	int id;
	NODE n;
	struct canvas *can;
	P formula;
	LIST xrange,yrange,zrange,wsize;
	STRING wname;
	V v;

	formula=(P)ARG0(arg);
	xrange=(LIST)ARG1(arg);
	yrange=(LIST)ARG2(arg);
	zrange=(LIST)ARG3(arg);
	wsize=(LIST)ARG4(arg);
	wname=(STRING)ARG5(arg);
	can=canvas[id=search_canvas()];
	if(xrange){
		n=BDY(xrange);can->vx=VR((P)BDY(n));n=NEXT(n);
		can->qxmin=(Q)BDY(n);n=NEXT(n);can->qxmax=(Q)BDY(n);
		can->xmin=ToReal(can->qxmin);can->xmax=ToReal(can->qxmax); 
	}
	if(yrange){
		n=BDY(yrange);can->vy=VR((P)BDY(n));n=NEXT(n);
		can->qymin=(Q)BDY(n);n=NEXT(n);can->qymax=(Q)BDY(n);
		can->ymin=ToReal(can->qymin);can->ymax=ToReal(can->qymax); 
	}
	can->mode=fn;
	if(zrange){
		n=BDY(zrange); can->zmin=ToReal(BDY(n)); 
		n=NEXT(n);can->zmax=ToReal(BDY(n));
		n=NEXT(n);
		if(can->mode==MODE_CONPLOT)can->nzstep=QTOS((Q)BDY(n));
		else {
			can->vx=VR((P)BDY(BDY(zrange)));
			can->nzstep=QTOS((Q)BDY(n));
		}
	}
	if(!wsize){
		can->width=DEFAULTWIDTH;
		can->height=DEFAULTHEIGHT;
	} else {
		can->width=QTOS((Q)BDY(BDY(wsize)));
		can->height=QTOS((Q)BDY(NEXT(BDY(wsize))));
	}
	if(wname) can->wname = BDY(wname);
	else can->wname="";
	can->formula=formula; 
	if(can->mode==MODE_PLOT){
		//plot
		plotcalc(can);
		create_canvas(can);
		plot_print(display,can);
	} else if(can->mode==MODE_POLARPLOT){
		//polarplot
		polarplotcalc(can);
		create_canvas(can);
		plot_print(display,can);
	} else {
		//ifplot,conplot
		create_canvas(can);
		ifplotmainOld(can);
	}
	copy_to_canvas(can);
	return id;
}

void ifplotmainOld(struct canvas *can){
	int i,width,height;
	double ** tabe;

	width=can->width;height=can->height;
	tabe=(double **)ALLOCA((width+1)*sizeof(double *));
	for(i=0;i<width;i++)tabe[i]=(double *)ALLOCA((height+1)*sizeof(double));
	define_cursor(can->window,runningcur);
	set_busy(can); set_selection();
	calc(tabe,can,0);
	if_printOld(display,tabe,can);
	reset_selection(); reset_busy(can);
	define_cursor(can->window,normalcur);
}

int memory_plot(NODE arg,LIST *bytes){
	NODE n;
	struct canvas tmp_can;
	struct canvas *can;
	P formula;
	LIST xrange,yrange,zrange,wsize;
	int width,height;
	double **tabe;
	int i;
	BYTEARRAY barray;
	Q qw,qh;

	formula=(P)ARG0(arg);
	xrange=(LIST)ARG1(arg);
	yrange=(LIST)ARG2(arg);
	zrange=(LIST)ARG3(arg);
	wsize=(LIST)ARG4(arg);

	bzero((char *)&tmp_can,sizeof(tmp_can));
	can=&tmp_can;
	n=BDY(xrange); can->vx=VR((P)BDY(n)); n=NEXT(n);
	can->qxmin=(Q)BDY(n); n=NEXT(n); can->qxmax=(Q)BDY(n);
	can->xmin=ToReal(can->qxmin); can->xmax=ToReal(can->qxmax); 
	if( yrange ){
		n=BDY(yrange); can->vy=VR((P)BDY(n)); n=NEXT(n);
		can->qymin=(Q)BDY(n); n=NEXT(n); can->qymax=(Q)BDY(n);
		can->ymin=ToReal(can->qymin); can->ymax=ToReal(can->qymax); 
		if( zrange ){
			n=NEXT(BDY(zrange));
			can->zmin=ToReal(BDY(n)); n=NEXT(n); can->zmax=ToReal(BDY(n));
			if(n=NEXT(n)) can->nzstep=QTOS((Q)BDY(n));
			else can->nzstep=MAXGC;
			can->mode=MODE_CONPLOT;
		} else
			can->mode=MODE_IFPLOT;
	} else
		can->mode=MODE_PLOT;
	if( !wsize ){
		can->width=DEFAULTWIDTH; can->height=DEFAULTHEIGHT;
	} else {
		can->width=QTOS((Q)BDY(BDY(wsize)));
		can->height=QTOS((Q)BDY(NEXT(BDY(wsize))));
	}
	can->wname="";
	can->formula=formula; 
	if( can->mode == MODE_PLOT ){
		plotcalc(can);
		memory_print(can,&barray);
		STOQ(can->width,qw); STOQ(can->height,qh);
		n=mknode(3,qw,qh,barray);
		MKLIST(*bytes,n);
	} else {
		width=can->width; height=can->height;
		tabe=(double **)ALLOCA(width*sizeof(double *));
		for( i=0; i<width; i++ )
			tabe[i]=(double *)ALLOCA(height*sizeof(double));
		calc(tabe,can,1);
		memory_if_print(tabe,can,&barray);
		STOQ(width,qw); STOQ(height,qh);
		n=mknode(3,qw,qh,barray);
		MKLIST(*bytes,n);
	}
}

int plotover(NODE arg){
	int id,orgcolor,color;
	P formula;
	struct canvas *can;
	VL vl,vl0;

	id=QTOS((Q)ARG0(arg));
	formula=(P)ARG1(arg);
	color=QTOS((Q)ARG2(arg));

	can=canvas[id];
	orgcolor=can->color;
	can->color=color;
	get_vars_recursive((Obj)formula,&vl);
	for(vl0=vl;vl0;vl0=NEXT(vl0))
		if(vl0->v->attr==(pointer)V_IND)
			if(vl->v!=can->vx && vl->v!=can->vy)return -1;
	if(argc(arg)==3) can->color=QTOS((Q)ARG2(arg));
	else can->color=0;
#if !defined(VISUAL)
	set_drawcolor(can->color);
#endif
	current_can=can;
	can->formula=formula;
	if(can->mode==MODE_PLOT){
		plotcalc(can);
		plot_print(display,can);
	} else ifplotmainOld(can);
	copy_to_canvas(can);
	can->color=color;
#if !defined(VISUAL)
	set_drawcolor(can->color);
#endif
	return index;
}

int drawcircle(NODE arg){
#if !defined(VISUAL)
	int id,index,wx,wy,wr,c;
	pointer ptr;
	Q ret;
	LIST xyr;
	Obj x,y,r;
	struct canvas *can;

	index=QTOS((Q)ARG0(arg));
	xyr=(LIST)ARG1(arg); 
	x=(Obj)ARG0(BDY(xyr)); y=(Obj)ARG1(BDY(xyr)); r=(Obj)ARG2(BDY(xyr));
	c=QTOS((Q)ARG2(arg));
	can=canvas[index];
	if(!can->window)return -1;
	else {
		current_can=can;
		set_drawcolor(c);
		wx=(ToReal(x)-can->xmin)*can->width/(can->xmax-can->xmin);
		wy=(can->ymax-ToReal(y))*can->height/(can->ymax-can->ymin);
		wr=ToReal(r);
		XFillArc(display,can->pix,cdrawGC,wx-wr/2,wy-wr/2,wr,wr,0,360*64);
		copy_to_canvas(can);
		set_drawcolor(can->color);
		return index;
	}
#endif
}

int draw_obj(NODE arg){
	int index,color,x,y,u,v,len,r;
	NODE obj,n;
	RealVect *vect;
	struct canvas *can;

	index=QTOS((Q)ARG0(arg));
	can=canvas[index];
	if(!can && closed_canvas[index]){
		canvas[index]=closed_canvas[index];
		closed_canvas[index]=0;
		can=canvas[index];
		popup_canvas(index);
		current_can=can;
	} else if(!can||(can && !can->window)){
		set_lasterror("draw_obj : canvas does not exist");
		return -1;
	}

	obj=BDY((LIST)ARG1(arg));
	if(argc(arg)== 3) color=QTOS((Q)ARG2(arg));
	else color=0; // black
	switch(len=length(obj)){
		case 2: // point
			x=(int)ToReal((Q)ARG0(obj)); y=(int)ToReal((Q)ARG1(obj));
			draw_point(display,can,x,y,color);
			MKRVECT3(vect,x,y,color); MKNODE(n,vect,can->history);
			can->history=n;
			break;
		case 3: // circle
			x=(int)ToReal((Q)ARG0(obj)); y=(int)ToReal((Q)ARG1(obj));
			r=(int)ToReal((Q)ARG2(obj));
			MKRVECT4(vect,x,y,r,color); MKNODE(n,vect,can->history);
			can->history=n;
			break;
		case 4: // line
			x=(int)ToReal((Q)ARG0(obj)); y=(int)ToReal((Q)ARG1(obj));
			u=(int)ToReal((Q)ARG2(obj)); v=(int)ToReal((Q)ARG3(obj));
			draw_line(display,can,x,y,u,v,color);
			MKRVECT5(vect,x,y,u,v,color); MKNODE(n,vect,can->history);
			can->history=n;
			break;
		default:
			set_lasterror("draw_obj : invalid request");
			return -1;
	}
#if !defined(VISUAL)
	set_drawcolor(can->color);
#endif
	return 0;
}

int draw_string(NODE arg){
	int index,x,y,color;
	char *str;
	NODE pos;
	struct canvas *can;

	index=QTOS((Q)ARG0(arg));
	can=canvas[index];
	if(!can && closed_canvas[index]){
		canvas[index]=closed_canvas[index];
		closed_canvas[index]=0;
		can=canvas[index];
		popup_canvas(index);
		current_can=can;
	} else if(!can||(can && !can->window)){
		set_lasterror("draw_obj : canvas does not exist");
		return -1;
	}

	pos=BDY((LIST)ARG1(arg));
	str=BDY((STRING)ARG2(arg));
	if(argc(arg)==4)color=QTOS((Q)ARG3(arg));
	else color=0; // black
	x=(int)ToReal((Q)ARG0(pos));
	y=(int)ToReal((Q)ARG1(pos));
	draw_character_string(display,can,x,y,str,color);
#if !defined(VISUAL)
	set_drawcolor(can->color);
#endif
	return 0;
}

int clear_canvas(NODE arg){
	int index;
	struct canvas *can;

	index=QTOS((Q)ARG0(arg));
	can=canvas[index];
	if(!can||!can->window) return -1;
	clear_pixmap(can);
	copy_to_canvas(can);
	// clear the history
	can->history=0;
}

#define RealtoDbl(r) ((r)?BDY(r):0.0)

int arrayplot(NODE arg){
	int id,ix,w,h;
	VECT array;
	LIST xrange,wsize;
	char *wname;
	NODE n;
	double ymax,ymin,dy,xstep;
	Real *tab;
	struct canvas *can;
	POINT *pa;

	array=(VECT)ARG0(arg);
	xrange=(LIST)ARG1(arg);
	can=canvas[id=search_canvas()];
	n=BDY(xrange); can->vx=VR((P)BDY(n)); n=NEXT(n);
	can->qxmin=(Q)BDY(n); n=NEXT(n); can->qxmax=(Q)BDY(n);
	can->xmin=ToReal(can->qxmin); can->xmax=ToReal(can->qxmax); 
	if(!wsize){
		can->width=DEFAULTWIDTH;
		can->height=DEFAULTHEIGHT;
	} else {
		can->width=QTOS((Q)BDY(BDY(wsize)));
		can->height=QTOS((Q)BDY(NEXT(BDY(wsize))));
	}
	can->wname=wname; can->formula=0; can->mode=MODE_PLOT;
	create_canvas(can);
	w=array->len;
	h=can->height;
	tab=(Real *)BDY(array);
	if(can->ymax==can->ymin){
		for(ymax=ymin=RealtoDbl(tab[0]),ix=1; ix<w; ix++){
			if(RealtoDbl(tab[ix])>ymax)ymax=RealtoDbl(tab[ix]);
			if(RealtoDbl(tab[ix])<ymin)ymin=RealtoDbl(tab[ix]);
		}
		can->ymax=ymax;
		can->ymin=ymin;
	} else {
		ymax=can->ymax;
		ymin=can->ymin;
	}
	dy=ymax-ymin;
	can->pa=(struct pa *)MALLOC(sizeof(struct pa));
	can->pa[0].length=w;
	can->pa[0].pos=pa=(POINT *)MALLOC(w*sizeof(POINT));
	xstep=(double)can->width/(double)(w-1);
	for(ix=0;ix<w;ix++){
#ifndef MAXSHORT
#define MAXSHORT ((short)0x7fff)
#endif
		double t;

		pa[ix].x=(int)(ix*xstep); 
		t=(h - 1)*(ymax - RealtoDbl(tab[ix]))/dy;
		if(t>MAXSHORT)pa[ix].y=MAXSHORT;
		else if(t<-MAXSHORT)pa[ix].y=-MAXSHORT;
		else pa[ix].y=(long)t;
	}
	plot_print(display,can);
	copy_to_canvas(can);
	return id;
}

void ifplot_resize(struct canvas *can,POINT spos,POINT epos){
	struct canvas *ncan;
	struct canvas fakecan;
	Q dx,dy,dx2,dy2,xmin,xmax,ymin,ymax,xmid,ymid;
	Q sx,sy,ex,ey,cw,ch,ten,two;
	Q s,t;
	int new;
	int w,h,m;

	if(XC(spos)<XC(epos) && YC(spos)<YC(epos)){
		if(can->precise && !can->wide){
			fakecan=*can;
			ncan=&fakecan;
		} else {
			new=search_canvas();
			ncan=canvas[new];
		}
		ncan->mode=can->mode;
		ncan->zmin=can->zmin; ncan->zmax=can->zmax;
		ncan->nzstep=can->nzstep;
		ncan->wname=can->wname;
		ncan->vx=can->vx; ncan->vy=can->vy;
		ncan->formula=can->formula;
		w=XC(epos)-XC(spos);
		h=YC(epos)-YC(spos);
		m=MAX(can->width,can->height);
		if(can->precise){
			ncan->width=w;
			ncan->height=h;
		} else if(w>h){
			ncan->width=m;
			ncan->height=m*h/w;
		} else {
			ncan->width=m*w/h;
			ncan->height=m;
		}
		if(can->wide){
			STOQ(10,ten);
			STOQ(2,two);
			subq(can->qxmax,can->qxmin,&t);
			mulq(t,ten,&dx);
			subq(can->qymax,can->qymin,&t);
			mulq(t,ten,&dy);
			addq(can->qxmax,can->qxmin,&t);
			divq(t,two,&xmid);
			addq(can->qymax,can->qymin,&t);
			divq(t,two,&ymid);
			divq(dx,two,&dx2);
			divq(dy,two,&dy2);
			subq(xmid,dx2,&xmin);
			addq(xmid,dx2,&xmax);
			subq(ymid,dy2,&ymin);
			addq(ymid,dy2,&ymax);
		} else {
			subq(can->qxmax,can->qxmin,&dx);
			subq(can->qymax,can->qymin,&dy);
			xmin=can->qxmin;
			xmax=can->qxmax; 
			ymin=can->qymin;
			ymax=can->qymax;
		}
		STOQ(XC(spos),sx); STOQ(YC(spos),sy); STOQ(XC(epos),ex); STOQ(YC(epos),ey);
		STOQ(can->width,cw); STOQ(can->height,ch);
		mulq(sx,dx,&t); divq(t,cw,&s); addq(xmin,s,&ncan->qxmin);
		mulq(ex,dx,&t); divq(t,cw,&s); addq(xmin,s,&ncan->qxmax);
		mulq(ey,dy,&t); divq(t,ch,&s); subq(ymax,s,&ncan->qymin);
		mulq(sy,dy,&t); divq(t,ch,&s); subq(ymax,s,&ncan->qymax);
		ncan->xmin=ToReal(ncan->qxmin); ncan->xmax=ToReal(ncan->qxmax);
		ncan->ymin=ToReal(ncan->qymin); ncan->ymax=ToReal(ncan->qymax);
		if(can->precise && !can->wide){
			current_can=can;
			alloc_pixmap(ncan);
#if defined(VISUAL)
			ncan->real_can=can;
#endif
			qifplotmain(ncan);
			copy_subimage(ncan,can,spos);
			copy_to_canvas(can);
		} else {
			create_canvas(ncan);
			if( can->precise ) qifplotmain(ncan);
			else ifplotmain(ncan);
			copy_to_canvas(ncan);
		}
	}
}

void plot_resize(struct canvas *can,POINT spos,POINT epos){
	struct canvas *ncan;
	Q dx,dx2,xmin,xmax,xmid;
	double dy,ymin,ymax,ymid;
	Q sx,ex,cw,ten,two;
	Q s,t;
	int new;
	int w,h,m;

	if( XC(spos)<XC(epos) && YC(spos)<YC(epos) ){
		new=search_canvas(); ncan=canvas[new];
		ncan->mode=can->mode;
		ncan->zmin=can->zmin; ncan->zmax=can->zmax;
		ncan->nzstep=can->nzstep;
		ncan->wname=can->wname;
		ncan->vx=can->vx; ncan->vy=can->vy;
		ncan->formula=can->formula;
		w=XC(epos)-XC(spos);
		h=YC(epos)-YC(spos);
		m=MAX(can->width,can->height);
		if( w>h ){
			ncan->width=m; ncan->height=m * h/w;
		} else {
			ncan->width=m * w/h; ncan->height=m;
		}
		if( can->wide ){
			STOQ(10,ten); STOQ(2,two);
			subq(can->qxmax,can->qxmin,&t); mulq(t,ten,&dx);
			addq(can->qxmax,can->qxmin,&t); divq(t,two,&xmid);
			divq(dx,two,&dx2); subq(xmid,dx2,&xmin); addq(xmid,dx2,&xmax);

			dy=(can->ymax-can->ymin)*10;
			ymid=(can->ymax+can->ymin)/2;
			ymin=ymid-dy/2; ymax=ymid+dy/2;
		} else {
			subq(can->qxmax,can->qxmin,&dx); 
			xmin=can->qxmin; xmax=can->qxmax; 

			dy=can->ymax-can->ymin;
			ymin=can->ymin; ymax=can->ymax;
		}
		STOQ(XC(spos),sx); STOQ(XC(epos),ex); STOQ(can->width,cw); 
		mulq(sx,dx,&t); divq(t,cw,&s); addq(xmin,s,&ncan->qxmin);
		mulq(ex,dx,&t); divq(t,cw,&s); addq(xmin,s,&ncan->qxmax);
		ncan->xmin=ToReal(ncan->qxmin); ncan->xmax=ToReal(ncan->qxmax);

		ncan->ymin=ymax-YC(epos)*dy/can->height;
		ncan->ymax=ymax-YC(spos)*dy/can->height;

		create_canvas(ncan); 
		plotcalc(ncan);
		plot_print(display,ncan);
		copy_to_canvas(ncan);
	}
}

void qifplotmain(struct canvas *can)
{
	int width,height;
	char **tabe,*tabeb;
	int i;

	width=can->width; height=can->height;
	tabe=(char **)ALLOCA(width*sizeof(char *)+width*height*sizeof(char));
	bzero((void *)tabe,width*sizeof(char *)+width*height*sizeof(char));
	for( i=0, tabeb=(char *)(tabe+width); i<width; i++ )
		tabe[i]=tabeb + height*i;
	define_cursor(can->window,runningcur);
	set_busy(can); set_selection();
	qcalc(tabe,can); qif_print(display,tabe,can);
	reset_selection(); reset_busy(can);
	define_cursor(can->window,normalcur);
}

//*******************ifplotNG
int ifplotNG(NODE arg,char *func){
	int id,orgcolor,color,op_code;
	NODE n;
	struct canvas *can;
	P formula;
	LIST xrange,yrange,zrange,wsize;
	STRING wname;

	formula=(P)ARG0(arg);
	color=QTOS((Q)ARG1(arg));
	xrange=(LIST)ARG2(arg);
	yrange=(LIST)ARG3(arg);
	zrange=(LIST)ARG4(arg);
	wsize=(LIST)ARG5(arg);
	wname=(STRING)ARG6(arg);

	can=canvas[id=search_canvas()];
	orgcolor=can->color;
	can->color=color;
	can->division=0;
	// set canvas data
	if(xrange){
		n=BDY(xrange); can->vx=VR((P)BDY(n)); n=NEXT(n);
		can->qxmin=(Q)BDY(n); n=NEXT(n); can->qxmax=(Q)BDY(n);
		can->xmin=ToReal(can->qxmin); can->xmax=ToReal(can->qxmax);
	}
	if(yrange){
		n=BDY(yrange); can->vy=VR((P)BDY(n)); n=NEXT(n);
		can->qymin=(Q)BDY(n); n=NEXT(n); can->qymax=(Q)BDY(n);
		can->ymin=ToReal(can->qymin); can->ymax=ToReal(can->qymax); 
	}
	if(zrange){
		n=BDY(zrange); can->zmin=ToReal(BDY(n));
		n=NEXT(n); can->zmax=ToReal(BDY(n));
		n=NEXT(n); can->nzstep=QTOS((Q)BDY(n));
	}
	if(!wsize){
		can->width=DEFAULTWIDTH;
		can->height=DEFAULTHEIGHT;
	} else {
		can->width=QTOS((Q)BDY(BDY(wsize)));
		can->height=QTOS((Q)BDY(NEXT(BDY(wsize))));
	}
	if(wname) can->wname=BDY(wname);
	else can->wname="";
	can->formula=formula; 
	set_drawcolor(color);
	if(!strcmp(func,"ifplot"))can->mode=MODE_IFPLOT;
	else if(!strcmp(func,"ifplotD"))can->mode=MODE_IFPLOTD;
	else if(!strcmp(func,"ifplotQ"))can->mode=MODE_IFPLOTQ;
	else if(!strcmp(func,"ifplotB"))can->mode=MODE_IFPLOTB;
	else if(!strcmp(func,"ineqnD"))can->mode=MODE_INEQND;
	else if(!strcmp(func,"ineqnQ"))can->mode=MODE_INEQNQ;
	else if(!strcmp(func,"ineqnB"))can->mode=MODE_INEQNB;
	else if(!strcmp(func,"conplotD"))can->mode=MODE_CONPLOTD;
	else if(!strcmp(func,"conplotQ"))can->mode=MODE_CONPLOTQ;
	else if(!strcmp(func,"conplotB"))can->mode=MODE_CONPLOTB;
	else if(!strcmp(func,"itvifplot")){
		can->mode=MODE_ITVIFPLOT;
		can->division=QTOS((Q)ARG7(arg));
	}
	else can->mode=MODE_IFPLOTD;
	create_canvas(can);
	ifplotmain(can);
	set_drawcolor(orgcolor);
	copy_to_canvas(can);
	can->color=orgcolor;
	return id;
}

int ifplotOP(NODE arg,char *func){
	//ineqnor[D,Q,B],ineqnand[D,Q,B],ineqnxor[D,Q,b]
	int index,orgcolor,color,op_code;
	NODE n;
	struct canvas *can;
	P formula;
	//s_id,fname,w_id,poly,color
	index=QTOS((Q)ARG0(arg));
	formula=(P)ARG1(arg);
	color=QTOS((Q)ARG2(arg));
	can=canvas[index];
	orgcolor=can->color;
	can->color=color;
	// set canvas data
	can->formula=formula; 
	if(!strcmp(func,"ineqnandD"))can->mode=MODE_INEQNANDD;
	else if(!strcmp(func,"ineqnandQ"))can->mode=MODE_INEQNANDQ;
	else if(!strcmp(func,"ineqnandB"))can->mode=MODE_INEQNANDB;
	else if(!strcmp(func,"ineqnorD"))can->mode=MODE_INEQNORD;
	else if(!strcmp(func,"ineqnorQ"))can->mode=MODE_INEQNORQ;
	else if(!strcmp(func,"ineqnorB"))can->mode=MODE_INEQNORB;
	else if(!strcmp(func,"ineqnxorD"))can->mode=MODE_INEQNXORD;
	else if(!strcmp(func,"ineqnxorQ"))can->mode=MODE_INEQNXORQ;
	else if(!strcmp(func,"ineqnxorB"))can->mode=MODE_INEQNXORB;
	else if(!strcmp(func,"plotoverD"))can->mode=MODE_PLOTOVERD;
	else if(!strcmp(func,"plotoverQ"))can->mode=MODE_PLOTOVERQ;
	else if(!strcmp(func,"plotoverB"))can->mode=MODE_PLOTOVERB;
	else can->mode=MODE_IFPLOTD;
	set_drawcolor(color);
	ifplotmain(can);
	set_drawcolor(orgcolor);
	copy_to_canvas(can);
	can->color=orgcolor;
	return index;
}

void ifplotmain(struct canvas *can){
	int width,height,i,j,ix,iy,**mask;
	double **tabe;

	width=can->width; height=can->height;
	tabe=(double **)ALLOCA((width+1)*sizeof(double *));
	for(i=0;i<width;i++)tabe[i]=(double *)ALLOCA((height+1)*sizeof(double));
	define_cursor(can->window,runningcur);
	set_busy(can); set_selection();
	set_drawcolor(can->color);
	switch(can->mode){
	case MODE_IFPLOTD:
		calc(tabe,can,0);
		if_print(display,tabe,can,1);
		break;	
	case MODE_IFPLOTQ:
		calcq(tabe,can,0);
		if_print(display,tabe,can,1);
		break;	
	case MODE_IFPLOTB:
		calcb(tabe,can,0);
		if_print(display,tabe,can,0);
		break;	
#if defined(INTERVAL)
	case MODE_INEQND:
		calc(tabe,can,0);
		area_print(display,tabe,can,0);
		break;	
	case MODE_INEQNQ:
		calcq(tabe,can,0);
		area_print(display,tabe,can,0);
		break;	
	case MODE_INEQNB:
		calcb(tabe,can,0);
		area_print(display,tabe,can,0);
		break;	
	case MODE_INEQNANDD:
		calc(tabe,can,0);
		area_print(display,tabe,can,0);
		break;	
	case MODE_INEQNANDQ:
		calcq(tabe,can,0);
		area_print(display,tabe,can,2);
		break;	
	case MODE_INEQNANDB:
		calcb(tabe,can,0);
		area_print(display,tabe,can,2);
		break;	
	case MODE_INEQNORD:
		calc(tabe,can,0);
		area_print(display,tabe,can,3);
		break;	
	case MODE_INEQNORQ:
		calcq(tabe,can,0);
		area_print(display,tabe,can,3);
		break;	
	case MODE_INEQNORB:
		calcb(tabe,can,0);
		area_print(display,tabe,can,3);
		break;	
	case MODE_INEQNXORD:
		calc(tabe,can,0);
		area_print(display,tabe,can,4);
		break;	
	case MODE_INEQNXORQ:
		calcq(tabe,can,0);
		area_print(display,tabe,can,4);
		break;	
	case MODE_INEQNXORB:
		calcb(tabe,can,0);
		area_print(display,tabe,can,4);
		break;	
	case MODE_CONPLOTD:
		calc(tabe,can,0);
		con_print(display,tabe,can);
		break;
	case MODE_CONPLOTB:
		calcb(tabe,can,0);
		con_print(display,tabe,can);
		break;
	case MODE_CONPLOTQ:
		calcq(tabe,can,0);
		con_print(display,tabe,can);
		break;
	case MODE_PLOTOVERD:
		calc(tabe,can,0);
		over_print(display,tabe,can,0);
		break;
	case MODE_PLOTOVERQ:
		calcq(tabe,can,0);
		over_print(display,tabe,can,0);
		break;
	case MODE_PLOTOVERB:
		calcb(tabe,can,0);
		over_print(display,tabe,can,0);
		break;
	case MODE_ITVIFPLOT:
		itvcalc(tabe,can,1);
		if_print(display,tabe,can,1);
		break;
#endif
	}
	set_drawcolor(can->color);
	reset_selection(); reset_busy(can);
	define_cursor(can->window,normalcur);
}

#if defined(INTERVAL)
int objcp(NODE arg){
	int idsrc, idtrg, op_code;
	struct canvas *cansrc, *cantrg;

	idsrc=QTOS((Q)ARG0(arg));
	idtrg=QTOS((Q)ARG1(arg));
	op_code=QTOS((Q)ARG2(arg));
	cansrc=canvas[idsrc];
	cantrg=canvas[idtrg];
	obj_op(cansrc, cantrg, op_code);
	return idsrc;
}

void obj_op(struct canvas *cansrc, struct canvas *cantrg, int op){
	XImage *imgsrc, *imgtrg;
	int width, height, i, j;
	unsigned long src, trg, black, white;

	width=cansrc->width; height=cansrc->height;
	imgsrc=XGetImage(display, cansrc->pix, 0, 0, width, height, -1, ZPixmap);
	imgtrg=XGetImage(display, cantrg->pix, 0, 0, width, height, -1, ZPixmap);
	black=GetColor(display, "black");
	white=GetColor(display, "white");
	flush();
	define_cursor(cantrg->window,runningcur);
	set_busy(cantrg); set_selection();
	cantrg->precise=cansrc->precise;
	cantrg->noaxis=cansrc->noaxis;
	cantrg->noaxisb=cansrc->noaxisb;
	cantrg->vx=cansrc->vx;
	cantrg->vy=cansrc->vy;
	cantrg->formula=cansrc->formula;
	cantrg->width=cansrc->width;
	cantrg->height=cansrc->height;
	cantrg->xmin=cansrc->xmin;
	cantrg->xmax=cansrc->xmax;
	cantrg->ymin=cansrc->ymin;
	cantrg->ymax=cansrc->ymax;
	cantrg->zmin=cansrc->zmin;
	cantrg->zmax=cansrc->zmax;
	cantrg->nzstep=cansrc->nzstep;
	cantrg->qxmin=cansrc->qxmin;
	cantrg->qxmax=cansrc->qxmax;
	cantrg->qymin=cansrc->qymin;
	cantrg->qymax=cansrc->qymax;
	cantrg->pa=cansrc->pa;
	switch(op){
		case 1:/* and case */
			for(i=0;i<width;i++)for(j=0;j<height;j++){
				src=XGetPixel(imgsrc,i,j);
				trg=XGetPixel(imgtrg,i,j);
				if( (src == black) || (trg == black) )
					XPutPixel(imgtrg,i,j,black);
				else if( (src == white) || (trg == white) )
					XPutPixel(imgtrg,i,j,white);
				else XPutPixel(imgtrg,i,j,(src & trg));
			}
			break;
		case 3:/* copy case */
			imgtrg->data=imgsrc->data;
			break;
		case 6:/* xor case */
			for(i=0;i<width;i++)for(j=0;j<height;j++){
				src=XGetPixel(imgsrc,i,j);
				trg=XGetPixel(imgtrg,i,j);
				if( (src == black) || (trg == black) )
					XPutPixel(imgtrg,i,j,black);
				else if( (src == white) && (trg == white) )
					XPutPixel(imgtrg,i,j,trg|src);
				else if( (src != white) && (trg != white) )
					XPutPixel(imgtrg,i,j,white);
				else if( src == white )
					XPutPixel(imgtrg,i,j,src);
			}
			break;
		case 7:/* or case */
			for(i=0;i<width;i++)for(j=0;j<height;j++){
				src=XGetPixel(imgsrc,i,j);
				trg=XGetPixel(imgtrg,i,j);
				if( (src == black) || (trg == black) )
					XPutPixel(imgtrg,i,j,black);
				else if(src == white)
					XPutPixel(imgtrg,i,j,trg);
				else if(trg == white)
					XPutPixel(imgtrg,i,j,src);
			}
			break;
		default:
			break;
	}
	XPutImage(display, cantrg->pix, drawGC, imgtrg, 0, 0, 0, 0, width, height);
	reset_selection(); reset_busy(cantrg);
	define_cursor(cantrg->window,normalcur);
	copy_to_canvas(cantrg);
	count_and_flush();
	flush();
}

//ifplotNG
/*
int ineqnover(NODE arg)
{
  int id;
  struct canvas *can;
  int orgcolor, op_code;

  id = QTOS((Q)ARG0(arg));
  can = canvas[id];
  orgcolor = can->color;
  can->formula = (P)ARG1(arg);
  can->color   = QTOS((Q)ARG2(arg));
  op_code      = QTOS((Q)ARG3(arg));
  can->mode    = MODE_INEQNP;
  ineqnmain(can, orgcolor, op_code);
  return id;
}
void ineqnmain(struct canvas *can, int orgcolor, int op_code)
{
  int **mask;
  double **tbl;
  int i,j;

  current_can = can;
  tbl = (double **)ALLOCA((can->height+1)*sizeof(double *));
  for ( i = 0; i <= can->height; i++ )
    tbl[i] = (double *)ALLOCA((can->width+1)*sizeof(double));
  mask = (int **)ALLOCA(can->height*sizeof(int *));
  for ( i = 0; i < can->height; i++)
    mask[i] = (int *)ALLOCA(can->width*sizeof(int));

  define_cursor(can->window,runningcur);
  set_busy(can); set_selection();
//  ineqncalc(tbl, can, 1);
  calc(tab,can,1);
  for (j = 0; j < can->height; j++){
    for (i = 0; i < can->width; i++){
      if ( tbl[j][i] >= 0 ){
        if ( (tbl[j+1][i] <= 0 ) ||
          (tbl[j][i+1] <= 0) ||
          (tbl[j+1][i+1] <= 0) ) mask[j][i] = 0;
        else mask[j][i] = 1;
      } else {
        if( (tbl[j+1][i] >= 0) ||
          (tbl[j][i+1] >= 0) ||
          (tbl[j+1][i+1] >= 0) ) mask[j][i] = 0;
        else mask[j][i] = -1;
      }
    }
  }
  area_print(display, mask, can, op_code);
  reset_selection();
  reset_busy(can);
  define_cursor(can->window,normalcur);
#if !defined(VISUAL)
  set_drawcolor(orgcolor);
  can->color = orgcolor;
#endif
  copy_to_canvas(can);
}
*/
int polarplotNG(NODE arg){
	int id,color,orgcolor;
	NODE n;
	struct canvas *can;
	P formula;
	LIST xrange,wsize;
	STRING wname;
	V v;

	formula=(P)ARG0(arg);
	xrange=(LIST)ARG1(arg);
	color=QTOS((Q)ARG2(arg));
	wsize=(LIST)ARG3(arg);
	wname=(STRING)ARG4(arg);

	can=canvas[id=search_canvas()];
	if(xrange){
		n=BDY(xrange); can->vx=VR((P)BDY(n)); n=NEXT(n);
		can->qxmin=(Q)BDY(n); n=NEXT(n); can->qxmax=(Q)BDY(n);
		can->xmin=ToReal(can->qxmin); can->xmax=ToReal(can->qxmax); 
	}
	can->mode=MODE_POLARPLOT;
	if(!wsize){
		can->width=DEFAULTWIDTH;
		can->height=DEFAULTHEIGHT;
	} else {
		can->width=QTOS((Q)BDY(BDY(wsize)));
		can->height=QTOS((Q)BDY(NEXT(BDY(wsize))));
	}
	if(wname)can->wname=BDY(wname);
	else can->wname="";
	can->formula=formula; 
	orgcolor=can->color;
	can->color=color;
	polarplotcalc(can);
	create_canvas(can);
	plot_print(display,can);
	can->color=orgcolor;
	copy_to_canvas(can);
	return id;
}

int plotoverD(NODE arg){
	int index, color;
	P formula;
	struct canvas *can;
	VL vl,vl0;

	index=QTOS((Q)ARG0(arg));
	formula=(P)ARG1(arg);
	can=canvas[index];
	color=can->color;
	if(!can->window)return -1;
	get_vars_recursive((Obj)formula,&vl);
	for(vl0=vl;vl0;vl0=NEXT(vl0))
		if(vl0->v->attr==(pointer)V_IND)
			if(vl->v!=can->vx && vl->v!=can->vy) return -1;
	if(argc(arg)== 3)can->color=QTOS((Q)ARG2(arg));
	else can->color=0;
#if !defined(VISUAL)
	set_drawcolor(can->color);
#endif
	current_can=can;
	can->formula=formula;
	if(can->mode==MODE_PLOT){
		calcq(can);
		plot_print(display,can);
	} else ifplotmain(can);
	copy_to_canvas(can);
	can->color=color;
#if !defined(VISUAL)
	set_drawcolor(can->color);
#endif
	return index;
}
#endif
