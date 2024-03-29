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
 * $OpenXM$
*/
#include "ca.h"
#include "inline.h"

extern int (*cmpdl)(int,DL,DL);
extern int do_weyl;

MP _mp_free_list;
DP _dp_free_list;
DL _dl_free_list;
int current_dl_length;

void GC_gcollect();

void _free_private_storage()
{
  _mp_free_list = 0;
  _dp_free_list = 0;
  _dl_free_list = 0;
  GC_gcollect();
}

void _DL_alloc()
{
  int *p;
  int i,dl_len;
  static int DL_alloc_count;

/*  fprintf(stderr,"DL_alloc : %d \n",++DL_alloc_count); */
  dl_len = (current_dl_length+1);
#if SIZEOF_LONG == 8
  if ( dl_len & 1 )
    dl_len += 1;
#endif
  for ( i = 0; i < 128; i++, p += dl_len ) {
    p = (int *)MALLOC(dl_len*sizeof(int));
    *(DL *)p = _dl_free_list;
    _dl_free_list = (DL)p;
  }
}

void _MP_alloc()
{
  MP p;
  int i;
  static int MP_alloc_count;

/*  fprintf(stderr,"MP_alloc : %d \n",++MP_alloc_count); */
  for ( i = 0; i < 1024; i++ ) {
    p = (MP)MALLOC(sizeof(struct oMP));
    p->next = _mp_free_list; _mp_free_list = p;
  }
}
    
void _DP_alloc()
{
  DP p;
  int i;
  static int DP_alloc_count;

/*  fprintf(stderr,"DP_alloc : %d \n",++DP_alloc_count); */
  for ( i = 0; i < 1024; i++ ) {
    p = (DP)MALLOC(sizeof(struct oDP));
    p->body = (MP)_dp_free_list; _dp_free_list = p;
  }
}

/* merge p1 and p2 into pr */

void _addmd_destructive(int mod,DP p1,DP p2,DP *pr)
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
            _NEXTMP2(mr0,mr,s); C(mr) = (Obj)STOI(t);
          } else {
            _FREEDL(s->dl); _FREEMP(s);
          }
          s = m2; m2 = NEXT(m2); _FREEDL(s->dl); _FREEMP(s);
          break;
        case 1:
          s = m1; m1 = NEXT(m1); _NEXTMP2(mr0,mr,s);
          break;
        case -1:
          s = m2; m2 = NEXT(m2); _NEXTMP2(mr0,mr,s);
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
    _MKDP(NV(p1),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = MAX(p1->sugar,p2->sugar);
    _FREEDP(p1); _FREEDP(p2);
  }
}

void _mulmd_dup(int mod,DP p1,DP p2,DP *pr)
{
  if ( !do_weyl )
    _comm_mulmd_dup(mod,p1,p2,pr);
  else
    _weyl_mulmd_dup(mod,p1,p2,pr);
}

void _comm_mulmd_dup(int mod,DP p1,DP p2,DP *pr)
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
      _mulmdm_dup(mod,p1,w[i],&t); _addmd_destructive(mod,s,t,&u); s = u;
    }
    bzero(w,l*sizeof(MP));
    *pr = s;
  }
}

void _weyl_mulmd_dup(int mod,DP p1,DP p2,DP *pr)
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
      _weyl_mulmdm_dup(mod,w[i],p2,&t); _addmd_destructive(mod,s,t,&u); s = u;
    }
    bzero(w,l*sizeof(MP));
    *pr = s;
  }
}

void _mulmdm_dup(int mod,DP p,MP m0,DP *pr)
{
  MP m,mr,mr0;
  DL d,dt,dm;
  int c,n,i,c1,c2;
  int *pt,*p1,*p2;

  if ( !p )
    *pr = 0;
  else {
    for ( mr0 = 0, m = BDY(p), c = ITOS(C(m0)), d = m0->dl, n = NV(p); 
      m; m = NEXT(m) ) {
      _NEXTMP(mr0,mr);
      c1 = ITOS(C(m));
      DMAR(c1,c,0,mod,c2);
      C(mr) = (Obj)STOI(c2);
      _NEWDL_NOINIT(dt,n); mr->dl = dt;
      dm = m->dl;
      dt->td = d->td + dm->td;
      for ( i = 0, pt = dt->d, p1=d->d, p2 = dm->d; i < n; i++ )
        *pt++ = *p1++ + *p2++;
    }
    NEXT(mr) = 0; _MKDP(NV(p),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = p->sugar + m0->dl->td;
  }
}

void _weyl_mulmdm_dup(int mod,MP m0,DP p,DP *pr)
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
      _weyl_mulmmm_dup(mod,m0,w[i],n,tab,tlen);
      for ( j = 0; j < tlen; j++ ) {
        if ( tab[j].c ) {
          _NEWMP(m); m->dl = tab[j].d;
          C(m) = (Obj)STOI(tab[j].c); NEXT(m) = psum[j];
          psum[j] = m;
        }
      }
    }
    for ( j = tlen-1, r = 0; j >= 0; j-- ) 
      if ( psum[j] ) {
        _MKDP(n,psum[j],t); _addmd_destructive(mod,r,t,&t1); r = t1;
      }
    if ( r )
      r->sugar = p->sugar + m0->dl->td;
    *pr = r;
  }
}

/* m0 = x0^d0*x1^d1*... * dx0^d(n/2)*dx1^d(n/2+1)*... */

void _weyl_mulmmm_dup(int mod,MP m0,MP m1,int n,struct cdlm *rtab,int rtablen)
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

  _NEWDL(d,n);
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
        _NEWDL(d,n);
        d->d[i] = a-j; d->d[n2+i] = b-j;
        d->td = s;
        d->d[n-1] = s-(MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i)); 
        tab[j].d = d;
        tab[j].c = ctab[j];
      }
    else
      for ( j = 0; j <= min; j++ ) {
        _NEWDL(d,n);
        d->d[i] = a-j; d->d[n2+i] = b-j;
        d->td = MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i);  /* XXX */
        tab[j].d = d;
        tab[j].c = ctab[j];
      }
#if 0
    _comm_mulmd_tab(mod,n,rtab,curlen,tab,k+1,tmptab);
    for ( j = 0; j < curlen; j++ )
      if ( rtab[j].d ) { _FREEDL(rtab[j].d); }
    for ( j = 0; j <= min; j++ )
      if ( tab[j].d ) { _FREEDL(tab[j].d); }
    curlen *= k+1;
    bcopy(tmptab,rtab,curlen*sizeof(struct cdlm));
#else
    _comm_mulmd_tab_destructive(mod,n,rtab,curlen,tab,k+1);
    for ( j = 0; j <= min; j++ )
      if ( tab[j].d ) { _FREEDL(tab[j].d); }
    curlen *= k+1;
#endif
  }
}

/* direct product of two cdlm tables
  rt[] = [
    t[0]*t1[0],...,t[n-1]*t1[0],
    t[0]*t1[1],...,t[n-1]*t1[1],
    ...
    t[0]*t1[n1-1],...,t[n-1]*t1[n1-1]
  ]
*/

void _comm_mulmd_tab(int mod,int nv,struct cdlm *t,int n,struct cdlm *t1,int n1,struct cdlm *rt)
{
  int i,j;
  struct cdlm *p;
  int c;
  DL d;

  bzero(rt,n*n1*sizeof(struct cdlm));
  for ( j = 0, p = rt; j < n1; j++ ) {
    c = t1[j].c;
    d = t1[j].d;
    if ( !c )
      break;
    for ( i = 0; i < n; i++, p++ ) {
      if ( t[i].c ) {
        p->c = dmar(t[i].c,c,0,mod);
        _adddl_dup(nv,t[i].d,d,&p->d);
      }
    }
  }
}

void _comm_mulmd_tab_destructive(int mod,int nv,struct cdlm *t,int n,struct cdlm *t1,int n1)
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
        _adddl_dup(nv,t[i].d,d,&p->d);
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

void dlto_dl(DL d,DL *dr)
{
  int i,n;
  DL t;

  n = current_dl_length;
  _NEWDL(t,n); *dr = t;
  t->td = d->td;
  for ( i = 0; i < n; i++ )
    t->d[i] = d->d[i];  
}

void _dltodl(DL d,DL *dr)
{
  int i,n;
  DL t;

  n = current_dl_length;
  NEWDL(t,n); *dr = t;
  t->td = d->td;
  for ( i = 0; i < n; i++ )
    t->d[i] = d->d[i];  
}

void _adddl_dup(int n,DL d1,DL d2,DL *dr)
{
  DL dt;
  int i;

  _NEWDL(dt,n);
  *dr = dt;
  dt->td = d1->td + d2->td;
  for ( i = 0; i < n; i++ )
    dt->d[i] = d1->d[i]+d2->d[i];
}

void _free_dlarray(DL *a,int n)
{
  int i;

  for ( i = 0; i < n; i++ ) { _FREEDL(a[i]); }
}

void _free_dp(DP f)
{
  MP m,s;

  if ( !f )
    return;
  m = f->body;
  while ( m ) {
    s = m; m = NEXT(m); _FREEDL(s->dl); _FREEMP(s);
  }
  _FREEDP(f);
}

void dpto_dp(DP p,DP *r)
{
  MP m,mr0,mr;
  DL t;

  if ( !p )
    *r = 0;
  else {
    /* XXX : dummy call to set current_dl_length */
    _NEWDL_NOINIT(t,NV(p));

    for ( m = BDY(p), mr0 = 0; m; m = NEXT(m) ) {
      _NEXTMP(mr0,mr);
      dlto_dl(m->dl,&mr->dl);
      mr->c = m->c;
    }
    NEXT(mr) = 0;
    _MKDP(p->nv,mr0,*r);
    (*r)->sugar = p->sugar;
  }
}

void _dptodp(DP p,DP *r)
{
  MP m,mr0,mr;

  if ( !p )
    *r = 0;
  else {
    for ( m = BDY(p), mr0 = 0; m; m = NEXT(m) ) {
      NEXTMP(mr0,mr);
      _dltodl(m->dl,&mr->dl);
      mr->c = m->c;
    }
    NEXT(mr) = 0;
    MKDP(p->nv,mr0,*r);
    (*r)->sugar = p->sugar;
  }
}

/* 
 * destructive merge of two list
 *
 * p1, p2 : list of DL
 * return : a merged list
 */

NODE _symb_merge(NODE m1,NODE m2,int n)
{
  NODE top,prev,cur,m,t;

  if ( !m1 )
    return m2;
  else if ( !m2 )
    return m1;
  else {
    switch ( (*cmpdl)(n,(DL)BDY(m1),(DL)BDY(m2)) ) {
      case 0:
        top = m1; _FREEDL((DL)BDY(m2)); m = NEXT(m2);
        break;
      case 1:
        top = m1; m = m2;
        break;
      case -1:
        top = m2; m = m1;
        break;
    }
    prev = top; cur = NEXT(top);
    /* BDY(prev) > BDY(m) always holds */
    while ( cur && m ) {
      switch ( (*cmpdl)(n,(DL)BDY(cur),(DL)BDY(m)) ) {
        case 0:
          _FREEDL(BDY(m)); m = NEXT(m);
          prev = cur; cur = NEXT(cur);
          break;
        case 1:
          t = NEXT(cur); NEXT(cur) = m; m = t;
          prev = cur; cur = NEXT(cur);
          break;
        case -1:
          NEXT(prev) = m; m = cur;
          prev = NEXT(prev); cur = NEXT(prev);
          break;
      }
    }
    if ( !cur )
      NEXT(prev) = m;
    return top;
  }
}

/* merge p1 and p2 into pr */

void _addd_destructive(VL vl,DP p1,DP p2,DP *pr)
{
  int n;
  MP m1,m2,mr,mr0,s;
  P t;

  if ( !p1 )
    *pr = p2;
  else if ( !p2 )
    *pr = p1;
  else {
    for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; )
      switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
        case 0:
          addp(vl,(P)C(m1),(P)C(m2),&t);
          s = m1; m1 = NEXT(m1);
          if ( t ) {
            _NEXTMP2(mr0,mr,s); C(mr) = (Obj)t;
          } else {
            _FREEDL(s->dl); _FREEMP(s);
          }
          s = m2; m2 = NEXT(m2); _FREEDL(s->dl); _FREEMP(s);
          break;
        case 1:
          s = m1; m1 = NEXT(m1); _NEXTMP2(mr0,mr,s);
          break;
        case -1:
          s = m2; m2 = NEXT(m2); _NEXTMP2(mr0,mr,s);
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
    _MKDP(NV(p1),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = MAX(p1->sugar,p2->sugar);
    _FREEDP(p1); _FREEDP(p2);
  }
}

void _muld_dup(VL vl,DP p1,DP p2,DP *pr)
{
  if ( !do_weyl )
    _comm_muld_dup(vl,p1,p2,pr);
  else
    _weyl_muld_dup(vl,p1,p2,pr);
}

void _comm_muld_dup(VL vl,DP p1,DP p2,DP *pr)
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
      _muldm_dup(vl,p1,w[i],&t); _addd_destructive(vl,s,t,&u); s = u;
    }
    bzero(w,l*sizeof(MP));
    *pr = s;
  }
}

void _weyl_muld_dup(VL vl,DP p1,DP p2,DP *pr)
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
      _weyl_muldm_dup(vl,w[i],p2,&t); _addd_destructive(vl,s,t,&u); s = u;
    }
    bzero(w,l*sizeof(MP));
    *pr = s;
  }
}

void _muldm_dup(VL vl,DP p,MP m0,DP *pr)
{
  MP m,mr,mr0;
  DL d,dt,dm;
  P c;
  int n,i;
  int *pt,*p1,*p2;

  if ( !p )
    *pr = 0;
  else {
    for ( mr0 = 0, m = BDY(p), c = (P)C(m0), d = m0->dl, n = NV(p); 
      m; m = NEXT(m) ) {
      _NEXTMP(mr0,mr);
      mulp(vl,(P)C(m),c,(P *)&C(mr));
      _NEWDL_NOINIT(dt,n); mr->dl = dt;
      dm = m->dl;
      dt->td = d->td + dm->td;
      for ( i = 0, pt = dt->d, p1=d->d, p2 = dm->d; i < n; i++ )
        *pt++ = *p1++ + *p2++;
    }
    NEXT(mr) = 0; _MKDP(NV(p),mr0,*pr);
    if ( *pr )
      (*pr)->sugar = p->sugar + m0->dl->td;
  }
}

void _weyl_muldm_dup(VL vl,MP m0,DP p,DP *pr)
{
  DP r,t,t1;
  MP m;
  DL d0;
  int n,n2,l,i,j,tlen;
  static MP *w,*psum;
  static struct cdl *tab;
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
      tab = (struct cdl *)MALLOC(rtlen*sizeof(struct cdl));
      psum = (MP *)MALLOC(rtlen*sizeof(MP));
    }
    bzero(psum,tlen*sizeof(MP));
    for ( i = l-1; i >= 0; i-- ) {
      bzero(tab,tlen*sizeof(struct cdl));
      _weyl_mulmm_dup(vl,m0,w[i],n,tab,tlen);
      for ( j = 0; j < tlen; j++ ) {
        if ( tab[j].c ) {
          _NEWMP(m); m->dl = tab[j].d;
          C(m) = tab[j].c; NEXT(m) = psum[j];
          psum[j] = m;
        }
      }
    }
    for ( j = tlen-1, r = 0; j >= 0; j-- ) 
      if ( psum[j] ) {
        _MKDP(n,psum[j],t); _addd_destructive(vl,r,t,&t1); r = t1;
      }
    if ( r )
      r->sugar = p->sugar + m0->dl->td;
    *pr = r;
  }
}

/* m0 = x0^d0*x1^d1*... * dx0^d(n/2)*dx1^d(n/2+1)*... */

void _weyl_mulmm_dup(VL vl,MP m0,MP m1,int n,struct cdl *rtab,int rtablen)
{
  P c;
  DL d,d0,d1,dt;
  int i,j,a,b,k,l,n2,s,min,curlen;
  struct cdl *p;
  static Z *ctab;
  static struct cdl *tab;
  static int tablen;
  static struct cdl *tmptab;
  static int tmptablen;

  if ( !m0 || !m1 ) {
    rtab[0].c = 0;
    rtab[0].d = 0;
    return;
  }
  mulp(vl,(P)C(m0),(P)C(m1),&c);
  d0 = m0->dl; d1 = m1->dl;
  n2 = n>>1;
  curlen = 1;

  _NEWDL(d,n);
  if ( n & 1 )
    /* offset of h-degree */
    d->td = d->d[n-1] = d0->d[n-1]+d1->d[n-1];
  else
    d->td = 0;
  rtab[0].c = (Obj)c;
  rtab[0].d = d;

  if ( rtablen > tmptablen ) {
    if ( tmptab ) GCFREE(tmptab);
    tmptab = (struct cdl *)MALLOC(rtablen*sizeof(struct cdl));
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
      tab = (struct cdl *)MALLOC(tablen*sizeof(struct cdl));
      ctab = (Z *)MALLOC(tablen*sizeof(P));
    }
    /* compute xi^a*(Di^k*xi^l)*Di^b */
    min = MIN(k,l);
    mkwc(k,l,ctab);
    bzero(tab,(k+1)*sizeof(struct cdl));
    /* n&1 != 0 => homogenized computation; dx-xd=h^2 */
    if ( n & 1 )
      for ( j = 0; j <= min; j++ ) {
        _NEWDL(d,n);
        d->d[i] = a-j; d->d[n2+i] = b-j;
        d->td = s;
        d->d[n-1] = s-(MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i));
        tab[j].d = d;
        tab[j].c = (Obj)ctab[j];
      }
    else
      for ( j = 0; j <= min; j++ ) {
        _NEWDL(d,n);
        d->d[i] = a-j; d->d[n2+i] = b-j;
        d->td = MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i); /* XXX */
        tab[j].d = d;
        tab[j].c = (Obj)ctab[j];
      }
#if 0
    _comm_muld_tab(vl,n,rtab,curlen,tab,k+1,tmptab);
    for ( j = 0; j < curlen; j++ )
      if ( rtab[j].d ) { _FREEDL(rtab[j].d); }
    for ( j = 0; j <= min; j++ )
      if ( tab[j].d ) { _FREEDL(tab[j].d); }
    curlen *= k+1;
    bcopy(tmptab,rtab,curlen*sizeof(struct cdl));
#else
    _comm_muld_tab_destructive(vl,n,rtab,curlen,tab,k+1);
    for ( j = 0; j <= min; j++ )
      if ( tab[j].d ) { _FREEDL(tab[j].d); }
    curlen *= k+1;
#endif
  }
}

/* direct product of two cdl tables
  rt[] = [
    t[0]*t1[0],...,t[n-1]*t1[0],
    t[0]*t1[1],...,t[n-1]*t1[1],
    ...
    t[0]*t1[n1-1],...,t[n-1]*t1[n1-1]
  ]
*/

void _comm_muld_tab(VL vl,int nv,struct cdl *t,int n,struct cdl *t1,int n1,struct cdl *rt)
{
  int i,j;
  struct cdl *p;
  P c;
  DL d;

  bzero(rt,n*n1*sizeof(struct cdl));
  for ( j = 0, p = rt; j < n1; j++ ) {
    c = (P)t1[j].c;
    d = t1[j].d;
    if ( !c )
      break;
    for ( i = 0; i < n; i++, p++ ) {
      if ( t[i].c ) {
        mulp(vl,(P)t[i].c,c,(P *)&p->c);
        _adddl_dup(nv,t[i].d,d,&p->d);
      }
    }
  }
}

void _comm_muld_tab_destructive(VL vl,int nv,struct cdl *t,int n,struct cdl *t1,int n1)
{
  int i,j;
  struct cdl *p;
  P c;
  DL d;

  for ( j = 1, p = t+n; j < n1; j++ ) {
    c = (P)t1[j].c;
    d = t1[j].d;
    if ( !c )
      break;
    for ( i = 0; i < n; i++, p++ ) {
      if ( t[i].c ) {
        mulp(vl,(P)t[i].c,c,(P *)&p->c);
        _adddl_dup(nv,t[i].d,d,&p->d);
      }
    }
  }
  c = (P)t1[0].c;
  d = t1[0].d;
  for ( i = 0, p = t; i < n; i++, p++ )
    if ( t[i].c ) {
      mulp(vl,(P)t[i].c,c,(P *)&p->c);
      /* t[i].d += d */
      adddl_destructive(nv,t[i].d,d);
    }
}
