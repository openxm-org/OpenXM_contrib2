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
 * $OpenXM: OpenXM_contrib2/asir2000/engine-27/N-27.c,v 1.2 2000/08/21 08:31:32 noro Exp $ 
*/
#include "ca-27.h"
#include "base.h"

#ifndef HMEXT
#define HMEXT
#endif

#ifdef HMEXT
#undef FULLSET
#undef CALL
#undef DIVISION

int igcd_algorithm = 0;
    /*	== 0 : Euclid,
     *	== 1 : binary,
     *	== 2 : bmod,
     *	>= 3 : (Weber's accelerated)/(Jebelean's generalized binary) algorithm,
     */
int igcd_thre_inidiv = 50;
    /*
     *  In the non-Euclidean algorithms, if the ratio of the lengths (number
     *  of words) of two integers is >= igcd_thre_inidiv, we first perform
     *  remainder calculation.
     *  If == 0, this remainder calculation is not performed.
     */
int igcdacc_thre = 10;
    /*
     *  In the accelerated algorithm, if the bit-lengths of two integers is
     *  > igcdacc_thre, "bmod" reduction is done.
     */

#include "inline.h"

#define TRAILINGZEROS(t,cntr) for(cntr=0;(t&1)==0;t>>=1)cntr++;

#define W_NALLOC(d) ((N)ALLOCA(TRUESIZE(oN,(d)-1,int)))

#define ShouldCompRemInit(n1,n2) (igcd_thre_inidiv != 0 && PL(n1) >= igcd_thre_inidiv*PL(n2))

#define IniDiv(n1,n2) \
	if ( ShouldCompRemInit(n1,n2) ) {\
		N q, r; int w, b; \
		divn_27(n1,n2,&q,&r); \
		if ( !r ) return(n2); \
		b = trailingzerosn( r, &w ); \
		q = n1;  n1 = n2;  n2 = q; \
		rshiftn( r, w, b, n2 ); \
	}
	
/*
 *	Binary GCD algorithm by J.Stein
 *	[J. Comp. Phys. Vol. 1 (1967), pp. 397-405)]:
 *	The right-shift binary algorithm is used.
 */


/*
 *	subsidiary routines for gcdbinn below.
 */
static int /* number of bits */ trailingzeros_nbd( /* BD of N */ nbd, pnw )
int *nbd, *pnw /* number of zero words */;
{
	int nw, nb, w;

	for ( nw = 0; (w = *nbd) == 0; nbd++ ) nw++;
	TRAILINGZEROS(w,nb);
	*pnw = nw;
	return nb;
}

#define trailingzerosn(n,pnw) trailingzeros_nbd(BD(n),pnw)

static int /* PL of N */ rshift_nbd( /* BD of N */ nbd, /* PL of N */ nl,
		       /* # words */ shw, /* # bits */ shb, /* BD of N */ p )
int *nbd, nl, shw, shb, *p;
{
	int i, v, w, lshb;

	nbd += shw,  i = (nl -= shw);
	if ( shb == 0 ) {
		for ( ; nl > 0; nl-- ) *p++ = *nbd++;
		return i;
	} else if ( nl < 2 ) {
		*p = (*nbd) >> shb;
		return 1;
	}
	for ( lshb = BSH27 - shb, v = *nbd++; --nl > 0; v = w ) {
		w = *nbd++;
		*p++ = (v >> shb) | ((w << lshb)&BMASK27);
	}
	if ( (v >>= shb) == 0 ) return( i-1 );
	*p = v;
	return i;
}

#define rshiftn(ns,shw,shb,nd) (PL(nd)=rshift_nbd(BD(ns),PL(ns),shw,shb,BD(nd)))
     /* nd <= ns << (shb + shw*BSH27), returns PL of the result */

#ifdef FULLSET
static N N_of_i_lshifted_by_wb( i, gw, gb )
int i, gw, gb;
	/*
	 *	returns pointer to a new struct (N)(((int)i) >> (gb + gw*BSH27))
	 */
{
	int j, l, *p;
	N n;

	j = i >> (BSH27 - gb);
	i = (i << gb)&BMASK27;
	l = j != 0 ? gw + 2 : gw + 1;
	n = NALLOC(l);
	PL(n) = l;
	for ( p = BD(n); gw-- > 0; ) *p++ = 0;
	*p++ = i;
	if ( j != 0 ) *p = j;
	return n;
}
#endif /* FULLSET */

/*
 *	routines to make a new struct
 *		(N)(((BD of N)(b[0],...,b[lb-1])) << (gb + gw*BSH27))
 */
static N N_of_nbd_lshifted_by_wb( /* BD of N */ b, /* PL of N */ lb, gw, gb )
int *b, lb, gw, gb;
	/*
	 *	returns pointer to a new struct
	 *		(N)(((BD of N)(b[0],...,b[lb-1])) << (gb + gw*BSH27))
	 */
{
	int rsh, s, t, *p, l;
	N n;

	l = lb + gw;
	if ( gb == 0 ) {
		n = NALLOC(l);
		PL(n) = l;
		for ( p = BD(n); gw-- > 0; ) *p++ = 0;
		while ( lb-- > 0 ) *p++ = *b++;
		return n;
	}
	rsh = BSH27 - gb;  s = b[lb-1];
	if ( (t = s >> rsh) != 0 ) {
		n = NALLOC(l+1);
		PL(n) = l+1;
		(p = BD(n))[l] = t;
	} else {
		n = NALLOC(l);
		PL(n) = l;
		p = BD(n);
	}
	while ( gw-- > 0 ) *p++ = 0;
	*p++ = ((t = *b++) << gb)&BMASK27;
	for ( ; --lb > 0; t = s )
		*p++ = (t >> rsh) | (((s = *b++) << gb)&BMASK27);
	return n;
}

#define N_of_n_lshifted_by_wb(a,gw,gb) N_of_nbd_lshifted_by_wb(BD(a),PL(a),gw,gb)

#define SWAP(a,b,Type) { Type temp=a;a=b;b=temp;}
#define SIGNED_VAL(a,s) ((s)>0?(a):-(a))


#ifdef CALL
static int bw_int32( n )
int n;
{
	int w;

	w = 0;
#if BSH27 >= 32
	if ( n > 0xffffffff ) w += 32, n >>= 32;
#endif
	if ( n >= 0x10000 ) w += 16, n >>= 16;
	if ( n >=   0x100 ) w +=  8, n >>=  8;
	if ( n >=    0x10 ) w +=  4, n >>=  4;
	if ( n >=     0x4 ) w +=  2, n >>=  2;
	if ( n >=     0x2 ) w +=  1, n >>=  1;
	if ( n != 0 ) ++w;
	return w;
}
#define BitWidth(n,bw) bw = bw_int32( n )
#else

#if BSH27 >= 32
#define BitWidth(n,bw) {\
	int k = (n); \
	bw = 0; \
	if ( BSH27 >= 32 ) if ( k > 0xffffffff ) bw += 32, k >>= 32; \
	if ( k >= 0x10000 ) bw += 16, k >>= 16; \
	if ( k >=   0x100 ) bw +=  8, k >>=  8; \
	if ( k >=    0x10 ) bw +=  4, k >>=  4; \
	if ( k >=     0x4 ) bw +=  2, k >>=  2; \
	if ( k >=     0x2 ) bw +=  1, k >>=  1; \
	if ( k != 0 ) bw++; \
}
#else
#define BitWidth(n,bw) {\
	int k = (n); \
	bw = 0; \
	if ( k >= 0x10000 ) bw += 16, k >>= 16; \
	if ( k >=   0x100 ) bw +=  8, k >>=  8; \
	if ( k >=    0x10 ) bw +=  4, k >>=  4; \
	if ( k >=     0x4 ) bw +=  2, k >>=  2; \
	if ( k >=     0x2 ) bw +=  1, k >>=  1; \
	if ( k != 0 ) bw++; \
}
#endif
#endif

#include "igcdhack.c"

/*
 *	Implementation of the binary GCD algorithm for two oN structs
 *	(big-integers) in risa.
 *
 *	The major operations in the following algorithms are the binary-shifts
 *	and the updates of (u, v) by (min(u,v), |u-v|), and are to be open-coded
 *	without using routines for oN structures just as in addn() or subn().
 */

static int igcd_binary_2w( u, lu, v, lv, pans )
int *u, lu, *v, lv, *pans;
	/*  both u[0:lu-1] and v[0:lv-1] are assumed to be odd */
{
	int i, h1, l1, h2, l2;

	l1 = u[0],  l2 = v[0];
	h1 = lu <= 1 ? 0 : u[1];
	h2 = lv <= 1 ? 0 : v[1];
	/**/
loop:	if ( h1 == 0 ) {
	no_hi1:	if ( h2 == 0 ) goto one_word;
	no_hi1n:if ( l1 == 1 ) return 0;
		if ( (l2 -= l1) == 0 ) {
			for ( l2 = h2; (l2&1) == 0; l2 >>= 1 ) ;
			goto one_word;
		} else if ( l2 < 0 ) h2--, l2 += BASE27;
		i = 0;  do { l2 >>= 1, i++; } while ( (l2&1) == 0 );
		l2 |= ((h2 << (BSH27 - i)) & BMASK27);
		h2 >>= i;
		goto no_hi1;
	} else if ( h2 == 0 ) {
	no_hi2:	if ( l2 == 1 ) return 0;
		if ( (l1 -= l2) == 0 ) {
			for ( l1 = h1; (l1&1) == 0; l1 >>= 1 ) ;
			goto one_word;
		} else if ( l1 < 0 ) h1--, l1 += BASE27;
		i = 0;  do { l1 >>= 1, i++; } while ( (l1&1) == 0 );
		l1 |= ((h1 << (BSH27 - i)) & BMASK27);
		if ( (h1 >>= i) == 0 ) goto one_word;
		goto no_hi2;
	} else if ( l1 == l2 ) {
		if ( h1 == h2 ) {
			pans[0] = l1, pans[1] = h1;
			return 2;
		} else if ( h1 > h2 ) {
			for ( l1 = h1 - h2; (l1&1) == 0; l1 >>= 1 ) ;
			goto no_hi1n;
		} else {
			for ( l2 = h2 - h1; (l2&1) == 0; l2 >>= 1 ) ;
			goto no_hi2;
		}
	} else if ( h1 == h2 ) {
		if ( l1 > l2 ) {
			for ( l1 -= l2; (l1&1) == 0; l1 >>= 1 ) ;
			goto no_hi1n;
		} else {
			for ( l2 -= l1; (l2&1) == 0; l2 >>= 1 ) ;
			goto no_hi2;
		}
	} else if ( h1 > h2 ) {
		h1 -= h2;
		if ( (l1 -= l2) < 0 ) h1--, l1 += BASE27;
		i = 0; do { l1 >>= 1, i++; } while ( (l1&1) == 0 );
		l1 |= ((h1 << (BSH27 - i)) & BMASK27);
		h1 >>= i;
	} else {
		h2 -= h1;
		if ( (l2 -= l1) < 0 ) h2--, l2 += BASE27;
		i = 0;  do { l2 >>= 1, i++; } while ( (l2&1) == 0 );
		l2 |= ((h2 << (BSH27 - i)) & BMASK27);
		h2 >>= i;
	}
	goto loop;
one_word:
	if ( l1 == 1 || l2 == 1 ) return 0;
	else if ( l1 == l2 ) {
		pans[0] = l1;
		return 1;
	}
one_word_neq:
	if ( l1 > l2 ) {
		l1 -= l2;
		do { l1 >>= 1; } while ( (l1&1) == 0 );
		goto one_word;
	} else {
		l2 -= l1;
		do { l2 >>= 1; } while ( (l2&1) == 0 );
		goto one_word;
	}
}

static N igcd_binary( n1, n2, nt )
N n1, n2, nt;
	/*  both n1 and n2 are assumed to be odd */
{
	int l1, *b1, l2, *b2, *bt = BD(nt);
	int l;

	if ( (l = cmpn( n1, n2 )) == 0 ) return n1;
	else if ( l < 0 ) { SWAP( n1, n2, N ); }
	IniDiv( n1, n2 );
	if ( UNIN(n2) ) return 0;
	l1 = PL(n1), b1 = BD(n1),  l2 = PL(n2), b2 = BD(n2);
loop:	if ( l1 <= 2 && l2 <= 2 ) {
		l = igcd_binary_2w( b1, l1, b2, l2, bt );
		if ( l == 0 ) return 0;
		PL(nt) = l;
		return nt;
	}
	/**/
	l = abs_U_V_maxrshift( b1, l1, b2, l2, bt );
	/**/
	if ( l == 0 ) {
		PL(n1) = l1;
		return n1;
	} else if ( l > 0 ) {
		l1 = l;
		SWAP( b1, bt, int * ); SWAP( n1, nt, N );
	} else {
		l2 = -l;
		SWAP( b2, bt, int * ); SWAP( n2, nt, N );
	}
	goto loop;
}

#define RetTrueGCD(p,gw,gb,nr,l0) \
  if (p==0) { l0: if (gw==0&&gb==0) { *(nr)=ONEN; return; } else p=ONEN; } \
  *(nr) = N_of_n_lshifted_by_wb(p,gw,gb); \
  return;

void gcdbinn( n1, n2, nr )
N n1, n2, *nr;
{
	int s1, s2, gw, gb, t1, t2;
	int w1, w2;
	N tn1, tn2, tnt, p;

	if ( !n1 ) {
		*nr = n2;
		return;
	} else if ( !n2 ) {
		*nr = n1;
		return;
	}
	s1 = trailingzerosn( n1, &w1 );
	s2 = trailingzerosn( n2, &w2 );
	if ( w1 == w2 ) gw = w1,  gb = s1 <= s2 ? s1 : s2;
	else if ( w1 < w2 ) gw = w1,  gb = s1;
	else gw = w2,  gb = s2;
	/*
	 *	true GCD must be multiplied by 2^{gw*BSH27+gb}.
	 */
	t1 = PL(n1) - w1;
	t2 = PL(n2) - w2;
	if ( t1 < t2 ) t1 = t2;
	tn1 = W_NALLOC(t1);  tn2 = W_NALLOC(t1);  tnt = W_NALLOC(t1);
	rshiftn( n1, w1, s1, tn1 );
	rshiftn( n2, w2, s2, tn2 );
	p = igcd_binary( tn1, tn2, tnt );
	RetTrueGCD( p, gw, gb, nr, L0 )
}


/*
 *	The bmod gcd algorithm stated briefly in K.Weber's paper
 *	[ACM TOMS, Vol.21, No. 1 (1995), pp. 111-122].
 *	It replaces the subtraction (n1 - n2) in the binary algorithm
 *	by (n1 - S*n2) with such an S that (n1 - S*n2) \equiv 0 \bmod 2^BSH27,
 *	which should improve the efficiency when n1 \gg n2.
 */

/* subsidiary routines */
#ifdef CALL
#ifndef DIVISION
static int u_div_v_mod_2tos( u, v, s )
int u, v, s;
	/*
	 *    u/v mod 2^s.
	 */
{
	int i,lsh_i, mask, m, two_to_s;

	mask = (two_to_s = 1 << s) - 1;
	lsh_i = (sizeof(int) << 3) - 1;
	m = i = 0;
	u &= mask,  v &= mask;
	do {
		if ( u == 0 ) break;
		if ( (u << lsh_i) != 0 ) {
			m += (1 << i);
			u -= (v << i);
			u &= mask;
		}
		lsh_i--;
	} while ( ++i != s );
	return m;
}
#else
static int u_div_v_mod_2tos( u, v, s )
int u, v, s;
{
	int f1 = 1 << s, f2 = u, q, r, c1 = 0, c2 = v, m;

	m = f1 - 1;
	do {	q = f1 / f2;
		r = f1 - q*f2;  f1 = f2;  f2 = r;
		r = c1 - q*c2;  c1 = c2;  c2 = r;
	} while ( f2 != 1 );
	return( c2 & m );
}
#endif /* DIVISION */

#define Comp_U_div_V_mod_BASE27(U,V,R) R = u_div_v_mod_2tos(U,V,BSH27)
#else
#ifndef DIVISION
#define Comp_U_div_V_mod_BASE27(U,V,R) {\
	int u = (U), v = (V), i, lsh; \
	/* U and V are assumed to be odd */ \
	i = R = 1, lsh = (sizeof(int) << 3) - 2;  u = (u - v) & BMASK27; \
	do {	if ( u == 0 ) break; \
		if ( (u << lsh) != 0 ) R += (1 << i),  u = (u - (v << i)) & BMASK27; \
		i++, lsh--; \
	} while ( i < BSH27 ); \
}
#else
#define Comp_U_div_V_mod_BASE27(U,V,R) {\
	int f1 = BASE27, f2 = (V), q, r, c1 = 0, c2 = (U); \
	do {	q = f1 / f2; \
		r = f1 - q*f2;  f1 = f2;  f2 = r; \
		r = c1 - q*c2;  c1 = c2;  c2 = r; \
	} while ( f2 != 1 ); \
	R = c2 & BMASK27; \
}
#endif /* DIVISION */
#endif


static int bmod_n( nu, nv, na )
N nu, nv, na;
	/*
	 *	Computes (u[] \bmod v[]) >> (as much as possible) in r[].
	 */
{
	int *u = BD(nu), lu = PL(nu), *v = BD(nv), lv = PL(nv),
		     *r = BD(na);
	int *p, a, t, l, z, v0, vh, bv, v0r;

	v0 = v[0];
	if ( lv == 1 ) {
		if ( lu == 1 ) a = u[0] % v0;
		else {
			p = &u[--lu];
			a = (*p) % v0, t = BASE27 % v0;
			for ( ; --lu >= 0; a = l ) {
				--p;
				DMAR(a,t,*p,v0,l)
				/*  l <= (a*t + p[0])%v0   */
			}
		}
		if ( a == 0 ) return 0;
		while ( (a&1) == 0 ) a >>= 1;
		*r = a;
		return( PL(na) = 1 );
	}
	Comp_U_div_V_mod_BASE27( 1, v0, v0r );
	vh = v[lv -1];
	BitWidth( vh, bv );
	bv--;
	t = 1 << bv;
	l = lv + 1;
	for ( z = -1; lu > l || lu == l && u[lu-1] >= t; z = -z ) {
		a = (v0r*u[0])&BMASK27;
		/**/
		lu = abs_U_aV_maxrshift( u, lu, a, v, lv, r );
		/**/
		if ( lu == 0 ) return 0;
		p = r;
		r = u;
		u = p;
	}
	if ( lu < lv ) goto ret;
	t = u[lu-1];
	if ( lu > lv ) l = BSH27;
	else if ( t < vh ) goto ret;
	else l = 0;
	BitWidth( t, a );
	l += (a - bv);
	a = (v0r*u[0])&(BMASK27 >> (BSH27 - l));
	/**/
	lu = abs_U_aV_maxrshift( u, lu, a, v, lv, r );
	/**/
	if ( lu == 0 ) return 0;
	z = -z;
ret:	if ( z > 0 ) return( PL(na) = lu );
	PL(nu) = lu;
	return( -lu );
}


static N igcd_bmod( n1, n2, nt )
N n1, n2, nt;
	/*  both n1 and n2 are assumed to be odd */
{
	int l1, l2;
	int l;

	if ( (l = cmpn( n1, n2 )) == 0 ) return n1;
	else if ( l < 0 ) { SWAP( n1, n2, N ); }
	IniDiv( n1, n2 );
	if ( UNIN(n2) ) return 0;
loop:	if ( (l1 = PL(n1)) <= 2 && (l2 = PL(n2)) <= 2 ) {
		l = igcd_binary_2w( BD(n1), l1, BD(n2), l2, BD(nt) );
		if ( l == 0 ) return 0;
		PL(nt) = l;
		return nt;
	}
	/**/
	l = bmod_n( n1, n2, nt );
	/**/
	if ( l == 0 ) return n2;
	else if ( l > 0 ) {
		N tmp = n1;

		n1 = n2;
		n2 = nt;
		nt = tmp;
	} else SWAP( n1, n2, N );
	goto loop;
}

void gcdbmodn( n1, n2, nr )
N n1, n2, *nr;
{
	int s1, s2, gw, gb, t1, t2;
	int w1, w2;
	N tn1, tn2, tnt, p;

	if ( !n1 ) {
		*nr = n2;
		return;
	} else if ( !n2 ) {
		*nr = n1;
		return;
	}
	s1 = trailingzerosn( n1, &w1 );
	s2 = trailingzerosn( n2, &w2 );
	if ( w1 == w2 ) gw = w1,  gb = s1 <= s2 ? s1 : s2;
	else if ( w1 < w2 ) gw = w1,  gb = s1;
	else gw = w2,  gb = s2;
	/*
	 *	true GCD must be multiplied by 2^{gw*BSH27+gs}.
	 */
	t1 = PL(n1) - w1;
	t2 = PL(n2) - w2;
	if ( t1 < t2 ) t1 = t2;
	tn1 = W_NALLOC(t1);  tn2 = W_NALLOC(t1);  tnt = W_NALLOC(t1);
	rshiftn( n1, w1, s1, tn1 );
	rshiftn( n2, w2, s2, tn2 );
	p = igcd_bmod( tn1, tn2, tnt );
	RetTrueGCD( p, gw, gb, nr, L0 )
}

/*
 *	The accelerated integer GCD algorithm by K.Weber
 *	[ACM TOMS, Vol.21, No. 1 (1995), pp. 111-122]:
 */

static int ReducedRatMod( x, y, pn, pd )
N x, y;
int *pn, *pd;
	/*
	 *    Let m = 2^{2*BSH27} = 2*BASE27.  We assume x, y > 0 and \gcd(x,m)
	 *    = \gcd(y,m) = 1.  This routine computes n and d (resp. returned
	 *    in *pn and *pd) such that 0 < n, |d| < \sqrt{m} and
	 *    n*y \equiv x*d \bmod m.
	 */
{
	int n1h, n1l, d1h, d1l, n2h, n2l, d2h, d2l;
	int s1, s2, l1, l2;


	{
		int xh, xl, yh, yl, tl, i, lsh_i;
		int th;

		xl = BD(x)[0];
		xh = PL(x) > 1 ? BD(x)[1] : 0;
		yl = BD(y)[0];
		yh = PL(y) > 1 ? BD(y)[1] : 0;
		Comp_U_div_V_mod_BASE27( xl, yl, n2l );
		DM27(n2l,yl,th,tl)  /* n2l*yl = tl+th*BASE27, where tl==xl. */;
		if ( xh > th ) xh -= th;
		else xh += (BASE27 - th);
		DM27(n2l,yh,th,tl)  /* n2l*yh = tl+th*BASE27. */;
		if ( xh > tl ) xh -= tl;
		else xh += (BASE27 - tl);
		n2h = i = 0,  lsh_i = 31;
		do {
			if ( xh == 0 ) break;
			if ( (xh << lsh_i) != 0 ) {
				n2h += (1 << i);
				tl = (yl << i)&BMASK27;
				if ( xh > tl ) xh -= tl;
				else xh += (BASE27 - tl);
			}
			lsh_i--;
		} while ( ++i != BSH27 );
	}
	/*
	 *	n2l + n2h*BASE27 = x/y mod 2^{2*BSH27}.
	 */
	n1h = BASE27, n1l = 0, l1 = BSH27,
	d1h = d1l = 0, s1 = 0,
	d2h = 0, d2l = 1, s2 = 1;
	/**/
	while ( n2h != 0 ) {
		int i, ir, th, tl;

		BitWidth( n2h, l2 );
		ir = BSH27 - (i = l1 - l2);
		do {
			if ( i == 0 ) th = n2h, tl = n2l;
			else
				th = (n2h << i) | (n2l >> ir),
				tl = (n2l << i) & BMASK27;
			if ( th > n1h || (th == n1h && tl > n1l) ) goto next_i;
			if ( tl <= n1l ) n1l -= tl;
			else n1l += (BASE27 - tl), n1h--;
			n1h -= th;
			/*  (s1:d1h,d1l) -= ((s2:d2h,d2l) << i);  */
			if ( s2 != 0 ) {
				if ( i == 0 ) th = d2h, tl = d2l;
				else
					th = (d2h << i)&BMASK27 | (d2l >> ir),
					tl = (d2l << i)&BMASK27;
				if ( s1 == 0 )
					s1 = -s2, d1h = th, d1l = tl;
				else if ( s1 != s2 ) {
					tl += d1l;
					d1l = tl&BMASK27;
					d1h = (th + (tl >> BSH27))&BMASK27;
					if ( d1h == 0 && d1l == 0 ) s1 = 0;
				} else if ( d1h > th ) {
					if ( d1l >= tl ) d1l -= tl;
					else d1l += (BASE27 - tl), d1h--;
					d1h -= th;
				} else if ( d1h == th ) {
					d1h = 0;
					if ( d1l == tl ) s1 = d2h = 0;
					else if ( d1l > tl ) d1l -= tl;
					else d1l = tl - d1l, s1 = -s1;
				} else {
					if ( tl >= d1l ) d1l = tl - d1l;
					else d1l = tl + (BASE27 - d1l), th--;
					d1h = th - d1h;
					s1 = -s1;
				}
			}
		next_i:	i--, ir++;
		} while ( n1h > n2h || (n1h == n2h && n1l >= n2l) );
		/*  swap 1 and 2  */
		th = n1h, tl = n1l;
		n1h = n2h, n1l = n2l;
		n2h = th, n2l = tl;
		l1 = l2;
		th = d1h, tl = d1l, i = s1;
		d1h = d2h, d1l = d2l, s1 = s2;
		d2h = th, d2l = tl, s2 = i;
	}
	/**/
	*pn = n2l,  *pd = d2l;
	return s2;
}

static int igcd_spurious_factor;

#define SaveN(s,d) {\
	int i, l; \
	for ( l = PL(d) = PL(s), i = 0; i < l; i++ ) BD(d)[i] = BD(s)[i]; \
}

static N igcd_acc( n1, n2, nt )
N n1, n2, nt;
	/*  both n1 and n2 are assumed to be odd */
{
	int l1, l2, *b1, *b2, bw1, bw2;
	int l;
	int n, d;
	N p, s1, s2;

	if ( (l = cmpn( n1, n2 )) == 0 ) return n1;
	else if ( l < 0 ) { SWAP( n1, n2, N ); }
	if ( ShouldCompRemInit(n1,n2) ) {
		int w, b;

		divn_27( n1, n2, &s1, &s2 );
		if ( !s2 ) return n2;
		b = trailingzerosn( s2, &w );
		p = n1;  n1 = n2;  n2 = p;
		rshiftn( s2, w, b, n2 );
		if ( UNIN(n2) ) return 0;
		l1 = PL(n1);
		if ( !s1 || PL(s1) < l1 ) s1 = NALLOC(l1);
	} else if ( UNIN(n2) ) return 0;
	else {
		s1 = NALLOC(PL(n1));
		s2 = NALLOC(PL(n2));
	}
	SaveN( n1, s1 );
	SaveN( n2, s2 );
	igcd_spurious_factor = 0;
loop:	l1 = PL(n1), l2 = PL(n2);
	if ( l1 <= 2 && l2 <= 2 ) {
		l = igcd_binary_2w( BD(n1), l1, BD(n2), l2, BD(nt) );
		if ( l == 0 ) return 0;
		PL(nt) = l;
		SWAP( n2, nt, N );
		goto ret;
	}
	/**/
	b1 = BD(n1), b2 = BD(n2);
	BitWidth( b1[l1 -1], bw1 );
	BitWidth( b2[l2 -1], bw2 );
	if ( (l1*BSH27 + bw1) - (l2*BSH27 + bw2) <= igcdacc_thre ) {
		l = ReducedRatMod( n1, n2, &n, &d );
		l = l < 0 ? aUplusbV_maxrshift( n, b2, l2, d, b1, l1, BD(nt) ) :
		    abs_axU_bxV_maxrshift( n, b2, l2, d, b1, l1, BD(nt) );
		igcd_spurious_factor++;
		if ( l == 0 ) goto ret;
		PL(nt) = l;
	} else {
		l = bmod_n( n1, n2, nt );
		if ( l == 0 ) goto ret;
		else if ( l < 0 ) {
			SWAP( n1, n2, N );
			goto loop;
		}
	}
	p = n1;
	n1 = n2;
	n2 = nt;
	nt = p;
	goto loop;
	/**/
ret:	if ( igcd_spurious_factor != 0 && !UNIN(n2) ) {
		if ( (p = igcd_bmod( n2, s1, n1 )) == 0 ) return 0;
		if ( (p = igcd_bmod( p, s2, nt )) == 0 ) return 0;
		return p;
	} else return n2;
}

void gcdaccn( n1, n2, nr )
N n1, n2, *nr;
{
	int s1, s2, gw, gb, t1, t2;
	int w1, w2;
	N tn1, tn2, tnt, p;

	if ( !n1 ) {
		*nr = n2;
		return;
	} else if ( !n2 ) {
		*nr = n1;
		return;
	}
	s1 = trailingzerosn( n1, &w1 );
	s2 = trailingzerosn( n2, &w2 );
	if ( w1 == w2 ) gw = w1,  gb = s1 <= s2 ? s1 : s2;
	else if ( w1 < w2 ) gw = w1,  gb = s1;
	else gw = w2,  gb = s2;
	/*
	 *	true GCD must be multiplied by 2^{gw*BSH27+gs}.
	 */
	t1 = PL(n1) - w1;
	t2 = PL(n2) - w2;
	if ( t1 < t2 ) t1 = t2;
	tn1 = W_NALLOC(t1);  tn2 = W_NALLOC(t1);  tnt = W_NALLOC(t1);
	rshiftn( n1, w1, s1, tn1 );
	rshiftn( n2, w2, s2, tn2 );
	/**/
	p = igcd_acc( tn1, tn2, tnt );
	/**/
	if ( p == 0 ) goto L0;
	RetTrueGCD( p, gw, gb, nr, L0 )
}


/********************************/

void gcdBinary_27n( n1, n2, nr )
N n1, n2, *nr;
{
	int b1, b2, w1, w2, gw, gb;
	int l1, l2;
	N tn1, tn2, tnt, a;

	if ( !n1 ) {
		*nr = n2; return;
	} else if ( !n2 ) {
		*nr = n1; return;
	}
	b1 = trailingzerosn( n1, &w1 );
	b2 = trailingzerosn( n2, &w2 );
	if ( w1 == w2 ) gw = w1, gb = b1 <= b2 ? b1 : b2;
	else if ( w1 < w2 ) gw = w1, gb = b1;
	else gw = w2, gb = b2;
	/*
	 *	true GCD must be multiplied by 2^{gw*BSH27+gb}.
	 */
	l1 = PL(n1) - w1;
	l2 = PL(n2) - w2;
	if ( l1 < l2 ) l1 = l2;
	tn1 = W_NALLOC( l1 );  tn2 = W_NALLOC( l1 );  tnt = W_NALLOC( l1 );
	rshiftn( n1, w1, b1, tn1 );
	rshiftn( n2, w2, b2, tn2 );
	/**/
	if ( igcd_algorithm == 1 ) {
		a = igcd_binary( tn1, tn2, tnt );
	} else if ( igcd_algorithm == 2 ) {
		a = igcd_bmod( tn1, tn2, tnt );
	} else {
		a = igcd_acc( tn1, tn2, tnt, -igcd_algorithm );
		if ( igcd_spurious_factor != 0 ) {
		}
	}
	RetTrueGCD( a, gw, gb, nr, L0 )
}

/**************************/
N maxrshn( n, p )
N n;
int *p;
{
	int nw, nb, c, l;
	N new;

	nb = trailingzerosn( n, &nw );
	l = PL(n);
	c = BD(n)[l -1];
	l -= nw;
	if ( (c >> nb) == 0 ) l--;
	new = NALLOC(l);
	rshiftn( n, nw, nb, new );
	*p = nb + nw*BSH27;
	return new;
}
#endif /* HMEXT */
