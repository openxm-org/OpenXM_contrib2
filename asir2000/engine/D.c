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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/D.c,v 1.2 2000/08/21 08:31:24 noro Exp $ 
*/
#include "ca.h"

void dtest(f,list,hint,dcp)
P f;
ML list;
int hint;
DCP *dcp;
{
	int n,np,bound,q;
	int i,j,k;
	int *win;
	P g,factor,cofactor;
	Q csum,csumt;
	DCP dcf,dcf0;
	LUM *c;
	ML wlist;
	int z;

	n = UDEG(f); np = list->n; bound = list->bound; q = list->mod;
	win = W_ALLOC(np+1);
	ucsump(f,&csum); mulq(csum,(Q)COEF(DC(f)),&csumt); csum = csumt;
	wlist = W_MLALLOC(np); wlist->n = list->n;
	wlist->mod = list->mod; wlist->bound = list->bound;
	c = (LUM *)COEF(wlist); bcopy((char *)COEF(list),(char *)c,(int)(sizeof(LUM)*np));
	for ( g = f, k = 1, dcf = dcf0 = 0, win[0] = 1, --np, z = 0; ; ) {
#if 0
		if ( !(++z % 10000) )
			fprintf(stderr,"z=%d\n",z);
#endif
		if ( degtest(k,win,wlist,hint) &&
			dtestmain(g,csum,wlist,k,win,&factor,&cofactor) ) {
			NEXTDC(dcf0,dcf); DEG(dcf) = ONE; COEF(dcf) = factor;
			g = cofactor;
			ucsump(g,&csum); mulq(csum,(Q)COEF(DC(g)),&csumt); csum = csumt;
			for ( i = 0; i < k - 1; i++ ) 
				for ( j = win[i] + 1; j < win[i + 1]; j++ ) 
					c[j-i-1] = c[j];
			for ( j = win[k-1] + 1; j <= np; j++ ) 	
					c[j-k] = c[j];
			if ( ( np -= k ) < k ) 
				break;
			if ( np - win[0] + 1 < k ) 
				if ( ++k > np )
					break;
				else
					for ( i = 0; i < k; i++ ) 
						win[i] = i + 1;
			else 
				for ( i = 1; i < k; i++ ) 
					win[i] = win[0] + i;
		} else if ( !ncombi(1,np,k,win) ) 
			if ( k == np ) 
				break;
			else
				for ( i = 0, ++k; i < k; i++ ) 
					win[i] = i + 1;
	}
	NEXTDC(dcf0,dcf); COEF(dcf) = g;
	DEG(dcf) = ONE; NEXT(dcf) = 0;*dcp = dcf0;
}

void dtestsql(f,list,dc,dcp)
P f;
ML list;
struct oDUM *dc;
DCP *dcp;
{
	int j,n,m,b;
	P t,s,fq,fr;
	P *true;
	Q tq;
	LUM *c;
	DCP dcr,dcr0;

	n = list->n; m = list->mod; b = list->bound; c = (LUM *)list->c;
	true = (P*)ALLOCA(n*sizeof(P));
	for ( j = 0; j < n; j++ ) {
		dtestsq(m,b,f,c[j],&t);
		if ( t ) 
			true[j] = t;
		else {
			*dcp = 0;
			return;
		}
	}
	for ( t = f, j = 0; j < n; j++ ) {
		STOQ(dc[j].n,tq); pwrp(CO,true[j],tq,&s); udivpz(t,s,&fq,&fr);
		if ( fq && !fr )
			t = fq;
		else {
			*dcp = 0;
			return;
		}
	}
	for ( j = 0, dcr = dcr0 = 0; j < n; j++ ) {
		NEXTDC(dcr0,dcr); STOQ(dc[j].n,DEG(dcr)); COEF(dcr) = true[j];
	}
	NEXT(dcr) = 0; *dcp = dcr0;
}

void dtestsq(q,bound,f,fl,g)
int q,bound;
P f;
LUM fl;
P *g;
{
	P lcf,t,fq,fr,s;
	struct oML list;
	int in = 0;
	
	list.n = 1;
	list.mod = q;
	list.bound = bound;
	list.c[0] = (pointer)fl;

	mullumarray(f,&list,1,&in,&t); mulp(CO,f,COEF(DC(f)),&lcf);
	udivpz(lcf,t,&fq,&fr);
	if( fq && !fr )
		ptozp(t,1,(Q *)&s,g);
	else
		*g = 0;
}

void dtestroot(m,b,f,fl,dc,dcp)
int m,b;
P f;
LUM fl;
struct oDUM *dc;
DCP *dcp;
{
	P t,s,u;
	DCP dcr;
	Q q;

	dtestroot1(m,b,f,fl,&t);
	if ( !t ) {
		*dcp = 0;
		return;
	}
	STOQ(dc[0].n,q); pwrp(CO,t,q,&s); subp(CO,s,f,&u);
	if ( u )
		*dcp = 0;	
	else {
		NEWDC(dcr); STOQ(dc[0].n,DEG(dcr));
		COEF(dcr) = t; NEXT(dcr) = 0; *dcp = dcr;
	}
}

void dtestroot1(q,bound,f,fl,g)
int q,bound;
P f;
LUM fl;
P *g;
{
	P fq,fr,t;

	lumtop(VR(f),q,bound,fl,&t); udivpz(f,t,&fq,&fr);
	*g = (fq && !fr) ? t : 0;
}

int dtestmain(g,csum,list,k,in,fp,cofp)
P g;
Q csum;
ML list;
int k;
int *in;
P *fp,*cofp;
{
	int mod;
	P fmul,lcg;
	Q csumg;
	N nq,nr;
	P fq,fr,t;
	
	if (!ctest(g,list,k,in))
		return 0;
	mod = list->mod;
	mullumarray(g,list,k,in,&fmul); mulp(CO,g,COEF(DC(g)),&lcg);
	if ( csum ) {
		ucsump(fmul,&csumg);
		if ( csumg ) {
			divn(NM(csum),NM(csumg),&nq,&nr);
			if ( nr )
				return 0;
		} 
	}
	udivpz(lcg,fmul,&fq,&fr);
	if ( fq && !fr ) {
		ptozp(fq,1,(Q *)&t,cofp); ptozp(fmul,1,(Q *)&t,fp);
		return 1;
	} else
		return 0;
}

int degtest(k,win,list,hint)
int k;
int *win;
ML list;
int hint;
{
	register int i,d;
	LUM *c;

	if ( hint == 1 )
		return 1;
	for ( c = (LUM*)list->c, i = 0, d = 0; i < k; i++ ) 
		d += DEG(c[win[i]]);
	return !(d % hint);
}

int ctest(g,list,k,in)
P g;
ML list;
int k;
int *in;
{
	register int i;
	int q,bound;
	int *wm,*wm1,*tmpp;
	DCP dc;
	Q dvr;
	N lcn,cstn,dndn,dmyn,rn;
	LUM *l;

	for ( dc = DC(g); dc && DEG(dc); dc = NEXT(dc) );
	if ( dc ) 
		cstn = NM((Q)COEF(dc));
	else 
		return 1;
	q = list->mod; bound = list->bound;
	ntobn(q,NM((Q)COEF(DC(g))),&lcn);;
	W_CALLOC(bound+1,int,wm); W_CALLOC(bound+1,int,wm1); 
	for ( i = 0; i < PL(lcn); i++ ) 
		wm[i] = BD(lcn)[i];
	for ( i = 0, l = (LUM *)list->c; i < k; i++ ) {
		mulpadic(q,bound,wm,COEF(l[in[i]])[0],wm1);
		tmpp = wm; wm = wm1; wm1 = tmpp;
	}
	padictoq(q,bound,wm,&dvr);
	kmuln(NM((Q)COEF(DC(g))),cstn,&dndn); divn(dndn,NM(dvr),&dmyn,&rn);
	return rn ? 0 : 1;
}

/*
int ncombi(n0,n,k,c)
int n0,n,k,*c;
{
	register int i,tmp;
	
	if ( !k ) 
		return 0;
	if ( !ncombi(c[1],n,k-1,c+1) ) {
		if ( c[0] + k > n ) 
			return 0;
		else {
			for ( i = 0, tmp = c[0]; i < k; i++ ) 
				c[i] = tmp + i + 1;
			return 1;
		}
	} else 
		return 1;
}
*/

int ncombi(n0,n,k,c)
int n0,n,k,*c;
{
	register int i,t;
	
	if ( !k ) 
		return 0;
	for ( i = k-1; i >= 0 && c[i] == n+i-(k-1); i-- );
	if ( i < 0 )
		return 0;
	t = ++c[i++]; 
	for ( t++ ; i < k; i++, t++ )
		c[i] = t;
	return 1;
}

void nthrootn(number,n,root)
int n;
N number,*root;
{
	N s,t,u,pn,base,n1,n2,q,r,gcd,num;
	int sgn,index,p,i,tmp,tp,mlr,num0;

	for (  i = 0; !(n % 2); n /= 2, i++ );
	for ( index = 0, num = number; ; index++ ) {
		if ( n == 1 )
			goto TAIL;
		p = lprime[index];
		if ( !p )
			error("nthrootn : lprime[] exhausted.");
		if ( !(num0 = rem(num,p)) ) 
			continue;
		STON(n,n1); STON(p-1,n2); gcdn(n1,n2,&gcd);
		if ( !UNIN(gcd) )
			continue;
		tp = pwrm(p,num0,invm(n,p-1)); STON(tp,s);
		mlr = invm(dmb(p,n,pwrm(p,tp,n-1),&tmp),p);
		STON(p,base); STON(p,pn);
		while ( 1 ) {
			pwrn(s,n,&t); sgn = subn(num,t,&u);
			if ( !u ) {
				num = s;
				break;
			}
			if ( sgn < 0 ) {
				*root = 0;
				return;
			}
			divn(u,base,&q,&r);
			if ( r ) {
				*root = 0;
				return;
			}
			STON(dmb(p,mlr,rem(q,p),&tmp),t);
			kmuln(t,base,&u); addn(u,s,&t); s = t;
			kmuln(base,pn,&t); base = t;
		}
TAIL :		
		for ( ; i; i-- ) {
			sqrtn(num,&t);
			if ( !t ) {
				*root = 0;
				return;
			}
			num = t;
		}
		*root = num;
		return;
	}
}

void sqrtn(number,root)
N number,*root;
{
	N a,s,r,q;
	int sgn;

	for ( a = ONEN; ; ) {
		divn(number,a,&q,&r); sgn = subn(q,a,&s);
		if ( !s ) {
			*root = !r ? a : 0;
			return;
		} else if ( UNIN(s) ) {
			*root = 0;
			return;
		} else {
			divin(s,2,&q); 
			if ( sgn > 0 ) 
				addn(a,q,&r); 
			else
				subn(a,q,&r); 
			a = r;
		}
	}
}

void lumtop(v,mod,bound,f,g)
V v;
int mod;
int bound;
LUM f;
P *g;
{
	DCP dc,dc0;
	int **l;
	int i;
	Q q;

	for ( dc0 = NULL, i = DEG(f), l = COEF(f); i >= 0; i-- ) {
		padictoq(mod,bound,l[i],&q);
		if ( q ) {
			NEXTDC(dc0,dc);
			if ( i ) 
				STOQ(i,DEG(dc));
			else 
				DEG(dc) = 0;
			COEF(dc) = (P)q;
		}
	}
	if ( !dc0 )
		*g = 0;
	else {
		NEXT(dc) = 0; MKP(v,dc0,*g);
	}
}
			
void padictoq(mod,bound,p,qp)
int mod,bound,*p;
Q *qp;
{
	register int h,i,t;
	int br,sgn;
	unsigned int *ptr;
	N n,tn;
	int *c;

	c = W_ALLOC(bound);
	for ( i = 0; i < bound; i++ )
		c[i] = p[i];
	h = (mod%2?(mod-1)/2:mod/2); i = bound - 1;
	while ( i >= 0 && c[i] == h ) i--;
	if ( i == -1 || c[i] > h ) {
		for (i = 0, br = 0; i < bound; i++ ) 
			if ( ( t = -(c[i] + br) ) < 0 ) {
				c[i] = t + mod; br = 1;
			} else {
				c[i] = 0; br = 0;
			}
		sgn = -1;
	} else 
		sgn = 1;
	for ( i = bound - 1; ( i >= 0 ) && ( c[i] == 0 ); i--);
	if ( i == -1 ) 
		*qp = 0;
	else {
		n = NALLOC(i+1); PL(n) = i+1;
		for ( i = 0, ptr = BD(n); i < PL(n); i++ )
			ptr[i] = c[i];	
		bnton(mod,n,&tn); NTOQ(tn,sgn,*qp);
	}
}

void mullumarray(f,list,k,in,g)
P f;
ML list;
int k;
int *in;
P *g;
{
	int np,bound,q,n,i,u;
	int *tmpp;
	LUM lclum,wb0,wb1,tlum;
	LUM *l;
	N lc;

	n = UDEG(f); np = list->n; bound = list->bound; q = list->mod;
	W_LUMALLOC(n,bound,wb0); W_LUMALLOC(n,bound,wb1);
	W_LUMALLOC(0,bound,lclum);
	ntobn(q,NM((Q)COEF(DC(f))),&lc);
	for ( i = 0, tmpp = COEF(lclum)[0], u = MIN(bound,PL(lc)); 
		  i < u; i++ ) 
		tmpp[i] = BD(lc)[i];
	l = (LUM *)list->c;
	mullum(q,bound,lclum,l[in[0]],wb0);
	for ( i = 1; i < k; i++ ) {
		mullum(q,bound,l[in[i]],wb0,wb1);
		tlum = wb0; wb0 = wb1; wb1 = tlum;
	}
	lumtop(VR(f),q,bound,wb0,g);
}

void ucsump(f,s)
P f;
Q *s;
{
	Q t,u;
	DCP dc;

	for ( dc = DC(f), t = 0; dc; dc = NEXT(dc) ) {
		addq((Q)COEF(dc),t,&u); t = u;
	}
	*s = t;
}

