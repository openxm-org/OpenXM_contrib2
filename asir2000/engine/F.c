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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/F.c,v 1.12 2013/01/08 07:25:58 noro Exp $ 
*/
#include "ca.h"
#include <math.h>

int use_new_hensel;

void fctrp(VL vl,P f,DCP *dcp)
{
  VL nvl;
  DCP dc;

  if ( !f || NUM(f) ) {
    NEWDC(dc); COEF(dc) = f; DEG(dc) = ONE;
    NEXT(dc) = 0; *dcp = dc;
    return;
  } else if ( !qpcheck((Obj)f) )
    error("fctrp : invalid argument");  
  else {
    clctv(vl,f,&nvl);
    if ( !NEXT(nvl) ) 
      ufctr(f,1,dcp);
    else
      mfctr(nvl,f,dcp);
  }
}

void fctr_wrt_v_p(VL vl,P f,V v,DCP *dcp)
{
  VL nvl;
  DCP dc;

  if ( !f || NUM(f) ) {
    NEWDC(dc); COEF(dc) = f; DEG(dc) = ONE;
    NEXT(dc) = 0; *dcp = dc;
    return;
  } else if ( !qpcheck((Obj)f) )
    error("fctrp : invalid argument");  
  else {
    clctv(vl,f,&nvl);
    if ( !NEXT(nvl) ) 
      ufctr(f,1,dcp);
    else
      mfctr_wrt_v(nvl,f,v,dcp);
  }
}

void homfctr(VL vl,P g,DCP *dcp)
{
  P s,t,u,f,h,x;
  Q e;
  int d,d0;
  DCP dc,dct;

  substp(vl,g,vl->v,(P)ONE,&t); fctrp(vl,t,&dc); MKV(vl->v,x);
  for ( dct = dc; dct; dct = NEXT(dct) )
    if ( !NUM(dc->c) ) {
      for ( s = 0, f = dc->c, d = d0 = homdeg(f); f; d = homdeg(f) ) {
        exthp(vl,f,d,&h); STOQ(d0-d,e); pwrp(vl,x,e,&t);
        mulp(vl,t,h,&u); addp(vl,s,u,&t); s = t;
        subp(vl,f,h,&u); f = u;
      }
      dc->c = s;
    }
  *dcp = dc;
}

void mfctr(VL vl,P f,DCP *dcp)
{
  DCP dc,dc0,dct,dcs,dcr;
  P p,pmin,ppmin,cmin,t;
  VL mvl;
  Q c;

  ptozp(f,1,&c,&p);
  NEWDC(dc0); dc = dc0; COEF(dc) = (P)c; DEG(dc) = ONE; NEXT(dc) = 0;
  msqfr(vl,p,&dct);
  for ( ; dct; dct = NEXT(dct) ) {
    mindegp(vl,COEF(dct),&mvl,&pmin); 
#if 0
    minlcdegp(vl,COEF(dct),&mvl,&pmin); 
    min_common_vars_in_coefp(vl,COEF(dct),&mvl,&pmin); 
#endif
    pcp(mvl,pmin,&ppmin,&cmin);
    if ( !NUM(cmin) ) {
      mfctr(mvl,cmin,&dcs);
      for ( dcr = NEXT(dcs); dcr; dcr = NEXT(dcr) ) {
        DEG(dcr) = DEG(dct);
        reorderp(vl,mvl,COEF(dcr),&t); COEF(dcr) = t;
      }
      for ( ; NEXT(dc); dc = NEXT(dc) );
      NEXT(dc) = NEXT(dcs);
    }
    mfctrmain(mvl,ppmin,&dcs);
    for ( dcr = dcs; dcr; dcr = NEXT(dcr) ) {
      DEG(dcr) = DEG(dct);
      reorderp(vl,mvl,COEF(dcr),&t); COEF(dcr) = t;
    }
    for ( ; NEXT(dc); dc = NEXT(dc) );
    NEXT(dc) = dcs;
  }
  adjsgn(f,dc0); *dcp = dc0;
}

void mfctr_wrt_v(VL vl,P f,V v,DCP *dcp)
{
  DCP dc,dc0,dct,dcs,dcr;
  P p,pmin,ppmin,cmin,t;
  VL nvl,mvl;
  Q c;

  ptozp(f,1,&c,&p);
  NEWDC(dc0); dc = dc0; COEF(dc) = (P)c; DEG(dc) = ONE; NEXT(dc) = 0;
  msqfr(vl,p,&dct);
  for ( ; dct; dct = NEXT(dct) ) {
    clctv(vl,COEF(dct),&nvl);
    reordvar(nvl,v,&mvl);
    reorderp(mvl,vl,COEF(dct),&pmin);
    pcp(mvl,pmin,&ppmin,&cmin);
    if ( !NUM(cmin) ) {
      mfctrmain(mvl,cmin,&dcs);
      for ( dcr = dcs; dcr; dcr = NEXT(dcr) ) {
        DEG(dcr) = DEG(dct);
        reorderp(vl,mvl,COEF(dcr),&t); COEF(dcr) = t;
      }
      for ( ; NEXT(dc); dc = NEXT(dc) );
      NEXT(dc) = dcs;
    }
    mfctrmain(mvl,ppmin,&dcs);
    for ( dcr = dcs; dcr; dcr = NEXT(dcr) ) {
      DEG(dcr) = DEG(dct);
      reorderp(vl,mvl,COEF(dcr),&t); COEF(dcr) = t;
    }
    for ( ; NEXT(dc); dc = NEXT(dc) );
    NEXT(dc) = dcs;
  }
  adjsgn(f,dc0); *dcp = dc0;
}

#if 0
void adjsgn(P p,DCP dc)
{
  int sgn;
  DCP dct;
  P c;

  for ( dct = dc, sgn = headsgn(p); dct; dct = NEXT(dct) )
    if ( !EVENN(NM(dct->d)) )
      sgn *= headsgn(COEF(dct));
  if ( sgn < 0 ) {
    chsgnp(COEF(dc),&c); COEF(dc) = c;
  }
}
#else
void adjsgn(P p,DCP dc)
{     
  DCP dct;
  P c;

  if ( headsgn(COEF(dc)) != headsgn(p) ) {
    chsgnp(COEF(dc),&c); COEF(dc) = c;
  }
  for ( dct = NEXT(dc); dct; dct = NEXT(dct) )
    if ( headsgn(COEF(dct)) < 0 ) {
      chsgnp(COEF(dct),&c); COEF(dct) = c;
    }
}
#endif

int headsgn(P p)
{
  if ( !p )
    return 0;
  else {
    while ( !NUM(p) )
      p = COEF(DC(p));
    return SGN((Q)p);
  }
}

void fctrwithmvp(VL vl,P f,V v,DCP *dcp)
{
  VL nvl;
  DCP dc;

  if ( NUM(f) ) {
    NEWDC(dc); COEF(dc) = f; DEG(dc) = ONE;
    NEXT(dc) = 0; *dcp = dc;
    return;
  }

  clctv(vl,f,&nvl);
  if ( !NEXT(nvl) ) 
    ufctr(f,1,dcp);
  else
    mfctrwithmv(nvl,f,v,dcp);
}

void mfctrwithmv(VL vl,P f,V v,DCP *dcp)
{
  DCP dc,dc0,dct,dcs,dcr;
  P p,pmin,ppmin,cmin,t;
  VL mvl;
  Q c;

  ptozp(f,1,&c,&p);
  NEWDC(dc0); dc = dc0; COEF(dc) = (P)c; DEG(dc) = ONE; NEXT(dc) = 0;
  msqfr(vl,p,&dct);
  for ( ; dct; dct = NEXT(dct) ) {
    if ( !v )
      mindegp(vl,COEF(dct),&mvl,&pmin);
    else {
      reordvar(vl,v,&mvl); reorderp(mvl,vl,COEF(dct),&pmin);
    }
    pcp(mvl,pmin,&ppmin,&cmin);
    if ( !NUM(cmin) ) {
      mfctrmain(mvl,cmin,&dcs);
      for ( dcr = dcs; dcr; dcr = NEXT(dcr) ) {
        DEG(dcr) = DEG(dct);
        reorderp(vl,mvl,COEF(dcr),&t); COEF(dcr) = t;
      }
      for ( ; NEXT(dc); dc = NEXT(dc) );
      NEXT(dc) = dcs;
    }
    mfctrmain(mvl,ppmin,&dcs);
    for ( dcr = dcs; dcr; dcr = NEXT(dcr) ) {
      DEG(dcr) = DEG(dct);
      reorderp(vl,mvl,COEF(dcr),&t); COEF(dcr) = t;
    }
    for ( ; NEXT(dc); dc = NEXT(dc) );
    NEXT(dc) = dcs;
  }
  *dcp = dc0;
}

void ufctr(P f,int hint,DCP *dcp)
{
  P p,c;
  DCP dc,dct,dcs,dcr;

  ptozp(f,SGN((Q)UCOEF(f)),(Q *)&c,&p);
  NEWDC(dc); COEF(dc) = c; DEG(dc) = ONE; NEXT(dc) = 0;
  usqp(p,&dct);
  for ( *dcp = dc; dct; dct = NEXT(dct) ) {
    ufctrmain(COEF(dct),hint,&dcs);
    for ( dcr = dcs; dcr; dcr = NEXT(dcr) ) 
      DEG(dcr) = DEG(dct);
    for ( ; NEXT(dc); dc = NEXT(dc) );
    NEXT(dc) = dcs;
  }
}

void mfctrmain(VL vl,P p,DCP *dcp)
{
  int i,j,k,*win,np,x;
  VL nvl,tvl;
  VN vn,vnt,vn1;
  P p0,f,g,f0,g0,s,t,lp,m;
  P *fp0,*fpt,*l,*tl;
  DCP dc,dc0,dcl;
  int count,nv;
  int *nonzero;

  if ( !cmpq(DEG(DC(p)),ONE) ) {
    NEWDC(dc); DEG(dc) = ONE; COEF(dc) = p; NEXT(dc) = 0;
    *dcp = dc; return;
  }
  lp = LC(p); fctrp(vl,lp,&dcl); clctv(vl,p,&nvl);
  for ( nv = 0, tvl = nvl; tvl; tvl = NEXT(tvl), nv++);
  W_CALLOC(nv,struct oVN,vn); W_CALLOC(nv,struct oVN,vnt);
  W_CALLOC(nv,struct oVN,vn1);
  W_CALLOC(nv,int,nonzero);

  for ( i = 0, tvl = NEXT(nvl); tvl; tvl = NEXT(tvl), i++ ) 
    vn1[i].v = vn[i].v = tvl->v;
  vn1[i].v = vn[i].v = 0;

  /* determine a heuristic bound of deg(GCD(p,p')) */
  while ( 1 ) {
    for ( i = 0; vn1[i].v; i++ )
      vn1[i].n = ((unsigned int)random())%256+1;
    substvp(nvl,LC(p),vn1,&p0);
    if ( p0 ) {
      substvp(nvl,p,vn1,&p0);
      if ( sqfrchk(p0) ) {
        ufctr(p0,1,&dc0); 
        if ( NEXT(NEXT(dc0)) == 0 ) {
          NEWDC(dc); DEG(dc) = ONE; COEF(dc) = p; NEXT(dc) = 0;
          *dcp = dc;
          return;
        } else {
          for ( dc0 = NEXT(dc0), np = 0; dc0; dc0 = NEXT(dc0), np++ );
          break;
        }
      }
    }
  }
  
  /* determine the position of variables which is not allowed to
     be set to 0 */
  for ( i = 0; vn1[i].v; i++ ) {
    x = vn1[i].n; vn1[i].n = 0;
    substvp(nvl,LC(p),vn1,&p0);
    if ( !p0 )
      vn1[i].n = x;
    else {
      substvp(nvl,p,vn1,&p0);
      if ( !sqfrchk(p0) )
        vn1[i].n = x;
      else {
        ufctr(p0,1,&dc0); 
        for ( dc0 = NEXT(dc0), j = 0; dc0; dc0 = NEXT(dc0), j++ );
        if ( j > np )
          vn1[i].n = x;
      }
    }
  }
  for ( i = 0; vn1[i].v; i++ )
    if (vn1[i].n )
      nonzero[i] = 1;

  count = 0;
  while  ( 1 ) {
    while ( 1 ) {
      count++;
      for ( i = 0, j = 0; vn[i].v; i++ ) 
        if ( vn[i].n )
          vnt[j++].v = (V)i;
      vnt[j].n = 0; mulsgn(vn,vnt,j,vn1);
      for ( i = 0; vn1[i].v; i++ )
        if ( vn1[i].n ) {
          if ( vn1[i].n > 0 )
            vn1[i].n = sprime[vn1[i].n];
          else
            vn1[i].n = -sprime[-vn1[i].n];
        }
      if ( valideval(nvl,dcl,vn1) ) {
        substvp(nvl,p,vn1,&p0);
        if ( sqfrchk(p0) ) {
          ufctr(p0,1,&dc0); 
          if ( NEXT(NEXT(dc0)) == 0 ) {
            NEWDC(dc); DEG(dc) = ONE; COEF(dc) = p; NEXT(dc) = 0;
            *dcp = dc;
            return;
          } else {
            for ( dc = NEXT(dc0), i = 0; dc; dc = NEXT(dc), i++ );
            if ( i <= np )
              goto MAIN;
            if ( i < np )
              np = i;
          }
        }
      }
      if ( nextbin(vnt,j) ) 
        break;
    }
    while ( 1 ) {
      next(vn);
      for ( i = 0; vn[i].v; i++ )
        if ( nonzero[i] && !vn[i].n )
          break;
      if ( !vn[i].v )
        break;
    }
  }
MAIN :
#if 0
  for ( i = 0; vn1[i].v; i++ )
    fprintf(stderr,"%d ",vn1[i].n);
  fprintf(stderr,"\n");
#endif
  for ( np = 0, dc = NEXT(dc0); dc; dc = NEXT(dc), np++ );
  fp0 = (P *)ALLOCA((np + 1)*sizeof(P)); 
  fpt = (P *)ALLOCA((np + 1)*sizeof(P));
  l = tl = (P *)ALLOCA((np + 1)*sizeof(P));
  for ( i = 1, dc = NEXT(dc0); dc; i++, dc = NEXT(dc) ) 
    fp0[i-1] = COEF(dc);
#if 0
  sort_by_deg(np,fp0,fpt);
  sort_by_deg_rev(np-1,fpt+1,fp0+1);
#endif
#if 0
  sort_by_deg_rev(np,fp0,fpt); fp0[0] = fpt[0];
  sort_by_deg(np-1,fpt+1,fp0+1); fp0[np] = 0;
#endif
  fp0[np] = 0;
  win = W_ALLOC(np + 1); f = p; divsp(vl,p0,COEF(dc0),&f0);
  for ( k = 1, win[0] = 1, --np; ; ) {
    P h0,lcg,lch;
    Q c;

    g0 = fp0[win[0]];
    for ( i = 1; i < k; i++ ) {
      mulp(vl,g0,fp0[win[i]],&m); g0 = m;
    }
    mulq((Q)LC(g0),(Q)COEF(dc0),&c); estimatelc(nvl,c,dcl,vn1,&lcg);
    divsp(nvl,f0,g0,&h0);
    mulq((Q)LC(h0),(Q)COEF(dc0),&c); estimatelc(nvl,c,dcl,vn1,&lch);
    mfctrhen2(nvl,vn1,f,f0,g0,h0,lcg,lch,&g);    
    if ( g ) {
      *tl++ = g; divsp(vl,f,g,&t); 
      f = t; divsp(vl,f0,g0,&t); ptozp(t,1,(Q *)&s,&f0); 
      for ( i = 0; i < k - 1; i++ ) 
        for ( j = win[i] + 1; j < win[i + 1]; j++ ) 
          fp0[j - i - 1] = fp0[j];
      for ( j = win[k - 1] + 1; j <= np; j++ )   
          fp0[j - k] = fp0[j];
      if ( ( np -= k ) < k ) break;
      if ( np - win[0] + 1 < k ) 
        if ( ++k <= np ) {
          for ( i = 0; i < k; i++ ) win[i] = i + 1;
          continue;
        } else
          break;
      else for ( i = 1; i < k; i++ ) win[i] = win[0] + i;
    } else {
      if ( ncombi(1,np,k,win) == 0 ) 
        if ( k == np ) break;
        else for ( i = 0, ++k; i < k; i++ ) win[i] = i + 1;
    }
  }
  *tl++ = f; *tl = 0;
  for ( dc0 = 0; *l; l++ ) {
    NEXTDC(dc0,dc); DEG(dc) = ONE; COEF(dc) = *l;
  }
  NEXT(dc) = 0; *dcp = dc0;
}

void ufctrmain(P p,int hint,DCP *dcp)
{
  ML list;
  DCP dc;

  if ( NUM(p) || (UDEG(p) == 1) ) {
    NEWDC(dc); DEG(dc) = ONE; COEF(dc) = p; NEXT(dc) = 0;
    *dcp = dc;
  } else if ( iscycm(p) )
    cycm(VR(p),UDEG(p),dcp);
  else if ( iscycp(p) )
    cycp(VR(p),UDEG(p),dcp);
  else {
    if ( use_new_hensel )
      hensel2(5,5,p,&list);
    else
      hensel(5,5,p,&list);
    if ( list->n == 1 ) {
      NEWDC(dc); DEG(dc) = ONE; COEF(dc) = p; NEXT(dc) = 0;
      *dcp = dc;
    } else 
      dtest(p,list,hint,dcp);
  }
}

void cycm(V v,int n,DCP *dcp)
{
  register int i,j;
  struct oMF *mfp;
  DCP dc,dc0;

  if ( n == 1 ) {
    NEWDC(dc); mkssum(v,1,1,-1,&COEF(dc)); DEG(dc) = ONE;
  } else {
    mfp = (struct oMF *) ALLOCA((n+1)*sizeof(struct oMF));
    bzero((char *)mfp,(int)(sizeof(struct oMF)*(n+1)));
    for ( i = 1, j = 0; i <= n; i++ )
      if ( !(n%i) ) mfp[j++].m = i;
    mkssum(v,1,1,-1,&mfp[0].f);
    for ( i = 1; i < j; i++ )
      calcphi(v,i,mfp);
    for ( dc0 = 0, i = 0; i < j; i++ ) {
      NEXTDC(dc0,dc); COEF(dc) = mfp[i].f; DEG(dc) = ONE;
    }
  }
  NEXT(dc) = 0; *dcp = dc0;
}

void cycp(V v,int n,DCP *dcp)
{
  register int i,j;
  int n0;
  struct oMF *mfp;
  DCP dc,dc0;

  if ( n == 1 ) {
    NEWDC(dc); mkssum(v,1,1,1,&COEF(dc)); DEG(dc) = ONE;
  } else {
    n0 = n; n *= 2;
    mfp = (struct oMF *) ALLOCA((n+1)*sizeof(struct oMF));
    bzero((char *)mfp,(int)(sizeof(struct oMF)*(n+1)));
    for ( i = 1, j = 0; i <= n; i++ )
      if ( !(n%i) ) mfp[j++].m = i;
    mkssum(v,1,1,-1,&mfp[0].f);
    for ( i = 1; i < j; i++ )
      calcphi(v,i,mfp);
    for ( dc0 = 0, i = 0; i < j; i++ ) 
      if ( n0 % mfp[i].m ) {
        NEXTDC(dc0,dc); COEF(dc) = mfp[i].f; DEG(dc) = ONE;
      }
  }
  NEXT(dc) = 0; *dcp = dc0;
}

void calcphi(V v,int n,struct oMF *mfp)
{
  register int i,m;
  P t,s,tmp;

  for ( t = (P)ONE, i = 0, m = mfp[n].m; i < n; i++ )
    if ( !(m%mfp[i].m) ) {
      mulp(CO,t,mfp[i].f,&s); t = s;
    }
  mkssum(v,m,1,-1,&s); udivpz(s,t,&mfp[n].f,&tmp);
  if ( tmp ) 
    error("calcphi: cannot happen");
}

void mkssum(V v,int e,int s,int sgn,P *r)
{
  register int i,sgnt;
  DCP dc,dc0;
  Q q;

  for ( dc0 = 0, i = s, sgnt = 1; i >= 0; i--, sgnt *= sgn ) {
    if ( !dc0 ) {
      NEWDC(dc0); dc = dc0;
    } else {
      NEWDC(NEXT(dc)); dc = NEXT(dc);
    }
    STOQ(i*e,DEG(dc)); STOQ(sgnt,q),COEF(dc) = (P)q;
  }
  NEXT(dc) = 0; MKP(v,dc0,*r);
}

int iscycp(P f)
{
  DCP dc;
  dc = DC(f);

  if ( !UNIQ((Q)COEF(dc)) ) 
    return ( 0 );
  dc = NEXT(dc);
  if ( NEXT(dc) || DEG(dc) || !UNIQ((Q)COEF(dc)) ) 
    return ( 0 );
  return ( 1 );  
}

int iscycm(P f)
{
  DCP dc;

  dc = DC(f);
  if ( !UNIQ((Q)COEF(dc)) ) 
    return ( 0 );
  dc = NEXT(dc);
  if ( NEXT(dc) || DEG(dc) || !MUNIQ((Q)COEF(dc)) ) 
    return ( 0 );
  return ( 1 );  
}

void sortfs(DCP *dcp)
{
  int i,k,n,k0,d;
  DCP dc,dct,t;
  DCP *a;

  dc = *dcp;
  for ( n = 0, dct = dc; dct; dct = NEXT(dct), n++ );
  a = (DCP *)ALLOCA((n+1)*(sizeof(DCP)));
  for ( i = 0, dct = dc; dct; dct = NEXT(dct), i++ ) 
    a[i] = dct;
  a[n] = 0;

  for ( i = 0; i < n; i++ ) {
    for ( k0 = k = i, d = UDEG(COEF(a[i])); k < n; k++ ) 
      if ( (int)UDEG(COEF(a[k])) < d ) {
        k0 = k;
        d = UDEG(COEF(a[k]));
      }
    if ( i != k0 ) {
      t = a[i]; a[i] = a[k0]; a[k0] = t;
    }
  }
  for ( *dcp = dct = a[0], i = 1; i < n; i++ )
    dct = NEXT(dct) = a[i];
  NEXT(dct) = 0;
}

void sortfsrev(DCP *dcp)
{
  int i,k,n,k0,d;
  DCP dc,dct,t;
  DCP *a;

  dc = *dcp;
  for ( n = 0, dct = dc; dct; dct = NEXT(dct), n++ );
  a = (DCP *)ALLOCA((n+1)*(sizeof(DCP)));
  for ( i = 0, dct = dc; dct; dct = NEXT(dct), i++ ) 
    a[i] = dct;
  a[n] = 0;

  for ( i = 0; i < n; i++ ) {
    for ( k0 = k = i, d = UDEG(COEF(a[i])); k < n; k++ ) 
      if ( (int)UDEG(COEF(a[k])) > d ) {
        k0 = k;
        d = UDEG(COEF(a[k]));
      }
    if ( i != k0 ) {
      t = a[i]; a[i] = a[k0]; a[k0] = t;
    }
  }
  for ( *dcp = dct = a[0], i = 1; i < n; i++ )
    dct = NEXT(dct) = a[i];
  NEXT(dct) = 0;
}

void nthrootchk(P f,struct oDUM *dc,ML fp,DCP *dcp)
{
  register int i,k;
  int e,n,dr,tmp,t;
  int *tmpp,**tmppp;
  int **pp,**wpp;
  LUM fpa,tpa,f0l;
  UM wt,wq,ws,dvr,f0,fe;
  N lc;
  int lcm;
  int m,b;

  m = fp->mod; b = fp->bound; fe = *((UM *)fp->c); 
  e = dc->n; f0 = dc->f; nthrootn(NM((Q)COEF(DC(f))),e,&lc);
  if ( !lc ) {
    *dcp = 0;
    return;
  }
  lcm = rem(lc,m); W_LUMALLOC(DEG(f0),b,f0l);
  for ( i = DEG(f0), tmppp = COEF(f0l), tmpp = COEF(f0);
    i >= 0; i-- ) 
    *(tmppp[i]) = dmb(m,tmpp[i],lcm,&tmp);
  dtestroot(m,1,f,f0l,dc,dcp);
  if ( *dcp ) 
    return;
  n = UDEG(f); W_LUMALLOC(n,b,fpa); W_LUMALLOC(n,b,tpa);
  ptolum(m,b,f,fpa);
  dvr = W_UMALLOC(n); wq = W_UMALLOC(n); 
  wt = W_UMALLOC(n); ws = W_UMALLOC(n);
  cpyum(fe,dvr); divum(m,dvr,f0,wq);
  t = dmb(m,pwrm(m,lcm,e-1),e,&tmp); DEG(dvr) = DEG(wq);
  for ( k = 0; k <= DEG(wq); k++ ) 
    COEF(dvr)[k] = dmb(m,COEF(wq)[k],t,&tmp);
  for ( i = 1; i < b; i++ ) {
    pwrlum(m,i+1,f0l,e,tpa);
    for ( k = 0, pp = COEF(fpa), wpp = COEF(tpa); 
      k <= n; k++ )
      COEF(wt)[k] = (pp[k][i]-wpp[k][i]+m)%m;
    degum(wt,n); dr = divum(m,wt,dvr,ws);
    if ( dr >= 0 ) {
      *dcp = 0;
      return;
    }
    for ( k = 0, pp = COEF(f0l); k <= DEG(ws); k++ )
      pp[k][i] = COEF(ws)[k];
    dtestroot(m,i+1,f,f0l,dc,dcp);
    if ( *dcp )
      return;
  }
}

void sqfrp(VL vl,P f,DCP *dcp)
{
  P c,p;
  DCP dc,dc0;

  if ( !f || NUM(f) ) {
    NEWDC(dc0); *dcp = dc0; DEG(dc0) = ONE;
    COEF(dc0) = f; NEXT(dc0) = 0;
  } else if ( !qpcheck((Obj)f) )
    error("sqfrp : invalid argument");  
  else {
    NEWDC(dc0); *dcp = dc0; DEG(dc0) = ONE;
    ptozp(f,1,(Q *)&c,&p); msqfr(vl,p,&dc);
    COEF(dc0) = c; NEXT(dc0) = dc;
    adjsgn(f,dc0);
  }
}

/* 
 * f : must be a poly with int coef, ignore int content
 */
void msqfr(VL vl,P f,DCP *dcp)
{
  DCP dc,dct,dcs;
  P c,p,t,s,pc;
  VL mvl;

  ptozp(f,1,(Q *)&c,&t); monomialfctr(vl,t,&p,&dc);
  if ( NUM(p) ) {
    *dcp = dc;
    return;
  }
  mindegp(vl,p,&mvl,&s);
#if 0
  minlcdegp(vl,p,&mvl,&s);
  min_common_vars_in_coefp(vl,p,&mvl,&s);
#endif
  pcp(mvl,s,&p,&pc);
  if ( !NUM(pc) ) {
    msqfr(mvl,pc,&dcs);
    if ( !dc )
      dc = dcs;
    else {
      for ( dct = dc; NEXT(dct); dct = NEXT(dct) );
      NEXT(dct) = dcs;
    }
  }
  msqfrmain(mvl,p,&dcs); 
  for ( dct = dcs; dct; dct = NEXT(dct) ) {
    reorderp(vl,mvl,COEF(dct),&t); COEF(dct) = t;
  }
  if ( !dc )
    *dcp = dcs;
  else {
    for ( dct = dc; NEXT(dct); dct = NEXT(dct) );
    NEXT(dct) = dcs; *dcp = dc;
  }
}

void usqp(P f,DCP *dcp)
{
  int index,nindex;
  P g,c,h;
  DCP dc;

  ptozp(f,1,(Q *)&c,&h); 
  if ( SGN((Q)LC(h)) < 0 )
    chsgnp(h,&g);
  else
    g = h;
  for ( index = 0, dc = 0; !dc; index = nindex )
    hsq(index,5,g,&nindex,&dc);
  *dcp = dc;
}

void msqfrmain(VL vl,P p,DCP *dcp)
{
  int i,j;
  VL nvl,tvl;
  VN vn,vnt,vn1;
  P gg,tmp,p0,g;
  DCP dc,dc0,dcr,dcr0;
  int nv,d,d1;
  int found;
  VN svn1;
  P sp0;
  DCP sdc0;

  if ( deg(VR(p),p) == 1 ) {
    NEWDC(dc); DEG(dc) = ONE; COEF(dc) = p; NEXT(dc) = 0;
    *dcp = dc;
    return;
  }
  clctv(vl,p,&nvl);
  for ( nv = 0, tvl = nvl; tvl; tvl = NEXT(tvl), nv++);
  if ( nv == 1 ) {
    usqp(p,dcp);
    return;
  }
#if 0
  if ( heusqfrchk(nvl,p) ) {
    NEWDC(dc); DEG(dc) = ONE; COEF(dc) = p; NEXT(dc) = 0;
    *dcp = dc;
    return;
  }
#endif
  W_CALLOC(nv,struct oVN,vn);
  W_CALLOC(nv,struct oVN,vnt);
  W_CALLOC(nv,struct oVN,vn1);
  W_CALLOC(nv,struct oVN,svn1);
  for ( i = 0, tvl = NEXT(nvl); tvl; tvl = NEXT(tvl), i++ ) 
    vn1[i].v = vn[i].v = tvl->v;
  vn1[i].v = vn[i].v = 0;

  /* determine a heuristic bound of deg(GCD(p,p')) */
  while ( 1 ) {
    for ( i = 0; vn1[i].v; i++ )
      vn1[i].n = ((unsigned int)random())%256+1;
    substvp(nvl,LC(p),vn1,&tmp);
    if ( tmp ) {
      substvp(nvl,p,vn1,&p0);
      usqp(p0,&dc0);
      for ( d1 = 0, dc = dc0; dc; dc = NEXT(dc) )
        if ( DEG(dc) )
          d1 += (QTOS(DEG(dc))-1)*UDEG(COEF(dc));
      if ( d1 == 0 ) {
        /* p is squarefree */
        NEWDC(dc); DEG(dc) = ONE; COEF(dc) = p; NEXT(dc) = 0;
        *dcp = dc;
        return;
      } else {
        d = d1+1; /* XXX : try searching better evaluation */
        found = 0;
        break;
      }
    }
  }

  for  ( dcr0 = 0, g = p; ; ) {
    while ( 1 ) {
      for ( i = 0, j = 0; vn[i].v; i++ ) 
        if ( vn[i].n ) vnt[j++].v = (V)i;
      vnt[j].n = 0;

      mulsgn(vn,vnt,j,vn1);
      substvp(nvl,LC(g),vn1,&tmp);
      if ( tmp ) {
        substvp(nvl,g,vn1,&p0);
        usqp(p0,&dc0);
        for ( d1 = 0, dc = dc0; dc; dc = NEXT(dc) )
          if ( DEG(dc) )
            d1 += (QTOS(DEG(dc))-1)*UDEG(COEF(dc));

        if ( d1 == 0 ) {
          NEWDC(dc); DEG(dc) = ONE; COEF(dc) = g; NEXT(dc) = 0;
          if ( !dcr0 ) 
            dcr0 = dc;
          else
            NEXT(dcr) = dc;
          *dcp = dcr0;
          return;
        }

        if ( d < d1 )
          goto END;
        if ( d > d1 ) {
          d = d1;
          found = 1;
          bcopy((char *)vn1,(char *)svn1,(int)(sizeof(struct oVN)*nv));
          sp0 = p0; sdc0 = dc0;
          goto END;
        } 
        /* d1 == d */
        if ( found ) {
          found = 0;
#if 0
        if ( d > d1 ) {
          d = d1;
              /*} */
#endif
          msqfrmainmain(nvl,g,svn1,sp0,sdc0,&dc,&gg); g = gg;
          if ( dc ) {
            if ( !dcr0 )
              dcr0 = dc;
            else
              NEXT(dcr) = dc;
            for ( dcr = dc; NEXT(dcr); dcr = NEXT(dcr) );
            if ( NUM(g) ) {
              NEXT(dcr) = 0; *dcp = dcr0;
              return;
            } 
            d = deg(VR(g),g);
          }
        }
      }
END:
      if ( nextbin(vnt,j) ) 
        break;
    }
    next(vn);
  }
}

void msqfrmainmain(VL vl,P p,VN vn,P p0,DCP dc0,DCP *dcp,P *pp)
{
  int i,j,k,np;
  DCP *a;
  DCP dc,dcr,dcr0,dct;
  P g,t,s,u,t0,f,f0,d,d0,g0,h0,x,xx;
  Q q;
  V v;

  for ( np = 0, dc = dc0; dc; dc = NEXT(dc), np++ );
  a = (DCP *)ALLOCA((np + 1)*sizeof(DCP)); 
  for ( i = 0, dc = dc0; dc; i++, dc = NEXT(dc) ) 
    a[np-i-1] = dc;

  for ( i = 0, dcr0 = 0, f = p, f0 = p0, v = VR(p); 
    i < np; i++ ) { 
    if ( (i == (np-1))&&UNIQ(DEG(a[i])) ) {
      NEXTDC(dcr0,dcr);
      DEG(dcr) = DEG(a[i]);
      COEF(dcr) = f;
      f = (P)ONE;
    } else if ( (i == (np-1))&&UNIQ(DEG(DC(COEF(a[i])))) ) {
      diffp(vl,f,v,&s); pcp(vl,s,&t,&u);
      if ( divtpz(vl,f,t,&s) ) {
        NEXTDC(dcr0,dcr);
        DEG(dcr) = DEG(a[i]);
        COEF(dcr) = s;
        f = (P)ONE;
      } else 
        break;
    } else { 
      for ( t = f, t0 = f0,
        j = 0, k = QTOS(DEG(a[i]))-1; j < k; j++ ) {
        diffp(vl,t,v,&s); t = s;
        diffp(vl,t0,v,&s); t0 = s;
      }
      factorial(k,&q); 
      divsp(vl,t,(P)q,&s); 
      for ( dct = DC(s); NEXT(dct); dct = NEXT(dct) );
      if ( DEG(dct) ) {
        MKV(VR(s),x); pwrp(vl,x,DEG(dct),&xx); 
        divsp(vl,s,xx,&d); 
      } else {
        xx = (P)ONE; d = s;
      }
      divsp(vl,t0,xx,&t); 
      divsp(vl,t,(P)q,&s); 
      ptozp(s,1,(Q *)&t,&d0); 

      for ( dct = DC(COEF(a[i])); NEXT(dct); dct = NEXT(dct) );
      if ( DEG(dct) )
        divsp(vl,COEF(a[i]),xx,&g0);
      else {
        xx = (P)ONE; g0 = COEF(a[i]);
      }

      pcp(vl,d,&t,&u); d = t;
      ptozp(g0,1,(Q *)&u,&t); g0 = t;

    {
      DCP dca,dcb;

      fctrp(vl,LC(d),&dca);
      for ( dcb = dca, u = (P)ONE; dcb; dcb = NEXT(dcb) ) {
        if ( NUM(COEF(dcb)) ) {
          mulp(vl,u,COEF(dcb),&t); u = t;
        } else {
          Q qq;
          P tt;

          pwrp(vl,COEF(dcb),DEG(a[i]),&s);
          for ( t = LC(f), j = 0; divtpz(vl,t,s,&tt); j++, t = tt );
          STOQ(j,qq);
          if ( cmpq(qq,DEG(dcb)) > 0 )
            qq = DEG(dcb);
          pwrp(vl,COEF(dcb),qq,&t); mulp(vl,u,t,&s); u = s;
        }
      }
      divsp(vl,d0,g0,&h0);
    }
      mfctrhen2(vl,vn,d,d0,g0,h0,u,LC(d),&s); 
      if ( s ) {
        mulp(vl,s,xx,&g); 
        pwrp(vl,g,DEG(a[i]),&t);
        if ( divtpz(vl,f,t,&s) ) {
          NEXTDC(dcr0,dcr);
          DEG(dcr) = DEG(a[i]); COEF(dcr) = g;
          f = s; substvp(vl,f,vn,&f0);
        } else
          break;
      } else 
        break;
    }
  }
  *pp = f;  
  if ( dcr0 ) 
    NEXT(dcr) = 0;
  *dcp = dcr0;
}

void mfctrhen2(VL vl,VN vn,P f,P f0,P g0,P h0,P lcg,P lch,P *gp)
{
  V v;
  P f1,lc,lc0,lcg0,lch0;
  P m,ff0,gg0,hh0,gk,hk,ggg,gggr,hhh,ak,bk,tmp;
  Q bb,qk,s;
  Q cbd; 
  int dbd;
  int d;

  if ( NUM(g0) ) {
    *gp = (P)ONE;
    return;
  } 

  v = VR(f); d = deg(v,f);
  if ( d == deg(v,g0) ) {
    pcp(vl,f,gp,&tmp); 
    return;
  }    

  mulp(vl,lcg,lch,&lc);
  if ( !divtpz(vl,lc,LC(f),(P *)&s) ) {
    *gp = 0;
    return;
  }
  mulp(vl,(P)s,f,&f1);
  dbd = dbound(VR(f1),f1) + 1; cbound(vl,f1,&cbd);

  substvp(vl,lc,vn,&lc0);
  divq((Q)lc0,(Q)LC(f0),(Q *)&m); mulp(vl,f0,m,&ff0); 
  substvp(vl,lcg,vn,&lcg0);
  divq((Q)lcg0,(Q)LC(g0),(Q *)&m); mulp(vl,g0,m,&gg0); 
  substvp(vl,lch,vn,&lch0);
  divq((Q)lch0,(Q)LC(h0),(Q *)&m); mulp(vl,h0,m,&hh0); 
  addq(cbd,cbd,&bb);
  henzq1(gg0,hh0,bb,&bk,&ak,&qk); gk = gg0; hk = hh0;
  henmv(vl,vn,f1,gk,hk,ak,bk,
    lcg,lch,lcg0,lch0,qk,dbd,&ggg,&hhh); 

  if ( divtpz(vl,f1,ggg,&gggr) )
    pcp(vl,ggg,gp,&tmp);
  else
    *gp = 0;
}

int sqfrchk(P p)
{
  Q c;
  P f;
  DCP dc;

  ptozp(p,SGN((Q)UCOEF(p)),&c,&f); usqp(f,&dc); 
  if ( NEXT(dc) || !UNIQ(DEG(dc)) ) 
    return ( 0 );
  else 
    return ( 1 );
}

int cycchk(P p)
{
  Q c;
  P f;

  ptozp(p,SGN((Q)UCOEF(p)),&c,&f);
  if ( iscycp(f) || iscycm(f) )
    return 0;
  else 
    return 1;
}

int zerovpchk(VL vl,P p,VN vn)
{
  P t;

  substvp(vl,p,vn,&t);
  if ( t )
    return ( 0 );
  else
    return ( 1 );
}

int valideval(VL vl,DCP dc,VN vn)
{
  DCP dct;
  Q *a;
  int i,j,n;
  N q,r;

  for ( dct = NEXT(dc), n = 0; dct; dct = NEXT(dct), n++ );
  W_CALLOC(n,Q,a);
  for ( dct = NEXT(dc), i = 0; dct; dct = NEXT(dct), i++ ) {
    substvp(vl,COEF(dct),vn,(P *)&a[i]);
    if ( !a[i] )
      return ( 0 );

    for ( j = 0; j < i; j++ ) {
      divn(NM(a[j]),NM(a[i]),&q,&r);
      if ( !r )
        return ( 0 );
      divn(NM(a[i]),NM(a[j]),&q,&r);
      if ( !r )
        return ( 0 );
    }
  }
  return ( 1 );
}

void estimatelc(VL vl,Q c,DCP dc,VN vn,P *lcp)
{
  int i;
  DCP dct;
  P r,s,t;
  Q c0,c1,c2;

  for ( dct = dc, r = (P)ONE; dct; dct = NEXT(dct) ) {
    if ( NUM(COEF(dct)) ) {
      mulp(vl,r,COEF(dct),&s); r = s;
    } else {
      substvp(vl,COEF(dct),vn,(P *)&c0);
      for ( i = 0, c1 = c; i < (int)QTOS(DEG(dct)); i++ ) {
        divq(c1,c0,&c2);
        if ( !INT(c2) )
          break;
        else
          c1 = c2;
      }
      if ( i ) {
        STOQ(i,c1);
        pwrp(vl,COEF(dct),c1,&s); mulp(vl,r,s,&t); r = t;
      }
    }
  }
  *lcp = r;
}

void monomialfctr(VL vl,P p,P *pr,DCP *dcp)
{
  VL nvl,avl;
  Q d;
  P f,t,s;
  DCP dc0,dc;

  clctv(vl,p,&nvl);
  for ( dc0 = 0, avl = nvl, f = p; avl; avl = NEXT(avl) ) {
    getmindeg(avl->v,f,&d);
    if ( d ) {
      MKV(avl->v,t); NEXTDC(dc0,dc); DEG(dc) = d; COEF(dc) = t;
      pwrp(vl,t,d,&s); divsp(vl,f,s,&t); f = t;
    }
  }
  if ( dc0 )
    NEXT(dc) = 0;
  *pr = f; *dcp = dc0;
}

void afctr(VL vl,P p0,P p,DCP *dcp)
{
  DCP dc,dc0,dcr,dct,dcs;
  P t;
  VL nvl;

  if ( VR(p) == VR(p0) ) {
    NEWDC(dc);
    DEG(dc) = ONE;
    COEF(dc) = p;
    NEXT(dc) = 0;
    *dcp = dc;
    return;
  }

  clctv(vl,p,&nvl);
  if ( !NEXT(nvl) ) 
    ufctr(p,1,&dc);
  else {
    sqa(vl,p0,p,&dc);
    for ( dct = dc; dct; dct = NEXT(dct) ) {
      pmonic(vl,p0,COEF(dct),&t); COEF(dct) = t;
    }
  }
  if ( NUM(COEF(dc)) ) 
    dcr = NEXT(dc);
  else
    dcr = dc;
  for ( dc0 = 0; dcr; dcr = NEXT(dcr) ) {
    afctrmain(vl,p0,COEF(dcr),1,&dcs);

    for ( dct = dcs; dct; dct = NEXT(dct) ) 
      DEG(dct) = DEG(dcr);
    if ( !dc0 )
      dc0 = dcs;
    else {
      for ( dct = dc0; NEXT(dct); dct = NEXT(dct) );
      NEXT(dct) = dcs;
    }
  }
  *dcp = dc0;
}

void afctrmain(VL vl,P p0,P p,int init,DCP *dcp)
{
  P x,y,s,m,a,t,u,pt,pt1,res,g;
  Q q;
  DCP dc,dc0,dcsq0,dcr0,dcr,dct,dcs;
  V v,v0;

  if ( !cmpq(DEG(DC(p)),ONE) ) {
    NEWDC(dc); DEG(dc) = ONE; NEXT(dc) = 0;
    pmonic(vl,p0,p,&COEF(dc)); *dcp = dc;
    return;
  }

  v = VR(p); MKV(v,x);
  v0 = VR(p0); MKV(v0,y);
  STOQ(init,q),s = (P)q;
  mulp(vl,s,y,&m); subp(vl,x,m,&t); addp(vl,x,m,&a); 
  substp(vl,p,v,t,&pt); 
  remsdcp(vl,pt,p0,&pt1); 

/*
  if ( ( deg(v0,p0) <= 3 ) || ( TCPQ(p0) <= 2 ) ) 
    resultp(vl,v0,p0,pt1,&res);
  else 
    srcrnorm(vl,v0,pt1,p0,&res);
*/
#if 0
  srcr(vl,v0,pt1,p0,&res);
#endif
  resultp(vl,v0,p0,pt1,&res); 
  usqp(res,&dcsq0); 
  for ( dc0 = 0, dct = dcsq0; dct; dct = NEXT(dct) ) {
    if ( UNIQ(DEG(dct)) ) 
      ufctr(COEF(dct),deg(v0,p0),&dcs);
    else
      ufctr(COEF(dct),1,&dcs);
    for ( dcr = dcs; dcr; dcr = NEXT(dcr) )
      DEG(dcr) = DEG(dct);
    if ( !dc0 ) {
      dc0 = NEXT(dcs);
      dc = dc0;  
    } else {
      for ( ; NEXT(dc); dc = NEXT(dc) );
      NEXT(dc) = NEXT(dcs);
    }
  }
  sortfs(&dc0);

  for ( g = p, dcr = dcr0 = 0, dc = dc0; dc; dc = NEXT(dc) ) {
    if ( !UNIQ(DEG(dc)) ) {
      substp(vl,COEF(dc),v,a,&pt); remsdcp(vl,pt,p0,&s); 
      gcda(vl,p0,s,g,&u); 
      if ( !NUM(u) && (VR(u) == v)) {
        afctrmain(vl,p0,u,init+1,&dct);
        for ( dcs = dct, t = (P)ONE; dcs; dcs = NEXT(dcs) ) {
          mulp(vl,t,COEF(dcs),&s); remsdcp(vl,s,p0,&t);
        }
        pdiva(vl,p0,g,t,&s); g = s;
        if ( !dcr0 )
          dcr0 = dct;
        else 
          NEXT(dcr) = dct;
        for ( dcr = dct; NEXT(dcr); dcr = NEXT(dcr) );
      }
    } else {
      substp(vl,COEF(dc),v,a,&pt); remsdcp(vl,pt,p0,&s); 
      gcda(vl,p0,s,g,&u); 
      if ( !NUM(u) && (VR(u) == v)) {
        NEXTDC(dcr0,dcr);
        DEG(dcr) = ONE;
        COEF(dcr) = u;
        pdiva(vl,p0,g,u,&t); g = t;
      } 
    }
  }
  if ( dcr0 )
    NEXT(dcr) = 0;
  *dcp = dcr0;
}
