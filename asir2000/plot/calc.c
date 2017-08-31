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
 * $OpenXM: OpenXM_contrib2/asir2000/plot/calc.c,v 1.11 2014/06/27 07:58:29 saito Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ifplot.h"
#include <math.h>
#if defined(PARI)
#include "genpari.h"
#endif

#ifndef MAXSHORT
#define MAXSHORT ((short)0x7fff)
#endif

void calc(double **tab,struct canvas *can,int nox){
	//memory_plot,IFPLOTD,INEQND,INEQNANDD,INEQNORD
	//INEQNXORD,conplotmainD
	double x,y,xstep,ystep;
	int ix,iy;
	Real r,rx,ry;
	Obj fr,g,t,s;

	if(!nox)initmarker(can,"Evaluating...");
  todouble((Obj)can->formula,&fr);
	xstep=(can->xmax-can->xmin)/can->width;
	ystep=(can->ymax-can->ymin)/can->height;
	MKReal(1.0,rx); MKReal(1.0,ry); // dummy real
	BDY(rx)=can->xmin;
	substr(CO,0,fr,can->vx,can->xmin?(Obj)rx:0,&t); devalr(CO,t,&g);
	BDY(ry)=can->ymin;
	substr(CO,0,g,can->vy,can->ymin?(Obj)ry:0,&t); devalr(CO,t,&s);
	can->vmax=can->vmin=ToReal(s);
	for(ix=0,x=can->xmin; ix<can->width; ix++,x+=xstep){
		BDY(rx)=x; substr(CO,0,fr,can->vx,x?(Obj)rx:0,&t);
		devalr(CO,t,&g);
		if(!nox)marker(can,DIR_X,ix);
		for(iy=0,y=can->ymin; iy<can->height; iy++,y+=ystep){
			BDY(ry)=y;
			substr(CO,0,g,can->vy,y?(Obj)ry:0,&t);
			devalr(CO,t,&s);
			tab[ix][iy]=ToReal(s);
			if(can->vmax<tab[ix][iy])can->vmax=tab[ix][iy];
			if(can->vmin>tab[ix][iy])can->vmin=tab[ix][iy];
		}
	}
}

void calcq(double **tab,struct canvas *can,int nox){
	//IFPLOTQ,INEQNQ,INEQNANDQ,INEQNORQ,INEQNXORQ
	//plotoverD
	Q dx,dy,xstep,ystep,q1,w,h,c;
	P g,g1,f1,f2,x,y;
	int ix,iy;
	Obj fr,gm,t,s;
	Real r,rx,ry;

  todouble((Obj)can->formula,&fr);
	MKReal(1.0,rx); MKReal(1.0,ry); // dummy real
	BDY(rx)=can->xmin;
	substr(CO,0,fr,can->vx,can->xmin?(Obj)rx:0,&t); devalr(CO,t,&gm);
	BDY(ry)=can->ymin;
	substr(CO,0,gm,can->vy,can->ymin?(Obj)ry:0,&t); devalr(CO,t,&s);
	can->vmax=can->vmin=ToReal(s);

	subq(can->qxmax,can->qxmin,&dx); STOQ(can->width,w); divq(dx,w,&xstep);
	subq(can->qymax,can->qymin,&dy); STOQ(can->height,h); divq(dy,h,&ystep);
	MKV(can->vx,x); mulp(CO,(P)xstep,x,(P *)&t);
	addp(CO,(P)can->qxmin,(P)t,(P *)&s); substp(CO,can->formula,can->vx,(P)s,&f1);
	MKV(can->vy,y); mulp(CO,(P)ystep,y,(P *)&t);
	addp(CO,(P)can->qymin,(P)t,(P *)&s); substp(CO,f1,can->vy,(P)s,&f2);
	ptozp(f2,1,&c,&g);
	if(!nox) initmarker(can,"Evaluating...");
	for(iy=0;iy<can->height;iy++){
		marker(can,DIR_Y,iy);
		STOQ(iy,q1); substp(CO,g,can->vy,(P)q1,(P *)&t); ptozp((P)t,1,&c,&g1);
		for(ix=0;ix<can->width;ix++){
			STOQ(ix,q1);substp(CO,g1,can->vx,(P)q1,(P *)&t);
			devalr(CO,t,&s);
			tab[ix][iy]=ToReal(s);
			if(can->vmax<tab[ix][iy])can->vmax=tab[ix][iy];
			if(can->vmin>tab[ix][iy])can->vmin=tab[ix][iy];
		}
	}
}

void calcb(double **tab,struct canvas *can,int nox){
	//IFPLOTB,INEQNB,INEQNANDB,INEQNORB,INEQNXORB
	Q dx,dy,xstep,ystep,q1,w,h,c;
	P g,g1,f1,f2,x,y,t,s;
	int ix,iy,*a,*pa;
	VECT ss;
	Obj fr,gm,tm,sm;
	Real r,rx,ry;

  todouble((Obj)can->formula,&fr);
	MKReal(1.0,rx); MKReal(1.0,ry); // dummy real
	BDY(rx)=can->xmin;
	substr(CO,0,fr,can->vx,can->xmin?(Obj)rx:0,&tm); devalr(CO,tm,&gm);
	BDY(ry)=can->ymin;
	substr(CO,0,gm,can->vy,can->ymin?(Obj)ry:0,&tm); devalr(CO,tm,&sm);
	can->vmax=can->vmin=ToReal(sm);

	for(iy=0;iy<can->height;iy++)for(ix=0;ix<can->width;ix++)tab[ix][iy]=1.0;
	subq(can->qxmax,can->qxmin,&dx); STOQ(can->width,w); divq(dx,w,&xstep);
	subq(can->qymax,can->qymin,&dy); STOQ(can->height,h); divq(dy,h,&ystep);
	MKV(can->vx,x); mulp(CO,(P)xstep,x,&t);
	addp(CO,(P)can->qxmin,t,&s); substp(CO,can->formula,can->vx,s,&f1);
	MKV(can->vy,y); mulp(CO,(P)ystep,y,&t);
	addp(CO,(P)can->qymin,t,&s); substp(CO,f1,can->vy,s,&f2);
	ptozp(f2,1,&c,&g);
	a=(int *)ALLOCA((MAX(can->width,can->height)+1)*sizeof(int));
	for(iy=0;iy<can->height;iy++)for(ix=0;ix<can->width;ix++)tab[ix][iy]=1.0;
	subq(can->qxmax,can->qxmin,&dx); STOQ(can->width,w); divq(dx,w,&xstep);
	subq(can->qymax,can->qymin,&dy); STOQ(can->height,h); divq(dy,h,&ystep);
	MKV(can->vx,x); mulp(CO,(P)xstep,x,&t);
	addp(CO,(P)can->qxmin,t,&s); substp(CO,can->formula,can->vx,s,&f1);
	MKV(can->vy,y); mulp(CO,(P)ystep,y,&t);
	addp(CO,(P)can->qymin,t,&s); substp(CO,f1,can->vy,s,&f2);
	ptozp(f2,1,&c,&g);
	a=(int *)ALLOCA((MAX(can->width,can->height)+1)*sizeof(int));
	for(ix=0;ix<can->width;ix++){
		STOQ(ix,q1); substp(CO,g,can->vx,(P)q1,&t); ptozp(t,1,&c,&g1);
		if(!g1)for(iy=0;iy<can->height;iy++)tab[ix][iy]=0.0;
		else if(!NUM(g1)){
			sturmseq(CO,g1,&ss);
			seproot(ss,0,can->width,a);
			for(iy=0,pa=a;iy<can->height;iy++,pa++){
				if(*pa<0||(*(pa+1)>=0&&(*pa>*(pa+1))))tab[ix][iy]=0.0;
				else {
					STOQ(iy,q1);substp(CO,g1,can->vy,(P)q1,&t);
					devalr(CO,(Obj)t,(Obj *)&s);
					tab[ix][iy]=ToReal(s);
					if(can->vmax<tab[ix][iy])can->vmax=tab[ix][iy];
					if(can->vmin>tab[ix][iy])can->vmin=tab[ix][iy];
				}
			}
		}
	}
	for(iy=0;iy<can->height;iy++){
		STOQ(iy,q1); substp(CO,g,can->vy,(P)q1,&t); ptozp(t,1,&c,&g1);
		if(!g1) for(ix=0;ix<can->width;ix++)tab[ix][iy]=0.0;
		else if(!NUM(g1)){
			sturmseq(CO,g1,&ss);
			seproot(ss,0,can->height,a);
			for(ix=0,pa=a;ix<can->width;ix++,pa++){
				if(tab[ix][iy]!=0.0){
					if(*pa<0||(*(pa+1)>=0&&(*pa>*(pa+1))))tab[ix][iy]=0.0;
					else {
						STOQ(ix,q1);substp(CO,g1,can->vx,(P)q1,&t);
						devalr(CO,(Obj)t,(Obj *)&s);
						tab[ix][iy]=ToReal(s);
						if(can->vmax<tab[ix][iy])can->vmax=tab[ix][iy];
						if(can->vmin>tab[ix][iy])can->vmin=tab[ix][iy];
					}
				}
			}
		}
	}
}

double usubstrp(P p,double r){
	DCP dc;
	int d;
	double t,pwrreal0();

	if(!p) t=0.0;
	else if(NUM(p))t=BDY((Real)p);
	else {
		dc=DC(p); t=BDY((Real)COEF(dc));
		for(d=QTOS(DEG(dc)),dc=NEXT(dc);dc;d=QTOS(DEG(dc)),dc=NEXT(dc)){
			t=t*pwrreal0(r,(d-QTOS(DEG(dc))))+BDY((Real)COEF(dc));
		}
		if(d)t*=pwrreal0(r,d);
	}
	return t;
}

void qcalc(char **tab,struct canvas *can){
	//qifplotmain(Old type)
	Q dx,dy,w,h,xstep,ystep,c,q1;
	P g,g1,f1,f2,x,y,t,s;
	int ix,iy;
	int *a,*pa;
	VECT ss;

	subq(can->qxmax,can->qxmin,&dx); STOQ(can->width,w); divq(dx,w,&xstep);
	subq(can->qymax,can->qymin,&dy); STOQ(can->height,h); divq(dy,h,&ystep);
	MKV(can->vx,x); mulp(CO,(P)xstep,x,&t); 
	addp(CO,(P)can->qxmin,t,&s); substp(CO,can->formula,can->vx,s,&f1);
	MKV(can->vy,y); mulp(CO,(P)ystep,y,&t);
	addp(CO,(P)can->qymin,t,&s); substp(CO,f1,can->vy,s,&f2);
	ptozp(f2,1,&c,&g);
	a=(int *)ALLOCA((MAX(can->width,can->height)+1)*sizeof(int));
	initmarker(can,"Horizontal scan...");
	for( ix=0; ix < can->width; ix++ ){
		marker(can,DIR_X,ix);
		STOQ(ix,q1); substp(CO,g,can->vx,(P)q1,&t); ptozp(t,1,&c,&g1);
		if( !g1 )
			for(iy=0; iy < can->height; iy++ )
				tab[ix][iy]=1;
		else if( !NUM(g1) ){
			sturmseq(CO,g1,&ss); seproot(ss,0,can->height,a);
			for(iy=0, pa=a; iy < can->height; iy++, pa++ )
				if( *pa < 0 || (*(pa+1) >= 0 && (*pa > *(pa+1))) )
					tab[ix][iy]=1;
		}
	}
	initmarker(can,"Vertical scan...");
	for( iy=0; iy < can->height; iy++ ){
		marker(can,DIR_Y,iy);
		STOQ(iy,q1); substp(CO,g,can->vy,(P)q1,&t); ptozp(t,1,&c,&g1);
		if( !g1 )
			for(ix=0; ix < can->width; ix++ )
				tab[ix][iy]=1;
		else if( !NUM(g1) ){
			sturmseq(CO,g1,&ss); seproot(ss,0,can->width,a);
			for(ix=0, pa=a; ix < can->width; ix++, pa++ )
				if( *pa < 0 || (*(pa+1) >= 0 && (*pa > *(pa+1))) )
					tab[ix][iy]=1;
		}
	}
}

void sturmseq(VL vl,P p,VECT *rp){
	P g1,g2,q,r,s,*t;
	V v;
	VECT ret;
	int i,j;
	Q a,b,c,d,h,l,m,x;

	v=VR(p);t=(P *)ALLOCA((deg(v,p)+1)*sizeof(P));
	g1=t[0]=p;diffp(vl,p,v,(P *)&a);ptozp((P)a,1,&c,&g2);t[1]=g2;
	for(i=1,h=ONE,x=ONE;;){
		if(NUM(g2)) break;
		subq(DEG(DC(g1)),DEG(DC(g2)),&d);
		l=(Q)LC(g2);
		if(SGN(l)<0){
			chsgnq(l,&a);l=a;
		}
		addq(d,ONE,&a);pwrq(l,a,&b);mulp(vl,(P)b,g1,(P *)&a);
		divsrp(vl,(P)a,g2,&q,&r);
		if(!r) break;
		chsgnp(r,&s);r=s;i++;
		if(NUM(r)){
			t[i]=r;break;
		}
		pwrq(h,d,&m);g1=g2;
		mulq(m,x,&a);divsp(vl,r,(P)a,&g2);t[i]=g2;
		x=(Q)LC(g1);
		if(SGN(x)<0){
			chsgnq(x,&a);x=a;
		}
		pwrq(x,d,&a);mulq(a,h,&b);divq(b,m,&h);
	}
	MKVECT(ret,i+1);
	for(j=0;j<=i;j++)
		ret->body[j]=(pointer)t[j];
	*rp=ret;	
}

void seproot(VECT s,int min,int max,int *ar){
	P f,*ss;
	Q q,t;
	int i,j,k;

	ss=(P *)s->body;f=ss[0];
	for(i=min;i<=max;i++){
		STOQ(i,q);usubstqp(f,q,&t);
		if(!t)ar[i]=-1;
		else {
			ar[i]=numch(s,q,t);break;
		}
	}
	if(i>max) return;
	for(j=max;j>= min;j--){
		STOQ(j,q); usubstqp(f,q,&t);
		if(!t)ar[j]=-1;
		else {
			if(i!=j)ar[j]=numch(s,q,t);
			break;
		}
	}
	if(j<=i+1) return;
	if(ar[i]==ar[j]){
		for(k=i+1;k<j;k++)ar[k]=ar[i];
		return;
	}
	k=(i+j)/2;
	seproot(s,i,k,ar);
	seproot(s,k,j,ar);
}

int numch(VECT s,Q n,Q a0){
	int len,i,c;
	Q a;
	P *ss;

	len=s->len;ss=(P *)s->body;
	for(i=1,c=0;i<len;i++){
		usubstqp(ss[i],n,&a);
		if(a){
			if((SGN(a)>0 && SGN(a0)<0)||(SGN(a)<0&&SGN(a0)>0))c++;
			a0=a;
		}
	}
	return c;
}

void usubstqp(P p,Q r,Q *v){
	Q d,d1,a,b,t;
	DCP dc;

	if(!p)
		*v=0;
	else if(NUM(p))*v=(Q)p;
	else {
		dc=DC(p);t=(Q)COEF(dc);
		for(d=DEG(dc),dc=NEXT(dc);dc;d=DEG(dc),dc=NEXT(dc)){
			subq(d,DEG(dc),&d1);pwrq(r,d1,&a);
			mulq(t,a,&b);addq(b,(Q)COEF(dc),&t);
		}
		if(d){
			pwrq(r,d,&a);mulq(t,a,&b);t=b;
		}
		*v=t;
	}
}

void plotcalc(struct canvas *can){
	//plot,memory_plot,plotover,plot_resize
	double x,xmin,xstep,ymax,ymin,dy,*tab,usubstrp();
	int ix,w,h;
	Real r,rx;
	Obj fr,t,s;
	POINT *pa;

  todouble((Obj)can->formula,&fr);
	w=can->width;h=can->height;
	xmin=can->xmin;xstep=(can->xmax-can->xmin)/w;
	tab=(double *)ALLOCA(w*sizeof(double));
	MKReal(1,rx); // dummy real number
	for(ix=0,x=xmin;ix<w;ix++,x+=xstep){
		// full substitution
		BDY(rx)=x;
		substr(CO,0,fr,can->vx,x?(Obj)rx:0,&s);
		devalr(CO,(Obj)s,&t);
		if(t&&(OID(t)!=O_N||NID((Num)t)!=N_R))
			error("plotcalc : invalid evaluation");
		tab[ix]=ToReal((Num)t);	
	}
	if(can->ymax==can->ymin){
		for(ymax=ymin=tab[0],ix=1;ix<w;ix++){
			if(tab[ix]>ymax)ymax=tab[ix];
			if(tab[ix]<ymin)ymin=tab[ix];
		}
		can->ymax=ymax;can->ymin=ymin;
	} else {
		ymax=can->ymax;ymin=can->ymin;
	}
	dy=ymax-ymin;
	can->pa=(struct pa *)MALLOC(sizeof(struct pa));
	can->pa[0].length=w;
	can->pa[0].pos=pa=(POINT *)MALLOC(w*sizeof(POINT));
	for(ix=0;ix<w;ix++){
		double t;
		XC(pa[ix])=ix; 
		t=(h-1)*(ymax-tab[ix])/dy;
		if(t>MAXSHORT)YC(pa[ix])=MAXSHORT;
		else if(t<-MAXSHORT)YC(pa[ix])=-MAXSHORT;
		else YC(pa[ix])=(long)t;
	}
}

void polarcalc(struct canvas *can){
	double xmax,xmin,ymax,ymin,dx,dy,pmin,pstep,tr,p,*tabx,*taby;
	double usubstrp();
	int i,nstep,w,h;
	POINT *pa;
	Real r;
	Obj fr,t,s;

  todouble((Obj)can->formula,&fr);
	w=can->width; h=can->height; nstep=can->nzstep;
	pmin=can->zmin; pstep=(can->zmax-can->zmin)/nstep;
	tabx=(double *)ALLOCA(nstep*sizeof(double));
	taby=(double *)ALLOCA(nstep*sizeof(double));
	MKReal(1,r); // dummy real number

	for(i=0,p=pmin;i<nstep;i++,p+= pstep){
		// full substitution
		BDY(r)=p;
		substr(CO,0,fr,can->vx,p?(Obj)r:0,&s);
		devalr(CO,(Obj)s,&t);
		if(t&&(OID(t)!=O_N||NID((Num)t)!=N_R))
			error("polarcalc : invalid evaluation");
		tr=ToReal((Num)t);	
		tabx[i]=tr*cos(p);
		taby[i]=tr*sin(p);
	}
	xmax=xmin=tabx[0];
	ymax=ymin=taby[0];
	for(i=1;i<nstep;i++){
		if(tabx[i]>xmax)xmax=tabx[i];
		if(tabx[i]<xmin)xmin=tabx[i];
		if(taby[i]>ymax)ymax=taby[i];
		if(taby[i]<ymin)ymin=taby[i];
	}
	can->xmax=xmax;can->xmin=xmin;
	can->ymax=ymax;can->ymin=ymin;
	dx=xmax-xmin;
	dy=ymax-ymin;
	can->pa=(struct pa *)MALLOC(sizeof(struct pa));
	can->pa[0].length=nstep;
	can->pa[0].pos=pa=(POINT *)MALLOC(w*sizeof(POINT));
	for(i=0;i<nstep;i++){
		XC(pa[i])=(w-1)*(tabx[i]-xmin)/dx; 
		YC(pa[i])=(h-1)*(ymax-taby[i])/dy; 
	}
}

void polarcalcNG(struct canvas *can){
	//polarplotNG
	double xmax,xmin,ymax,ymin,dx,dy,pmin,pstep,tr,p, *tabx,*taby;
	double usubstrp();
	int i,ix,iy,nstep,w,h;
	POINT *pa;
	Real r;
	Obj fr,t,s;

  todouble((Obj)can->formula,&fr);
	w=can->width; h=can->height; nstep=can->nzstep;
	pmin=can->zmin; pstep=(can->zmax-can->zmin)/nstep;
	tabx=(double *)ALLOCA(nstep*sizeof(double));
	taby=(double *)ALLOCA(nstep*sizeof(double));
	MKReal(1,r); // dummy real number

	for(i=0,p=pmin;i<nstep;i++,p+= pstep){
		// full substitution
		BDY(r)=p;
		substr(CO,0,fr,can->vx,p?(Obj)r:0,&s);
		devalr(CO,(Obj)s,&t);
		if(t&&(OID(t)!=O_N||NID((Num)t)!=N_R))
			error("polarcalc : invalid evaluation");
		tr=ToReal((Num)t);	
		tabx[i]=tr*cos(p);
		taby[i]=tr*sin(p);
		if(i==0){
			xmax=xmin=tabx[0];
			ymax=ymin=taby[0];
		} else {
			if(tabx[i]>xmax)xmax=tabx[i];
			if(tabx[i]<xmin)xmin=tabx[i];
			if(taby[i]>ymax)ymax=taby[i];
			if(taby[i]<ymin)ymin=taby[i];
		}
	}
	can->xmax=xmax;can->xmin=xmin;
	can->ymax=ymax;can->ymin=ymin;
	dx=xmax-xmin;
	dy=ymax-ymin;
	can->pa=(struct pa *)MALLOC(sizeof(struct pa));
	can->pa[0].length=nstep;
	can->pa[0].pos=pa=(POINT *)MALLOC(w*sizeof(POINT));
	for(i=0;i<nstep;i++){
		XC(pa[i])=(w-1)*(tabx[i]-xmin)/dx; 
		YC(pa[i])=(h-1)*(ymax-taby[i])/dy; 
	}
}

/*
void ineqncalc(double **tab,struct canvas *can,int nox){
	double x,y,xmin,ymin,xstep,ystep;
	int ix,iy,w,h;
	Real r,rx,ry;
	Obj fr,g,t,s;
	V vx,vy;

	if(!nox) initmarker(can,"Evaluating...");
  todouble((Obj)can->formula,&fr);
	vx=can->vx;vy=can->vy;
	w=can->width;h=can->height; 
	xmin=can->xmin;xstep=(can->xmax-can->xmin)/w;
	ymin=can->ymin;ystep=(can->ymin-can->ymin)/h;
	MKReal(1.0,rx); MKReal(1.0,ry); // dummy real

	for(ix=0,x=xmin;ix<=w;ix++,x+=xstep){
		BDY(rx)=x; substr(CO,0,fr,vx,x?(Obj)rx:0,&t);
		devalr(CO,t,&g);
		if(!nox) marker(can,DIR_X,ix);
		for(iy=0,y=ymin;iy<=h;iy++,y+=ystep){
			BDY(ry)=y;
			substr(CO,0,g,vy,y?(Obj)ry:0,&t);
			devalr(CO,t,&s);
			tab[ix][iy]=ToReal(s);
		}
	}
}
*/

#if defined(INTERVAL)
void itvcalc(double **mask, struct canvas *can, int nox){
	//ITVIFPLOT
	double x,y,xstep,ystep,dx,dy,wx,wy;
	int idv,ix,iy,idx,idy;
	Itv ity,itx,ddx,ddy;
	Real r,rx,ry,rx1,ry1,rdx,rdy,rdx1,rdy1;
	V vx,vy;
	Obj fr,g,t,s;

	idv=can->division;
  todouble((Obj)can->formula,&fr);
	vx=can->vx; vy=can->vy;
	xstep=(can->xmax-can->xmin)/can->width;
	ystep=(can->ymax-can->ymin)/can->height;
	if(idv!=0){
		wx=xstep/can->division;
		wy=ystep/can->division;
	}
	MKReal(can->ymin,ry1);
	for(iy=0,y=can->ymin; iy<can->height; iy++,y+=ystep){
		ry=ry1;
		MKReal(y+ystep,ry1);
		istoitv((Num)(ry1),(Num)ry,&ity);
		substr(CO,0,(Obj)fr,vy,(Obj)ity,&t);
		MKReal(can->xmin,rx1);
		for(ix=0,x=can->xmin; ix<can->width; ix++,x+=xstep){
			rx=rx1;
			MKReal(x+xstep,rx1);
			istoitv((Num)(rx1),(Num)rx,&itx);
			substr(CO,0,(Obj)fr,vx,(Obj)itx,&t);
			MKReal(can->ymin,ry1);
			for(iy=0,y=can->ymin; iy<can->height; iy++,y+=ystep){
				ry=ry1;
				MKReal(y+ystep,ry1);
				istoitv((Num)ry,(Num)ry1,&ity);
				substr(CO,0,(Obj)t,vy,(Obj)ity,&g);
				if(compnum(0,0,(Num)g))mask[ix][iy]=-1;
				else {
					mask[ix][iy]=0;
/*
					if(idv==0) mask[ix][iy]=0;
					else {
						MKReal(y,rdy1);
						for(idy=0,dy=y;idy<idv;dy+=wy,idy++){
							rdy=rdy1;
							MKReal(dy+wy,rdy1);
							istoitv((Num)rdy,(Num)rdy1,&ddy);
							substr(CO,0,(Obj)fr,vy,(Obj)ddy,&t);
							MKReal(x,rdx1);
							for(idx=0,dx=x;idx<idx;dx+=wx,idx++){
								rdx=rdx1;
								MKReal(dx+wx,rdx1);
								istoitv((Num)rdx,(Num)rdx1,&ddx);
								substr(CO,0,(Obj)t,vx,(Obj)ddx,&g);
								if(!compnum(0,0,(Num)g)){
									mask[ix][iy]=0;
									break;
								}
							}
							if(mask[ix][iy]==0)break;
						}
					}
*/
				}
			}
		}
	}
}
#endif
