/* $OpenXM: OpenXM/src/asir99/engine/igcdhack.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#if 0
#include "base.h"

#define SWAP(a,b,Type) { Type temp=a; a = b; b = temp; }
#define SIGNED_VAL(a,s) ((s)>0?(a): -(a))
#endif

#undef FULLSET


static int negate_nbd( b, l )
int *b, l;
    /*  It is assumed that l > 0 and b[0] != 0 */
{
  int i, nz;

  *b = BASE - *b;
  for ( nz = i = 1 ; i < l; ) {
    b++, i++;
    if ( (*b ^= BMASK) != 0 ) nz = i;
  }
  return nz;
}

/****************/
/****************/

static int abs_U_V_maxrshift( u, lu, v, lv, a )
int *u, lu, *v, lv, *a;
    /*
     *  Compute | U - V | >> (as much as possible) in a[], where U = u[0:lu-1]
     *  and V = v[0:lv-1].  Value returned is the length of the result
     *  multiplied by the sign of (U-V).
     */
{
  int sign, rsh, t, b, i, i_1, l, *p = a;

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
  if ( (t = u[0] - v[0]) >= 0 ) b = 0;
  else b = 1,  t += BASE;
  TRAILINGZEROS( t, rsh );
  /**/
  if ( rsh != 0 ) {
    int w, lsh = BSH - rsh;

    /* subtract v[*] from u[*] */
    for ( i = 1, l = 0; i < lv; i++, t = w >> rsh ) {
      if ( (w = u[i] - v[i] - b) >= 0 ) b = 0;
      else b = 1,  w += BASE;
      if ( (*p++ = t | ((w << lsh) & BMASK)) != 0 ) l = i;
    }
    /* subtraction of V completed. Note there may exist non-0 borrow b */
    if ( i >= lu ) /* lu == lv, and b==0 is implied */ goto chk_nz;
    /* we still have (lu-i) elms in U */
    if ( b != 0 ) {	/* non-zero borrow */
      if ( (w = u[i++] -1) != 0 ) t |= ((w << lsh) & BMASK);
      if ( i >= lu ) {	/* lu == lv+1.  The M.S. word w may beome 0 */
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
    l = i -1;
  chk_nz:
    if ( t != 0 ) *p = t, l = /*1 + (p - a)*/ lu;
    goto ret_nz;
  }
  /* rsh == 0 : need not be right-shifted */
  *p++ = t, l = 1;
  for ( i = 1; i < lv; ) {
    if ( (t = u[i] - v[i] - b) >= 0 ) b = 0;
    else b = 1, t += BASE;
    i++;
    if ( (*p++ = t) != 0 ) l = i;
  }
  if ( i >= lu ) /* lu == lv, and b==0 is implied */ goto ret_nz;
  if ( b != 0 ) {	/* non-zero borrow */
    t = u[i++] -1;
    if ( i >= lu ) /* lu == lv+1.  The M.S. word w may beome 0 */ goto chk_nz;
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
int *u, lu, a, *v, lv, *ans;
{
  int i, t, c, rsh, l, *p = ans, w, lsh;
  int hi;

  if ( a == 1 ) return( (l = abs_U_V_maxrshift( u, lu, v, lv, ans )) >= 0 ? l : -l );
  /**/
  for ( l = lu <= lv ? lu : lv, i = c = 0; i < l; ) {
    DMA( a, v[i], c, hi, t, not_used );
    c = hi;
    if ( (t -= u[i++]) != 0 ) goto non0w;
  }
  /**/
  if ( i < lv ) {  /* no more u[] elm.'s, and we still have non-zero v[] elm's */
    do {
      DMA( a, v[i], c, hi, t, not_used );
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
      for ( ; i < lv; i++, c = hi, *p++ = t ) { DMA( a, v[i], c, hi, t, not_used ); }
      if ( c == 0 ) return( i + 1 );
      *p = c;
      return( i + 2 );
    } else {
      for ( lsh = BSH - rsh; i < lv; i++, c = hi, t = w >> rsh ) {
	DMA( a, v[i], c, hi, w, not_used );
	*p++ = t | ((w << lsh) & BMASK);
      }
      if ( c != 0 ) {
	*p++ = t | ((c << lsh) & BMASK);
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
  t = u[i++] - c;
  if ( i >= lu ) {
    if ( t == 0 ) return 0;
    c = t < 0 ? -t : t;
    goto c_sh;
  } else if ( t == 0 ) {
    while ( (t = u[i++]) == 0 ) ;
    c = 0;
  } else if ( t < 0 ) t += BASE, c = 1;
  else c = 0;
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
	*p++ = t | ((BMASK << lsh) & BMASK);
	for ( ; (w = u[i++]) == 0; *p++ = BMASK ) ;
	t = BMASK >> rsh;
      }
      w--;
      *p++ = t | ((w << lsh) & BMASK);
      t = w >> rsh;
    }
    for ( ; i < lu; i++, t = w >> rsh ) *p++ = t | (((w = u[i]) << lsh) & BMASK);
    if ( t == 0 ) return( i );
    *p = t;
  }
  return( i + 1 );
  /**/
 non0w:
  u += i, lu -= i,  v += i, lv -= i;
  /*  | t + BASE*(c + a*v[0:lv-1] - u[0:lu-1] | */
  if ( lv < lu ) {  /* a*V < U if lv<lu-1, and very likely if lv==lu-1 */
    t = -t;	    /* We shall compute U - a*V */
    if ( t < 0 ) t += BASE, c++;
  } else /* a*V > U if lv>lu, and very likely even if lv==lu */
    if ( t < 0 ) t += BASE, c--;
  TRAILINGZEROS( t, rsh );
  return( rsh == 0 ? (lv < lu ? abs_U_aV_c( t, u, lu, a, v, lv, c, ans ) :
		      abs_aVplusc_U( t, a, v, lv, c, u, lu, ans )) :
	  lv < lu ? abs_U_aV_c_sh( t, u, lu, a, v, lv, c, rsh, ans ) :
	  abs_aVplusc_U_sh( t, a, v, lv, c, u, lu, rsh, ans ) );
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

  for ( i = 0; i < lv; i++, *p++ = t, c = hi ) { DMA( a, v[i], c, hi, t, not_used); }
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
    DMA( a, v[i], c, hi, w, not_used );
    *p++ = t | ((w << lsh) & BMASK);
  }
  if ( c != 0 ) {
    *p++ = t | ((c << lsh) & BMASK);
    i++;
    t = c >> rsh;
  }
  if ( t == 0 ) return i;
  *p = t;
  return( i + 1 );
}

/*********/

static int abs_aVplusc_U( t, a, v, lv, c, u, lu, ans )
int t, a, *v, lv, c, *u, lu, *ans;
    /*  compute | t + BASE*(a*V+c - U) | in ans[], where V=v[0:lv-1], U=u[0:lu-1],
     *  and a > 1, -1 <= c < BASE, lv >= lu >=0 && t != 0 are assumed.
     */
{
  int i, l, b, *p = ans;
  int hi;

  *p++ = t, l = 1;
  if ( c < 0 ) c = 0, b = 1;
  else b = 0;
  for ( i = 0; i < lu; *p++ = t ) {
    DMA( a, v[i], c, hi, t, not_used );
    t -= u[i++],  c = hi;
    if ( b != 0 ) t--;
    b = 0;
    if ( t > 0 ) l = i + 1;
    else if ( t < 0 ) {
      t += BASE,  l = i + 1;
      if ( c != 0 ) c--;
      else b = 1;
    }
  }
  /* at this point, i == lu and we have written (i+1) entries in ans[] */
  if ( i >= lv ) {  /* no more elms in v[] */
    if ( b != 0 ) {
      if ( i == 0 ) { ans[i] = BASE - t; return 1; }
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
    *p++ - t;
  }
#ifdef CALL
  return( aV_plus_c( a, &v[i], lv - i, c, p ) + i +1 );
#else
  for ( ; i < lv; i++, *p++ = t, c = hi ) { DMA( a, v[i], c, hi, t, not_used );}
  if ( c == 0 ) return( i + 1 );
  *p = c;
  return( i + 2 );
#endif
}

static int abs_aVplusc_U_sh( t, a, v, lv, c, u, lu, rsh, ans )
int t, a, *v, lv, c, *u, lu, rsh, *ans;
{
  int i, l, w, b, lsh = BSH - rsh, *p = ans;
  int hi;

  if ( c < 0 ) c = 0, b = 1;
  else b = 0;
  for ( i = 0; i < lu; t = w >> rsh ) {
    DMA( a, v[i], c, hi, w, not_used );
    w -= u[i++],  c = hi;
    if ( b != 0 ) w--;
    b = 0;
    if ( w < 0 ) {
      w += BASE;
      if ( c != 0 ) c--;
      else b = 1;
    }
    if ( (*p++ = t | ((w << lsh) & BMASK)) != 0 ) l = i;
  }
  /* at this point, i == lu and we have written i entries in ans[] */
  if ( i >= lv ) {
    if ( b != 0 ) {  /* diff turned out to be < 0 */
      if ( i == 0 ) {
	*p = (BASE >> rsh) - t;
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
    *p++ = t | ((c << lsh) & BMASK);
    if ( (t = c >> rsh) == 0 ) return( i + 1 );
    *p = t;
    return( i + 2 );
  }
  /* lv >= lu+1 = i+1, and diff is > 0 */
  if ( b != 0 ) {  /* c == 0 */
    if ( (w = v[i++]) == 0 ) {
      *p++ = t | ((BMASK << lsh) & BMASK);
      while ( (w = v[i++]) == 0 ) *p++ = BMASK;
      t = BMASK >> rsh;
    } else {
      DM( a, w, c, b );
      if ( b != 0 ) b--;
      else b = BMASK, c--;
      *p++ = t | ((b << lsh) & BMASK);
      t = b >> rsh;
    }
  }
  /**/
#ifdef CALL
  return( aV_plus_c_sh( a, &v[i], lv - i, c, rsh, t, p ) + i );
#else
  for ( ; i < lv; i++, t = w >> rsh, c = hi ) {
    DMA( a, v[i], c, hi, w, not_used );
    *p++ = t | ((w << lsh) & BMASK);
  }
  if ( c != 0 ) {
    *p++ = t | ((c << lsh) & BMASK);
    i++,  t = c >> rsh;
  }
  if ( t == 0 ) return i;
  *p = t;
  return( i + 1 );
#endif
}

/*********/

static int abs_U_aV_c( t, u, lu, a, v, lv, c, ans )
int t, *u, lu, a, *v, lv, c, *ans;
    /*  compute | t + BASE*(U - a*V - c) | in ans[], where U = u[0:lu-1], V = v[0:lv-1],
     *	c is <= BASE-1, and lu >= lv+1 && t != 0 assumed.
     */
{
  int i, w, l, *p = ans;
  int hi;

  *p++ = t, l = 1;
  for ( i = 0; i < lv; *p++ = t ) {
    DMA( a, v[i], c, hi, w, not_used );
    t = u[i] - w;
    i++, c = hi;
    if ( t != 0 ) l = i + 1;
    if ( t < 0 ) t += BASE, c++;
  }
  /* at this point, i == lv */
  if ( lu == lv+1 ) {
    if ( (t = u[i]) == c ) return l;
    else if ( t > c ) { *p = t - c; return( lu + 1 ); }
    l = negate_nbd( ans, lu );		/* <================ */
    if ( (c -= (t + 1)) == 0 ) return l;
    *p = c;
    return( lu + 1 );
  }
  /*  lu >= lv+2 and the diff turned out to be >= 0 */
  if ( c != 0 ) {
    /* note c <= BASE.  If c == BASE, we must care about the length of the result
       of the case when lu == lv+2 && u[lu-2:lu-1]=u[i:i+1] == BASE  */
    if ( c >= BASE && i+2 == lu && u[i+1] == 1 ) {
      /* m.s. word of the diff becomes 0 */
      if ( (t = u[i] - (c & BMASK)) == 0 ) return l;
      *p = t;
      return lu;
    }
    for ( ; 1; c = 1, *p++ = t + BASE ) if ( (t = u[i++] - c) >= 0 ) break;
    if ( t == 0 && i >= lu ) return lu;
    *p++ = t;
  }
  for ( ; i < lu; i++ ) *p++ = u[i];
  return( lu + 1 );
}

static int abs_U_aV_c_sh( t, u, lu, a, v, lv, c, rsh, ans )
int t, *u, lu, a, *v, lv, c, rsh, *ans;
    /*  r-shift version of the above  */
{
  int i, w, l, lsh = BSH - rsh, *p = ans;
  int hi;

  for ( l = i = 0; i < lv; t = w >> rsh ) {
    DMA( a, v[i], c, hi, w, not_used );
    w = u[i] - w, c = hi;
    i++;
    if ( w < 0 ) w += BASE, c++;
    if ( (*p++ = t | ((w << lsh) & BMASK)) != 0 ) l = i;
  }
  /* at this point, i == lv, and we have written lv elm.s in ans[] */
  if ( lu == lv+1 ) {
    if ( (w = u[i] - c) == 0 ) {
      if ( t == 0 ) return l;
      *p = t;
      return lu; /* == lv+1 */
    } else if ( w > 0 ) {
 l0:  *p++ = t | ((w << lsh) & BMASK);
      if ( (w >>= rsh) == 0 ) return lu; /* == lv+1 */
      *p = w;
      return( lu + 1 );
    }
    /* diff turned out to be < 0 */
    w = - w - 1;
    if ( lv == 0 ) {	/* lu == 1 */
      t = (BASE >> rsh) - t;
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
    if ( c >= BASE && i+2 == lu && t == 0 && u[i] == 0 && u[i+1] == 1 )
      return l; /* m.s. word of the diff has become 0 */
    for ( ; 1; t = w >> rsh, c = 1 ) {
      if ( (w = u[i++] - c) >= 0 ) break;
      w += BASE;
      *p++ = t | ((w << lsh) & BMASK);
    }
    t |= ((w << lsh) & BMASK);
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
  for ( ; i < lu; i++, t = w >> rsh ) *p++ = t | (((w = u[i]) << lsh) & BMASK);
  if ( t == 0 ) return i;
  *p = t;
  return( i + 1 );
}

/****************/
/****************/
#ifdef FULLSET
static int abs_axU_V_maxrshift(), abs_axU_bxV_maxrshift();

static int abs_aU_bV_maxrshift( a, u, lu, b, v, lv, ans )
int a, *u, lu, b, *v, lv, *ans;
    /*
     *  Compute | a*U - b*V | >> (as much as possible) in ans[], where U=u[0:lu-1]
     *  and V=v[0:lv-1].
     */
{
  if ( a == 1 ) return abs_U_aV_maxrshift( u, lu, b, v, lv, ans );
  else if ( b == 1 ) return abs_U_aV_maxrshift( v, lv, a, u, lu, ans );
  else if ( a == b ) return abs_axU_V_maxrshift( a, u, lu, v, lv, ans );
  return abs_axU_bxV_maxrshift( a, u, lu, b, v, lv, ans );
}

static int abs_axU_V_maxrshift( a, u, lu, v, lv, ans )
int a, *u, lu, *v, lv, *ans;
    /*
     *  Compute a*| U - V | >> (as much as possible) in ans[], where U=u[0:lu-1]
     *  and V=v[0:lv-1], 0 < a < 2^BASE.
     */
{
  int i, l, b, c, t, rsh, *p = ans, lsh, w, x;
  int hi;

  if ( lu > lv ) goto U_V;
  else if ( lu < lv ) goto swap;
  else {
    for ( i = lu; i-- > 0; ) {
      if ( u[i] == v[i] ) continue;
      lu = lv = i + 1;
      if ( u[i] > v[i] ) goto U_V;
      else goto swap;
    }
    return 0;
  }
 swap:
  SWAP( lu, lv, int ); SWAP( u, v, int * );
 U_V:
  for ( b = c = i = 0; i < lv; ) {
    if ( (w = u[i] - v[i] - b) < 0 ) w += BASE, b = 1;
    else b = 0;
    DMA( a, w, c, hi, t, not_used );
    i++, c = hi;
    if ( t != 0 ) goto non0w;
  }
  while ( i < lu ) {
    if ( (w = u[i] - b) < 0 ) w += BASE, b = 1;
    else b = 0;
    DMA( a, w, c, hi, t, not_used );
    i++, c = hi;
    if ( t != 0 ) goto non0w;
  }
  if ( b != 0 ) c -= a;
  else if ( c == 0 ) return 0;
  while ( (c&1) == 0 ) c >>= 1;
  *p = c;
  return 1;
  /**/
 non0w:
  u += i, lu -= i,  v += i, lv -= i;
  TRAILINGZEROS( t, rsh );
  i = 0;
  if ( rsh == 0 ) {
    *p++ = t, l = 0;
    for ( ; i < lv; c = hi ) {
      if ( (w = u[i] - v[i] - b) < 0 ) w += BASE, b = 1;
      else b = 0;
      DMA( a, w, c, hi, t, not_used );
      i++;
      if ( (*p++ = t) != 0 ) l = i;
    }
    for ( ; i < lu; c = hi ) {
      if ( (w = u[i] - b) < 0 ) w += BASE, b = 1;
      else b = 1;
      DMA( a, w, c, hi, t, not_used );
      i++;
      if ( (*p++ = t) != 0 ) l = i;
    }
    if ( b != 0 ) c -= a;
    if ( c == 0 ) return( l + 1 );
    *p = c;
    return( i + 2 );
  } else {
    for ( lsh = BSH - rsh, l = 0; i < lv; c = hi, t = x >> rsh ) {
      if ( (w = u[i] - v[i] - b) < 0 ) w += BASE, b = 1;
      else b = 0;
      DMA( a, w, c, hi, x, not_used );
      i++;
      if ( (*p++ = t | ((x << lsh) & BMASK)) != 0 ) l = i;
    }
    for ( ; i < lu; i++, c = hi, t = x >> rsh ) {
      if ( (w = u[i] - b) < 0 ) w += BASE, b = 1;
      else b = 0;
      DMA( a, w, c, hi, x, not_used );
      i++;
      if ( (*p++ = t | ((x << lsh) & BMASK)) != 0 ) l = i;
    }
    if ( b != 0 ) c -= a;
    if ( c != 0 ) {
      *p++ = t | ((c << lsh) & BMASK);
      i++, t = c >> rsh;
    } else if ( t == 0 ) return l;
    if ( t == 0 ) return( i );
    *p = t;
    return( i + 1 );
  }
}
#endif /* FULLSET */

static int abs_axU_bxV_maxrshift( a, u, lu, b, v, lv, ans )
int a, *u, lu, b, *v, lv, *ans;
    /*
     *  Compute | a*U - b*V | >> (as much as possible) in ans[], where U=u[0:lu-1]
     *  and V=v[0:lv-1], 0 < a, b < 2^BASE.
     */
{
  int i, l, c, d, s, t, w, rsh, *p = ans, lsh;
  int hi;
  static int bw_int32();

  BitWidth( a, c );  BitWidth( u[lu -1], s );
  BitWidth( b, d );  BitWidth( v[lv -1], t );
  if ( lu < lv || lu == lv && (c + s) < (d + t) ) {
    SWAP( a, b, int );  SWAP( lu, lv, int );  SWAP( u, v, int * );
  }
  for ( i = c = d = 0; i < lv; ) {
    DMA( a, u[i], c, hi, t, not_used );  /* 0 <= c <= 2^BSH -2 */
    c = hi;
    DMA( b, v[i], d, hi, s, not_used );  /* 0 <= d <= 2^BSH -1 */
    i++, d = hi;
    if ( (t -= s) == 0 ) continue;
    else if ( t < 0 ) {
      t += BASE;
      if ( c != 0 ) c--;
      else d++;
    }
    goto non0w;
  }
  if ( i >= lu ) {  /* no more elm.'s in u[] and v[] */
  only_c:
    if ( (c -= d) == 0 ) return 0;
    else if ( c < 0 ) c = -c;
  sh_onlyc:
    /* TRAILINGZEROS( c, rsh ); */
    while ( (c&1) == 0 ) c >>= 1;
    *p = c;
    return 1;
  }
  /* there is at least one more elm. in u[] */
  DMA( a, u[i], c, hi, t, not_used );
  i++, c = hi;
  if ( i >= lu ) {  /* in this case, diff still can be <= 0 */
    if ( hi == 0 ) { c = t; goto only_c; }
    if ( (t -= d) == 0 ) { c = hi; goto sh_onlyc; }
    else if ( t < 0 ) t += BASE, hi--;
    TRAILINGZEROS( t, rsh );
    if ( rsh == 0 ) *p++ = t;
    else {
      *p++ = t | ((hi << (BSH - rsh)) & BMASK);
      hi >>= rsh;
    }
    if ( hi == 0 ) return 1;
    *p = hi;
    return 2;
  } /* diff turned out to be > 0 */ else if ( (t -= d) > 0 ) d = 0;
  else if ( t < 0 ) t += BASE, d = 1;
  else {
    while ( i < lu ) {
      DMA( a, u[i], c, hi, t, not_used );
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
  /**/
 non0w:
  u += i, lu -= i,  v += i, lv -= i;
  TRAILINGZEROS( t, rsh );
  i = 0;
  if ( rsh == 0 ) {
    *p++ = t, l = 0;
    for ( ; i < lv; *p++ = t ) {
      DMA( a, u[i], c, hi, t, not_used );
      c = hi;
      DMA( b, v[i], d, hi, s, not_used );
      i++, d = hi;
      if ( (t -= s) == 0 ) continue;
      else if ( t < 0 ) {
	t += BASE;
	if ( c != 0 ) c--;
	else d++;
      }
      l = i;
    }
  no_more_v:
    if ( i >= lu ) {
    final_c_d:
      if ( (c -= d) == 0 ) return( l + 1 );
      else if ( c < 0 ) {
	l = negate_nbd( ans, i + 1 );		/* <================ */
	if ( (c = -c - 1) == 0 ) return l;
      }
      *p = c;
      return( i + 2 );
    }
    /* a*u[i:lu-1] + c - d */
    if ( c >= d ) c -= d;
    else {
      DMA( a, u[i], c, hi, t, not_used );
      if ( i+1 >= lu ) {
	if ( hi == 0 ) { c = t; goto final_c_d; }
	if ( (t -= d) < 0 ) t += BASE, c = hi - 1;
	else c = hi;
	i++, *p++ = t;
	goto final_c;
      }
      i++, c = hi;
      if ( (t -= d) >= 0 ) *p++ = t;
      else {
	*p++ = t + BASE;
	if ( c != 0 ) c--;
	else {
	  for ( ; i < lu; *p++ = BMASK ) {
	    DMA( a, u[i], c, hi, t, not_used );
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
    for ( ; i < lu; i++, c = hi, *p++ = t ) { DMA( a, u[i], c, hi, t, not_used ); }
  final_c:
    if ( c == 0 ) return( i + 1 );
    *p = c;
    return( i + 2 );
  } else {  /*  rsh != 0 */
    for ( lsh = BSH - rsh; i < lv; t = w >> rsh ) {
      DMA( a, u[i], c, hi, w, not_used );
      c = hi;
      DMA( b, v[i], d, hi, s, not_used );
      i++, d = hi;
      if ( (w -= s) < 0 ) {
	w += BASE;
	if ( c != 0 ) c--;
	else d++;
      }
      if ( (*p++ = t | ((w << lsh) & BMASK)) != 0 ) l = i;
    }
  no_more_v_sh:
    if ( i >= lu ) {
    final_c_d_sh:
      if ( (c -= d) == 0 )
	if ( t == 0 ) return l;
	else { *p = t; return( i + 1 ); }
      else if ( c < 0 ) {
	if ( i == 0 ) t = (BASE >> rsh) - t;
	else {
	  l = negate_nbd( ans, i );		/* <================ */
/*	  t = (BASE >> rsh) - t;*/
	  t ^= ((BMASK >> rsh));
	}
	c = -c -1;
      }
      *p++ = t | ((c << lsh) & BMASK);
      if ( (c >>= rsh) == 0 ) return( i + 1 );
      *p = c;
      return( i + 2 );
    } else if ( c >= d ) c -= d;
    else {
      DMA( a, u[i], c, hi, w, not_used );
      if ( i+1 >= lu ) {
	if ( hi == 0 ) { c = w; goto final_c_d_sh; }
	if ( (w -= d) < 0 ) w += BASE, c = hi - 1;
	else c = hi;
	i++, *p++ = t | ((w << lsh) & BMASK);
	t = w >> rsh;
	goto final_c_sh;
      }
      i++, c = hi;
      if ( (w -= d) >= 0 ) { *p++ = t | ((w << lsh) & BMASK); t = w >> rsh; }
      else {
	w += BASE;
	*p++ = t | ((w << lsh) & BMASK);
	t = w >> rsh;
	if ( c != 0 ) c--;
	else {
	  while ( i < lu ) {
	    DMA( a, u[i], c, hi, w, not_used );
	    i++, c = hi;
	    if ( w == 0 ) {
	      *p++ = t | ((BMASK << lsh) & BMASK);
	      t = BMASK >> rsh;
	      continue;
	    } else {
	      w--;
	      *p++ = t | ((w << lsh) & BMASK);
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
      DMA( a, u[i], c, hi, w, not_used );
      *p++ = t | ((w << lsh) & BMASK);
    }
  final_c_sh:
    if ( c != 0 ) {
      *p++ = t | ((c << lsh) & BMASK);
      i++, t = c >> rsh;
    }
    if ( t == 0 ) return i;
    *p = t;
    return( i + 1 );
  }
}
/****************/
/****************/
#ifdef FULLSET
static int UplusV_maxrshift(), aUplusV_maxrshift(), axUplusV_maxrshift(), aUplusbV_maxrshift();

static int aU_plus_bV_maxrshift( a, u, lu, b, v, lv, ans )
int a, *u, lu, b, *v, lv, *ans;
{
  if ( a == 1 )
    return( b == 1 ? UplusV_maxrshift( u, lu, v, lv, ans ) :
	    aUplusV_maxrshift( b, v, lv, u, lu, ans ) );
  else if ( b == 1 ) return aUplusV_maxrshift( a, u, lu, v, lv, ans );
  else if ( a == b ) return axUplusV_maxrshift( a, u, lu, v, lv, ans );
  return aUplusbV_maxrshift( a, u, lu, b, v, lv, ans );
}

static int UplusV_maxrshift( u, lu, v, lv, ans )
int *u, lu, *v, lv, *ans;
    /*
     *  Compute ( (U + V) >> (as much as possible) ) in ans[], where U=u[0:lu-1],
     *  V=v[0:lv-1], and 0 < a < BASE.
     */
{
  int i, t, c, rsh, *p = ans;
  int hi;

  if ( lu < lv ) { SWAP( lu, lv, int ); SWAP( u, v, int * ); }
  for ( c = i = 0; i < lv; ) {
    t = (c += (u[i] + v[i])) & BMASK;
    /* 0 <= c <= 2*(2^BSH -1) + 1 = 2^BSH + (2^BSH -1) */
    i++, c >>= BSH;
    if ( t != 0 ) goto non0w;
  }
  while ( i < lu ) {
    t = (c += u[i]) & BMASK;
    i++, c >>= BSH;
    if ( t != 0 ) goto non0w;
  }
  *p = c;
  return 1;
  /**/
 non0w:
  u += i, lu -= i,  v += i, lv -= i;
  TRAILINGZEROS( t, rsh );
  i = 0;
  if ( rsh == 0 ) {
    *p++ = t;
    for ( ; i < lv; i++, c >>= BSH ) *p++ = (c += (u[i] + v[i])) & BMASK;
    if ( c != 0 ) {
      while ( i < lu )
	if ( (c = u[i++] + 1) >= BASE ) *p++ = c & BMASK;
	else {
	  *p++ = c;
	  goto cpu;
	}
      *p = 1;  /* == c */
      return( lu + 2 );
    }
  cpu:
    for ( ; i < lu; i++ ) *p++ = u[i];
    return( lu + 1 );
  } else {
    int lsh = BSH - rsh;

    for ( ; i < lv; i++, c >>= BSH ) {
      c += (u[i] + v[i]);
      *p++ = t | ((c << lsh) & BMASK);
      t = (c & BMASK) >> rsh;
    }
    if ( c != 0 ) {
      while ( i < lu ) {
	c = u[i++] + 1;
	*p++ = t | ((c << lsh) & BMASK);
	t = (c & BMASK) >> rsh;
	if ( (c >>= BSH) == 0 ) goto cpu_sh;
      }
      *p = t | (1 << lsh);
      return( lu + 1 );
    }
  cpu_sh:
    for ( ; i < lu; t = c >> rsh ) *p++ = t | (((c = u[i++]) << lsh) & BMASK);
    if ( t == 0 ) return( lu );
    *p = t;
    return( lu + 1 );
  }
}

static int aUplusV_maxrshift( a, u, lu, v, lv, ans )
int a, *u, lu, *v, lv, *ans;
    /*
     *  Compute ( (a*U + V) >> (as much as possible) ) in ans[], where U=u[0:lu-1],
     *  V=v[0:lv-1], and 1 < a < BASE.
     */
{
  int i, l, t, c, rsh, *p = ans, w, lsh;
  int hi;

  for ( l = lu < lv ? lu : lv, c = i = 0; i < l; ) {
    /*  Note that (carry in a*U[*]) <= 2^BSH-2 because {(2^j-1)}^2 + (2^j-2)
     *  = 2^j*(2^j -2)+(2^j -1), and the following c is <= 2^j -1 because
     *  {(2^j -1)}^2 + (2^j -1) + (2^j -1) = 2^j*(2^j -1) + (2^j -1).
     *                  ^^^^^^carry^^^^^^ pv[?].
     */
    c += v[i];
    DMA( a, u[i], c, hi, t, not_used );
    i++, c = hi;
    if ( t != 0 ) goto non0sum;
  }
  if ( i >= lu ) {  /* compute (c + v[i:lv-1]) */
    while ( i < lv ) {
      t = (c += v[i++]) & BMASK;
      c >>= BSH;
      if ( t != 0 ) goto non0w_v;
    }
    *p = 1;
    return 1;
  non0w_v:
    v += i, lv -= i;
    i = 0;
    TRAILINGZEROS( t, rsh );
    if ( rsh == 0 ) {
      *p++ = t;
    L_addv:
      if ( c != 0 ) {
	while ( i < lv )
	  if ( (c = u[i++] + 1) >= BASE ) *p++ = c & BMASK;
	  else {
	    *p++ = c;
	    goto cpv;
	  }
	*p = 1;  /* == c */
	return( lv + 2 );
      }
    cpv:
      for ( ; i < lv; i++ ) *p++ = v[i];
      return( lv + 1 );
    } else {
      lsh = BSH - rsh;
    L_addv_sh:
      if ( c != 0 ) {
	while ( i < lv ) {
	  c += v[i++];
	  *p++ = t | ((c << lsh) & BMASK);
	  t = (c & BMASK) >> rsh;
	  if ( (c >>= BSH) == 0 ) goto cpv_sh;
	}
	*p = t | (1 << lsh);
	return( lv + 1 );
      }
    cpv_sh:
      for ( ; i < lv; t = c >> rsh ) *p++ = t | (((c = v[i++]) << lsh) & BMASK);
      if ( t == 0 ) return( lv );
      *p = t;
      return( lv + 1 );
    }
  } else {  /* i >= lv, and compute (c + a*u[i:lu-1]) */
    while ( i < lu ) {
      DMA( a, u[i], c, hi, t, not_used );
      i++, c = hi;
      if ( t != 0 ) goto non0w_u;
    }
    TRAILINGZEROS( c, rsh );
    *p = c;
    return 1;
    /**/
  non0w_u:
    u += i, lu -= i;
    TRAILINGZEROS( t, rsh );
    i = 0;
    if ( rsh == 0 ) {
      *p++ = t;
    L_addu:
#ifdef CALL
      return( aV_plus_c( a, u, lv, c, p ) + 1 );
#else
      for ( ; i < lu; i++, *p++ = t, c = hi ) { DMA( a, u[i], c, hi, t, not_used ); }
      if ( c == 0 ) return( lu + 1 );
      *p = c;
      return( lu + 2 );
#endif
    } else {
      lsh = BSH - rsh;
    L_addu_sh:
#ifdef CALL
      return aV_plus_c_sh( a, u, lu, c, rsh, t, p );
#else
      for ( ; i < lu; i++, c = hi, t = w >> rsh ) {
	DMA( a, u[i], c, hi, w, not_used );
	*p++ = t | ((w << lsh) & BMASK);
      }
      if ( c != 0 ) {
	*p++ = t | ((c << lsh) & BMASK);
	i++, t = c >> rsh;
      }
      if ( t == 0 ) return i;
      *p = t;
      return( i + 1 );
#endif
    }
  }
  /**/
 non0sum:
  u += i, lu -= i, v += i, lv -= i, l -= i;
  TRAILINGZEROS( t, rsh );
  i = 0;
  if ( rsh == 0 ) {
    *p++ = t;
    for ( ; i < l; i++, *p++ = t, c = hi ) {
      c += v[i];
      DMA( a, u[i], c, hi, t, not_used );
    }
    if ( i >= lu ) /* compute (c + v[i:lv-1]).  note i may be >= lv */ goto L_addv;
    else /* i >= lv, and compute (c + u[i:lu-1]) */ goto L_addu;
  } else {
    lsh = BSH - rsh;
    for ( ; i < l; i++, c = hi, t = w >> rsh ) {
      c += v[i];
      DMA( a, u[i], c, hi, w, not_used );
      *p++ = t | ((w << lsh) & BMASK);
    }
    if ( i >= lu ) /* compute (c + v[i:lv-1]) >> rsh.  note i may be >= lv */ goto L_addv_sh;
    else /* i >= lv, and compute (c + u[i:lu-1]) >> rsh */ goto L_addu_sh;
  }
}


static int axUplusV_maxrshift( a, u, lu, v, lv, ans )
int a, *u, lu, *v, lv, *ans;
    /*
     *  Compute ( a*(U + V) >> (as much as possible) ) in ans[], where U=u[0:lu-1],
     *  V=v[0:lv-1], and 1 < a < BASE.
     */
{
  int i, t, c, d, rsh, w, *p = ans, lsh, x;
  int hi;

  if ( lu < lv ) { SWAP( lu, lv, int ); SWAP( u, v, int * ); }
  for ( i = c = d = 0; i < lv; ) {
    w = (d += (u[i] + v[i])) & BMASK;
    DMA( a, w, c, hi, t, not_used );
    i++, d >>= BSH, c = hi;
    if ( t != 0 ) goto non0w;
  }
  while ( i < lu ) {
    w = (d += u[i++]) & BMASK;
    DMA( a, w, c, hi, t, not_used );
    d >>= BSH, c = hi;
    if ( t != 0 ) goto non0w;
  }
  if ( d != 0 ) c += a;
  TRAILINGZEROS( c, rsh );
  if ( rsh != 0 || c <= BMASK ) { *p = c; return 1; }
  *p++ = c & BMASK;
  *p = c >> BSH;
  return 2;
  /**/
 non0w:
  u += i, lu -= i,  v += i, lv -= i;
  TRAILINGZEROS( t, rsh );
  i = 0;
  if ( rsh == 0 ) {
    *p++ = t;
    for ( ; i < lv; i++, d >>= BSH, *p++ = t, c = hi ) {
      w = (d += (u[i] + v[i])) & BMASK;
      DMA( a, w, c, hi, t, not_used );
    }
    for ( ; i < lu; d >>= BSH, *p++ = t, c = hi ) {
      w = (d += u[i++]) & BMASK;
      DMA( a, w, c, hi, t, not_used );
    }
    if ( d != 0 ) c += a;
    if ( c == 0 ) return( lu + 1 );
    *p++ = c & BMASK;
    if ( (c >>= BSH) == 0 ) return( lu + 2 );
    *p = c;
    return( lu + 3 );
  } else {
    for ( lsh = BSH - rsh; i < lv; i++, d >>= BSH, t = x >> rsh, c = hi ) {
      w = (d += (u[i] + v[i])) & BMASK;
      DMA( a, w, c, hi, x, not_used );
      *p++ = t | ((x << lsh) & BMASK);
    }
    for ( ; i < lu; d >>= BSH, t = x >> rsh, c = hi ) {
      w = (d += u[i++]) & BMASK;
      DMA( a, w, c, hi, x, not_used );
      *p++ = t | ((x << lsh) & BMASK);
    }
    if ( d != 0 ) c += a;
    t |= ((c << lsh) & BMASK);
    c >>= rsh;
    if ( c != 0 ) {
      *p++ = t;
      *p = c;
      return( lu + 2 );
    } else if ( t == 0 ) return lu;
    *p = t;
    return( lu + 1 );
  }
}
#endif /* FULLSET */

static int aUplusbV_maxrshift( a, u, lu, b, v, lv, ans )
int a, *u, lu, b, *v, lv, *ans;
    /*
     *  Compute ( (a*U + b*V) >> (as much as possible) ) in ans[], where U=u[0:lu-1],
     *  V=v[0:lv-1], and 0 < a,b < BASE.
     */
{
  int i, c, d, s, t, w, rsh, *p = ans, lsh;
  int hi;

  if ( lu < lv ) { SWAP( a, b, int ); SWAP( lu, lv, int ); SWAP( u, v, int * ); }
  for ( c = d = i = 0; i < lv; ) {
    DMA( a, u[i], c, hi, t, not_used );  /* 0 <= c <= 2^BSH -1 */
    c = hi;
    DMA( b, v[i], d, hi, s, not_used );  /* 0 <= d <= 2^BSH -2 */
    i++, d = hi, t += s;
    if ( t > BMASK ) c++, t &= BMASK;
    if ( t != 0 ) goto non0w;
  }
  c += d;
  for ( d = 0; i < lu; ) {
    DMA( a, u[i], c, hi, t, not_used );
    i++, c = hi;
    if ( t != 0 ) goto non0w;
  }
  TRAILINGZEROS( c, rsh );
  if ( rsh != 0 || c <= BMASK ) { *p = c; return 1; }
  *p++ = c & BMASK;
  *p = 1;
  return 2;
  /**/
 non0w:
  u += i, lu -= i,  v += i, lv -= i;
  TRAILINGZEROS( t, rsh );
  i = 0;
  if ( rsh == 0 ) {
    *p++ = t;
    for ( ; i < lv; i++, *p++ = t ) {
      DMA( a, u[i], c, hi, t, not_used );
      c = hi;
      DMA( b, v[i], d, hi, s, not_used );
      d = hi, t += s;
      if ( t > BMASK ) c++, t &= BMASK;
    }
    c += d;
    for ( ; i < lu; i++, *p++ = t, c = hi ) {
      DMA( a, u[i], c, hi, t, not_used );
    }
    if ( c == 0 ) return( lu + 1 );
    else if ( c <= BMASK ) { *p = c; return( lu + 2 ); }
    *p++ = c & BMASK;
    *p = 1;
    return( lu + 3 );
  } else {
    for ( lsh = BSH - rsh; i < lv; i++, t = w >> rsh ) {
      DMA( a, u[i], c, hi, w, not_used );
      c = hi;
      DMA( b, v[i], d, hi, s, not_used );
      d = hi, w += s;
      if ( w > BMASK ) c++, w &= BMASK;
      *p++ = t | ((w << lsh) & BMASK);
    }
    c += d;  /* <= 2^BSH + (2^BSH - 3) */
    for ( ; i < lu; i++, c = hi, t = w >> rsh ) {
      DMA( a, u[i], c, hi, w, not_used );
      *p++ = t | ((w << lsh) & BMASK);
    }
    if ( c == 0 ) {
      if ( t == 0 ) return lu;
      *p = t;
      return( lu + 1 );
    }
    *p++ = t | ((c << lsh) & BMASK);
    if ( (c >>= rsh) == 0 ) return( lu + 1 );
    *p = c;
    return( lu + 2 );
  }
}
