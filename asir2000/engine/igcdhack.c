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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/igcdhack.c,v 1.4 2000/12/21 02:51:45 murao Exp $ 
*/
#if 0
#include "base.h"

#define SWAP(a,b,Type) { Type temp=a; a = b; b = temp; }
#define SIGNED_VAL(a,s) ((s)>0?(a): -(a))
#endif

static int negate_nbd( b, l )
int *b, l;
    /*  It is assumed that l > 0 and b[0] != 0 */
{
  int i, nz;

#if BSH == BIT_WIDTH_OF_INT
  *b = - *b;
  for ( nz = i = 1 ; i < l; ) {
    b++, i++;
    if ( (*b ^= BMask) != 0 ) nz = i;
  }
#else
  *b = BASE - *b;
  for ( nz = i = 1 ; i < l; ) {
    b++, i++;
    if ( (*b ^= BMASK) != 0 ) nz = i;
  }
#endif
  return nz;
}

/****************/
/****************/

static int abs_U_V_maxrshift( u, lu, v, lv, a )
#if BSH == BIT_WIDTH_OF_INT
unsigned
#endif
int *u, lu, *v, lv, *a;
    /*
     *  Compute | U - V | >> (as much as possible) in a[], where U = u[0:lu-1]
     *  and V = v[0:lv-1].  Value returned is the length of the result
     *  multiplied by the sign of (U-V).
     */
{
  int sign;
#if BSH == BIT_WIDTH_OF_INT
  unsigned
#endif
  int rsh, t, b, i, i_1, l, *p = a;

  /* first, determine U <, ==, > V */
  sign = 1;
  if ( lu == lv ) {
    int i;

    for ( i = lu -1; i >= 0; i-- ) {
      if ( u[i] == v[i] ) continue;
      lu = lv = i + 1;
      if ( u[i] > v[i] ) goto U_V;
      else goto swap;
    }
    return 0;
  } else if ( lu > lv ) goto U_V;
 swap:
  SWAP( lu, lv, int );  SWAP( u, v, int * );
  sign = -1;
 U_V:	/* U > V, i.e., (lu > lv) || (lu == lv && u[lu-1] > v[lv-1]) */
  for ( i = 0; i < lv; i++ ) if ( u[i] != v[i] ) break;
  if ( i != 0 ) u += i, lu -= i,  v += i, lv -= i;
  if ( lv == 0 ) {	/* no more pv[], and lu > 0 */
    i = trailingzeros_nbd( u, &rsh );
    i = rshift_nbd( u, lu, rsh, i, a );
    return SIGNED_VAL( i, sign );
  }
  /**/
#if BSH == BIT_WIDTH_OF_INT
  t = u[0] - v[0];
  b = u[0] >= v[0] ? 0 : 1;
#else
  if ( (t = u[0] - v[0]) >= 0 ) b = 0;
  else b = 1,  t += BASE;
#endif
  TRAILINGZEROS( t, rsh );
  /**/
  if ( rsh != 0 ) {
#if BSH == BIT_WIDTH_OF_INT
    unsigned
#endif
    int w, lsh = BSH - rsh;

    /* subtract v[*] from u[*] */
    for ( i = 1, l = 0; i < lv; i++, t = w >> rsh ) {
#if BSH == BIT_WIDTH_OF_INT
      w = u[i] - v[i] - b;
      b = (u[i] > v[i]) || (u[i] == v[i] && b == 0) ? 0 : 1;
      if ( (*p++ = t | (w << lsh)) != 0 ) l = i;
#else
      if ( (w = u[i] - v[i] - b) >= 0 ) b = 0;
      else b = 1,  w += BASE;
      if ( (*p++ = t | ((w << lsh) & BMASK)) != 0 ) l = i;
#endif
    }
    /* subtraction of V completed. Note there may exist non-0 borrow b */
    if ( i >= lu ) /* lu == lv, and b==0 is implied */ goto chk_nz;
    /* we still have (lu-i) elms in U */
#if BSH == BIT_WIDTH_OF_INT
    if ( b != 0 ) {	/* non-zero borrow */
      if ( (w = u[i++] -1) != 0 ) t |= (w << lsh);
      if ( i >= lu ) {	/* lu == lv+1.  The M.S. word w may become 0 */
	if ( w != 0 ) {
	  *p++ = t;
	  l = lv;
	  t = w >> rsh;
	} else lu--;
	goto chk_nz;
      }
      /**/
      *p++ = t;
      if ( w == BMask ) {
	while ( (w = u[i++]) == 0 ) *p++ = BMask;
	w -= 1;
	*p++ = (BMask >> rsh) | (w << lsh);
      }
      t = w >> rsh;
    }
    for ( ; i < lu; i++, t = w >> rsh ) *p++ = t | ((w = u[i]) << lsh);
#else
    if ( b != 0 ) {	/* non-zero borrow */
      if ( (w = u[i++] -1) != 0 ) t |= ((w << lsh) & BMASK);
      if ( i >= lu ) {	/* lu == lv+1.  The M.S. word w may become 0 */
	if ( w != 0 ) {
	  *p++ = t;
	  l = lv;
	  t = w >> rsh;
	} else lu--;
	goto chk_nz;
      }
      *p++ = t;
      if ( w < 0 ) {
	while ( (w = u[i++]) == 0 ) *p++ = BMASK;
	w -= 1;
	*p++ = (BMASK >> rsh) | ((w << lsh) & BMASK);
      }
      t = w >> rsh;
    }
    for ( ; i < lu; i++, t = w >> rsh ) *p++ = t | (((w = u[i]) << lsh) & BMASK);
#endif
    l = i -1;
  chk_nz:
    if ( t != 0 ) *p = t, l = /*1 + (p - a)*/ lu;
    goto ret_nz;
  }
  /* rsh == 0 : need not be right-shifted */
  *p++ = t, l = 1;
  for ( i = 1; i < lv; ) {
#if BSH == BIT_WIDTH_OF_INT
    t = u[i] - v[i] - b;
    b = (u[i] > v[i]) || (u[i] == v[i] && b == 0) ? 0 : 1;
    i++;
    if ( (*p++ = t) != 0 ) l = i;
#else
    if ( (t = u[i] - v[i] - b) >= 0 ) b = 0;
    else b = 1, t += BASE;
    i++;
    if ( (*p++ = t) != 0 ) l = i;
#endif
  }
  if ( i >= lu ) /* lu == lv, and b==0 is implied */ goto ret_nz;
  if ( b != 0 ) {	/* non-zero borrow */
    t = u[i++] -1;
    if ( i >= lu ) /* lu == lv+1.  The M.S. word w may beome 0 */ goto chk_nz;
#if BSH == BIT_WIDTH_OF_INT
    if ( t == BMask ) {
      do *p++ = BMask; while ( (t = u[i++]) == 0 );
      t -= 1;
      if ( i >= lu ) {
	l = lu;
	if ( t == 0 ) l--;
	else *p = t;
	goto ret_nz;
      }
    }
#else
    if ( t < 0 ) {
      do *p++ = BMASK; while ( (t = u[i++]) == 0 );
      t -= 1;
      if ( i >= lu ) {
	l = lu;
	if ( t == 0 ) l--;
	else *p = t;
	goto ret_nz;
      }
    }
#endif
    *p++ = t;
  }
  for ( ; i < lu; i++ ) *p++ = u[i];
  l = lu;
  /**/
 ret_nz:
  return SIGNED_VAL( l, sign );
}

/****************/
/****************/

#ifdef CALL
static int aV_plus_c(), aV_plus_c_sh();
#endif
static int abs_U_aV_c(), abs_U_aV_c_sh();
static int abs_aVplusc_U(), abs_aVplusc_U_sh();

static int abs_U_aV_maxrshift( u, lu, a, v, lv, ans )
#if BSH == BIT_WIDTH_OF_INT
unsigned
#endif
int *u, lu, a, *v, lv, *ans;
{
#if BSH == BIT_WIDTH_OF_INT
  unsigned int i, t, c, rsh, *p = ans, w, lsh, b, hi;
  int l;
#else
  int i, t, c, rsh, l, *p = ans, w, lsh, hi;
#endif

  if ( a == 1 ) return( (l = abs_U_V_maxrshift( u, lu, v, lv, ans )) >= 0 ? l : -l );
  /**/
  for ( l = lu <= lv ? lu : lv, i = c = 0; i < l; ) {
    DMA( a, v[i], c, hi, t );
    c = hi;
    if ( (w = u[i++]) == t ) continue;
    /******/
    u += i, lu -= i,  v += i, lv -= i;
    /*  | t + BASE*(c + a*v[0:lv-1] - u[0:lu-1] | */
    if ( lv < lu ) {  /* a*V < U if lv<lu-1, and very likely if lv==lu-1 */
#if BSH == BIT_WIDTH_OF_INT
      if ( w < t ) c++;
      t = w - t;
#else
      t = w - t;	    /* We shall compute U - a*V */
      if ( t < 0 ) t += BASE, c++;
#endif
    } else { /* a*V > U if lv>lu, and very likely even if lv==lu */
      b = 0;
      if ( t >= w ) t -= w;
      else {
	t -= w;
#if BSH != BIT_WIDTH_OF_INT
	t += BASE;
#endif
	if ( c != 0 ) c--;
	else b = 1;
      }
    }
    TRAILINGZEROS( t, rsh );
    return( lv < lu ? ( rsh == 0 ? abs_U_aV_c( t, u, lu, a, v, lv, c, ans ) :
		        abs_U_aV_c_sh( t, u, lu, a, v, lv, c, rsh, ans ) ) :
	    rsh == 0 ? abs_aVplusc_U( t, a, v, lv, c, b, u, lu, ans ) :
	    abs_aVplusc_U_sh( t, a, v, lv, c, b, u, lu, rsh, ans ) );
  }
  /**/
  if ( i < lv ) {  /* no more u[] elm.'s, and we still have non-zero v[] elm's */
    do {
      DMA( a, v[i], c, hi, t );
      i++, c = hi;
    } while ( t == 0 );
    TRAILINGZEROS( t, rsh );
    v += i, lv -= i;
#ifdef CALL
    if ( rsh == 0 ) {
      *p++ = t;
      return( aV_plus_c( a, v, lv, c, p ) + 1 );
    } else return aV_plus_c_sh( a, v, lv, c, rsh, t, ans );
#else
    i = 0;
    if ( rsh == 0 ) {
      *p++ = t;
      for ( ; i < lv; i++, c = hi, *p++ = t ) {	DMA( a, v[i], c, hi, t ); }
      if ( c == 0 ) return( i + 1 );
      *p = c;
      return( i + 2 );
    } else {
      for ( lsh = BSH - rsh; i < lv; i++, c = hi, t = w >> rsh ) {
	DMA( a, v[i], c, hi, w );
#if BSH == BIT_WIDTH_OF_INT
	*p++ = t | (w << lsh);
#else
	*p++ = t | ((w << lsh) & BMASK);
#endif
      }
      if ( c != 0 ) {
#if BSH == BIT_WIDTH_OF_INT
	*p++ = t | (c << lsh);
#else
	*p++ = t | ((c << lsh) & BMASK);
#endif
	i++;
	t = c >> rsh;
      }
      if ( t == 0 ) return i;
      *p = t;
      return( i + 1 );
    }
#endif
  }
  /* no more v[] elm.'s */
  if ( i >= lu ) {
    if ( c == 0 ) return 0;
  c_sh:
    while ( (c&1) == 0 ) c >>= 1;
    *p = c;
    return 1;
  }
  t = u[i++];
  if ( i >= lu ) {
    if ( t == c ) return 0;
    c = t < c ? c - t : t - c;
    goto c_sh;
  } else if ( t == c ) {
    while ( (t = u[i++]) == 0 ) ;
    c = 0;
  } else if ( t < c ) {
#if BSH == BIT_WIDTH_OF_INT
    t = t - c;
#else
    t = t + (BASE - c);
#endif
    c = 1;
  } else { t -= c; c = 0; }
  /* diff turned out to be > 0 */
  u += i, lu -= i;
  TRAILINGZEROS( t, rsh );
  i = 0;
  if ( rsh == 0 ) {
    *p++ = t;
    if ( c != 0 ) {
      for ( ; i < lu; *p++ = BMASK ) if ( (t = u[i++]) != 0 ) break;
      t--;
      if ( i >= lu && t == 0 ) return i;
      *p++ = t;
    }
    for ( ; i < lu; i++ ) *p++ = u[i];
  } else { /* rsh != 0 */
    lsh = BSH - rsh;
    if ( c != 0 ) {  /* there still exists u[] elms.'s because diff is known to be > 0 */
      if ( (w = u[i++]) == 0 ) {
#if BSH == BIT_WIDTH_OF_INT
	*p++ = t | (BMask << lsh);
#else
	*p++ = t | ((BMASK << lsh) & BMASK);
#endif
	for ( ; (w = u[i++]) == 0; *p++ = BMASK ) ;
	t = BMASK >> rsh;
      }
      w--;
#if BSH == BIT_WIDTH_OF_INT
      *p++ = t | (w << lsh);
#else
      *p++ = t | ((w << lsh) & BMASK);
#endif
      t = w >> rsh;
    }
#if BSH == BIT_WIDTH_OF_INT
    for ( ; i < lu; i++, t = w >> rsh ) *p++ = t | ((w = u[i]) << lsh);
#else
    for ( ; i < lu; i++, t = w >> rsh ) *p++ = t | (((w = u[i]) << lsh) & BMASK);
#endif
    if ( t == 0 ) return( i );
    *p = t;
  }
  return( i + 1 );
}

/*********/

static int aV_plus_c( a, v, lv, c, p )
int a, *v, lv, c, *p;
    /*
     *  Compute (a*V + c) in p[], where V = v[0:lv-1].
     */
{
  int i, t;
  int hi;

  for ( i = 0; i < lv; i++, *p++ = t, c = hi ) { DMA( a, v[i], c, hi, t ); }
  if ( c == 0 ) return i;
  *p = c;
  return( i + 1 );
}

static int aV_plus_c_sh( a, v, lv, c, rsh, t, p )
int a, *v, lv, c, rsh, *p;
    /*
     *  Compute (t + BASE*((a*V + c) >> rsh)) in p[], where V = v[0:lv-1].
     */
{
  int i, w, lsh = BSH - rsh;
  int hi;

  for ( i = 0; i < lv; i++, c = hi, t = w >> rsh ) {
    DMA( a, v[i], c, hi, w );
#if BSH == BIT_WIDTH_OF_INT
    *p++ = t | (w << lsh);
#else
    *p++ = t | ((w << lsh) & BMASK);
#endif
  }
  if ( c != 0 ) {
#if BSH == BIT_WIDTH_OF_INT
    *p++ = t | (c << lsh);
#else
    *p++ = t | ((c << lsh) & BMASK);
#endif
    i++;
    t = c >> rsh;
  }
  if ( t == 0 ) return i;
  *p = t;
  return( i + 1 );
}

/*********/

static int abs_aVplusc_U( t, a, v, lv, c, b, u, lu, ans )
#if BSH == BIT_WIDTH_OF_INT
unsigned
#endif
int t, a, *v, lv, c, b, *u, lu, *ans;
    /*  compute | t + BASE*(a*V+c - U) | in ans[], where V=v[0:lv-1], U=u[0:lu-1],
     *  and a > 1, -1 <= c < BASE, lv >= lu >=0 && t != 0 are assumed.
     */
{
#if BSH == BIT_WIDTH_OF_INT
  unsigned
#endif
  int i, l, *p = ans, hi, w;

  *p++ = t, l = 1;
  for ( i = 0; i < lu; *p++ = t ) {
    DMA( a, v[i], c, hi, t );
    c = hi;
    if ( t > (w = u[i++]) ) {
      t = (t - w) - b;
      b = 0;
    } else if ( t < w ) {
      t = (t - w) - b;
#if BSH != BIT_WIDTH_OF_INT
      t += BASE;
#endif
      if ( c != 0 ) c--, b = 0;
      else b = 1;
    } else if ( b == 0 ) t = 0;
    else {
      t = BMASK;
      if ( c != 0 ) c--, b = 0;
    }
    if ( t != 0 ) l = i + 1;
  }
  /* at this point, i == lu and we have written (i+1) entries in ans[] */
  if ( i >= lv ) {  /* no more elms in v[] */
    if ( b != 0 ) {
#if BSH == BIT_WIDTH_OF_INT
      if ( i == 0 ) { ans[i] = - t; return 1; }
#else
      if ( i == 0 ) { ans[i] = BASE - t; return 1; }
#endif
      l = negate_nbd( ans, i );		/* <================ */
/*    if ( (t = BMASK ^ ans[i]) == 0 ) return l;*/
      if ( (t ^= BMASK) == 0 ) return l;
      ans[i] = t;
      return( i + 1 );
    }
    if ( c == 0 ) return l;
    *p = c;
    return( i + 2 );
  }
  /* diff turned out to be > 0 */
  if ( b != 0 ) {  /* c == 0 */
    while ( (b = v[i++]) == 0 ) *p++ = BMASK;
    DM( a, b, hi, t );
    if ( t != 0 ) t--, c = hi;
    else t = BMASK, c = hi - 1;
    *p++ = t;
  }
#ifdef CALL
  return( aV_plus_c( a, &v[i], lv - i, c, p ) + i +1 );
#else
  for ( ; i < lv; i++, *p++ = t, c = hi ) { DMA( a, v[i], c, hi, t ); }
  if ( c == 0 ) return( i + 1 );
  *p = c;
  return( i + 2 );
#endif
}

static int abs_aVplusc_U_sh( t, a, v, lv, c, b, u, lu, rsh, ans )
#if BSH == BIT_WIDTH_OF_INT
unsigned
#endif
int t, a, *v, lv, c, b, *u, lu, rsh, *ans;
{
#if BSH == BIT_WIDTH_OF_INT
  unsigned
#endif
  int i, l, w, lsh = BSH - rsh, *p = ans, hi, x;

  for ( i = 0; i < lu; t = w >> rsh ) {
    DMA( a, v[i], c, hi, w );
    c = hi;
    if ( w > (x = u[i++]) ) {
      w = (w - x) - b;
      b = 0;
    } else if ( w < x ) {
      w = (w - x) - b;
#if BSH != BIT_WIDTH_OF_INT
      w += BASE;
#endif
      if ( c != 0 ) c--, b = 0;
      else b = 1;
    } else if ( b == 0 ) w = 0;
    else {
      w = BMASK;
      if ( c != 0 ) c--, b = 0;
    }
#if BSH == BIT_WIDTH_OF_INT
    if ( (*p++ = t | (w << lsh)) != 0 ) l = i;
#else
    if ( (*p++ = t | ((w << lsh) & BMASK)) != 0 ) l = i;
#endif
  }
  /* at this point, i == lu and we have written i entries in ans[] */
  if ( i >= lv ) {
    if ( b != 0 ) {  /* diff turned out to be < 0 */
      if ( i == 0 ) {
#if BSH == BIT_WIDTH_OF_INT
	*p = (1 << lsh) - t;
#else
	*p = (BASE >> rsh) - t;
#endif
	return 1;
      }
      l = negate_nbd( ans, i );		/* <================ */
      if ( (t ^= (BMASK >> rsh)) == 0 ) return l;
      *p = t;
      return( i + 1 );
    }
    if ( c == 0 ) {
      if ( t == 0 ) return l;
      *p = t;
      return( i + 1 );
    }
#if BSH == BIT_WIDTH_OF_INT
    *p++ = t | (c << lsh);
#else
    *p++ = t | ((c << lsh) & BMASK);
#endif
    if ( (t = c >> rsh) == 0 ) return( i + 1 );
    *p = t;
    return( i + 2 );
  }
  /* lv >= lu+1 = i+1, and diff is > 0 */
  if ( b != 0 ) {  /* c == 0 */
    if ( (w = v[i++]) == 0 ) {
#if BSH == BIT_WIDTH_OF_INT
      *p++ = t | (BMASK << lsh);
#else
      *p++ = t | ((BMASK << lsh) & BMASK);
#endif
      while ( (w = v[i++]) == 0 ) *p++ = BMASK;
      t = BMASK >> rsh;
    } else {
      DM( a, w, c, b );
      if ( b != 0 ) b--;
      else b = BMASK, c--;
#if BSH == BIT_WIDTH_OF_INT
      *p++ = t | (b << lsh);
#else
      *p++ = t | ((b << lsh) & BMASK);
#endif
      t = b >> rsh;
    }
  }
  /**/
#ifdef CALL
  return( aV_plus_c_sh( a, &v[i], lv - i, c, rsh, t, p ) + i );
#else
  for ( ; i < lv; i++, t = w >> rsh, c = hi ) {
    DMA( a, v[i], c, hi, w );
#if BSH == BIT_WIDTH_OF_INT
    *p++ = t | (w << lsh);
#else
    *p++ = t | ((w << lsh) & BMASK);
#endif
  }
  if ( c != 0 ) {
#if BSH == BIT_WIDTH_OF_INT
    *p++ = t | (c << lsh);
#else
    *p++ = t | ((c << lsh) & BMASK);
#endif
    i++,  t = c >> rsh;
  }
  if ( t == 0 ) return i;
  *p = t;
  return( i + 1 );
#endif
}

/*********/

static int abs_U_aV_c( t, u, lu, a, v, lv, c, ans )
#if BSH == BIT_WIDTH_OF_INT
unsigned
#endif
int t, *u, lu, a, *v, lv, c, *ans;
    /*  compute | t + BASE*(U - a*V - c) | in ans[], where U = u[0:lu-1], V = v[0:lv-1],
     *	c is <= BASE-1, and lu >= lv+1 && t != 0 assumed.
     */
{
#if BSH == BIT_WIDTH_OF_INT
  unsigned
#endif
  int i, w, l, *p = ans, hi, b;

  *p++ = t, l = 1, b = 0;
  for ( i = 0; i < lv; *p++ = t ) {
    DMA( a, v[i], c, hi, w );
    c = hi;
    if ( (t = u[i++]) > w ) {
      t = (t - w) - b;
      b = 0;
    } else if ( t == w && b == 0 ) {
      t = 0;
      continue;
    } else {
      t = (t - w) - b;
#if BSH != BIT_WIDTH_OF_INT
      t += BASE;
#endif
      if ( c == BMASK ) b = 1;
      else b = 0, c++;
    }
    if ( t != 0 ) l = i + 1;
  }
  /* at this point, i == lv */
  if ( lu == lv+1 ) {
    t = u[i] - b;
    if ( t == c ) return l;
    else if ( t > c ) { *p = t - c; return( lu + 1 ); }
    l = negate_nbd( ans, lu );		/* <================ */
    if ( (c -= (t + 1)) == 0 ) return l;
    *p = c;
    return( lu + 1 );
  }
  /*  lu >= lv+2 and therfore, the diff turned out to be >= 0 */
  if ( c != 0 ) {
    /* note c+b <= BASE.  If c == BASE, we must care about the length of the result
       of the case when lu == lv+2 && u[lu-2:lu-1]=u[i:i+1] == BASE  */
    if ( /* c == BMASK && */ b != 0 ) {
      if ( i+2 == lu && u[i+1] == 1 ) {
	/* m.s. word of the diff becomes 0 */
	if ( (t = u[i]) == 0 ) return l;
	*p = t;
	return lu;
      }
      *p++ = u[i++];
      c = 1;
    }
#if BSH == BIT_WIDTH_OF_INT
    do {
      if ( (t = u[i++]) >= c ) break;
      else *p++ = t - c;
    } while ( c = 1 );
    if ( (t -= c) == 0 && i >= lu ) return lu;
#else
    for ( ; 1; c = 1, *p++ = t + BASE ) if ( (t = u[i++] - c) >= 0 ) break;
    if ( t == 0 && i >= lu ) return lu;
#endif
    *p++ = t;
  }
  for ( ; i < lu; i++ ) *p++ = u[i];
  return( lu + 1 );
}

static int abs_U_aV_c_sh( t, u, lu, a, v, lv, c, rsh, ans )
#if BSH == BIT_WIDTH_OF_INT
unsigned
#endif
int t, *u, lu, a, *v, lv, c, rsh, *ans;
    /*  r-shift version of the above  */
{
#if BSH == BIT_WIDTH_OF_INT
  unsigned
#endif
  int i, w, l, lsh = BSH - rsh, *p = ans, hi, b, x;

  for ( b = l = i = 0; i < lv; t = w >> rsh ) {
    DMA( a, v[i], c, hi, w );
    c = hi;
    if ( (x = u[i++]) > w ) {
      w = (x - w) - b;
      b = 0;
    } else if ( x == w && b == 0 ) w = 0;
    else {
      w = (x - w) - b;
#if BSH != BIT_WIDTH_OF_INT
      w += BASE;
#endif
      if ( c == BMASK ) b = 1;
      else b = 0, c++;
    }
#if BSH == BIT_WIDTH_OF_INT
    x = t | (w << lsh);
#else
    x = t | ((w << lsh) & BMASK);
#endif
    if ( (*p++ = x) != 0 ) l = i;
  }
  /* at this point, i == lv, and we have written lv elm.s in ans[] */
  if ( lu == lv+1 ) {
    x = u[i] - b;
    if ( x == c ) {
      if ( t == 0 ) return l;
      *p = t;
      return lu; /* == lv+1 */
    } else if ( x > c ) {
      w = x - c;
    l0:
#if BSH == BIT_WIDTH_OF_INT
      *p++ = t | (w << lsh);
#else
      *p++ = t | ((w << lsh) & BMASK);
#endif
      if ( (w >>= rsh) == 0 ) return lu; /* == lv+1 */
      *p = w;
      return( lu + 1 );
    }
    /* diff turned out to be < 0 */
    w = c - x - 1;
    if ( lv == 0 ) {	/* lu == 1 */
#if BSH == BIT_WIDTH_OF_INT
      t = (1 << lsh) - t;
#else
      t = (BASE >> rsh) - t;
#endif
      if ( w != 0 ) goto l0;
      *p = t;
      return 1;
    }
    l = negate_nbd( ans, lv );		/* <================ */
    t ^= (BMASK >> rsh);
    if ( w != 0 ) goto l0;
    if ( t == 0 ) return l;
    *p = t;
    return lu; /* == lv + 1 */
  }
  /* now, lu >= lv+2 == i+2 */
  if ( c != 0 ) {
    /* note c <= BASE.  If c == BASE, we must care about the length of the result
       of the case when lu == lv+2 && u[lu-2:lu-1]=u[i:i+1] == BASE  */
    if ( /* c == BMASK && */ b != 0 ) {
      if ( i+2 == lu && u[i+1] == 1 && t == 0 && u[i] == 0 )
	return l; /* m.s. word of the diff has become 0 */
      w = u[i++];
#if BSH == BIT_WIDTH_OF_INT
      *p++ = t | (w << lsh);
#else
      *p++ = t | ((w << lsh) & BMASK);
#endif
      c = 1, t = w >> rsh;
    }
    for ( ; 1; t = w >> rsh, c = 1 ) {
      w = (x = u[i++]) - c;
      if ( x >= c ) break;
#if BSH == BIT_WIDTH_OF_INT
      *p++ = t | (w << lsh);
#else
      w += BASE;
      *p++ = t | ((w << lsh) & BMASK);
#endif
    }
#if BSH == BIT_WIDTH_OF_INT
    t |= (w << lsh);
#else
    t |= ((w << lsh) & BMASK);
#endif
    w >>= rsh;
    if ( i >= lu ) {
      if ( w != 0 ) {
	*p++ = t;
	*p = w;
	return( lu + 1 );
      } else if ( t == 0 ) return( i - 1 );
      else { *p = t; return i; }
    }
    *p++ = t;
    t = w;
  }
#if BSH == BIT_WIDTH_OF_INT
  for ( ; i < lu; i++, t = w >> rsh ) *p++ = t | ((w = u[i]) << lsh);
#else
  for ( ; i < lu; i++, t = w >> rsh ) *p++ = t | (((w = u[i]) << lsh) & BMASK);
#endif
  if ( t == 0 ) return i;
  *p = t;
  return( i + 1 );
}

/****************//****************//****************//****************/
/****************//****************//****************//****************/
static int abs_axU_bxV_maxrshift( a, u, lu, b, v, lv, ans )
#if BSH == BIT_WIDTH_OF_INT
unsigned
#endif
int a, *u, lu, b, *v, lv, *ans;
    /*
     *  Compute | a*U - b*V | >> (as much as possible) in ans[], where U=u[0:lu-1]
     *  and V=v[0:lv-1], 0 < a, b < 2^BASE.
     */
{
#if BSH == BIT_WIDTH_OF_INT
  unsigned
#endif
  int i, l, c, d, s, t, w, rsh, *p = ans, lsh, hi;
  extern int bw_int32();

  BitWidth( a, c );  BitWidth( u[lu -1], s );
  BitWidth( b, d );  BitWidth( v[lv -1], t );
  if ( lu < lv || lu == lv && (c + s) < (d + t) ) {
    SWAP( a, b, int );  SWAP( lu, lv, int );  SWAP( u, v, int * );
  }
  for ( i = c = d = 0; i < lv; ) {
    DMA( a, u[i], c, hi, t );  /* 0 <= c <= 2^BSH -2 */  c = hi;
    DMA( b, v[i], d, hi, s );  /* 0 <= d <= 2^BSH -1 */  d = hi;
    i++;
    if ( t == s ) continue;
    else if ( t > s ) t -= s;
    else {
      t -= s;
#if BSH != BIT_WIDTH_OF_INT
      t += BASE;
#endif
      if ( c != 0 ) c--;
      else d++;
    }
    goto non0w;
  }
  /******/
  if ( i >= lu ) {  /* no more elm.'s in u[] and v[] */
  only_c:
    if ( c == d ) return 0;
    c = c > d ? c - d : d - c;
  sh_onlyc:
    /* TRAILINGZEROS( c, rsh ); */
    while ( (c&1) == 0 ) c >>= 1;
    *p = c;
    return 1;
  }
  /* there is at least one more elm. in u[] */
  DMA( a, u[i], c, hi, t );
  i++, c = hi;
  if ( i >= lu ) {  /* in this case, diff still can be <= 0 */
    if ( hi == 0 ) { c = t; goto only_c; }
    if ( t == d ) { c = hi;  goto sh_onlyc; }
    else if ( t > d ) t -= d;
    else {
      t -= d, hi--;
#if BSH != BIT_WIDTH_OF_INT
      t += BASE;
#endif
    }
    TRAILINGZEROS( t, rsh );
    if ( rsh == 0 ) *p++ = t;
    else {
#if BSH == BIT_WIDTH_OF_INT
      *p++ = t | (hi << (BSH - rsh));
#else
      *p++ = t | ((hi << (BSH - rsh)) & BMASK);
#endif
      hi >>= rsh;
    }
    if ( hi == 0 ) return 1;
    *p = hi;
    return 2;
  } /* diff turned out to be > 0 */ else if ( t > d ) {
    t -= d;
    d = 0;
  } else if ( t < d ) {
    t -= d;
#if BSH != BIT_WIDTH_OF_INT
    t += BASE;
#endif
    d = 1;
  } else {
    while ( i < lu ) {
      DMA( a, u[i], c, hi, t );
      i++, c = hi;
      if ( t != 0 ) break;
    }
  }
  u += i, lu -= i;
  TRAILINGZEROS( t, rsh );
  l = i = 0;
  if ( rsh == 0 ) {
    *p++ = t;
    goto no_more_v;
  } else goto no_more_v_sh;
  /******/
 non0w:
  u += i, lu -= i,  v += i, lv -= i;
  TRAILINGZEROS( t, rsh );
  i = 0;
  if ( rsh == 0 ) {
    *p++ = t, l = 0;
    for ( ; i < lv; *p++ = t ) {
      DMA( a, u[i], c, hi, t );  c = hi;
      DMA( b, v[i], d, hi, s );  d = hi;
      i++;
      if ( t == s ) continue;
      else if ( t > s ) t -= s;
      else {
	t -= s;
#if BSH != BIT_WIDTH_OF_INT
	t += BASE;
#endif
	if ( c != 0 ) c--;
	else d++;
      }
      l = i;
    }
  no_more_v:
    if ( i >= lu ) {
    final_c_d:
      if ( c == d ) return( l + 1 );
      else if ( c > d ) c -= d;
      else {
	l = negate_nbd( ans, i + 1 );		/* <================ */
	if ( (c = d-c - 1) == 0 ) return l;
      }
      *p = c;
      return( i + 2 );
    }
    /* a*u[i:lu-1] + c - d */
    if ( c >= d ) c -= d;
    else {
      DMA( a, u[i], c, hi, t );
      if ( i+1 >= lu ) {
	if ( hi == 0 ) { c = t; goto final_c_d; }
	c = hi;
	if ( t >= d ) t -= d;
	else {
	  t -= d;
#if BSH != BIT_WIDTH_OF_INT
	  t += BASE;
#endif
	  c--;
	}
	i++, *p++ = t;
	goto final_c;
      }
      i++, c = hi;
      if ( t >= d ) *p++ = t - d;
      else {
	t -= d;
#if BSH != BIT_WIDTH_OF_INT
	t += BASE;
#endif
	*p++ = t;
	if ( c != 0 ) c--;
	else {
	  for ( ; i < lu; *p++ = BMASK ) {
	    DMA( a, u[i], c, hi, t );
	    i++, c = hi;
	    if ( t == 0 ) continue;
	    *p++ = t - 1;
	    goto aU;
	  }
	  c--;
	  goto final_c;
	}
      }
    }
    /*** return( aV_plus_c( a, &u[i], lu - i, c, p ) + i + 1 ); ***/
  aU:
    for ( ; i < lu; i++, c = hi, *p++ = t ) { DMA( a, u[i], c, hi, t ); }
  final_c:
    if ( c == 0 ) return( i + 1 );
    *p = c;
    return( i + 2 );
  } else {  /*  rsh != 0 */
    for ( lsh = BSH - rsh; i < lv; t = w >> rsh ) {
      DMA( a, u[i], c, hi, w );  c = hi;
      DMA( b, v[i], d, hi, s );  d = hi;
      i++;
      if ( w >= s ) w -= s;
      else {
	w -= s;
#if BSH != BIT_WIDTH_OF_INT
	w += BASE;
#endif
	if ( c != 0 ) c--;
	else d++;
      }
#if BSH == BIT_WIDTH_OF_INT
      if ( (*p++ = t | (w << lsh)) != 0 ) l = i;
#else
      if ( (*p++ = t | ((w << lsh) & BMASK)) != 0 ) l = i;
#endif
    }
  no_more_v_sh:
    if ( i >= lu ) {
    final_c_d_sh:
      if ( c == d )
	if ( t == 0 ) return l;
	else { *p = t; return( i + 1 ); }
      else if ( c > d ) c -= d;
      else {
#if BSH == BIT_WIDTH_OF_INT
	if ( i == 0 ) t = (1 << lsh) - t;
#else
	if ( i == 0 ) t = (BASE >> rsh) - t;
#endif
	else {
	  l = negate_nbd( ans, i );		/* <================ */
/*	  t = (BASE >> rsh) - t;*/
	  t ^= ((BMASK >> rsh));
	}
	c = d-c -1;
      }
#if BSH == BIT_WIDTH_OF_INT
      *p++ = t | (c << lsh);
#else
      *p++ = t | ((c << lsh) & BMASK);
#endif
      if ( (c >>= rsh) == 0 ) return( i + 1 );
      *p = c;
      return( i + 2 );
    } else if ( c >= d ) c -= d;
    else {
      DMA( a, u[i], c, hi, w );
      if ( i+1 >= lu ) {
	if ( hi == 0 ) { c = w; goto final_c_d_sh; }
	c = hi;
	if ( w >= d ) w -= d;
	else {
	  w -= d;
#if BSH != BIT_WIDTH_OF_INT
	  w += BASE;
#endif
	  c--;
	}
#if BSH == BIT_WIDTH_OF_INT
	i++, *p++ = t | (w << lsh);
#else
	i++, *p++ = t | ((w << lsh) & BMASK);
#endif
	t = w >> rsh;
	goto final_c_sh;
      }
      i++, c = hi;
      if ( w >= d ) {
	w -= d;
#if BSH == BIT_WIDTH_OF_INT
	*p++ = t | (w << lsh);
#else
	*p++ = t | ((w << lsh) & BMASK);
#endif
	t = w >> rsh;
      } else {
	w -= d;
#if BSH == BIT_WIDTH_OF_INT
	*p++ = t | (w << lsh);
#else
	w += BASE;
	*p++ = t | ((w << lsh) & BMASK);
#endif
	t = w >> rsh;
	if ( c != 0 ) c--;
	else {
	  while ( i < lu ) {
	    DMA( a, u[i], c, hi, w );
	    c = hi;
	    i++;
	    if ( w == 0 ) {
#if BSH == BIT_WIDTH_OF_INT
	      *p++ = t | (BMASK << lsh);
	      t = BMASK >> rsh;
#else
	      *p++ = t | ((BMASK << lsh) & BMASK);
	      t = BMASK >> rsh;
#endif
	      continue;
	    } else {
	      w--;
#if BSH == BIT_WIDTH_OF_INT
	      *p++ = t | (w << lsh);
#else
	      *p++ = t | ((w << lsh) & BMASK);
#endif
	      t = w >> rsh;
	      goto aU_sh;
	    }
	  }
	  c--;
	  goto final_c_sh;
	}
      }
    }
    /*** return( aV_plus_c_sh( a, &u[i], lu - i, c, rsh, t ) + i ); ***/
  aU_sh:
    for ( ; i < lu; i++, c = hi, t = w >> rsh ) {
      DMA( a, u[i], c, hi, w );
#if BSH == BIT_WIDTH_OF_INT
      *p++ = t | (w << lsh);
#else
      *p++ = t | ((w << lsh) & BMASK);
#endif
    }
  final_c_sh:
    if ( c != 0 ) {
#if BSH == BIT_WIDTH_OF_INT
      *p++ = t | (c << lsh);
#else
      *p++ = t | ((c << lsh) & BMASK);
#endif
      i++, t = c >> rsh;
    }
    if ( t == 0 ) return i;
    *p = t;
    return( i + 1 );
  }
}
/****************/
/****************/

static int aUplusbV_maxrshift( a, u, lu, b, v, lv, ans )
#if BSH == BIT_WIDTH_OF_INT
unsigned
#endif
int a, *u, lu, b, *v, lv, *ans;
    /*
     *  Compute ( (a*U + b*V) >> (as much as possible) ) in ans[], where U=u[0:lu-1],
     *  V=v[0:lv-1], and 0 < a,b < BASE.
     */
{
#if BSH == BIT_WIDTH_OF_INT
  unsigned
#endif
  int i, c, d, s, t, w, rsh, *p = ans, lsh, hi;

  if ( lu < lv ) { SWAP( a, b, int ); SWAP( lu, lv, int ); SWAP( u, v, int * ); }
  for ( c = d = i = 0; i < lv; ) {
    DMA( a, u[i], c, hi, t );  /* 0 <= c <= 2^BSH -1 */  c = hi;
    DMA( b, v[i], d, hi, s );  /* 0 <= d <= 2^BSH -2 */  d = hi;
    i++;
#if BSH == BIT_WIDTH_OF_INT
    t += s;
    if ( t < s ) c++;
#else
    t += s;
    if ( t > BMASK ) c++, t &= BMASK;
#endif
    if ( t != 0 ) goto non0w;
  }
#if BSH == BIT_WIDTH_OF_INT
  c += d;
  if ( c < d ) {  /* overflow, and c can never be >= 2^BSH - 1 */
    if ( i >= lu ) { d = 1;  goto only_cd; }
    DMA( a, u[i], c, hi, t );
    c = hi + 1;
    i++;
    if ( t != 0 ) goto non0t_u;
  }
  for ( d = 0; i < lu; ) {
    DMA( a, u[i], c, hi, t );
    c = hi;
    i++;
    if ( t != 0 ) goto non0t_u;
  }
 only_cd:
  TRAILINGZEROS( c, rsh );
  if ( d != 0 )
    if ( rsh == 0 ) { *p++ = c; *p = 1; return 2; }
    else c |= (1 << (BSH - rsh));
  *p = c;
  return 1;
  /**/
non0t_u:
  TRAILINGZEROS( t, rsh );
  u += i, lu -= i;
  i = 0;
  if ( rsh == 0 ) { *p++ = t;  goto u_rest; }
  lsh = BSH - rsh;
  goto u_rest_sh;
#else
  c += d;
  for ( d = 0; i < lu; ) {
    DMA( a, u[i], c, hi, t );
    c = hi;
    i++;
    if ( t != 0 ) goto non0w;
  }
  TRAILINGZEROS( c, rsh );
  if ( rsh != 0 || c <= BMASK ) { *p = c; return 1; }
  *p++ = c & BMASK;
  *p = 1;
  return 2;
#endif
  /**/
 non0w:
  u += i, lu -= i,  v += i, lv -= i;
  TRAILINGZEROS( t, rsh );
  i = 0;
  if ( rsh == 0 ) {
    *p++ = t;
    for ( ; i < lv; i++, *p++ = t ) {
      DMA( a, u[i], c, hi, t );  c = hi;
      DMA( b, v[i], d, hi, s );  d = hi;
      t += s;
#if BSH == BIT_WIDTH_OF_INT
      if ( t < s ) c++;
#else
      if ( t > BMASK ) c++, t &= BMASK;
#endif
    }
    c += d;
#if BSH == BIT_WIDTH_OF_INT
    if ( c < d ) {
      if ( i >= lu ) { *p++ = c; *p = 1; return( lu + 3 ); }
      DMA( a, u[i], c, hi, t );
      c = hi + 1;
      i++;
      *p++ = t;
    }
u_rest:
    for ( ; i < lu; i++, *p++ = t, c = hi ) {
      DMA( a, u[i], c, hi, t );
    }
    if ( c == 0 ) return( lu + 1 );
    *p = c;
    return( lu + 2 );
#else
u_rest:
    for ( ; i < lu; i++, *p++ = t, c = hi ) {
      DMA( a, u[i], c, hi, t );
    }
    if ( c == 0 ) return( lu + 1 );
    else if ( c <= BMASK ) { *p = c; return( lu + 2 ); }
    *p++ = c & BMASK;
    *p = 1;
    return( lu + 3 );
#endif
  } else {
    for ( lsh = BSH - rsh; i < lv; i++, t = w >> rsh ) {
      DMA( a, u[i], c, hi, w );  c = hi;
      DMA( b, v[i], d, hi, s );  d = hi;
      w += s;
#if BSH == BIT_WIDTH_OF_INT
      if ( w < s ) c++;
      *p++ = t | (w << lsh);
#else
      if ( w > BMASK ) c++, w &= BMASK;
      *p++ = t | ((w << lsh) & BMASK);
#endif
    }
    c += d;  /* <= 2^BSH + (2^BSH - 3) */
#if BSH == BIT_WIDTH_OF_INT
    if ( c < d ) {
      if ( i >= lu ) {
	*p++ = t | (c << lsh);
	*p = (c >> rsh) | (1 << lsh);
	return( lu + 2 );
      }
      DMA( a, u[i], c, hi, w );
      c = hi + 1;
      i++;
      *p++ = t | (w << lsh);
      t = w >> rsh;
    }
  u_rest_sh:
    for ( ; i < lu; i++, t = w >> rsh ) {
      DMA( a, u[i], c, hi, w );
      c = hi;
      *p++ = t | (w << lsh);
    }
#else
u_rest_sh:
    for ( ; i < lu; i++, c = hi, t = w >> rsh ) {
      DMA( a, u[i], c, hi, w );
      *p++ = t | ((w << lsh) & BMASK);
    }
#endif
    if ( c == 0 ) {
      if ( t == 0 ) return lu;
      *p = t;
      return( lu + 1 );
    }
#if BSH == BIT_WIDTH_OF_INT
    *p++ = t | (c << lsh);
#else
    *p++ = t | ((c << lsh) & BMASK);
#endif
    if ( (c >>= rsh) == 0 ) return( lu + 1 );
    *p = c;
    return( lu + 2 );
  }
}
