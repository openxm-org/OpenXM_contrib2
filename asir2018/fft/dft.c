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
 * $OpenXM: OpenXM_contrib2/asir2018/fft/dft.c,v 1.1 2018/09/19 05:45:07 noro Exp $
*/
/*
 *    store      arith    modulus
 *  -------------------------------------------------------
 *  IEEE:  float (23+1)  <->  double (52+1)  24 bits
 *    int   (32)  <->  double (52+1)  26 bits
 *  370:  float (24)  <->  double (56)  24/25 bits
 *    int   (32)  <->  double (56)  28 bits
 */


#include "dft.h"

/*
#define C_DFT_FORE C_DFTfore
#define C_DFT_BACK C_DFTback
#define C_PREP_ALPHA C_prep_alpha
*/


/*****************************************************/

void C_DFT_FORE( in, nin, i1, K, powa,
#ifdef POWA_STRIDE
     a1,
#endif
     out, o1, P, Pinv, wk )
ModNum in[], powa[], out[], wk[];
ModNum P;
int nin, i1, K, o1;
#ifdef POWA_STRIDE
int a1;
#endif
double Pinv;
/*
 *  Let N = 2^K, and \alpha be a primitive N-th root of 1.
 *  out[0:N-1][o1] = DFT of in[0:nin-1][i1] using \alpha modulo P.
 *  The powers \alpha (to the degrees upto N/2=2^{K-1} at least) are given in powa[*],
 *  (or powa[a1**]).
 *  nin <= N assumed.
 *  wk[] must have 2^K entries at least.
 */
{
  int istep, ostep, i, j, k, idist, odist, n, Ndiv2n, K_k,
#ifndef OPT
      K_k_1,
#endif
#ifdef POWA_STRIDE
      aj,
#endif
      inwk;
  int idistwk, idistout, iostep, oostep;
  ModNum *qi, *qo, *qis, *qos, a, t0, tn;

  /*
   *  for k = K-1, ..., 0 do the following:
   *
   *      Out[2^{K-k}*i + j]       <= t0 + \alpha^{2^k * j} * tn;
   *      Out[2^{K-k}*i + j + 2^{K-k-1}] <= t0 - \alpha^{2^k * j} * tn;
   *
   *      where t0 = In[2^{K-k-1}*i + j] and
   *    tn = In[2^{K-k-1}*i + j + 2^{K-1}],
   *      for 0 <= i < 2^k and 0 <= j < 2^{K-k-1}.
   *
   */
  /*  In the following, n = 2^k and Ndiv2n = 2^{K-k-1}.
   */
  n = 1 << (k = K - 1);
  idistwk = n,  idistout = o1 << k;
  /*  when k = K-1, Out[2*i] <= In[i] + In[i+2^{K-1}],
   *  and           Out[2*i+1] <= In[i] - In[i+2^{K-1}], for 0 <= i < 2^{K-1}.
   */
  if ( k&1 ) qo = wk, ostep = 2, inwk = 1, odist = 1;
  else qo = out, ostep = o1 << 1, inwk = 0, odist = o1;
  qi = in;
  /**/
  if ( nin <= n ) {
    for ( i = nin; i > 0; i--, qi += i1, qo += ostep ) qo[0] = qo[odist] = qi[0];
    for ( i = n - nin; i > 0; i--, qo += ostep ) qo[0] = qo[odist] = 0;
  } else {
    idist = i1 << k;  /* distance = 2^{K-1} */
    for ( j = i = nin - n; i > 0; i--, qi += i1, qo += ostep ) {
      t0 = qi[0],  tn = qi[idist];
      if ( tn == (ModNum)0 ) qo[0] = qo[odist] = t0;
      else {
  qo[0] = AplusBmodP( t0, tn, P );
  qo[odist] = A_BmodP( t0, tn, P );
      }
    }
    for ( i = n - j; i > 0; i--, qi += i1, qo += ostep )
      qo[0] = qo[odist] = qi[0];
  }
  /******/
#ifndef J_INNER
  for ( K_k = 1, Ndiv2n = 1; --k > 0; inwk = 1 - inwk ) {
#ifndef OPT
    K_k = (K_k_1 = K_k) + 1;
#endif
    n >>= 1;    /* == 2^k */
    Ndiv2n <<= 1;  /* == 2^{K-k-1} */
    if ( inwk )
      qi = wk, qo = out, iostep = 1, oostep = o1, idist = idistwk;
    else
      qi = out, qo = wk, iostep = o1, oostep = 1, idist = idistout;
    qis = qi,  qos = qo;
    /**/
    istep = ostep;  /* = iostep << K_k_1; */
#ifndef OPT
    ostep = oostep << K_k;
    odist = oostep << K_k_1;
#else
    odist = oostep << K_k;
    ++K_k;
    ostep = oostep << K_k;
#endif /* OPT */
    /* j = 0 */
    for ( i = n; i-- > 0; qi += istep, qo += ostep ) {
      t0 = qi[0],  tn = qi[idist];
      if ( tn == (ModNum)0 ) qo[0] = qo[odist] = t0;
      else {
  qo[0] = AplusBmodP( t0, tn, P );
  qo[odist] = A_BmodP( t0, tn, P );
      }
    }
    /**/
#ifdef POWA_STRIDE
    for ( aj = a1, j = 1; j < Ndiv2n; aj += a1, j++ ) {
      a = powa[aj << k];
#else
    for ( j = 1; j < Ndiv2n; j++ ) {
      a = powa[j << k];
#endif
      qi = (qis += iostep),  qo = (qos += oostep);
      for ( i = 0; i < n; i++, qi += istep, qo += ostep ) {
  t0 = qi[0],  tn = qi[idist];
  if ( tn == (ModNum)0 ) qo[0] = qo[odist] = t0;
  else {
    AxBmodP( tn, ModNum, tn, a, P, Pinv );
    qo[0] = AplusBmodP( t0, tn, P );
    qo[odist] = A_BmodP( t0, tn, P );
  }
      }
    }
  }
  /*
   *  When k = 0, for i = 0 (0 <= i < 2^0=1) and 0 <= j < 2^{K-1},
   *  Out[j] <= t0 + \alpha^{j}*tn and Out[j+2^{K-1}] <= t0 - \alpha^{j}*tn,
   *  where t0 = In[j] and tn = In[j + 2^{K-1}].
   */
  qi = wk, qo = out, idist = idistwk, odist = idistout;
  /*                       == 2^{K-1},      == o1 << (K-1) */
  t0 = qi[0],  tn = qi[idist];
  if ( tn == (ModNum)0 ) qo[0] = qo[odist] = t0;
  else {
    qo[0] = AplusBmodP( t0, tn, P );
    qo[odist] = A_BmodP( t0, tn, P );
  }
#ifdef POWA_STRIDE
  for ( k = o1, aj = a1, j = 1; j < idist; aj += a1, j++ ) {
#else
  for ( k = o1, j = 1; j < idist; j++ ) {
#endif
    qi++,  qo += k;
    t0 = qi[0],  tn = qi[idist];
    if ( tn == (ModNum)0 ) qo[0] = qo[odist] = t0;
    else {
#ifdef POWA_STRIDE
      a = powa[aj];
#else
      a = powa[j];
#endif
      AxBmodP( tn, ModNum, tn, a, P, Pinv );
      qo[0] = AplusBmodP( t0, tn, P );
      qo[odist] = A_BmodP( t0, tn, P );
    }
  }
#else
  for ( K_k = 1, Ndiv2n = 1; --k >= 0; inwk = 1 - inwk ) {
#ifndef OPT
    K_k = (K_k_1 = K_k) + 1;
#endif
    n >>= 1;    /* == 2^k */
    Ndiv2n <<= 1;  /* == 2^{K-k-1} */
    if ( inwk ) qi = wk, qo = out, istep = 1, ostep = o1, idist = idistwk;
    else qi = out, qo = wk, istep = o1, ostep = 1, idist = idistout;
    qis = qi,  qos = qo;
    /**/
    iostep = oostep;  /* = istep << K_k_1; */
#ifndef OPT
    oostep = ostep << K_k;
    odist = ostep << K_k_1;
#else
    odist = ostep << K_k;
    ++K_k;
    oostep = ostep << K_k;
#endif /* OPT */
    for ( i = n; i-- > 0; qis += iostep, qos += oostep ) {
      /* j = 0 */
      t0 = qis[0],  tn = qis[idist];
      if ( tn == (ModNum)0 ) qos[0] = qos[odist] = t0;
      else {
  qos[0] = AplusBmodP( t0, tn, P );
  qos[odist] = A_BmodP( t0, tn, P );
      }
#ifdef POWA_STRIDE
      for ( aj = a1, j = 1, qi = qis, qo = qos; j < Ndiv2n; aj += a1, j++ ) {
#else
      for ( j = 1, qi = qis, qo = qos; j < Ndiv2n; j++ ) {
#endif
  qi += istep,  qo += ostep;
  t0 = qi[0],  tn = qi[idist];
  if ( tn == (ModNum)0 ) qo[0] = qo[odist] = t0;
  else {
#ifdef POWA_STRIDE
    a = powa[aj << k];
#else
    a = powa[j << k];
#endif
    AxBmodP( tn, ModNum, tn, a, P, Pinv );
    qo[0] = AplusBmodP( t0, tn, P );
    qo[odist] = A_BmodP( t0, tn, P );
  }
      }
    }
  }
#endif
}

/*********************************************************************
 *
 *  NOTE:
 *  (1) Let \alpha be a primitive N-th root of unity modulo P, 
 *  where N is even,
 *  and t be an integer s.t. 0 <= t < N/2.
 *  Then, 1/\alpha^t is given by (- \alpha^s) with s = N/2 - t.
 *
 *  (2) Let P be a prime s.t. P = 2^n*Q + 1, where Q is an odd integer.
 *  Then, for 0 < s <= n, 2^{-s} \equiv - 2^{n-s}*Q \bmod P, 
 *  because 1 \equiv - 2^n*Q \bmod P.
 *
 **********************************************************************/

void C_DFT_BACK( in, N, i1, log2N, powa,
#ifdef POWA_STRIDE
     a1,
#endif
     out, o1, osi, nout, Ninv, P, Pinv, wk )
ModNum in[], powa[], out[], wk[];
int N, log2N, osi, nout, i1, o1;
#ifdef POWA_STRIDE
int a1;
#endif
ModNum P, Ninv;
double Pinv;
/*
 *  Let K denote log2N.  Let N = 2^K, and \alpha be a primitive N-th root of 1.
 *  This routine computes the inverse discrete-Fourier transform of in[0:N-1][i1]
 *  using \alpha^{-1} modulo P, and store the `nout' transformed data from the
 *  `osi'-th in out[0:nout-1][o1].  0 <= osi < N-1 and 0 < nout <= N.
 *  The powers of \alpha (to the degrees upto N/2=2^{K-1} at least) are given in powa[*] (or powa[a1* *]).
 *  The area wk[] is used to contain the intermediate transformed data, and thus,
 *  must have 2*N entries at least.  Notice that `out' cannot be used because
 * its space amount (`nout') may not be sufficient for this purpose.
 */
{
  int i, j, k, n, is, os, istep, ostep, idist, inwk, halfN;
#ifdef POWA_STRIDE
  int aj, halfNa;
#endif
  ModNum *qi, *qo, *qis, *qos, a, t0, tn, tt;

  /*
   *  for k = K-1, ..., 0 do the following:
   *
   *      Out[2^{K-k}*i + j]       <= t0 + \alpha^{- 2^k * j} * tn;
   *              = t0 - \alpha^{N/2 - 2^k * j}*tn;
   *      Out[2^{K-k}*i + j + 2^{K-k-1}] <= t0 - \alpha^{- 2^k * j} * tn;
   *              = t0 + \alpha^{N/2 - 2^k * j}*tn;
   *
   *      where t0 = In[2^{K-k-1}*i + j] and
   *    tn = In[2^{K-k-1}*i + j + 2^{K-1}],
   *      for 0 <= i < 2^k and 0 <= j < 2^{K-k-1}.
   *
   */
  if ( log2N == 1 ) {
    /*  K = 1, k = 0, 0 <= i < 1, 0 <= j < 1.
     *  Out[0] <= t0 + tn, Out[1] <= t0 - tn,
     *  where t0 = In[0] and Tn = In[1].
     */
    t0 = in[0],  tn = in[i1];
    if ( osi == 0 ) {
      /*  out[0] = (t0 + tn)*Ninv; */
      tt = tn == (ModNum)0 ? t0 : AplusBmodP( t0, tn, P );
      if ( tt != (ModNum)0 ) {
  AxBmodP( tt, ModNum, tt, Ninv, P, Pinv );
      }
      out[0] = tt;
      i = 1;
    } else i = 0;
    /**/
    if ( osi + nout >= 2 ) {
      /*  out[osi == 0 ? 1 : 0] = (t0 - tn)*Ninv; */
      tt = tn == (ModNum)0 ? t0 : A_BmodP( t0, tn, P );
      if ( tt != (ModNum)0 ) {
  AxBmodP( tt, ModNum, tt, Ninv, P, Pinv );
      }
      out[i] = tt;
    }
    return;
  }
  /****/
  halfN = n = 1 << (k = log2N - 1);  /* == 2^{K-1} */
#ifdef POWA_STRIDE
  halfNa = a1 << k;
#endif
  /*
   *  when k = K-1,
   *  Out[2*i] <= t0 + tn, and Out[2*i+1] <= t0 - tn,
   *  where t0 = In[i] and tn = In[i+2^{K-1}],
   *  for 0 <= i < 2^{K-1}.
   */
  qi = in, istep = i1, idist = i1 << k, qo = wk, inwk = 0;
  for ( i = n; i-- > 0; qi += istep, qo += 2 ) {
    t0 = qi[0],  tn = qi[idist];
    if ( tn == (ModNum)0 ) qo[0] = qo[1] = t0;
    else {
      qo[0] = AplusBmodP( t0, tn, P );
      qo[1] = A_BmodP( t0, tn, P );
    }
  }
#ifdef EBUG
  fprintf( stderr, "::: DFT^{-1} ::: after the first step\n" );
  for ( qo = wk, i = 0; i < N; i++ ) {
    if ( (i%5) == 0 ) fprintf( stderr, "\t" );
    fprintf( stderr, "%10d,", (int)wk[i] );
    if ( (i%5) == 4 ) fprintf( stderr, "\n" );
  }
  if ( (i%5) != 0 ) fprintf( stderr, "\n" );
#endif
  /**/
  idist = halfN;
  for ( ostep = 2; --k > 0; inwk = 1 - inwk ) {
    n >>= 1;
    istep = ostep;  /* == 2^{K-k-1} */
    ostep <<= 1;  /* == 2^{K-k} */
    if ( inwk ) qi = &wk[N], qo = wk;
    else qi = wk, qo = &wk[N];
    qis = qi,  qos = qo;
    /*
     *  for j = 0,
     *  Out[2^{K-k}*i] <= t0 + tn, and Out[2^{K-k}*i + 2^{K-k-1}] <= t0 - tn,
     *  where t0 = In[2^{K-k-1}*i] and tn = In[2^{K-k-1}*i + 2^{K-1}].
     */
    for ( i = n, is = os = 0; i-- > 0; qi += istep, qo += ostep ) {
      t0 = qi[0],  tn = qi[idist];
      if ( tn == (ModNum)0 ) qo[0] = qo[istep] = t0;
      else {
  qo[0] = AplusBmodP( t0, tn, P );
  qo[istep] = A_BmodP( t0, tn, P );
      }
    }
#ifdef POWA_STRIDE
    for ( aj = a1, j = 1; j < istep; aj += a1, j++ ) {
/***      a = P - powa[halfNa - (aj << k)]; ***/
      a = powa[halfNa - (aj << k)];
#else
    for ( j = 1; j < istep; j++ ) {
/***      a = P - powa[halfN - (j << k)]; ***/
      a = powa[halfN - (j << k)];
#endif
      qi = ++qis,  qo = ++qos;
      for ( i = n; i-- > 0; qi += istep, qo += ostep ) {
  t0 = qi[0],  tn = qi[idist];
  if ( tn == (ModNum)0 ) qo[0] = qo[istep] = t0;
  else {
    AxBmodP( tn, ModNum, tn, a, P, Pinv );
/***    qo[0] = AplusBmodP( t0, tn, P ); ***/
/***    qo[istep] = A_BmodP( t0, tn, P ); ***/
    qo[0] = A_BmodP( t0, tn, P );
    qo[istep] = AplusBmodP( t0, tn, P );
  }
      }
    }
  }
#if 0
 final:
#endif
  /*
   *  The final step of k = 0.  i can take only the value 0.
   *
   *      Out[j]       <= t0 + \alpha^{-j} * tn;
   *      Out[j + 2^{K-1}] <= t0 - \alpha^{-j} * tn;
   *
   *      where t0 = In[j] and tn = In[j + 2^{K-1}],
   *      for 0 <= j < 2^{K-1}.
   *
   */
  qo = out,  qi = &wk[inwk ? N : 0];
  if ( (n = osi + nout) > N ) nout = (n = N) - osi;  /* true nout */
  if ( (k = nout - halfN) <= 0 ) {  /* no overlap, i.e., no +- */
    if ( osi <= 0 ) {
      t0 = qi[0],  tn = qi[idist];
      if ( tn != (ModNum)0 ) t0 = AplusBmodP( t0, tn, P );
      if ( t0 == (ModNum)0 ) qo[0] = t0;
      else {
  AxBmodP( qo[0], ModNum, t0, Ninv, P, Pinv );
      }
#ifdef POWA_STRIDE
      aj = a1;
#endif
      j = 1,  k = n,  i = 0,  qo += o1;
    } else {
      j = osi;
#ifdef POWA_STRIDE
      aj = osi*a1;
#endif
      if ( n <= halfN ) i = 0, k = n;    /* lower only */
      else if ( j == halfN ) goto L_halfN;  /* start with j = N/2 */
      else if ( j > halfN ) goto L_upper;  /* upper only */
      else i = n - halfN + 1, k = halfN;  /* we have lower, and i upper */
    }
  } else {
    os = o1*halfN;
#ifdef EBUG
    fprintf( stderr, "::::: DFT^{-1} ::: os=%d*%d=%d\n", o1, halfN, os );
#endif
    if ( osi <= 0 ) {
      t0 = qi[0],  tn = qi[idist];
      if ( tn == (ModNum)0 ) {
  if ( t0 == (ModNum)0 ) tt = t0;
  else { AxBmodP( tt, ModNum, t0, Ninv, P, Pinv ); }
  qo[0] = qo[os] = tt;
      } else {
  tt = AplusBmodP( t0, tn, P );
  if ( tt == (ModNum)0 ) qo[0] = tt;
  else { AxBmodP( qo[0], ModNum, tt, Ninv, P, Pinv ); }
  tt = A_BmodP( t0, tn, P );
  if ( tt == (ModNum)0 ) qo[os] = tt;
  else { AxBmodP( qo[os], ModNum, tt, Ninv, P, Pinv ); }
      }
#ifdef EBUG
    fprintf( stderr, "::::: DFT^{-1} ::: [0]=%d, [%d]=%d\n", (int)qo[0], os, (int)qo[os] );
#endif
#ifdef POWA_STRIDE
      j = 1,  aj = a1, n = halfN,  i = 0,  qo += o1,  k--;
    } else j = osi, aj = osi*a1,  i = (n -= k) - halfN;
    /**/
    for ( ; k-- > 0; aj += a1, j++, qo += o1 ) {
#else
      j = 1,  n = halfN,  i = 0,  qo += o1,  k--;
    } else j = osi,  i = (n -= k) - halfN;
    /**/
    for ( ; k-- > 0; j++, qo += o1 ) {
#endif
      t0 = qi[j],  tn = qi[j+idist];
      if ( tn == (ModNum)0 ) {
  if ( t0 != (ModNum)0 ) {
    AxBmodP( t0, ModNum, t0, Ninv, P, Pinv );
  }
  qo[0] = qo[os] = t0;
      } else {
#ifdef POWA_STRIDE
/***  a = P - powa[halfNa - aj]; ***/
  a = powa[halfNa - aj];
#else
/***  a = P - powa[halfN - j]; ***/
  a = powa[halfN - j];
#endif
  AxBmodP( tn, ModNum, tn, a, P, Pinv );
/***  tt = AplusBmodP( t0, tn, P ); ***/
  tt = A_BmodP( t0, tn, P );
  if ( tt == (ModNum)0 ) qo[0] = tt;
  else { AxBmodP( qo[0], ModNum, tt, Ninv, P, Pinv ); }
/***  tt = A_BmodP( t0, tn, P ); ***/
  tt = AplusBmodP( t0, tn, P );
  if ( tt == (ModNum)0 ) qo[os] = tt;
  else { AxBmodP( qo[os], ModNum, tt, Ninv, P, Pinv ); }
      }
    }
    k = halfN;
  }
  /*
   *  At this point, k is an upper limit < N/2 for j, i is the number of j's
   *  > N/2, and n is the real upper limit of j.
   */
#ifdef POWA_STRIDE
  for ( ; j < k; aj += a1, j++, qo += o1 ) {
#else
  for ( ; j < k; j++, qo += o1 ) {
#endif
    t0 = qi[j],  tn = qi[j+idist];
    if ( tn == (ModNum)0 ) {
      if ( t0 == (ModNum)0 ) qo[0] = t0;
      else { AxBmodP( qo[0], ModNum, t0, Ninv, P, Pinv ); }
    } else {
#ifdef POWA_STRIDE
      a = P - powa[halfNa - aj];
#else
      a = P - powa[halfN - j];
#endif
      AxBplusCmodP( tt, ModNum, tn, a, t0, P, Pinv );
      if ( tt == (ModNum)0 ) qo[0] = tt;
      else { AxBmodP( qo[0], ModNum, tt, Ninv, P, Pinv ); }
    }
  }
  if ( i <= 0 ) return;
  /**/
 L_halfN:
  t0 = qi[0],  tn = qi[idist];
  tt = tn == (ModNum)0 ? t0 : A_BmodP( t0, tn, P );
  if ( tt == (ModNum)0 ) qo[0] = tt;
  else { AxBmodP( qo[0], ModNum, tt, Ninv, P, Pinv ); }
  qo += o1,  j++;
  /**/
 L_upper:
#ifdef POWA_STRIDE
  for ( n -= halfN, aj = a1, j = 1; j < n; aj += a1, j++, qo += o1 ) {
#else
  for ( n -= halfN, j = 1; j < n; j++, qo += o1 ) {
#endif
    t0 = qi[j],  tn = qi[j+idist];
    if ( tn == (ModNum)0 ) {
      if ( t0 == (ModNum)0 ) qo[0] = t0;
      else { AxBmodP( qo[0], ModNum, t0, Ninv, P, Pinv ); }
    } else {
#ifdef POWA_STRIDE
      a = powa[halfNa - aj];
#else
      a = powa[halfN - j];
#endif
      AxBplusCmodP( tt, ModNum, tn, a, t0, P, Pinv );
      if ( tt == (ModNum)0 ) qo[0] = tt;
      else { AxBmodP( qo[0], ModNum, tt, Ninv, P, Pinv ); }
    }
  }
}

#if defined(USE_FLOAT)
void C_PREP_ALPHA( r, log2ord, log2k, n, tbl, P, Pinv )
ModNum r, tbl[], P;
int log2ord, log2k, n;
double Pinv;
/*
 *  Let K and k denote log2ord and log2k, respectively.
 *  Let r be a primitive (2^K)-th root of unity in Z/(P), where P is a prime.
 *  Compute a = r^{2^{K-k}}, a primitive (2^k)-th root of unity, and
 *  its powers to the degrees upto n in tbl[*].
 */
{
  int i;
  ModNum *q;
  double t, w, dp = (double) P;

  for ( w = (double) r, i = log2ord - log2k; i > 0; i-- ) {
    w *= w;
    if ( w < dp ) continue;
    w -= (dp*(double)((int) (w*Pinv)));
  }
  tbl[0] = 1;
  tbl[1] = (ModNum)(t = w);
  for ( q = &tbl[2], i = n - 1; i > 0; i-- ) {
    t *= w;
    if ( t >= dp ) t -= (dp*(double)((int) (t*Pinv)));
    *q++ = t;
  }
}
#else

void C_PREP_ALPHA( r, log2ord, log2k, n, tbl, P, Pinv )
ModNum r, tbl[], P;
int log2ord, log2k, n;
double Pinv;
/*
 *  Let K and k denote log2ord and log2k, respectively.
 *  Let r be a primitive (2^K)-th root of unity in Z/(P), where P is a prime.
 *  Compute a = r^{2^{K-k}}, a primitive (2^k)-th root of unity, and
 *  its powers to the degrees upto n in tbl[*].
 */
{
  int i;
  ModNum *q;
  ModNum t, w, s;

  for ( w = r, i = log2ord - log2k; i > 0; i-- ) {
    AxBmodP( t, ModNum, w, w, P, Pinv );
    w = t;
  }
  tbl[0] = 1;
  tbl[1] = (ModNum)(t = w);
  for ( q = &tbl[2], i = n - 1; i > 0; i-- ) {
    AxBmodP( s, ModNum, t, w, P, Pinv );
    t = s;
    *q++ = t;
  }
}
#endif
