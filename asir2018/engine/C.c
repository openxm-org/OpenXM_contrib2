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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/C.c,v 1.1 2018/09/19 05:45:06 noro Exp $
*/
#include "ca.h"
#include "inline.h"
#include "base.h"

V up_var;

/* binary has at least 32 leading 0 chars. */
void binarytoz(char *binary,Z *np)
{
  int i,w,len;
  int *b;
  char buf[33];
  mpz_t z;

  binary += strlen(binary)%32;
  len = strlen(binary);
  w = len/32; /* sufficient for holding binary */
  b = CALLOC(w,sizeof(int));
  for ( i = 0; i < w; i++ ) {
    strncpy(buf,binary+len-32*(i+1),32); buf[32] = 0;
    b[i] = strtoul(buf,0,2);
  }
  for ( i = w-1; i >= 0 && !b[i]; i-- );
  if ( i < 0 )
    *np = 0;
  else {
    len = i+1;
    mpz_init(z);
    mpz_import(z,len,-1,sizeof(int),0,0,b);
    MPZTOZ(z,*np);
  }
}

/* hex has at least 8 leading 0 chars. */
void hextoz(char *hex,Z *np)
{
  int i,w,len;
  int *b;
  mpz_t z;
  char buf[9];

  hex += strlen(hex)%8;
  len = strlen(hex);
  w = len/8; /* sufficient for holding hex */
  b = CALLOC(w,sizeof(int));
  for ( i = 0; i < w; i++ ) {
    strncpy(buf,hex+len-8*(i+1),8); buf[8] = 0;
    b[i] = strtoul(buf,0,16);
  }
  for ( i = w-1; i >= 0 && !b[i]; i-- );
  if ( i < 0 )
    *np = 0;
  else {
    len = i+1;
    mpz_init(z);
    mpz_import(z,len,-1,sizeof(int),0,0,b);
    MPZTOZ(z,*np);
  }
}

/* Z -> w[0]+w[1]*base+...+w[d-1]*base^(d-1) */

int ztonadic(int base,Z n,unsigned int **nrp)
{
  int i,plc;
  size_t d;
  unsigned int *c,*x,*w,*p;
  unsigned int r;
  L m;

  if ( !n ) {
    *nrp = NULL;
    return 0;
  }
  w = mpz_export(0,&d,-1,sizeof(int),0,0,BDY(n));
  for ( i = 1, m = 1; m <= LBASE/(L)base; m *= base, i++ );

  c = (unsigned int *)W_ALLOC(d*i+1);
  x = (unsigned int *)W_ALLOC(d+1);
  for ( i = 0; i < d; i++ ) 
    x[i] = w[i];
  for ( plc = 0; d >= 1; plc++ ) {
    for ( i = d - 1, r = 0; i >= 0; i-- ) {
      DSAB((unsigned int)base,r,x[i],x[i],r)
    }
    c[plc] = r;
    if ( !x[d-1] ) d--;
  }
  
  *nrp = p = (unsigned int *)CALLOC(plc,sizeof(unsigned int));
  for ( i = 0; i < plc; i++ )
    p[i] = c[i];
  return plc;
}

/* w[0]+w[1]*base+...+w[d-1]*base^(d-1) -> Z */

void nadictoz(int base,int d,unsigned int *w,Z *nrp)
{
  unsigned int carry;
  unsigned int *x;
  int i,j,plc;
  mpz_t z;

  if ( !d ) {
    *nrp = 0;
    return;
  }
  x = (unsigned int *)CALLOC(d + 1,sizeof(unsigned int));
  for ( plc = 0, i = d - 1; i >= 0; i-- ) {
    for ( carry = w[i],j = 0; j < plc; j++ ) {
      DMA(x[j],(unsigned int)base,carry,carry,x[j])
    }
    if ( carry ) x[plc++] = carry;
  }
  mpz_init(z);
  mpz_import(z,plc,-1,sizeof(int),0,0,x);
  MPZTOZ(z,*nrp);
}

void intarraytoz(int len,unsigned int *w,Z *nrp)
{
  mpz_t z;
  mpz_init(z);
  mpz_import(z,len,-1,sizeof(int),0,0,w);
  MPZTOZ(z,*nrp);
}

void ptomp(int m,P p,P *pr)
{
  DCP dc,dcr,dcr0;
  unsigned int a,b;
  P t;
  MQ s;

  if ( !p )
    *pr = 0;
  else if ( NUM(p) ) {
    a = remqi((Q)p,m);
    STOMQ(a,s); *pr = (P)s;
  } else {
    for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
      ptomp(m,COEF(dc),&t);
      if ( t ) {
        NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
      }
    }
    if ( !dcr0 ) 
      *pr = 0;
    else {
      NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
    }
  }
}
    
void mptop(P f,P *gp)
{
  DCP dc,dcr,dcr0;
  Z q;

  if ( !f ) 
    *gp = 0;
  else if ( NUM(f) )
    STOZ(CONT((MQ)f),q),*gp = (P)q;
  else {
    for ( dc = DC(f), dcr0 = 0; dc; dc = NEXT(dc) ) {
      NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); mptop(COEF(dc),&COEF(dcr));
    }
    NEXT(dcr) = 0; MKP(VR(f),dcr0,*gp);
  }
}

void ptosfp(P p,P *pr)
{
  DCP dc,dcr,dcr0;
  GFS a;
  P t;

  if ( !p )
    *pr = 0;
  else if ( NUM(p) ) {
    if ( NID((Num)p) == N_GFS )
      *pr = (P)p;
    else {
      qtogfs((Q)p,&a); *pr = (P)a;
    }
  } else {
    for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
      ptosfp(COEF(dc),&t);
      if ( t ) {
        NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
      }
    }
    if ( !dcr0 ) 
      *pr = 0;
    else {
      NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
    }
  }
}

void sfptop(P f,P *gp)
{
  DCP dc,dcr,dcr0;
  Z q;
  MQ fq;

  if ( !f ) 
    *gp = 0;
  else if ( NUM(f) ) {
    gfstomq((GFS)f,&fq);
    STOZ(CONT(fq),q);
    *gp = (P)q;
  } else {
    for ( dc = DC(f), dcr0 = 0; dc; dc = NEXT(dc) ) {
      NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); sfptop(COEF(dc),&COEF(dcr));
    }
    NEXT(dcr) = 0; MKP(VR(f),dcr0,*gp);
  }
}

void sfptopsfp(P f,V v,P *gp)
{
  DCP dc,dcr,dcr0;
  Q q;
  P fq;

  if ( !f ) 
    *gp = 0;
  else if ( NUM(f) )
    gfstopgfs((GFS)f,v,gp);
  else {
    for ( dc = DC(f), dcr0 = 0; dc; dc = NEXT(dc) ) {
      NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); 
      sfptopsfp(COEF(dc),v,&COEF(dcr));
    }
    NEXT(dcr) = 0; MKP(VR(f),dcr0,*gp);
  }
}

void sf_galois_action(P p,Q e,P *pr)
{
  DCP dc,dcr,dcr0;
  GFS a;
  P t;

  if ( !p )
    *pr = 0;
  else if ( NUM(p) ) {
    gfs_galois_action((GFS)p,e,&a); *pr = (P)a;
  } else {
    for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
      sf_galois_action(COEF(dc),e,&t);
      if ( t ) {
        NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
      }
    }
    if ( !dcr0 ) 
      *pr = 0;
    else {
      NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
    }
  }
}

/* GF(pn)={0,1,a,a^2,...} -> GF(pm)={0,1,b,b^2,..} ; a -> b^k */

void sf_embed(P p,int k,int pm,P *pr)
{
  DCP dc,dcr,dcr0;
  GFS a;
  P t;

  if ( !p )
    *pr = 0;
  else if ( NUM(p) ) {
    gfs_embed((GFS)p,k,pm,&a); *pr = (P)a;
  } else {
    for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
      sf_embed(COEF(dc),k,pm,&t);
      if ( t ) {
        NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
      }
    }
    if ( !dcr0 ) 
      *pr = 0;
    else {
      NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
    }
  }
}

void ptolmp(P p,P *pr)
{
  DCP dc,dcr,dcr0;
  LM a;
  P t;

  if ( !p )
    *pr = 0;
  else if ( NUM(p) ) {
    qtolm((Q)p,&a); *pr = (P)a;
  } else {
    for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
      ptolmp(COEF(dc),&t);
      if ( t ) {
        NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
      }
    }
    if ( !dcr0 ) 
      *pr = 0;
    else {
      NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
    }
  }
}
    
void lmptop(P f,P *gp)
{
  DCP dc,dcr,dcr0;
  Z q;

  if ( !f ) 
    *gp = 0;
  else if ( NUM(f) ) {
    MPZTOZ(((LM)f)->body,q); *gp = (P)q;
  } else {
    for ( dc = DC(f), dcr0 = 0; dc; dc = NEXT(dc) ) {
      NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); lmptop(COEF(dc),&COEF(dcr));
    }
    NEXT(dcr) = 0; MKP(VR(f),dcr0,*gp);
  }
}

void ptoum(int m,P f,UM wf)
{
  unsigned int r;
  int i;
  DCP dc;

  for ( i = UDEG(f); i >= 0; i-- ) 
    COEF(wf)[i] = 0;

  for ( dc = DC(f); dc; dc = NEXT(dc) ) {
    r = remqi(((Q)COEF(dc)),m);
    COEF(wf)[ZTOS(DEG(dc))] = r;
  }
  degum(wf,UDEG(f));
}

void umtop(V v,UM w,P *f)
{
  int *c;
  DCP dc,dc0;
  int i;
  Z q;

  if ( DEG(w) < 0 )
    *f = 0;
  else if ( DEG(w) == 0 ) 
    STOZ(COEF(w)[0],q), *f = (P)q;
  else {
    for ( i = DEG(w), c = COEF(w), dc0 = 0; i >= 0; i-- ) 
      if ( c[i] ) {
        NEXTDC(dc0,dc);
        STOZ(i,DEG(dc));
        STOZ(c[i],q), COEF(dc) = (P)q;
      }
    NEXT(dc) = 0;
    MKP(v,dc0,*f);
  }
}

void ptosfum(P f,UM wf)
{
  GFS c;
  int i;
  DCP dc;

  if ( OID(f) == O_N ) {
    DEG(wf) = 0;
    ntogfs((Obj)f,&c);
    COEF(wf)[0] = FTOIF(CONT(c));
    return;
  }

  for ( i = UDEG(f); i >= 0; i-- ) 
    COEF(wf)[i] = 0;

  for ( dc = DC(f); dc; dc = NEXT(dc) ) {
    ntogfs((Obj)COEF(dc),&c);
    if ( c )
      COEF(wf)[ZTOS(DEG(dc))] = FTOIF(CONT(c));
  }
  degum(wf,UDEG(f));
}

void sfumtop(V v,UM w,P *f)
{
  int *c;
  DCP dc,dc0;
  int i,t;
  GFS q;

  if ( DEG(w) < 0 )
    *f = 0;
  else if ( DEG(w) == 0 ) {
    t = COEF(w)[0];
    t = IFTOF(t);
    MKGFS(t,q);
    *f = (P)q;
  } else {
    for ( i = DEG(w), c = COEF(w), dc0 = 0; i >= 0; i-- ) 
      if ( c[i] ) {
        NEXTDC(dc0,dc);
        STOZ(i,DEG(dc));
        t = COEF(w)[i];
        t = IFTOF(t);
        MKGFS(t,q);
        COEF(dc) = (P)q;
      }
    NEXT(dc) = 0;
    MKP(v,dc0,*f);
  }
}

void ptoup(P n,UP *nr)
{
  DCP dc;
  UP r;
  int d;

  if ( !n )
    *nr = 0;
  else if ( OID(n) == O_N ) {
    *nr = r = UPALLOC(0);
    DEG(r) = 0; COEF(r)[0] = (Num)n;
  } else {
    d = UDEG(n);
    up_var = VR(n);
    *nr = r = UPALLOC(d); DEG(r) = d;
    for ( dc = DC(n); dc; dc = NEXT(dc) ) {
      COEF(r)[ZTOS(DEG(dc))] = (Num)COEF(dc);
    }
  }
}

void uptop(UP n,P *nr)
{
  int i;
  DCP dc0,dc;

  if ( !n )
    *nr = 0;
  else if ( !DEG(n) )
    *nr = (P)COEF(n)[0];
  else {
    for ( i = DEG(n), dc0 = 0; i >= 0; i-- )
      if ( COEF(n)[i] ) {
        NEXTDC(dc0,dc); STOZ(i,DEG(dc)); COEF(dc) = (P)COEF(n)[i];
      }
    if ( !up_var )
      up_var = CO->v;
    MKP(up_var,dc0,*nr);
  }
}

void ulmptoum(int m,UP f,UM wf)
{
  int i,d;
  LM *c;
  Z z;

  if ( !f )
    wf->d = -1;
  else {
    wf->d = d = f->d;
    c = (LM *)f->c;
    for ( i = 0, d = f->d; i <= d; i++ ) {
      MPZTOZ(BDY(c[i]),z);
      COEF(wf)[i] = remqi((Q)z,m);
    }
  }
}

void mptoum(P p,UM pr)
{
  DCP dc;

  if ( !p ) 
    DEG(pr) = -1;
  else if ( NUM(p) ) {
    DEG(pr) = 0; COEF(pr)[0] = CONT((MQ)p);
  } else {
    bzero((char *)pr,(int)((UDEG(p)+2)*sizeof(int)));
    for ( dc = DC(p); dc; dc = NEXT(dc) )
      COEF(pr)[ZTOS(DEG(dc))] = CONT((MQ)COEF(dc));
    degum(pr,UDEG(p));
  }
}

void umtomp(V v,UM p,P *pr)
{
  DCP dc,dc0;
  int i;
  MQ q;

  if ( !p || (DEG(p) < 0) ) 
    *pr = 0;
  else if ( !DEG(p) )
    STOMQ(COEF(p)[0],q), *pr = (P)q;
  else {
    for ( dc0 = 0, i = DEG(p); i >= 0; i-- )
      if ( COEF(p)[i] ) {
        NEXTDC(dc0,dc); STOZ(i,DEG(dc));
        STOMQ(COEF(p)[i],q), COEF(dc) = (P)q;
      }
    NEXT(dc) = 0; MKP(v,dc0,*pr);
  }
}

/* f(p) -> f(x) */

void enc_to_p(int p,int a,V v,P *pr)
{
  DCP dc,dct;
  int i,c;
  Z dq,cq;

  dc = 0;
  for ( i = 0; a; i++, a /= p ) {
    c = a%p;
    if ( c ) {
      STOZ(i,dq); STOZ(c,cq);
      NEWDC(dct); DEG(dct) = dq; COEF(dct) = (P)cq;
      NEXT(dct) = dc; dc = dct;
    }
  }
  MKP(v,dc,*pr);
}
