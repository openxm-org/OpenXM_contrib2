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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/int.c,v 1.5 2000/08/22 05:03:58 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "base.h"

void Pidiv(), Pirem(), Pigcd(), Pilcm(), Pfac(), Prandom(), Pinv();
void Pup2_inv(),Pgf2nton(), Pntogf2n();
void Pup2_init_eg(), Pup2_show_eg();
void Piqr(), Pprime(), Plprime(), Pinttorat();
void Piand(), Pior(), Pixor(), Pishift();
void Pisqrt();
void iand(), ior(), ixor();
void isqrt();
void Plrandom();
void Pset_upkara(), Pset_uptkara(), Pset_up2kara(), Pset_upfft();
void Pmt_save(), Pmt_load();
void Psmall_jacobi();
void Pdp_set_mpi();
void Pntoint32(),Pint32ton();

#ifdef HMEXT
void Pigcdbin(), Pigcdbmod(), PigcdEuc(), Pigcdacc(), Pigcdcntl();

void Pihex();
void Pimaxrsh(), Pilen();
void Ptype_t_NB();

#endif /* HMEXT */

struct ftab int_tab[] = {
	{"dp_set_mpi",Pdp_set_mpi,-1},
	{"isqrt",Pisqrt,1},
	{"idiv",Pidiv,2},
	{"irem",Pirem,2},
	{"iqr",Piqr,2},
	{"igcd",Pigcd,-2},
	{"ilcm",Pilcm,2},
	{"up2_inv",Pup2_inv,2},
	{"up2_init_eg",Pup2_init_eg,0},
	{"up2_show_eg",Pup2_show_eg,0},
	{"type_t_NB",Ptype_t_NB,2},
	{"gf2nton",Pgf2nton,1},
	{"ntogf2n",Pntogf2n,1},
	{"set_upkara",Pset_upkara,-1},
	{"set_uptkara",Pset_uptkara,-1},
	{"set_up2kara",Pset_up2kara,-1},
	{"set_upfft",Pset_upfft,-1},
	{"inv",Pinv,2},
	{"inttorat",Pinttorat,3},
	{"fac",Pfac,1},
	{"prime",Pprime,1},
	{"lprime",Plprime,1},
	{"random",Prandom,-1},
	{"lrandom",Plrandom,1},
	{"iand",Piand,2},
	{"ior",Pior,2},
	{"ixor",Pixor,2},
	{"ishift",Pishift,2},
	{"small_jacobi",Psmall_jacobi,2},
#ifdef HMEXT
	{"igcdbin",Pigcdbin,2},		/* HM@CCUT extension */
	{"igcdbmod",Pigcdbmod,2},	/* HM@CCUT extension */
	{"igcdeuc",PigcdEuc,2},		/* HM@CCUT extension */
	{"igcdacc",Pigcdacc,2},		/* HM@CCUT extension */
	{"igcdcntl",Pigcdcntl,-1},	/* HM@CCUT extension */
	{"ihex",Pihex,1},	/* HM@CCUT extension */
	{"imaxrsh",Pimaxrsh,1},	/* HM@CCUT extension */
	{"ilen",Pilen,1},	/* HM@CCUT extension */
#endif /* HMEXT */
	{"mt_save",Pmt_save,1},
	{"mt_load",Pmt_load,1},
	{"ntoint32",Pntoint32,1},
	{"int32ton",Pint32ton,1},
	{0,0,0},
};

static int is_prime_small(unsigned int);
static unsigned int gcd_small(unsigned int,unsigned int);
int TypeT_NB_check(unsigned int, unsigned int);
int mpi_mag;

void Pntoint32(arg,rp)
NODE arg;
USINT *rp;
{
	Q q;
	unsigned int t;

	asir_assert(ARG0(arg),O_N,"ntoint32");
	q = (Q)ARG0(arg);
	if ( !q ) {
		MKUSINT(*rp,0);
		return;
	}
	if ( NID(q)!=N_Q || !INT(q) || PL(NM(q))>1 )
		error("ntoint32 : invalid argument");
	t = BD(NM(q))[0];
	if ( SGN(q) < 0 )
		t = -t;
	MKUSINT(*rp,t);
}

void Pint32ton(arg,rp)
NODE arg;
Q *rp;
{
	int t;

	asir_assert(ARG0(arg),O_USINT,"int32ton");
	t = (int)BDY((USINT)ARG0(arg));
	STOQ(t,*rp);
}

void Pdp_set_mpi(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"dp_set_mpi");
		mpi_mag = QTOS((Q)ARG0(arg));
	}
	STOQ(mpi_mag,*rp);
}

void Psmall_jacobi(arg,rp)
NODE arg;
Q *rp;
{
	Q a,m;
	int a0,m0,s;

	a = (Q)ARG0(arg);
	m = (Q)ARG1(arg);
	asir_assert(a,O_N,"small_jacobi");
	asir_assert(m,O_N,"small_jacobi");
	if ( !a )
		 *rp = ONE;
	else if ( !m || !INT(m) || !INT(a) 
		|| PL(NM(m))>1 || PL(NM(a))>1 || SGN(m) < 0 || EVENN(NM(m)) )
		error("small_jacobi : invalid input");
	else {
		a0 = QTOS(a); m0 = QTOS(m);
		s = small_jacobi(a0,m0);
		STOQ(s,*rp);
	}
}

int small_jacobi(a,m)
int a,m;
{
	int m4,m8,a4,j1,i,s;

	a %= m;
	if ( a == 0 || a == 1 )
		return 1;
	else if ( a < 0 ) {
		j1 = small_jacobi(-a,m);
		m4 = m%4;
		return m4==1?j1:-j1;
	} else { 
		for ( i = 0; a && !(a&1); i++, a >>= 1 );
		if ( i&1 ) {
			m8 = m%8;
			s = (m8==1||m8==7) ? 1 : -1;
		} else
			s = 1;
		/* a, m are odd */
		j1 = small_jacobi(m%a,a);
		m4 = m%4; a4 = a%4;
		s *= (m4==1||a4==1) ? 1 : -1; 
		return j1*s;
	}  
}

void Ptype_t_NB(arg,rp)
NODE arg;
Q *rp;
{
	if ( TypeT_NB_check(QTOS((Q)ARG0(arg)),QTOS((Q)ARG1(arg))))
		*rp = ONE;
	else
		*rp = 0;
}

int TypeT_NB_check(unsigned int m, unsigned int t)
{
	unsigned int p,k,u,h,d;

	if ( !(m%8) )
		return 0;
	p = t*m+1;
	if ( !is_prime_small(p) )
		return 0;
	for ( k = 1, u = 2%p; ; k++ )
		if ( u == 1 )
			break;
		else
			u = (2*u)%p;
	h = t*m/k;
	d = gcd_small(h,m);
	return d == 1 ? 1 : 0;
}

/*
 * a simple prime checker
 * return value: 1  ---  prime number
 *               0  ---  composite number
 */

static int is_prime_small(unsigned int a)
{
	unsigned int b,t,i;

	if ( !(a%2) ) return 0;
	for ( t = a, i = 0; t; i++, t >>= 1 );
	/* b >= sqrt(a) */
	b = 1<<((i+1)/2);

	/* divisibility test by all odd numbers <= b */
	for ( i = 3; i <= b; i += 2 )
		if ( !(a%i) )
			return 0;
	return 1;
}

/*
 * gcd for unsigned int as integers
 * return value: GCD(a,b)
 *
 */


static unsigned int gcd_small(unsigned int a,unsigned int b)
{
	unsigned int t;

	if ( b > a ) {
		t = a; a = b; b = t;
	}
	/* Euclid's algorithm */
	while ( 1 )
		if ( !(t = a%b) ) return b;
		else {
			a = b; b = t;
		}
}

void Pmt_save(arg,rp)
NODE arg;
Q *rp;
{
	int ret;

	ret = mt_save(BDY((STRING)ARG0(arg)));
	STOQ(ret,*rp);
}

void Pmt_load(arg,rp)
NODE arg;
Q *rp;
{
	int ret;

	ret = mt_load(BDY((STRING)ARG0(arg)));
	STOQ(ret,*rp);
}

void Pisqrt(arg,rp)
NODE arg;
Q *rp;
{
	Q a;
	N r;

	a = (Q)ARG0(arg);
	asir_assert(a,O_N,"isqrt");
	if ( !a )
		*rp = 0;
	else if ( SGN(a) < 0 )
		error("isqrt : negative argument");
	else {
		isqrt(NM(a),&r);
		NTOQ(r,1,*rp);
	}
}

void Pidiv(arg,rp)
NODE arg;
Obj *rp;
{
	N q,r;
	Q a;
	Q dnd,dvr;
	
	dnd = (Q)ARG0(arg); dvr = (Q)ARG1(arg);
	asir_assert(dnd,O_N,"idiv");
	asir_assert(dvr,O_N,"idiv");
	if ( !dvr )
		error("idiv: division by 0");
	else if ( !dnd )
		*rp = 0;
	else {
		divn(NM(dnd),NM(dvr),&q,&r);
		NTOQ(q,SGN(dnd)*SGN(dvr),a); *rp = (Obj)a;
	}
}

void Pirem(arg,rp)
NODE arg;
Obj *rp;
{
	N q,r;
	Q a;
	Q dnd,dvr;
	
	dnd = (Q)ARG0(arg); dvr = (Q)ARG1(arg);
	asir_assert(dnd,O_N,"irem");
	asir_assert(dvr,O_N,"irem");
	if ( !dvr )
		error("irem: division by 0");
	else if ( !dnd )
		*rp = 0;
	else {
		divn(NM(dnd),NM(dvr),&q,&r);
		NTOQ(r,SGN(dnd),a); *rp = (Obj)a;
	}
}

void Piqr(arg,rp)
NODE arg;
LIST *rp;
{
	N q,r;
	Q a,b;
	Q dnd,dvr;
	NODE node1,node2;

	dnd = (Q)ARG0(arg); dvr = (Q)ARG1(arg);
	if ( !dvr )
		error("iqr: division by 0");
	else if ( !dnd )
		a = b = 0;
	else if ( OID(dnd) == O_VECT ) {
		iqrv((VECT)dnd,dvr,rp); return;
	} else {
		asir_assert(dnd,O_N,"iqr");
		asir_assert(dvr,O_N,"iqr");
		divn(NM(dnd),NM(dvr),&q,&r);
		NTOQ(q,SGN(dnd)*SGN(dvr),a);
		NTOQ(r,SGN(dnd),b);
	}
	MKNODE(node2,b,0); MKNODE(node1,a,node2); MKLIST(*rp,node1);
}

void Pinttorat(arg,rp)
NODE arg;
LIST *rp;
{
	Q cq,qq,t,u1,v1,r1,nm;
	N m,b,q,r,c,u2,v2,r2;
	NODE node1,node2;
	P p;

	asir_assert(ARG0(arg),O_N,"inttorat");
	asir_assert(ARG1(arg),O_N,"inttorat");
	asir_assert(ARG2(arg),O_N,"inttorat");
	cq = (Q)ARG0(arg); m = NM((Q)ARG1(arg)); b = NM((Q)ARG2(arg));
	if ( !cq ) {
		MKNODE(node2,(pointer)ONE,0); MKNODE(node1,0,node2); MKLIST(*rp,node1);
		return;
	}
	divn(NM(cq),m,&q,&r);
	if ( !r ) {
		MKNODE(node2,(pointer)ONE,0); MKNODE(node1,0,node2); MKLIST(*rp,node1);
		return;
	} else if ( SGN(cq) < 0 ) {
		subn(m,r,&c);
	} else
		c = r;
	u1 = 0; v1 = ONE; u2 = m; v2 = c;
	while ( cmpn(v2,b) >= 0 ) {
		divn(u2,v2,&q,&r2); u2 = v2; v2 = r2;
		NTOQ(q,1,qq); mulq(qq,v1,&t); subq(u1,t,&r1); u1 = v1; v1 = r1; 
	}
	if ( cmpn(NM(v1),b) >= 0 )
		*rp = 0;
	else {
		if ( SGN(v1) < 0 ) {
			chsgnp((P)v1,&p); v1 = (Q)p; NTOQ(v2,-1,nm);
		} else
			NTOQ(v2,1,nm);
		MKNODE(node2,v1,0); MKNODE(node1,nm,node2); MKLIST(*rp,node1);
	}
}

void Pigcd(arg,rp)
NODE arg;
Q *rp;
{
	N g;
	Q n1,n2,a;
	
	if ( argc(arg) == 1 ) {
		igcdv((VECT)ARG0(arg),rp);
		return;
	}
	n1 = (Q)ARG0(arg); n2 = (Q)ARG1(arg);
	asir_assert(n1,O_N,"igcd");
	asir_assert(n2,O_N,"igcd");
	if ( !n1 )
		*rp = n2;
	else if ( !n2 )
		*rp = n1;
	else {
		gcdn(NM(n1),NM(n2),&g);
		NTOQ(g,1,a); *rp = a;
	}
}

int comp_n(a,b)
N *a,*b;
{
	return cmpn(*a,*b);
}

void iqrv(a,dvr,rp)
VECT a;
Q dvr;
LIST *rp;
{
	int i,n;
	VECT q,r;
	Q dnd,qi,ri;
	Q *b;
	N qn,rn;
	NODE n0,n1;

	if ( !dvr )
		error("iqrv: division by 0");
	n = a->len; b = (Q *)BDY(a);
	MKVECT(q,n); MKVECT(r,n);
	for ( i = 0; i < n; i++ ) {
		dnd = b[i];
		if ( !dnd ) {
			qi = ri = 0;
		} else {
			divn(NM(dnd),NM(dvr),&qn,&rn);
			NTOQ(qn,SGN(dnd)*SGN(dvr),qi);
			NTOQ(rn,SGN(dnd),ri);
		}
		BDY(q)[i] = (pointer)qi; BDY(r)[i] = (pointer)ri;
	}
	MKNODE(n1,r,0); MKNODE(n0,q,n1); MKLIST(*rp,n0);
}

void igcdv(a,rp)
VECT a;
Q *rp;
{
	int i,j,n,nz;
	N g,gt,q,r;
	N *c;

	n = a->len;
	c = (N *)ALLOCA(n*sizeof(N));
	for ( i = 0; i < n; i++ )
		c[i] = BDY(a)[i]?NM((Q)(BDY(a)[i])):0;
	qsort(c,n,sizeof(N),(int (*) (const void *,const void *))comp_n);
	for ( ; n && ! *c; n--, c++ );

	if ( !n ) {
		*rp = 0; return;
	} else if ( n == 1 ) {
		NTOQ(*c,1,*rp); return;
	}
	gcdn(c[0],c[1],&g);
	for ( i = 2; i < n; i++ ) {
		divn(c[i],g,&q,&r);
		gcdn(g,r,&gt);
		if ( !cmpn(g,gt) ) {
			for ( j = i+1, nz = 0; j < n; j++ ) {
				divn(c[j],g,&q,&r); c[j] = r;
				if ( r )
					nz = 1;
			}
		} else
			g = gt;
	}
	NTOQ(g,1,*rp);
}

void igcdv_estimate(a,rp)
VECT a;
Q *rp;
{
	int n,i,m;
	N s,t,u,g;
	Q *q;

	n = a->len; q = (Q *)a->body;
	if ( n == 1 ) {
		NTOQ(NM(q[0]),1,*rp); return;		
	}

	m = n/2;
	for ( i = 0 , s = 0; i < m; i++ ) {
		addn(s,NM(q[i]),&u); s = u;
	}
	for ( t = 0; i < n; i++ ) {
		addn(t,NM(q[i]),&u); t = u;
	}
	gcdn(s,t,&g); NTOQ(g,1,*rp);
}

void Pilcm(arg,rp)
NODE arg;
Obj *rp;
{
	N g,q,r,l;
	Q n1,n2,a;
	
	n1 = (Q)ARG0(arg); n2 = (Q)ARG1(arg);
	asir_assert(n1,O_N,"ilcm");
	asir_assert(n2,O_N,"ilcm");
	if ( !n1 || !n2 )
		*rp = 0;
	else {
		gcdn(NM(n1),NM(n2),&g); divn(NM(n1),g,&q,&r);
		muln(q,NM(n2),&l); NTOQ(l,1,a); *rp = (Obj)a;
	}
}

void Piand(arg,rp)
NODE arg;
Q *rp;
{
	N g;
	Q n1,n2,a;
	
	n1 = (Q)ARG0(arg); n2 = (Q)ARG1(arg);
	asir_assert(n1,O_N,"iand");
	asir_assert(n2,O_N,"iand");
	if ( !n1 || !n2 )
		*rp = 0;
	else {
		iand(NM(n1),NM(n2),&g);
		NTOQ(g,1,a); *rp = a;
	}
}

void Pior(arg,rp)
NODE arg;
Q *rp;
{
	N g;
	Q n1,n2,a;
	
	n1 = (Q)ARG0(arg); n2 = (Q)ARG1(arg);
	asir_assert(n1,O_N,"ior");
	asir_assert(n2,O_N,"ior");
	if ( !n1 )
		*rp = n2;
	else if ( !n2 )
		*rp = n1;
	else {
		ior(NM(n1),NM(n2),&g);
		NTOQ(g,1,a); *rp = a;
	}
}

void Pixor(arg,rp)
NODE arg;
Q *rp;
{
	N g;
	Q n1,n2,a;
	
	n1 = (Q)ARG0(arg); n2 = (Q)ARG1(arg);
	asir_assert(n1,O_N,"ixor");
	asir_assert(n2,O_N,"ixor");
	if ( !n1 )
		*rp = n2;
	else if ( !n2 )
		*rp = n1;
	else {
		ixor(NM(n1),NM(n2),&g);
		NTOQ(g,1,a); *rp = a;
	}
}

void Pishift(arg,rp)
NODE arg;
Q *rp;
{
	N g;
	Q n1,s,a;
	
	n1 = (Q)ARG0(arg); s = (Q)ARG1(arg);
	asir_assert(n1,O_N,"ixor");
	asir_assert(s,O_N,"ixor");
	if ( !n1 )
		*rp = 0;
	else if ( !s )
		*rp = n1;
	else {
		bshiftn(NM(n1),QTOS(s),&g);
		NTOQ(g,1,a); *rp = a;
	}
}

void isqrt(a,r)
N a,*r;
{
	int k;
	N x,t,x2,xh,quo,rem;

	if ( !a )
		*r = 0;
	else if ( UNIN(a) )
		*r = ONEN;
	else {
		k = n_bits(a); /* a <= 2^k-1 */
		bshiftn(ONEN,-((k>>1)+(k&1)),&x); /* a <= x^2 */
		while ( 1 ) {
			pwrn(x,2,&t);
			if ( cmpn(t,a) <= 0 ) {
				*r = x; return;
			} else {
				if ( BD(x)[0] & 1 )
					addn(x,a,&t);
				else
					t = a;
				bshiftn(x,-1,&x2); divn(t,x2,&quo,&rem);
				bshiftn(x,1,&xh); addn(quo,xh,&x);
			}
		}
	}
}

void iand(n1,n2,r)
N n1,n2,*r;
{
	int d1,d2,d,i;
	N nr;
	int *p1,*p2,*pr;

	d1 = PL(n1); d2 = PL(n2);
	d = MIN(d1,d2); 
	nr = NALLOC(d);
	for ( i = 0, p1 = BD(n1), p2 = BD(n2), pr = BD(nr); i < d; i++ )
		pr[i] = p1[i] & p2[i];
	for ( i = d-1; i >= 0 && !pr[i]; i-- );
	if ( i < 0 )
		*r = 0;
	else {
		PL(nr) = i+1; *r = nr;
	}
}

void ior(n1,n2,r)
N n1,n2,*r;
{
	int d1,d2,i;
	N nr;
	int *p1,*p2,*pr;

	if ( PL(n1) < PL(n2) ) {
		nr = n1; n1 = n2; n2 = nr;
	}
	d1 = PL(n1); d2 = PL(n2);
	*r = nr = NALLOC(d1);
	for ( i = 0, p1 = BD(n1), p2 = BD(n2), pr = BD(nr); i < d2; i++ )
		pr[i] = p1[i] | p2[i];
	for ( ; i < d1; i++ )
		pr[i] = p1[i];
	for ( i = d1-1; i >= 0 && !pr[i]; i-- );
	if ( i < 0 )
		*r = 0;
	else {
		PL(nr) = i+1; *r = nr;
	}
}

void ixor(n1,n2,r)
N n1,n2,*r;
{
	int d1,d2,i;
	N nr;
	int *p1,*p2,*pr;

	if ( PL(n1) < PL(n2) ) {
		nr = n1; n1 = n2; n2 = nr;
	}
	d1 = PL(n1); d2 = PL(n2);
	*r = nr = NALLOC(d1);
	for ( i = 0, p1 = BD(n1), p2 = BD(n2), pr = BD(nr); i < d2; i++ )
		pr[i] = p1[i] ^ p2[i];
	for ( ; i < d1; i++ )
		pr[i] = p1[i];
	for ( i = d1-1; i >= 0 && !pr[i]; i-- );
	if ( i < 0 )
		*r = 0;
	else {
		PL(nr) = i+1; *r = nr;
	}
}

void Pup2_init_eg(rp)
Obj *rp;
{
	up2_init_eg();
	*rp = 0;
}

void Pup2_show_eg(rp)
Obj *rp;
{
	up2_show_eg();
	*rp = 0;
}

void Pgf2nton(arg,rp)
NODE arg;
Q *rp;
{
	if ( !ARG0(arg) )
		*rp = 0;
	else
		up2ton(((GF2N)ARG0(arg))->body,rp);
}

void Pntogf2n(arg,rp)
NODE arg;
GF2N *rp;
{
	UP2 t;

	ntoup2(ARG0(arg),&t);
	MKGF2N(t,*rp);
}

void Pup2_inv(arg,rp)
NODE arg;
P *rp;
{
	UP2 a,b,t;

	ptoup2(ARG0(arg),&a);
	ptoup2(ARG1(arg),&b);
	invup2(a,b,&t);
	up2top(t,rp);
}

void Pinv(arg,rp)
NODE arg;
Num *rp;
{
	Num n;
	Q mod;
	MQ r;	
	int inv;

	n = (Num)ARG0(arg); mod = (Q)ARG1(arg);
	asir_assert(n,O_N,"inv");
	asir_assert(mod,O_N,"inv");
	if ( !n || !mod )
		error("inv: invalid input");
	else
		switch ( NID(n) ) {
			case N_Q:
				invl((Q)n,mod,(Q *)rp);
				break;
			case N_M:
				inv = invm(CONT((MQ)n),QTOS(mod));
				STOMQ(inv,r);
				*rp = (Num)r;
				break;
			default:
				error("inv: invalid input");
		}
}

void Pfac(arg,rp)
NODE arg;
Q *rp;
{ 
	asir_assert(ARG0(arg),O_N,"fac");
	factorial(QTOS((Q)ARG0(arg)),rp); 
}

void Plrandom(arg,rp)
NODE arg;
Q *rp;
{
	N r;

	asir_assert(ARG0(arg),O_N,"lrandom");
	randomn(QTOS((Q)ARG0(arg)),&r);
	NTOQ(r,1,*rp);
}

void Prandom(arg,rp)
NODE arg;
Q *rp;
{
	unsigned int r;

#if 0
#if defined(_PA_RISC1_1)
	r = mrand48()&BMASK;
#else
	if ( arg )
		srandom(QTOS((Q)ARG0(arg)));
	r = random()&BMASK;
#endif
#endif
	if ( arg )
		mt_sgenrand(QTOS((Q)ARG0(arg)));
	r = mt_genrand();
	UTOQ(r,*rp);
}

#if defined(VISUAL)
void srandom(unsigned int);

static unsigned int R_Next;

unsigned int random() {
        if ( !R_Next )
                R_Next = 1;
        return R_Next = (R_Next * 1103515245 + 12345);
}

void srandom(s)
unsigned int s;
{
		if ( s )
			R_Next = s;
        else if ( !R_Next )
            R_Next = 1;
}
#endif

void Pprime(arg,rp)
NODE arg;
Q *rp;
{
	int i;

	asir_assert(ARG0(arg),O_N,"prime");
	i = QTOS((Q)ARG0(arg));
	if ( i < 0 || i >= 1900 )
		*rp = 0;
	else
		STOQ(sprime[i],*rp);
}

void Plprime(arg,rp)
NODE arg;
Q *rp;
{
	int i;

	asir_assert(ARG0(arg),O_N,"lprime");
	i = QTOS((Q)ARG0(arg));
	if ( i < 0 || i >= 1000 )
		*rp = 0;
	else
		STOQ(lprime[i],*rp);
}

extern int up_kara_mag, up_tkara_mag, up_fft_mag;

void Pset_upfft(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"set_upfft");
		up_fft_mag = QTOS((Q)ARG0(arg));
	}
	STOQ(up_fft_mag,*rp);
}

void Pset_upkara(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"set_upkara");
		up_kara_mag = QTOS((Q)ARG0(arg));
	}
	STOQ(up_kara_mag,*rp);
}

void Pset_uptkara(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"set_uptkara");
		up_tkara_mag = QTOS((Q)ARG0(arg));
	}
	STOQ(up_tkara_mag,*rp);
}

extern int up2_kara_mag;

void Pset_up2kara(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"set_up2kara");
		up2_kara_mag = QTOS((Q)ARG0(arg));
	}
	STOQ(up2_kara_mag,*rp);
}

#ifdef HMEXT
void Pigcdbin(arg,rp)
NODE arg;
Obj *rp;
{
	N g;
	Q n1,n2,a;
	
	n1 = (Q)ARG0(arg); n2 = (Q)ARG1(arg);
	asir_assert(n1,O_N,"igcd");
	asir_assert(n2,O_N,"igcd");
	if ( !n1 )
		*rp = (Obj)n2;
	else if ( !n2 )
		*rp = (Obj)n1;
	else {
		gcdbinn(NM(n1),NM(n2),&g);
		NTOQ(g,1,a); *rp = (Obj)a;
	}
}

void Pigcdbmod(arg,rp)
NODE arg;
Obj *rp;
{
	N g;
	Q n1,n2,a;
	
	n1 = (Q)ARG0(arg); n2 = (Q)ARG1(arg);
	asir_assert(n1,O_N,"igcdbmod");
	asir_assert(n2,O_N,"igcdbmod");
	if ( !n1 )
		*rp = (Obj)n2;
	else if ( !n2 )
		*rp = (Obj)n1;
	else {
		gcdbmodn(NM(n1),NM(n2),&g);
		NTOQ(g,1,a); *rp = (Obj)a;
	}
}

void Pigcdacc(arg,rp)
NODE arg;
Obj *rp;
{
	N g;
	Q n1,n2,a;
	
	n1 = (Q)ARG0(arg); n2 = (Q)ARG1(arg);
	asir_assert(n1,O_N,"igcdacc");
	asir_assert(n2,O_N,"igcdacc");
	if ( !n1 )
		*rp = (Obj)n2;
	else if ( !n2 )
		*rp = (Obj)n1;
	else {
		gcdaccn(NM(n1),NM(n2),&g);
		NTOQ(g,1,a); *rp = (Obj)a;
	}
}

void PigcdEuc(arg,rp)
NODE arg;
Obj *rp;
{
	N g;
	Q n1,n2,a;
	
	n1 = (Q)ARG0(arg); n2 = (Q)ARG1(arg);
	asir_assert(n1,O_N,"igcdbmod");
	asir_assert(n2,O_N,"igcdbmod");
	if ( !n1 )
		*rp = (Obj)n2;
	else if ( !n2 )
		*rp = (Obj)n1;
	else {
		gcdEuclidn(NM(n1),NM(n2),&g);
		NTOQ(g,1,a); *rp = (Obj)a;
	}
}

extern int igcd_algorithm;
    /*	== 0 : Euclid,
     *	== 1 : binary,
     *	== 2 : bmod,
     *	>= 3 : (Weber's accelerated)/(Jebelean's generalized binary) algorithm,
     */
extern int igcd_thre_inidiv;
    /*
     *  In the non-Euclidean algorithms, if the ratio of the lengths (number
     *  of words) of two integers is >= igcd_thre_inidiv, we first perform
     *  remainder calculation.
     *  If == 0, this remainder calculation is not performed.
     */
extern int igcdacc_thre;
    /*
     *  In the accelerated algorithm, if the bit-lengths of two integers is
     *  > igcdacc_thre, "bmod" reduction is done.
     */

void Pigcdcntl(arg,rp)
NODE arg;
Obj *rp;
{
	Obj p;
	Q a;
	int k, m;

	if ( arg ) {
		p = (Obj)ARG0(arg);
		if ( !p ) {
			igcd_algorithm = 0;
			*rp = p;
			return;
		} else if ( OID(p) == O_N ) {
			k = QTOS((Q)p);
			a = (Q)p;
			if ( k >= 0 ) igcd_algorithm = k;
			else if ( k == -1 ) {
			ret_thre:
				k = - igcd_thre_inidiv - igcdacc_thre*10000;
				STOQ(k,a);
				*rp = (Obj)a;
				return;
			} else {
				if ( (m = (-k)%10000) != 0 ) igcd_thre_inidiv = m;
				if ( (m = -k/10000) != 0 ) igcdacc_thre = m;
				goto ret_thre;
			}
		} else if ( OID(p) == O_STR ) {
			char *n = BDY((STRING) p);

			if ( !strcmp( n, "binary" ) || !strcmp( n, "Binary" )
			     || !strcmp( n, "bin" ) || !strcmp( n, "Bin" ) )
				k = igcd_algorithm = 1;
			else if ( !strcmp( n, "bmod" ) || !strcmp( n, "Bmod" ) )
				igcd_algorithm = 2;
			else if ( !strcmp( n, "euc" ) || !strcmp( n, "Euc" )
				  || !strcmp( n, "euclid" ) || !strcmp( n, "Euclid" ) )
				igcd_algorithm = 0;
			else if ( !strcmp( n, "acc" ) || !strcmp( n, "Acc" )
			     || !strcmp( n, "gen" ) || !strcmp( n, "Gen" )
			     || !strcmp( n, "genbin" ) || !strcmp( n, "GenBin" ) )
				igcd_algorithm = 3;
			else error( "igcdhow : invalid algorithm specification" );
		}
	}
	STOQ(igcd_algorithm,a);
	*rp = (Obj)a;
	return;
}


void Pimaxrsh(arg,rp)
NODE arg;
LIST *rp;
{
	N n1, n2;
	Q q;
	NODE node1, node2;
	int bits;
	N maxrshn();
	
	q = (Q)ARG0(arg);
	asir_assert(q,O_N,"imaxrsh");
	if ( !q ) n1 = n2 = 0;
	else {
		n1 = maxrshn( NM(q), &bits );
		STON( bits, n2 );
	}
	NTOQ( n2, 1, q );
	MKNODE( node2, q, 0 );
	NTOQ( n1, 1, q );
	MKNODE( node1, q, node2 );
	MKLIST( *rp, node1 );
}
void Pilen(arg,rp)
NODE arg;
Obj *rp;
{
	Q q;
	int l;
	
	q = (Q)ARG0(arg);
	asir_assert(q,O_N,"ilenh");
	if ( !q ) l = 0;
	else l = PL(NM(q));
	STOQ(l,q);
	*rp = (Obj)q;
}

void Pihex(arg,rp)
NODE arg;
Obj *rp;
{
	Q n1;
	
	n1 = (Q)ARG0(arg);
	asir_assert(n1,O_N,"ihex");
	if ( n1 ) {
		int i, l = PL(NM(n1)), *p = BD(NM(n1));

		for ( i = 0; i < l; i++ ) printf( " 0x%08x", p[i] );
		printf( "\n" );
	} else printf( " 0x%08x\n", 0 );
	*rp = (Obj)n1;
}
#endif /* HMEXT */
