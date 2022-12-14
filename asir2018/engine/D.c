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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/D.c,v 1.2 2018/09/28 08:20:28 noro Exp $
*/
#include "ca.h"

void dtest(P f,ML list,int hint,DCP *dcp)
{
  int n,np,bound,q;
  int i,j,k;
  int *win;
  P g,factor,cofactor;
  Q csum,csumt;
  DCP dcf,dcf0;
  LUM *c;
  ML wlist;
  int z;

  n = UDEG(f); np = list->n; bound = list->bound; q = list->mod;
  win = W_ALLOC(np+1);
  ucsump(f,&csum); mulq(csum,(Q)COEF(DC(f)),&csumt); csum = csumt;
  wlist = W_MLALLOC(np); wlist->n = list->n;
  wlist->mod = list->mod; wlist->bound = list->bound;
  c = (LUM *)COEF(wlist); bcopy((char *)COEF(list),(char *)c,(int)(sizeof(LUM)*np));
  for ( g = f, k = 1, dcf = dcf0 = 0, win[0] = 1, --np, z = 0; ; ) {
#if 0
    if ( !(++z % 10000) )
      fprintf(stderr,"z=%d\n",z);
#endif
    if ( degtest(k,win,wlist,hint) &&
      dtestmain(g,csum,wlist,k,win,&factor,&cofactor) ) {
      NEXTDC(dcf0,dcf); DEG(dcf) = ONE; COEF(dcf) = factor;
      g = cofactor;
      ucsump(g,&csum); mulq(csum,(Q)COEF(DC(g)),&csumt); csum = csumt;
      for ( i = 0; i < k - 1; i++ ) 
        for ( j = win[i] + 1; j < win[i + 1]; j++ ) 
          c[j-i-1] = c[j];
      for ( j = win[k-1] + 1; j <= np; j++ )   
          c[j-k] = c[j];
      if ( ( np -= k ) < k ) 
        break;
      if ( np - win[0] + 1 < k ) 
        if ( ++k > np )
          break;
        else
          for ( i = 0; i < k; i++ ) 
            win[i] = i + 1;
      else 
        for ( i = 1; i < k; i++ ) 
          win[i] = win[0] + i;
    } else if ( !ncombi(1,np,k,win) ) {
      if ( k == np ) 
        break;
      else
        for ( i = 0, ++k; i < k; i++ ) 
          win[i] = i + 1;
    }
  }
  NEXTDC(dcf0,dcf); COEF(dcf) = g;
  DEG(dcf) = ONE; NEXT(dcf) = 0;*dcp = dcf0;
}

void dtestsql(P f,ML list,struct oDUM *dc,DCP *dcp)
{
  int j,n,m,b;
  P t,s,fq,fr;
  P *true;
  Z tq;
  LUM *c;
  DCP dcr,dcr0;

  n = list->n; m = list->mod; b = list->bound; c = (LUM *)list->c;
  true = (P*)ALLOCA(n*sizeof(P));
  for ( j = 0; j < n; j++ ) {
    dtestsq(m,b,f,c[j],&t);
    if ( t ) 
      true[j] = t;
    else {
      *dcp = 0;
      return;
    }
  }
  for ( t = f, j = 0; j < n; j++ ) {
    STOZ(dc[j].n,tq); pwrp(CO,true[j],tq,&s); udivpz(t,s,&fq,&fr);
    if ( fq && !fr )
      t = fq;
    else {
      *dcp = 0;
      return;
    }
  }
  for ( j = 0, dcr = dcr0 = 0; j < n; j++ ) {
    NEXTDC(dcr0,dcr); STOZ(dc[j].n,DEG(dcr)); COEF(dcr) = true[j];
  }
  NEXT(dcr) = 0; *dcp = dcr0;
}

void dtestsq(int q,int bound,P f,LUM fl,P *g)
{
  P lcf,t,fq,fr,s;
  struct oML list;
  int in = 0;
  
  list.n = 1;
  list.mod = q;
  list.bound = bound;
  list.c[0] = (pointer)fl;

  mullumarray(f,&list,1,&in,&t); mulp(CO,f,COEF(DC(f)),&lcf);
  udivpz(lcf,t,&fq,&fr);
  if( fq && !fr )
    ptozp(t,1,(Q *)&s,g);
  else
    *g = 0;
}

void dtestroot(int m,int b,P f,LUM fl,struct oDUM *dc,DCP *dcp)
{
  P t,s,u;
  DCP dcr;
  Z q;

  dtestroot1(m,b,f,fl,&t);
  if ( !t ) {
    *dcp = 0;
    return;
  }
  STOZ(dc[0].n,q); pwrp(CO,t,q,&s); subp(CO,s,f,&u);
  if ( u )
    *dcp = 0;  
  else {
    NEWDC(dcr); STOZ(dc[0].n,DEG(dcr));
    COEF(dcr) = t; NEXT(dcr) = 0; *dcp = dcr;
  }
}

void dtestroot1(int q,int bound,P f,LUM fl,P *g)
{
  P fq,fr,t;

  lumtop(VR(f),q,bound,fl,&t); udivpz(f,t,&fq,&fr);
  *g = (fq && !fr) ? t : 0;
}

int dtestmain(P g,Q csum,ML list,int k,int *in,P *fp,P *cofp)
{
  int mod;
  P fmul,lcg;
  Q csumg;
  Z nq,nr;
  P fq,fr,t;
  
  if (!ctest(g,list,k,in))
    return 0;
  mod = list->mod;
  mullumarray(g,list,k,in,&fmul); mulp(CO,g,COEF(DC(g)),&lcg);
  if ( csum ) {
    ucsump(fmul,&csumg);
    if ( csumg ) {
      divqrz((Z)csum,(Z)csumg,&nq,&nr);
      if ( nr )
        return 0;
    } 
  }
  udivpz(lcg,fmul,&fq,&fr);
  if ( fq && !fr ) {
    ptozp(fq,1,(Q *)&t,cofp); ptozp(fmul,1,(Q *)&t,fp);
    return 1;
  } else
    return 0;
}

int degtest(int k,int *win,ML list,int hint)
{
  register int i,d;
  LUM *c;

  if ( hint == 1 )
    return 1;
  for ( c = (LUM*)list->c, i = 0, d = 0; i < k; i++ ) 
    d += DEG(c[win[i]]);
  return !(d % hint);
}

int ctest(P g,ML list,int k,int *in)
{
  register int i;
  int q,bound,len;
  unsigned int *wm,*wm1,*tmpp;
  DCP dc;
  Z dvr;
  Z cstn,dndn,dmyn,rn;
  unsigned int *lcn;
  LUM *l;

  for ( dc = DC(g); dc && DEG(dc); dc = NEXT(dc) );
  if ( dc ) 
    cstn = (Z)COEF(dc);
  else 
    return 1;
  q = list->mod; bound = list->bound;
  len = ztonadic(q,(Z)COEF(DC(g)),&lcn);
  W_CALLOC(bound+1,unsigned int,wm); W_CALLOC(bound+1,unsigned int,wm1); 
  for ( i = 0; i < len; i++ ) 
    wm[i] = lcn[i];
  for ( i = 0, l = (LUM *)list->c; i < k; i++ ) {
    mulpadic(q,bound,wm,(unsigned int *)COEF(l[in[i]])[0],wm1);
    tmpp = wm; wm = wm1; wm1 = tmpp;
  }
  padictoq(q,bound,(int *)wm,&dvr);
  mulz((Z)COEF(DC(g)),cstn,&dndn); divqrz(dndn,dvr,&dmyn,&rn);
  return rn ? 0 : 1;
}

/*
int ncombi(n0,n,k,c)
int n0,n,k,*c;
{
  register int i,tmp;
  
  if ( !k ) 
    return 0;
  if ( !ncombi(c[1],n,k-1,c+1) ) {
    if ( c[0] + k > n ) 
      return 0;
    else {
      for ( i = 0, tmp = c[0]; i < k; i++ ) 
        c[i] = tmp + i + 1;
      return 1;
    }
  } else 
    return 1;
}
*/

int ncombi(int n0,int n,int k,int *c)
{
  register int i,t;
  
  if ( !k ) 
    return 0;
  for ( i = k-1; i >= 0 && c[i] == n+i-(k-1); i-- );
  if ( i < 0 )
    return 0;
  t = ++c[i++]; 
  for ( t++ ; i < k; i++, t++ )
    c[i] = t;
  return 1;
}

void nthrootz(Z number,int n,Z *root)
{
  Z s,t,u,pn,base,n1,n2,q,r,gcd,num,z;
  int sgn,index,p,i,tp,mlr,num0;
  unsigned int tmp;

  for (  i = 0; !(n % 2); n /= 2, i++ );
  STOZ(n,z);
  for ( index = 0, num = number; ; index++ ) {
    if ( n == 1 )
      goto TAIL;
    p = get_lprime(index);
    if ( !(num0 = remqi((Q)num,p)) ) 
      continue;
    STOZ(n,n1); STOZ(p-1,n2); gcdz(n1,n2,&gcd);
    if ( !UNIQ(gcd) )
      continue;
    tp = pwrm(p,num0,invm(n,p-1)); STOZ(tp,s);
    mlr = invm(dmb(p,n,pwrm(p,tp,n-1),&tmp),p);
    STOZ(p,base); STOZ(p,pn);
    while ( 1 ) {
      pwrz(s,z,&t); subz(num,t,&u);
      if ( !u ) {
        num = s;
        break;
      }
      if ( sgnz(u) < 0 ) {
        *root = 0;
        return;
      }
      divqrz(u,base,&q,&r);
      if ( r ) {
        *root = 0;
        return;
      }
      STOZ(dmb(p,mlr,remqi((Q)q,p),&tmp),t);
      mulz(t,base,&u); addz(u,s,&t); s = t;
      mulz(base,pn,&t); base = t;
    }
TAIL :    
    for ( ; i; i-- ) {
      sqrtz(num,&t);
      if ( !t ) {
        *root = 0;
        return;
      }
      num = t;
    }
    *root = num;
    return;
  }
}

void sqrtz(Z number,Z *root)
{
  Z a,s,r,q,sa,two;
  int sgn;

  for ( a = ONE; ; ) {
    divqrz(number,a,&q,&r); 
    subz(q,a,&s);
    if ( !s ) {
      *root = !r ? a : 0;
      return;
    } else {
      if ( sgnz(s) < 0 ) {
        chsgnz(s,&sa); sgn = -1;
      } else {
        sa = s; sgn = 1;
      }
      if ( UNIZ(sa) ) {
        *root = 0;
        return;
      } else {
        STOZ(2,two);
        divqrz(sa,two,&q,&r); 
        if ( sgn > 0 ) 
          addz(a,q,&r); 
        else
          subz(a,q,&r); 
        a = r;
      }
    }
  }
}

void lumtop(V v,int mod,int bound,LUM f,P *g)
{
  DCP dc,dc0;
  int **l;
  int i;
  Z q;

  for ( dc0 = NULL, i = DEG(f), l = COEF(f); i >= 0; i-- ) {
    padictoq(mod,bound,l[i],&q);
    if ( q ) {
      NEXTDC(dc0,dc);
      if ( i ) 
        STOZ(i,DEG(dc));
      else 
        DEG(dc) = 0;
      COEF(dc) = (P)q;
    }
  }
  if ( !dc0 )
    *g = 0;
  else {
    NEXT(dc) = 0; MKP(v,dc0,*g);
  }
}
      
void padictoq(int mod,int bound,int *p,Z *qp)
{
  int h,i,t;
  int br,sgn;
  int *c;
  Z z;

  c = W_ALLOC(bound);
  for ( i = 0; i < bound; i++ )
    c[i] = p[i];
  h = (mod%2?(mod-1)/2:mod/2); i = bound - 1;
  while ( i >= 0 && c[i] == h ) i--;
  if ( i == -1 || c[i] > h ) {
    for (i = 0, br = 0; i < bound; i++ ) 
      if ( ( t = -(c[i] + br) ) < 0 ) {
        c[i] = t + mod; br = 1;
      } else {
        c[i] = 0; br = 0;
      }
    sgn = -1;
  } else 
    sgn = 1;
  for ( i = bound - 1; ( i >= 0 ) && ( c[i] == 0 ); i--);
  if ( i == -1 ) 
    *qp = 0;
  else {
    nadictoz(mod,i+1,(unsigned int *)c,&z);
    if ( sgn < 0 ) chsgnz(z,qp);
    else *qp = z;
  }
}

void padictoq_unsigned(int,int,int *,Z *);

void lumtop_unsigned(V v,int mod,int bound,LUM f,P *g)
{
  DCP dc,dc0;
  int **l;
  int i;
  Z q;

  for ( dc0 = NULL, i = DEG(f), l = COEF(f); i >= 0; i-- ) {
    padictoq_unsigned(mod,bound,l[i],&q);
    if ( q ) {
      NEXTDC(dc0,dc);
      if ( i ) 
        STOZ(i,DEG(dc));
      else 
        DEG(dc) = 0;
      COEF(dc) = (P)q;
    }
  }
  if ( !dc0 )
    *g = 0;
  else {
    NEXT(dc) = 0; MKP(v,dc0,*g);
  }
}
      
void padictoq_unsigned(int mod,int bound,int *p,Z *qp)
{
  nadictoz(mod,bound,(unsigned int *)p,qp);
}

void mullumarray(P f,ML list,int k,int *in,P *g)
{
  int np,bound,q,n,i,u;
  int *tmpp;
  LUM lclum,wb0,wb1,tlum;
  LUM *l;
  int len;
  unsigned int *lc;

  n = UDEG(f); np = list->n; bound = list->bound; q = list->mod;
  W_LUMALLOC(n,bound,wb0); W_LUMALLOC(n,bound,wb1);
  W_LUMALLOC(0,bound,lclum);
  len = ztonadic(q,(Z)COEF(DC(f)),&lc);
  for ( i = 0, tmpp = COEF(lclum)[0], u = MIN(bound,len); i < u; i++ ) 
    tmpp[i] = lc[i];
  l = (LUM *)list->c;
  mullum(q,bound,lclum,l[in[0]],wb0);
  for ( i = 1; i < k; i++ ) {
    mullum(q,bound,l[in[i]],wb0,wb1);
    tlum = wb0; wb0 = wb1; wb1 = tlum;
  }
  lumtop(VR(f),q,bound,wb0,g);
}

void ucsump(P f,Q *s)
{
  Q t,u;
  DCP dc;

  for ( dc = DC(f), t = 0; dc; dc = NEXT(dc) ) {
    addq((Q)COEF(dc),t,&u); t = u;
  }
  *s = t;
}

