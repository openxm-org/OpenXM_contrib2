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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/gr.c,v 1.1 2018/09/19 05:45:06 noro Exp $
*/
#include "ca.h"
#include "parse.h"
#include "base.h"
#include "ox.h"

#if defined(__GNUC__)
#define INLINE static inline
#elif defined(VISUAL) || defined(__MINGW32__)
#define INLINE __inline
#else
#define INLINE
#endif

#define HMAG(p) (p_mag((P)BDY(p)->c))

#define NEWDP_pairs ((DP_pairs)MALLOC(sizeof(struct dp_pairs)))

static DP_pairs collect_pairs_of_hdlcm( DP_pairs d1, DP_pairs *prest );
double get_rtime();  

struct oEGT eg_nf,eg_nfm;
struct oEGT eg_znfm,eg_pz,eg_np,eg_ra,eg_mc,eg_gc;
int TP,N_BP,NMP,NFP,NDP,ZR,NZR;

extern int (*cmpdl)();
extern int do_weyl;

extern int DP_Print;

extern int dp_nelim;
extern int dp_fcoeffs;
static DP *ps,*psm;
static DL *psh;
static P *psc;

static int *pss;
static int psn,pslen;
static int NVars,CNVars;
static VL VC;

int PCoeffs;
int DP_Print = 0;
int DP_PrintShort = 0;
int DP_Multiple = 0;
int DP_NFStat = 0;
LIST Dist = 0;
int NoGCD = 0;
int GenTrace = 0;
int GenSyz = 0;
int OXCheck = -1;
int OneZeroHomo = 0;
int MaxDeg = 0;

int NoSugar = 0;
static int NoCriB = 0;
static int NoGC = 0;
static int NoMC = 0;
static int NoRA = 0;
static int ShowMag = 0;
static int Stat = 0;
int Denominator = 1;
int Top = 0;
int Reverse = 0;
static int Max_mag = 0;
static int Max_coef = 0;
char *Demand = 0;
static int PtozpRA = 0;

int doing_f4;
NODE TraceList;
NODE AllTraceList;

void Pox_cmo_rpc(NODE,Obj *);
void Pox_rpc(NODE,Obj *);
void Pox_pop_local(NODE,Obj *);

INLINE int eqdl(int nv,DL dl1,DL dl2)
{
  int i;
  int *p1,*p2;

  if ( dl1->td != dl2->td )
    return 0;
  i = nv-1;
  p1 = dl1->d;
  p2 = dl2->d;
  while ( i >= 7 ) {
    if ( *p1++ != *p2++ ) return 0;
    if ( *p1++ != *p2++ ) return 0;
    if ( *p1++ != *p2++ ) return 0;
    if ( *p1++ != *p2++ ) return 0;
    if ( *p1++ != *p2++ ) return 0;
    if ( *p1++ != *p2++ ) return 0;
    if ( *p1++ != *p2++ ) return 0;
    if ( *p1++ != *p2++ ) return 0;
    i -= 8;
  }
  switch ( i ) {
    case 6:
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      return 1;
    case 5:
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      return 1;
    case 4:
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      return 1;
    case 3:
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      return 1;
    case 2:
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      return 1;
    case 1:
      if ( *p1++ != *p2++ ) return 0;
      if ( *p1++ != *p2++ ) return 0;
      return 1;
    case 0:
      if ( *p1++ != *p2++ ) return 0;
      return 1;
    default:
      return 1;
  }
}

/* b[] should be cleared */

void _dpmod_to_vect(DP f,DL *at,int *b)
{
  int i,nv;
  MP m;

  nv = f->nv;
  for ( m = BDY(f), i = 0; m; m = NEXT(m), i++ ) {
    for ( ; !eqdl(nv,m->dl,at[i]); i++ );
    b[i] = ITOS(m->c);
  }
}

/* [t,findex] -> tf -> compressed vector */

void _tf_to_vect_compress(NODE tf,DL *at,CDP *b)
{
  int i,j,k,nv,len;
  DL t,s,d1;
  DP f;
  MP m;
  CDP r;

  t = (DL)BDY(tf);
  f = ps[(long)BDY(NEXT(tf))];

  nv = f->nv;
  for ( m = BDY(f), len = 0; m; m = NEXT(m), len++ );
  r = (CDP)MALLOC(sizeof(struct oCDP));
  r->len = len;
  r->psindex = (long)BDY(NEXT(tf));
  r->body = (unsigned int *)MALLOC_ATOMIC(sizeof(unsigned int)*len);

  NEWDL_NOINIT(s,nv);
  for ( m = BDY(f), i = j = 0; m; m = NEXT(m), j++ ) {
    d1 = m->dl;
    s->td = t->td+d1->td;
    for ( k = 0; k < nv; k++ )
      s->d[k] = t->d[k]+d1->d[k];
    for ( ; !eqdl(nv,s,at[i]); i++ );
    r->body[j] = i;
  }
  *b = r;
}

void dp_to_vect(DP f,DL *at,Q *b)
{
  int i,nv;
  MP m;

  nv = f->nv;
  for ( m = BDY(f), i = 0; m; m = NEXT(m), i++ ) {
    for ( ; !eqdl(nv,m->dl,at[i]); i++ );
    b[i] =(Q)m->c;
  }
}

NODE dp_dllist(DP f)
{
  MP m;
  NODE mp,mp0;

  if ( !f )
    return 0;
  mp0 = 0;
  for ( m = BDY(f); m; m = NEXT(m) ) {
    NEXTNODE(mp0,mp); BDY(mp) = (pointer)m->dl;
  }
  NEXT(mp) = 0;
  return mp0;
}

NODE mul_dllist(DL d,DP f)
{
  MP m;
  NODE mp,mp0;
  DL t,d1;
  int i,nv;

  if ( !f )
    return 0;
  nv = NV(f);
  mp0 = 0;
  for ( m = BDY(f); m; m = NEXT(m) ) {
    NEXTNODE(mp0,mp);
    NEWDL_NOINIT(t,nv);
    d1 = m->dl;
    t->td = d->td+d1->td;
    for ( i = 0; i < nv; i++ )
      t->d[i] = d->d[i]+d1->d[i];
    BDY(mp) = (pointer)t;
  }
  NEXT(mp) = 0;
  return mp0;
}

void pdl(NODE f)
{
  while ( f ) {
    printdl(BDY(f)); f = NEXT(f);
  }
  fflush(stdout);
  printf("\n");
}

void dp_gr_main(LIST f,LIST v,Num homo,int modular,int field,struct order_spec *ord,LIST *rp)
{
  int i,mindex,m,nochk;
  struct order_spec *ord1;
  Z q;
  VL fv,vv,vc;
  NODE fd,fd0,fi,fi0,r,r0,t,subst,x,s,xx;
  NODE ind,ind0;
  LIST trace,gbindex;
  int input_is_dp = 0;

  mindex = 0; nochk = 0; dp_fcoeffs = field;
  get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&vc);
  NVars = length((NODE)vv); PCoeffs = vc ? 1 : 0; VC = vc;
  CNVars = homo ? NVars+1 : NVars;
  if ( ord->id && NVars != ord->nv )
    error("dp_gr_main : invalid order specification");
  initd(ord);
  if ( homo ) {
    homogenize_order(ord,NVars,&ord1);
    for ( fd0 = fi0 = 0, t = BDY(f); t; t = NEXT(t) ) {
      NEXTNODE(fd0,fd); NEXTNODE(fi0,fi);
      if ( BDY(t) && OID(BDY(t)) == O_DP ) {
        dp_sort((DP)BDY(t),(DP *)&BDY(fi)); input_is_dp = 1;
      } else 
        ptod(CO,vv,(P)BDY(t),(DP *)&BDY(fi));
      dp_homo((DP)BDY(fi),(DP *)&BDY(fd));
    }
    if ( fd0 ) NEXT(fd) = 0;
    if ( fi0 ) NEXT(fi) = 0;
    initd(ord1);
  } else {
    for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
      NEXTNODE(fd0,fd);
      if ( BDY(t) && OID(BDY(t)) == O_DP ) {
        dp_sort((DP)BDY(t),(DP *)&BDY(fd)); input_is_dp = 1;
      } else
        ptod(CO,vv,(P)BDY(t),(DP *)&BDY(fd));
    }
    if ( fd0 ) NEXT(fd) = 0;
    fi0 = fd0;
  }
  if ( modular < 0 ) {
    modular = -modular; nochk = 1;
  }
  if ( modular )
    m = modular > 1 ? modular : get_lprime(mindex);
  else
    m = 0;
  makesubst(vc,&subst);
  setup_arrays(fd0,0,&s);
  init_stat();
  while ( 1 ) {
    if ( homo ) {
      initd(ord1); CNVars = NVars+1;
    }
    if ( DP_Print && modular ) {
      fprintf(asir_out,"mod= %d, eval = ",m); printsubst(subst);
    }
    x = gb(s,m,subst);
    if ( x ) {
      if ( homo ) {
        reducebase_dehomo(x,&xx); x = xx;
        initd(ord); CNVars = NVars;
      }
      reduceall(x,&xx); x = xx;
      if ( modular ) {
        if ( nochk || (membercheck(fi0,x) && gbcheck(x)) )
          break;
      } else
        break;
    }
    if ( modular )
      if ( modular > 1 ) {
        *rp = 0; return;
      } else
        m = get_lprime(++mindex);
    makesubst(vc,&subst);
    psn = length(s);
    for ( i = psn; i < pslen; i++ ) {
      pss[i] = 0; psh[i] = 0; psc[i] = 0; ps[i] = 0;
    }
  }
  for ( r0 = 0, ind0 = 0; x; x = NEXT(x) ) {
    NEXTNODE(r0,r); dp_load((long)BDY(x),&ps[(long)BDY(x)]);
    if ( input_is_dp )
      BDY(r) = (pointer)ps[(long)BDY(x)];
    else
      dtop(CO,vv,ps[(long)BDY(x)],(Obj *)&BDY(r));
    NEXTNODE(ind0,ind);
    STOZ((long)BDY(x),q); BDY(ind) = q;
  }
  if ( r0 ) NEXT(r) = 0;
  if ( ind0 ) NEXT(ind) = 0;
  MKLIST(*rp,r0);
  MKLIST(gbindex,ind0);

  if ( GenTrace && OXCheck < 0 ) {

    x = AllTraceList;
    for ( r = 0; x; x = NEXT(x) ) {
      MKNODE(r0,BDY(x),r); r = r0;
    }
    MKLIST(trace,r);
    r0 = mknode(3,*rp,gbindex,trace);
    MKLIST(*rp,r0);
  }
  print_stat();
  if ( ShowMag )
    fprintf(asir_out,"\nMax_mag=%d, Max_coef=%d\n",Max_mag, Max_coef);
}

void dp_interreduce(LIST f,LIST v,int field,struct order_spec *ord,LIST *rp)
{
  int i,mindex,m,nochk;
  struct order_spec *ord1;
  Z q;
  VL fv,vv,vc;
  NODE fd,fd0,fi,fi0,r,r0,t,subst,x,s,xx;
  NODE ind,ind0;
  LIST trace,gbindex;
  int input_is_dp = 0;

  mindex = 0; nochk = 0; dp_fcoeffs = field;
  get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&vc);
  NVars = length((NODE)vv); PCoeffs = vc ? 1 : 0; VC = vc;
  CNVars = NVars;
  if ( ord->id && NVars != ord->nv )
    error("dp_interreduce : invalid order specification");
  initd(ord);
  for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
    NEXTNODE(fd0,fd);
    if ( BDY(t) && OID(BDY(t)) == O_DP ) {
      dp_sort((DP)BDY(t),(DP *)&BDY(fd)); input_is_dp = 1;
    } else
      ptod(CO,vv,(P)BDY(t),(DP *)&BDY(fd));
  }
  if ( fd0 ) NEXT(fd) = 0;
  fi0 = fd0;
  setup_arrays(fd0,0,&s);
  init_stat();
  x = s;
  reduceall(x,&xx); x = xx;
  for ( r0 = 0, ind0 = 0; x; x = NEXT(x) ) {
    NEXTNODE(r0,r); dp_load((long)BDY(x),&ps[(long)BDY(x)]);
    if ( input_is_dp )
      BDY(r) = (pointer)ps[(long)BDY(x)];
    else
      dtop(CO,vv,ps[(long)BDY(x)],(Obj *)&BDY(r));
    NEXTNODE(ind0,ind);
    STOZ((long)BDY(x),q); BDY(ind) = q;
  }
  if ( r0 ) NEXT(r) = 0;
  if ( ind0 ) NEXT(ind) = 0;
  MKLIST(*rp,r0);
  MKLIST(gbindex,ind0);
}

void dp_gr_mod_main(LIST f,LIST v,Num homo,int m,struct order_spec *ord,LIST *rp)
{
  struct order_spec *ord1;
  VL fv,vv,vc;
  NODE fd,fd0,r,r0,t,x,s,xx;
  DP a,b,c;
  extern struct oEGT eg_red_mod;
  int input_is_dp = 0;

  get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&vc);
  NVars = length((NODE)vv); PCoeffs = vc ? 1 : 0; VC = vc;
  CNVars = homo ? NVars+1 : NVars;
  if ( ord->id && NVars != ord->nv )
    error("dp_gr_mod_main : invalid order specification");
  initd(ord);
  if ( homo ) {
    for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
      if ( BDY(t) && OID(BDY(t)) == O_DP ) {
        dp_sort((DP)BDY(t),&a); input_is_dp = 1;
      } else 
        ptod(CO,vv,(P)BDY(t),&a);
      dp_homo(a,&b);
      if ( PCoeffs )
        dp_mod(b,m,0,&c);
      else
        _dp_mod(b,m,(NODE)0,&c);
      if ( c ) {
        NEXTNODE(fd0,fd); BDY(fd) = (pointer)c;
      }
    }
    homogenize_order(ord,NVars,&ord1); initd(ord1);
  } else {
    for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
      if ( BDY(t) && OID(BDY(t)) == O_DP ) {
        dp_sort((DP)BDY(t),&b); input_is_dp = 1;
      } else 
        ptod(CO,vv,(P)BDY(t),&b);
      if ( PCoeffs )
        dp_mod(b,m,0,&c);
      else
        _dp_mod(b,m,0,&c);
      if ( c ) {
        NEXTNODE(fd0,fd); BDY(fd) = (pointer)c;
      }
    }
  }
  if ( fd0 ) NEXT(fd) = 0;
  setup_arrays(fd0,m,&s);
  init_stat();
  if ( homo ) {
    initd(ord1); CNVars = NVars+1;
  }
/* init_eg(&eg_red_mod); */
  x = gb_mod(s,m);
/* print_eg("Red_mod",&eg_red_mod); */
  if ( homo ) {
    reducebase_dehomo(x,&xx); x = xx;
    initd(ord); CNVars = NVars;
  }
  reduceall_mod(x,m,&xx); x = xx;
  if ( PCoeffs )
    for ( r0 = 0; x; x = NEXT(x) ) {
      NEXTNODE(r0,r); 
      if ( input_is_dp )
        mdtodp(ps[(long)BDY(x)],(DP *)&BDY(r));
      else
        mdtop(CO,m,vv,ps[(long)BDY(x)],(P *)&BDY(r));
    }
  else
    for ( r0 = 0; x; x = NEXT(x) ) {
      NEXTNODE(r0,r); 
      if ( input_is_dp )
        _mdtodp(ps[(long)BDY(x)],(DP *)&BDY(r));
      else
        _dtop_mod(CO,vv,ps[(long)BDY(x)],(P *)&BDY(r));
    }
  print_stat();
  if ( r0 ) NEXT(r) = 0;
  MKLIST(*rp,r0);
}

void dp_f4_main(LIST f,LIST v,struct order_spec *ord,LIST *rp)
{
  int homogen;
  VL fv,vv,vc;
  NODE fd,fd0,r,r0,t,x,s,xx;
  int input_is_dp = 0;

  dp_fcoeffs = 0;
  get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&vc);
  NVars = length((NODE)vv); PCoeffs = vc ? 1 : 0; VC = vc;
  CNVars = NVars;
  if ( ord->id && NVars != ord->nv )
    error("dp_f4_main : invalid order specification");
  initd(ord);
  for ( fd0 = 0, t = BDY(f), homogen = 1; t; t = NEXT(t) ) {
    NEXTNODE(fd0,fd); 
    if ( BDY(t) && OID(BDY(t)) == O_DP ) {
      dp_sort((DP)BDY(t),(DP *)&BDY(fd)); input_is_dp = 1;
    } else 
      ptod(CO,vv,(P)BDY(t),(DP *)&BDY(fd));
    if ( homogen )
      homogen = dp_homogeneous(BDY(fd));
  }
  if ( fd0 ) NEXT(fd) = 0;
  setup_arrays(fd0,0,&s);
  x = gb_f4(s);
  if ( !homogen ) {
    reduceall(x,&xx); x = xx;
  }
  for ( r0 = 0; x; x = NEXT(x) ) {
    NEXTNODE(r0,r); dp_load((long)BDY(x),&ps[(long)BDY(x)]);
    if ( input_is_dp )
      BDY(r) = (pointer)ps[(long)BDY(x)];
    else
      dtop(CO,vv,ps[(long)BDY(x)],(Obj *)&BDY(r));
  }
  if ( r0 ) NEXT(r) = 0;
  MKLIST(*rp,r0);
}

void dp_f4_mod_main(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp)
{
  int homogen;
  VL fv,vv,vc;
  DP b,c,c1;
  NODE fd,fd0,r,r0,t,x,s,xx;
  int input_is_dp = 0;

  dp_fcoeffs = 0;
  get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&vc);
  NVars = length((NODE)vv); PCoeffs = vc ? 1 : 0; VC = vc;
  CNVars = NVars;
  if ( ord->id && NVars != ord->nv )
    error("dp_f4_mod_main : invalid order specification");
  initd(ord);
  for ( fd0 = 0, t = BDY(f), homogen = 1; t; t = NEXT(t) ) {
    if ( BDY(t) && OID(BDY(t)) == O_DP ) {
      dp_sort((DP)BDY(t),&b); input_is_dp = 1;
    } else 
      ptod(CO,vv,(P)BDY(t),&b);
    if ( homogen )
      homogen = dp_homogeneous(b);
    _dp_mod(b,m,0,&c);
    _dp_monic(c,m,&c1);
    if ( c ) {
      NEXTNODE(fd0,fd); BDY(fd) = (pointer)c1;
    }
  }
  if ( fd0 ) NEXT(fd) = 0;
  setup_arrays(fd0,m,&s);
  init_stat();
  if ( do_weyl )
    x = gb_f4_mod_old(s,m);
  else
    x = gb_f4_mod(s,m);
  if ( !homogen ) {
    reduceall_mod(x,m,&xx); x = xx;
  }
  for ( r0 = 0; x; x = NEXT(x) ) {
    NEXTNODE(r0,r);
    if ( input_is_dp )
      _mdtodp(ps[(long)BDY(x)],(DP *)&BDY(r));
    else
      _dtop_mod(CO,vv,ps[(long)BDY(x)],(P *)&BDY(r));
  }
  if ( r0 ) NEXT(r) = 0;
  MKLIST(*rp,r0);
  print_stat();
}

NODE gb_f4(NODE f)
{
  int i,k,nh,row,col,nv;
  NODE r,g,gall;
  NODE s,s0;
  DP_pairs d,dm,dr,t;
  DP nf,nf1,f2,sp,sd,tdp;
  MP mp,mp0;
  NODE blist,bt;
  DL *ht,*at;
  MAT mat,nm;
  int *rind,*cind;
  int rank,nred;
  Z dn;
  struct oEGT tmp0,tmp1,eg_split_symb;
  extern struct oEGT eg_mod,eg_elim,eg_chrem,eg_gschk,eg_intrat,eg_symb;

  init_eg(&eg_mod); init_eg(&eg_elim); init_eg(&eg_chrem);
  init_eg(&eg_gschk); init_eg(&eg_intrat); init_eg(&eg_symb);

  doing_f4 = 1;
  for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
    i = (long)BDY(r);
    d = updpairs(d,g,i);
    g = updbase(g,i);
    gall = append_one(gall,i);
  }
  if ( gall )
    nv = ((DP)ps[(long)BDY(gall)])->nv;
  while ( d ) {
    get_eg(&tmp0);
    minsugar(d,&dm,&dr); d = dr;
    if ( DP_Print )
      fprintf(asir_out,"sugar=%d\n",dm->sugar);
    blist = 0; s0 = 0;
    /* asph : sum of all head terms of spoly */
    for ( t = dm; t; t = NEXT(t) ) {
      dp_sp(ps[t->dp1],ps[t->dp2],&sp);
      if ( sp ) {
        MKNODE(bt,sp,blist); blist = bt;
        s0 = symb_merge(s0,dp_dllist(sp),nv);
      }
    }
    /* s0 : all the terms appeared in symbolic redunction */
    for ( s = s0, nred = 0; s; s = NEXT(s) ) {
      for ( r = gall;  r; r = NEXT(r) )
        if ( _dl_redble(BDY(ps[(long)BDY(r)])->dl,BDY(s),nv) )
          break;
      if ( r ) {
        dltod(BDY(s),nv,&tdp);
        dp_subd(tdp,ps[(long)BDY(r)],&sd);
        muld(CO,sd,ps[(long)BDY(r)],&f2);
        MKNODE(bt,f2,blist); blist = bt;
        s = symb_merge(s,dp_dllist(f2),nv);
        nred++;
      }
    }

    /* the first nred polys in blist are reducers */
    /* row = the number of all the polys */
    for ( r = blist, row = 0; r; r = NEXT(r), row++ );
    ht = (DL *)MALLOC(nred*sizeof(DL));
    for ( r = blist, i = 0; i < nred; r = NEXT(r), i++ )
      ht[i] = BDY((DP)BDY(r))->dl;
    for ( s = s0, col = 0; s; s = NEXT(s), col++ );
    at = (DL *)MALLOC(col*sizeof(DL));
    for ( s = s0, i = 0; i < col; s = NEXT(s), i++ )
      at[i] = (DL)BDY(s);
    MKMAT(mat,row,col);
    for ( i = 0, r = blist; i < row; r = NEXT(r), i++ )
      dp_to_vect(BDY(r),at,(Q *)mat->body[i]);
    get_eg(&tmp1); add_eg(&eg_symb,&tmp0,&tmp1);
    init_eg(&eg_split_symb); add_eg(&eg_split_symb,&tmp0,&tmp1);
    if ( DP_Print ) {
      print_eg("Symb",&eg_split_symb);
      fprintf(asir_out,"mat : %d x %d",row,col);
      fflush(asir_out);
    }
#if 0
    rank = generic_gauss_elim_hensel(mat,&nm,&dn,&rind,&cind);
#else
    rank = generic_gauss_elim(mat,&nm,&dn,&rind,&cind);
#endif
    if ( DP_Print )
      fprintf(asir_out,"done rank = %d\n",rank,row,col);
    for ( i = 0; i < rank; i++ ) {
      for ( k = 0; k < nred; k++ )
        if ( !cmpdl(nv,at[rind[i]],ht[k]) )
          break;
      if ( k == nred ) {
        /* this is a new base */
        mp0 = 0;
        NEXTMP(mp0,mp); mp->dl = at[rind[i]]; mp->c = (Obj)dn;
        for ( k = 0; k < col-rank; k++ )
          if ( nm->body[i][k] ) {
            NEXTMP(mp0,mp); mp->dl = at[cind[k]];
            mp->c = (Obj)nm->body[i][k];
          }
        NEXT(mp) = 0;
        MKDP(nv,mp0,nf); nf->sugar = dm->sugar;
        dp_ptozp(nf,&nf1);
        nh = newps(nf1,0,0);
        d = updpairs(d,g,nh);
        g = updbase(g,nh);
        gall = append_one(gall,nh);
      }
    }
  }
  if ( DP_Print ) {
    print_eg("Symb",&eg_symb);
    print_eg("Mod",&eg_mod); print_eg("GaussElim",&eg_elim);
    print_eg("ChRem",&eg_chrem); print_eg("IntToRat",&eg_intrat);
    print_eg("Check",&eg_gschk);
  }
  return g;
}

/* initial bases are monic */

unsigned int **psca;
GeoBucket create_bucket();
DL remove_head_bucket(GeoBucket,int);

NODE gb_f4_mod(NODE f,int m)
{
  int i,j,k,nh,row,col,nv;
  NODE r,g,gall;
  NODE s,s0;
  DP_pairs d,dm,dr,t;
  DP nf,sp,sd,tdp;
  MP mp,mp0;
  NODE blist,bt,bt1,dt;
  DL *at,*st;
  int **spmat;
  CDP *redmat;
  int *colstat,*w,*w1;
  int rank,nred,nsp,nsp0,nonzero,spcol;
  int *indred,*isred;
  CDP ri;
  int pscalen;
  GeoBucket bucket;
  DL head;
  struct oEGT tmp0,tmp1,eg_split_symb,eg_split_conv,eg_split_elim1,eg_split_elim2;
  extern struct oEGT eg_symb,eg_conv,eg_elim1,eg_elim2;

  /* initialize coeffcient array list of ps[] */
  pscalen = pslen;
  psca = (unsigned int **)MALLOC(pscalen*sizeof(unsigned int *));

  init_eg(&eg_symb); init_eg(&eg_conv); init_eg(&eg_elim1); init_eg(&eg_elim2);
  for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
    i = (long)BDY(r);
    d = updpairs(d,g,i);
    g = updbase(g,i);
    gall = append_one(gall,i);
    dptoca(ps[i],&psca[i]);
  }
  if ( gall )
    nv = ((DP)ps[(long)BDY(gall)])->nv;
  while ( d ) {
    get_eg(&tmp0);
    minsugar(d,&dm,&dr); d = dr;
    if ( DP_Print )
      fprintf(asir_out,"sugar=%d\n",dm->sugar);
    blist = 0;
    bucket = create_bucket();
    /* asph : sum of all head terms of spoly */
    for ( t = dm; t; t = NEXT(t) ) {
      _dp_sp_mod(ps[t->dp1],ps[t->dp2],m,&sp);
/*      fprintf(stderr,"splen=%d-",dp_nt(sp)); */
      if ( sp ) {
        MKNODE(bt,sp,blist); blist = bt;
        add_bucket(bucket,dp_dllist(sp),nv);
/*        fprintf(stderr,"%d-",length(s0)); */
      }
    }
#if 0
    if ( DP_Print )
      fprintf(asir_out,"initial spmat : %d x %d ",length(blist),length(s0));
#endif
    /* s0 : all the terms appeared in symbolic reduction */
    nred = 0;
    s0 = 0;
    while ( 1 ) {
      head = remove_head_bucket(bucket,nv);
      if ( !head ) break;
      else {
        NEXTNODE(s0,s);
        BDY(s) = (pointer)head;
      }
      for ( r = gall;  r; r = NEXT(r) )
        if ( _dl_redble(BDY(ps[(long)BDY(r)])->dl,head,nv) )
          break;
      if ( r ) {
        dltod(head,nv,&tdp);
        dp_subd(tdp,ps[(long)BDY(r)],&sd);
        dt = mul_dllist(BDY(sd)->dl,ps[(long)BDY(r)]);
        add_bucket(bucket,NEXT(dt),nv);
/*        fprintf(stderr,"[%d]",length(dt)); */
        /* list of [t,f] */
        bt1 = mknode(2,BDY(sd)->dl,BDY(r));
        MKNODE(bt,bt1,blist); blist = bt;
/*        fprintf(stderr,"%d-",length(s0));  */
        nred++;
      }
    }
    if ( s0 ) NEXT(s) = 0;
/*    fprintf(stderr,"\n"); */
    get_eg(&tmp1); add_eg(&eg_symb,&tmp0,&tmp1);
    init_eg(&eg_split_symb); add_eg(&eg_split_symb,&tmp0,&tmp1);

    if ( DP_Print )
      fprintf(asir_out,"number of reducers : %d\n",nred);

    get_eg(&tmp0);
    /* the first nred polys in blist are reducers */
    /* row = the number of all the polys */
    for ( r = blist, row = 0; r; r = NEXT(r), row++ );

    /* col = number of all terms */
    for ( s = s0, col = 0; s; s = NEXT(s), col++ );

    /* head terms of all terms */
    at = (DL *)MALLOC(col*sizeof(DL));
    for ( s = s0, i = 0; i < col; s = NEXT(s), i++ )
      at[i] = (DL)BDY(s);

    /* store coefficients separately in spmat and redmat */
    nsp = row-nred;

    /* reducer matrix */
    /* indred : register the position of the head term */
    redmat = (CDP *)MALLOC(nred*sizeof(CDP));
    for ( i = 0, r = blist; i < nred; r = NEXT(r), i++ )
      _tf_to_vect_compress(BDY(r),at,&redmat[i]);

    /* register the position of the head term */
    indred = (int *)MALLOC_ATOMIC(nred*sizeof(int));
    bzero(indred,nred*sizeof(int));
    isred = (int *)MALLOC_ATOMIC(col*sizeof(int));
    bzero(isred,col*sizeof(int));
    for ( i = 0; i < nred; i++ ) {
      ri = redmat[i];
      indred[i] = ri->body[0];
      isred[indred[i]] = 1;
    }

    spcol = col-nred;
    /* head terms not in ht */
    st = (DL *)MALLOC(spcol*sizeof(DL));
    for ( j = 0, k = 0; j < col; j++ )
      if ( !isred[j] )
        st[k++] = at[j];
    get_eg(&tmp1); add_eg(&eg_conv,&tmp0,&tmp1);
    init_eg(&eg_split_conv); add_eg(&eg_split_conv,&tmp0,&tmp1);

    get_eg(&tmp1);
    /* spoly matrix; stored in reduced form; terms in ht[] are omitted */
    spmat = (int **)MALLOC(nsp*sizeof(int *));
    w = (int *)MALLOC_ATOMIC(col*sizeof(int));

    /* skip reducers in blist */
    for ( i = 0, r = blist; i < nred; r = NEXT(r), i++ );
    for ( i = 0; r; r = NEXT(r) ) {
      bzero(w,col*sizeof(int));
      _dpmod_to_vect(BDY(r),at,w);
      reduce_sp_by_red_mod_compress(w,redmat,indred,nred,col,m);
      for ( j = 0; j < col; j++ )
        if ( w[j] )
          break;
      if ( j < col ) {
        w1 = (int *)MALLOC_ATOMIC(spcol*sizeof(int));
        for ( j = 0, k = 0; j < col; j++ )
          if ( !isred[j] )
            w1[k++] = w[j];
        spmat[i] = w1;
        i++;
      }
    }
    /* update nsp */
    nsp0 = nsp;
    nsp = i;

    /* XXX free redmat explicitly */
    for ( k = 0; k < nred; k++ ) {
      GCFREE(BDY(redmat[k]));
      GCFREE(redmat[k]);
    }

    get_eg(&tmp0); add_eg(&eg_elim1,&tmp1,&tmp0);
    init_eg(&eg_split_elim1); add_eg(&eg_split_elim1,&tmp1,&tmp0);

    colstat = (int *)MALLOC_ATOMIC(spcol*sizeof(int));
    bzero(colstat,spcol*sizeof(int));
    for ( i = 0, nonzero=0; i < nsp; i++ )
      for ( j = 0; j < spcol; j++ )
        if ( spmat[i][j] )
          nonzero++;
    if ( DP_Print && nsp )
      fprintf(asir_out,"spmat : %d x %d (nonzero=%f%%)...",
        nsp,spcol,((double)nonzero*100)/(nsp*spcol));
    if ( nsp )
      rank = generic_gauss_elim_mod(spmat,nsp,spcol,m,colstat);
    else
      rank = 0;
    get_eg(&tmp1); add_eg(&eg_elim2,&tmp0,&tmp1);
    init_eg(&eg_split_elim2); add_eg(&eg_split_elim2,&tmp0,&tmp1);

    if ( DP_Print ) {
      fprintf(asir_out,"done rank = %d\n",rank,row,col);
      print_eg("Symb",&eg_split_symb);
      print_eg("Conv",&eg_split_conv);
      print_eg("Elim1",&eg_split_elim1);
      print_eg("Elim2",&eg_split_elim2);
      fprintf(asir_out,"\n");
    }

    NZR += rank;
    ZR += nsp0-rank;

    if ( !rank )
      continue;

    for ( j = 0, i = 0; j < spcol; j++ )
      if ( colstat[j] ) {
        mp0 = 0;
        NEXTMP(mp0,mp); mp->dl = st[j]; mp->c = (Obj)STOI(1);
        for ( k = j+1; k < spcol; k++ )
          if ( !colstat[k] && spmat[i][k] ) {
            NEXTMP(mp0,mp); mp->dl = st[k];
            mp->c = (Obj)STOI(spmat[i][k]);
        }
        NEXT(mp) = 0;
        MKDP(nv,mp0,nf); nf->sugar = dm->sugar;
        nh = newps_mod(nf,m);
        if ( nh == pscalen ) {
          psca = (unsigned int **)
            REALLOC(psca,2*pscalen*sizeof(unsigned int *));
          pscalen *= 2;
        }
        dptoca(ps[nh],&psca[nh]);
        d = updpairs(d,g,nh);
        g = updbase(g,nh);
        gall = append_one(gall,nh);
        i++;
      }

    /* XXX free spmat[] explicitly */
    for ( j = 0; j < nsp; j++ ) {
      GCFREE(spmat[j]);
    }
  }
  if ( DP_Print ) {
    print_eg("Symb",&eg_symb);
    print_eg("Conv",&eg_conv);
    print_eg("Elim1",&eg_elim1);
    print_eg("Elim2",&eg_elim2);
    fflush(asir_out);
  }
  return g;
}

NODE gb_f4_mod_old(NODE f,int m)
{
  int i,j,k,nh,row,col,nv;
  NODE r,g,gall;
  NODE s,s0;
  DP_pairs d,dm,dr,t;
  DP nf,f2,sp,sd,sdm,tdp;
  MP mp,mp0;
  NODE blist,bt;
  DL *ht,*at,*st;
  int **spmat,**redmat;
  int *colstat,*w;
  int rank,nred,nsp,nonzero,spcol;
  int *indred,*isred,*ri;
  struct oEGT tmp0,tmp1,eg_split_symb,eg_split_elim1,eg_split_elim2;
  extern struct oEGT eg_symb,eg_elim1,eg_elim2;

  init_eg(&eg_symb); init_eg(&eg_elim1); init_eg(&eg_elim2);
  for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
    i = (long)BDY(r);
    d = updpairs(d,g,i);
    g = updbase(g,i);
    gall = append_one(gall,i);
  }
  if ( gall )
    nv = ((DP)ps[(long)BDY(gall)])->nv;
  while ( d ) {
    get_eg(&tmp0);
    minsugar(d,&dm,&dr); d = dr;
    if ( DP_Print )
      fprintf(asir_out,"sugar=%d\n",dm->sugar);
    blist = 0; s0 = 0;
    /* asph : sum of all head terms of spoly */
    for ( t = dm; t; t = NEXT(t) ) {
      _dp_sp_mod(ps[t->dp1],ps[t->dp2],m,&sp);
      if ( sp ) {
        MKNODE(bt,sp,blist); blist = bt;
        s0 = symb_merge(s0,dp_dllist(sp),nv);
      }
    }
    /* s0 : all the terms appeared in symbolic redunction */
    for ( s = s0, nred = 0; s; s = NEXT(s) ) {
      for ( r = gall;  r; r = NEXT(r) )
        if ( _dl_redble(BDY(ps[(long)BDY(r)])->dl,BDY(s),nv) )
          break;
      if ( r ) {
        dltod(BDY(s),nv,&tdp);
        dp_subd(tdp,ps[(long)BDY(r)],&sd);
        _dp_mod(sd,m,0,&sdm);
        mulmd_dup(m,sdm,ps[(long)BDY(r)],&f2);
        MKNODE(bt,f2,blist); blist = bt;
        s = symb_merge(s,dp_dllist(f2),nv);
        nred++;
      }
    }
    
    get_eg(&tmp1); add_eg(&eg_symb,&tmp0,&tmp1);
    init_eg(&eg_split_symb); add_eg(&eg_split_symb,&tmp0,&tmp1);

    /* the first nred polys in blist are reducers */
    /* row = the number of all the polys */
    for ( r = blist, row = 0; r; r = NEXT(r), row++ );

    /* head terms of reducers */
    ht = (DL *)MALLOC(nred*sizeof(DL));
    for ( r = blist, i = 0; i < nred; r = NEXT(r), i++ )
      ht[i] = BDY((DP)BDY(r))->dl;

    /* col = number of all terms */
    for ( s = s0, col = 0; s; s = NEXT(s), col++ );

    /* head terms of all terms */
    at = (DL *)MALLOC(col*sizeof(DL));
    for ( s = s0, i = 0; i < col; s = NEXT(s), i++ )
      at[i] = (DL)BDY(s);

    /* store coefficients separately in spmat and redmat */
    nsp = row-nred;

    /* reducer matrix */
    redmat = (int **)almat(nred,col);
    for ( i = 0, r = blist; i < nred; r = NEXT(r), i++ )
      _dpmod_to_vect(BDY(r),at,redmat[i]);
    /* XXX */
/*    reduce_reducers_mod(redmat,nred,col,m); */
    /* register the position of the head term */
    indred = (int *)MALLOC(nred*sizeof(int));
    bzero(indred,nred*sizeof(int));
    isred = (int *)MALLOC(col*sizeof(int));
    bzero(isred,col*sizeof(int));
    for ( i = 0; i < nred; i++ ) {
      ri = redmat[i];
      for ( j = 0; j < col && !ri[j]; j++ );
      indred[i] = j;
      isred[j] = 1;
    }

    spcol = col-nred;
    /* head terms not in ht */
    st = (DL *)MALLOC(spcol*sizeof(DL));
    for ( j = 0, k = 0; j < col; j++ )
      if ( !isred[j] )
        st[k++] = at[j];

    /* spoly matrix; stored in reduced form; terms in ht[] are omitted */
    spmat = almat(nsp,spcol);
    w = (int *)MALLOC(col*sizeof(int));
    for ( ; i < row; r = NEXT(r), i++ ) {
      bzero(w,col*sizeof(int));
      _dpmod_to_vect(BDY(r),at,w);
      reduce_sp_by_red_mod(w,redmat,indred,nred,col,m);
      for ( j = 0, k = 0; j < col; j++ )
        if ( !isred[j] )
          spmat[i-nred][k++] = w[j];
    }

    get_eg(&tmp0); add_eg(&eg_elim1,&tmp1,&tmp0);
    init_eg(&eg_split_elim1); add_eg(&eg_split_elim1,&tmp1,&tmp0);

    colstat = (int *)MALLOC_ATOMIC(spcol*sizeof(int));
    for ( i = 0, nonzero=0; i < nsp; i++ )
      for ( j = 0; j < spcol; j++ )
        if ( spmat[i][j] )
          nonzero++;
    if ( DP_Print && nsp )
      fprintf(asir_out,"spmat : %d x %d (nonzero=%f%%)...",
        nsp,spcol,((double)nonzero*100)/(nsp*spcol));
    if ( nsp )
      rank = generic_gauss_elim_mod(spmat,nsp,spcol,m,colstat);
    else
      rank = 0;
    get_eg(&tmp1); add_eg(&eg_elim2,&tmp0,&tmp1);
    init_eg(&eg_split_elim2); add_eg(&eg_split_elim2,&tmp0,&tmp1);

    if ( DP_Print ) {
      fprintf(asir_out,"done rank = %d\n",rank,row,col);
      print_eg("Symb",&eg_split_symb);
      print_eg("Elim1",&eg_split_elim1);
      print_eg("Elim2",&eg_split_elim2);
      fprintf(asir_out,"\n");
    }
    for ( j = 0, i = 0; j < spcol; j++ )
      if ( colstat[j] ) {
        mp0 = 0;
        NEXTMP(mp0,mp); mp->dl = st[j]; mp->c = (Obj)STOI(1);
        for ( k = j+1; k < spcol; k++ )
          if ( !colstat[k] && spmat[i][k] ) {
            NEXTMP(mp0,mp); mp->dl = st[k];
            mp->c = (Obj)STOI(spmat[i][k]);
        }
        NEXT(mp) = 0;
        MKDP(nv,mp0,nf); nf->sugar = dm->sugar;
        nh = newps_mod(nf,m);
        d = updpairs(d,g,nh);
        g = updbase(g,nh);
        gall = append_one(gall,nh);
        i++;
      }
  }
  if ( DP_Print ) {
    print_eg("Symb",&eg_symb);
    print_eg("Elim1",&eg_elim1);
    print_eg("Elim2",&eg_elim2);
    fflush(asir_out);
  }
  return g;
}

int DPPlength(DP_pairs n)
{
  int i;

  for ( i = 0; n; n = NEXT(n), i++ );
  return i;
}

void printdl(DL dl)
{
  int i;

  fprintf(asir_out,"<<");
  for ( i = 0; i < CNVars-1; i++ )
    fprintf(asir_out,"%d,",dl->d[i]);
  fprintf(asir_out,"%d>>",dl->d[i]);
}

void pltovl(LIST l,VL *vl)
{
  NODE n;
  VL r,r0;

  n = BDY(l);
  for ( r0 = 0; n; n = NEXT(n) ) {
    NEXTVL(r0,r); r->v = VR((P)BDY(n));
  }
  if ( r0 ) NEXT(r) = 0;
  *vl = r0;
}

void vltopl(VL vl,LIST *l)
{
  VL n;
  NODE r,r0;
  P p;

  n = vl;
  for ( r0 = 0; n; n = NEXT(n) ) {
    NEXTNODE(r0,r); MKV(n->v,p); BDY(r) = (pointer)p;
  }
  if ( r0 ) NEXT(r) = 0;
  MKLIST(*l,r0);
}

void makesubst(VL v,NODE *s)
{
  NODE r,r0;
  Z q;
  unsigned int n;

  for ( r0 = 0; v; v = NEXT(v) ) {
    NEXTNODE(r0,r); BDY(r) = (pointer)v->v;
#if defined(_PA_RISC1_1)
    n = mrand48()&BMASK; UTOZ(n,q);
#else
    n = random(); UTOZ(n,q);
#endif
    NEXTNODE(r0,r); BDY(r) = (pointer)q;
  }
  if ( r0 ) NEXT(r) = 0;
  *s = r0;
}

void printsubst(NODE s)
{
  fputc('[',asir_out);
  while ( s ) {
    printv(CO,(V)BDY(s)); s = NEXT(s);
    fprintf(asir_out,"->%d",ZTOS((Q)BDY(s)));
    if ( NEXT(s) ) {
      fputc(',',asir_out); s = NEXT(s);
    } else
      break;
  }
  fprintf(asir_out,"]\n"); return;
}

void vlminus(VL v,VL w,VL *d)
{
  int i,j,n,m;
  V *va,*wa;
  V a;
  VL r,r0;
  VL t;

  for ( n = 0, t = v; t; t = NEXT(t), n++ );
  va = (V *)ALLOCA(n*sizeof(V));
  for ( i = 0, t = v; t; t = NEXT(t), i++ )
    va[i] = t->v;
  for ( m = 0, t = w; t; t = NEXT(t), m++ );
  wa = (V *)ALLOCA(m*sizeof(V));
  for ( i = 0, t = w; t; t = NEXT(t), i++ )
    wa[i] = t->v;
  for ( i = 0; i < n; i++ ) {
    a = va[i];
    for ( j = 0; j < m; j++ )
      if ( a == wa[j] )
        break;
    if ( j < m )
      va[i] = 0;
  }
  for ( r0 = 0, i = 0; i < n; i++ )
    if ( va[i] ) { NEXTVL(r0,r); r->v = va[i];  }
  if ( r0 ) NEXT(r) = 0;
  *d = r0;
}

int validhc(P a,int m,NODE s)
{
  P c,c1;
  V v;

  if ( !a )
    return 0;
  for ( c = a; s; s = NEXT(s) ) {
    v = (V)BDY(s); s = NEXT(s);
    substp(CO,c,v,(P)BDY(s),&c1); c = c1;
  }
  ptomp(m,c,&c1);
  return c1 ? 1 : 0;
}

void setup_arrays(NODE f,int m,NODE *r)
{
  int i;
  NODE s,s0,f0;

#if 1
  f0 = f = NODE_sortb(f,1);
#else
  f0 = f;
#endif
  psn = length(f); pslen = 2*psn;
  ps = (DP *)MALLOC(pslen*sizeof(DP));
  psh = (DL *)MALLOC(pslen*sizeof(DL));
  pss = (int *)MALLOC(pslen*sizeof(int));
  psc = (P *)MALLOC(pslen*sizeof(P));
  for ( i = 0; i < psn; i++, f = NEXT(f) ) {
    prim_part((DP)BDY(f),m,&ps[i]);
    if ( Demand )
      dp_save(i,(Obj)ps[i],0);
    psh[i] = BDY(ps[i])->dl;
    pss[i] = ps[i]->sugar;
    psc[i] = (P)BDY(ps[i])->c;
  }
  if ( GenTrace ) {
    Z q;
    STRING fname;
    LIST input;
    NODE arg,t,t1;
    Obj obj;
  
    t = 0;
    for ( i = psn-1; i >= 0; i-- ) {
      MKNODE(t1,ps[i],t);
      t = t1;
    }
    MKLIST(input,t);

    if ( OXCheck >= 0 ) {
      STOZ(OXCheck,q);
      MKSTR(fname,"register_input");
      arg = mknode(3,q,fname,input);
      Pox_cmo_rpc(arg,&obj);
    } else if ( OXCheck < 0 ) {
      MKNODE(AllTraceList,input,0);
    }
  }
  for ( s0 = 0, i = 0; i < psn; i++ ) {
    NEXTNODE(s0,s); BDY(s) = (pointer)((long)i);
  }
  if ( s0 ) NEXT(s) = 0;
  *r = s0;
}

void prim_part(DP f,int m,DP *r)
{
  P d,t;

  if ( m > 0 ) {
    if ( PCoeffs )
      dp_prim_mod(f,m,r);
    else
      _dp_monic(f,m,r);
  } else {
    if ( dp_fcoeffs || PCoeffs )
      dp_prim(f,r);
    else
      dp_ptozp(f,r);
    if ( GenTrace && TraceList ) {
      /* adust the denominator according to the final 
         content reduction */
      divsp(CO,(P)BDY(f)->c,(P)BDY(*r)->c,&d);
      mulp(CO,(P)ARG3(BDY((LIST)BDY(TraceList))),d,&t);
      ARG3(BDY((LIST)BDY(TraceList))) = t;
    }
  }
}

NODE /* of DP */ NODE_sortb_insert( DP newdp, NODE /* of DP */ nd, int dec )
{
  register NODE last, p;
  register DL newdl = BDY(newdp)->dl;
  register int (*cmpfun)() = cmpdl, nv = CNVars;
  NODE newnd;
  int sgn = dec ? 1 : -1;
  MKNODE( newnd, newdp, 0 );
  if ( !(last = nd) || sgn*(*cmpfun)( nv, newdl, BDY((DP) BDY(last))->dl ) > 0 ) {
    NEXT(newnd) = last;
    return newnd;
  }
  for ( ; p = NEXT(last); last = p )
    if ( sgn*(*cmpfun)( nv, newdl, BDY((DP) BDY(p))->dl ) > 0 ) break;
  if ( p ) NEXT(NEXT(last) = newnd) = p;
  else NEXT(last) = newnd;
  return nd;
}

NODE NODE_sortb( NODE node, int dec )
{
  register NODE nd, ans;

  for ( ans = 0, nd = node; nd; nd = NEXT(nd) )
    ans = NODE_sortb_insert( (DP) BDY(nd), ans, dec );
  return ans;
}

NODE /* of index */ NODE_sortbi_insert( int newdpi, NODE /* of index */ nd, int dec )
{
  register NODE last, p;
  register DL newdl = psh[newdpi];
  register int (*cmpfun)() = cmpdl, nv = CNVars;
  NODE newnd;
  int sgn = dec ? 1 : -1;
  MKNODE( newnd, (pointer)((long)newdpi), 0 );
  if ( !(last = nd) || sgn*(*cmpfun)( nv, newdl, psh[(long)BDY(last)] ) > 0 ) {
    NEXT(newnd) = last;
    return newnd;
  }
  for ( ; p = NEXT(last); last = p )
    if ( sgn*(*cmpfun)( nv, newdl, psh[(long)BDY(p)] ) > 0 ) break;
  if ( p ) NEXT(NEXT(last) = newnd) = p;
  else NEXT(last) = newnd;
  return nd;
}

NODE NODE_sortbi( NODE node, int dec )
{
  register NODE nd, ans;

  for ( ans = 0, nd = node; nd; nd = NEXT(nd) )
    ans = NODE_sortbi_insert( (long) BDY(nd), ans, dec );
  return ans;
}

void reduceall(NODE in,NODE *h)
{
  NODE r,t,top;
  int n,i,j;
  int *w;
  DP g,g1;
  struct oEGT tmp0,tmp1;

  if ( NoRA ) {
    *h = in; return;
  }
  if ( DP_Print || DP_PrintShort ) {
    fprintf(asir_out,"reduceall\n"); fflush(asir_out);
  }
  r = NODE_sortbi(in,0);
  n = length(r);
  w = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, t = r; i < n; i++, t = NEXT(t) )
    w[i] = (long)BDY(t);
  /* w[i] < 0 : reduced to 0 */
  for ( i = 0; i < n; i++ ) {
    for ( top = 0, j = n-1; j >= 0; j-- )
      if ( w[j] >= 0 && j != i ) {
        MKNODE(t,(pointer)((long)w[j]),top); top = t;
      }
    get_eg(&tmp0);
    dp_load(w[i],&ps[w[i]]);

    if ( GenTrace ) {
      Z q;
      NODE node;
      LIST hist;

      STOZ(w[i],q);
      node = mknode(4,ONE,q,ONE,ONE);
      MKLIST(hist,node);
      MKNODE(TraceList,hist,0);
    }
    _dp_nf(top,ps[w[i]],ps,1,&g);
    prim_part(g,0,&g1);
    get_eg(&tmp1); add_eg(&eg_ra,&tmp0,&tmp1);
    if ( DP_Print || DP_PrintShort ) {
      fprintf(asir_out,"."); fflush(asir_out);
    }
    if ( g1 ) {
      w[i] = newps(g1,0,(NODE)0);
    } else {
      w[i] = -1;
    }
  }
  for ( top = 0, j = n-1; j >= 0; j-- ) {
    if ( w[j] >= 0 ) {
      MKNODE(t,(pointer)((long)w[j]),top); top = t;
    }
  }
  *h = top;
  if ( DP_Print || DP_PrintShort )
    fprintf(asir_out,"\n");
}

void reduceall_mod(NODE in,int m,NODE *h)
{
  NODE r,t,top;
  int n,i,j;
  int *w;
  DP g,p;
  struct oEGT tmp0,tmp1;

  if ( NoRA ) {
    *h = in; return;
  }
  if ( DP_Print || DP_PrintShort ) {
    fprintf(asir_out,"reduceall\n"); fflush(asir_out);
  }
  r = NODE_sortbi(in,0);
  n = length(r);
  w = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, t = r; i < n; i++, t = NEXT(t) )
    w[i] = (long)BDY(t);
  /* w[i] < 0 : reduced to 0 */
  for ( i = 0; i < n; i++ ) {
    for ( top = 0, j = n-1; j >= 0; j-- )
      if ( w[j] >= 0 && j != i ) {
        MKNODE(t,(pointer)((long)w[j]),top); top = t;
      }
    get_eg(&tmp0);
    if ( PCoeffs )
      dp_nf_mod(top,ps[w[i]],ps,m,1,&g);
    else {
      dpto_dp(ps[w[i]],&p);
      _dp_nf_mod_destructive(top,p,ps,m,1,&g);
    }
    get_eg(&tmp1); add_eg(&eg_ra,&tmp0,&tmp1);
    if ( DP_Print || DP_PrintShort ) {
      fprintf(asir_out,"."); fflush(asir_out);
    }
    if ( g ) {
      w[i] = newps_mod(g,m);
    } else {
      w[i] = -1;
    }
  }
  for ( top = 0, j = n-1; j >= 0; j-- ) {
    if ( w[j] >= 0 ) {
      MKNODE(t,(pointer)((long)w[j]),top); top = t;
    }
  }
  *h = top;
  if ( DP_Print || DP_PrintShort )
    fprintf(asir_out,"\n");
}

int newps(DP a,int m,NODE subst)
{
  if ( m && !validhc(!a?0:(P)BDY(a)->c,m,subst) )
    return -1;
  if ( psn == pslen ) {
    pslen *= 2;
    ps = (DP *)REALLOC((char *)ps,pslen*sizeof(DP));
    psh = (DL *)REALLOC((char *)psh,pslen*sizeof(DL));
    pss = (int *)REALLOC((char *)pss,pslen*sizeof(int));
    psc = (P *)REALLOC((char *)psc,pslen*sizeof(P));
    if ( m )
      psm = (DP *)REALLOC((char *)psm,pslen*sizeof(DP));
  }
  if ( Demand ) {
    if ( doing_f4 )
      ps[psn] = a;
    else
      ps[psn] = 0;
    dp_save(psn,(Obj)a,0);
  } else
    ps[psn] = a;
  psh[psn] = BDY(a)->dl;
  pss[psn] = a->sugar;
  psc[psn] = (P)BDY(a)->c;
  if ( m )
    _dp_mod(a,m,subst,&psm[psn]);
  if ( GenTrace ) {
    NODE tn,tr,tr1;
    LIST trace,trace1;
    NODE arg;
    Z q1,q2;
    STRING fname;
    Obj obj;
  
    /* reverse the TraceList */
    tn = TraceList;
    for ( tr = 0; tn; tn = NEXT(tn) ) {
      MKNODE(tr1,BDY(tn),tr); tr = tr1;
    }
    MKLIST(trace,tr);
    if ( OXCheck >= 0 ) {
      STOZ(OXCheck,q1);
      MKSTR(fname,"check_trace");
      STOZ(psn,q2);
      arg = mknode(5,q1,fname,a,q2,trace);
      Pox_cmo_rpc(arg,&obj);
    } else if ( OXCheck < 0 ) {
      STOZ(psn,q1);
      tn = mknode(2,q1,trace);
      MKLIST(trace1,tn);
      MKNODE(tr,trace1,AllTraceList);
      AllTraceList = tr;
    } else
      dp_save(psn,(Obj)trace,"t");
    TraceList = 0;
  }
  return psn++;
}

int newps_nosave(DP a,int m,NODE subst)
{
  if ( m && !validhc(!a?0:(P)BDY(a)->c,m,subst) )
    return -1;
  if ( psn == pslen ) {
    pslen *= 2;
    ps = (DP *)REALLOC((char *)ps,pslen*sizeof(DP));
    psh = (DL *)REALLOC((char *)psh,pslen*sizeof(DL));
    pss = (int *)REALLOC((char *)pss,pslen*sizeof(int));
    psc = (P *)REALLOC((char *)psc,pslen*sizeof(P));
    if ( m )
      psm = (DP *)REALLOC((char *)psm,pslen*sizeof(DP));
  }
  ps[psn] = 0;
  psh[psn] = BDY(a)->dl;
  pss[psn] = a->sugar;
  psc[psn] = (P)BDY(a)->c;
  if ( m )
    _dp_mod(a,m,subst,&psm[psn]);
  return psn++;
}

int newps_mod(DP a,int m)
{
  if ( psn == pslen ) {
    pslen *= 2;
    ps = (DP *)REALLOC((char *)ps,pslen*sizeof(DP));
    psh = (DL *)REALLOC((char *)psh,pslen*sizeof(DL));
    pss = (int *)REALLOC((char *)pss,pslen*sizeof(int));
    psc = (P *)REALLOC((char *)psc,pslen*sizeof(P)); /* XXX */
  }
  ps[psn] = a;
  psh[psn] = BDY(ps[psn])->dl;
  pss[psn] = ps[psn]->sugar;
  return psn++;
}

void reducebase_dehomo(NODE f,NODE *g)
{
  long l;
  int n,i,j,k;
  int *r;
  DL *w,d;
  DP u;
  NODE t,top;

  n = length(f);
  w = (DL *)ALLOCA(n*sizeof(DL));
  r = (int *)ALLOCA(n*sizeof(int));
  for ( i = 0, t = f; i < n; i++, t = NEXT(t) ) {
    r[i] = (long)BDY(t); w[i] = psh[r[i]];
  }
  for ( i = 0; i < n; i++ ) {
    for ( j = 0, d = w[i]; j < n; j++ ) {
      if ( j != i ) {
        for ( k = 0; k < NVars; k++ )
          if ( d->d[k] < w[j]->d[k] )
            break;
        if ( k == NVars )
          break;
      }
    }
    if ( j != n )
      r[i] = -1;
  }
  for ( top = 0, i = n-1; i >= 0; i-- )
    if ( r[i] >= 0 ) {
      dp_load(r[i],&ps[r[i]]); dp_dehomo(ps[r[i]],&u);
      if ( GenTrace ) {
        Z q;
        LIST hist;
        NODE node;

        STOZ(r[i],q);
        node = mknode(4,NULLP,q,NULLP,NULLP);
        MKLIST(hist,node);
        MKNODE(TraceList,hist,0);
      }
      l = newps(u,0,0);
      MKNODE(t,(pointer)l,top); top = t;
    }
  *g = top;
}

NODE append_one(NODE f,long n)
{
  NODE t;

  if ( Reverse || !f ) {
    MKNODE(t,(pointer)n,f); return t;
  } else {
    for ( t = f; NEXT(t); t = NEXT(t) );
    MKNODE(NEXT(t),(pointer)n,0);
    return f;
  }
}

DP_pairs minp( DP_pairs d, DP_pairs *prest )
{
  register DP_pairs m, ml, p, l;
  register DL lcm;
  register int s, nv = CNVars;
  register int (*cmpfun)() = cmpdl;

  if ( !(p = NEXT(m = d)) ) {
    *prest = p;
    NEXT(m) = 0;
    return m;
  }
  for ( lcm = m->lcm, s = m->sugar, ml = 0, l = m; p; p = NEXT(l = p) )
    if ( NoSugar ? (*cmpfun)( nv, lcm, p->lcm ) >= 0 :
         (s > p->sugar || s == p->sugar && (*cmpfun)( nv, lcm, p->lcm ) >= 0) )
      ml = l,  lcm = (m = p)->lcm,  s = p->sugar;
  if ( !ml ) *prest = NEXT(m);
  else {
    NEXT(ml) = NEXT(m);
    *prest = d;
  }
  NEXT(m) = 0;
  return m;
}

void minsugar(DP_pairs d,DP_pairs *dm,DP_pairs *dr)
{
  int msugar;
  DP_pairs t,dm0,dr0,dmt,drt;

  for ( msugar = d->sugar, t = NEXT(d); t; t = NEXT(t) )
    if ( t->sugar < msugar )
      msugar = t->sugar;
  dm0 = 0; dr0 = 0;
  for ( t = d; t; t = NEXT(t) ) {
    if ( t->sugar == msugar ) {
      NEXTDPP(dm0,dmt);
      dmt->dp1 = t->dp1; dmt->dp2 = t->dp2;
      dmt->lcm = t->lcm; dmt->sugar = t->sugar;
    } else {
      NEXTDPP(dr0,drt);
      drt->dp1 = t->dp1; drt->dp2 = t->dp2;
      drt->lcm = t->lcm; drt->sugar = t->sugar;
    }
  }
  if ( dm0 ) NEXT(dmt) = 0;
  if ( dr0 ) NEXT(drt) = 0;
  *dm = dm0; *dr = dr0;
}

NODE gb(NODE f,int m,NODE subst)
{
  int i,nh,prev,mag,mag0,magt;
  NODE r,g,gall;
  DP_pairs d;
  DP_pairs l;
  DP h,nf,nfm,dp1,dp2;
  MP mp;
  struct oEGT tnf0,tnf1,tnfm0,tnfm1,tpz0,tpz1,tnp0,tnp1;
  int skip_nf_flag;
  double t_0;
  Z q;
  int new_sugar;
  static int prev_sugar = -1;

  Max_mag = 0;
  Max_coef = 0;
  prev = 1;
  doing_f4 = 0;
  init_denomlist();
  if ( m ) {
    psm = (DP *)MALLOC(pslen*sizeof(DP));
    for ( i = 0; i < psn; i++ )
      if ( psh[i] && !validhc(psc[i],m,subst) )
        return 0;
      else
        _dp_mod(ps[i],m,subst,&psm[i]);
  }
  for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
    i = (long)BDY(r);
    d = updpairs(d,g,i);
    g = updbase(g,i);
    gall = append_one(gall,i);
  }
  while ( d ) {
    l = minp(d,&d);
    if ( m ) {
      _dp_sp_mod_dup(psm[l->dp1],psm[l->dp2],m,&h);
      if ( h )
        new_sugar = h->sugar;
      get_eg(&tnfm0);
      _dp_nf_mod_destructive(gall,h,psm,m,0,&nfm);
      get_eg(&tnfm1); add_eg(&eg_nfm,&tnfm0,&tnfm1);
    } else
      nfm = (DP)1;
    if ( nfm ) {
      if ( Demand ) {
        if ( dp_load_t(psn,&nf) ) {
          skip_nf_flag = 1;
          goto skip_nf;
        } else {
          skip_nf_flag = 0;
          dp_load(l->dp1,&dp1); dp_load(l->dp2,&dp2);
          dp_sp(dp1,dp2,&h);
        }
      } else
        dp_sp(ps[l->dp1],ps[l->dp2],&h);
      if ( GenTrace ) {
        STOZ(l->dp1,q); ARG1(BDY((LIST)BDY(NEXT(TraceList)))) = q;
        STOZ(l->dp2,q); ARG1(BDY((LIST)BDY(TraceList))) = q;
      }
      if ( h )
        new_sugar = h->sugar;
      get_eg(&tnf0);
      t_0 = get_rtime();
      if ( PCoeffs || dp_fcoeffs )
        _dp_nf(gall,h,ps,!Top,&nf);
      else
        _dp_nf_z(gall,h,ps,!Top,DP_Multiple,&nf);
      if ( DP_Print && nf )
        fprintf(asir_out,"(%.3g)",get_rtime()-t_0);
      get_eg(&tnf1); add_eg(&eg_nf,&tnf0,&tnf1);
    } else
      nf = 0;
skip_nf:
    if ( nf ) {
      NZR++;
      get_eg(&tpz0);
      prim_part(nf,0,&h);
      get_eg(&tpz1); add_eg(&eg_pz,&tpz0,&tpz1);
      add_denomlist((P)BDY(h)->c);
      get_eg(&tnp0);
      if ( Demand && skip_nf_flag )
        nh = newps_nosave(h,m,subst);
      else
        nh = newps(h,m,subst);
      get_eg(&tnp1); add_eg(&eg_np,&tnp0,&tnp1);
      if ( nh < 0 )
        return 0;
      d = updpairs(d,g,nh);
      g = updbase(g,nh);
      gall = append_one(gall,nh);
      if ( !dp_fcoeffs && ShowMag ) {
        for ( mag = 0, mag0 = 0, mp = BDY(h); mp; mp = NEXT(mp) ) {
          magt = p_mag((P)mp->c);
          mag0 = MAX(mag0,magt);
          mag += magt;
        }
        Max_coef = MAX(Max_coef,mag0);
        Max_mag = MAX(Max_mag,mag);
      }
      if ( DP_Print ) {
        if ( !prev )
          fprintf(asir_out,"\n");
        print_split_e(&tnf0,&tnf1); print_split_e(&tpz0,&tpz1);
        printdl(psh[nh]);
        fprintf(asir_out,"(%d,%d),nb=%d,nab=%d,rp=%d,sugar=%d",
          l->dp1,l->dp2,length(g),length(gall),DPPlength(d),
          pss[nh]);
        if ( ShowMag )
          fprintf(asir_out,",mag=(%d,%d)",mag,mag0);
        fprintf(asir_out,"\n"); fflush(asir_out);
      } else if ( DP_PrintShort ) {
        fprintf(asir_out,"+"); fflush(asir_out);
      }
      prev = 1;
    } else {
      if ( m )
        add_eg(&eg_znfm,&tnfm0,&tnfm1);
      ZR++;
      if ( DP_Print || DP_PrintShort ) {
        if ( new_sugar != prev_sugar ) {
          fprintf(asir_out,"[%d]",new_sugar);  
          prev_sugar = new_sugar;
        }
        fprintf(asir_out,"."); fflush(asir_out); prev = 0;
      }
    }
  }
  if ( DP_Print || DP_PrintShort )
    fprintf(asir_out,"gb done\n");
  return g;
}

NODE gb_mod(NODE f,int m)
{
  int i,nh,prev;
  NODE r,g,gall;
  DP_pairs d;
  DP_pairs l;
  DP h,nf;
  struct oEGT tnfm0,tnfm1,tpz0,tpz1;

  prev = 1;
  for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
    i = (long)BDY(r);
    d = updpairs(d,g,i);
    g = updbase(g,i);
    gall = append_one(gall,i);
  }
  while ( d ) {
    l = minp(d,&d);
    if ( PCoeffs ) {
      dp_sp_mod(ps[l->dp1],ps[l->dp2],m,&h);
      get_eg(&tnfm0);
      dp_nf_mod(gall,h,ps,m,!Top,&nf);
    } else {
      _dp_sp_mod_dup(ps[l->dp1],ps[l->dp2],m,&h);
      get_eg(&tnfm0);
      _dp_nf_mod_destructive(gall,h,ps,m,!Top,&nf);
    }
    get_eg(&tnfm1); add_eg(&eg_nfm,&tnfm0,&tnfm1);
    if ( nf ) {
      NZR++;
      get_eg(&tpz0);
      prim_part(nf,m,&h);
      get_eg(&tpz1); add_eg(&eg_pz,&tpz0,&tpz1);
      nh = newps_mod(h,m);
      if ( nh < 0 )
        return 0;
      d = updpairs(d,g,nh);
      g = updbase(g,nh);
      gall = append_one(gall,nh);
      if ( DP_Print ) {
        if ( !prev )
          fprintf(asir_out,"\n");
        print_split_eg(&tnfm0,&tnfm1); fflush(asir_out);
        fprintf(asir_out,"(%d,%d),nb=%d,nab=%d,rp=%d,sugar=%d",l->dp1,l->dp2,length(g),length(gall),DPPlength(d),pss[nh]);
        printdl(psh[nh]); fprintf(asir_out,"\n"); fflush(asir_out);
      } else if ( DP_PrintShort ) {
        fprintf(asir_out,"+"); fflush(asir_out);
      }
      prev = 1;
    } else {
      add_eg(&eg_znfm,&tnfm0,&tnfm1);
      ZR++;
      if ( DP_Print || DP_PrintShort ) {
        fprintf(asir_out,"."); fflush(asir_out); prev = 0;
      }
    }
  }
  if ( DP_Print || DP_PrintShort )
    fprintf(asir_out,"gb_mod done\n");
  return g;
}

DP_pairs updpairs( DP_pairs d, NODE /* of index */ g, int t)
{
  register DP_pairs d1, dd, nd;
  int dl,dl1;

  if ( !g ) return d;
  if ( !NoCriB && d ) {
    dl = DPPlength(d);
    d = criterion_B( d, t );
    dl -= DPPlength(d); N_BP += dl;
  }
  d1 = newpairs( g, t );
  if ( NEXT(d1) ) {
    dl = DPPlength(d1); TP += dl;
    d1 = criterion_M( d1 );
    dl1 = DPPlength(d1); NMP += (dl-dl1); dl = dl1;
    d1 = criterion_F( d1 );
    dl1 = DPPlength(d1); NFP += (dl-dl1); dl = dl1;
  } else
    dl = 1;
  if ( !do_weyl )
    for ( dd = 0; d1; d1 = nd ) {
      nd = NEXT(d1);
      if ( !criterion_2( d1->dp1, d1->dp2 ) ) {
        NEXT(d1) = dd;
        dd = d1;
      }
    }
  else
    dd = d1;
  dl1 = DPPlength(dd); NDP += (dl-dl1);
  if ( !(nd = d) ) return dd;
  while ( nd = NEXT(d1 = nd) ) ;
  NEXT(d1) = dd;
  return d;
}

DP_pairs newpairs( NODE /* of index */ g, int t )
{
  register NODE r;
  register DL tdl = psh[t];
  register int ts;
  register DP_pairs p, last;
  int dp;
  register DL dl;
  register int s;

  ts = pss[t] - tdl->td;
  for ( last = 0, r = g; r; r = NEXT(r) ) {
    NEXT(p = NEWDP_pairs) = last;
    last = p;
    dp = p->dp1 = (long)BDY(r);  p->dp2 = t;
    p->lcm = lcm_of_DL(CNVars, dl = psh[dp], tdl, (DL)0 );
#if 0
    if ( do_weyl )
      p->sugar = dl_weyl_weight(p->lcm);
    else
#endif
      p->sugar = (ts > (s = pss[dp] - dl->td) ? ts : s) + p->lcm->td;
  }
  return last;
}

DP_pairs criterion_B( DP_pairs d, int s )
{
  register DP_pairs dd, p;
  register DL tij, t = psh[s], dltmp;

  if ( !d ) return 0;
  NEWDL( dltmp, CNVars );
  for ( dd = 0; d; d = p ) {
    p = NEXT(d),
    tij = d->lcm;
    if ( tij->td != lcm_of_DL(CNVars, tij, t, dltmp )->td
      || !dl_equal(CNVars, tij, dltmp )
      || (tij->td == lcm_of_DL(CNVars, psh[d->dp1], t, dltmp )->td
          && dl_equal(CNVars, dltmp, tij ))
      || (tij->td == lcm_of_DL(CNVars, psh[d->dp2], t, dltmp )->td
          && dl_equal(CNVars, dltmp, tij )) ) {
      NEXT(d) = dd;
      dd = d;
    }
  }
  return dd;
}

DP_pairs criterion_M( DP_pairs d1 )
{
  register DP_pairs dd, e, d3, d2, p;
  register DL itdl, jtdl;
  register int itdltd, jtdltd;

  for ( dd = 0, e = d1; e; e = d3 ) {
    if ( !(d2 = NEXT(e)) ) {
      NEXT(e) = dd;
      return e;
    }
    itdltd = (itdl = e->lcm)->td;
    for ( d3 = 0; d2; d2 = p ) {
      p = NEXT(d2),
      jtdltd = (jtdl = d2->lcm)->td;
      if ( jtdltd == itdltd  )
        if ( dl_equal(CNVars, itdl, jtdl ) ) ;
        else if ( dl_redble( jtdl, itdl ) ) continue;
        else if ( dl_redble( itdl, jtdl ) ) goto delit;
        else ;
      else if ( jtdltd > itdltd )
        if ( dl_redble( jtdl, itdl ) ) continue;
        else ;
      else if ( dl_redble( itdl, jtdl ) ) goto delit;
      NEXT(d2) = d3;
      d3 = d2;
    }
    NEXT(e) = dd;
    dd = e;
    continue;
    /**/
  delit:  NEXT(d2) = d3;
    d3 = d2;
    for ( ; p; p = d2 ) {
      d2 = NEXT(p);
      NEXT(p) = d3;
      d3 = p;
    }
  }
  return dd;
}

static DP_pairs collect_pairs_of_hdlcm( DP_pairs d1, DP_pairs *prest )
{
  register DP_pairs w, p, r, s;
  register DL ti;
  register int td;

  td = (ti = (w = d1)->lcm)->td;
  s = NEXT(w);
  NEXT(w) = 0;
  for ( r = 0; s; s = p ) {
    p = NEXT(s);
    if ( td == s->lcm->td && dl_equal(CNVars, ti, s->lcm ) )
    {
      NEXT(s) = w;
      w = s;
    } else {
      NEXT(s) = r;
      r = s;
    }
  }
  *prest = r;
  return w;
}

int criterion_2( int dp1, int dp2 )
{
  register int i, *d1, *d2;

  d1 = psh[dp1]->d,  d2 = psh[dp2]->d;
  for ( i = CNVars; --i >= 0; d1++, d2++ )
    if ( (*d1 <= *d2 ? *d1 : *d2) > 0  ) return 0;
  return 1;
}

DP_pairs criterion_F( DP_pairs d1 )
{
  DP_pairs rest, head;
  register DP_pairs last, p, r, w;
  register int s;

  for ( head = last = 0, p = d1; NEXT(p); ) {
    s = (r = w = collect_pairs_of_hdlcm( p, &rest ))->sugar;
    while ( w = NEXT(w) )
      if ( !do_weyl && criterion_2( w->dp1, w->dp2 ) ) {
        r = w;
        break;
      } else if ( w->sugar < s ) s = (r = w)->sugar;
    if ( last ) NEXT(last) = r;
    else head = r;
    NEXT(last = r) = 0;
    if ( !(p = rest) ) return head;
  }
  if ( !last ) return p;
  NEXT(last) = p;
  return head;
}

NODE updbase(NODE g,int t)
{
  g = remove_reducibles(g,t);
  g = append_one(g,t);
  return g;
}

NODE /* of index */ remove_reducibles(NODE /* of index */ nd, int newdp )
{
  register DL dl, dln;
  register NODE last, p, head;
  register int td;

  dl = psh[newdp];
  td = dl->td;
  for ( head = last = 0, p = nd; p; ) {
    dln = psh[(long)BDY(p)];
    if ( dln->td >= td && dl_redble( dln, dl ) ) {
      p = NEXT(p);
      if ( last ) NEXT(last) = p;
    } else {
      if ( !last ) head = p;
      p = NEXT(last = p);
    }
  }
  return head;
}

int dl_redble(DL dl1,DL dl2)
{
  register int n, *d1, *d2;

  for ( d1 = dl1->d, d2 = dl2->d, n = CNVars; --n >= 0; d1++, d2++ )
    if ( *d1 < *d2 ) return 0;
  return 1;
}

#if 0
int dl_weyl_weight(DL dl)
{
  int n,w,i;

  n = CNVars/2;
  for ( i = 0, w = 0; i < n; i++ )
    w += (-dl->d[i]+dl->d[n+i]);
  return w;
}
#endif

int gbcheck(NODE f)
{
  int i;
  NODE r,g,gall;
  DP_pairs d,l;
  DP h,nf,dp1,dp2;
  struct oEGT tmp0,tmp1;

  if ( NoGC )
    return 1;
  for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
    i = (long)BDY(r);
    d = updpairs(d,g,i);
    g = updbase(g,i);
    gall = append_one(gall,i);
  }
  if ( DP_Print || DP_PrintShort ) {
    fprintf(asir_out,"gbcheck total %d pairs\n",DPPlength(d)); fflush(asir_out);
  }
  while ( d ) {
    l = d; d = NEXT(d);
    get_eg(&tmp0);
    dp_load(l->dp1,&dp1); dp_load(l->dp2,&dp2);
    dp_sp(dp1,dp2,&h);
/* fprintf(stderr,"{%d,%d}",l->dp1,l->dp2); */
    _dp_nf(gall,h,ps,1,&nf);
    get_eg(&tmp1); add_eg(&eg_gc,&tmp0,&tmp1);
    if ( DP_Print || DP_PrintShort ) {
      fprintf(asir_out,"."); fflush(asir_out);
    }
    if ( nf )
      return 0;
  }
  if ( DP_Print || DP_PrintShort )
    fprintf(asir_out,"\n");
  return 1;
}

void gbcheck_list(NODE f,int n,VECT *gp,LIST *pp)
{
  int i;
  NODE r,g,gall,u,u0,t;
  VECT vect;
  LIST pair;
  DP_pairs d,l;
  Z q1,q2;

  /* we need the following settings */
  NVars = CNVars = n;
  setup_arrays(f,0,&r);
  for ( gall = g = 0, d = 0; r; r = NEXT(r) ) {
    i = (long)BDY(r);
    d = updpairs(d,g,i);
    g = updbase(g,i);
    gall = append_one(gall,i);
  }
  NEWVECT(vect); vect->len = psn; vect->body = (pointer)ps;
  *gp = vect;

  for ( u0 = 0, l = d; l; l = NEXT(l) ) {
    NEXTNODE(u0,u);
    STOZ(l->dp1,q1);
    STOZ(l->dp2,q2);
    t = mknode(2,q1,q2);
    MKLIST(pair,t);
    BDY(u) = (pointer)pair;
  }
  if ( u0 )
    NEXT(u) = 0;
  MKLIST(*pp,u0);
}

int membercheck(NODE f,NODE x)
{
  DP g;
  struct oEGT tmp0,tmp1;

  if ( NoMC )
    return 1;
  if ( DP_Print || DP_PrintShort ) {
    fprintf(asir_out,"membercheck\n"); fflush(asir_out);
  }
  for ( ; f; f = NEXT(f) ) {
    get_eg(&tmp0);
    _dp_nf(x,(DP)BDY(f),ps,1,&g);
    get_eg(&tmp1); add_eg(&eg_mc,&tmp0,&tmp1);
    if ( DP_Print ) {
      print_split_eg(&tmp0,&tmp1); fflush(asir_out);
    } else if ( DP_PrintShort ) {
      fprintf(asir_out,"."); fflush(asir_out);
    }
    if ( g )
      return 0;
  }
  if ( DP_Print || DP_PrintShort )
    fprintf(asir_out,"\n");
  return 1;
}

void dp_set_flag(Obj name,Obj value)
{
  char *n;
  int v;
  Q ratio;
  Z t;

  if ( OID(name) != O_STR )
    return;
  n = BDY((STRING)name);
  if ( !strcmp(n,"Demand") ) {
    Demand = value ? BDY((STRING)value) : 0; return;
  }
  if ( !strcmp(n,"Dist") ) {
    Dist = (LIST)value; return;
  }
  if ( !strcmp(n,"Content") ) {
    ratio = (Q)value;
    if ( ratio ) {
      nmq(ratio,&t);
      DP_Multiple = ZTOS(t);
      dnq(ratio,&t);
      Denominator = ZTOS(t);
    } else {
      DP_Multiple = 0;
      Denominator = 1;
    }
  }
  if ( value && OID(value) != O_N )
    return;
  v = ZTOS((Q)value);
  if ( !strcmp(n,"NoSugar") )
    NoSugar = v;
  else if ( !strcmp(n,"NoCriB") )
    NoCriB = v;
  else if ( !strcmp(n,"NoGC") )
    NoGC = v;
  else if ( !strcmp(n,"NoMC") )
    NoMC = v;
  else if ( !strcmp(n,"NoRA") )
    NoRA = v;
  else if ( !strcmp(n,"NoGCD") )
    NoGCD = v;
  else if ( !strcmp(n,"Top") )
    Top = v;
  else if ( !strcmp(n,"ShowMag") )
    ShowMag = v;
  else if ( !strcmp(n,"PrintShort") )
    DP_PrintShort = v;
  else if ( !strcmp(n,"Print") )
    DP_Print = v;
  else if ( !strcmp(n,"NFStat") )
    DP_NFStat = v;
  else if ( !strcmp(n,"Stat") )
    Stat = v;
  else if ( !strcmp(n,"Reverse") )
    Reverse = v;
  else if ( !strcmp(n,"Multiple") )
    DP_Multiple = v;
  else if ( !strcmp(n,"Denominator") )
    Denominator = v;
  else if ( !strcmp(n,"PtozpRA") )
    PtozpRA = v;
  else if ( !strcmp(n,"GenTrace") )
    GenTrace = v;
  else if ( !strcmp(n,"OXCheck") )
    OXCheck = v;
  else if ( !strcmp(n,"GenSyz") )
    GenSyz = v;
  else if ( !strcmp(n,"OneZeroHomo") )
    OneZeroHomo = v;
  else if ( !strcmp(n,"MaxDeg") )
    MaxDeg = v;
}

void dp_make_flaglist(LIST *list)
{
  Z v,nm,dn;
  Q r;
  STRING name,path;
  NODE n,n1;

#if 0
  STOZ(DP_Multiple,v); MKNODE(n,v,0); MKSTR(name,"DP_Multiple"); MKNODE(n1,name,n); n = n1;
  STOZ(Denominator,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"Denominator"); MKNODE(n1,name,n); n = n1;
#else
  if ( DP_Multiple ) {
    STOZ(DP_Multiple,nm); STOZ(Denominator,dn); divq((Q)nm,(Q)dn,&r);
  } else
    v = 0;
  MKNODE(n,v,0); MKSTR(name,"Content"); MKNODE(n1,name,n); n = n1;
#endif
  MKNODE(n1,Dist,n); n = n1; MKSTR(name,"Dist"); MKNODE(n1,name,n); n = n1;
  STOZ(Reverse,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"Reverse"); MKNODE(n1,name,n); n = n1;
  STOZ(Stat,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"Stat"); MKNODE(n1,name,n); n = n1;
  STOZ(DP_Print,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"Print"); MKNODE(n1,name,n); n = n1;
  STOZ(DP_PrintShort,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"PrintShort"); MKNODE(n1,name,n); n = n1;
  STOZ(DP_NFStat,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NFStat"); MKNODE(n1,name,n); n = n1;
  STOZ(OXCheck,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"OXCheck"); MKNODE(n1,name,n); n = n1;
  STOZ(GenTrace,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"GenTrace"); MKNODE(n1,name,n); n = n1;
  STOZ(GenSyz,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"GenSyz"); MKNODE(n1,name,n); n = n1;
  STOZ(MaxDeg,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"MaxDeg"); MKNODE(n1,name,n); n = n1;
  STOZ(OneZeroHomo,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"OneZeroHomo"); MKNODE(n1,name,n); n = n1;
  STOZ(PtozpRA,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"PtozpRA"); MKNODE(n1,name,n); n = n1;
  STOZ(ShowMag,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"ShowMag"); MKNODE(n1,name,n); n = n1;
  STOZ(Top,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"Top"); MKNODE(n1,name,n); n = n1;
  STOZ(NoGCD,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoGCD"); MKNODE(n1,name,n); n = n1;
  STOZ(NoRA,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoRA"); MKNODE(n1,name,n); n = n1;
  STOZ(NoMC,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoMC"); MKNODE(n1,name,n); n = n1;
  STOZ(NoGC,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoGC"); MKNODE(n1,name,n); n = n1;
  STOZ(NoCriB,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoCriB"); MKNODE(n1,name,n); n = n1;
  STOZ(NoSugar,v); MKNODE(n1,v,n); n = n1; MKSTR(name,"NoSugar"); MKNODE(n1,name,n); n = n1;
  if ( Demand )
    MKSTR(path,Demand);
  else
    path = 0;
  MKNODE(n1,path,n); n = n1; MKSTR(name,"Demand"); MKNODE(n1,name,n); n = n1;
  MKLIST(*list,n);
}

#define DELIM '/'

void dp_save(int index,Obj p,char *prefix)
{
  FILE *fp;
  char path[BUFSIZ];

  if ( prefix )
    sprintf(path,"%s%c%s%d",Demand,DELIM,prefix,index);
  else
    sprintf(path,"%s%c%d",Demand,DELIM,index);
  if ( !(fp = fopen(path,"wb") ) )
    error("dp_save : cannot open a file");
  savevl(fp,VC); saveobj(fp,p); fclose(fp);
}

void dp_load(int index,DP *p)
{
  FILE *fp;
  char path[BUFSIZ];

  if ( !Demand || ps[index] )
    *p = ps[index];
  else {
    sprintf(path,"%s%c%d",Demand,DELIM,index);
    if ( !(fp = fopen(path,"rb") ) )
      error("dp_load : cannot open a file");
    if ( PCoeffs )
      loadvl(fp);
    else
      skipvl(fp);
    loadobj(fp,(Obj *)p); fclose(fp);
  }
}

int dp_load_t(int index,DP *p)
{
  FILE *fp;
  char path[BUFSIZ];

  sprintf(path,"%s%c%d",Demand,DELIM,index);
  if ( !(fp = fopen(path,"rb") ) )
    return 0;
  else {
    if ( PCoeffs )
      loadvl(fp);
    else
      skipvl(fp);
    loadobj(fp,(Obj *)p); fclose(fp); return 1;
  }
}

void init_stat() {
  init_eg(&eg_nf); init_eg(&eg_nfm); init_eg(&eg_znfm);
  init_eg(&eg_pz); init_eg(&eg_np);
  init_eg(&eg_ra); init_eg(&eg_mc); init_eg(&eg_gc);
  ZR = NZR = TP = NMP = N_BP = NFP = NDP = 0;
}

void print_stat() {
  if ( !DP_Print && !Stat )
    return;
  print_eg("NF",&eg_nf); print_eg("NFM",&eg_nfm); print_eg("ZNFM",&eg_znfm);
  print_eg("PZ",&eg_pz); print_eg("NP",&eg_np);
  print_eg("RA",&eg_ra); print_eg("MC",&eg_mc); print_eg("GC",&eg_gc);
  fprintf(asir_out,"T=%d,B=%d M=%d F=%d D=%d ZR=%d NZR=%d\n",TP,N_BP,NMP,NFP,NDP,ZR,NZR);
}

/*
 * dp_nf used in gb()
 *
 */

double pz_t_e, pz_t_d, pz_t_d1, pz_t_c, im_t_s, im_t_r;

extern int GenTrace;
extern NODE TraceList;
extern int mpi_mag;

void dp_mulc_d(DP p,P c,DP *r)
{
  if ( Dist && BDY(Dist)
    && HMAG(p) > mpi_mag
    && p_mag((P)c) > mpi_mag ) {
    if ( DP_NFStat ) fprintf(asir_out,"~");
    dp_imul_d(p,(Q)c,r);
  } else {
    if ( DP_NFStat ) fprintf(asir_out,"_");
    muldc(CO,p,(Obj)c,r);
  }
}

void _dp_nf(NODE b,DP g,DP *ps,int full,DP *rp)
{
  DP u,p,d,s,t,mult;
  P coef;
  NODE l;
  MP m,mr;
  int sugar,psugar;

  if ( !g ) {
    *rp = 0; return;
  }
  sugar = g->sugar;
  for ( d = 0; g; ) {
    for ( u = 0, l = b; l; l = NEXT(l) ) {
      if ( dl_redble(BDY(g)->dl,psh[(long)BDY(l)]) ) {
        dp_load((long)BDY(l),&p);
        /* t+u = coef*(d+g) - mult*p (t = coef*d) */
        dp_red(d,g,p,&t,&u,&coef,&mult);
        psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
        sugar = MAX(sugar,psugar);
        if ( GenTrace ) {
          LIST hist;
          Z cq;
          NODE node,node0;

          STOZ((long)BDY(l),cq);
          node0 = mknode(4,coef,cq,mult,ONE);
          MKLIST(hist,node0);
          MKNODE(node,hist,TraceList); TraceList = node;
        }
        if ( !u ) {
          if ( d )
            d->sugar = sugar;
          *rp = d; return;
        }
        d = t;
        break;
      }
    }
    if ( u )
      g = u;
    else if ( !full ) {
      if ( g ) {
        MKDP(g->nv,BDY(g),t); t->sugar = sugar; g = t;
      }
      *rp = g; return;
    } else {
      m = BDY(g); NEWMP(mr); mr->dl = m->dl; mr->c = m->c;
      NEXT(mr) = 0; MKDP(g->nv,mr,t); t->sugar = mr->dl->td;
      addd(CO,d,t,&s); d = s;
      dp_rest(g,&t); g = t;
    }
  }
  if ( d )
    d->sugar = sugar;
  *rp = d;
}

void _dp_nf_z(NODE b,DP g,DP *ps,int full,int multiple,DP *r)
{
  DP u,dp,rp,t,t1,red,shift;
  Z hr,hred,cont,dc,c,dcq,cq,cr,rcq,gcd,rc,cred,mcred;
  Z rcred,mrcred;
  NODE l;
  int hmag,denom;
  int sugar,psugar;
  STRING imul;
  double t_0,tt,t_p,t_m,t_g,t_a;
  LIST hist;
  NODE node;

  if ( !g ) {
    *r = 0; return;
  }
  pz_t_e = pz_t_d = pz_t_d1 = pz_t_c = 0;
  t_p = t_m = t_g = t_a = 0;

  denom = Denominator?Denominator:1;
  hmag = multiple*HMAG(g)/denom;
  sugar = g->sugar;

  dc = 0; dp = 0; rc = ONE; rp = g;
  MKSTR(imul,"dp_imul_index");

  /* g = dc*dp+rc*rp */
  for ( ; rp; ) {
    for ( u = 0, l = b; l; l = NEXT(l) ) {
      if ( dl_redble(BDY(rp)->dl,psh[(long)BDY(l)]) ) {
        t_0 = get_rtime();
        dp_load((long)BDY(l),&red);
        hr = (Z)BDY(rp)->c; hred = (Z)BDY(red)->c;
        igcd_cofactor(hr,hred,&gcd,&cred,&cr);
        tt = get_rtime(); t_p += tt-t_0;

        dp_subd(rp,red,&shift);
        dp_mulc_d(rp,(P)cr,&t);
        chsgnz(cred,&mcred);
        dp_mulc_d(red,(P)mcred,&t1);
        muld(CO,shift,t1,&t1);
        addd(CO,t,t1,&u);
        t_m += get_rtime()-tt;

        psugar = (BDY(rp)->dl->td - BDY(red)->dl->td) + red->sugar;
        sugar = MAX(sugar,psugar);

        if ( GenTrace ) {
          /* u = cr*rp + (-cred)*shift*red */ 
          STOZ((long)BDY(l),cq);
          node = mknode(4,cr,cq,NULLP,NULLP);
          mulz(cred,rc,&rcred);
          chsgnz(rcred,&mrcred);
          muldc(CO,shift,(Obj)mrcred,(DP *)&ARG2(node));
          MKLIST(hist,node);
        }

        if ( !u ) {
          if ( dp )
            dp->sugar = sugar;
          *r = dp;
          if ( GenTrace ) {
            ARG3(BDY(hist)) = ONE;
            MKNODE(node,hist,TraceList); TraceList = node;
          }
          goto final;
        }
        break;
      }
    }
    if ( u ) {
      if ( multiple && HMAG(u) > hmag ) {
        t_0 = get_rtime();
        dp_ptozp(u,&rp);
        tt = get_rtime(); t_g += tt-t_0;

        divsz((Z)BDY(u)->c,(Z)BDY(rp)->c,&cont);
        if ( !dp_fcoeffs && DP_NFStat ) {
          fprintf(asir_out,
            "(%d)",p_mag((P)cont)*100/p_mag((P)BDY(u)->c));
          fflush(asir_out);
        }
        mulz(cr,dc,&dcq); mulz(cont,rc,&rcq);
        igcd_cofactor(dcq,rcq,&gcd,&dc,&rc);
        t_a = get_rtime()-tt;

        hmag = multiple*HMAG(rp)/denom;
        if ( GenTrace ) {
          ARG3(BDY(hist)) = (pointer)gcd;
          MKNODE(node,hist,TraceList); TraceList = node;
        }
      } else {
        rp = u;
        t_0 = get_rtime();
        mulz(cr,dc,&dc);
        t_a += get_rtime()-t_0;
        if ( GenTrace ) {
          ARG3(BDY(hist)) = (pointer)ONE;
          MKNODE(node,hist,TraceList); TraceList = node;
        }
      }
    } else if ( !full ) {
      if ( rp ) {
        MKDP(rp->nv,BDY(rp),t); t->sugar = sugar; rp = t;
      }
      *r = rp;
      goto final;
    } else {
      t_0 = get_rtime();
      mulz((Z)BDY(rp)->c,rc,&c);
      igcd_cofactor(dc,c,&dc,&dcq,&cq);
      muldc(CO,dp,(Obj)dcq,&t);
      dp_hm(rp,&t1); BDY(t1)->c = (Obj)cq;  addd(CO,t,t1,&dp);
      dp_rest(rp,&rp);
      t_a += get_rtime()-t_0;
    }
  }
  if ( GenTrace ) {
    mulz((Z)ARG3(BDY((LIST)BDY(TraceList))),dc,&cq);
    ARG3(BDY((LIST)BDY(TraceList))) = (pointer)cq;
  }
  if ( dp )
    dp->sugar = sugar;
  *r = dp;
final:
  if ( DP_NFStat )
    fprintf(asir_out,
      "(%.3g %.3g %.3g %.3g %.3g %.3g %.3g %.3g)",
      t_p,t_m,t_g,t_a,
      pz_t_e, pz_t_d, pz_t_d1, pz_t_c);
}

void imulv();

void dp_imul_d(DP p,Q q,DP *rp)
{
  int nsep,ndist,i,j,k,l,n;
  double t0,t1,t2;
  Q *s;
  pointer *b;
  VECT c,cs,ri;
  VECT *r;
  MP m;
  NODE tn,dist,n0;
  Obj obj;
  STRING imul;
  extern LIST Dist;

  if ( !p || !q ) {
    *rp = 0; return;
  }
  dist = BDY(Dist);
  for ( tn = dist, ndist = 0; tn; tn = NEXT(tn), ndist++ );
  nsep = ndist + 1;
  for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
  if ( n <= nsep ) {
    muldc(CO,p,(Obj)q,rp); return;
  }
  MKSTR(imul,"imulv");
  t0 = get_rtime();
  dp_dtov(p,&c);
  sepvect(c,nsep,&cs);
  r = (VECT *)CALLOC(nsep,sizeof(VECT *));
  for ( i = 0, tn = dist, b = BDY(cs); i < ndist; i++, tn = NEXT(tn) ) {
    n0 = mknode(4,BDY(tn),imul,b[i],q);
    Pox_rpc(n0,&obj);
  }
  t1 = get_rtime();
  im_t_s += t1 - t0;
  imulv(b[i],q,&r[i]);
  t1 = get_rtime();
  for ( i = 0, tn = dist; i < ndist; i++, tn = NEXT(tn) ) {
    MKNODE(n0,BDY(tn),0);
    Pox_pop_local(n0,&obj); r[i] = (VECT)obj;
    if ( OID(r[i]) == O_ERR ) {
      printexpr(CO,(Obj)r[i]);
      error("dp_imul_d : aborted");
    }
  }
  t2 = get_rtime();
  im_t_r += t2 - t1;
  s = (Q *)CALLOC(n,sizeof(Q));
  for ( i = j = 0; i < nsep; i++ ) {
    for ( k = 0, ri = r[i], l = ri->len; k < l; k++, j++ ) {
      s[j] = (Q)BDY(ri)[k];
    }
  }
  dp_vtod(s,p,rp);
}

void imulv(VECT w,Q c,VECT *rp)
{
  int n,i;
  VECT r;

  n = w->len;
  MKVECT(r,n); *rp = r;
  for ( i = 0; i < n; i++ )
    mulq((Q)BDY(w)[i],(Q)c,(Q *)&BDY(r)[i]);
}

void dptoca(DP p,unsigned int **rp)
{
  int i;
  MP m;
  unsigned int *r;

  if ( !p )
    *rp = 0;
  else {
    for ( m = BDY(p), i = 0; m; m = NEXT(m), i++ );
    *rp = r = (unsigned int *)MALLOC_ATOMIC(i*sizeof(unsigned int));
    for ( m = BDY(p), i = 0; m; m = NEXT(m), i++ )
      r[i] = ITOS(C(m));
  }
}

