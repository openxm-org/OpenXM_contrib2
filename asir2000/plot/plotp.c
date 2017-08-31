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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/plotp.c,v 1.23 2015/08/14 13:51:56 fujimoto Exp $ 
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

#if defined(VISUAL) || defined(__MINGW32__)
static POINT oldpos;
#endif

#if !defined(VISUAL) && !defined(__MINGW32__)
extern Pixel BackPixel;
unsigned long GetColor(Display *, char *);

unsigned long GetColor(Display *dis, char *color_name)
{
	Colormap cmap;
	XColor near_color, true_color;

	cmap = DefaultColormap( dis, 0 );
	XAllocNamedColor( dis, cmap, color_name, &near_color, &true_color );
	return( near_color.pixel );
}

void area_print(DISPLAY *display,double **tab,struct canvas *can,int GXcode){
	int ix,iy,width,height,wc,**mask;
	XImage *image;
	DRAWABLE pix;
	unsigned int color,black,white,c2;
	//unsigned long color,black,white,c2;
	double *px,*px1,*px2;
	//GXcode 0:new 1:cp 2:and 3:or 4:xor
	width=can->width;
	height=can->height;
	pix=can->pix;
	mask=(int **)ALLOCA((width)*sizeof(int *));
	for(ix=0;ix<width;ix++)mask[ix]=(int *)ALLOCA((height)*sizeof(int));
	// create mask table values boundary:0 positive:1 negativ:-1
	for(ix=0;ix<width;ix++)for(iy=0;iy<height;iy++)
		if((tab[ix][iy]>0)&&(tab[ix+1][iy]>0)&&(tab[ix][iy+1]>0)&&
			(tab[ix+1][iy+1]>0)) mask[ix][iy]=1;//all postive
		else if((tab[ix][iy]<0)&&(tab[ix+1][iy]<0)&&(tab[ix][iy+1]<0)&&
			(tab[ix+1][iy+1]<0)) mask[ix][iy]=-1;//all negativ
		else mask[ix][iy]=0;//boundary
	flush();
	black=GetColor(display,"black");
	white=GetColor(display,"white");
	set_drawcolor(can->color);
	color=can->color;
	image=XGetImage(display,pix,0,0,width,height,-1,ZPixmap);
	for(ix=0;ix<width;ix++){
		for(iy=0;iy<height;iy++){
			c2=XGetPixel(image,ix,height-iy-1);
			if(c2==white)wc=-1;
			else if(c2==black)wc=0;
			else wc=1;
			if(wc!=0){//XPutPixel(image,ix,iy,black);
				if(mask[ix][iy]==0)XPutPixel(image,ix,height-iy-1,black);//boundary
				else {
					switch (GXcode){
					case 0: //new window
					case 1: //cp
						if(mask[ix][iy]==1)XPutPixel(image,ix,height-iy-1,color);
						else XPutPixel(image,ix,height-iy-1,BackPixel);
						break;
					case 2: //and
						if((mask[ix][iy]==1)&&(wc==1))XPutPixel(image,ix,height-iy-1,color);
						else XPutPixel(image,ix,height-iy-1,BackPixel);
						break;
					case 3: //or
						if((mask[ix][iy]==1)||(wc==1))XPutPixel(image,ix,height-iy-1,color);
						else XPutPixel(image,ix,height-iy-1,BackPixel);
						break;
					case 4: //xor
						if((mask[ix][iy]==1)^(wc==1))XPutPixel(image,ix,height-iy-1,color);
						else XPutPixel(image,ix,height-iy-1,BackPixel);
						break;
					}
				}
			}
		}
	}
	XPutImage(display,pix,drawGC,image,0,0,0,0,width,height);
	count_and_flush();
	flush();
}
#else
void area_print(DISPLAY *display,double **tab,struct canvas *can,int GXcode){
    /* not implemented */
}
#endif

void over_print(DISPLAY *display,double **tab,struct canvas *can,int GXcode){
	int ix,iy,width,height;
	DRAWABLE pix;
	double vmin,vmax;
	//GXcode 0:over 1:cp 2:and 3:or 4:xor
	pix=can->pix; width=can->width; height=can->height;
	for(ix=0;ix<width;ix++){
		for(iy=0;iy<height;iy++){
			vmax=
			MAX(MAX(tab[ix][iy],tab[ix][iy+1]),MAX(tab[ix+1][iy],tab[ix+1][iy+1]));
			vmin=
			MIN(MIN(tab[ix][iy],tab[ix][iy+1]),MIN(tab[ix+1][iy],tab[ix+1][iy+1]));
			if(vmin<=0.0 && vmax>=0.0) DRAWPOINT(display,pix,cdrawGC,ix,height-iy-1);
		}
		count_and_flush();
	}
	flush();
}

void if_printNG(DISPLAY *display,double **tab,struct canvas *can,int cond){
	int ix,iy,width,height;
	double zst,zed,zsp;
	DRAWABLE pix;
	width=can->width; height=can->height; pix=can->pix;
	if(cond==0){
		//IFPLOTB
		for(iy=0;iy<height-1;iy++)for(ix=0;ix<width-1;ix++)
			if(tab[ix][iy]==0.0) DRAWPOINT(display,pix,cdrawGC,ix,height-iy-1);
	} else if(cond==1) {
		//IFPLOT,IFPLOTD,IFPLOTQ
		for(iy=0;iy<height-1;iy++)for(ix=0;ix<width-1;ix++){
			if(tab[ix][iy]==0.0) DRAWPOINT(display,pix,cdrawGC,ix,height-iy-1);
			else if(tab[ix][iy]>0){
				if(tab[ix+1][iy+1]<0||tab[ix][iy+1]<0||tab[ix+1][iy]<0)
					DRAWPOINT(display,pix,cdrawGC,ix,height-iy-1);
			} else if(tab[ix+1][iy+1]>0||tab[ix][iy+1]>0||tab[ix+1][iy]>0)
				DRAWPOINT(display,pix,cdrawGC,ix,height-iy-1);
			count_and_flush();
		}
	}
	flush();
}

void polar_print(DISPLAY *display,struct canvas *can){
	int len,i,j,x,y;
	unsigned int color;
	POINT *pa;

#if defined(VISUAL) || defined(__MINGW32__)
	HDC dc;
	HPEN pen,oldpen;
	len=can->pa[0].length;
	color=can->color;
	pa=can->pa[0].pos;
	for(i=1;i<len;i++){
		j=i-1;
		if(color){
			pen=CreatePen(PS_SOLID,1,color);
			oldpen=SelectObject(can->pix,pen);
			DRAWLINE(display,can->pix,drawGC,XC(pa[j]),YC(pa[j]),XC(pa[i]),YC(pa[i]));
			SelectObject(can->pix,oldpen);

			dc = GetDC(can->hwnd);
			oldpen = SelectObject(dc,pen);
			DRAWLINE(display,dc,drawGC,XC(pa[j]),YC(pa[j]),XC(pa[i]),YC(pa[i]));
			SelectObject(dc,oldpen);
			ReleaseDC(can->hwnd,dc);

			DeleteObject(pen);
		} else {
			DRAWLINE(display,can->pix,drawGC,XC(pa[j]),YC(pa[j]),XC(pa[i]),YC(pa[i]));
			dc = GetDC(can->hwnd);
			DRAWLINE(display,dc,drawGC,XC(pa[j]),YC(pa[j]),XC(pa[i]),YC(pa[i]));
			ReleaseDC(can->hwnd,dc);
		}
	}
#else
	len=can->pa[0].length;
	color=can->color;
	pa=can->pa[0].pos;
	for(i=1;i<len;i++){
		j=i-1;
		DRAWLINE(display,can->pix,cdrawGC,XC(pa[j]),YC(pa[j]),XC(pa[i]),YC(pa[i]));
	}
	XFlush(display);
#endif
}


void if_print(DISPLAY *display,double **tab,struct canvas *can){
	int ix,iy,width,height;
	double *px,*px1,*px2;
	DRAWABLE pix;

	if ( can->mode == modeNO(CONPLOT) ) {
		con_print(display,tab,can); return;
	}
	flush();
	width = can->width; height = can->height; pix = can->pix;
	for( ix=0; ix<width-1; ix++ )
		for(iy=0, px=tab[ix], px1 = tab[ix+1], px2 = px+1;
			iy<height-1 ;iy++, px++, px1++, px2++ )
			if ( ((*px >= 0) && ((*px1 <= 0) || (*px2 <= 0))) ||
				((*px <= 0) && ((*px1 >= 0) || (*px2 >= 0))) ) {
		DRAWPOINT(display,pix,cdrawGC,ix,height-iy-1);
		count_and_flush();
	}
	flush();
/*
	int i,ix,iy,width,height;
	double *px,*px1,*px2;
	double **vmax,**vmin,*zst,zstep,zv,u,l;
	DRAWABLE pix;
	POINT *pa,*pa1;
	struct pa *parray;

	if(can->mode==modeNO(CONPLOT)){
		width=can->width;height=can->height;pix=can->pix;
		//con_print(display,tab,can);
	  // calc all cell max,min value
		vmax=((double **)ALLOCA((width+1)*sizeof(double *)));
		vmin=((double **)ALLOCA((width+1)*sizeof(double *)));
		for(i=0;i<width;i++){
			vmax[i]=(double *)ALLOCA((height+1)*sizeof(double));
			vmin[i]=(double *)ALLOCA((height+1)*sizeof(double));
		}
		for(ix=0;ix<width;ix++){
			for(iy=0;iy<height;iy++){
				vmax[ix][iy]=
				MAX(MAX(tab[ix][iy],tab[ix][iy+1]),MAX(tab[ix+1][iy],tab[ix+1][iy+1]));
				vmin[ix][iy]=
				MIN(MIN(tab[ix][iy],tab[ix][iy+1]),MIN(tab[ix+1][iy],tab[ix+1][iy+1]));
			}
		}
		if(can->zmax==can->zmin)zstep=(can->vmax-can->vmin)/can->nzstep;
		else zstep=(can->zmax-can->zmin)/can->nzstep;
		zst=(double *)ALLOCA((can->nzstep+1)*sizeof(double));
		zv=can->xmin;
		for(i=0,zv=can->xmin;i<can->nzstep;zv+=zstep,i++)zst[i]=zv;
		for(iy=0;iy<height-1;iy++){
			for(ix=0;ix<width-1;ix++){
				for(i=0;i<can->nzstep;i++){
					if(vmin[ix][iy]<=zst[i] && vmax[ix][iy]>=zst[i]){
						DRAWPOINT(display,pix,cdrawGC,ix,height-iy-1);
						break;
					}
					count_and_flush();
				}
			}
		}
	} else {
		width=can->width;height=can->height;pix=can->pix;
		for(ix=0;ix<width-1;ix++)
		for(iy=0,px=tab[ix],px1=tab[ix+1],px2=px+1;iy<height-1;
			iy++,px++,px1++,px2++)
			if(((*px>=0)&&((*px1<=0)||(*px2<=0)))||
				((*px<=0)&&((*px1>=0)||(*px2>=0)))){
				DRAWPOINT(display,pix,cdrawGC,ix,height-iy-1);
				count_and_flush();
			}
	}
	flush();
*/
}

#define MEMORY_DRAWPOINT(a,len,x,y) (((a)[(len)*(y)+((x)>>3)]) |= (1<<((x)&7)))

void memory_if_print(double **tab,struct canvas *can,BYTEARRAY *bytes){
	int ix,iy,width,height;
	double *px,*px1,*px2;
	unsigned char *array;
	int scan_len;
	if ( can->mode==modeNO(CONPLOT)){
		memory_con_print(tab,can,bytes);
		return;
	}
	width = can->width; height = can->height;
	// scan_len = byte length of the scan line
	scan_len = (width+7)/8;
	MKBYTEARRAY(*bytes,scan_len*height);
	array = BDY(*bytes);
	for( ix=0; ix<width-1; ix++ )
		for(iy=0, px=tab[ix], px1 = tab[ix+1], px2 = px+1;
			iy<height-1 ;iy++, px++, px1++, px2++ )
			if ( ((*px >= 0) && ((*px1 <= 0) || (*px2 <= 0))) ||
				 ((*px <= 0) && ((*px1 >= 0) || (*px2 >= 0))) ) {
				MEMORY_DRAWPOINT(array,scan_len,ix,height-iy-1);
			}
}

void con_print(DISPLAY *display,double **tab,struct canvas *can){
	int i,ix,iy,width,height;
	double *px,*px1,*px2;
	double **vmax,**vmin,*zst,zstep,zv,u,l;
	DRAWABLE pix;
	POINT *pa,*pa1;
	struct pa *parray;

	width=can->width;height=can->height;pix=can->pix;
	vmax=((double **)ALLOCA((width+1)*sizeof(double *)));
	vmin=((double **)ALLOCA((width+1)*sizeof(double *)));
	for(i=0;i<width;i++){
		vmax[i]=(double *)ALLOCA((height+1)*sizeof(double));
		vmin[i]=(double *)ALLOCA((height+1)*sizeof(double));
	}
	for(ix=0;ix<width;ix++){
		for(iy=0;iy<height;iy++){
			vmax[ix][iy]=
			MAX(MAX(tab[ix][iy],tab[ix][iy+1]),MAX(tab[ix+1][iy],tab[ix+1][iy+1]));
			vmin[ix][iy]=
			MIN(MIN(tab[ix][iy],tab[ix][iy+1]),MIN(tab[ix+1][iy],tab[ix+1][iy+1]));
		}
	}
	if(can->zmax==can->zmin)zstep=(can->vmax-can->vmin)/can->nzstep;
	else zstep=(can->zmax-can->zmin)/can->nzstep;
	zst=(double *)ALLOCA((can->nzstep+1)*sizeof(double));
	zv=can->xmin;
	for(i=0,zv=can->xmin;i<can->nzstep;zv+=zstep,i++)zst[i]=zv;
	for(iy=0;iy<height-1;iy++){
		for(ix=0;ix<width-1;ix++){
			for(i=0;i<can->nzstep;i++){
				if(vmin[ix][iy]<=zst[i] && vmax[ix][iy]>=zst[i]){
					DRAWPOINT(display,pix,cdrawGC,ix,height-iy-1);
					break;
				}
				count_and_flush();
			}
		}
	}
	flush();
}

void memory_con_print(double **tab,struct canvas *can,BYTEARRAY *bytes){
	int ix,iy,iz,width,height,pas,pai,len;
	double zstep,z;
	double *px,*px1,*px2;
	POINT *pa,*pa1;
	struct pa *parray;
	unsigned char *array;
	int scan_len;

	width = can->width; height = can->height;
	zstep = (can->zmax-can->zmin)/can->nzstep;
	can->pa = (struct pa *)MALLOC((can->nzstep+1)*sizeof(struct pa));
	pas = width;
	pa = (POINT *)ALLOCA(pas*sizeof(POINT));
	for ( z = can->zmin, iz = 0; z <= can->zmax; z += zstep, iz++ ) {
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
	/* scan_len = byte length of the scan line */
	scan_len = (width+7)/8;
	MKBYTEARRAY(*bytes,scan_len*height);
	array = BDY(*bytes);
	for ( parray = can->pa, iz = 0; iz <= can->nzstep; iz++, parray++ )
		for ( pa = parray->pos, len = parray->length, ix = 0; ix < len; ix++ ) {
			MEMORY_DRAWPOINT(array,scan_len,XC(pa[ix]),YC(pa[ix]));
		}
}

void memory_print(struct canvas *can,BYTEARRAY *bytes){
	int len,scan_len,i;
	POINT *pa;
	char *array;

	/* scan_len = byte length of the scan line */
	scan_len = (can->width+7)/8;
	MKBYTEARRAY(*bytes,scan_len*can->height);
	array = (char*)BDY(*bytes);

	len = can->pa[0].length;
	pa = can->pa[0].pos;
	for ( i = 0; i < len; i++ ) {
		MEMORY_DRAWPOINT(array,scan_len,pa[i].x,pa[i].y);
	}
}

void qif_print(DISPLAY *display,char **tab,struct canvas *can){
	int ix,iy,width,height;
	char *px;
	DRAWABLE pix;

	flush();
	width = can->width; height = can->height; pix = can->pix;
	for( ix = 0; ix < width; ix++ )
		for(iy = 0, px = tab[ix]; iy < height ;iy++, px++ )
			if ( *px ) {
				DRAWPOINT(display,pix,cdrawGC,ix,height-iy-1);
				count_and_flush();
			}
	flush();
}

void plot_print(DISPLAY *display,struct canvas *can){
	int len;
	POINT *pa;

#if defined(VISUAL) || defined(__MINGW32__)	
	len = can->pa[0].length;
	pa = can->pa[0].pos;

	Polyline(can->pix,pa,len);
//	for ( i = 0; i < len-1; i++ ) {
//		DRAWLINE(display,can->pix,drawGC,pa[i].x,pa[i].y,pa[i+1].x,pa[i+1].y);
//	}
#else
	XDrawLines(display,can->pix,drawGC,
		can->pa[0].pos,can->pa[0].length,CoordModeOrigin);
	XFlush(display);
#endif
}

void draw_point(DISPLAY *display,struct canvas *can,int x,int y,unsigned int color){
//void draw_point(DISPLAY *display,struct canvas *can,int x,int y,int color){
#if defined(VISUAL) || defined(__MINGW32__)
	HDC dc;

	SetPixel(can->pix,x,y,(COLORREF)color);
	dc = GetDC(can->hwnd);
	SetPixel(dc,x,y,(COLORREF)color);
	ReleaseDC(can->hwnd,dc);
#else
	set_drawcolor(color);
	DRAWPOINT(display,can->pix,cdrawGC,x,y);
	DRAWPOINT(display,can->window,cdrawGC,x,y);
	XFlush(display);
#endif
}

void draw_line(
	DISPLAY *display,struct canvas *can,int x,int y,int u,int v,unsigned int color){
	//DISPLAY *display,struct canvas *can,int x,int y,int u,int v,int color){
#if defined(VISUAL) || defined(__MINGW32__)
	HDC dc;
	HPEN pen,oldpen;

	if ( color ) {
		pen = CreatePen(PS_SOLID,1,color);
		oldpen = SelectObject(can->pix,pen);
		DRAWLINE(display,can->pix,drawGC,x,y,u,v);
		SelectObject(can->pix,oldpen);

		dc = GetDC(can->hwnd);
		oldpen = SelectObject(dc,pen);
		DRAWLINE(display,dc,drawGC,x,y,u,v);
		SelectObject(dc,oldpen);
		ReleaseDC(can->hwnd,dc);

		DeleteObject(pen);
	} else {
		DRAWLINE(display,can->pix,drawGC,x,y,u,v);
		dc = GetDC(can->hwnd);
		DRAWLINE(display,dc,drawGC,x,y,u,v);
		ReleaseDC(can->hwnd,dc);
	}
#else
	set_drawcolor(color);
	DRAWLINE(display,can->pix,cdrawGC,x,y,u,v);
	DRAWLINE(display,can->window,cdrawGC,x,y,u,v);
	XFlush(display);
#endif
}

void draw_character_string(
	DISPLAY *display,struct canvas *can,int x,int y,char *str,unsigned int color){
	//DISPLAY *display,struct canvas *can,int x,int y,char *str,int color){
#if defined(VISUAL) || defined(__MINGW32__)
	HDC dc;
	COLORREF oldcolor;

	if ( color ) {
		oldcolor = SetTextColor(can->pix,color);
		DRAWSTRING(display,can->pix,drawGC,x,y,str,strlen(str));
		SetTextColor(can->pix,oldcolor);

		dc = GetDC(can->hwnd);
		oldcolor = SetTextColor(dc,color);
		DRAWSTRING(display,dc,drawGC,x,y,str,strlen(str));
		SetTextColor(dc,oldcolor);
		ReleaseDC(can->hwnd,dc);
	} else {
		DRAWSTRING(display,can->pix,drawGC,x,y,str,strlen(str));
		dc = GetDC(can->hwnd);
		DRAWSTRING(display,dc,drawGC,x,y,str,strlen(str));
		ReleaseDC(can->hwnd,dc);
	}
#else
	set_drawcolor(color);
	DRAWSTRING(display,can->pix,cdrawGC,x,y,str,strlen(str));
	DRAWSTRING(display,can->window,cdrawGC,x,y,str,strlen(str));
	XFlush(display);
#endif
}

#define D 5

void pline(DISPLAY *display,struct canvas *can,DRAWABLE d){
	double w,w1,e,n;
	int x0,y0,x,y,xadj,yadj;
	char buf[BUFSIZ];
	double adjust_scale();

	/* XXX : should be cleaned up */
	if ( can->noaxis || (can->mode==modeNO(PLOT)&& !can->pa) )
		return;
	if ( can->mode==modeNO(INTERACTIVE))
		return;

	xadj = yadj = 0;
	if ( (can->xmin < 0) && (can->xmax > 0) ) {
		x0 = (int)((can->width)*(-can->xmin/(can->xmax-can->xmin)));
		DRAWLINE(display,d,dashGC,x0,0,x0,can->height);
	} else if ( can->xmin >= 0 )
		x0 = 0;
	else
		x0 = can->width-D;
	if ( (can->ymin < 0) && (can->ymax > 0) ) {
		y0 = (int)((can->height)*(can->ymax/(can->ymax-can->ymin)));
		DRAWLINE(display,d,dashGC,0,y0,can->width,y0);
	} else if ( can->ymin >= 0 )
		y0 = can->height;
	else
		y0 = D;

	/* scale on x-axis */
	w = can->xmax-can->xmin; 
	w1 = w * DEFAULTWIDTH/can->width;
	e = adjust_scale(EXP10(floor(log10(w1))),w1);
	for ( n = ceil(can->xmin/e); n*e<= can->xmax; n++ ) {
		x = (int)(can->width*(n*e-can->xmin)/w);
		DRAWLINE(display,d,drawGC,x,y0,x,y0-D);
		sprintf(buf,"%g",n*e);
		DRAWSTRING(display,d,scaleGC,x+2,y0+2,buf,strlen(buf));
	}

	/* scale on y-axis */
	w = can->ymax-can->ymin;
	w1 = w * DEFAULTHEIGHT/can->height;
	e = adjust_scale(EXP10(floor(log10(w1))),w1);
	for ( n = ceil(can->ymin/e); n*e<= can->ymax; n++ ) {
		y = (int)(can->height*(1-(n*e-can->ymin)/w));
		DRAWLINE(display,d,drawGC,x0,y,x0+D,y);
		sprintf(buf,"%g",n*e);
		if ( can->xmax <= 0 ) {
#if !defined(VISUAL) && !defined(__MINGW32__)
			xadj = TEXTWIDTH(sffs,buf,strlen(buf));
#else
			SIZE size;

			GetTextExtentPoint32(d,buf,strlen(buf),&size);
			xadj = size.cx;
#endif
		}
		DRAWSTRING(display,d,scaleGC,x0+2-xadj,y+2,buf,strlen(buf));
	}
}

double adjust_scale(double e,double w){
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

void initmarker(struct canvas *can,char *message){
#if defined(VISUAL) || defined(__MINGW32__)
	can->real_can->percentage = 0;
	can->real_can->prefix = message;
#else
	XawScrollbarSetThumb(can->xdone,0.0,0.0);
	XawScrollbarSetThumb(can->ydone,1.0,0.0);
	XFlush(display);
#endif
}

void marker(struct canvas *can,int dir,int p){
#if defined(VISUAL) || defined(__MINGW32__)
	if ( dir == DIR_X )
		can->real_can->percentage = (int)ceil((float)p/(float)can->real_can->width*100);
	else if ( dir == DIR_Y )
		can->real_can->percentage = (int)ceil((float)p/(float)can->real_can->height*100);
	else
		can->real_can->percentage = (int)ceil((float)p/(float)can->real_can->nzstep*100);
#else
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
#endif
}

void define_cursor(WINDOW w,CURSOR cur){
#if !defined(VISUAL) && !defined(__MINGW32__)
	XDefineCursor(display,w,cur); flush();
#endif
}

static int flush_count;
#if defined(linux) || defined(__FreeBSD__) || defined(__NetBSD__)
#define MAX_COUNT 64
#else
#define MAX_COUNT 32
#endif

void count_and_flush(){
#if !defined(VISUAL) && !defined(__MINGW32__)
	if ( ++flush_count == MAX_COUNT )
		flush();
#endif
}

void flush(){
#if !defined(VISUAL) && !defined(__MINGW32__)
	flush_count = 0;
	XFlush(display);
#endif
}
