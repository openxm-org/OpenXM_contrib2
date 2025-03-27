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
 * $OpenXM: OpenXM_contrib2/asir2018/engine/distm.c,v 1.2 2018/09/28 08:20:28 noro Exp $
*/
#include "ca.h"
#include "inline.h"

extern int (*cmpdl)(int,DL,DL);
extern int do_weyl;

void ptomd(VL vl,int mod,VL dvl,P p,DP *pr)
{
  P t;

  ptomp(mod,p,&t);
  mptomd(vl,mod,dvl,t,pr);
}

void mptomd(VL vl,int mod,VL dvl,P p,DP *pr)
{
  int n,i;
  VL tvl;
  V v;
  DL d;
  MP m;
  DCP dc;
  DP r,s,t,u;
  P x,c;

  if ( !p )
    *pr = 0;
  else {
    for ( n = 0, tvl = dvl; tvl; tvl = NEXT(tvl), n++ );
    if ( NUM(p) ) {
      NEWDL(d,n);
      NEWMP(m); m->dl = d; C(m) = (Obj)p; NEXT(m) = 0; MKDP(n,m,*pr);
    } else {
      for ( i = 0, tvl = dvl, v = VR(p);
        tvl && tvl->v != v; tvl = NEXT(tvl), i++ );
      if ( !tvl ) {
        for ( dc = DC(p), s = 0, MKV(v,x); dc; dc = NEXT(dc) ) {
          mptomd(vl,mod,dvl,COEF(dc),&t); pwrmp(vl,mod,x,DEG(dc),&c);
          mulmdc(vl,mod,t,c,&r); addmd(vl,mod,r,s,&t); s = t;
        }
        *pr = s;
      } else {
        for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) ) {
          mptomd(vl,mod,dvl,COEF(dc),&t);
          NEWDL(d,n); d->d[i] = ZTOS(DEG(dc));
          d->td = MUL_WEIGHT(d->d[i],i);
          NEWMP(m); m->dl = d; C(m) = (Obj)ONEM; NEXT(m) = 0; MKDP(n,m,u);
          comm_mulmd(vl,mod,t,u,&r); addmd(vl,mod,r,s,&t); s = t;
        }
        *pr = s;
      }
    }
  }
}

void mdtodp(DP p,DP *pr)
{
  MP m,mr0,mr;

  if ( !p )
    *pr = 0;
  else {
    for ( m = BDY(p), mr0 = 0; m; m = NEXT(m) ) {
      NEXTMP(mr0,mr); mr->dl = m->dl;
      mptop((P)C(m),(P *)&C(mr));
    }
    NEXT(mr) = 0;
    MKDP(NV(p),mr0,*pr);
    (*pr)->sugar = p->sugar;
  }
}

void _mdtodp(DP p,DP *pr)
{
  MP m,mr0,mr;
  int i;
  Z q;

  if ( !p )
    *pr = 0;
  else {
    for ( m = BDY(p), mr0 = 0; m; m = NEXT(m) ) {
      NEXTMP(mr0,mr); mr->dl = m->dl;
      i = ITOS(m->c); STOZ(i,q); C(mr) = (Obj)q;
    }
    NEXT(mr) = 0;
    MKDP(NV(p),mr0,*pr);
    (*pr)->sugar = p->sugar;
  }
}

void mdtop(VL vl,int mod,VL dvl,DP p,P *pr)
{
  int n,i;
  DL d;
  MP m;
  P r,s,t,u,w;
  Z q;
  VL tvl;

  if ( !p )
    *pr = 0;
  else {
    for ( n = p->nv, m = BDY(p), s = 0; m; m = NEXT(m) ) {
      for ( i = 0, t = (P)C(m), d = m->dl, tvl = dvl;
        i < n; tvl = NEXT(tvl), i++ ) {
        MKMV(tvl->v,r); STOZ(d->d[i],q); pwrmp(vl,mod,r,q,&u);
        mulmp(vl,mod,t,u,&w); t = w;
      }
      addmp(vl,mod,s,t,&u); s = u;
    }
    mptop(s,pr);
  }
}

void addmd(VL vl,int mod,DP p1,DP p2,DP *pr)
{
  int n;
  MP m1,m2,mr,mr0;
  P t;
  int tmp;
  MQ q;

  if ( !p1 )
    *pr = p2;
  else if ( !p2 )
    *pr = p1;
  else {
    for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; )
      switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
        case 0:
          if ( NUM(C(m1)) && NUM(C(m2)) ) {
            tmp = (CONT((MQ)C(m1))+CONT((MQ)C(m2))) - mod;
            if ( tmp < 0 )
              tmp += mod;
            if ( tmp ) {
              STOMQ(tmp,q); t = (P)q;
            } else
              t = 0;
          } else
            addmp(vl,mod,(P)C(m1),(P)C(m2),&t);
          if ( t ) {
            NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = (Obj)t;
          }
          m1 = NEXT(m1); m2 = NEXT(m2); break;
        case 1:
          NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = C(m1);
          m1 = NEXT(m1); break;
        case -1:
          NEXTMP(mr0,mr); mr->dl = m2->dl; C(mr) = C(m2);
          m2 = NEXT(m2); break;
      }
    if ( !mr0 )
      if ( m1 )
        mr0 = m1;
      else if ( m2 )
        mr0 = m2;
      else {
        *pr = 0;
        return;
      }
    else if ( m1 )
      NEXT(mr) = m1;
    else if ( m2 )
      NEXT(mr) = m2;
    else
      NEXT(mr) = 0;
    MKDP(NV(p1),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = MAX(p1->sugar,p2->sugar);
  }
}

void submd(VL vl,int mod,DP p1,DP p2,DP *pr)
{
  DP t;

  if ( !p2 )
    *pr = p1;
  else {
    chsgnmd(mod,p2,&t); addmd(vl,mod,p1,t,pr);
  }
}

void chsgnmd(int mod,DP p,DP *pr)
{
  MP m,mr,mr0;

  if ( !p )
    *pr = 0;
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      NEXTMP(mr0,mr); chsgnmp(mod,(P)C(m),(P *)&C(mr)); mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = p->sugar;
  }
}

void mulmd(VL vl,int mod,DP p1,DP p2,DP *pr)
{
  if ( !do_weyl )
    comm_mulmd(vl,mod,p1,p2,pr);
  else
    weyl_mulmd(vl,mod,p1,p2,pr);
}

void comm_mulmd(VL vl,int mod,DP p1,DP p2,DP *pr)
{
  MP m;
  DP s,t,u;
  int i,l,l1;
  static MP *w;
  static int wlen;

  if ( !p1 || !p2 )
    *pr = 0;
  else if ( OID(p1) <= O_P )
    mulmdc(vl,mod,p2,(P)p1,pr);
  else if ( OID(p2) <= O_P )
    mulmdc(vl,mod,p1,(P)p2,pr);
  else {
    for ( m = BDY(p1), l1 = 0; m; m = NEXT(m), l1++ );
    for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
    if ( l1 < l ) {
      t = p1; p1 = p2; p2 = t;
      l = l1;
    }
    if ( l > wlen ) {
      if ( w ) GCFREE(w);
      w = (MP *)MALLOC(l*sizeof(MP));
      wlen = l;
    }
    for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
      w[i] = m;
    for ( s = 0, i = l-1; i >= 0; i-- ) {
      mulmdm(vl,mod,p1,w[i],&t); addmd(vl,mod,s,t,&u); s = u;
    }
    bzero(w,l*sizeof(MP));
    *pr = s;
  }
}

void weyl_mulmd(VL vl,int mod,DP p1,DP p2,DP *pr)
{
  MP m;
  DP s,t,u;
  int i,l;
  static MP *w;
  static int wlen;

  if ( !p1 || !p2 )
    *pr = 0;
  else if ( OID(p1) <= O_P )
    mulmdc(vl,mod,p2,(P)p1,pr);
  else if ( OID(p2) <= O_P )
    mulmdc(vl,mod,p1,(P)p2,pr);
  else {
    for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
    if ( l > wlen ) {
      if ( w ) GCFREE(w);
      w = (MP *)MALLOC(l*sizeof(MP));
      wlen = l;
    }
    for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
      w[i] = m;
    for ( s = 0, i = l-1; i >= 0; i-- ) {
      weyl_mulmdm(vl,mod,p1,w[i],&t); addmd(vl,mod,s,t,&u); s = u;
    }
    bzero(w,l*sizeof(MP));
    *pr = s;
  }
}

void mulmdm(VL vl,int mod,DP p,MP m0,DP *pr)
{
  MP m,mr,mr0;
  P c;
  DL d;
  int n,t;
  MQ q;

  if ( !p )
    *pr = 0;
  else {
    for ( mr0 = 0, m = BDY(p), c = (P)C(m0), d = m0->dl, n = NV(p); 
      m; m = NEXT(m) ) {
      NEXTMP(mr0,mr);
      if ( NUM(C(m)) && NUM(c) ) {
        t = dmar(((MQ)(C(m)))->cont,((MQ)c)->cont,0,mod);
        STOMQ(t,q); C(mr) = (Obj)q;
      } else
        mulmp(vl,mod,(P)C(m),c,(P *)&C(mr));
      adddl(n,m->dl,d,&mr->dl);
    }
    NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = p->sugar + m0->dl->td;
  }
}

void weyl_mulmdm(VL vl,int mod,DP p,MP m0,DP *pr)
{
  DP r,t,t1;
  MP m;
  int n,l,i;
  static MP *w;
  static int wlen;

  if ( !p )
    *pr = 0;
  else {
    for ( m = BDY(p), l = 0; m; m = NEXT(m), l++ );
    if ( l > wlen ) {
      if ( w ) GCFREE(w);
      w = (MP *)MALLOC(l*sizeof(MP));
      wlen = l;
    }
    for ( m = BDY(p), i = 0; i < l; m = NEXT(m), i++ )
      w[i] = m;
    for ( r = 0, i = l-1, n = NV(p); i >= 0; i-- ) {
      weyl_mulmmm(vl,mod,w[i],m0,n,&t);
      addmd(vl,mod,r,t,&t1); r = t1;
    }
    bzero(w,l*sizeof(MP));
    if ( r )
      r->sugar = p->sugar + m0->dl->td;
    *pr = r;
  }
}

/* m0 = x0^d0*x1^d1*... * dx0^d(n/2)*dx1^d(n/2+1)*... */

void weyl_mulmmm(VL vl,int mod,MP m0,MP m1,int n,DP *pr)
{
  MP mr,mr0;
  MQ mq;
  DP r,t,t1;
  P c,c0,c1;
  DL d,d0,d1;
  int i,j,a,b,k,l,n2,s,min;
  static int *tab;
  static int tablen;

  if ( !m0 || !m1 )
    *pr = 0;
  else {
    c0 = (P)C(m0); c1 = (P)C(m1);
    mulmp(vl,mod,c0,c1,&c);
    d0 = m0->dl; d1 = m1->dl;
    n2 = n>>1;
    if ( n & 1 ) { 
      /* homogenized computation; dx-xd=h^2 */
      /* offset of h-degree */
      NEWDL(d,n);
      d->td = d->d[n-1] = d0->d[n-1]+d1->d[n-1];
      NEWMP(mr); mr->c = (Obj)ONEM; mr->dl = d; NEXT(mr) = 0;
      MKDP(n,mr,r); r->sugar = d->td;
    } else
      r = (DP)ONEM;
    for ( i = 0; i < n2; i++ ) {
      a = d0->d[i]; b = d1->d[n2+i];
      k = d0->d[n2+i]; l = d1->d[i];

      /* degree of xi^a*(Di^k*xi^l)*Di^b */
      a += l;
      b += k;
      s = MUL_WEIGHT(a,i)+MUL_WEIGHT(b,n2+i);

      /* compute xi^a*(Di^k*xi^l)*Di^b */
      min = MIN(k,l);

      if ( min+1 > tablen ) {
        if ( tab ) GCFREE(tab);
        tab = (int *)MALLOC((min+1)*sizeof(int));
        tablen = min+1;
      }
      mkwcm(k,l,mod,tab);
      if ( n & 1 )
        for ( mr0 = 0, j = 0; j <= min; j++ ) {
          NEXTMP(mr0,mr); NEWDL(d,n);
          d->d[i] = a-j; d->d[n2+i] = b-j;
          d->td = s;
          d->d[n-1] = s-(MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i));
          STOMQ(tab[j],mq); mr->c = (Obj)mq; mr->dl = d;
        }
      else
        for ( mr0 = 0, s = 0, j = 0; j <= min; j++ ) {
          NEXTMP(mr0,mr); NEWDL(d,n);
          d->d[i] = a-j; d->d[n2+i] = b-j;
          d->td = MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i); /* XXX */
          s = MAX(s,d->td); /* XXX */
          STOMQ(tab[j],mq); mr->c = (Obj)mq; mr->dl = d;
        }
      bzero(tab,(min+1)*sizeof(int));
      if ( mr0 )
        NEXT(mr) = 0;
      MKDP(n,mr0,t);
      if ( t )
        t->sugar = s;
      comm_mulmd(vl,mod,r,t,&t1); r = t1;
    }
    mulmdc(vl,mod,r,c,pr);
  }
}

void mulmdc(VL vl,int mod,DP p,P c,DP *pr)
{
  MP m,mr,mr0;
  int t;
  MQ q;

  if ( !p || !c )
    *pr = 0;
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      NEXTMP(mr0,mr);
      if ( NUM(C(m)) && NUM(c) ) {
        t = dmar(((MQ)(C(m)))->cont,((MQ)c)->cont,0,mod);
        STOMQ(t,q); C(mr) = (Obj)q;
      } else
        mulmp(vl,mod,(P)C(m),c,(P *)&C(mr));
      mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = p->sugar;
  }
}

void _mulmdc(VL vl,int mod,DP *p,P c)
{
  MP m;
  int t;
  MQ q;
  P obj;

  if ( *p == 0|| c == 0 )
    *p = 0;
  else {
    for ( m = BDY(*p); m; m = NEXT(m) ) {
      if ( NUM(C(m)) && NUM(c) ) {
        t = dmar(((MQ)(C(m)))->cont,((MQ)c)->cont,0,mod);
        STOMQ(t,q); C(m) = (Obj)q;
      } else {
        mulmp(vl,mod,(P)C(m),c,&obj); C(m) = (Obj)obj;
      }
    }
  }
}

void divsmdc(VL vl,int mod,DP p,P c,DP *pr)
{
  MP m,mr,mr0;

  if ( !c )
    error("disvsdc : division by 0");
  else if ( !p )
    *pr = 0;
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      NEXTMP(mr0,mr); divsmp(vl,mod,(P)C(m),c,(P *)&C(mr)); mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = p->sugar;
  }
}

void _dtop_mod(VL vl,VL dvl,DP p,P *pr)
{
  int n,i;
  DL d;
  MP m;
  P r,s,t,u,w;
  Z q;
  VL tvl;

  if ( !p )
    *pr = 0;
  else {
    for ( n = p->nv, m = BDY(p), s = 0; m; m = NEXT(m) ) {
      i = ITOS(m->c); STOZ(i,q); t = (P)q;
      for ( i = 0, d = m->dl, tvl = dvl;
        i < n; tvl = NEXT(tvl), i++ ) {
        MKV(tvl->v,r); STOZ(d->d[i],q); pwrp(vl,r,q,&u);
        mulp(vl,t,u,&w); t = w;
      }
      addp(vl,s,t,&u); s = u;
    }
    *pr = s;
  }
}

void _dp_mod(DP p,int mod,NODE subst,DP *rp)
{
  MP m,mr,mr0;
  P t,s;
  NODE tn;

  if ( !p )
    *rp = 0;
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      for ( tn = subst, s = (P)m->c; tn; tn = NEXT(NEXT(tn)) ) {
        substp(CO,s,BDY(tn),BDY(NEXT(tn)),&t);
        s = t;
      }
      ptomp(mod,s,&t);
      if ( t ) {
        NEXTMP(mr0,mr); mr->c = (Obj)STOI(CONT((MQ)t)); mr->dl = m->dl;
      }
    }
    if ( mr0 ) {
      NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
    } else
      *rp = 0;
  }
}

void _dp_monic(DP p,int mod,DP *rp)
{
  MP m,mr,mr0;
  int c,c1;

  if ( !p )
    *rp = 0;
  else {
    c = invm(ITOS(BDY(p)->c),mod);
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      c1 = dmar(ITOS(m->c),c,0,mod);
      NEXTMP(mr0,mr); mr->c = (Obj)STOI(c1); mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
  }
}

void _printdp(DP d)
{
  int n,i;
  MP m;
  DL dl;

  if ( !d ) {
    printf("0"); return;
  }
  for ( n = d->nv, m = BDY(d); m; m = NEXT(m) ) {
    printf("%ld*<<",ITOS(m->c));
    for ( i = 0, dl = m->dl; i < n-1; i++ )
      printf("%d,",dl->d[i]);
    printf("%d",dl->d[i]);
    printf(">>");
    if ( NEXT(m) )
      printf("+");
  }
}

/* merge p1 and p2 into pr */

void addmd_destructive(int mod,DP p1,DP p2,DP *pr)
{
  int n;
  MP m1,m2,mr,mr0,s;
  int t;

  if ( !p1 )
    *pr = p2;
  else if ( !p2 )
    *pr = p1;
  else {
    for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; )
      switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
        case 0:
          t = (ITOS(C(m1))+ITOS(C(m2))) - mod;
          if ( t < 0 )
            t += mod;
          s = m1; m1 = NEXT(m1);
          if ( t ) {
            NEXTMP2(mr0,mr,s); C(mr) = (Obj)STOI(t);
          }
          s = m2; m2 = NEXT(m2);
          break;
        case 1:
          s = m1; m1 = NEXT(m1); NEXTMP2(mr0,mr,s);
          break;
        case -1:
          s = m2; m2 = NEXT(m2); NEXTMP2(mr0,mr,s);
          break;
      }
    if ( !mr0 )
      if ( m1 )
        mr0 = m1;
      else if ( m2 )
        mr0 = m2;
      else {
        *pr = 0;
        return;
      }
    else if ( m1 )
      NEXT(mr) = m1;
    else if ( m2 )
      NEXT(mr) = m2;
    else
      NEXT(mr) = 0;
    MKDP(NV(p1),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = MAX(p1->sugar,p2->sugar);
  }
}

void mulmd_dup(int mod,DP p1,DP p2,DP *pr)
{
  if ( !do_weyl )
    comm_mulmd_dup(mod,p1,p2,pr);
  else
    weyl_mulmd_dup(mod,p1,p2,pr);
}

void comm_mulmd_dup(int mod,DP p1,DP p2,DP *pr)
{
  MP m;
  DP s,t,u;
  int i,l,l1;
  static MP *w;
  static int wlen;

  if ( !p1 || !p2 )
    *pr = 0;
  else {
    for ( m = BDY(p1), l1 = 0; m; m = NEXT(m), l1++ );
    for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
    if ( l1 < l ) {
      t = p1; p1 = p2; p2 = t;
      l = l1;
    }
    if ( l > wlen ) {
      if ( w ) GCFREE(w);
      w = (MP *)MALLOC(l*sizeof(MP));
      wlen = l;
    }
    for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
      w[i] = m;
    for ( s = 0, i = l-1; i >= 0; i-- ) {
      mulmdm_dup(mod,p1,w[i],&t); addmd_destructive(mod,s,t,&u); s = u;
    }
    bzero(w,l*sizeof(MP));
    *pr = s;
  }
}

void weyl_mulmd_dup(int mod,DP p1,DP p2,DP *pr)
{
  MP m;
  DP s,t,u;
  int i,l;
  static MP *w;
  static int wlen;

  if ( !p1 || !p2 )
    *pr = 0;
  else {
    for ( m = BDY(p1), l = 0; m; m = NEXT(m), l++ );
    if ( l > wlen ) {
      if ( w ) GCFREE(w);
      w = (MP *)MALLOC(l*sizeof(MP));
      wlen = l;
    }
    for ( m = BDY(p1), i = 0; i < l; m = NEXT(m), i++ )
      w[i] = m;
    for ( s = 0, i = l-1; i >= 0; i-- ) {
      weyl_mulmdm_dup(mod,w[i],p2,&t); addmd_destructive(mod,s,t,&u); s = u;
    }
    bzero(w,l*sizeof(MP));
    *pr = s;
  }
}

void mulmdm_dup(int mod,DP p,MP m0,DP *pr)
{
  MP m,mr,mr0;
  DL d,dt,dm;
  int c,n,i;
  int *pt,*p1,*p2;

  if ( !p )
    *pr = 0;
  else {
    for ( mr0 = 0, m = BDY(p), c = ITOS(C(m0)), d = m0->dl, n = NV(p); 
      m; m = NEXT(m) ) {
      NEXTMP(mr0,mr);
      C(mr) = (Obj)STOI(dmar(ITOS(C(m)),c,0,mod));
      NEWDL_NOINIT(dt,n); mr->dl = dt;
      dm = m->dl;
      dt->td = d->td + dm->td;
      for ( i = 0, pt = dt->d, p1=d->d, p2 = dm->d; i < n; i++ )
        *pt++ = *p1++ + *p2++;
    }
    NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = p->sugar + m0->dl->td;
  }
}

void weyl_mulmdm_dup(int mod,MP m0,DP p,DP *pr)
{
  DP r,t,t1;
  MP m;
  DL d0;
  int n,n2,l,i,j,tlen;
  static MP *w,*psum;
  static struct cdlm *tab;
  static int wlen;
  static int rtlen;

  if ( !p )
    *pr = 0;
  else {
    for ( m = BDY(p), l = 0; m; m = NEXT(m), l++ );
    if ( l > wlen ) {
      if ( w ) GCFREE(w);
      w = (MP *)MALLOC(l*sizeof(MP));
      wlen = l;
    }
    for ( m = BDY(p), i = 0; i < l; m = NEXT(m), i++ )
      w[i] = m;
    n = NV(p); n2 = n>>1;
    d0 = m0->dl;

    for ( i = 0, tlen = 1; i < n2; i++ )
      tlen *= d0->d[n2+i]+1;
    if ( tlen > rtlen ) {
      if ( tab ) GCFREE(tab);
      if ( psum ) GCFREE(psum);
      rtlen = tlen;
      tab = (struct cdlm *)MALLOC(rtlen*sizeof(struct cdlm));
      psum = (MP *)MALLOC(rtlen*sizeof(MP));
    }
    bzero(psum,tlen*sizeof(MP));
    for ( i = l-1; i >= 0; i-- ) {
      bzero(tab,tlen*sizeof(struct cdlm));
      weyl_mulmmm_dup(mod,m0,w[i],n,tab,tlen);
      for ( j = 0; j < tlen; j++ ) {
        if ( tab[j].c ) {
          NEWMP(m); m->dl = tab[j].d;
          C(m) = (Obj)STOI(tab[j].c); NEXT(m) = psum[j];
          psum[j] = m;
        }
      }
    }
    for ( j = tlen-1, r = 0; j >= 0; j-- ) 
      if ( psum[j] ) {
        MKDP(n,psum[j],t); addmd_destructive(mod,r,t,&t1); r = t1;
      }
    if ( r )
      r->sugar = p->sugar + m0->dl->td;
    *pr = r;
  }
}

/* m0 = x0^d0*x1^d1*... * dx0^d(n/2)*dx1^d(n/2+1)*... */

void weyl_mulmmm_dup(int mod,MP m0,MP m1,int n,struct cdlm *rtab,int rtablen)
{
  int c,c0,c1;
  DL d,d0,d1,dt;
  int i,j,a,b,k,l,n2,s,min,curlen;
  struct cdlm *p;
  static int *ctab;
  static struct cdlm *tab;
  static int tablen;
  static struct cdlm *tmptab;
  static int tmptablen;

  if ( !m0 || !m1 ) {
    rtab[0].c = 0;
    rtab[0].d = 0;
    return;
  }
  c0 = ITOS(C(m0)); c1 = ITOS(C(m1));
  c = dmar(c0,c1,0,mod);
  d0 = m0->dl; d1 = m1->dl;
  n2 = n>>1;
  curlen = 1;

  NEWDL(d,n);
  if ( n & 1 )
    /* offset of h-degree */
    d->td = d->d[n-1] = d0->d[n-1]+d1->d[n-1];
  else
    d->td = 0;
  rtab[0].c = c;
  rtab[0].d = d;

  if ( rtablen > tmptablen ) {
    if ( tmptab ) GCFREE(tmptab);
    tmptab = (struct cdlm *)MALLOC(rtablen*sizeof(struct cdlm));
    tmptablen = rtablen; 
  }

  for ( i = 0; i < n2; i++ ) {
    a = d0->d[i]; b = d1->d[n2+i];
    k = d0->d[n2+i]; l = d1->d[i];

    /* degree of xi^a*(Di^k*xi^l)*Di^b */
    a += l;
    b += k;
    s = MUL_WEIGHT(a,i)+MUL_WEIGHT(b,n2+i);

    if ( !k || !l ) {
      for ( j = 0, p = rtab; j < curlen; j++, p++ ) {
        if ( p->c ) {
          dt = p->d;
          dt->d[i] = a;
          dt->d[n2+i] = b;
          dt->td += s;
        }
      }
      curlen *= k+1;
      continue;
    }
    if ( k+1 > tablen ) {
      if ( tab ) GCFREE(tab);
      if ( ctab ) GCFREE(ctab);
      tablen = k+1;
      tab = (struct cdlm *)MALLOC(tablen*sizeof(struct cdlm));
      ctab = (int *)MALLOC(tablen*sizeof(int));
    }
    /* compute xi^a*(Di^k*xi^l)*Di^b */
    min = MIN(k,l);
    mkwcm(k,l,mod,ctab);
    bzero(tab,(k+1)*sizeof(struct cdlm));
    /* n&1 != 0 => homogenized computation; dx-xd=h^2 */
    if ( n & 1 )
      for ( j = 0; j <= min; j++ ) {
        NEWDL(d,n);
        d->d[i] = a-j; d->d[n2+i] = b-j;
        d->td = s;
        d->d[n-1] = s-(MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i));
        tab[j].d = d;
        tab[j].c = ctab[j];
      }
    else
      for ( j = 0; j <= min; j++ ) {
        NEWDL(d,n);
        d->d[i] = a-j; d->d[n2+i] = b-j;
        d->td = MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i); /* XXX */
        tab[j].d = d;
        tab[j].c = ctab[j];
      }
    comm_mulmd_tab_destructive(mod,n,rtab,curlen,tab,k+1);
    curlen *= k+1;
  }
}

void comm_mulmd_tab_destructive(int mod,int nv,struct cdlm *t,int n,struct cdlm *t1,int n1)
{
  int i,j;
  struct cdlm *p;
  int c;
  DL d;

  for ( j = 1, p = t+n; j < n1; j++ ) {
    c = t1[j].c;
    d = t1[j].d;
    if ( !c )
      break;
    for ( i = 0; i < n; i++, p++ ) {
      if ( t[i].c ) {
        p->c = dmar(t[i].c,c,0,mod);
        adddl_dup(nv,t[i].d,d,&p->d);
      }
    }
  }
  c = t1[0].c;
  d = t1[0].d;
  for ( i = 0, p = t; i < n; i++, p++ )
    if ( t[i].c ) {
      p->c = dmar(t[i].c,c,0,mod);
      /* t[i].d += d */
      adddl_destructive(nv,t[i].d,d);
    }
}

void adddl_dup(int n,DL d1,DL d2,DL *dr)
{
  DL dt;
  int i;

  NEWDL(dt,n);
  *dr = dt;
  dt->td = d1->td + d2->td;
  for ( i = 0; i < n; i++ )
    dt->d[i] = d1->d[i]+d2->d[i];
}
