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
 * $OpenXM: OpenXM_contrib2/asir2018/io/ox.c,v 1.2 2019/07/25 05:57:55 noro Exp $
*/
#include "ca.h"
#include "parse.h"
#include "signal.h"
#include "wsio.h"
#include "ox.h"

#define ISIZ sizeof(int)

extern Obj VOIDobj;

extern int nserver_102, myrank_102;
extern int ox_need_conv;
int ox_usr1_sent, ox_int_received, critical_when_signal;
unsigned int ox_serial;
int ox_flushing;
int ox_batch;
int ox_check=1;
int ox_exchange_mathcap=1;
JMP_BUF ox_env;

MATHCAP my_mathcap;

struct oxcap {
  unsigned int ox;
  int ncap;
  int *cap;
};

struct mathcap {
  LIST mc;
  unsigned int version;
  char *servername;
  int nsmcap;
  unsigned int *smcap;
  int noxcap;
  struct oxcap *oxcap;
  int no_ox_reset;
};

struct oxcap *my_oxcap;

static struct mathcap my_mc;
static struct mathcap *remote_mc;
static int remote_mc_len;

void mclist_to_mc(LIST mclist,struct mathcap *mc);
Obj asir_pop_one();
void asir_push_one(Obj);

#if defined(VISUAL) || defined(__MINGW32__)
/* XXX : mainly used in engine2000/io.c, but declared here */
typedef void *HANDLE;
HANDLE hStreamNotify,hStreamNotify_Ack;

void cleanup_events()
{
  /* ox_watch_stream may be waiting for hStreamNotify_Ack to be set */

  ResetEvent(hStreamNotify);
  SetEvent(hStreamNotify_Ack);
}
#endif

/* 1 if no_ox_reset, 0 if ox_reset OK, -1 if invalid */
int no_ox_reset(int s)
{
  if ( remote_mc && s >= 0 && s < remote_mc_len )
    return remote_mc[s].no_ox_reset;
  else
    return -1;
}

void ox_resetenv(char *s)
{
#if defined(VISUAL) || defined(__MINGW32__)
  cleanup_events();
#endif
  fprintf(stderr,"%s\n",s);
  LONGJMP(ox_env,1);
}

static int available_cmo[] = {
  CMO_NULL, CMO_INT32, CMO_DATUM, CMO_STRING, CMO_MATHCAP,
  CMO_ERROR, CMO_ERROR2, CMO_LIST, CMO_MONOMIAL32,
  CMO_ZZ, CMO_QQ, CMO_ZERO,
  CMO_DMS_GENERIC, CMO_DMS_OF_N_VARIABLES,
  CMO_RING_BY_NAME, CMO_DISTRIBUTED_POLYNOMIAL,
    CMO_RATIONAL,
  CMO_RECURSIVE_POLYNOMIAL, CMO_UNIVARIATE_POLYNOMIAL,
  CMO_INDETERMINATE,
  CMO_TREE, CMO_BIGFLOAT32, CMO_COMPLEX,
  0
};

static int asir_available_sm[] = {
  SM_dupErrors, SM_getsp, SM_popSerializedLocalObject,
  SM_popCMO, SM_popString, SM_pushCMOtag, SM_setName,
  SM_evalName, SM_executeStringByLocalParser,
  SM_executeStringByLocalParserInBatchMode,
  SM_executeFunction, SM_shutdown, SM_pops,
  SM_mathcap, SM_setMathcap, SM_nop,
  SM_beginBlock, SM_endBlock,
  0
};

static int ox_asir_available_sm[] = {
  SM_dupErrors, SM_getsp, SM_popSerializedLocalObject,
  SM_popCMO, SM_popString, SM_pushCMOtag, SM_setName,
  SM_evalName, SM_executeStringByLocalParser,
  SM_executeStringByLocalParserInBatchMode,
  SM_executeFunction, SM_shutdown, SM_pops,
  SM_mathcap, SM_setMathcap, SM_nop,
  SM_beginBlock, SM_endBlock,  SM_executeFunctionSync,
  SM_set_rank_102, SM_tcp_accept_102, SM_tcp_connect_102, SM_reset_102,
  SM_bcast_102, SM_reduce_102,
  0
};

static int ox_plot_available_sm[] = {
  SM_dupErrors, SM_getsp, SM_popSerializedLocalObject,
  SM_popCMO, SM_popString, SM_setName,
  SM_evalName, SM_executeStringByLocalParser,
  SM_executeFunction, SM_shutdown, SM_pops,
  SM_mathcap, SM_setMathcap, SM_nop,
  0
};

/*
  mathcap = 
    [
      version list,
      SMlist,
      [
        [OX tag,CMO tag list],
        [OX tag,CMO tag list],
        ...
      ]
    ]
*/

void create_my_mathcap(char *system)
{
  NODE n,n0;
  int i,k;
  STRING str;
  LIST sname,smlist,oxlist,cmolist,asirlist,oxasir,r;
  USINT tag,t,t1;

  if ( my_mathcap )
    return;
  /* version */
  MKSTR(str,system);
  MKUSINT(t,OX_VERSION);
  n0 = mknode(2,t,str); MKLIST(sname,n0);

  /* sm tag */
  n0 = 0;
  if ( !strcmp(system,"asir") ) {
    for ( i = 0; k = asir_available_sm[i]; i++ ) {
      NEXTNODE(n0,n); MKUSINT(t,k); BDY(n) = (pointer)t;
    }
  } else if ( !strcmp(system,"ox_asir") ) {
    for ( i = 0; k = ox_asir_available_sm[i]; i++ ) {
      NEXTNODE(n0,n); MKUSINT(t,k); BDY(n) = (pointer)t;
    }
    NEXT(n) = 0;
  } else if ( !strcmp(system,"ox_plot") ) {
    for ( i = 0; k = ox_plot_available_sm[i]; i++ ) {
      NEXTNODE(n0,n); MKUSINT(t,k); BDY(n) = (pointer)t;
    }
    NEXT(n) = 0;
  }
  MKLIST(smlist,n0);

  /* creation of [OX_DATA,CMO list] */
  /* ox tag */
  MKUSINT(tag,OX_DATA);
  /* cmo tag */
  for ( n0 = 0, i = 0; k = available_cmo[i]; i++ ) {
    NEXTNODE(n0,n); MKUSINT(t,k); BDY(n) = (pointer)t;
  }
  NEXT(n) = 0; MKLIST(cmolist,n0);
  /* [ox tag, cmo list] */
  n0 = mknode(2,tag,cmolist);
  MKLIST(oxlist,n0);

  /* creation of [OX_LOCAL_OBJECT_ASIR,ASIR tag] */
  /* ox tag */
  MKUSINT(tag,OX_LOCAL_OBJECT_ASIR);
  /* local tag */
  MKUSINT(t,ASIR_VL);
  MKUSINT(t1,ASIR_OBJ);
  n0 = mknode(2,t,t1); MKLIST(cmolist,n0);
  /* [ox tag, local list] */
  n0 = mknode(2,tag,cmolist);
   MKLIST(asirlist,n0);

  /* [oxlist,asirlist] */
  n0 = mknode(2,oxlist,asirlist); MKLIST(oxasir,n0);

  /* [version,sm,oxasir] */
  n0 = mknode(3,sname,smlist,oxasir); MKLIST(r,n0);

  MKMATHCAP(my_mathcap,r);
  mclist_to_mc(r,&my_mc);
  my_oxcap = my_mc.oxcap;
}

void store_remote_mathcap(int s,MATHCAP mc)
{
  if ( !remote_mc ) {
     remote_mc_len = 16;
     remote_mc = (struct mathcap *)
       CALLOC(remote_mc_len,sizeof(struct mathcap));
  }
  if ( s >= remote_mc_len ) {
    remote_mc_len *= 2;
    remote_mc = (struct mathcap *)REALLOC(remote_mc,
      remote_mc_len*sizeof(struct mathcap));
  }
  mclist_to_mc(BDY(mc),&remote_mc[s]);
}

/*
  mathcap = 
    [
      version list,
      SMlist,
      [
        [OX tag,CMO tag list],
        [OX tag,CMO tag list],
        ...
      ]
    ]

    ===>

  mathcap
    | version | &servername | nsmcap |   &smcap | noxcap | &oxcap |
  smcap
    | SM_xxx | SM_yyy | ... |
  oxcap
    | oxcap[0] | oxcap[1] | ... |
  oxcap[i]
    | ox | ncap | &cap |
  cap
    | CMO_xxx | CMO_yyy | ... |
*/

void mclist_to_mc(LIST mclist,struct mathcap *mc)
{
  int l,i,j;
  NODE n,t,oxcmo,cap;
  int *ptr;

  /*
    [ 
      [ version,servername ]
      [sm1,sm2,...],
      [
        [o1,[n11,n12,...]],
        [o2,[n21,n22,...]],
        ...
      ]
      (optional)[s1,s2,...] (no_ox_reset)
       ]
  */
  n = BDY(mclist);
  mc->mc = mclist;
  mc->version = BDY((USINT)BDY(BDY((LIST)BDY(n))));
  mc->servername = BDY((STRING)BDY(NEXT(BDY((LIST)BDY(n)))));

  /* smcap */
  n = NEXT(n);
  t = BDY((LIST)BDY(n));
  mc->nsmcap = length(t);
  mc->smcap = (int *)MALLOC_ATOMIC(mc->nsmcap*sizeof(int));
  for ( j = 0, ptr = mc->smcap; j < mc->nsmcap; j++, t = NEXT(t) )
    ptr[j] = BDY((USINT)BDY(t));

  n = NEXT(n);
  n = BDY((LIST)BDY(n));
  /* n -> BDY([[OX1,CMOlist1], [OX2,CMOlist2], ...]) */
  mc->noxcap = length(n);
  mc->oxcap = (struct oxcap *)MALLOC(mc->noxcap*sizeof(struct oxcap));
  for ( j = 0; j < mc->noxcap; j++, n = NEXT(n) ) {
    oxcmo = BDY((LIST)BDY(n));
    /* oxcmo = BDY([OXj,CMOlistj]) */
    mc->oxcap[j].ox = BDY((USINT)BDY(oxcmo));
    cap = BDY((LIST)BDY(NEXT(oxcmo)));
    /* cap ->BDY(CMOlistj) */
    l = length(cap);
    mc->oxcap[j].ncap = l;
    mc->oxcap[j].cap = (unsigned int *)CALLOC(l+1,sizeof(unsigned int));
    for ( t = cap, ptr = mc->oxcap[j].cap, i = 0; i < l; t = NEXT(t), i++ )
      ptr[i] = BDY((USINT)BDY(t));
  }
  /* check of no_ox_reset */
  mc->no_ox_reset = 0;
  n = BDY(mclist);
  if ( length(n) >= 4 ) {
    t = BDY((LIST)ARG3(n));
    for ( ; t; t = NEXT(t) )
      if ( !strcmp(BDY((STRING)BDY(t)),"no_ox_reset") )
        mc->no_ox_reset = 1;
  }
}

int check_sm_by_mc(int s,unsigned int smtag)
{
  struct mathcap *rmc;
  int nsmcap,i;
  unsigned int *smcap;

  /* XXX : return 1 if remote_mc is not available. */
  if ( !remote_mc )
    return 1;
  rmc = &remote_mc[s];
  nsmcap = rmc->nsmcap;
  smcap = rmc->smcap;
  if ( !smcap )
    return 1;
  for ( i = 0; i < nsmcap; i++ )
    if ( smcap[i] == smtag )
      break;
  if ( i == nsmcap )
    return 0;
  else
    return 1;
}

int check_by_mc(int s,unsigned int oxtag,unsigned int cmotag)
{
  struct mathcap *rmc;
  int noxcap,ncap,i,j;
  struct oxcap *oxcap;
  unsigned int *cap;

  /* XXX : return 1 if remote_mc is not available. */
  if ( !remote_mc )
    return 1;
  rmc = &remote_mc[s];
  noxcap = rmc->noxcap;
  oxcap = rmc->oxcap;
  if ( !oxcap )
    return 1;
  for ( i = 0; i < noxcap; i++ )
    if ( oxcap[i].ox == oxtag )
      break;
  if ( i == noxcap )
    return 0;
  ncap = oxcap[i].ncap;
  cap = oxcap[i].cap;
  for ( j = 0; j < ncap; j++ )
    if ( cap[j] == cmotag )
      break;
  if ( j == ncap )
    return 0;
  else
    return 1;
}

extern int Im_ox_plot;
#if defined(ANDROID)
int Im_ox_plot = 0;
#endif

void begin_critical() {
  critical_when_signal = 1;
}

void end_critical() {
  critical_when_signal = 0;
  if ( ox_usr1_sent ) {
    ox_usr1_sent = 0;
#if !defined(VISUAL) && !defined(__MINGW32__)
  ox_usr1_handler(SIGUSR1);
#else
  ox_usr1_handler(0);
#endif
  }
  if ( ox_int_received ) {
    ox_int_received = 0; int_handler(SIGINT);
  }
}

extern NODE user_int_handler;
extern int caught_intr,in_gc;

void ox_usr1_handler(int sig)
{
  NODE t;

#if !defined(VISUAL) && !defined(__MINGW32__)
  set_signal_for_restart(SIGUSR1,ox_usr1_handler);
#endif
  if ( Im_ox_plot ) {
    ox_flushing = 1;
    ox_send_sync(0);
    return;
  }
  if ( critical_when_signal ) {
    fprintf(stderr,"usr1 : critical\n");
    ox_usr1_sent = 1;
    } else if ( in_gc ) {
    fprintf(stderr,"usr1 : in_gc\n");
    caught_intr = 2;
  } else {
    ox_flushing = 1;
    if ( user_int_handler ) {
      fprintf(stderr,
        "usr1 : calling the registered exception handlers...");
      for ( t = user_int_handler; t; t = NEXT(t) )
        bevalf((FUNC)BDY(t),0);
      fprintf(stderr, "done.\n");
    }
    LEAVE_SIGNAL_CS_ALL;
    ox_resetenv("usr1 : return to toplevel by SIGUSR1");
  }
}

void clear_readbuffer()
{
#if defined(ANDROID)
  fpurge(iofp[0].in);
#elif defined(linux)
  iofp[0].in->_IO_read_ptr = iofp[0].in->_IO_read_end;
#elif defined(__FreeBSD__)
  fpurge(iofp[0].in);
#endif
/*
  sock = fileno(iofp[0].in);
  interval.tv_sec = (int)0;
  interval.tv_usec = (int)0;

  FD_ZERO(&r); FD_ZERO(&w); FD_ZERO(&e);
  FD_SET(sock,&r);
  while ( 1 ) {
    n = select(FD_SETSIZE,&r,&w,&e,&interval);
    if ( !n )
      break;
    read(sock,&c,1);
  }
*/
}

#if MPI
int ox_data_is_available(int s)
{
  return 1;
}

void wait_for_data(int s)
{
  return;
}

void wait_for_data_102(int rank)
{
  return;
}
#else
int ox_data_is_available(int s)
{
  return FP_DATA_IS_AVAILABLE(iofp[s].in);
}

void wait_for_data(int s)
{
  fd_set r;
  int sock;

  if ( !FP_DATA_IS_AVAILABLE(iofp[s].in) ) {
#if defined(VISUAL) || defined(__MINGW32__)
    sock = iofp[s].in->fildes;
    FD_ZERO(&r);
    FD_SET((unsigned int)sock,&r);
    select(0,&r,NULL,NULL,NULL);
#else
    sock = fileno(iofp[s].in);
    FD_ZERO(&r);
    FD_SET(sock,&r);
    select(FD_SETSIZE,&r,NULL,NULL,NULL);
#endif
  }
}

void wait_for_data_102(int rank)
{
  fd_set r;
  int sock;

  if ( !FP_DATA_IS_AVAILABLE(iofp_102[rank].in) ) {
#if defined(VISUAL) || defined(__MINGW32__)
    sock = iofp_102[rank].in->fildes;
    FD_ZERO(&r);
    FD_SET((unsigned int)sock,&r);
    select(0,&r,NULL,NULL,NULL);
#else
    sock = fileno(iofp_102[rank].in);
    FD_ZERO(&r);
    FD_SET(sock,&r);
    select(FD_SETSIZE,&r,NULL,NULL,NULL);
#endif
  }
}
#endif

void ox_send_data(int s,pointer p)
{
  ERR err;
    Obj p0;

    p0 = (Obj)p;
  if ( ox_check && !ox_check_cmo(s,(Obj)p) ) {
    create_error(&err,ox_serial,"ox_send_data : Mathcap violation",0);
    p = (pointer)err;
  }
  begin_critical();
  ox_write_int(s,OX_DATA);
  ox_write_int(s,ox_serial++);
  ox_write_cmo(s,p);
  ox_flush_stream(s);
  end_critical();
}

void ox_send_data_102(int rank,pointer p)
{
  ERR err;

  begin_critical();
  ox_write_int_102(rank,OX_DATA);
  ox_write_int_102(rank,ox_serial++);
  ox_write_cmo_102(rank,p);
  ox_flush_stream_102(rank);
  end_critical();
}

void ox_bcast_102(int root)
{
  Obj data;
  int r,mask,id,src,dst;

  r = myrank_102-root;
  if ( r == 0 )
    data = (Obj)asir_pop_one();
    
  if ( r < 0 ) r += nserver_102;
  for ( mask = 1; mask < nserver_102; mask <<= 1 )
    if ( r&mask ) {
      src = myrank_102-mask;
      if ( src < 0 ) src += nserver_102;
      ox_recv_102(src,&id,&data);
      break;
    }
  for ( mask >>= 1; mask > 0; mask >>= 1 )
    if ( (r+mask) < nserver_102 ) {
      dst = myrank_102+mask;
      if ( dst >= nserver_102 ) dst -= nserver_102;
      ox_send_data_102(dst,data);
    }
  asir_push_one(data);
}

/* func : an arithmetic funcion func(vl,a,b,*c) */

void ox_reduce_102(int root,void (*func)())
{
  Obj data,data0,t;
  int r,mask,id,src,dst;

  r = myrank_102-root;
  if ( r < 0 ) r += nserver_102;
  data = (Obj)asir_pop_one();
  for ( mask = 1; mask < nserver_102; mask <<= 1 )
    if ( r&mask ) {
      dst = (r-mask)+root;
      if ( dst >= nserver_102 ) dst -= nserver_102;
      ox_send_data_102(dst,data);
      break;
    } else {
      src = r+mask;
      if ( src < nserver_102 ) {
        src += root;
        if ( src >= nserver_102 ) src -= nserver_102;
        ox_recv_102(src,&id,&data0);
        (*func)(CO,data,data0,&t); data = t;
      }
    }
  asir_push_one(r?0:data);
}

void ox_send_cmd(int s,int id)
{
  if ( ox_check && !check_sm_by_mc(s,id) )
    error("ox_send_cmd : Mathcap violation");
  begin_critical();
  ox_write_int(s,OX_COMMAND);
  ox_write_int(s,ox_serial++);
  ox_write_int(s,id);
  ox_flush_stream(s);
  end_critical();
}

void ox_send_sync(int s)
{
  begin_critical();
  ox_write_int(s,OX_SYNC_BALL);
  ox_write_int(s,ox_serial++);
  ox_flush_stream(s);
  end_critical();
}

void ox_send_sync_102(int rank)
{
  begin_critical();
  ox_write_int_102(rank,OX_SYNC_BALL);
  ox_write_int_102(rank,ox_serial++);
  ox_flush_stream_102(rank);
  end_critical();
}

void ox_send_local_data(int s,Obj p)
{
  begin_critical();
  ox_write_int(s,OX_LOCAL_OBJECT_ASIR);
  ox_write_int(s,ox_serial++);
  ox_write_int(s,ASIR_OBJ);
  saveobj((FILE *)iofp[s].out,p);
  ox_flush_stream(s);
  end_critical();
}

void ox_send_local_data_102(int rank,Obj p)
{
  begin_critical();
  ox_write_int_102(rank,OX_LOCAL_OBJECT_ASIR);
  ox_write_int_102(rank,ox_serial++);
  ox_write_int_102(rank,ASIR_OBJ);
  saveobj((FILE *)iofp_102[rank].out,p);
  ox_flush_stream_102(rank);
  end_critical();
}

void ox_send_local_ring(int s,VL vl)
{
  begin_critical();
  ox_write_int(s,OX_LOCAL_OBJECT_ASIR);
  ox_write_int(s,ox_serial++);
  ox_write_int(s,ASIR_VL);
  savevl((FILE *)iofp[s].out,vl);
  ox_flush_stream(s);
  end_critical();
}

void ox_send_local_ring_102(int rank,VL vl)
{
  begin_critical();
  ox_write_int_102(rank,OX_LOCAL_OBJECT_ASIR);
  ox_write_int_102(rank,ox_serial++);
  ox_write_int_102(rank,ASIR_VL);
  savevl((FILE *)iofp_102[rank].out,vl);
  ox_flush_stream_102(rank);
  end_critical();
}

unsigned int ox_recv(int s, int *id, Obj *p)
{
  unsigned int cmd,serial;
  USINT ui;

  wait_for_data(s);
  begin_critical();
  ox_read_int(s,id);
  ox_read_int(s,&serial);
  switch ( *id ) {
    case OX_COMMAND:
      ox_read_int(s,&cmd);
      MKUSINT(ui,cmd);
      *p = (Obj)ui;
      break;
    case OX_DATA:
      ox_read_cmo(s,p);
      break;
    case OX_LOCAL_OBJECT_ASIR:
      ox_read_local(s,p);
      break;
    default:
      *p = 0;
      break;
  }
  end_critical();
  return serial;
}

unsigned int ox_recv_102(int rank, int *id, Obj *p)
{
  unsigned int cmd,serial;
  USINT ui;

  wait_for_data_102(rank);
  begin_critical();
  ox_read_int_102(rank,id);
  ox_read_int_102(rank,&serial);
  switch ( *id ) {
    case OX_COMMAND:
      ox_read_int_102(rank,&cmd);
      MKUSINT(ui,cmd);
      *p = (Obj)ui;
      break;
    case OX_DATA:
      ox_read_cmo_102(rank,p);
      break;
    case OX_LOCAL_OBJECT_ASIR:
      ox_read_local_102(rank,p);
      break;
    default:
      *p = 0;
      break;
  }
  end_critical();
  return serial;
}

void ox_get_result(int s,Obj *rp)
{
  int id;
  Obj obj,r;
  int level;

  level = 0;
  r = 0;
  do {
    ox_recv(s,&id,&obj);
    if ( id == OX_COMMAND ) {
      switch ( ((USINT)obj)->body ) {
        case SM_beginBlock:
          level++;
          break;
        case SM_endBlock:
          level--;
      }
    } else 
      r = obj;
  } while ( level );
  *rp = r;
}

void ox_read_int(int s, int *n)
{
  ox_need_conv = iofp[s].conv;
  read_int((FILE *)iofp[s].in,n);
}

void ox_read_int_102(int rank, int *n)
{
  ox_need_conv = iofp_102[rank].conv;
  read_int((FILE *)iofp_102[rank].in,n);
}

void ox_read_cmo(int s, Obj *rp)
{
  ox_need_conv = iofp[s].conv;
  read_cmo((FILE *)iofp[s].in,rp);
}

void ox_read_cmo_102(int rank, Obj *rp)
{
  ox_need_conv = iofp_102[rank].conv;
  read_cmo((FILE *)iofp_102[rank].in,rp);
}


void ox_read_local(int s, Obj *rp)
{
  int id;

  ox_need_conv = iofp[s].conv;
  read_int((FILE *)iofp[s].in,&id);
  switch ( id ) {
    case ASIR_VL:
      loadvl((FILE *)iofp[s].in);
      *rp = VOIDobj;
      break;
    case ASIR_OBJ:
      loadobj((FILE *)iofp[s].in,rp);
      break;
    default:
      error("ox_read_local : unsupported id");
      break;
  }
}

void ox_read_local_102(int rank, Obj *rp)
{
  int id;

  ox_need_conv = iofp_102[rank].conv;
  read_int((FILE *)iofp_102[rank].in,&id);
  switch ( id ) {
    case ASIR_VL:
      loadvl((FILE *)iofp_102[rank].in);
      *rp = VOIDobj;
      break;
    case ASIR_OBJ:
      loadobj((FILE *)iofp_102[rank].in,rp);
      break;
    default:
      error("ox_read_local_102 : unsupported id");
      break;
  }
}

void ox_write_int(int s, int n)
{
  ox_need_conv = iofp[s].conv;
  write_int((FILE *)iofp[s].out,&n);
}

void ox_write_int_102(int rank, int n)
{
  ox_need_conv = iofp_102[rank].conv;
  write_int((FILE *)iofp_102[rank].out,&n);
}

void ox_write_cmo(int s, Obj obj)
{
  ox_need_conv = iofp[s].conv;
  write_cmo((FILE *)iofp[s].out,obj);
}

void ox_write_cmo_102(int rank, Obj obj)
{
  ox_need_conv = iofp_102[rank].conv;
  write_cmo((FILE *)iofp_102[rank].out,obj);
}

int ox_check_cmo(int s, Obj obj)
{
  NODE m;

  if ( !obj )
    return 1;
  switch ( OID(obj) ) {
    case O_MATHCAP: case O_STR: case O_ERR: case O_USINT: case O_VOID:
    case O_BYTEARRAY:
      return 1;
    case O_P:
      if ( !check_by_mc(s,OX_DATA,CMO_RECURSIVE_POLYNOMIAL) )
        return 0;
      else
        return ox_check_cmo_p(s,(P)obj);  
    case O_R:
      if ( !check_by_mc(s,OX_DATA,CMO_RATIONAL) )
        return 0;
      else if ( !check_by_mc(s,OX_DATA,CMO_RECURSIVE_POLYNOMIAL) )
        return 0;
      else
        return ox_check_cmo_p(s,NM((R)obj)) && ox_check_cmo_p(s,DN((R)obj));
    case O_DP:
      return ox_check_cmo_dp(s,(DP)obj);  
    case O_N:
      switch ( NID((Num)obj) ) {
        case N_Q:
          if ( INT((Q)obj) )
            return check_by_mc(s,OX_DATA,CMO_ZZ);
          else
            return check_by_mc(s,OX_DATA,CMO_QQ);
        case N_R: case N_B: case N_C:
          return 1;
        default:
          return 0;
      }
      break;
    case O_LIST:
      for ( m = BDY((LIST)obj); m; m = NEXT(m) )
        if ( !ox_check_cmo(s,(BDY(m))) )
          return 0;
      return 1;
    case O_QUOTE: /* XXX */
      return 1;
    case O_MAT: /* MAT is sent as a list */
      return 1;
    default:
      return 0;
  }
}

void ox_get_serverinfo(int s, LIST *rp)
{
  if ( remote_mc )
    *rp = remote_mc[s].mc;
  else {
    MKLIST(*rp,0);
  }
}

char *ox_get_servername(int s)
{
  return (remote_mc && remote_mc[s].servername)?remote_mc[s].servername:0;
}


int ox_check_cmo_p(int s, P p)
{
  DCP dc;

  if ( NUM(p) )
    return ox_check_cmo(s,(Obj)p);
  else {
    for ( dc = DC(p); dc; dc = NEXT(dc) )
      if ( !ox_check_cmo_p(s,COEF(dc)) )
        return 0;
    return 1;
  }
}

int ox_check_cmo_dp(int s, DP p)
{
  MP m;

  for ( m = BDY(p); m; m = NEXT(m) )
    if ( !ox_check_cmo(s,(Obj)m->c) )
      return 0;
  return 1;
}

void ox_flush_stream(int s)
{
  if ( ox_batch )
    return;
#if defined(VISUAL) || defined(__MINGW32__) || defined(MPI)
  if ( WSIO_fileno(iofp[s].out) < 0 )
    cflush(iofp[s].out);
  else
#endif
  fflush((FILE *)iofp[s].out);
}

void ox_flush_stream_force(int s)
{
#if defined(VISUAL) || defined(__MINGW32__) || defined(MPI)
  if ( WSIO_fileno(iofp[s].out) < 0 )
    cflush(iofp[s].out);
  else
#endif
  fflush((FILE *)iofp[s].out);
}

void ox_flush_stream_102(int rank)
{
  if ( !ox_batch )
    ox_flush_stream_force_102(rank);
}

void ox_flush_stream_force_102(int rank)
{
  if ( iofp_102[rank].out )
#if defined(VISUAL) || defined(__MINGW32__)
    cflush(iofp_102[rank].out);
#elif MPI
    cflush(iofp_102[rank].out);
#else
    fflush(iofp_102[rank].out);
#endif
}
