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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/dist.c,v 1.15 2001/03/29 09:49:57 noro Exp $ 
*/
#include "ca.h"

#define ORD_REVGRADLEX 0
#define ORD_GRADLEX 1
#define ORD_LEX 2
#define ORD_BREVGRADLEX 3
#define ORD_BGRADLEX 4
#define ORD_BLEX 5
#define ORD_BREVREV 6
#define ORD_BGRADREV 7
#define ORD_BLEXREV 8
#define ORD_ELIM 9
#define ORD_WEYL_ELIM 10
#define ORD_HOMO_WW_DRL 11

int (*cmpdl)()=cmpdl_revgradlex;
int (*primitive_cmpdl[3])() = {cmpdl_revgradlex,cmpdl_gradlex,cmpdl_lex};

void comm_muld(VL,DP,DP,DP *);
void weyl_muld(VL,DP,DP,DP *);
void weyl_muldm(VL,MP,DP,DP *);
void weyl_mulmm(VL,MP,MP,int,struct cdl *,int);
void comm_muld_tab(VL,int,struct cdl *,int,struct cdl *,int,struct cdl *);

void mkwc(int,int,Q *);

int cmpdl_weyl_elim();
int cmpdl_homo_ww_drl();

int do_weyl;

int dp_nelim,dp_fcoeffs;
struct order_spec dp_current_spec;
int *dp_dl_work;

int has_fcoef(DP);
int has_fcoef_p(P);

int has_fcoef(f)
DP f;
{
	MP t;

	if ( !f )
		return 0;
	for ( t = BDY(f); t; t = NEXT(t) )
		if ( has_fcoef_p(t->c) )
			break;
	return t ? 1 : 0;
}

int has_fcoef_p(f)
P f;
{
	DCP dc;

	if ( !f )
		return 0;
	else if ( NUM(f) )
		return (NID((Num)f) == N_LM 
			|| NID((Num)f) == N_GF2N
			|| NID((Num)f) == N_GFPN
			|| NID((Num)f) == N_GFS) ? 1 : 0;
	else {
		for ( dc = DC(f); dc; dc = NEXT(dc) )
			if ( has_fcoef_p(COEF(dc)) )
				return 1;
		return 0;
	}
}

void initd(spec)
struct order_spec *spec;
{
	switch ( spec->id ) {
		case 2:
			cmpdl = cmpdl_matrix;
			dp_dl_work = (int *)MALLOC_ATOMIC(spec->nv*sizeof(int));	
			break;
		case 1:
			cmpdl = cmpdl_order_pair;
			break;
		default:
			switch ( spec->ord.simple ) {
				case ORD_REVGRADLEX:
					cmpdl = cmpdl_revgradlex; break;
				case ORD_GRADLEX:
					cmpdl = cmpdl_gradlex; break;
				case ORD_BREVGRADLEX:
					cmpdl = cmpdl_brevgradlex; break;
				case ORD_BGRADLEX:
					cmpdl = cmpdl_bgradlex; break;
				case ORD_BLEX:
					cmpdl = cmpdl_blex; break;
				case ORD_BREVREV:
					cmpdl = cmpdl_brevrev; break;
				case ORD_BGRADREV:
					cmpdl = cmpdl_bgradrev; break;
				case ORD_BLEXREV:
					cmpdl = cmpdl_blexrev; break;
				case ORD_ELIM:
					cmpdl = cmpdl_elim; break;
				case ORD_WEYL_ELIM:
					cmpdl = cmpdl_weyl_elim; break;
				case ORD_HOMO_WW_DRL:
					cmpdl = cmpdl_homo_ww_drl; break;
				case ORD_LEX: default:
					cmpdl = cmpdl_lex; break;
			}
			break;
	}
	dp_current_spec = *spec;
}

void ptod(vl,dvl,p,pr)
VL vl,dvl;
P p;
DP *pr;
{
	int isconst = 0;
	int n,i,j,k;
	VL tvl;
	V v;
	DL d;
	MP m;
	DCP dc;
	DCP *w;
	DP r,s,t,u;
	P x,c;

	if ( !p )
		*pr = 0;
	else {
		for ( n = 0, tvl = dvl; tvl; tvl = NEXT(tvl), n++ );
		if ( NUM(p) ) {
			NEWDL(d,n);
			NEWMP(m); m->dl = d; C(m) = p; NEXT(m) = 0; MKDP(n,m,*pr); (*pr)->sugar = 0;
		} else {
			for ( i = 0, tvl = dvl, v = VR(p);
				tvl && tvl->v != v; tvl = NEXT(tvl), i++ );
			if ( !tvl ) {
				for ( dc = DC(p), k = 0; dc; dc = NEXT(dc), k++ );
				w = (DCP *)ALLOCA(k*sizeof(DCP));
				for ( dc = DC(p), j = 0; j < k; dc = NEXT(dc), j++ )
					w[j] = dc;

				for ( j = k-1, s = 0, MKV(v,x); j >= 0; j-- ) {
					ptod(vl,dvl,COEF(w[j]),&t); pwrp(vl,x,DEG(w[j]),&c);
					muldc(vl,t,c,&r); addd(vl,r,s,&t); s = t;
				}
				*pr = s;
			} else {
				for ( dc = DC(p), k = 0; dc; dc = NEXT(dc), k++ );
				w = (DCP *)ALLOCA(k*sizeof(DCP));
				for ( dc = DC(p), j = 0; j < k; dc = NEXT(dc), j++ )
					w[j] = dc;

				for ( j = k-1, s = 0; j >= 0; j-- ) {
					ptod(vl,dvl,COEF(w[j]),&t);
					NEWDL(d,n); d->td = QTOS(DEG(w[j])); d->d[i] = d->td;
					NEWMP(m); m->dl = d; C(m) = (P)ONE; NEXT(m) = 0; MKDP(n,m,u); u->sugar = d->td;
					comm_muld(vl,t,u,&r); addd(vl,r,s,&t); s = t;
				}
				*pr = s;
			}
		}
	}
	if ( !dp_fcoeffs && has_fcoef(*pr) )
		dp_fcoeffs = 1;
}

void dtop(vl,dvl,p,pr)
VL vl,dvl;
DP p;
P *pr;
{
	int n,i,j,k;
	DL d;
	MP m;
	MP *a;
	P r,s,t,u,w;
	Q q;
	VL tvl;

	if ( !p )
		*pr = 0;
	else {
		for ( k = 0, m = BDY(p); m; m = NEXT(m), k++ );
		a = (MP *)ALLOCA(k*sizeof(MP));
		for ( j = 0, m = BDY(p); j < k; m = NEXT(m), j++ )
			a[j] = m;

		for ( n = p->nv, j = k-1, s = 0; j >= 0; j-- ) {
			m = a[j];
			t = C(m);
			if ( NUM(t) && NID((Num)t) == N_M ) {
				mptop(t,&u); t = u;
			}
			for ( i = 0, d = m->dl, tvl = dvl;
				i < n; tvl = NEXT(tvl), i++ ) {
				MKV(tvl->v,r); STOQ(d->d[i],q); pwrp(vl,r,q,&u);
				mulp(vl,t,u,&w); t = w;
			}
			addp(vl,s,t,&u); s = u;
		}
		*pr = s;
	}
}

void nodetod(node,dp)
NODE node;
DP *dp;
{
	NODE t;
	int len,i,td;
	Q e;
	DL d;
	MP m;
	DP u;

	for ( t = node, len = 0; t; t = NEXT(t), len++ );
	NEWDL(d,len);
	for ( t = node, i = 0, td = 0; i < len; t = NEXT(t), i++ ) {
		e = (Q)BDY(t);
		if ( !e )
			d->d[i] = 0;
		else if ( !NUM(e) || !RATN(e) || !INT(e) )
			error("nodetod : invalid input");
		else {
			d->d[i] = QTOS((Q)e); td += d->d[i];
		}
	}
	d->td = td;
	NEWMP(m); m->dl = d; C(m) = (P)ONE; NEXT(m) = 0;
	MKDP(len,m,u); u->sugar = td; *dp = u;
}

int sugard(m)
MP m;
{
	int s;

	for ( s = 0; m; m = NEXT(m) )
		s = MAX(s,m->dl->td);
	return s;
}

void addd(vl,p1,p2,pr)
VL vl;
DP p1,p2,*pr;
{
	int n;
	MP m1,m2,mr,mr0;
	P t;

	if ( !p1 )
		*pr = p2;
	else if ( !p2 )
		*pr = p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; )
			switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
				case 0:
					addp(vl,C(m1),C(m2),&t);
					if ( t ) {
						NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = t;
					}
					m1 = NEXT(m1); m2 = NEXT(m2); break;
				case 1:
					NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = C(m1);
					m1 = NEXT(m1); break;
				case -1:
					NEXTMP(mr0,mr); mr->dl = m2->dl; C(mr) = C(m2);
					m2 = NEXT(m2); break;
			}
		if ( !mr0 )
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else {
				*pr = 0;
				return;
			}
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		MKDP(NV(p1),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = MAX(p1->sugar,p2->sugar);
	}
}

/* for F4 symbolic reduction */

void symb_addd(p1,p2,pr)
DP p1,p2,*pr;
{
	int n;
	MP m1,m2,mr,mr0;
	P t;

	if ( !p1 )
		*pr = p2;
	else if ( !p2 )
		*pr = p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			NEXTMP(mr0,mr); C(mr) = (P)ONE;
			switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
				case 0:
					mr->dl = m1->dl;
					m1 = NEXT(m1); m2 = NEXT(m2); break;
				case 1:
					mr->dl = m1->dl;
					m1 = NEXT(m1); break;
				case -1:
					mr->dl = m2->dl;
					m2 = NEXT(m2); break;
			}
		}
		if ( !mr0 )
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else {
				*pr = 0;
				return;
			}
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		MKDP(NV(p1),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = MAX(p1->sugar,p2->sugar);
	}
}

/* 
 * destructive merge of two list
 *
 * p1, p2 : list of DL
 * return : a merged list
 */

NODE symb_merge(m1,m2,n)
NODE m1,m2;
int n;
{
	NODE top,prev,cur,m,t;

	if ( !m1 )
		return m2;
	else if ( !m2 )
		return m1;
	else {
		switch ( (*cmpdl)(n,(DL)BDY(m1),(DL)BDY(m2)) ) {
			case 0:
				top = m1; m = NEXT(m2);
				break;
			case 1:
				top = m1; m = m2;
				break;
			case -1:
				top = m2; m = m1;
				break;
		}
		prev = top; cur = NEXT(top);
		/* BDY(prev) > BDY(m) always holds */
		while ( cur && m ) {
			switch ( (*cmpdl)(n,(DL)BDY(cur),(DL)BDY(m)) ) {
				case 0:
					m = NEXT(m);
					prev = cur; cur = NEXT(cur);
					break;
				case 1:
					t = NEXT(cur); NEXT(cur) = m; m = t;
					prev = cur; cur = NEXT(cur);
					break;
				case -1:
					NEXT(prev) = m; m = cur;
					prev = NEXT(prev); cur = NEXT(prev);
					break;
			}
		}
		if ( !cur )
			NEXT(prev) = m;
		return top;
	}
}

void subd(vl,p1,p2,pr)
VL vl;
DP p1,p2,*pr;
{
	DP t;

	if ( !p2 )
		*pr = p1;
	else {
		chsgnd(p2,&t); addd(vl,p1,t,pr);
	}
}

void chsgnd(p,pr)
DP p,*pr;
{
	MP m,mr,mr0;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); chsgnp(C(m),&C(mr)); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void muld(vl,p1,p2,pr)
VL vl;
DP p1,p2,*pr;
{
	if ( ! do_weyl )
		comm_muld(vl,p1,p2,pr);
	else
		weyl_muld(vl,p1,p2,pr);
}

void comm_muld(vl,p1,p2,pr)
VL vl;
DP p1,p2,*pr;
{
	MP m;
	DP s,t,u;
	int i,l,l1;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else if ( OID(p1) <= O_P )
		muldc(vl,p2,(P)p1,pr);
	else if ( OID(p2) <= O_P )
		muldc(vl,p1,(P)p2,pr);
	else {
		for ( m = BDY(p1), l1 = 0; m; m = NEXT(m), l1++ );
		for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
		if ( l1 < l ) {
			t = p1; p1 = p2; p2 = t;
			l = l1;
		}
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			muldm(vl,p1,w[i],&t); addd(vl,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

void muldm(vl,p,m0,pr)
VL vl;
DP p;
MP m0;
DP *pr;
{
	MP m,mr,mr0;
	P c;
	DL d;
	int n;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p), c = C(m0), d = m0->dl, n = NV(p); 
			m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			if ( NUM(C(m)) && RATN(C(m)) && NUM(c) && RATN(c) )
				mulq((Q)C(m),(Q)c,(Q *)&C(mr));
			else
				mulp(vl,C(m),c,&C(mr));
			adddl(n,m->dl,d,&mr->dl);
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar + m0->dl->td;
	}
}

void weyl_muld(vl,p1,p2,pr)
VL vl;
DP p1,p2,*pr;
{
	MP m;
	DP s,t,u;
	int i,l;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else if ( OID(p1) <= O_P )
		muldc(vl,p2,(P)p1,pr);
	else if ( OID(p2) <= O_P )
		muldc(vl,p1,(P)p2,pr);
	else {
		for ( m = BDY(p1), l = 0; m; m = NEXT(m), l++ );
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p1), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			weyl_muldm(vl,w[i],p2,&t); addd(vl,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

/* monomial * polynomial */

void weyl_muldm(vl,m0,p,pr)
VL vl;
MP m0;
DP p;
DP *pr;
{
	DP r,t,t1;
	MP m;
	DL d0;
	int n,n2,l,i,j,tlen;
	static MP *w,*psum;
	static struct cdl *tab;
	static int wlen;
	static int rtlen;

	if ( !p )
		*pr = 0;
	else {
		for ( m = BDY(p), l = 0; m; m = NEXT(m), l++ );
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;

		n = NV(p); n2 = n>>1;
		d0 = m0->dl;
		for ( i = 0, tlen = 1; i < n2; i++ )
			tlen *= d0->d[n2+i]+1;
		if ( tlen > rtlen ) {
			if ( tab ) GC_free(tab);
			if ( psum ) GC_free(psum);
			rtlen = tlen;
			tab = (struct cdl *)MALLOC(rtlen*sizeof(struct cdl));
			psum = (MP *)MALLOC(rtlen*sizeof(MP));
		}
		bzero(psum,tlen*sizeof(MP));
		for ( i = l-1; i >= 0; i-- ) {
			bzero(tab,tlen*sizeof(struct cdl));
			weyl_mulmm(vl,m0,w[i],n,tab,tlen);
			for ( j = 0; j < tlen; j++ ) {
				if ( tab[j].c ) {
					NEWMP(m); m->dl = tab[j].d; C(m) = tab[j].c; NEXT(m) = psum[j];
					psum[j] = m;
				}
			}
		}
		for ( j = tlen-1, r = 0; j >= 0; j-- ) 
			if ( psum[j] ) {
				MKDP(n,psum[j],t); addd(vl,r,t,&t1); r = t1;
			}
		if ( r )
			r->sugar = p->sugar + m0->dl->td;
		*pr = r;
	}
}

/* m0 = x0^d0*x1^d1*... * dx0^e0*dx1^e1*... */
/* rtab : array of length (e0+1)*(e1+1)*... */

void weyl_mulmm(vl,m0,m1,n,rtab,rtablen)
VL vl;
MP m0,m1;
int n;
struct cdl *rtab;
int rtablen;
{
	MP m,mr,mr0;
	DP r,t,t1;
	P c,c0,c1,cc;
	DL d,d0,d1,dt;
	int i,j,a,b,k,l,n2,s,min,curlen;
	struct cdl *p;
	static Q *ctab;
	static struct cdl *tab;
	static int tablen;
	static struct cdl *tmptab;
	static int tmptablen;

	
	if ( !m0 || !m1 ) {
		rtab[0].c = 0;
		rtab[0].d = 0;
		return;
	}
	c0 = C(m0); c1 = C(m1);
	mulp(vl,c0,c1,&c);
	d0 = m0->dl; d1 = m1->dl;
	n2 = n>>1;
	curlen = 1;
	NEWDL(d,n);
	if ( n & 1 )
		/* offset of h-degree */
	 	d->td = d->d[n-1] = d0->d[n-1]+d1->d[n-1];
	else
		d->td = 0;
	rtab[0].c = c;
	rtab[0].d = d;

	if ( rtablen > tmptablen ) {
		if ( tmptab ) GC_free(tmptab);
		tmptab = (struct cdl *)MALLOC(rtablen*sizeof(struct cdl));
		tmptablen = rtablen;
	}
	for ( i = 0; i < n2; i++ ) {
		a = d0->d[i]; b = d1->d[n2+i];
		k = d0->d[n2+i]; l = d1->d[i];
		if ( !k || !l ) {
			a += l;
			b += k;
			s = a+b;
			for ( j = 0, p = rtab; j < curlen; j++, p++ ) {
				if ( p->c ) {
					dt = p->d;
					dt->d[i] = a;
					dt->d[n2+i] = b;
					dt->td += s;
				}
			}
			curlen *= k+1;
			continue;
		}
		if ( k+1 > tablen ) {
			if ( tab ) GC_free(tab);
			if ( ctab ) GC_free(ctab);
			tablen = k+1;
			tab = (struct cdl *)MALLOC(tablen*sizeof(struct cdl));
			ctab = (Q *)MALLOC(tablen*sizeof(Q));
		}
		/* degree of xi^a*(Di^k*xi^l)*Di^b */
		s = a+k+l+b;
		/* compute xi^a*(Di^k*xi^l)*Di^b */
		min = MIN(k,l);
		mkwc(k,l,ctab);
		bzero(tab,(k+1)*sizeof(struct cdl));
		if ( n & 1 )
			for ( j = 0; j <= min; j++ ) {
				NEWDL(d,n);
				d->d[i] = l-j+a; d->d[n2+i] = k-j+b;
				d->td = s;
				d->d[n-1] = s-(d->d[i]+d->d[n2+i]); 
				tab[j].d = d;
				tab[j].c = (P)ctab[j];
			}
		else
			for ( j = 0; j <= min; j++ ) {
				NEWDL(d,n);
				d->d[i] = l-j+a; d->d[n2+i] = k-j+b;
				d->td = d->d[i]+d->d[n2+i]; /* XXX */
				tab[j].d = d;
				tab[j].c = (P)ctab[j];
			}
		bzero(ctab,(min+1)*sizeof(Q));
		comm_muld_tab(vl,n,rtab,curlen,tab,k+1,tmptab);
		curlen *= k+1;
		bcopy(tmptab,rtab,curlen*sizeof(struct cdl));
	}
}

/* direct product of two cdl tables
  rt[] = [
    t[0]*t1[0],...,t[n-1]*t1[0],
    t[0]*t1[1],...,t[n-1]*t1[1],
    ...
    t[0]*t1[n1-1],...,t[n-1]*t1[n1-1]
  ]
*/

void comm_muld_tab(vl,nv,t,n,t1,n1,rt)
VL vl;
int nv;
struct cdl *t;
int n;
struct cdl *t1;
int n1;
struct cdl *rt;
{
	int i,j;
	struct cdl *p;
	P c;
	DL d;

	bzero(rt,n*n1*sizeof(struct cdl));
	for ( j = 0, p = rt; j < n1; j++ ) {
		c = t1[j].c;
		d = t1[j].d;
		if ( !c )
			break;
		for ( i = 0; i < n; i++, p++ ) {
			if ( t[i].c ) {
				mulp(vl,t[i].c,c,&p->c);
				adddl(nv,t[i].d,d,&p->d);
			}
		}
	}
}

void muldc(vl,p,c,pr)
VL vl;
DP p;
P c;
DP *pr;
{
	MP m,mr,mr0;

	if ( !p || !c )
		*pr = 0;
	else if ( NUM(c) && UNIQ((Q)c) )
		*pr = p;
	else if ( NUM(c) && MUNIQ((Q)c) )
		chsgnd(p,pr);
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			if ( NUM(C(m)) && RATN(C(m)) && NUM(c) && RATN(c) )
				mulq((Q)C(m),(Q)c,(Q *)&C(mr));
			else
				mulp(vl,C(m),c,&C(mr));
			mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void divsdc(vl,p,c,pr)
VL vl;
DP p;
P c;
DP *pr;
{
	MP m,mr,mr0;

	if ( !c )
		error("disvsdc : division by 0");
	else if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); divsp(vl,C(m),c,&C(mr)); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void adddl(n,d1,d2,dr)
int n;
DL d1,d2;
DL *dr;
{
	DL dt;
	int i;

	if ( !d1->td )
		*dr = d2;
	else if ( !d2->td )
		*dr = d1;
	else {
		*dr = dt = (DL)MALLOC_ATOMIC((n+1)*sizeof(int));
		dt->td = d1->td + d2->td;
		for ( i = 0; i < n; i++ )
			dt->d[i] = d1->d[i]+d2->d[i];
	}
}

/* d1 += d2 */

void adddl_destructive(n,d1,d2)
int n;
DL d1,d2;
{
	DL dt;
	int i;

	d1->td += d2->td;
	for ( i = 0; i < n; i++ )
		d1->d[i] += d2->d[i];
}

int compd(vl,p1,p2)
VL vl;
DP p1,p2;
{
	int n,t;
	MP m1,m2;

	if ( !p1 )
		return p2 ? -1 : 0;
	else if ( !p2 )
		return 1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2);
			m1 && m2; m1 = NEXT(m1), m2 = NEXT(m2) )
			if ( (t = (*cmpdl)(n,m1->dl,m2->dl)) ||
				(t = compp(vl,C(m1),C(m2)) ) )
				return t;
		if ( m1 )
			return 1;
		else if ( m2 )
			return -1;
		else
			return 0;
	}
}

int cmpdl_lex(n,d1,d2)
int n;
DL d1,d2;
{
	int i;

	for ( i = 0; i < n && d1->d[i] == d2->d[i]; i++ );
	return i == n ? 0 : (d1->d[i] > d2->d[i] ? 1 : -1);
}

int cmpdl_revlex(n,d1,d2)
int n;
DL d1,d2;
{
	int i;

	for ( i = n - 1; i >= 0 && d1->d[i] == d2->d[i]; i-- );
	return i < 0 ? 0 : (d1->d[i] < d2->d[i] ? 1 : -1);
}

int cmpdl_gradlex(n,d1,d2)
int n;
DL d1,d2;
{
	if ( d1->td > d2->td )
		return 1;
	else if ( d1->td < d2->td )
		return -1;
	else
		return cmpdl_lex(n,d1,d2);
}

int cmpdl_revgradlex(n,d1,d2)
int n;
DL d1,d2;
{
	register int i;
	register int *p1,*p2;

	if ( d1->td > d2->td )
		return 1;
	else if ( d1->td < d2->td )
		return -1;
	else {
		for ( i= n - 1, p1 = d1->d+n-1, p2 = d2->d+n-1;
			i >= 0 && *p1 == *p2; i--, p1--, p2-- );
		return i < 0 ? 0 : (*p1 < *p2 ? 1 : -1);
	}
}

int cmpdl_blex(n,d1,d2)
int n;
DL d1,d2;
{
	int c;

	if ( c = cmpdl_lex(n-1,d1,d2) )
		return c;
	else {
		c = d1->d[n-1] - d2->d[n-1];
		return c > 0 ? 1 : c < 0 ? -1 : 0;
	}
}

int cmpdl_bgradlex(n,d1,d2)
int n;
DL d1,d2;
{
	int e1,e2,c;

	e1 = d1->td - d1->d[n-1]; e2 = d2->td - d2->d[n-1];
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else {
		c = cmpdl_lex(n-1,d1,d2);
		if ( c )
			return c;
		else
			return d1->td > d2->td ? 1 : d1->td < d2->td ? -1 : 0;
	}
}

int cmpdl_brevgradlex(n,d1,d2)
int n;
DL d1,d2;
{
	int e1,e2,c;

	e1 = d1->td - d1->d[n-1]; e2 = d2->td - d2->d[n-1];
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else {
		c = cmpdl_revlex(n-1,d1,d2);
		if ( c )
			return c;
		else
			return d1->td > d2->td ? 1 : d1->td < d2->td ? -1 : 0;
	}
}

int cmpdl_brevrev(n,d1,d2)
int n;
DL d1,d2;
{
	int e1,e2,f1,f2,c,i;

	for ( i = 0, e1 = 0, e2 = 0; i < dp_nelim; i++ ) {
		e1 += d1->d[i]; e2 += d2->d[i];
	}
	f1 = d1->td - e1; f2 = d2->td - e2;
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else {
		c = cmpdl_revlex(dp_nelim,d1,d2);
		if ( c )
			return c;
		else if ( f1 > f2 )
			return 1;
		else if ( f1 < f2 )
			return -1;
		else {
			for ( i = n - 1; i >= dp_nelim && d1->d[i] == d2->d[i]; i-- );
			return i < dp_nelim ? 0 : (d1->d[i] < d2->d[i] ? 1 : -1);
		}
	}
}

int cmpdl_bgradrev(n,d1,d2)
int n;
DL d1,d2;
{
	int e1,e2,f1,f2,c,i;

	for ( i = 0, e1 = 0, e2 = 0; i < dp_nelim; i++ ) {
		e1 += d1->d[i]; e2 += d2->d[i];
	}
	f1 = d1->td - e1; f2 = d2->td - e2;
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else {
		c = cmpdl_lex(dp_nelim,d1,d2);
		if ( c )
			return c;
		else if ( f1 > f2 )
			return 1;
		else if ( f1 < f2 )
			return -1;
		else {
			for ( i = n - 1; i >= dp_nelim && d1->d[i] == d2->d[i]; i-- );
			return i < dp_nelim ? 0 : (d1->d[i] < d2->d[i] ? 1 : -1);
		}
	}
}

int cmpdl_blexrev(n,d1,d2)
int n;
DL d1,d2;
{
	int e1,e2,f1,f2,c,i;

	for ( i = 0, e1 = 0, e2 = 0; i < dp_nelim; i++ ) {
		e1 += d1->d[i]; e2 += d2->d[i];
	}
	f1 = d1->td - e1; f2 = d2->td - e2;
	c = cmpdl_lex(dp_nelim,d1,d2);
	if ( c )
		return c;
	else if ( f1 > f2 )
		return 1;
	else if ( f1 < f2 )
		return -1;
	else {
		for ( i = n - 1; i >= dp_nelim && d1->d[i] == d2->d[i]; i-- );
		return i < dp_nelim ? 0 : (d1->d[i] < d2->d[i] ? 1 : -1);
	}
}

int cmpdl_elim(n,d1,d2)
int n;
DL d1,d2;
{
	int e1,e2,i;

	for ( i = 0, e1 = 0, e2 = 0; i < dp_nelim; i++ ) {
		e1 += d1->d[i]; e2 += d2->d[i];
	}
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else 
		return cmpdl_revgradlex(n,d1,d2);
}

int cmpdl_weyl_elim(n,d1,d2)
int n;
DL d1,d2;
{
	int e1,e2,i;

	for ( i = 1, e1 = 0, e2 = 0; i <= dp_nelim; i++ ) {
		e1 += d1->d[n-i]; e2 += d2->d[n-i];
	}
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;
	else if ( d1->td > d2->td )
		return 1;
	else if ( d1->td < d2->td )
		return -1;
	else return -cmpdl_revlex(n,d1,d2);
}

/*
	a special ordering
	1. total order
	2. (-w,w) for the first 2*m variables
	3. DRL for the first 2*m variables
*/

extern int *current_weight_vector;

int cmpdl_homo_ww_drl(n,d1,d2)
int n;
DL d1,d2;
{
	int e1,e2,m,i;
	int *p1,*p2;

	if ( d1->td > d2->td )
		return 1;
	else if ( d1->td < d2->td )
		return -1;

	m = n>>1;
	for ( i = 0, e1 = e2 = 0; i < m; i++ ) {
		e1 += current_weight_vector[i]*(d1->d[m+i] - d1->d[i]);
		e2 += current_weight_vector[i]*(d2->d[m+i] - d2->d[i]);
	}
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;

	e1 = d1->td - d1->d[n-1];
	e2 = d2->td - d2->d[n-1];
	if ( e1 > e2 )
		return 1;
	else if ( e1 < e2 )
		return -1;

	for ( i= n - 1, p1 = d1->d+n-1, p2 = d2->d+n-1;
		i >= 0 && *p1 == *p2; i--, p1--, p2-- );
	return i < 0 ? 0 : (*p1 < *p2 ? 1 : -1);
}

int cmpdl_order_pair(n,d1,d2)
int n;
DL d1,d2;
{
	int e1,e2,i,j,l;
	int *t1,*t2;
	int len;
	struct order_pair *pair;

	len = dp_current_spec.ord.block.length;
	pair = dp_current_spec.ord.block.order_pair;

	for ( i = 0, t1 = d1->d, t2 = d2->d; i < len; i++ ) {
		l = pair[i].length;
		switch ( pair[i].order ) {
			case 0:
				for ( j = 0, e1 = e2 = 0; j < l; j++ ) {
					e1 += t1[j]; e2 += t2[j];
				}
				if ( e1 > e2 )
					return 1;
				else if ( e1 < e2 )
					return -1;
				else {
					for ( j = l - 1; j >= 0 && t1[j] == t2[j]; j-- );
					if ( j >= 0 )
						return t1[j] < t2[j] ? 1 : -1;
				}
				break;
			case 1:
				for ( j = 0, e1 = e2 = 0; j < l; j++ ) {
					e1 += t1[j]; e2 += t2[j];
				}
				if ( e1 > e2 )
					return 1;
				else if ( e1 < e2 )
					return -1;
				else {
					for ( j = 0; j < l && t1[j] == t2[j]; j++ );
					if ( j < l )
						return t1[j] > t2[j] ? 1 : -1;
				}
				break;
			case 2:
				for ( j = 0; j < l && t1[j] == t2[j]; j++ );
				if ( j < l )
					return t1[j] > t2[j] ? 1 : -1;
				break;
			default:
				error("cmpdl_order_pair : invalid order"); break;
		}
		t1 += l; t2 += l;
	}
	return 0;
}

int cmpdl_matrix(n,d1,d2)
int n;
DL d1,d2;
{
	int *v,*w,*t1,*t2;
	int s,i,j,len;
	int **matrix;

	for ( i = 0, t1 = d1->d, t2 = d2->d, w = dp_dl_work; i < n; i++ )
		w[i] = t1[i]-t2[i];
	len = dp_current_spec.ord.matrix.row;
	matrix = dp_current_spec.ord.matrix.matrix;
	for ( j = 0; j < len; j++ ) {
		v = matrix[j];
		for ( i = 0, s = 0; i < n; i++ )
			s += v[i]*w[i];	
		if ( s > 0 )
			return 1;
		else if ( s < 0 )
			return -1;
	}
	return 0;
}
