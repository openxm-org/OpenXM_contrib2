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
 * e-mail at risa-admin@flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/fft/polmul.c,v 1.1.1.1 1999/12/03 07:39:08 noro Exp $ 
*/
#include "dft.h"
extern struct PrimesS Primes[];

/*
#define TIMING
*/

#ifdef TIMING

#define MAXTIMING 20

#include <sys/time.h>
#include <sys/resource.h>
#if 0
#if define(hitm)
#include <machine/xclock.h>
#endif
#endif

static struct rusage ru_time[MAXTIMING];

struct timing {
  struct timeval user, sys;
};
static struct timing time_duration[MAXTIMING];

#define RECORD_TIME(i) getrusage( RUSAGE_SELF, &ru_time[i] )
#endif /* TIMING */

void FFT_primes(Num, p_prime, p_primroot, p_d)
int Num, *p_prime, *p_primroot, *p_d;
{
  *p_prime =  Primes[Num].prime;
  *p_primroot =  Primes[Num].primroot;
  *p_d = Primes[Num].d;
}    

int FFT_pol_square( d1, C1, Prod,  MinMod, wk)
unsigned int d1;
int MinMod;
unsigned int C1[], Prod[], wk[];
{

  unsigned int  Low0bits;
  unsigned int Proot, Prime;
  double Pinv;
  void MNpol_square_DFT();

   if ( MinMod < 0  || MinMod > NPrimes - 1 ) return 2;
   Prime =  Primes[MinMod].prime;
   Proot =  Primes[MinMod].primroot;
   Low0bits = Primes[MinMod].d;
   Pinv = (((double)1.0)/((double)Prime));

    MNpol_square_DFT( d1, C1, Prod, Proot, Low0bits, Prime, Pinv, wk );

    return 0;
}

int FFT_pol_product( d1, C1, d2, C2, Prod,  MinMod, wk)
unsigned int d1, d2;
int MinMod;
unsigned int C1[], C2[], Prod[], wk[];
{

  unsigned int  Low0bits;
  unsigned int Proot, Prime;
  double Pinv;
  void MNpol_product_DFT();

   if ( MinMod < 0  || MinMod > NPrimes - 1 ) return 2;
   Prime =  Primes[MinMod].prime;
   Proot =  Primes[MinMod].primroot;
   Low0bits = Primes[MinMod].d;
   Pinv = (((double)1.0)/((double)Prime));
   MNpol_product_DFT( d1, C1, d2, C2, Prod, Proot, Low0bits, Prime, Pinv, wk );
   return 0;
}

struct oEGT {
    double exectime,gctime;
};

extern struct oEGT eg_fore,eg_back;

void get_eg(struct oEGT *);
void add_eg(struct oEGT *,struct oEGT *, struct oEGT *);

void MNpol_product_DFT( d1, C1, d2, C2, Prod, a, low0s, P, Pinv, wk )
unsigned int d1, d2, low0s;
unsigned int C1[], C2[], Prod[], a, P, wk[];
double Pinv;
/*
 *  The amount of space of wk[] must be >= (11/2)*2^{\lceil \log_2(d1+d2+1) \rceil}.
 */
{
  int i, d, n, log2n, halfn;
  unsigned int *dft1, *dft2, *dftprod, *powa, *true_wk, ninv;
  struct oEGT eg0,eg1;

  d = d1 + d2;
  /**/
  for ( n = 2, log2n = 1; n <= d; n <<= 1) log2n++;
  halfn = n >> 1;
  dft1 = wk,  dft2 = &wk[n];
  dftprod = &dft2[n];
  powa = &dftprod[n];
  true_wk = &powa[halfn];	/* 2*n areas */
  /**/
  C_PREP_ALPHA( a, low0s, log2n, halfn -1, powa, P, Pinv );
#ifdef TIMING
  RECORD_TIME(1);
#endif
  /**/
  get_eg(&eg0);
  C_DFT_FORE( C1, d1 + 1, 1, log2n, powa, dft1, 1, P, Pinv, true_wk );
#ifdef TIMING
  RECORD_TIME(2);
#endif
  C_DFT_FORE( C2, d2 + 1, 1, log2n, powa, dft2, 1, P, Pinv, true_wk );
  get_eg(&eg1); add_eg(&eg_fore,&eg0,&eg1);
#ifdef TIMING
  RECORD_TIME(3);
#endif
  /**/
  for ( i = 0; i < n; i++ ) {
    AxBmodP( dftprod[i], unsigned int, dft1[i], dft2[i], P, Pinv );
  }
#ifdef TIMING
  RECORD_TIME(4);
#endif
  /**/
  ninv = P - (unsigned int)(((int)P) >> log2n);
  get_eg(&eg0);
  C_DFT_BACK( dftprod, n, 1, log2n, powa, Prod, 1, 0, d + 1, ninv, P, Pinv, true_wk );
  get_eg(&eg1); add_eg(&eg_back,&eg0,&eg1);
}

void MNpol_square_DFT( d1, C1, Prod, a, low0s, P, Pinv, wk )
unsigned int d1, low0s;
unsigned int C1[], Prod[], a, P, wk[];
double Pinv;
/*
 *  The amount of space of wk[] must be >= (11/2)*2^{\lceil \log_2(d1+d2+1) \rceil}.
 */
{
  int i, d, n, log2n, halfn;
  unsigned int *dft1, *dft2, *dftprod, *powa, *true_wk, ninv;
  struct oEGT eg0,eg1;

  d = d1 + d1;
  /**/
  for ( n = 2, log2n = 1; n <= d; n <<= 1) log2n++;
  halfn = n >> 1;
  dft1 = wk,  dft2 = &wk[n];
  dftprod = &dft2[n];
  powa = &dftprod[n];
  true_wk = &powa[halfn];	/* 2*n areas */
  /**/
  C_PREP_ALPHA( a, low0s, log2n, halfn -1, powa, P, Pinv );
#ifdef TIMING
  RECORD_TIME(1);
#endif
  /**/
  get_eg(&eg0);
  C_DFT_FORE( C1, d1 + 1, 1, log2n, powa, dft1, 1, P, Pinv, true_wk );
  get_eg(&eg1); add_eg(&eg_fore,&eg0,&eg1);
  /**/
  for ( i = 0; i < n; i++ ) {
    AxBmodP( dftprod[i], unsigned int, dft1[i], dft1[i], P, Pinv );
  }
#ifdef TIMING
  RECORD_TIME(4);
#endif
  /**/
  ninv = P - (unsigned int)(((int)P) >> log2n);
  get_eg(&eg0);
  C_DFT_BACK( dftprod, n, 1, log2n, powa, Prod, 1, 0, d + 1, ninv, P, Pinv, true_wk );
  get_eg(&eg1); add_eg(&eg_back,&eg0,&eg1);
}

/******************  TEST CODE ****************/

/*
#define TEST
*/
#ifdef TEST   

#define RETI 100

#include <stdio.h>

#define Prime 65537    /* 2^16 + 1 */
#define Low0bits 16
#define PrimRoot 3

#define PInv (((double)1.0)/((double)Prime))

#define MaxNPnts (1<<Low0bits)

static unsigned int Pol1[MaxNPnts], Pol2[MaxNPnts], PolProdDFT[MaxNPnts], PolProd[MaxNPnts], wk[6*MaxNPnts];

static void generate_pol(), dump_pol(), compare_vals();

#ifdef TIMING
static void time_spent();

#define pr_timing(pTiming) fprintf(stderr, "user:%3d.%06d + sys:%3d.%06d = %3d.%06d", \
(pTiming)->user.tv_sec,(pTiming)->user.tv_usec, \
(pTiming)->sys.tv_sec,(pTiming)->sys.tv_usec, \
(pTiming)->user.tv_sec+(pTiming)->sys.tv_sec, \
(pTiming)->user.tv_usec+(pTiming)->sys.tv_usec)

#define Pr_timing(i) pr_timing(&time_duration[i])
#endif /* TIMING */
void main( argc, argv )
int argc;
char **argv;
{
  int i, ac = argc - 1, d1, d2, d;

  int ii, *error;

  char **av = &argv[1];
  unsigned int mnP = (unsigned int)Prime, mnProot = (unsigned int)PrimRoot;
  double Pinv = PInv;

  for ( ; ac >= 2; ac -= 2, av += 2 ) {
    d = (d1 = atoi( av[0] )) + (d2 = atoi( av[1] ));
    if ( d1 <= 0 || d2 <= 0 || d >= MaxNPnts ) {
      fprintf( stderr, "*** invalid degrees %s & %s. (sum < %d)\n",
	       av[0], av[1], MaxNPnts );
      continue;
    }
    generate_pol( d1, Pol1 );
/*    dump_pol( d1, Pol1 );   */
    generate_pol( d2, Pol2 );
/*  dump_pol( d2, Pol2 );    */
    /**/



    for ( ii=0; ii < RETI; ii++ ){
#ifdef TIMING
    RECORD_TIME(0);
#endif
    MNpol_product_DFT( d1, Pol1, d2, Pol2, PolProdDFT, mnProot, Low0bits, mnP, Pinv, wk );
#ifdef TIMING
    RECORD_TIME(5);
    time_spent( 0, 1,  0 );
    time_spent( 1, 2,  1 );
    time_spent( 2, 3,  2 );
    time_spent( 3, 4,  3 );
    time_spent( 4, 5,  4 );
    time_spent( 0, 5,  5 );
#endif
    }


fprintf( stderr, "DFT mul done ( %d times )\n", RETI );





    for ( ii=0; ii < RETI; ii++ ){
#ifdef TIMING
    RECORD_TIME(10);
#endif
    MNpol_product( d1, Pol1, d2, Pol2, PolProd, mnP, Pinv );
#ifdef TIMING
    RECORD_TIME(11);
    time_spent( 10, 11, 10 );
#endif
    }



fprintf( stderr, "mul by classical alg. done ( %d times )\n", RETI );
    /**/


     /*     PolProdDFT[20] = 0.0;    */


    compare_vals( PolProdDFT, PolProd, d+1 , error);
    if ( *error == 0)
        fprintf( stderr, "******* Result is OK *******\n" );
    else 
        fprintf( stderr, "******* Result is NG *******\n" );

    /**/
#ifdef TIMING
    fprintf( stderr, "DFT     mul prep: " );  Pr_timing( 0 );
    fprintf( stderr, "\nDFT  transform 1: " );  Pr_timing( 1 );
    fprintf( stderr, "\nDFT  transform 2: " );  Pr_timing( 2 );
    fprintf( stderr, "\nDFT         mult: " );  Pr_timing( 3 );
    fprintf( stderr, "\nDFT    inv trans: " );  Pr_timing( 4 );
    fprintf( stderr, "\nDFT pol-mult Tot: " );  Pr_timing( 5 );
    /**/
    fprintf( stderr, "\nClassical mult:   " );  Pr_timing( 10 );
    fprintf( stderr, "\n" );
#endif /* TIMING */
  }
}

static void generate_pol( d, cf )
int d;
unsigned int cf[];
{
  unsigned int mnP = (unsigned int)Prime;
  int i, c;

  for ( i = 0; i < d; i++ ) cf[i] = random() % Prime;
  while ( (c = random() % Prime) == 0 ) ;
  cf[d] = c;
}

static void dump_pol( d, cf )
int d;
unsigned int cf[];
{
  int i;

  printf( "Pol of degree %d over Z/(%d) :\n", d, Prime );
  for ( i = 0; i <= d; i++ ) {
    if ( (i%5) == 0 ) printf( "\t" );
    printf( "%10d,", (int)cf[i] );
    if ( (i%5) == 4 ) printf( "\n" );
  }
  if ( (i%5) != 0 ) printf( "\n" );
}

void compare_vals( v1, v2, n , error)
unsigned int v1[], v2[];
int n;
int *error;
{
  int i, tmp;

  *error = 0;

  for ( i = 0; i < n; i ++) {
        tmp = (int)v1[i] - (int)v2[i];
        tmp = abs(tmp);
        if ( tmp > *error ) *error = tmp;
  }        



/*
  int i, j;



  for ( i = 0; i < n; i += 5 ) {
    printf( " %6d:%10d", i, (int)v1[i] );
    for ( j = 1; j < 5 && i + j < n; j++ ) printf( ",%10d", (int)v1[i+j] );
    printf( "\n" );
    if ( (int)v1[i] == (int)v2[i] && (int)v1[i+1] == (int)v2[i+1]
	 && (int)v1[i+2] == (int)v2[i+2] && (int)v1[i+3] == (int)v2[i+3]
	 && (int)v1[i+4] == (int)v2[i+4] ) continue;
    printf( "        " );
    for ( j = 0; j < 5 && i + j < n; j++ )
      if ( (int)v1[i+j] == (int)v2[i+j] ) printf( "           " );
      else printf( "%10d,", (int)v2[i+j] );
    printf( "\n" );
  }
*/



}

#ifdef TIMING

static void time_spent( s, e, n )
int s, e, n;
{
  struct rusage *rus = &ru_time[s], *rue = &ru_time[e];
  struct timeval *tms, *tme;
  long sec, usec;
  struct timing *pt = &time_duration[n];

  tms = &rus->ru_utime,  tme = &rue->ru_utime;
  sec = tme->tv_sec - tms->tv_sec,
  usec = tme->tv_usec - tms->tv_usec;
  if ( usec < 0 ) usec += 1000000, sec--;
/*  pt->user.tv_sec = sec,  pt->user.tv_usec = usec;   */
  pt->user.tv_sec += sec,  pt->user.tv_usec += usec;



  if ( pt->user.tv_usec > 999999 ){
  pt->user.tv_usec -= 1000000;
  pt->user.tv_sec++; 
  }



  /**/
  tms = &rus->ru_stime,  tme = &rue->ru_stime;
  sec = tme->tv_sec - tms->tv_sec,
  usec = tme->tv_usec - tms->tv_usec;
  if ( usec < 0 ) usec += 1000000, sec--;
/*  pt->sys.tv_sec = sec,  pt->sys.tv_usec = usec;   */
  pt->sys.tv_sec += sec,  pt->sys.tv_usec += usec;



  if ( pt->sys.tv_usec > 999999 ){
  pt->sys.tv_usec -= 1000000;
  pt->sys.tv_sec++; 
  }


}
#endif /* TIMING */

#endif /* TEST */

void MNpol_product( d1, C1, d2, C2, Prod, P, Pinv )
unsigned int d1, d2;
unsigned int C1[], C2[], Prod[], P;
double Pinv;
{
  unsigned int i, j;
  unsigned int c;

  c = C1[0];
  AxBmodP( Prod[0], unsigned int, c, C2[0], P, Pinv );
  for ( i = 1; i <= d2; i++ ) {
    AxBmodPnostrchk( Prod[i], unsigned int, c, C2[i], P, Pinv );
  }
  c = C1[d1];
  if ( d1 > d2 ) {
    for ( i = d2 + 1; i < d1; i++ ) Prod[i] = (unsigned int)0;
    for ( i = 0; i < d2; i++ ) {
      AxBmodPnostrchk( Prod[i+d1], unsigned int, c, C2[i], P, Pinv );
    }
  } else {
    j = d2 - d1;
    for ( i = 0; i <= j; i++ ) {
      AxBplusCmodPnostrchk( Prod[i+d1], unsigned int, c, C2[i], Prod[i+d1], P, Pinv );
    }
    for ( ; i < d2; i++ ) {
      AxBmodPnostrchk( Prod[i+d1], unsigned int, c, C2[i], P, Pinv );
    }
  }
  AxBmodP( Prod[d1+d2], unsigned int, c, C2[d2], P, Pinv );
  /**/
  for ( j = 1; j < d1 - 1; j++ ) {
    c = C1[j];
    if ( c == (unsigned int)0 ) {
      Prod[j] = AstrictmodP( Prod[j], P );
      continue;
    }
    AxBplusCmodP( Prod[j], unsigned int, c, C2[0], Prod[j], P, Pinv );
    for ( i = 1; i <= d2; i++ ) {
      AxBplusCmodPnostrchk( Prod[i+j], unsigned int, c, C2[i], Prod[i+j], P, Pinv );
    }
  }
  c = C1[j = d1-1];
  for ( i = 0; i <= d2; i++ ) {
    AxBplusCmodP( Prod[i+j], unsigned int, c, C2[i], Prod[i+j], P, Pinv );
  }
}
