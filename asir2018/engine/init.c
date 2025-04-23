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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/init.c,v 1.4 2018/10/01 05:49:06 noro Exp $
*/
#include "ca.h"
#include "parse.h"
#include "version.h"
#include "prime.h"

struct oZ oUNIQ,oTWOQ,oTHREEQ,oFOURQ,oFIVEQ,oSIXQ,oSEVENQ,oEIGHTQ;
struct oLM oUNILM,oTHREELM,oFOURLM,oEIGHTLM;

struct oUP2 oONEUP2;

Z ONE = &oUNIQ;
Z TWO = &oTWOQ;
Z THREE = &oTHREEQ;
Z FOUR = &oFOURQ;
Z FIVE = &oFIVEQ;
Z SIX = &oSIXQ;
Z SEVEN = &oSEVENQ;
Z EIGHT = &oEIGHTQ;

LM ONELM = &oUNILM;
LM THREE_LM = &oTHREELM;
LM FOUR_LM = &oFOURLM;
LM EIGHT_LM = &oEIGHTLM;

struct oR oUNIR;
struct oMQ oUNIMQ;
struct oC oIU;
struct oUSINT oVOID;

MQ ONEM = &oUNIMQ;

UP2 ONEUP2 = &oONEUP2;
C IU = &oIU;
USINT VOIDobj = &oVOID;

int bigfloat;
int evalef = 0;
int nez = 1;
int current_mod = 0;
int StrassenSize = 0;
int outputstyle = 0;
int Itvplot = 33;

static int *lprime;
static int lprime_size;

#define ADDBF addbf
#define SUBBF subbf
#define MULBF mulbf
#define DIVBF divbf
#define PWRBF pwrbf
#define CHSGNBF chsgnbf
#define CMPBF cmpbf

typedef void (*NFUNC)(Num,Num,Num *);
typedef void (*NFUNC2)(Num,Num *);
typedef int (*NFUNCI)(Num,Num);

#if defined(INTERVAL)
int zerorewrite = 0;
int zerorewriteCount = 0;

//			N_Q,  N_R,	N_A,	N_B, N4, N_IP,	N_IDouble,N_IQ, N_IBF, N_C,     N_M    N_LM, N_GF2N,   N_GFPN, N_GFS,  N_GFSN,  N_DA, 	N_GZ, N_GQ, N_PARIB
void (*addnumt[])(Num,Num,Num *) = { addq, addreal, addalg, ADDBF, 0, additvp, additvd, 0, additvf, addcplx, addmi, addlm, addgf2n, addgfpn, addgfs, addgfsn, adddalg };
void (*subnumt[])() = { subq, subreal, subalg, SUBBF, 0, subitvp, subitvd, 0, subitvf, subcplx, submi, sublm, subgf2n, subgfpn, subgfs, subgfsn, subdalg };
void (*mulnumt[])() = { mulq, mulreal, mulalg, MULBF, 0, mulitvp, mulitvd, 0, mulitvf, mulcplx, mulmi, mullm, mulgf2n, mulgfpn, mulgfs, mulgfsn, muldalg };
void (*divnumt[])() = { divq, divreal, divalg, DIVBF, 0, divitvp, divitvd, 0, divitvf, divcplx, divmi, divlm, divgf2n, divgfpn, divgfs, divgfsn, divdalg };
void (*pwrnumt[])() = { pwrq, pwrreal, pwralg, PWRBF, 0, pwritvp, pwritvd, 0, pwritvf, pwrcplx, pwrmi, pwrlm, pwrgf2n, pwrgfpn, pwrgfs, pwrgfsn, pwrdalg };
void (*chsgnnumt[])() = { chsgnq, chsgnreal, chsgnalg, CHSGNBF, 0, chsgnitvp, chsgnitvd, 0, chsgnitvf, chsgncplx, chsgnmi, chsgnlm, chsgngf2n, chsgngfpn, chsgngfs , chsgngfsn, chsgndalg};
int (*cmpnumt[])() = { cmpq, cmpreal, cmpalg, CMPBF, 0, cmpitvp, cmpitvd, 0, cmpitvf, cmpcplx, cmpmi, cmplm, cmpgf2n, cmpgfpn, cmpgfs, cmpgfsn, cmpdalg };
#else
//			N_Q,  N_R,	N_A,	N_B,   N_C,     N_M,   N_LM, N_GF2N,   N_GFPN, N_GFS,  N_GFSN,  N_DA, 	N_GZ, N_GQ, N_PARIB
NFUNC addnumt[] = { (NFUNC)addq, (NFUNC)addreal, (NFUNC)addalg, (NFUNC)ADDBF, (NFUNC)addcplx, (NFUNC)addmi, (NFUNC)addlm, (NFUNC)addgf2n, (NFUNC)addgfpn, (NFUNC)addgfs, (NFUNC)addgfsn, (NFUNC)adddalg };
NFUNC subnumt[] = { (NFUNC)subq, (NFUNC)subreal, (NFUNC)subalg, (NFUNC)SUBBF, (NFUNC)subcplx, (NFUNC)submi, (NFUNC)sublm, (NFUNC)subgf2n, (NFUNC)subgfpn, (NFUNC)subgfs, (NFUNC)subgfsn, (NFUNC)subdalg };
NFUNC mulnumt[] = { (NFUNC)mulq, (NFUNC)mulreal, (NFUNC)mulalg, (NFUNC)MULBF, (NFUNC)mulcplx, (NFUNC)mulmi, (NFUNC)mullm, (NFUNC)mulgf2n, (NFUNC)mulgfpn, (NFUNC)mulgfs, (NFUNC)mulgfsn, (NFUNC)muldalg };
NFUNC divnumt[] = { (NFUNC)divq, (NFUNC)divreal, (NFUNC)divalg, (NFUNC)DIVBF, (NFUNC)divcplx, (NFUNC)divmi, (NFUNC)divlm, (NFUNC)divgf2n, (NFUNC)divgfpn, (NFUNC)divgfs, (NFUNC)divgfsn, (NFUNC)divdalg };
NFUNC pwrnumt[] = { (NFUNC)pwrq, (NFUNC)pwrreal, (NFUNC)pwralg, (NFUNC)PWRBF, (NFUNC)pwrcplx, (NFUNC)pwrmi, (NFUNC)pwrlm, (NFUNC)pwrgf2n, (NFUNC)pwrgfpn, (NFUNC)pwrgfs, (NFUNC)pwrgfsn, (NFUNC)pwrdalg };
NFUNC2 chsgnnumt[] = { (NFUNC2)chsgnq, (NFUNC2)chsgnreal, (NFUNC2)chsgnalg, (NFUNC2)CHSGNBF, (NFUNC2)chsgncplx, (NFUNC2)chsgnmi, (NFUNC2)chsgnlm, (NFUNC2)chsgngf2n, (NFUNC2)chsgngfpn, (NFUNC2)chsgngfs, (NFUNC2)chsgngfsn, (NFUNC2)chsgndalg };
NFUNCI cmpnumt[] = { (NFUNCI)cmpq, (NFUNCI)cmpreal, (NFUNCI)cmpalg, (NFUNCI)CMPBF, (NFUNCI)cmpcplx, (NFUNCI)cmpmi, (NFUNCI)cmplm, (NFUNCI)cmpgf2n, (NFUNCI)cmpgfpn, (NFUNCI)cmpgfs, (NFUNCI)cmpgfsn, (NFUNCI)cmpdalg };
#endif

double get_current_time();
void init_lprime();

void nglob_init() {
  oONEUP2.w = 1; oONEUP2.b[0] = 1;

  STOZ0(1,&oUNIQ);
  STOZ0(2,&oTWOQ);
  STOZ0(3,&oTHREEQ);
  STOZ0(4,&oFOURQ);
  STOZ0(5,&oFIVEQ);
  STOZ0(6,&oSIXQ);
  STOZ0(7,&oSEVENQ);
  STOZ0(8,&oEIGHTQ);

  STOLM0(1,&oUNILM);
  STOLM0(3,&oTHREELM);
  STOLM0(4,&oFOURLM);
  STOLM0(8,&oEIGHTLM);

  OID(&oUNIR) = O_R; NM(&oUNIR) = (P)&oUNIQ; DN(&oUNIR) = (P)&oUNIQ; oUNIR.reduced = 1;
  OID(&oUNIMQ) = O_N; NID(&oUNIMQ) = N_M; CONT(&oUNIMQ) = 1;
  OID(&oIU) = O_N; NID(&oIU) = N_C; oIU.r = 0; oIU.i = (Num)ONE;

  OID(&oVOID) = O_VOID;

/* moved to parse/main.c */
#if 0
#if defined(PARI)
  risa_pari_init();
#endif
  srandom((int)get_current_time());
#endif
  init_up2_tab();

  init_lprime();
  init_gmpq();
}

double suspend_start;
double suspended_time=0;

void get_eg(struct oEGT *p)
{
  p->exectime = get_clock()-suspended_time; p->gctime = 0;
}

void init_eg(struct oEGT *eg)
{
  bzero((char *)eg,sizeof(struct oEGT));
}

void add_eg(struct oEGT *base,struct oEGT *start,struct oEGT *end)
{
  base->exectime += end->exectime - start->exectime;
}

void print_eg(char *item,struct oEGT *eg)
{
  printf("%s=(%.4g)",item,eg->exectime);
}

void print_split_eg(struct oEGT *start,struct oEGT *end)
{
  struct oEGT base;

  init_eg(&base); add_eg(&base,start,end);
  printf("(%.4g)",base.exectime);
}

void print_split_e(struct oEGT *start,struct oEGT *end)
{
  struct oEGT base;

  init_eg(&base); add_eg(&base,start,end);
  printf("(%.4g)",base.exectime);
}

void suspend_timer() {
  suspend_start = get_clock();
}

void resume_timer() {
  suspended_time += get_clock()-suspend_start;
}

extern int lm_lazy, up_lazy;
extern int GC_dont_gc;
extern int do_weyl;
extern int dp_fcoeffs;

void reset_engine() {
  lm_lazy = 0;
  up_lazy = 0;
  do_weyl = 0;
  dp_fcoeffs = 0;
  GC_dont_gc = 0;
}

unsigned int get_asir_version() {
  return ASIR_VERSION;
}

char *get_asir_distribution() {
  return ASIR_DISTRIBUTION;
}


void create_error(ERR *err,unsigned int serial,char *msg,LIST trace)
{
  int len;
  USINT ui,notsupp;
  NODE n,n1;
  LIST list;
  char *msg1;
  STRING errmsg;

  MKUSINT(ui,serial);
  MKUSINT(notsupp,-1);
  len = strlen(msg);
  msg1 = (char *)MALLOC(len+1);
  strcpy(msg1,msg);
  MKSTR(errmsg,msg1);
  if ( !trace )
    MKLIST(trace,0);
  n = mknode(4,ui,notsupp,errmsg,trace); MKLIST(list,n);
  MKERR(*err,list);
}

static mp_limb_t *lprime64;
static int lprime64_size;

void init_lprime()
{
  int s,i;

  s = sizeof(lprime_init);
  lprime = (int *)MALLOC_ATOMIC(s);
  lprime_size = s/sizeof(int);  
  for ( i = 0; i < lprime_size; i++ )
    lprime[i] = lprime_init[lprime_size-i-1];

#if SIZEOF_LONG == 8
  s = sizeof(lprime64_init);
  lprime64 = (mp_limb_t *)MALLOC_ATOMIC(s);
  lprime64_size = s/sizeof(mp_limb_t);  
  for ( i = 0; i < lprime64_size; i++ )
    lprime64[i] = lprime64_init[lprime64_size-i-1];
#endif
}

void create_new_lprimes(int);
void create_new_lprimes64(int);

int get_lprime(int index)
{
  if ( index >= lprime_size )
    create_new_lprimes(index);
  return lprime[index];
}

#if SIZEOF_LONG == 8
mp_limb_t get_lprime64(int index)
{
  if ( index >= lprime64_size )
    create_new_lprimes64(index);
  return lprime64[index];
}
#endif

void create_new_lprimes(int index)
{
  int count,p,i,j,d;

  if ( index < lprime_size )
    return;
  count = index-lprime_size+1;
  if ( count < 256 )
    count = 256;
  lprime = (int *)GC_realloc(lprime,(lprime_size+count)*sizeof(int));
  p = lprime[lprime_size-1]+2;
  for ( i = 0; i < count; p += 2 ) {
    for ( j = 0; (d = sprime[j]) != 0; j++ ) {
      if ( d*d > p ) {
        lprime[i+lprime_size] = p;
        i++;
        break;
      }
      if ( !(p%d) )
        break;
    }
  }
  lprime_size += count;
}

mp_limb_t nextprime(mp_limb_t a)
{
  static FUNC f=0;
  Z z,r;

  if ( f == 0 ) mkparif("nextprime",&f);
  STOZ(a,z); 
  r = (Z)evalparif(f,mknode(1,z));
  return ZTOS(r);
}

void create_new_lprimes64(int index)
{
  int count,j;
  mp_limb_t p;

  if ( index < lprime64_size )
    return;
  count = index-lprime64_size+1;
  if ( count < 256 )
    count = 256;
  lprime64 = (mp_limb_t *)GC_realloc(lprime64,(lprime64_size+count)*sizeof(mp_limb_t));
  for ( p = lprime64[lprime64_size-1], j = 0; j < count; j++ ) {
    p = nextprime(p+1);
    lprime64[lprime64_size+j] = p;
  }
  lprime64_size += count;
}

#define MAXTHREADS 64

#if defined(VISUAL)
#if 0
void *thread_args[MAXTHREADS];
static HANDLE thread[MAXTHREADS];
static CRITICAL_SECTION work_mutex;
static CONDITION_VARIABLE work_cond,finish_cond;
int generic_thread,thread_working;
static WORKER_FUNC worker_func;

static void notify_finish()
{
  //fprintf(stderr,"finish\n");
  EnterCriticalSection(&work_mutex);
  thread_working--;
  if ( thread_working == 0 )
    WakeConditionVariable(&finish_cond);
  LeaveCriticalSection(&work_mutex);
}

static DWORD WINAPI thread_worker(LPVOID arg)
{
  int id = (int)arg;
  //fprintf(stderr,"start worker %d\n",id);
  while ( 1 ) {
    EnterCriticalSection(&work_mutex);
    SleepConditionVariableCS(&work_cond,&work_mutex,INFINITE);
    LeaveCriticalSection(&work_mutex);
    //fprintf(stderr,"got event %d\n",id);
    if ( thread_args[id] != 0 ) {
      (*worker_func)(thread_args[id]);
      //fprintf(stderr,"end %d\n",id);
      notify_finish();
    }
  }
}

static void init_threads(int n)
{
  int i,ret;
  static int current_threads;
  DWORD dwThreadID;

  if ( current_threads == 0 ) {
    InitializeCriticalSection(&work_mutex);
    InitializeConditionVariable(&work_cond);
    InitializeConditionVariable(&finish_cond);
  }
  for ( i = n; i < current_threads; i++ ) thread_args[i] = 0;
  if ( current_threads >= n ) return;
  for ( i = current_threads; i < n; i++ ) {
    thread[i] = GC_CreateThread(NULL,0,thread_worker,(LPVOID)i,0,&dwThreadID);
    if ( thread[i] == NULL )
      error("init_threads : failed to create thread");
  }
  current_threads = n;
  Sleep(10); // XXX : should be corrected
}

void execute_worker(int nworker,WORKER_FUNC func)
{
  init_threads(nworker);
  EnterCriticalSection(&work_mutex);
  thread_working = nworker;
  worker_func = func;
  LeaveCriticalSection(&work_mutex);
  WakeAllConditionVariable(&work_cond);
  while ( 1 ) {
    if ( thread_working != 0 ) {
      EnterCriticalSection(&work_mutex);
      SleepConditionVariableCS(&finish_cond,&work_mutex,INFINITE);
      LeaveCriticalSection(&work_mutex);
    } else
      break;
  }
}

void create_and_execute_worker(int nworker,WORKER_FUNC func)
{
  int i,ret;
  DWORD dwThreadID;

  if ( nworker > MAXTHREADS )
    error("create_and_execute_worker : too many threads");
  if ( generic_thread != 0 ) {
    execute_worker(nworker,func);
    return;
  }
  for ( i = 0; i < nworker; i++ ) {
    thread[i] = GC_CreateThread(NULL,0,func,(LPVOID)thread_args[i],0,&dwThreadID);
    if ( thread[i] == NULL )
      error("create_and_execute_worker : failed to create thread");
  }
  WaitForMultipleObjects(nworker,thread,TRUE,INFINITE);
}
#endif
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

typedef int SOCKPAIR[2];

int generic_thread,thread_working;
void *thread_args[MAXTHREADS];
static SOCKPAIR sockpair[MAXTHREADS];
static pthread_t thread[MAXTHREADS];
static WORKER_FUNC worker_func;
static int current_threads;

static void thread_worker(int *idptr)
{
  int id = *idptr;
  char c;

  while ( 1 ) {
    read(sockpair[id][1],&c,1);
    (*worker_func)(thread_args[id]);
    write(sockpair[id][1],&c,1);
  }
}

static void init_threads(int n)
{
  int i,ret;

  if ( n > MAXTHREADS )
    error("init_threads : too many threads");
  for ( i = n; i < current_threads; i++ ) thread_args[i] = 0;
  if ( current_threads >= n ) return;
  for ( i = current_threads; i < n; i++ ) {
    if ( socketpair(AF_UNIX,SOCK_STREAM,0,(int *)&sockpair[i]) < 0 )
      error("init_threads : failed to create socketpair");
    ret = pthread_create(&thread[i],NULL,(void *)thread_worker,(void *)&i);
    if ( ret != 0 )
      error("init_threads : failed to create thread");
  }
  current_threads = n;
}

void check_caught_intr();

void execute_worker(int nworker,WORKER_FUNC func)
{
  char c;
  int i;

  init_threads(nworker);
  thread_working = nworker;
  worker_func = func;
  for ( i = 0; i < nworker; i++ )
    write(sockpair[i][0],&c,1);
  for ( i = 0; i < nworker; i++ )
    read(sockpair[i][0],&c,1);
  thread_working = 0;
  check_caught_intr();
}

void create_and_execute_worker(int nworker,WORKER_FUNC func)
{
  int i,ret;
  void *status;

  if ( nworker > MAXTHREADS )
    error("create_and_execute_worker : too many threads");
  if ( generic_thread != 0 ) {
    execute_worker(nworker,func);
    return;
  }
  for ( i = 0; i < nworker; i++ ) {
    ret = pthread_create(&thread[i],NULL,func,thread_args[i]);
    if ( ret != 0 )
      error("create_and_execute_worker : failed to create thread"); 
  }
  thread_working = nworker;
  for ( i = 0; i < nworker; i++ ) {
    ret = pthread_join(thread[i],&status);
    if ( ret != 0 )
      error("create_and_execute_worker : failed to join thread"); 
  }
  thread_working = 0;
  check_caught_intr();
}
#endif /* VISUAL */
