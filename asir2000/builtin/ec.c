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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/ec.c,v 1.3 2000/08/21 08:31:19 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "inline.h"

/*
 * Elliptic curve related functions
 *
 * Argument specifications
 * Point = vector(x,y,z)  (projective representation)
 * ECInfo = vector(a,b,p)
 *         a,b : integer for GF(p), GF2N for GF(2^n)
 *         p : integer for GF(p), polynomial for GF(2^n)
 *
 */

struct oKeyIndexPair {
	unsigned int key;
	int index;
};

void Psha1();
void Psha1_ec();
void Pecm_add_ff();
void Pecm_chsgn_ff();
void Pecm_sub_ff();
void Pecm_compute_all_key_homo_ff();
void Pnextvect1(),Psort_ktarray(),Pecm_find_match(),Pseparate_vect();
void Pecm_set_addcounter();
void Pecm_count_order();

void ecm_add_ff(VECT,VECT,VECT,VECT *);
void ecm_add_gfp(VECT,VECT,VECT,VECT *);
void ecm_add_gf2n(VECT,VECT,VECT,VECT *);

void ecm_chsgn_ff(VECT,VECT *);
void ecm_sub_ff(VECT,VECT,VECT,VECT *);

void compute_all_key_homo_gfp(VECT *,int,unsigned int *);
void compute_all_key_homo_gf2n(VECT *,int,unsigned int *);

unsigned int separate_vect(double *,int);
void ecm_find_match(unsigned int *,int,unsigned int *,int,LIST *);
int find_match(unsigned int,unsigned int *,int);

void sort_ktarray(VECT,VECT,LIST *);
int comp_kip(struct oKeyIndexPair *,struct oKeyIndexPair *);

int nextvect1(VECT,VECT);

unsigned int ecm_count_order_gfp(unsigned int,unsigned int,unsigned int);
unsigned int ecm_count_order_gf2n(UP2,GF2N,GF2N);

void sha_memory(unsigned char *,unsigned int,unsigned int *);

unsigned int ecm_addcounter;
extern int current_ff,lm_lazy;

struct ftab ec_tab[] = {
/* point addition */
	{"ecm_add_ff",Pecm_add_ff,3},

/* point change sign */
	{"ecm_chsgn_ff",Pecm_chsgn_ff,1},

/* point subtraction */
	{"ecm_sub_ff",Pecm_sub_ff,3},

/* key computation for sort and match */
	{"ecm_compute_all_key_homo_ff",Pecm_compute_all_key_homo_ff,1},

/* exhausitve search of rational points */
	{"ecm_count_order",Pecm_count_order,1},

/* common functions */
	{"nextvect1",Pnextvect1,2},
	{"sort_ktarray",Psort_ktarray,2},
	{"separate_vect",Pseparate_vect,1},
	{"ecm_find_match",Pecm_find_match,2},
	{"ecm_set_addcounter",Pecm_set_addcounter,-1},
	{"sha1",Psha1,-2},
#if 0
	{"sha1_free",Psha1_free,1},
#endif
	{0,0,0},
};

void Psha1(arg,rp)
NODE arg;
Q *rp;
{
	unsigned char *s;
	unsigned int digest[5];
	int i,j,l,bl,n;
	unsigned int t;
	N z,r;

	asir_assert(ARG0(arg),O_N,"sha1_free");	
	z = NM((Q)ARG0(arg));
	n = PL(z);
	l = n_bits(z);
	if ( argc(arg) == 2 )
		bl = QTOS((Q)ARG1(arg));
	else
		bl = (l+7)/8;
	s = (unsigned char *)MALLOC(bl);
	for ( i = 0, j = bl-1; i < n; i++ ) {
		t = BD(z)[i];
		if ( j >= 0 ) s[j--] = t&0xff; t>>=8;
		if ( j >= 0 ) s[j--] = t&0xff; t>>=8;
		if ( j >= 0 ) s[j--] = t&0xff; t>>=8;
		if ( j >= 0 ) s[j--] = t;
	}
	sha_memory(s,bl,digest);
	r = NALLOC(5);
	for ( i = 0; i < 5; i++ )
		BD(r)[i] = digest[4-i];
	for ( i = 4; i >= 0 && !BD(r)[i]; i-- );
	if ( i < 0 )
		*rp = 0;
	else {
		PL(r) = i+1;
		NTOQ(r,1,*rp);
	}
}

#if 0
void Psha1_ec(arg,rp)
NODE arg;
Q *rp;
{
#include <fj_crypt.h>
	SHS_CTX context;
	unsigned char *s;
	int i,j,l,bl,n;
	unsigned int t;
	N z,r;
	extern int little_endian;

	asir_assert(ARG0(arg),O_N,"sha1");	
	z = NM((Q)ARG0(arg));
	n = PL(z);
	l = n_bits(z);
	bl = (l+7)/8;
	s = (unsigned char *)MALLOC(bl);
	for ( i = 0, j = bl-1; i < n; i++ ) {
		t = BD(z)[i];
		if ( j >= 0 ) s[j--] = t&0xff; t>>=8;
		if ( j >= 0 ) s[j--] = t&0xff; t>>=8;
		if ( j >= 0 ) s[j--] = t&0xff; t>>=8;
		if ( j >= 0 ) s[j--] = t;
	}
	SHSInit(&context);
	SHSUpdate(&context,s,bl);
	SHSFinal(&context);
	r = NALLOC(5);
	if ( little_endian )
		for ( i = 0; i < 5; i++ ) {
			t = context.digest[4-i];
			BD(r)[i] = (t>>24)|((t&0xff0000)>>8)|((t&0xff00)<<8)|(t<<24);
	} else
		for ( i = 0; i < 5; i++ )
			BD(r)[i] = context.digest[4-i];
	for ( i = 4; i >= 0 && !BD(r)[i]; i-- );
	if ( i < 0 )
		*rp = 0;
	else {
		PL(r) = i+1;
		NTOQ(r,1,*rp);
	}
}
#endif

void Pecm_count_order(arg,rp)
NODE arg;
Q *rp;
{
	N p;
	UP2 d;
	Obj a,b;
	unsigned int p0,a0,b0,ord;
	VECT ec;
	Obj *vb;

	switch ( current_ff ) {
		case FF_GFP:
			getmod_lm(&p);
			if ( n_bits(p) > 32 )
				error("ecm_count_order : ground field too large");
			p0 = BD(p)[0];
			ec = (VECT)ARG0(arg);
			vb = (Obj *)BDY(ec); simp_ff(vb[0],&a); simp_ff(vb[1],&b);
			a0 = a?BD(BDY((LM)a))[0]:0;
			b0 = b?BD(BDY((LM)b))[0]:0;
			ord = ecm_count_order_gfp(p0,a0,b0);
			UTOQ(ord,*rp);
			break;
		case FF_GF2N:
			getmod_gf2n(&d);
			if ( degup2(d) > 10 )
				error("ecm_count_order : ground field too large");
			ec = (VECT)ARG0(arg);
			vb = (Obj *)BDY(ec); simp_ff(vb[0],&a); simp_ff(vb[1],&b);
			ord = ecm_count_order_gf2n(d,(GF2N)a,(GF2N)b);
			UTOQ(ord,*rp);
			break;
		default:
			error("ecm_count_order : current_ff is not set");
	}
}

void Pecm_set_addcounter(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg )
		ecm_addcounter = QTOS((Q)ARG0(arg));
	UTOQ(ecm_addcounter,*rp);
}

void Pecm_compute_all_key_homo_ff(arg,rp)
NODE arg;
VECT *rp;
{
	Obj mod;
	unsigned int *ka;
	int len,i;
	VECT *pa;
	VECT r,v;
	LIST *vb;
	USINT *b;

	v = (VECT)ARG0(arg);
	len = v->len;
	vb = (LIST *)v->body;
	pa = (VECT *)ALLOCA(len*sizeof(VECT));
	ka = (unsigned int *)ALLOCA(len*sizeof(unsigned int));
	for ( i = 0; i < len; i++ )
		pa[i] = (VECT)BDY(NEXT(BDY(vb[i])));
	switch ( current_ff ) {
		case FF_GFP:
			compute_all_key_homo_gfp(pa,len,ka); break;
		case FF_GF2N:
			compute_all_key_homo_gf2n(pa,len,ka); break;
		default:
			error("ecm_compute_all_key_homo_ff : current_ff is not set");
	}
	MKVECT(r,len); *rp = r;
	b = (USINT *)r->body;
	for ( i = 0; i < len; i++ )
		MKUSINT(b[i],ka[i]);
}

void Psort_ktarray(arg,rp)
NODE arg;
LIST *rp;
{
	sort_ktarray((VECT)ARG0(arg),(VECT)ARG1(arg),rp);
}

void Pecm_add_ff(arg,rp)
NODE arg;
VECT *rp;
{
	ecm_add_ff(ARG0(arg),ARG1(arg),ARG2(arg),rp);
}

void Pecm_sub_ff(arg,rp)
NODE arg;
VECT *rp;
{
	ecm_sub_ff(ARG0(arg),ARG1(arg),ARG2(arg),rp);
}

void Pecm_chsgn_ff(arg,rp)
NODE arg;
VECT *rp;
{
	ecm_chsgn_ff(ARG0(arg),rp);
}

void Pnextvect1(arg,rp)
NODE arg;
Q *rp;
{
	int index;

	index = nextvect1(ARG0(arg),ARG1(arg));
	STOQ(index,*rp);
}

/* XXX at least n < 32 must hold. What is the strict restriction for n ? */

void Pseparate_vect(arg,rp)
NODE arg;
LIST *rp;
{
	VECT v;
	int n,i;
	Q *b;
	double *w;
	unsigned int s;
	NODE ns,nc,t,t1;
	Q iq;
	LIST ls,lc;

	v = (VECT)ARG0(arg);
	n = v->len; b = (Q *)v->body;
	w = (double *)ALLOCA(n*sizeof(double));
	for ( i = 0; i < n; i++ )
		w[i] = (double)QTOS(b[i]);
	s = separate_vect(w,n);
	ns = nc = 0;
	for ( i = n-1; i >= 0; i-- )
		if ( s & (1<<i) ) {
			STOQ(i,iq); MKNODE(t,iq,ns); ns = t;
		} else {
			STOQ(i,iq); MKNODE(t,iq,nc); nc = t;
		}
	MKLIST(ls,ns); MKLIST(lc,nc);
	MKNODE(t,lc,0); MKNODE(t1,ls,t);
	MKLIST(*rp,t1);
}

void Pecm_find_match(arg,rp)
NODE arg;
LIST *rp;
{
	VECT g,b;
	int ng,nb,i;
	USINT *p;
	unsigned int *kg,*kb;

	g = (VECT)ARG0(arg); ng = g->len;
	kg = (unsigned int *)ALLOCA(ng*sizeof(unsigned int));
	for ( i = 0, p = (USINT *)g->body; i < ng; i++ )
		kg[i] = p[i]?BDY(p[i]):0;
	b = (VECT)ARG1(arg); nb = b->len;
	kb = (unsigned int *)ALLOCA(nb*sizeof(unsigned int));
	for ( i = 0, p = (USINT *)b->body; i < nb; i++ )
		kb[i] = p[i]?BDY(p[i]):0;
	ecm_find_match(kg,ng,kb,nb,rp);
}

void ecm_add_ff(p1,p2,ec,pr)
VECT p1,p2,ec;
VECT *pr;
{
	if ( !p1 )
		*pr = p2;
	else if ( !p2 )
		*pr = p1;
	else {
		switch ( current_ff ) {
			case FF_GFP:
				ecm_add_gfp(p1,p2,ec,pr); break;
			case FF_GF2N:
				ecm_add_gf2n(p1,p2,ec,pr); break;
			default:
				error("ecm_add_ff : current_ff is not set");
		}
	}
}

/* ec = [AX,BC]  */

void ecm_add_gf2n(p1,p2,ec,rp)
VECT p1,p2,ec;
VECT *rp;
{
	GF2N ax,bc,a0,a1,a2,b0,b1,b2;
	GF2N a2b0,a0b2,a2b1,a1b2,a02,a04,a22,a24,a0a2,a0a22,a1a2;
	GF2N t,s,u,r0,r1,r00,r01,r02,r002,r003,r022,r02q;
	VECT r;
	GF2N *vb,*rb;

	ecm_addcounter++;
	/* addition with O	*/
	if ( !p1 ) {
		*rp = p2;
		return;
	}
	if ( !p2 ) {
		*rp = p1;
		return;
	}
	vb = (GF2N *)BDY(ec); ax = vb[0]; bc = vb[1];
	vb = (GF2N *)BDY(p1); a0 = vb[0]; a1 = vb[1]; a2 = vb[2];
	vb = (GF2N *)BDY(p2); b0 = vb[0]; b1 = vb[1]; b2 = vb[2];

	mulgf2n(a2,b0,&a2b0); mulgf2n(a0,b2,&a0b2);
	if ( !cmpgf2n(a2b0,a0b2) ) {
		mulgf2n(a2,b1,&a2b1);	
		mulgf2n(a1,b2,&a1b2);	
		if ( !cmpgf2n(a2b1,a1b2) ) {
			if ( !a0 )
				*rp = 0;
			else {
				squaregf2n(a0,&a02); squaregf2n(a02,&a04);
				squaregf2n(a2,&a22); squaregf2n(a22,&a24);
				mulgf2n(a0,a2,&a0a2); squaregf2n(a0a2,&a0a22);
				mulgf2n(bc,a24,&t); addgf2n(a04,t,&r0);
				mulgf2n(a04,a0a2,&t); mulgf2n(a1,a2,&a1a2);
				addgf2n(a02,a1a2,&s); addgf2n(s,a0a2,&u);
				mulgf2n(u,r0,&s); addgf2n(t,s,&r1);
	
				MKVECT(r,3); rb = (GF2N *)r->body;
				mulgf2n(r0,a0a2,&rb[0]); rb[1] = r1; mulgf2n(a0a22,a0a2,&rb[2]);
				*rp = r;
			}
		} else
			*rp = 0;
	} else {
		mulgf2n(a1,b2,&a1b2); addgf2n(a0b2,a2b0,&r00);
		mulgf2n(a2,b1,&t); addgf2n(a1b2,t,&r01); mulgf2n(a2,b2,&r02);
		squaregf2n(r00,&r002); mulgf2n(r002,r00,&r003);

		addgf2n(r00,r01,&t); mulgf2n(t,r01,&s); mulgf2n(s,r02,&t);
		if ( ax ) {
			mulgf2n(r02,ax,&r02q);
			addgf2n(t,r003,&s); mulgf2n(r02q,r002,&t); addgf2n(s,t,&r0);
		} else
			addgf2n(t,r003,&r0);

		mulgf2n(a0b2,r002,&t); addgf2n(t,r0,&s); mulgf2n(r01,s,&t);
		mulgf2n(r002,a1b2,&s); addgf2n(r0,s,&u); mulgf2n(r00,u,&s);
		addgf2n(t,s,&r1);
	
		MKVECT(r,3); rb = (GF2N *)r->body;
		mulgf2n(r0,r00,&rb[0]); rb[1] = r1; mulgf2n(r003,r02,&rb[2]);
		*rp = r;
	}
}

extern LM THREE_LM,FOUR_LM,EIGHT_LM;

/* 0 < p < 2^16, 0 <= a,b < p */

unsigned int ecm_count_order_gfp(p,a,b)
unsigned int p,a,b;
{
	unsigned int x,y,rhs,ord,t;

	for ( x = 0, ord = 1; x < p; x++ ) {
		DMAR(x,x,a,p,t) /* t = x^2+a mod p */
		DMAR(t,x,b,p,rhs) /* rhs = x*(x^2+a)+b mod p */
		if ( !rhs )	
			ord++;
		else if ( small_jacobi(rhs,p)==1 )
			ord+=2;
	}
	return ord;
}

unsigned int ecm_count_order_gf2n(d,a,b)
UP2 d;
GF2N a,b;
{
	error("ecm_count_order_gf2n : not implemented yet");
}

/* ec = [AX,BC]  */

void ecm_add_gfp(p1,p2,ec,pr)
VECT p1,p2,ec;
VECT *pr;
{
	LM aa,bb,x1,y1,z1,x2,y2,z2,x1z2,v1,y1z2,u1,u2,v2,v3,z1z2;
	LM v2x1z2,a1,x3,y3,z3,w1,s1,s2,s3,s1y1,b1,h1;
	LM t,s,u;
	LM *vb;
	VECT r;

	ecm_addcounter++;
	/* addition with O	*/
	if( !p1 ) {
		*pr = p2;
		return;
	}
	if( !p2 ) {
		*pr = p1;
		return;
	}

	/*  set parameters  		*/

/*	aa = ec[0]; bb = ec[1]; */
	vb = (LM *)BDY(ec); aa = vb[0]; bb = vb[1];

/*	x1 = p1[0]; y1 = p1[1]; z1 = p1[2]; */
	vb = (LM *)BDY(p1); x1 = vb[0]; y1 = vb[1]; z1 = vb[2];

/*	x2 = p2[0]; y2 = p2[1]; z2 = p2[2]; */
	vb = (LM *)BDY(p2); x2 = vb[0]; y2 = vb[1]; z2 = vb[2];

	/* addition */

/*	x1z2 = (x1*z2) %p; */
	mullm(x1,z2,&x1z2);

/*	v1 = (x2*z1-x1z2) %p; */
	lm_lazy = 1;
	mullm(x2,z1,&t); sublm(t,x1z2,&s);
	lm_lazy = 0; simplm(s,&v1);

/*	y1z2 = (y1*z2) %p; */
	mullm(y1,z2,&y1z2);

/*	u1 = (y2*z1-y1z2) %p; */
	lm_lazy = 1;
	mullm(y2,z1,&t); sublm(t,y1z2,&s);
	lm_lazy = 0; simplm(s,&u1);

	if( v1 != 0 ) {
/*		u2 = (u1*u1) %p; */
		mullm(u1,u1,&u2);

/*		v2 = (v1*v1) %p; */
		mullm(v1,v1,&v2);

/*		v3 = (v1*v2) %p; */
		mullm(v1,v2,&v3);

/*		z1z2 = (z1*z2) %p; */
		mullm(z1,z2,&z1z2);

/*		v2x1z2 = (v2*x1z2) %p; */
		mullm(v2,x1z2,&v2x1z2);

/*		a1 = (u2*z1z2-v3-2*v2x1z2) %p; */
		lm_lazy = 1;
		mullm(u2,z1z2,&t); addlm(v2x1z2,v2x1z2,&s);
		addlm(v3,s,&u); sublm(t,u,&s);
		lm_lazy = 0; simplm(s,&a1);

/*		x3 = ( v1 * a1 ) %p; */
		mullm(v1,a1,&x3);

/*		y3 = ( u1 * ( v2x1z2 - a1 ) - v3 * y1z2 ) %p; */
		lm_lazy = 1;
		sublm(v2x1z2,a1,&t); mullm(u1,t,&s); mullm(v3,y1z2,&u); sublm(s,u,&t);
		lm_lazy = 0; simplm(t,&y3);

/*		z3 = ( v3 * z1z2 ) %p; */
		mullm(v3,z1z2,&z3);
	} else if( u1 == 0 ) {
/*		w1 = (aa*z1*z1+3*x1*x1) %p; */
		lm_lazy = 1;
		mullm(z1,z1,&t); mullm(aa,t,&s);
		mullm(x1,x1,&t); mullm(THREE_LM,t,&u); addlm(s,u,&t);
		lm_lazy = 0; simplm(t,&w1);

/*		s1 = (y1*z1) %p; */
		mullm(y1,z1,&s1);

/*		s2 = (s1*s1) %p; */
		mullm(s1,s1,&s2);

/*		s3 = (s1*s2) %p; */
		mullm(s1,s2,&s3);

/*		s1y1 = (s1*y1) %p; */
		mullm(s1,y1,&s1y1);

/*		b1 = (s1y1*x1) %p; */
		mullm(s1y1,x1,&b1);

/*		h1 = (w1*w1-8*b1) %p; */
		lm_lazy = 1;
		mullm(w1,w1,&t); mullm(EIGHT_LM,b1,&s); sublm(t,s,&u);
		lm_lazy = 0; simplm(u,&h1);

/*		x3 = ( 2 * h1 * s1 ) %p; */
		lm_lazy = 1;
		mullm(h1,s1,&t); addlm(t,t,&s);
		lm_lazy = 0; simplm(s,&x3);

/*		y3 = ( w1 * ( 4 * b1 - h1 ) - 8 * s1y1 * s1y1 ) %p; */
		lm_lazy = 1;
		mullm(FOUR_LM,b1,&t); sublm(t,h1,&s); mullm(w1,s,&u);
		mullm(s1y1,s1y1,&t); mullm(EIGHT_LM,t,&s); sublm(u,s,&t);
		lm_lazy = 0; simplm(t,&y3);

/*		z3 = ( 8 * s3 ) %p; */
		mullm(EIGHT_LM,s3,&z3);
	} else {
		*pr = 0;
		return;
	}
	if ( !z3 )
		*pr = 0;
	else {
		MKVECT(r,3); *pr = r;
		vb = (LM *)BDY(r); vb[0] = x3; vb[1] = y3; vb[2] = z3;
	}
}

void ecm_chsgn_ff(p,pr)
VECT p;
VECT *pr;
{
	Obj m,x,y,z;
	LM tl;
	GF2N tg;
	Obj *vb;
	VECT r;

	if( !p ) {
		*pr = 0;
		return;
	}

	/*	x = p[0]; y = p[1]; z = p[2]; */
	vb = (Obj *)BDY(p); x = vb[0]; y = vb[1]; z = vb[2];
	switch ( current_ff ) {
		case FF_GFP:
			if ( !y ) 
				*pr = p;
			else {
				chsgnlm((LM)y,&tl);
				MKVECT(r,3); *pr = r;
				vb = (Obj *)BDY(r); vb[0] = x; vb[1] = (Obj)tl; vb[2] = z;
			}
			break;
		case FF_GF2N:
			addgf2n((GF2N)x,(GF2N)y,&tg);
			MKVECT(r,3); *pr = r;
			vb = (Obj *)BDY(r); vb[0] = x; vb[1] = (Obj)tg; vb[2] = z;
			break;
		default:
			error("ecm_chsgn_ff : current_ff is not set");
	}
}

void ecm_sub_ff(p1,p2,ec,pr)
VECT p1,p2,ec;
VECT *pr;
{
	VECT mp2;

	ecm_chsgn_ff(p2,&mp2);
	ecm_add_ff(p1,mp2,ec,pr);
}

/* tplist = [[t,p],...]; t:interger, p=[p0,p1]:point (vector)  */

int comp_kip(a,b)
struct oKeyIndexPair *a,*b;
{
	unsigned int ka,kb;

	ka = a->key; kb = b->key;
	if ( ka > kb )
		return 1;
	else if ( ka < kb )
		return -1;
	else
		return 0;
}

#define EC_GET_XZ(p,x,z) \
	if ( !(p) ) {\
		(x)=0; (z)=(LM)ONE;\
	} else { \
		LM *vb;\
		vb = (LM *)BDY((VECT)(p));\
		(x) = vb[0]; (z) = vb[2];\
	}

#define EC_GET_XZ_GF2N(p,x,z) \
	if ( !(p) ) {\
		(x)=0; (z)=(GF2N)ONE;\
	} else { \
		GF2N *vb;\
		vb = (GF2N *)BDY((VECT)(p));\
		(x) = vb[0]; (z) = vb[2];\
	}

void compute_all_key_homo_gfp(pa,len,ka)
VECT *pa;
int len;
unsigned int *ka;
{
	LM *b,*x,*z;
	int i;
	LM t,s,m;
	N lm;
	Q mod;

	b = (LM *)ALLOCA((len+1)*sizeof(LM));
	x = (LM *)ALLOCA(len*sizeof(LM));
	z = (LM *)ALLOCA(len*sizeof(LM));
	MKLM(ONEN,b[0]);
	for ( i = 1; i <= len; i++ ) {
		EC_GET_XZ(pa[i-1],x[i-1],z[i-1]);
		mullm(b[i-1],z[i-1],&b[i]);
	}
	/* b[0] = 1 */
	divlm(b[0],b[len],&m);
	for ( i = len-1; i >= 0; i-- ) {
		mullm(m,b[i],&s); mullm(s,x[i],&t); s = t;
		ka[i] = s ? s->body->b[0] : 0; ka[i] |= 0x80000000;
		mullm(m,z[i],&s); m = s;
	}
}

void compute_all_key_homo_gf2n(pa,len,ka)
VECT *pa;
int len;
unsigned int *ka;
{
	GF2N *b,*x,*z;
	int i;
	GF2N t,s,m;

	b = (GF2N *)ALLOCA((len+1)*sizeof(Q));
	x = (GF2N *)ALLOCA(len*sizeof(Q));
	z = (GF2N *)ALLOCA(len*sizeof(Q));
	MKGF2N(ONEUP2,b[0]);
	for ( i = 1; i <= len; i++ ) {
		EC_GET_XZ_GF2N(pa[i-1],x[i-1],z[i-1]);
		mulgf2n(b[i-1],z[i-1],&b[i]);
	}
	invgf2n(b[len],&m);
	for ( i = len-1; i >= 0; i-- ) {
		mulgf2n(m,b[i],&s); mulgf2n(s,x[i],&t); s = t;
		ka[i] = s ? s->body->b[0] : 0; ka[i] |= 0x80000000;
		mulgf2n(m,z[i],&s); m = s;
	}
}

unsigned int separate_vect(v,n)
double *v;
int n;
{
	unsigned int max = 1<<n;
	unsigned int i,j,i0;
	double all,a,total,m;

	for ( i = 0, all = 1; i < (unsigned int)n; i++ )
		all *= v[i];

	for ( i = 0, m = 0; i < max; i++ ) {
		for ( a = 1, j = 0; j < (unsigned int)n; j++ )
			if ( i & (1<<j) )
				a *= v[j];
		total = a+(all/a)*2;
		if ( !m || total < m ) {
			m = total;
			i0 = i;
		}
	}
	return i0;
}

void ecm_find_match(g,ng,b,nb,r)
unsigned int *g;
int ng;
unsigned int *b;
int nb;
LIST *r;
{
	int i,j;
	Q iq,jq;
	NODE n0,n1,c0,c;
	LIST l;

	for ( i = 0, c0 = 0; i < ng; i++ ) {
		j = find_match(g[i],b,nb);
		if ( j >= 0 ) {
			STOQ(i,iq); STOQ(j,jq);
			MKNODE(n1,jq,0); MKNODE(n0,iq,n1); MKLIST(l,n0);
			NEXTNODE(c0,c);
			BDY(c) = (pointer)l;
		}
	}
	if ( c0 )
		NEXT(c) = 0;
	MKLIST(*r,c0);
}

int find_match(k,key,n)
unsigned int k;
unsigned int *key;
int n;
{
	int s,e,m;

	for ( s = 0, e = n; (e-s) > 1; ) {
		m = (s+e)/2;
		if ( k==key[m] )
			return m;
		else if ( k > key[m] )
			s = m;
		else
			e = m;
	}
	if ( k == key[s] )
		return s;
	else
		return -1;
}

int nextvect1(vect,bound)
VECT vect,bound;
{
	int size,i,a;
	Q *vb,*bb;

	size = vect->len;
	vb = (Q *)vect->body;
	bb = (Q *)bound->body;
	for ( i = size-1; i >= 0; i-- )
		if ( (a=QTOS(vb[i])) < QTOS(bb[i]) ) {
			a++; STOQ(a,vb[i]);
			break;
		} else
			vb[i] = 0;
	return i;
}

void sort_ktarray(karray,tarray,rp)
VECT karray,tarray;
LIST *rp;
{
	LIST *lb;
	NODE r,r1;
	int i,i0,k,len,same,tsame;
	struct oKeyIndexPair *kip;
	VECT key,value,v;
	Q *tb,*samebuf;
	USINT *kb;
	Obj *svb;
	USINT *skb;

	len = karray->len;
	kb = (USINT *)karray->body;

	kip = (struct oKeyIndexPair *)ALLOCA(len*sizeof(struct oKeyIndexPair));
	for ( i = 0; i < len; i++ ) {
		kip[i].key = BDY(kb[i]); kip[i].index = i;
	}
	qsort((void *)kip,len,sizeof(struct oKeyIndexPair),
		(int (*)(const void *,const void *))comp_kip);

	for ( same = tsame = i = i0 = 0, k = 1; i < len; i++, tsame++ )
		if ( kip[i0].key != kip[i].key ) {
			i0 = i; k++;
			same = MAX(tsame,same);
			tsame = 0;
		}
	same = MAX(tsame,same);
	samebuf = (Q *)ALLOCA(same*sizeof(Q));

	MKVECT(key,k); skb = (USINT *)BDY(key);
	MKVECT(value,k); svb = (Obj *)BDY(value);

	tb = (Q *)tarray->body;
	for ( same = i = i0 = k = 0; i <= len; i++ ) {
		if ( i == len || kip[i0].key != kip[i].key ) {
			skb[k] = kb[kip[i0].index];
			if ( same > 1 ) {
				MKVECT(v,same);
				bcopy((char *)samebuf,(char *)v->body,same*sizeof(Q));
				svb[k] = (Obj)v;
			} else
				svb[k] = (Obj)samebuf[0];
			i0 = i;
			k++;
			same = 0;
			if ( i == len )
				break;
		}
		samebuf[same++] = tb[kip[i].index];
	}
	MKNODE(r1,value,0); MKNODE(r,key,r1); MKLIST(*rp,r);
}

