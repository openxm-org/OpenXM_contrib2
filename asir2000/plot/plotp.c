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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/plotp.c,v 1.2 2000/03/07 01:32:44 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"
#include <math.h>

#if defined(sun) && !defined(__svr4__)
#define EXP10(a) exp10(a)
#else
#define EXP10(a) pow(10.0,a)
#endif

if_print(display,tab,can)
DISPLAY *display;
double **tab;
struct canvas *can;
{
	int ix,iy,width,height;
	double *px,*px1,*px2;
	DRAWABLE pix;

	if ( can->mode == MODE_CONPLOT ) {
		con_print(display,tab,can); return;
	}
	flush();
	width = can->width; height = can->height; pix = can->pix;
	for( ix=0; ix<width-1; ix++ )
		for(iy=0, px=tab[ix], px1 = tab[ix+1], px2 = px+1;
			iy<height-1 ;iy++, px++, px1++, px2++ )
			if ( ((*px >= 0) && ((*px1 <= 0) || (*px2 <= 0))) ||
				 ((*px <= 0) && ((*px1 >= 0) || (*px2 >= 0))) ) {
				DRAWPOINT(display,pix,drawGC,ix,height-iy-1);
				count_and_flush();
			}
	flush();
}

con_print(display,tab,can)
DISPLAY *display;
double **tab;
struct canvas *can;
{
	int ix,iy,iz,width,height,pas,pai,len;
	double zstep,z;
	double *px,*px1,*px2;
	DRAWABLE pix;
	POINT *pa,*pa1;
	struct pa *parray;

	
	width = can->width; height = can->height; pix = can->pix;
	zstep = (can->zmax-can->zmin)/can->nzstep;
	can->pa = (struct pa *)MALLOC((can->nzstep+1)*sizeof(struct pa));
	pas = width;
	pa = (POINT *)ALLOCA(pas*sizeof(POINT));
	initmarker(can,"Drawing...");
	for ( z = can->zmin, iz = 0; z <= can->zmax; z += zstep, iz++ ) {
		marker(can,DIR_Z,iz);
		pai = 0;	
		for( ix=0; ix<width-1; ix++ )
			for(iy=0, px=tab[ix], px1 = tab[ix+1], px2 = px+1;
				iy<height-1 ;iy++, px++, px1++, px2++ )
				if ( ((*px >= z) && ((*px1 <= z) || (*px2 <= z))) ||
				 	 ((*px <= z) && ((*px1 >= z) || (*px2 >= z))) ) {
					if ( pai == pas ) {
						pa1 = (POINT *)ALLOCA(2*pas*sizeof(POINT));
						bcopy((char *)pa,(char *)pa1,pas*sizeof(POINT)); pa = pa1;
						pas += pas;
					}
					XC(pa[pai]) = ix; YC(pa[pai]) = height-iy-1; pai++;
				}
		can->pa[iz].length = pai;
		if ( pai ) {
			pa1 = (POINT *)MALLOC(pai*sizeof(POINT));
			bcopy((char *)pa,(char *)pa1,pai*sizeof(POINT));
			can->pa[iz].pos = pa1;
		}
	}
	flush();
	for ( parray = can->pa, iz = 0; iz <= can->nzstep; iz++, parray++ )
		for ( pa = parray->pos, len = parray->length, ix = 0; ix < len; ix++ ) {
			DRAWPOINT(display,pix,drawGC,XC(pa[ix]),YC(pa[ix]));
			count_and_flush();
		}
	flush();
}

qif_print(display,tab,can)
DISPLAY *display;
char **tab;
struct canvas *can;
{
	int ix,iy,width,height;
	char *px;
	DRAWABLE pix;

	flush();
	width = can->width; height = can->height; pix = can->pix;
	for( ix = 0; ix < width; ix++ )
		for(iy = 0, px = tab[ix]; iy < height ;iy++, px++ )
			if ( *px ) {
				DRAWPOINT(display,pix,drawGC,ix,height-iy-1);
				count_and_flush();
			}
	flush();
}

plot_print(display,can)
DISPLAY *display;
struct canvas *can;
{
	int len,i;
	POINT *pa;
	
	XDrawLines(display,can->pix,drawGC,
		can->pa[0].pos,can->pa[0].length,CoordModeOrigin);
	XFlush(display);
}

#define D 5

pline(display,can,d)
DISPLAY *display;
struct canvas *can;
DRAWABLE d;
{
	double w,w1,k,e,n;
	int x0,y0,x,y,xadj,yadj;
	char buf[BUFSIZ];
	double adjust_scale();

	if ( can->noaxis )
		return;

	xadj = yadj = 0;
	if ( (can->xmin < 0) && (can->xmax > 0) ) {
		x0 = (int)((can->width-1)*(-can->xmin/(can->xmax-can->xmin)));
		DRAWLINE(display,d,dashGC,x0,0,x0,can->height);
	} else if ( can->xmin >= 0 )
		x0 = 0;
	else
		x0 = can->width-1-D;
	if ( (can->ymin < 0) && (can->ymax > 0) ) {
		y0 = (int)((can->height-1)*(can->ymax/(can->ymax-can->ymin)));
		DRAWLINE(display,d,dashGC,0,y0,can->width,y0);
	} else if ( can->ymin >= 0 )
		y0 = can->height-1;
	else
		y0 = D;
	w = can->xmax-can->xmin; 
	w1 = w * DEFAULTWIDTH/can->width;
	e = adjust_scale(EXP10(floor(log10(w1))),w1);
	for ( n = ceil(can->xmin/e); n*e<= can->xmax; n++ ) {
		x = (int)can->width*(n*e-can->xmin)/w;
		DRAWLINE(display,d,drawGC,x,y0,x,y0-D);
		sprintf(buf,"%g",n*e);
		DRAWSTRING(display,d,scaleGC,x+2,y0,buf,strlen(buf));
	}
	w = can->ymax-can->ymin;
	w1 = w * DEFAULTHEIGHT/can->height;
	e = adjust_scale(EXP10(floor(log10(w1))),w1);
	for ( n = ceil(can->ymin/e); n*e<= can->ymax; n++ ) {
		y = (int)can->height*(1-(n*e-can->ymin)/w);
		DRAWLINE(display,d,drawGC,x0,y,x0+D,y);
		sprintf(buf,"%g",n*e);
		if ( can->xmax <= 0 )
			xadj = TEXTWIDTH(sffs,buf,strlen(buf));
		DRAWSTRING(display,d,scaleGC,x0-xadj,y,buf,strlen(buf));
	}
}

double adjust_scale(e,w)
double e,w;
{
	switch ( (int)floor(w/e) ) {
		case 1:
			return e/4; break;
		case 2: case 3:
			return e/2; break;
		case 4: case 5: case 6: case 7:
			return e; break;
			break;
		case 8: case 9: case 10: default:
			return 2*e; break;
	}
}

initmarker(can,message)
struct canvas *can;
char *message;
{
	XawScrollbarSetThumb(can->xdone,0.0,0.0);
	XawScrollbarSetThumb(can->ydone,1.0,0.0);
	XFlush(display);
}

marker(can,dir,p)
struct canvas *can;
int dir,p;
{
	if ( dir == DIR_X ) {
		XawScrollbarSetThumb(can->xdone,(float)p/(float)can->width,0.05);
		count_and_flush();
	} else if ( dir == DIR_Y ) {
		XawScrollbarSetThumb(can->ydone,1.0-(float)p/(float)can->height,0.05);
		count_and_flush();
	} else {
		XawScrollbarSetThumb(can->ydone,1.0-(float)p/(float)can->nzstep,0.05);
		flush();
	}
}

define_cursor(w,cur)
WINDOW w;
CURSOR cur;
{
	XDefineCursor(display,w,cur); flush();
}

static int flush_count;
#if defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__)
#define MAX_COUNT 64
#else
#define MAX_COUNT 32
#endif

count_and_flush() {
	if ( ++flush_count == MAX_COUNT )
		flush();
}

flush() {
	flush_count = 0;
	XFlush(display);
}
