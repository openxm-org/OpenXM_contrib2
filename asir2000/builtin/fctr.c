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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/fctr.c,v 1.10 2001/11/19 00:57:10 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Pfctr(), Pgcd(), Pgcdz(), Plcm(), Psqfr(), Pufctrhint();
void Pptozp(), Pcont();
void Pafctr(), Pagcd();
void Pmodsqfr(),Pmodfctr(),Pddd(),Pnewddd(),Pddd_tab();
void Psfsqfr(),Psfbfctr(),Psfufctr(),Psfmintdeg();
void Pirred_check(), Pnfctr_mod();

void sfmintdeg(VL vl,P fx,int dy,int c,P *fr);
void create_bmono(P c,V x,int i,V y,int j,P *mono);

struct ftab fctr_tab[] = {
	{"fctr",Pfctr,-2},
	{"gcd",Pgcd,-3},
	{"gcdz",Pgcdz,2},
	{"lcm",Plcm,2},
	{"sqfr",Psqfr,1},
	{"ufctrhint",Pufctrhint,2},
	{"ptozp",Pptozp,1},
	{"cont",Pcont,-2},
	{"afctr",Pafctr,2},
	{"agcd",Pagcd,3},
	{"modsqfr",Pmodsqfr,2},
	{"modfctr",Pmodfctr,2},
	{"sfsqfr",Psfsqfr,1},
	{"sfufctr",Psfufctr,1},
	{"sfbfctr",Psfbfctr,-4},
	{"sfmintdeg",Psfmintdeg,5},
#if 0
	{"ddd",Pddd,2},
	{"newddd",Pnewddd,2},
#endif
	{"ddd_tab",Pddd_tab,2},
	{"irred_check",Pirred_check,2},
	{"nfctr_mod",Pnfctr_mod,2},
	{0,0,0},
};

void Pfctr(arg,rp)
NODE arg;
LIST *rp;
{
	DCP dc;

	asir_assert(ARG0(arg),O_P,"fctr");
	if ( argc(arg) == 1 )
		fctrp(CO,(P)ARG0(arg),&dc);
	else {
		asir_assert(ARG1(arg),O_P,"fctr");
		fctr_wrt_v_p(CO,(P)ARG0(arg),VR((P)ARG1(arg)),&dc);
	}
	dcptolist(dc,rp);
}

void Pgcd(arg,rp)
NODE arg;
P *rp;
{
	P p1,p2,g1,g2,g;
	Num m;
	int mod;

	p1 = (P)ARG0(arg); p2 = (P)ARG1(arg);
	asir_assert(p1,O_P,"gcd");
	asir_assert(p2,O_P,"gcd");
	if ( !p1 )
		*rp = p2;
	else if ( !p2 )
		*rp = p1;
	else if ( !qpcheck((Obj)p1) || !qpcheck((Obj)p2) )
		gcdprsp(CO,p1,p2,rp);
	else if ( argc(arg) == 2 )
		ezgcdp(CO,p1,p2,rp);
	else {
		m = (Num)ARG2(arg);
		asir_assert(m,O_P,"gcd");
		mod = QTOS((Q)m);
		ptomp(mod,p1,&g1); ptomp(mod,p2,&g2);
		gcdprsmp(CO,mod,g1,g2,&g);
		mptop(g,rp);
	}
}

void Pgcdz(arg,rp)
NODE arg;
P *rp;
{
	P p1,p2,t;
	Q c1,c2;
	N n;

	p1 = (P)ARG0(arg); p2 = (P)ARG1(arg);
	asir_assert(p1,O_P,"gcdz");
	asir_assert(p2,O_P,"gcdz");
	if ( !p1 )
		*rp = p2;
	else if ( !p2 )
		*rp = p1;
	else if ( !qpcheck((Obj)p1) || !qpcheck((Obj)p2) )
		error("gcdz : invalid argument");
	else if ( NUM(p1) || NUM(p2) ) {
		if ( NUM(p1) )
			c1 = (Q)p1;
		else
			ptozp(p1,1,&c1,&t);
		if ( NUM(p2) )
			c2 = (Q)p2;
		else
			ptozp(p2,1,&c2,&t);
		gcdn(NM(c1),NM(c2),&n); NTOQ(n,1,c1); *rp = (P)c1;
	} else {
#if 0
		w[0] = p1; w[1] = p2; nezgcdnpz(CO,w,2,rp);
#endif
		ezgcdpz(CO,p1,p2,rp);
	}
}

void Plcm(arg,rp)
NODE arg;
P *rp;
{
	P t1,t2,p1,p2,g,q;
	Q c;

	p1 = (P)ARG0(arg); p2 = (P)ARG1(arg);
	asir_assert(p1,O_P,"lcm");
	asir_assert(p2,O_P,"lcm");
	if ( !p1 || !p2 )
		*rp = 0;
	else if ( !qpcheck((Obj)p1) || !qpcheck((Obj)p2) )
		error("lcm : invalid argument");
	else {
		ptozp(p1,1,&c,&t1); ptozp(p2,1,&c,&t2);
		ezgcdp(CO,t1,t2,&g); divsp(CO,t1,g,&q); mulp(CO,q,t2,rp);
	}
}

void Psqfr(arg,rp)
NODE arg;
LIST *rp;
{
	DCP dc;

	asir_assert(ARG0(arg),O_P,"sqfr");
	sqfrp(CO,(P)ARG0(arg),&dc);
	dcptolist(dc,rp);
}

void Pufctrhint(arg,rp)
NODE arg;
LIST *rp;
{
	DCP dc;

	asir_assert(ARG0(arg),O_P,"ufctrhint");
	asir_assert(ARG1(arg),O_N,"ufctrhint");
	ufctr((P)ARG0(arg),QTOS((Q)ARG1(arg)),&dc);
	dcptolist(dc,rp);
}

#if 0
Pmgcd(arg,rp)
NODE arg;
Obj *rp;
{
	NODE node,tn;
	int i,m;
	P *l;

	node = BDY((LIST)ARG0(arg));
	for ( i = 0, tn = node; tn; tn = NEXT(tn), i++ );
	m = i; l = (P *)ALLOCA(m*sizeof(P));
	for ( i = 0, tn = node; i < m; tn = NEXT(tn), i++ )
		l[i] = (P)BDY(tn);
	nezgcdnpz(CO,l,m,rp);
}
#endif

void Pcont(arg,rp)
NODE arg;
P *rp;
{
	DCP dc;
	int m;
	P p,p1;
	P *l;
	V v;

	asir_assert(ARG0(arg),O_P,"cont");
	p = (P)ARG0(arg);
	if ( NUM(p) )
		*rp = p;
	else {
		if ( argc(arg) == 2 ) {
			v = VR((P)ARG1(arg));
			change_mvar(CO,p,v,&p1);
			if ( VR(p1) != v ) {
				*rp = p1; return;
			} else
				p = p1;
		}
		for ( m = 0, dc = DC(p); dc; dc = NEXT(dc), m++ );
		l = (P *)ALLOCA(m*sizeof(P));
		for ( m = 0, dc = DC(p); dc; dc = NEXT(dc), m++ )
			l[m] = COEF(dc);
		nezgcdnpz(CO,l,m,rp);
	}
}

void Pptozp(arg,rp)
NODE arg;
P *rp;
{
	Q t;

	asir_assert(ARG0(arg),O_P,"ptozp");
	ptozp((P)ARG0(arg),1,&t,rp);
}

void Pafctr(arg,rp)
NODE arg;
LIST *rp;
{
	DCP dc;
	
	asir_assert(ARG0(arg),O_P,"afctr");
	asir_assert(ARG1(arg),O_P,"afctr");
	afctr(CO,(P)ARG0(arg),(P)ARG1(arg),&dc);
	dcptolist(dc,rp);
}

void Pagcd(arg,rp)
NODE arg;
P *rp;
{
	asir_assert(ARG0(arg),O_P,"agcd");
	asir_assert(ARG1(arg),O_P,"agcd");
	asir_assert(ARG2(arg),O_P,"agcd");
	gcda(CO,(P)ARG0(arg),(P)ARG1(arg),(P)ARG2(arg),rp);
}

#if 1
#define Mulum mulum
#define Divum divum
#define Mulsum mulsum
#define Gcdum gcdum
#endif

void Mulum(), Mulsum(), Gcdum();
int Divum();

#define FCTR 0 /* berlekamp */
#define SQFR 1
#define DDD 2  /* Cantor-Zassenhauss */
#define NEWDDD 3  /* berlekamp + root-finding by Cantor-Zassenhauss */

UM *resberle();

void Pmodfctr(arg,rp)
NODE arg;
LIST *rp;
{
	DCP dc;
	int mod;

	mod = QTOS((Q)ARG1(arg));
	if ( mod < 0 )
		error("modfctr : invalid modulus");
	modfctrp(ARG0(arg),mod,NEWDDD,&dc);
	if ( !dc ) {
		NEWDC(dc); COEF(dc) = 0; DEG(dc) = ONE; NEXT(dc) = 0;
	}
	dcptolist(dc,rp);
}

void Psfsqfr(arg,rp)
NODE arg;
LIST *rp;
{
	DCP dc;

	sfsqfr(ARG0(arg),&dc);
	dcptolist(dc,rp);
}

void Psfufctr(arg,rp)
NODE arg;
LIST *rp;
{
	DCP dc;

	fctrsf(ARG0(arg),&dc);
	dcptolist(dc,rp);
}

void Psfbfctr(arg,rp)
NODE arg;
LIST *rp;
{
	V x,y;
	DCP dc,dct;
	P t;
	struct oVL vl1,vl2;
	VL vl;
	int degbound;

	x = VR((P)ARG1(arg));
	y = VR((P)ARG2(arg));
	vl1.v = x; vl1.next = &vl2;
	vl2.v = y; vl2.next = 0;
	vl = &vl1;
	if ( argc(arg) == 4 )
		degbound = QTOS((Q)ARG3(arg));
	else
		degbound = -1;

	sfbfctr((P)ARG0(arg),x,y,degbound,&dc);
	for ( dct = dc; dct; dct = NEXT(dct) ) {
		reorderp(CO,vl,COEF(dct),&t); COEF(dct) = t;
	}
	dcptolist(dc,rp);
}

void Psfmintdeg(arg,rp)
NODE arg;
P *rp;
{
	V x,y;
	P r;
	struct oVL vl1,vl2;
	VL vl;
	int dy,c;

	x = VR((P)ARG1(arg));
	y = VR((P)ARG2(arg));
	vl1.v = x; vl1.next = &vl2;
	vl2.v = y; vl2.next = 0;
	vl = &vl1;
	dy = QTOS((Q)ARG3(arg));
	c = QTOS((Q)ARG4(arg));
	sfmintdeg(vl,(P)ARG0(arg),dy,c,&r);
	reorderp(CO,vl,r,rp);
}

void Pmodsqfr(arg,rp)
NODE arg;
LIST *rp;
{
	DCP dc;

	if ( !ARG0(arg) ) {
		NEWDC(dc); COEF(dc) = 0; DEG(dc) = ONE; NEXT(dc) = 0;
	} else
		modfctrp(ARG0(arg),QTOS((Q)ARG1(arg)),SQFR,&dc);
	dcptolist(dc,rp);
}

void Pddd(arg,rp)
NODE arg;
LIST *rp;
{
	DCP dc;

	if ( !ARG0(arg) ) {
		NEWDC(dc); COEF(dc) = 0; DEG(dc) = ONE; NEXT(dc) = 0;
	} else
		modfctrp(ARG0(arg),QTOS((Q)ARG1(arg)),DDD,&dc);
	dcptolist(dc,rp);
}

void Pnewddd(arg,rp)
NODE arg;
LIST *rp;
{
	DCP dc=0;

	if ( !ARG0(arg) ) {
		NEWDC(dc); COEF(dc) = 0; DEG(dc) = ONE; NEXT(dc) = 0;
	} else 
		modfctrp(ARG0(arg),QTOS((Q)ARG1(arg)),NEWDDD,&dc);
	dcptolist(dc,rp);
}

void Pirred_check(arg,rp)
NODE arg;
Q *rp;
{
	P p;
	UM mp;
	int r,mod;

	p = (P)ARG0(arg);
	if ( !p ) {
		*rp = 0; return;
	}
	mp = W_UMALLOC(UDEG(p));
	mod = QTOS((Q)ARG1(arg));
	ptoum(mod,p,mp);
	r = irred_check(mp,mod);
	if ( r )
		*rp = ONE;
	else
		*rp = 0;
}

void Pnfctr_mod(arg,rp)
NODE arg;
Q *rp;
{
	P p;
	UM mp;
	int r,mod;

	p = (P)ARG0(arg);
	if ( !p ) {
		*rp = 0; return;
	}
	mp = W_UMALLOC(UDEG(p));
	mod = QTOS((Q)ARG1(arg));
	ptoum(mod,p,mp);
	r = nfctr_mod(mp,mod);
	STOQ(r,*rp);
}

void Pddd_tab(arg,rp)
NODE arg;
VECT *rp;
{
	P p;
	UM mp,t,q,r1,w,w1;
	UM *r,*s;
	int dr,mod,n,i;
	VECT result;
	V v;

	p = (P)ARG0(arg); mod = QTOS((Q)ARG1(arg));
	v = VR(p);
	n = UDEG(p); mp = W_UMALLOC(n);
	ptoum(mod,p,mp);
	r = (UM *)W_ALLOC(n); s = (UM *)W_ALLOC(n);
	r[0] = UMALLOC(0); DEG(r[0]) = 0; COEF(r[0])[0] = 1;
	t = W_UMALLOC(mod); bzero(COEF(t),sizeof(int)*(mod+1));
	DEG(t) = mod; COEF(t)[mod] = 1;
	q = W_UMALLOC(mod);
	dr = divum(mod,t,mp,q);
	DEG(t) = dr; r[1] = r1 = UMALLOC(dr); cpyum(t,r1);
	s[0] = W_UMALLOC(dr); cpyum(t,s[0]);
	w = W_UMALLOC(n); bzero(COEF(w),sizeof(int)*(n+1));
	w1 = W_UMALLOC(2*n); bzero(COEF(w1),sizeof(int)*(2*n+1));
	for ( i = 1; i < n; i++ ) {
		DEG(w) = i; COEF(w)[i-1] = 0; COEF(w)[i] = 1;
		mulum(mod,r1,w,w1);
		dr = divum(mod,w1,mp,q); DEG(w1) = dr;
		s[i] = W_UMALLOC(dr); cpyum(w1,s[i]);
	}
	for ( i = 2; i < n; i++ ) {
		mult_mod_tab(r[i-1],mod,s,w,n);
		r[i] = UMALLOC(DEG(w)); cpyum(w,r[i]);
	}
	MKVECT(result,n);
	for ( i = 0; i < n; i++ )
		umtop(v,r[i],(P *)&BDY(result)[i]);
	*rp = result;
}

struct lb {
	int pos,len;
	int *r;
	int *hist;
};

static NODE insert_lb(NODE g,struct lb *a)
{
	NODE prev,cur,n;

	prev = 0; cur = g;
	while ( cur ) {
		if ( a->pos < ((struct lb *)BDY(cur))->pos ) {
			MKNODE(n,a,cur);
			if ( !prev )
				return n;
			else {
				NEXT(prev) = n;
				return g;
			}
		} else {
			prev = cur;
			cur = NEXT(cur);
		}
	}
	MKNODE(n,a,0);
	NEXT(prev) = n;
	return g;
}

static void lnf(int *r,int *h,int n,int len,NODE g)
{
	struct lb *t;
	int pos,i,j,len1,c;
	int *r1,*h1;

	for ( ; g; g = NEXT(g) ) {
		t = (struct lb *)BDY(g);
		pos = t->pos;
		if ( c = r[pos] ) {
			r1 = t->r;
			h1 = t->hist;
			len1 = t->len;
			for ( i = pos; i < n; i++ )
				r[i] = _subsf(r[i],_mulsf(r1[i],c));
			for ( i = 0; i < len1; i++ )
				h[i] = _subsf(h[i],_mulsf(h1[i],c));
		}
	}
	for ( i = 0; i < n && !r[i]; i++ );
	if ( i < n ) {
		c = _invsf(r[i]);
		for ( j = i; j < n; j++ )
			r[j] = _mulsf(r[j],c);	
		for ( j = i; j < len; j++ )
			h[j] = _mulsf(h[j],c);	
	}
}

void print_vect(int *r,int len)
{
	int i;

	for ( i = 0; i < len; i++ )
		if ( r[i] ) printf("(%d %d)",i,IFTOF(r[i]));
	printf("\n");
}

void sfmintdeg(VL vl,P fx,int dy,int c,P *fr)
{
	V x,y;
	int dx,dxdy,i,j,k,l,d,len,len0,u,dyk;
	UP *rx;
	DCP dc;
	P t,f,mono,f1;
	UP ut,h;
	int ***nf;
	int *r,*hist,*prev,*r1;
	struct lb *lb;
	GFS s;
	NODE g;

	x = vl->v;
	y = NEXT(vl)->v;
	dx = getdeg(x,fx);
	dxdy = dx*dy;
	/* rx = -(fx-x^dx) */
	rx = (UP *)CALLOC(dx,sizeof(UP));
	for ( dc = DC(fx); dc; dc = NEXT(dc)) {
		chsgnp(COEF(dc),&t);
		ptoup(t,&ut);
		rx[QTOS(DEG(dc))] = ut;
	}
	/* nf[d] = normal form table of monomials with total degree d */
	nf = (int ***)CALLOC(dx+dy+1,sizeof(int **)); /* xxx */
	nf[0] = (int **)CALLOC(1,sizeof(int *));

	/* nf[0][0] = 1 */
	r = (int *)CALLOC(dxdy,sizeof(int));
	r[0] = _onesf();
	nf[0][0] = r;

	hist = (int *)CALLOC(1,sizeof(int));
	r[0] = _onesf();

	lb = (struct lb *)CALLOC(1,sizeof(struct lb));
	lb->pos = 0;
	lb->r = r;
	lb->hist = hist;
	lb->len = 1;

	/* g : table of normal form as linear form */
	MKNODE(g,lb,0);

	len = 1;
	h = UPALLOC(dy);
	for ( d = 1; ; d++ ) {
		if ( d > c ){		
			return;
		}
		nf[d] = (int **)CALLOC(d+1,sizeof(int *));
		len0 = len;
		len += d+1;

		for ( i = d; i >= 0; i-- ) {
			/* nf(x^(d-i)*y^i) = nf(y*nf(x^(d-i)*y^(i-1))) */
			/* nf(x^d) = nf(nf(x^(d-1))*x) */
			r = (int *)CALLOC(dxdy,sizeof(int));
			if ( i == 0 ) {
				prev = nf[d-1][0];
				bcopy(prev,r+dy,(dxdy-dy)*sizeof(int));

				/* create the head coeff */
				for ( l = 0, k = dxdy-dy; l < dy; l++, k++ ) {
					if ( prev[k] ) {
						u = IFTOF(prev[k]);
						MKGFS(u,s);
					} else
						s = 0;
					COEF(h)[l] = (Num)s;
				}
				for ( l = dy-1; l >= 0 && !COEF(h)[l]; l--);
				DEG(h) = l;

				for ( k = 0, dyk = 0; k < dx; k++, dyk += dy ) {
					tmulup(rx[k],h,dy,&ut);
					if ( ut ) 
						for ( l = 0; l < dy; l++ ) {
							s = (GFS)COEF(ut)[l];
							if ( s ) {
								u = CONT(s);
								r[dyk+l] = _addsf(r[dyk+l],FTOIF(u));
							}
						}
				}
			} else {
				prev = nf[d-1][i-1];
				for ( k = 0, dyk = 0; k < dx; k++, dyk += dy ) {
					for ( l = 1; l < dy; l++ )
						r[dyk+l] = prev[dyk+l-1];
				}
			}
			nf[d][i] = r;
			hist = (int *)CALLOC(len,sizeof(int));
			hist[len0+i] = _onesf();
			r1 = (int *)CALLOC(dxdy,sizeof(int));
			bcopy(r,r1,dxdy*sizeof(int));
			lnf(r1,hist,dxdy,len,g);
			for ( k = 0; k < dxdy && !r1[k]; k++ );
			if ( k == dxdy ) {
				f = 0;
				for ( k = j = 0; k <= d; k++ )
					for ( i = 0; i <= k; i++, j++ )
						if ( hist[j] ) {
							u = IFTOF(hist[j]);
							MKGFS(u,s);
							/* mono = s*x^(k-i)*y^i */
							create_bmono((P)s,x,k-i,y,i,&mono);
							addp(vl,f,mono,&f1);
							f = f1;
						}
				*fr = f;
				return;
			}	else {
				lb = (struct lb *)CALLOC(1,sizeof(struct lb));
				lb->pos = k;
				lb->r = r1;
				lb->hist = hist;
				lb->len = len;
				g = insert_lb(g,lb);
			}
		}
	}
}

void create_bmono(P c,V x,int i,V y,int j,P *mono)
{
	P t,s;

	if ( !i )
		if ( !j )
			t = c;
		else {
			/* c*y^j */
			MKV(y,t);
			COEF(DC(t)) = c;
			STOQ(j,DEG(DC(t)));
		}
	else if ( !j ) {
		/* c*x^i */
		MKV(x,t);
		COEF(DC(t)) = c;
		STOQ(i,DEG(DC(t)));
	} else {
		MKV(y,s);
		COEF(DC(s)) = c;
		STOQ(j,DEG(DC(s)));
		MKV(x,t);
		COEF(DC(t)) = s;
		STOQ(i,DEG(DC(t)));
	}
	*mono = t;
}
