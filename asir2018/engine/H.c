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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/H.c,v 1.2 2018/09/28 08:20:28 noro Exp $
*/
#include "ca.h"
#include "inline.h"
#include "base.h"
#include <math.h>

#if 1
#define Mulum mulum
#define Divum divum  
#define Mulsum mulsum
#define Gcdum gcdum  
#endif
    
#define FCTR 0
#define SQFR 1
#define DDD 2
#define NEWDDD 3
      
LUM LUMALLOC(int,int);

void berle(int index,int count,P f,ML *listp)
{
  UM wf,wf1,wf2,wfs,gcd;
  ML flist;
  int fn,fn1,fm,m,n,fhd;
  register int i,j,inv,hd,*ptr,*ptr1;

  n = UDEG(f);
  wf = W_UMALLOC(n); wf1 = W_UMALLOC(n); wf2 = W_UMALLOC(n);
  wfs = W_UMALLOC(n); gcd = W_UMALLOC(n);
  for ( j = 0, fn = n + 1; (j < count) && (fn > 1); ) {
    m = sprime[index++];
    if ( !remqi((Q)UCOEF(f),m) ) 
      continue;
    ptoum(m,f,wf); cpyum(wf,wf1); 
    diffum(m,wf1,wf2); gcdum(m,wf1,wf2,gcd);
    if ( DEG(gcd) > 0 ) 
      continue;
    hd = COEF(wf)[n]; inv = invm(hd,m);
    for ( i = n, ptr = COEF(wf); i >= 0; i-- ) 
      ptr[i] = ( ptr[i] * inv ) % m;
    fn1 = berlecnt(m,wf);
    if ( fn1 < fn ) {
      fn = fn1; fm = m; fhd = hd;
      for ( i = n, ptr = COEF(wf), ptr1 = COEF(wfs); i >= 0; i-- ) 
        ptr1[i] = ptr[i];
    }
    j++;
  }
  DEG(wfs) = n;
  *listp = flist = MLALLOC(fn); flist->n = fn; flist->mod = fm;
/*  berlemain(fm,wfs,(UM *)flist->c); */
  if ( fm == 2 )
    berlemain(fm,wfs,(UM *)flist->c);
  else
    newddd(fm,wfs,(UM *)flist->c);
  for ( i = DEG((UM)(flist->c[0])),
    ptr = COEF((UM)(flist->c[0])),
    hd = fhd, m = fm; i >= 0; i-- )
    ptr[i] = ( ptr[i] * hd ) % m;
}

int berlecnt(int mod,UM f)
{
  register int i,j,**c;
  int d,dr,n;
  UM w,q;
  int **almat(int,int);

  n = DEG(f); c = almat(n,n);
  w = W_UMALLOC(mod + n); q = W_UMALLOC(mod + n);
  for ( i = 1; ( d = ( mod * i ) ) < n; i++ ) 
    c[d][i - 1] = 1;
  DEG(w) = d; COEF(w)[d] = 1;
  for ( j = d - 1; j >= 0; j-- )
    COEF(w)[j] = 0;
  for ( ; ( i < n ) && ( ( dr = divum(mod,w,f,q) ) != -1 ); i++ ) {
    for ( j = dr; j >= 0; j-- ) 
      COEF(w)[j + mod] = c[j][i - 1] = COEF(w)[j];
    for ( j = mod - 1; j >= 0; j-- ) 
      COEF(w)[j] = 0;
    DEG(w) = dr + mod;
  }
  for ( i = 1; i < n; i++ ) 
    c[i][i - 1] = ( c[i][i - 1] + mod - 1 ) % mod;
  return berlecntmain(mod,n,n-1,c);
}

/* XXX berlecntmain should not be used for large mod */

int berlecntmain(int mod,int n,int m,int **c)
{
  register int *p1,*p2,i,j,k,l,a;
  int *tmp,inv;
  int cfs;

  for ( cfs = 1, j = k = 0; j < m; j++ ) {
    for ( i = k; ( n > i ) && ( c[i][j] == 0 ); i++ );
    if ( i == n ) {
      cfs++; continue;
    }
    if ( i != k ) {
      tmp = c[i]; c[i] = c[k]; c[k] = tmp;
    }
    p1 = c[k]; inv = invm((p1[j] + mod) % mod,mod);
    for ( l = j; l < m; l++ )
      p1[l] = ( p1[l] * inv ) % mod;
    for ( i = k + 1; i < n; c[i][j] = 0, i++ ) 
      if ( i != k && ( a = -c[i][j] ) ) 
        for ( l = j + 1, p2 = c[i]; l < m; l++ ) 
          p2[l] = (a*p1[l] + p2[l]) % mod;
    k++;
  }
  return ( cfs );
}

UM *berlemain(int mod,UM f,UM *fp)
{
  UM wg,ws,wf,f0,gcd,q;
  int n;
  register int i;

  n = DEG(f); wg = W_UMALLOC(n); mini(mod,f,wg);
  if ( DEG(wg) <= 0 ) {
    f0 = UMALLOC(n); cpyum(f,f0); *fp++ = f0;
    return ( fp );
  }
  f0 = W_UMALLOC(n); cpyum(f,f0);
  ws = W_UMALLOC(n); wf = W_UMALLOC(n);
  q = W_UMALLOC(n); gcd = W_UMALLOC(n);
  for ( i = 0; i < mod; i++ ) {
    cpyum(f0,wf); cpyum(wg,ws);
    COEF(ws)[0] = ( COEF(ws)[0] + mod - i ) % mod;
    gcdum(mod,wf,ws,gcd);
    if ( DEG(gcd) > 0 ) {
      if ( DEG(gcd) < n ) {
        divum(mod,f0,gcd,q); f0 = q; fp = berlemain(mod,gcd,fp);
      }
      break;
    }
  }
  fp = berlemain(mod,f0,fp);
  return ( fp );
}

void hensel(int index,int count,P f,ML *listp)
{
  register int i,j;
  int q,n,bound;
  int *p;
  int **pp;
  ML blist,clist,bqlist,cqlist,rlist;
  UM *b;
  LUM fl,tl;
  LUM *l;

  if ( UDEG(f) == 1 ) {
    *listp = blist = MLALLOC(1); blist->n = 1; blist->c[0] = 0;
    return;
  }
  berle(index,count,f,&blist);
  if ( blist->n == 1 ) {
    *listp = blist = MLALLOC(1); blist->n = 1; blist->c[0] = 0;
    return;
  }
  gcdgen(f,blist,&clist); henprep(f,blist,clist,&bqlist,&cqlist);
  n = bqlist->n; q = bqlist->mod;
  bqlist->bound = cqlist->bound = bound = mignotte(q,f);
  if ( bound == 1 ) {
    *listp = rlist = MLALLOC(n); 
    rlist->n = n; rlist->mod = q; rlist->bound = bound;
    for ( i = 0, b = (UM *)bqlist->c, l = (LUM *)rlist->c; i < n; i++ ) {
      tl = LUMALLOC(DEG(b[i]),1); l[i] = tl; p = COEF(b[i]);
      for ( j = 0, pp = COEF(tl); j <= DEG(tl); j++ )
          pp[j][0] = p[j];
    }
  } else {
    W_LUMALLOC((int)UDEG(f),bound,fl);
    ptolum(q,bound,f,fl); henmain(fl,bqlist,cqlist,listp);
  }
}

void hensel2(int index,int count,P f,ML *listp)
{
  register int i,j;
  int mod,q,n,bound,dx;
  ML blist,clist,bqlist,cqlist,rlist;
  UM fm,qfm,gm,qgm,hm,qhm,qam,qbm,w;
  UM *b;
  LUM fl,tl;
  int k;

  dx = UDEG(f);
  if ( dx == 1 ) {
    *listp = blist = MLALLOC(1); blist->n = 1; blist->c[0] = 0;
    return;
  }
  berle(index,count,f,&blist);
  n = blist->n;
  mod = blist->mod;

  if ( n == 1 ) {
    *listp = blist = MLALLOC(1); blist->n = 1; blist->c[0] = 0;
    return;
  }
  
  /* find k s.t. mod^k <= 2^27 < mod^(k+1); set q = mod^k */
  for ( k = 1, q = mod; q <= ((1<<27)/mod); q *= mod, k++ );

  /* mignotte bound */
  bound = mignotte(q,f);

  *listp = rlist = MLALLOC(n); 
  rlist->n = n;
  rlist->mod = q;
  rlist->bound = bound;

  if ( bound == 1 ) {
    gcdgen(f,blist,&clist);
    henprep(f,blist,clist,&bqlist,&cqlist);

    for ( i = 0, b = (UM *)bqlist->c; i < n; i++ ) {
      COEF(rlist)[i] = tl = LUMALLOC(DEG(b[i]),1);
      for ( j = 0; j <= DEG(tl); j++ )
          COEF(tl)[j][0] = COEF(b[i])[j];
      COEF(rlist)[i] = tl;
    }
  } else {
    /* fl = f mod q */
    fl = LUMALLOC(dx,bound);
    ptolum(q,bound,f,fl);
    /* fm = f mod mod */
    fm = W_UMALLOC(dx);
    ptoum(mod,f,fm);
    /* fm = f mod q */
    qfm = W_UMALLOC(dx);
    ptoum(q,f,qfm);

    gm = W_UMALLOC(dx); qgm = W_UMALLOC(dx);
    hm = W_UMALLOC(dx); qhm = W_UMALLOC(dx);
    qam = W_UMALLOC(dx); qbm = W_UMALLOC(dx);
    w = W_UMALLOC(dx);
    for ( i = 0; i < n-1; i++ ) {
      cpyum(COEF(blist)[i],gm);
      cpyum(fm,w);
      divum(mod,w,gm,hm);

      /* find am,bm s.t. qam*qgm+qbm*qhm=1 mod q, qgm=gm mod mod, qhm=hm mod mod */
      henprep2(mod,q,k,qfm,gm,hm,qgm,qhm,qam,qbm);

      henmain2(fl,qgm,qhm,qam,qbm,q,bound,&tl);
      rlist->c[i] = (pointer)tl;
      cpyum(hm,fm);
      cpyum(qhm,qfm);
    }
    rlist->c[i] = fl;
  }
}

/*
  f = g0*h0 mod m -> f = gk*hk mod m^(bound), f is replaced by hk
*/

void henmain2(LUM f,UM g0,UM h0,UM a0,UM b0,int m,int bound,LUM *gp)
{
  int n,dg,dh,i,k,j,dg1,dh1;
  UM wu,wr,ws,wt,q,wh1,wg1,wc,wd,we,wz;
  LUM wb0,wb1,wb2,fk,gk,hk;

  n = DEG(f); dg = DEG(g0); dh = DEG(h0);

  W_LUMALLOC(n,bound,wb0);
  W_LUMALLOC(n,bound,wb1);
  W_LUMALLOC(n,bound,wb2);

  wt = W_UMALLOC(2*n); ws = W_UMALLOC(2*n);
  wr = W_UMALLOC(2*n); wu = W_UMALLOC(2*n);
  q = W_UMALLOC(2*n);
  wh1 = W_UMALLOC(2*n); wg1 = W_UMALLOC(2*n);
  
  /* gk = g0 */
  gk = LUMALLOC(n,bound);
  DEG(gk) = dg;
  for ( i = 0; i <= dg; i++ )
    COEF(gk)[i][0] = COEF(g0)[i];

  /* hk = h0 */
  W_LUMALLOC(n,bound,hk);
  DEG(hk) = dh;
  for ( i = 0; i <= dh; i++ )
    COEF(hk)[i][0] = COEF(h0)[i];

  /* fk = gk*hk */
  W_LUMALLOC(n,bound,fk);
  mullum(m,bound,gk,hk,fk);

  wc = W_UMALLOC(2*n); wd = W_UMALLOC(2*n);
  we = W_UMALLOC(2*n); wz = W_UMALLOC(2*n);

#if 0
  mulum(m,a0,g0,wc);
  mulum(m,b0,h0,wd);
  addum(m,wc,wd,wz);
  if ( DEG(wz) != 0 || COEF(wz)[0] != 1 )
    error("henmain2 : cannot happen(extgcd)");
#endif

#if 1
  fprintf(stderr,"bound=%d\n",bound);
#endif
  for ( k = 1; k < bound; k++ ) {
#if 1
    fprintf(stderr,".");
#endif

#if defined(VISUAL) || defined(__MINGW32__)
    check_intr();
#endif
    /* at this point, f = gk*hk mod y^k */

#if 0
    for ( j = 0; j < k; j++ )
      for ( i = 0; i <= n; i++ )
        if ( COEF(f)[i][j] != COEF(f)[i][j] )
          error("henmain2 : cannot happen(f=fk)");
#endif

    /* wt = (f-gk*hk)/y^k */
    for ( i = 0; i <= n; i++ )
      COEF(ws)[i] = COEF(f)[i][k];
    degum(ws,n);
    for ( i = 0; i <= n; i++ )
      COEF(wu)[i] = COEF(fk)[i][k];
    degum(wu,n);
    subum(m,ws,wu,wt);
    
    /* compute wf1,wg1 s.t. wh1*g0+wg1*h0 = wt */
    mulum(m,a0,wt,wh1); DEG(wh1) = divum(m,wh1,h0,q);
    mulum(m,wh1,g0,wc); subum(m,wt,wc,wd); DEG(wd) = divum(m,wd,h0,wg1);

    /* check */
#if 0
    if ( DEG(wd) >= 0 || DEG(wg1) > dg )
      error("henmain2 : cannot happen(adj)");

    mulum(m,wg1,h0,wc); mulum(m,wh1,g0,wd); addum(m,wc,wd,we);
    subum(m,we,wt,wz);
    if ( DEG(wz) >= 0 )
      error("henmain2 : cannot happen(coef)");
#endif

    /* fk += ((wg1*hk+wh1*gk)*y^k+wg1*wh1*y^(2*k) mod m^bound */

    /* wb0 = wh1*y^k */
    clearlum(n,bound,wb0);
    DEG(wb0) = dh1 = DEG(wh1);
    for ( i = 0; i <= dh1; i++ )
      COEF(wb0)[i][k] = COEF(wh1)[i];

    /* wb2 = gk*wb0 mod y^bound */
    clearlum(n,bound,wb2);
    mullum(m,bound,gk,wb0,wb2);

    /* fk += wb2 */
    addtolum(m,bound,wb2,fk);

    /* wb1 = wg1*y^k */
    clearlum(n,bound,wb1);
    DEG(wb1) = dg1 = DEG(wg1);
    for ( i = 0; i <= n; i++ )
      COEF(wb1)[i][k] = COEF(wg1)[i];

    /* wb2 = hk*wb1 mod y^bound */
    clearlum(n,bound,wb2);
    mullum(m,bound,hk,wb1,wb2);

    /* fk += wb2 */
    addtolum(m,bound,wb2,fk);

    /* fk += wg1*wh1*y^(2*k) mod y^bound) */
    if ( 2*k < bound ) {
      clearlum(n,bound,wb2);
      mullum(m,bound,wb0,wb1,wb2);
      addtolum(m,bound,wb2,fk);
    }

    /* gk += wg1*y^k, hk += wh1*y^k */
    for ( i = 0; i <= DEG(wg1); i++ )
      COEF(gk)[i][k] = COEF(wg1)[i];
    for ( i = 0; i <= DEG(wh1); i++ )
      COEF(hk)[i][k] = COEF(wh1)[i];
  }
#if 1
  fprintf(stderr,"\n");
#endif
  *gp = gk;
  clearlum(n,bound,f);
  DEG(f) = dh;
  for ( i = 0; i <= dh; i++ )
    for ( j = 0; j < bound; j++ )
      COEF(f)[i][j] = COEF(hk)[i][j];
}

void clearlum(int n,int bound,LUM f)
{
  int i;

  for ( i = 0; i <= n; i++ )
    bzero(COEF(f)[i],bound*sizeof(int));
}

/* g += f */

void addtolum(int m,int bound,LUM f,LUM g)
{
  int n,i;

  n = DEG(f);
  for ( i = 0; i <= n; i++ )
    addpadic(m,bound,(unsigned int *)COEF(f)[i],(unsigned int *)COEF(g)[i]);
}

void hsq(int index,int count,P f,int *nindex,DCP *dcp)
{
  register int i,j,k;
  register int **pp,**fpp;
  register int *px,*py;
  int **wpp;
  int n,dr,m,b,e,np,dt;
  unsigned int tmp;
  LUM fpa,wb0,wb1,lcpa,tpa,tlum;
  struct oDUM *dct;
  UM wt,wq0,wq,wr,wm,wm0,wa,ws,wb;
  LUM *llist,*ll;
  UM *dlist,*l,*c;
  ML list,fp,cfp;
  DCP dc;

  sqfrum(index,count,f,nindex,&dct,&fp);
  np = fp->n; m = fp->mod;
  if ( ( np == 1 ) && ( dct[0].n == 1 ) ) {
    NEWDC(dc); DEG(dc) = ONE; COEF(dc) = f; NEXT(dc) = 0; *dcp = dc;
    return;
  }
  for ( i = 0, dt = 0; i < np; i++ ) 
    dt = MAX(DEG(dct[i].f),dt);
  b = mig(m,dt,f); fp->bound = b;  
  if ( np == 1 ) {
    nthrootchk(f,dct,fp,dcp);
    return;
  }
  list = W_MLALLOC(np); list->n = np; list->mod = m; list->bound = 1;
  for ( i = 0, ll = (LUM *)list->c; i < np; i++ ) {
    W_LUMALLOC(DEG(dct[i].f),b,ll[i]);
    for ( j = 0, px = COEF(dct[i].f), pp = COEF(ll[i]);
      j <= DEG(ll[i]); j++ )
        pp[j][0] = px[j];
  }
  dtestsql(f,list,dct,&dc);
  if ( dc ) {
    *dcp = dc;
    return;
  }
  n = UDEG(f);
  W_LUMALLOC(n,b,fpa); W_LUMALLOC(0,b,lcpa);
  W_LUMALLOC(n,b,wb0); W_LUMALLOC(n,b,wb1);
  W_LUMALLOC(n,b,tpa);
  wt = W_UMALLOC(n); ws = W_UMALLOC(n);
  wr = W_UMALLOC(n);
  wq = W_UMALLOC(2*n); wq0 = W_UMALLOC(n); 
  wm = W_UMALLOC(2*n); wm0 = W_UMALLOC(2*n);
  wa = W_UMALLOC(2*n); 
  ptolum(m,b,f,fpa); DEG(lcpa) = 0;
  for ( i = 0, pp = COEF(lcpa), fpp = COEF(fpa); i < b; i++ ) 
    pp[0][i] = fpp[n][i];
  gcdgen(f,fp,&cfp);
  llist = (LUM *) ALLOCA(np*sizeof(LUM));
  dlist = (UM *) ALLOCA(np*sizeof(UM));
  l = (UM *)fp->c; c = (UM *)cfp->c;
  for ( i = 0; i < np; i++ ) {
    W_LUMALLOC(DEG(l[i]),b,llist[i]);
    for ( j = DEG(l[i]), pp = COEF(llist[i]), px = COEF(l[i]); j >= 0; j-- ) 
      pp[j][0] = px[j];
    if ( ( e = dct[i].n ) != 1 ) {
      wb = dct[i].f; 
      dlist[i] = W_UMALLOC(DEG(wb)*e); cpyum(l[i],dlist[i]);
      divum(m,dlist[i],wb,wq); DEG(dlist[i])= DEG(wq);
      for ( k = 0; k <= DEG(wq); k++ ) 
        COEF(dlist[i])[k] = dmb(m,COEF(wq)[k],e,&tmp);
    }
  }
  for ( i = 1; i < b; i++ ) {
    mullum(m,i+1,lcpa,llist[0],wb0);
    for ( j = 1; j < np; j++ ) {
      mullum(m,i+1,llist[j],wb0,wb1);
      tlum = wb0; wb0 = wb1; wb1 = tlum;
    } 
    for ( j = n, px = COEF(wt), pp = COEF(fpa), wpp = COEF(wb0);
      j >= 0; j-- ) 
      px[j] = ( pp[j][i] - wpp[j][i] + m ) % m;
    degum(wt,n);
    for ( j = n, px = COEF(wq0); j >= 0; j-- )
      px[j] = 0;
    for ( j = 1; j < np; j++ ) {
      mulum(m,wt,c[j],wm); dr = divum(m,wm,l[j],wq);
      for ( k = DEG(wq), px = COEF(wq0), py = COEF(wq); k >= 0; k-- ) 
        px[k] = ( px[k] + py[k] ) % m;
      for ( k = dr, pp = COEF(llist[j]), px = COEF(wm); k >= 0; k-- ) 
        pp[k][i] = px[k];
    }
    degum(wq0,n); mulum(m,wq0,l[0],wm);
    mulum(m,wt,c[0],wm0); addum(m,wm,wm0,wa);
    for ( j = DEG(wa), pp = COEF(llist[0]), px = COEF(wa); j >= 0; j-- ) 
      pp[j][i] = px[j];
    for ( j = n, px = COEF(wq0); j >= 0; j-- ) 
      px[j] = 0;
    for ( j = 0; j < np; j++ ) 
      if ( dct[j].n == 1 ) 
        for ( k = 0,
          pp = COEF(llist[j]),
          wpp = COEF(((LUM *)list->c)[j]); 
          k <= DEG(llist[j]); k++ )
          wpp[k][i] = pp[k][i];
      else {
        pwrlum(m,i+1,((LUM *)list->c)[j],dct[j].n,tpa);
        for ( k = 0,
          pp = COEF(llist[j]),
          wpp = COEF(tpa); 
          k <= DEG(l[j]); k++ )
          COEF(wt)[k] = (pp[k][i]-wpp[k][i]+m)%m;
        degum(wt,DEG(l[j])); dr = divum(m,wt,dlist[j],ws);
        if ( dr >= 0 ) {
          *dcp = 0;
          return;
        } else
          for ( k = 0,
            pp = COEF(((LUM *)list->c)[j]); 
            k <= DEG(ws); k++ )
            pp[k][i] = COEF(ws)[k];
      }
    list->bound = i+1; dtestsql(f,list,dct,&dc);
    if ( dc ) {
      *dcp = dc;
      return;
    } 
  }
  *dcp = 0;
}

void gcdgen(P f,ML blist,ML *clistp)
{
  register int i;
  int n,d,mod,np;
  UM wf,wm,wx,wy,wu,wv,wa,wb,wg,q,tum;
  UM *in,*out;
  ML clist;

  n = UDEG(f); mod = blist->mod; np = blist->n;
  d = 2*n;
  q = W_UMALLOC(d); wf = W_UMALLOC(d);
  wm = W_UMALLOC(d); wx = W_UMALLOC(d);
  wy = W_UMALLOC(d); wu = W_UMALLOC(d);
  wv = W_UMALLOC(d); wg = W_UMALLOC(d);
  wa = W_UMALLOC(d); wb = W_UMALLOC(d);
  ptoum(mod,f,wf); DEG(wg) = 0; COEF(wg)[0] = 1;
  *clistp = clist = MLALLOC(np); clist->mod = mod; clist->n = np;
  for ( i = 0, in = (UM *)blist->c, out = (UM *)clist->c; i < np; i++ ) {
    divum(mod,wf,in[i],q); tum = wf; wf = q; q = tum;
    cpyum(wf,wx); cpyum(in[i],wy);
    eucum(mod,wx,wy,wa,wb); mulum(mod,wa,wg,wm);
    DEG(wm) = divum(mod,wm,in[i],q); out[i] = UMALLOC(DEG(wm));
    cpyum(wm,out[i]); mulum(mod,q,wf,wu);
    mulum(mod,wg,wb,wv); addum(mod,wu,wv,wg);
  }
}

/* find a,b s.t. qa*qg+qb*qh=1 mod q, qg=g mod mod, qh=h mod mod */
/* q = mod^k */

void henprep2(int mod,int q,int k,UM f,UM g,UM h,UM qg,UM qh,UM qa,UM qb)
{
  int n;
  UM wg,wh,wa,wb;
  ML bl,cl,bql,cql;
  P ff;

  n = DEG(f);
  wg = W_UMALLOC(2*n); wh = W_UMALLOC(2*n);
  wa = W_UMALLOC(2*n); wb = W_UMALLOC(2*n);
  cpyum(g,wg); cpyum(h,wh);

  /* wa*g+wb*h = 1 mod mod */
  eucum(mod,wg,wh,wa,wb);

#if 0
  /* check */
  wt = W_UMALLOC(2*n); ws = W_UMALLOC(2*n); wu = W_UMALLOC(2*n);
  mulum(mod,wa,g,wt);
  mulum(mod,wb,h,ws);
  addum(mod,wt,ws,wu);
   if ( DEG(wu) != 0 || COEF(wu)[0] != 1 )
    error("henprep 1");
#endif

  bl = MLALLOC(2); bl->n = 2; bl->mod = mod; bl->c[0] = g; bl->c[1] = h;
  cl = MLALLOC(2); cl->n = 2; cl->mod = mod; cl->c[0] = wb; cl->c[1] = wa;
  umtop(CO->v,f,&ff); /* XXX */
  henprep(ff,bl,cl,&bql,&cql); /* XXX */

  cpyum(bql->c[0],qg); cpyum(bql->c[1],qh);
  cpyum(cql->c[0],qb); cpyum(cql->c[1],qa);

#if 0
  /* check */
  mulum(q,qa,qg,wt);
  mulum(q,qb,qh,ws);
  addum(q,wt,ws,wu);
   if ( DEG(wu) != 0 || COEF(wu)[0] != 1 )
    error("henprep 2");
#endif
}

/*
  henprep(f,blist,clist,&bqlist,&cqlist);
 */

void henprep(P f,ML blist,ML clist,ML *bqlistp,ML *cqlistp)
{
  register int i,j,k,*px,*py,*pz;
  int n,pmax,dr,tmp,p,p1,mod,np,b,q;
  UM w,wm,wn,wa,wt,wq,wf,quot,tum,*in,*inc,*out,*outc;
  ML bqlist,cqlist;

  n = UDEG(f); p = mod = blist->mod; np = blist->n;
/*  for ( b = 1, q = mod; q <= (unsigned int)(LBASE / (L)mod); q *= mod, b++ ); */
  for ( b = 1, q = mod; q <= ((1<<27) / mod); q *= mod, b++ ); 
  w = W_UMALLOC(n); ptoum(q,f,w);
  wm = W_UMALLOC(2*n); wn = W_UMALLOC(2*n);
  wa = W_UMALLOC(2*n); wt = W_UMALLOC(2*n);
  wq = W_UMALLOC(2*n); wf = W_UMALLOC(2*n);
  quot = W_UMALLOC(2*n);
  *bqlistp = bqlist = MLALLOC(np); *cqlistp = cqlist = MLALLOC(np);
  for ( i = 0; i < n+2; i++ )
    COEF(wq)[i] = 0;
  for ( i = 0,
      in = (UM *)blist->c, inc = (UM *)clist->c,
      out = (UM *)bqlist->c, outc = (UM *)cqlist->c;
      i < np; i++ ) {
    out[i] = C_UMALLOC(n+1); cpyum(in[i],out[i]);
    outc[i] = C_UMALLOC(n+1); cpyum(inc[i],outc[i]);
  }
  for ( pmax = 1, i = b; i > 0; i-- ) 
    pmax *= mod;
  for ( i = 1; i < b; i++, p = p1 ) {
    cpyum(out[0],wm);
    for ( j = 1; j < np; j++ ) {
      mulum(pmax,wm,out[j],wn);
      tum = wm; wm = wn; wn = tum;
    }
    for ( j = n, px = COEF(w), py = COEF(wm), pz = COEF(wt); j >= 0; j-- ) {
      tmp = ( ( px[j] - py[j] ) / p ) % mod;
      pz[j] = ( tmp >= 0? tmp : tmp + mod );
    }
    degum(wt,n);
    for ( j = 1; j < np; j++ ) {
      mulum(mod,wt,inc[j],wm); dr = divum(mod,wm,in[j],quot);
      for ( k = DEG(quot); k >= 0; k-- ) 
        COEF(wq)[k] = ( COEF(wq)[k] + COEF(quot)[k] ) % mod;
      for ( k = dr, px = COEF(out[j]), py = COEF(wm); k >= 0; k-- ) 
        px[k] += p * py[k];
    }
    degum(wq,n); mulum(mod,wq,in[0],wm); 
    mulum(mod,wt,inc[0],wn); addum(mod,wm,wn,wa);
    for ( j = DEG(wa), px = COEF(out[0]), py = COEF(wa); j >= 0; j-- ) 
      px[j] += p * py[j];
    for ( j = n, px = COEF(wq); j >= 0; j-- ) 
      px[j] = 0;
    p1 = p * mod;
    for ( j = n, px = COEF(wt); j >= 1; j-- ) 
      px[j] = 0;
    px[0] = 1;
    for ( j = 0; j < np; j++ ) {
      cpyum(w,wf);
      for ( k = DEG(wf), px = COEF(wf); k >= 0; k-- )
        px[k] %= p1;
      divum(p1,wf,out[j],quot); mulum(p1,outc[j],quot,wm);
      for ( k = DEG(wm), px = COEF(wt), py = COEF(wm); k >= 0; k-- ) 
        px[k] = ( px[k] - py[k] ) % p1;
    }
    degum(wt,n);
    for ( j = DEG(wt), px = COEF(wt); j >= 0; j-- ) 
      px[j] = ((tmp=(px[j]/p)%mod)>= 0?tmp:tmp + mod);
    for ( j = 0; j < np; j++ ) {
      mulum(mod,wt,outc[j],wm); dr = divum(mod,wm,in[j],quot);
      for ( k = dr, px = COEF(outc[j]), py = COEF(wm); k >= 0; k-- ) 
        px[k] += p * py[k];
      degum(outc[j],MAX(DEG(outc[j]),dr));
    }
  }
  bqlist->n = cqlist->n = np;
  bqlist->mod = cqlist->mod = q;
}

/*
  henmain(fl,bqlist,cqlist,listp)
*/

void henmain(LUM f,ML bqlist,ML cqlist,ML *listp)
{
  register int i,j,k;
  int *px,*py;
  int **pp,**pp1;
  int n,np,mod,bound,dr,tmp;
  UM wt,wq0,wq,wr,wm,wm0,wa,q;
  LUM wb0,wb1,tlum;
  UM *b,*c;
  LUM *l;
  ML list;

  n = DEG(f); np = bqlist->n; mod = bqlist->mod; bound = bqlist->bound;
  *listp = list = MLALLOC(n);
  list->n = np; list->mod = mod; list->bound = bound;
  W_LUMALLOC(n,bound,wb0); W_LUMALLOC(n,bound,wb1);
  wt = W_UMALLOC(n); wq0 = W_UMALLOC(n); wq = W_UMALLOC(n);
  wr = W_UMALLOC(n); wm = W_UMALLOC(2*n); wm0 = W_UMALLOC(2*n);
  wa = W_UMALLOC(2*n); q = W_UMALLOC(2*n);
  b = (UM *)bqlist->c; c = (UM *)cqlist->c; l = (LUM *)list->c; 
  for ( i = 0; i < np; i++ ) {
    l[i] = LUMALLOC(DEG(b[i]),bound);
    for ( j = DEG(b[i]), pp = COEF(l[i]), px = COEF(b[i]); j >= 0; j-- ) 
      pp[j][0] = px[j];
  }
#if 0
  fprintf(stderr,"bound=%d\n",bound);
#endif
  for ( i = 1; i < bound; i++ ) {
#if 0
    fprintf(stderr,".");
#endif
#if defined(VISUAL) || defined(__MINGW32__)
    check_intr();
#endif
    mullum(mod,i+1,l[0],l[1],wb0);
    for ( j = 2; j < np; j++ ) {
      mullum(mod,i+1,l[j],wb0,wb1);
      tlum = wb0; wb0 = wb1; wb1 = tlum;
    }
    for ( j = n, px = COEF(wt); j >= 0; j-- )
      px[j] = 0;
    for ( j = n, pp = COEF(f), pp1 = COEF(wb0); j >= 0; j-- ) {
      tmp = ( pp[j][i] - pp1[j][i] ) % mod;
      COEF(wt)[j] = ( tmp < 0 ? tmp + mod : tmp );
    }
    degum(wt,n);
    for ( j = n, px = COEF(wq0); j >= 0; j-- )
      px[j] = 0;
    for ( j = 1; j < np; j++ ) {
      mulum(mod,wt,c[j],wm); dr = divum(mod,wm,b[j],q);
      for ( k = DEG(q), px = COEF(wq0), py = COEF(q); k >= 0; k-- ) 
        px[k] = ( px[k] + py[k] ) % mod;
      for ( k = dr, pp = COEF(l[j]), px = COEF(wm); k >= 0; k-- ) 
        pp[k][i] = px[k];
    }
    degum(wq0,n); mulum(mod,wq0,b[0],wm);
    mulum(mod,wt,c[0],wm0); addum(mod,wm,wm0,wa);
    for ( j = DEG(wa), pp = COEF(l[0]), px = COEF(wa); j >= 0; j-- ) 
      pp[j][i] = px[j];
    for ( j = n, px = COEF(wq0); j >= 0; j-- ) 
      px[j] = 0;
  }
#if 0
  fprintf(stderr,"\n");
#endif
}

/*
  henmain_incremental(fl,bqlist,cqlist,start)
    fl = bqlist[0]*... mod q^start
*/

void henmain_incremental(LUM f,LUM *bqlist,ML cqlist,
  int np, int mod, int start, int bound)
{
  register int i,j,k;
  int *px,*py;
  int **pp,**pp1;
  int n,dr,tmp;
  UM wt,wq0,wq,wr,wm,wm0,wa,q;
  LUM wb0,wb1,tlum;
  UM *b,*c;
  LUM *l;
  ML list;

  n = DEG(f);
  W_LUMALLOC(n,bound,wb0); W_LUMALLOC(n,bound,wb1);
  wt = W_UMALLOC(n); wq0 = W_UMALLOC(n); wq = W_UMALLOC(n);
  wr = W_UMALLOC(n); wm = W_UMALLOC(2*n); wm0 = W_UMALLOC(2*n);
  wa = W_UMALLOC(2*n); q = W_UMALLOC(2*n);
  c = (UM *)cqlist->c; l = bqlist;
  b = (UM *)ALLOCA(n*sizeof(UM));
  for ( i = 0; i < np; i++ ) {
    j = DEG(l[i]);
    b[i] = W_UMALLOC(j);
    DEG(b[i]) = j;
    for ( pp = COEF(l[i]), px = COEF(b[i]); j >= 0; j-- )
      px[j] = pp[j][0];
  }
#if 0
  fprintf(stderr,"bound=%d\n",bound);
#endif
  for ( i = start; i < bound; i++ ) {
#if 0
    fprintf(stderr,".");
#endif
    mullum(mod,i+1,l[0],l[1],wb0);
    for ( j = 2; j < np; j++ ) {
      mullum(mod,i+1,l[j],wb0,wb1);
      tlum = wb0; wb0 = wb1; wb1 = tlum;
    }
    for ( j = n, px = COEF(wt); j >= 0; j-- )
      px[j] = 0;
    for ( j = n, pp = COEF(f), pp1 = COEF(wb0); j >= 0; j-- ) {
      tmp = ( pp[j][i] - pp1[j][i] ) % mod;
      COEF(wt)[j] = ( tmp < 0 ? tmp + mod : tmp );
    }
    degum(wt,n);
    for ( j = n, px = COEF(wq0); j >= 0; j-- )
      px[j] = 0;
    for ( j = 1; j < np; j++ ) {
      mulum(mod,wt,c[j],wm); dr = divum(mod,wm,b[j],q);
      for ( k = DEG(q), px = COEF(wq0), py = COEF(q); k >= 0; k-- ) 
        px[k] = ( px[k] + py[k] ) % mod;
      for ( k = dr, pp = COEF(l[j]), px = COEF(wm); k >= 0; k-- ) 
        pp[k][i] = px[k];
    }
    degum(wq0,n); mulum(mod,wq0,b[0],wm);
    mulum(mod,wt,c[0],wm0); addum(mod,wm,wm0,wa);
    for ( j = DEG(wa), pp = COEF(l[0]), px = COEF(wa); j >= 0; j-- ) 
      pp[j][i] = px[j];
    for ( j = n, px = COEF(wq0); j >= 0; j-- ) 
      px[j] = 0;
  }
#if 0
  fprintf(stderr,"\n");
#endif
}

static double M;
static int E;

int mig(int q,int d,P f)
{
  mpz_t z;
  int s;
  mp_limb_t m;
  DCP dc;

  for ( dc = DC(f), M = 0, E = 0; dc; dc = NEXT(dc) ) {
    z[0] = BDY((Z)COEF(dc))[0]; s = mpz_size(z); m = mpz_getlimbn(z,s-1);
    sqad(m,(s-1)*sizeof(mp_limb_t)*8);
  }
  if (E % 2) M *= 2; M = ceil(sqrt(M)); E /= 2;
  z[0] = BDY((Z)COEF(DC(f)))[0]; s = mpz_size(z); m = mpz_getlimbn(z,s-1);
  M *= ((double)m)+1; E += (s-1) * sizeof(mp_limb_t)*8;
  return (int)ceil( (0.31*(E+d+1)+log10((double)M)) / log10((double)q) );
}

int mignotte(int q,P f)
{
  return mig(q,UDEG(f),f);
}

/* man*2^exp */

void sqad(mp_limb_t man,int exp)
{
  int e,sqe;
  mp_limb_t t;
  double man1,d,sqm;
  int diff;

  if ( man+1 == 0 ) {
    e = sizeof(mp_limb_t)*8; man1 = 1.0;
  } else {
    man += 1;
    for ( e = 0, t = man; t; e++, t >>= 1 );  
    e--; d = (double)(1<<e);
    man1 = ((double)man)/d;
  }
  exp += e; sqm = man1 * man1; sqe = 2 * exp;
  if ( sqm >= 2.0 ) {
    sqm /= 2.0; sqe++;
  }
  diff = E - sqe;
  if ( diff > 18 ) 
    return;
  if ( diff < -18 ) {
    M = sqm; E = sqe;
    return;
  }
  if ( diff >= 0 )
    M += (sqm / (double)(1<<diff));
  else {
    M = ( ( M / (double)(1<<-diff)) + sqm ); E = sqe;
  }
  if ( M >= 2.0 ) {
    M /= 2.0; E++;
  }
}

void ptolum(int q,int bound,P f,LUM fl)
{
  DCP dc;
  int i,j;
  int **pp;
  int d,br,s;
  unsigned int r;
  int *c;
  unsigned int *w;
  Z z;

  for ( dc = DC(f), pp = COEF(fl); dc; dc = NEXT(dc) ) {
    absz((Z)COEF(dc),&z);
    d = ztonadic(q,z,&w);
    c = pp[ZTOS(DEG(dc))];
    for ( i = 0; i < d; i++ ) c[i] = w[i];
    for ( ; i < bound; i++ ) c[i] = 0;
    if ( sgnz((Z)COEF(dc)) < 0 ) 
      for (i = 0, br = 0; i < bound; i++ ) {
        if ( ( s = -(c[i] + br) ) < 0 ) {
          c[i] = s + q; br = 1;
        } else {
          c[i] = 0; br = 0;
        }
      }
  }
}

void modfctrp(P p,int mod,int flag,DCP *dcp)
{
  int cm,n,i,j,k;
  DCP dc,dc0;
  P zp;
  Q c;
  Z q;
  UM mp;
  UM *tl;
  struct oDUM *udc,*udc1;

  if ( !p ) {
    *dcp = 0; return;
  }
  ptozp(p,1,&c,&zp); 
  if ( !INT(c) || !(cm = remqi(c,mod)) ) {
    *dcp = 0; return;
  }
  mp = W_UMALLOC(UDEG(p));
  ptoum(mod,zp,mp);
  if ( (n = DEG(mp)) < 0 ) {
    *dcp = 0; return;
  } else if ( n == 0 ) {
    cm = dmar(cm,COEF(mp)[0],0,mod); STOZ(cm,q); 
    NEWDC(dc); COEF(dc) = (P)q; DEG(dc) = ONE; 
    NEXT(dc) = 0; *dcp = dc;
    return;
  }
  if ( COEF(mp)[n] != 1 ) {
    cm = dmar(cm,COEF(mp)[n],0,mod);
    i = invm(COEF(mp)[n],mod);
    for ( j = 0; j <= n; j++ )
      COEF(mp)[j] = dmar(COEF(mp)[j],i,0,mod);
  }
  W_CALLOC(n+1,struct oDUM,udc);
  gensqfrum(mod,mp,udc);
  switch ( flag ) {
    case FCTR:
      tl = (UM *)ALLOCA((n+1)*sizeof(UM));
      W_CALLOC(DEG(mp)+1,struct oDUM,udc1);
      for ( i = 0,j = 0; udc[i].f; i++ )
        if ( DEG(udc[i].f) == 1 ) {
          udc1[j].f = udc[i].f; udc1[j].n = udc[i].n; j++;
        } else {
          bzero((char *)tl,(n+1)*sizeof(UM));
          berlemain(mod,udc[i].f,tl);
          for ( k = 0; tl[k]; k++, j++ ) {
            udc1[j].f = tl[k]; udc1[j].n = udc[i].n;
          }
        }
      udc = udc1; break;
    case SQFR:
      break;
    case DDD:
      tl = (UM *)ALLOCA((n+1)*sizeof(UM));
      W_CALLOC(DEG(mp)+1,struct oDUM,udc1);
      for ( i = 0,j = 0; udc[i].f; i++ )
        if ( DEG(udc[i].f) == 1 ) {
          udc1[j].f = udc[i].f; udc1[j].n = udc[i].n; j++;
        } else {
          bzero((char *)tl,(n+1)*sizeof(UM));
          ddd(mod,udc[i].f,tl);
          for ( k = 0; tl[k]; k++, j++ ) {
            udc1[j].f = tl[k]; udc1[j].n = udc[i].n;
          }
        }
      udc = udc1; break;
    case NEWDDD:
      tl = (UM *)ALLOCA((n+1)*sizeof(UM));
      W_CALLOC(DEG(mp)+1,struct oDUM,udc1);
      for ( i = 0,j = 0; udc[i].f; i++ )
        if ( DEG(udc[i].f) == 1 ) {
          udc1[j].f = udc[i].f; udc1[j].n = udc[i].n; j++;
        } else {
          bzero((char *)tl,(n+1)*sizeof(UM));
          if ( mod == 2 )
            berlemain(mod,udc[i].f,tl);
          else
            newddd(mod,udc[i].f,tl);
          for ( k = 0; tl[k]; k++, j++ ) {
            udc1[j].f = tl[k]; udc1[j].n = udc[i].n;
          }
        }
      udc = udc1; break;
  }
  NEWDC(dc0); STOZ(cm,q); COEF(dc0) = (P)q; DEG(dc0) = ONE; dc = dc0;
  for ( n = 0; udc[n].f; n++ ) {
    NEWDC(NEXT(dc)); dc = NEXT(dc);
    STOZ(udc[n].n,DEG(dc)); umtop(VR(p),udc[n].f,&COEF(dc));
  }
  NEXT(dc) = 0; *dcp = dc0;
}

void gensqfrum(int mod,UM p,struct oDUM *dc)
{
  int n,i,j,d;
  UM t,s,g,f,f1,b;

  if ( (n = DEG(p)) == 1 ) {
    dc[0].f = UMALLOC(DEG(p)); cpyum(p,dc[0].f); dc[0].n = 1;
    return;
  }
  t = W_UMALLOC(n); s = W_UMALLOC(n); g = W_UMALLOC(n);
  f = W_UMALLOC(n); f1 = W_UMALLOC(n); b = W_UMALLOC(n);
  diffum(mod,p,t); cpyum(p,s); Gcdum(mod,t,s,g); 
  if ( !DEG(g) ) {
    dc[0].f = UMALLOC(DEG(p)); cpyum(p,dc[0].f); dc[0].n = 1;
    return;
  }
  cpyum(p,b); cpyum(p,t); Divum(mod,t,g,f);
  for ( i = 0, d = 0; DEG(f); i++ ) {
    while ( 1 ) {
      cpyum(b,t);
      if ( Divum(mod,t,f,s) >= 0 )
        break;
      else {
        cpyum(s,b); d++;
      }
    }
    cpyum(b,t); cpyum(f,s); Gcdum(mod,t,s,f1);
    Divum(mod,f,f1,s); cpyum(f1,f);
    dc[i].f = UMALLOC(DEG(s)); cpyum(s,dc[i].f); dc[i].n = d;
  }
  if ( DEG(b) > 0 ) {
    d = 1;
    while ( 1 ) {
      cpyum(b,t);
      for ( j = DEG(t); j >= 0; j-- )
        if ( COEF(t)[j] && (j % mod) )
          break;
      if ( j >= 0 )
        break;
      else {
        DEG(s) = DEG(t)/mod;
        for ( j = 0; j <= DEG(t); j++ )
          COEF(s)[j] = COEF(t)[j*mod];  
        cpyum(s,b); d *= mod;
      }
    }
    gensqfrum(mod,b,dc+i);
    for ( j = i; dc[j].f; j++ )
      dc[j].n *= d;
  }
}

#if 0
void srchum(int mod,UM p1,UM p2,UM gr)
{
  UM m,m1,m2,q,r,t,g1,g2;
  int lc,d,d1,d2,i,j,k,l,l1,l2,l3,tmp,adj;
  V v;

  d = MAX(DEG(p1),DEG(p2));
  g1 = W_UMALLOC(d); g2 = W_UMALLOC(d);
  bzero((char *)g1,(d+2)*sizeof(int)); bzero((char *)g2,(d+2)*sizeof(int));
  if ( d == DEG(p1) ) {
    cpyum(p1,g1); cpyum(p2,g2);
  } else {
    cpyum(p1,g2); cpyum(p2,g1);
  }
  if ( ( d1 = DEG(g1) ) > ( d2 = DEG(g2) ) ) {
    j = d1 - 1; adj = 1;
  } else 
    j = d2;
  lc = 1;
  r = W_UMALLOC(d1+d2); q = W_UMALLOC(d1+d2); 
  m1 = W_UMALLOC(d1+d2); t = W_UMALLOC(d1+d2);
  bzero((char *)r,(d1+d2+2)*sizeof(int)); bzero((char *)q,(d1+d2+2)*sizeof(int)); 
  bzero((char *)m1,(d1+d2+2)*sizeof(int)); bzero((char *)t,(d1+d2+2)*sizeof(int)); 
  m = W_UMALLOC(0); bzero((char *)m,2*sizeof(int));
  adj = pwrm(mod,COEF(g2)[DEG(g2)],DEG(g1));
  DEG(m) = 0; COEF(m)[0] = invm(COEF(g2)[DEG(g2)],mod);
  Mulum(mod,g2,m,r); cpyum(r,g2);
  while ( 1 ) {
    if ( ( k = DEG(g2) ) < 0 ) {
      DEG(gr) = -1;
      return;
    }
    if ( k == j ) {
      if ( k == 0 ) {
        DEG(m) = 0; COEF(m)[0] = adj;
        Mulum(mod,g2,m,gr);
        return;
      } else {
        DEG(m) = 0; 
        COEF(m)[0] = pwrm(mod,COEF(g2)[k],DEG(g1)-k+1);
        Mulum(mod,g1,m,r); DEG(r) = Divum(mod,r,g2,t);
        DEG(m) = 0; COEF(m)[0] = dmb(mod,lc,lc,&tmp);
        Divum(mod,r,m,q); cpyum(g2,g1); cpyum(q,g2);
        lc = COEF(g1)[DEG(g1)]; j = k - 1;
      }
    } else {
      d = j - k;
      DEG(m) = 0; COEF(m)[0] = pwrm(mod,COEF(g2)[DEG(g2)],d);
      Mulum(mod,g2,m,m1); l = pwrm(mod,lc,d);
      DEG(m) = 0; COEF(m)[0] = l; Divum(mod,m1,m,t);
      if ( k == 0 ) {
        DEG(m) = 0; COEF(m)[0] = adj;
        Mulum(mod,t,m,gr);
        return;
      } else {
        DEG(m) = 0; 
        COEF(m)[0] = pwrm(mod,COEF(g2)[k],DEG(g1)-k+1);
        Mulum(mod,g1,m,r); DEG(r) = Divum(mod,r,g2,q);
        l1 = dmb(mod,lc,lc,&tmp); l2 = dmb(mod,l,l1,&tmp);
        DEG(m) = 0; COEF(m)[0] = l2;
        Divum(mod,r,m,q); cpyum(t,g1); cpyum(q,g2);
        if ( d % 2 ) 
          for ( i = DEG(g2); i >= 0; i-- ) 
            COEF(g2)[i] = ( mod - COEF(g2)[i] ) % mod;
        lc = COEF(g1)[DEG(g1)]; j = k - 1;
      }
    }
  }
}

UM *resberle(int mod,UM f,UM *fp)
{
  UM w,wg,ws,wf,f0,gcd,q,res;
  int n;
  register int i;

  n = DEG(f); wg = W_UMALLOC(n); mini(mod,f,wg);
  if ( DEG(wg) <= 0 ) {
    f0 = UMALLOC(n); cpyum(f,f0); *fp++ = f0;
    return ( fp );
  }
  f0 = W_UMALLOC(n); cpyum(f,f0);
  ws = W_UMALLOC(n); wf = W_UMALLOC(n);
  q = W_UMALLOC(n); gcd = W_UMALLOC(n);
  res = W_UMALLOC(2*n);
  srchum(mod,f,wg,res);
  for ( i = 0; i < mod; i++ ) {
    if ( substum(mod,res,i)  )
      continue;
    cpyum(f0,wf); cpyum(wg,ws);
    COEF(ws)[0] = ( COEF(ws)[0] + mod - i ) % mod;
    Gcdum(mod,wf,ws,gcd);
    if ( DEG(gcd) > 0 ) {
      if ( DEG(gcd) < n ) {
        Divum(mod,f0,gcd,q); f0 = q; fp = resberle(mod,gcd,fp);
      }
      break;
    }
  }
  fp = resberle(mod,f0,fp);
  return ( fp );
}

int substum(int mod,UM p,int a)
{
  int i,j,s;
  int *c;

  if ( DEG(p) < 0 )
    return 0;
  if ( DEG(p) == 0 )
    return COEF(p)[0];
  for ( i = DEG(p), c = COEF(p), s = c[i]; i >= 0; ) {
    for ( j = i--; (i>=0) && !c[i]; i-- );
    if ( i >= 0 )
      s = (s*pwrm(mod,a,j-i)%mod+c[i])%mod;
    else
      s = s*pwrm(mod,a,j)%mod;
  }
  return s;
}
#endif

void ddd(int mod,UM f,UM *r)
{
  register int i,j;
  int d,n;
  UM q,s,t,u,v,w,g,x,m;
  UM *base;

  n = DEG(f);
  if ( n == 1 ) {
    r[0] = UMALLOC(1); cpyum(f,r[0]); r[1] = 0; return;
  }
  base = (UM *)ALLOCA(n*sizeof(UM)); bzero((char *)base,n*sizeof(UM));
  w = W_UMALLOC(2*n); q = W_UMALLOC(2*n); m = W_UMALLOC(2*n);
  base[0] = W_UMALLOC(0); DEG(base[0]) = 0; COEF(base[0])[0] = 1;
  t = W_UMALLOC(1); DEG(t) = 1; COEF(t)[0] = 0; COEF(t)[1] = 1;
  pwrmodum(mod,t,mod,f,w); base[1] = W_UMALLOC(DEG(w)); cpyum(w,base[1]);
  for ( i = 2; i < n; i++ ) {
    mulum(mod,base[i-1],base[1],m); DEG(m) = divum(mod,m,f,q);
    base[i] = W_UMALLOC(DEG(m)); cpyum(m,base[i]);
  }
  v = W_UMALLOC(n); cpyum(f,v);
  DEG(w) = 1; COEF(w)[0] = 0; COEF(w)[1] = 1;
  x = W_UMALLOC(1); DEG(x) = 1; COEF(x)[0] = 0; COEF(x)[1] = 1;
  t = W_UMALLOC(n); s = W_UMALLOC(n); u = W_UMALLOC(n); g = W_UMALLOC(n);
  for ( j = 0, d = 1; 2*d <= DEG(v); d++ ) {
    for ( DEG(t) = -1, i = 0; i <= DEG(w); i++ )
      if ( COEF(w)[i] ) {
        Mulsum(mod,base[i],COEF(w)[i],s); 
        addum(mod,s,t,u); cpyum(u,t);
      }
    cpyum(t,w); cpyum(v,s); subum(mod,w,x,t); Gcdum(mod,s,t,g);
    if ( DEG(g) >= 1 ) {
      canzas(mod,g,d,base,r+j); j += DEG(g)/d;
      Divum(mod,v,g,q); cpyum(q,v);
      DEG(w) = Divum(mod,w,v,q);
      for ( i = 0; i < DEG(v); i++ )
        DEG(base[i]) = Divum(mod,base[i],v,q);
    }
  }
  if ( DEG(v) ) {
    r[j] = UMALLOC(DEG(v)); cpyum(v,r[j]); j++;
  }
  r[j] = 0;
}

void canzas(int mod,UM f,int d,UM *base,UM *r)
{
  UM t,s,u,w,g,o,q;
  Z n1,n2,n3,n4,n5,z;
  UM *b;
  int n,m,i;

  if ( DEG(f) == d ) {
    r[0] = UMALLOC(d); cpyum(f,r[0]);
    return;
  } else {
    n = DEG(f); b = (UM *)ALLOCA(n*sizeof(UM)); bzero((char *)b,n*sizeof(UM));
    for ( i = 0, m = 0; i < n; i++ )
      m = MAX(m,DEG(base[i]));
    q = W_UMALLOC(m);
    for ( i = 0; i < n; i++ ) {
      b[i] = W_UMALLOC(DEG(base[i])); cpyum(base[i],b[i]);
      DEG(b[i]) = Divum(mod,b[i],f,q);
    }
    t = W_UMALLOC(2*d);
    s = W_UMALLOC(DEG(f)); u = W_UMALLOC(DEG(f)); 
    w = W_UMALLOC(DEG(f)); g = W_UMALLOC(DEG(f));
    o = W_UMALLOC(0); DEG(o) = 0; COEF(o)[0] = 1;
    STOZ(mod,n1); STOZ(d,z); pwrz(n1,z,&n2); subz(n2,ONE,&n3);
    STOZ(2,n4); divz(n3,n4,&n5);
    while ( 1 ) {
      randum(mod,2*d,t); spwrum0(mod,f,t,n5,s);
      subum(mod,s,o,u); cpyum(f,w); Gcdum(mod,w,u,g);
      if ( (DEG(g) >= 1) && (DEG(g) < DEG(f)) ) {
        canzas(mod,g,d,b,r); 
        cpyum(f,w); Divum(mod,w,g,s);
        canzas(mod,s,d,b,r+DEG(g)/d); 
        return;
      }
    }
  }
}

void randum(int mod,int d,UM p)
{
  unsigned int n;
  int i;

  n = ((unsigned int)random()) % d; DEG(p) = n; COEF(p)[n] = 1;
  for ( i = 0; i < (int)n; i++ )
    COEF(p)[i] = ((unsigned int)random()) % mod;
}

void pwrmodum(int mod,UM p,int e,UM f,UM pr)
{
  UM wt,ws,q;

  if ( e == 0 ) {
    DEG(pr) = 0; COEF(pr)[0] = 1;
  } else if ( DEG(p) < 0 ) 
    DEG(pr) = -1;
  else if ( e == 1 ) {
    q = W_UMALLOC(DEG(p)); cpyum(p,pr);
    DEG(pr) = divum(mod,pr,f,q);
  } else if ( DEG(p) == 0 ) {
    DEG(pr) = 0; COEF(pr)[0] = pwrm(mod,COEF(p)[0],e);
  } else {
    wt = W_UMALLOC(2*DEG(f)); ws = W_UMALLOC(2*DEG(f));
    q = W_UMALLOC(2*DEG(f));
    pwrmodum(mod,p,e/2,f,wt);
    if ( !(e%2) )  {
      mulum(mod,wt,wt,pr); DEG(pr) = divum(mod,pr,f,q);
    } else {
      mulum(mod,wt,wt,ws); DEG(ws) = divum(mod,ws,f,q);
      mulum(mod,ws,p,pr); DEG(pr) = divum(mod,pr,f,q);
    }
  }
}

void spwrum0(int mod,UM m,UM f,Z e,UM r)
{
  UM t,s,q;
  Z e1,rem,two;

  if ( !e ) {
    DEG(r) = 0; COEF(r)[0] = 1;
  } else if ( UNIZ(e) )
    cpyum(f,r);
  else {
    STOZ(2,two);
    divqrz(e,two,&e1,&rem);
    t = W_UMALLOC(2*DEG(m)); spwrum0(mod,m,f,e1,t);
    s = W_UMALLOC(2*DEG(m)); q = W_UMALLOC(2*DEG(m));
    Mulum(mod,t,t,s); DEG(s) = Divum(mod,s,m,q);
    if ( rem ) {
      Mulum(mod,s,f,t); DEG(t) = Divum(mod,t,m,q); cpyum(t,r);
        } else
      cpyum(s,r);
  }
}

void mult_mod_tab(UM p,int mod,UM *tab,UM r,int d)
{
  UM w,w1,c;
  int n,i;
  int *pc;

  w = W_UMALLOC(d); w1 = W_UMALLOC(d);
  c = W_UMALLOC(1); DEG(c) = 0;
  n = DEG(p); DEG(r) = -1;
  for ( i = 0, pc = COEF(p); i <= n; i++ )
    if ( pc[i] ) {
      COEF(c)[0] = pc[i];
      mulum(mod,tab[i],c,w);
      addum(mod,r,w,w1);
      cpyum(w1,r);
    }
}

void make_qmat(UM p,int mod,UM *tab,int ***mp)
{
  int n,i,j;
  int *c;
  UM q,r;
  int **mat;

  n = DEG(p);
  *mp = mat = almat(n,n);
  for ( j = 0; j < n; j++ ) {
    r = W_UMALLOC(DEG(tab[j])); q = W_UMALLOC(DEG(tab[j]));  
    cpyum(tab[j],r); DEG(r) = divum(mod,r,p,q);
    for ( i = 0, c = COEF(r); i <= DEG(r); i++ )
      mat[i][j] = c[i];
  }
  for ( i = 0; i < n; i++ )
    mat[i][i] = (mat[i][i]+mod-1) % mod;
}

void null_mod(int **mat,int mod,int n,int *ind)
{
  int i,j,l,s,h,inv;
  int *t,*u;

  bzero((char *)ind,n*sizeof(int));
  ind[0] = 0;
  for ( i = j = 0; j < n; i++, j++ ) {
    for ( ; j < n; j++ ) {
      for ( l = i; l < n; l++ )
        if ( mat[l][j] )
          break;
      if ( l < n ) {
        t = mat[i]; mat[i] = mat[l]; mat[l] = t; break;
      } else
        ind[j] = 1;
    }
    if ( j == n )
      break;
    inv = invm(mat[i][j],mod);
    for ( s = j, t = mat[i]; s < n; s++ )
      t[s] =  dmar(t[s],inv,0,mod);
    for ( l = 0; l < n; l++ ) {
      if ( l == i )
        continue;
      for ( s = j, u = mat[l], h = (mod-u[j])%mod; s < n; s++ )
        u[s] = dmar(h,t[s],u[s],mod);
    }
  }
}

void null_to_sol(int **mat,int *ind,int mod,int n,UM *r)
{
  int i,j,k,l;
  int *c;
  UM w;

  for ( i = 0, l = 0; i < n; i++ ) {
    if ( !ind[i] )
      continue;
    w = UMALLOC(n);
    for ( j = k = 0, c = COEF(w); j < n; j++ )
      if ( ind[j] )
        c[j] = 0;
      else
        c[j] = mat[k++][i];
    c[i] = mod-1;
    for ( j = n; j >= 0; j-- )
      if ( c[j] )
        break;
    DEG(w) = j;
    r[l++] = w;
  }
}
/*
make_qmat(p,mod,tab,mp)
null_mod(mat,mod,n,ind)
null_to_sol(mat,ind,mod,n,r)
*/

void newddd(int mod,UM f,UM *r)
{
  register int i,j;
  int d,n;
  UM q,s,t,u,v,w,g,x,m;
  UM *base;

  n = DEG(f); base = (UM *)ALLOCA(n*sizeof(UM)); bzero((char *)base,n*sizeof(UM));
  w = W_UMALLOC(2*n); q = W_UMALLOC(2*n); m = W_UMALLOC(2*n);
  base[0] = W_UMALLOC(0); DEG(base[0]) = 0; COEF(base[0])[0] = 1;
  t = W_UMALLOC(1); DEG(t) = 1; COEF(t)[0] = 0; COEF(t)[1] = 1;
  pwrmodum(mod,t,mod,f,w); base[1] = W_UMALLOC(DEG(w)); cpyum(w,base[1]);
  for ( i = 2; i < n; i++ ) {
/*    fprintf(stderr,"i=%d\n",i); */
    mulum(mod,base[i-1],base[1],m); DEG(m) = divum(mod,m,f,q);
    base[i] = W_UMALLOC(DEG(m)); cpyum(m,base[i]);
  }
  v = W_UMALLOC(n); cpyum(f,v);
  DEG(w) = 1; COEF(w)[0] = 0; COEF(w)[1] = 1;
  x = W_UMALLOC(1); DEG(x) = 1; COEF(x)[0] = 0; COEF(x)[1] = 1;
  t = W_UMALLOC(n); s = W_UMALLOC(n); u = W_UMALLOC(n); g = W_UMALLOC(n);
  for ( j = 0, d = 1; 2*d <= DEG(v); d++ ) {
/*    fprintf(stderr,"d=%d\n",d); */
    for ( DEG(t) = -1, i = 0; i <= DEG(w); i++ )
      if ( COEF(w)[i] ) {
        Mulsum(mod,base[i],COEF(w)[i],s); 
        addum(mod,s,t,u); cpyum(u,t);
      }
    cpyum(t,w); cpyum(v,s); subum(mod,w,x,t); Gcdum(mod,s,t,g);
    if ( DEG(g) >= 1 ) {
      berlekamp(g,mod,d,base,r+j); j += DEG(g)/d;
      Divum(mod,v,g,q); cpyum(q,v);
      DEG(w) = Divum(mod,w,v,q);
      for ( i = 0; i < DEG(v); i++ )
        DEG(base[i]) = Divum(mod,base[i],v,q);
    }
  }
  if ( DEG(v) ) {
    r[j] = UMALLOC(DEG(v)); cpyum(v,r[j]); j++;
  }
  r[j] = 0;
}

int nfctr_mod(UM f,int mod)
{
  register int i,j;
  int d,n;
  UM q,s,t,u,v,w,g,x,m;
  UM *base;

  n = DEG(f); base = (UM *)ALLOCA(n*sizeof(UM)); bzero((char *)base,n*sizeof(UM));
  w = W_UMALLOC(2*n); q = W_UMALLOC(2*n); m = W_UMALLOC(2*n);
  base[0] = W_UMALLOC(0); DEG(base[0]) = 0; COEF(base[0])[0] = 1;
  t = W_UMALLOC(1); DEG(t) = 1; COEF(t)[0] = 0; COEF(t)[1] = 1;
  pwrmodum(mod,t,mod,f,w); base[1] = W_UMALLOC(DEG(w)); cpyum(w,base[1]);
  for ( i = 2; i < n; i++ ) {
/*    fprintf(stderr,"i=%d\n",i); */
    mulum(mod,base[i-1],base[1],m); DEG(m) = divum(mod,m,f,q);
    base[i] = W_UMALLOC(DEG(m)); cpyum(m,base[i]);
  }
  v = W_UMALLOC(n); cpyum(f,v);
  DEG(w) = 1; COEF(w)[0] = 0; COEF(w)[1] = 1;
  x = W_UMALLOC(1); DEG(x) = 1; COEF(x)[0] = 0; COEF(x)[1] = 1;
  t = W_UMALLOC(n); s = W_UMALLOC(n); u = W_UMALLOC(n); g = W_UMALLOC(n);
  for ( j = 0, d = 1; 2*d <= DEG(v); d++ ) {
/*    fprintf(stderr,"d=%d\n",d); */
    for ( DEG(t) = -1, i = 0; i <= DEG(w); i++ )
      if ( COEF(w)[i] ) {
        Mulsum(mod,base[i],COEF(w)[i],s); 
        addum(mod,s,t,u); cpyum(u,t);
      }
    cpyum(t,w); cpyum(v,s); subum(mod,w,x,t); Gcdum(mod,s,t,g);
    if ( DEG(g) >= 1 ) {
      j += DEG(g)/d;
      Divum(mod,v,g,q); cpyum(q,v);
      DEG(w) = Divum(mod,w,v,q);
      for ( i = 0; i < DEG(v); i++ )
        DEG(base[i]) = Divum(mod,base[i],v,q);
    }
  }
  if ( DEG(v) ) j++;
  return j;
}

int irred_check(UM f,int mod)
{
  register int i,j;
  int d,n;
  UM q,s,t,u,v,w,g,x,m,f1,b;
  UM *base;

  if ( (n = DEG(f)) == 1 )
    return 1;
  t = W_UMALLOC(n); s = W_UMALLOC(n); g = W_UMALLOC(n);
  f1 = W_UMALLOC(n); b = W_UMALLOC(n);
  diffum(mod,f,t); cpyum(f,s); Gcdum(mod,t,s,g); 
  if ( DEG(g) )
    return 0;

  base = (UM *)ALLOCA(n*sizeof(UM)); bzero((char *)base,n*sizeof(UM));
  w = W_UMALLOC(2*n); q = W_UMALLOC(2*n); m = W_UMALLOC(2*n);
  base[0] = W_UMALLOC(0); DEG(base[0]) = 0; COEF(base[0])[0] = 1;
  t = W_UMALLOC(1); DEG(t) = 1; COEF(t)[0] = 0; COEF(t)[1] = 1;
  pwrmodum(mod,t,mod,f,w); base[1] = W_UMALLOC(DEG(w)); cpyum(w,base[1]);
  for ( i = 2; i < n; i++ ) {
/*    fprintf(stderr,"i=%d\n",i); */
    mulum(mod,base[i-1],base[1],m); DEG(m) = divum(mod,m,f,q);
    base[i] = W_UMALLOC(DEG(m)); cpyum(m,base[i]);
  }
  v = W_UMALLOC(n); cpyum(f,v);
  DEG(w) = 1; COEF(w)[0] = 0; COEF(w)[1] = 1;
  x = W_UMALLOC(1); DEG(x) = 1; COEF(x)[0] = 0; COEF(x)[1] = 1;
  t = W_UMALLOC(n); s = W_UMALLOC(n); u = W_UMALLOC(n); g = W_UMALLOC(n);
  for ( j = 0, d = 1; 2*d <= n; d++ ) {
/*    fprintf(stderr,"d=%d\n",d); */
    for ( DEG(t) = -1, i = 0; i <= DEG(w); i++ )
      if ( COEF(w)[i] ) {
        Mulsum(mod,base[i],COEF(w)[i],s); 
        addum(mod,s,t,u); cpyum(u,t);
      }
    cpyum(t,w); cpyum(v,s); subum(mod,w,x,t); Gcdum(mod,s,t,g);
    if ( DEG(g) >= 1 )
      return 0;
  }
  return 1;
}

int berlekamp(UM p,int mod,int df,UM *tab,UM *r)
{
  int n,i,j,k,nf,d,nr;
  int **mat;
  int *ind;
  UM mp,w,q,gcd,w1,w2;
  UM *u;
  int *root;

  n = DEG(p);
  ind = ALLOCA(n*sizeof(int));
  make_qmat(p,mod,tab,&mat);
  null_mod(mat,mod,n,ind);
  for ( i = 0, d = 0; i < n; i++ )
    if ( ind[i] )
      d++;
  if ( d == 1 ) {
    r[0] = UMALLOC(n); cpyum(p,r[0]); return 1;
  }
  u = ALLOCA(d*sizeof(UM *));
  r[0] = UMALLOC(n); cpyum(p,r[0]);
  null_to_sol(mat,ind,mod,n,u);
  root = ALLOCA(d*sizeof(int));
  w = W_UMALLOC(n); mp = W_UMALLOC(d);
  w1 = W_UMALLOC(n); w2 = W_UMALLOC(n);
  for ( i = 1, nf = 1; i < d; i++ ) {
    minipoly_mod(mod,u[i],p,mp);
    nr = find_root(mod,mp,root);
    for ( j = 0; j < nf; j++ ) {
      if ( DEG(r[j]) == df )
        continue;
      for ( k = 0; k < nr; k++ ) {
        cpyum(u[i],w1); cpyum(r[j],w2);
        COEF(w1)[0] = (mod-root[k]) % mod;
        gcdum(mod,w1,w2,w);
        if ( DEG(w) > 0 && DEG(w) < DEG(r[j]) ) {
          gcd = UMALLOC(DEG(w));
          q = UMALLOC(DEG(r[j])-DEG(w));
          cpyum(w,gcd); divum(mod,r[j],w,q);
          r[j] = q; r[nf++] = gcd;
        }
        if ( nf == d )
          return d;
      }
    }
  }
  /* NOTREACHED */
  error("berlekamp : cannot happen");
  return -1;
}

void minipoly_mod(int mod,UM f,UM p,UM mp)
{
  struct p_pair *list,*l,*l1,*lprev;  
  int n,d;
  UM u,p0,p1,np0,np1,q,w;

  list = (struct p_pair *)MALLOC(sizeof(struct p_pair));
  list->p0 = u = W_UMALLOC(0); DEG(u) = 0; COEF(u)[0] = 1;
  list->p1 = W_UMALLOC(0); cpyum(list->p0,list->p1);
  list->next = 0;
  n = DEG(p); w = UMALLOC(2*n);
  p0 = UMALLOC(2*n); cpyum(list->p0,p0);
  p1 = UMALLOC(2*n); cpyum(list->p1,p1);
  q = W_UMALLOC(2*n);
  while ( 1 ) {
    COEF(p0)[DEG(p0)] = 0; DEG(p0)++; COEF(p0)[DEG(p0)] = 1;
    mulum(mod,f,p1,w); DEG(w) = divum(mod,w,p,q); cpyum(w,p1);
    np0 = UMALLOC(n); np1 = UMALLOC(n);
    lnf_mod(mod,n,p0,p1,list,np0,np1);
    if ( DEG(np1) < 0 ) {
      cpyum(np0,mp); return;
    } else {
      l1 = (struct p_pair *)MALLOC(sizeof(struct p_pair));
      l1->p0 = np0; l1->p1 = np1;
      for ( l = list, lprev = 0, d = DEG(np1);
        l && (DEG(l->p1) > d); lprev = l, l = l->next );
      if ( lprev ) {
        lprev->next = l1; l1->next = l;
      } else {
        l1->next = list; list = l1;
      }
    }
  }
}

void lnf_mod(int mod,int n,UM p0,UM p1,struct p_pair *list,UM np0,UM np1)
{
  int inv,h,d1;
  UM t0,t1,s0,s1;
  struct p_pair *l;

  cpyum(p0,np0); cpyum(p1,np1);
  t0 = W_UMALLOC(n); t1 = W_UMALLOC(n);
  s0 = W_UMALLOC(n); s1 = W_UMALLOC(n);
  for ( l = list; l; l = l->next ) {
    d1 = DEG(np1);
    if ( d1 == DEG(l->p1) ) {
      inv = invm((mod-COEF(l->p1)[d1])%mod,mod);
      h = dmar(COEF(np1)[d1],inv,0,mod);
      mulsum(mod,l->p0,h,t0); addum(mod,np0,t0,s0); cpyum(s0,np0);
      mulsum(mod,l->p1,h,t1); addum(mod,np1,t1,s1); cpyum(s1,np1);
    }
  }
}

int find_root(int mod,UM p,int *root)
{
  UM *r;
  int i,j;

  r = ALLOCA((DEG(p)+1)*sizeof(UM));
  ddd(mod,p,r);
  for ( i = 0, j = 0; r[i]; i++ )
    if ( DEG(r[i]) == 1 )
      root[j++] = (mod - COEF(r[i])[0]) % mod;
  return j;
}

void showum(UM p)
{
  int i;
  int *c;

  for ( i = DEG(p), c = COEF(p); i >= 0; i-- )
    if ( c[i] )
      printf("+%dx^%d",c[i],i);
  printf("\n");
}

void showumat(int **mat,int n)
{
  int i,j;

  for ( i = 0; i < n; i++ ) {
    for ( j = 0; j < n; j++ )
      printf("%d ",mat[i][j]);
    printf("\n");
  }
}
