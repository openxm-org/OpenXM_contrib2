/*
 * $OpenXM$
*/

#include "ca.h"
#include "base.h"

static NumberField current_numberfield;
extern struct order_spec *dp_current_spec;
void simpdalg(DAlg da,DAlg *r);
int invdalg(DAlg a,DAlg *c);
void rmcontdalg(DAlg a, DAlg *c);
void algtodalg(Alg a,DAlg *r);
void dalgtoalg(DAlg da,Alg *r);

NumberField get_numberfield()
{
  return current_numberfield;
}

void setfield_dalg(NODE alist)
{
  NumberField nf;
  VL vl,vl1,vl2;
  int n,i,dim;
  Alg *gen;
  P *defpoly;
  P p;
  Z iq,two;
  Q c;
  DP *ps,*mb;
  DP one;
  NODE t,b,b1,b2,hlist,mblist;
  struct order_spec *current_spec;

  nf = (NumberField)MALLOC(sizeof(struct oNumberField));
  current_numberfield = nf;
  vl = 0;
  for ( t = alist; t; t = NEXT(t) ) {
    clctalg((P)BDY((Alg)BDY(t)),&vl1);
    mergev(ALG,vl,vl1,&vl2); vl = vl2;
  }
  for ( n = 0, vl1 = vl; vl1; vl1 = NEXT(vl1), n++ ); 
  nf->n = n;
  nf->vl = vl;
  nf->defpoly = defpoly = (P *)MALLOC(n*sizeof(P));
  nf->ps = ps = (DP *)MALLOC(n*sizeof(DP));
  current_spec = dp_current_spec;
  STOQ(2,two);
  create_order_spec(0,(Obj)two,&nf->spec);
  initd(nf->spec);  
  for ( b = hlist = 0, i = 0, vl1 = vl; i < n; vl1 = NEXT(vl1), i++ ) {
    ptozp(vl1->v->attr,1,&c,&defpoly[i]);
    ptod(ALG,vl,defpoly[i],&ps[i]);
    STOQ(i,iq); MKNODE(b1,(pointer)iq,b); b = b1;
    MKNODE(b2,(pointer)ps[i],hlist); hlist = b2;
  }
  ptod(ALG,vl,(P)ONE,&one);
  MKDAlg(one,ONE,nf->one);
  nf->ind = b;
  dp_mbase(hlist,&mblist);
  initd(current_spec);  
  nf->dim = dim = length(mblist);
  nf->mb = mb = (DP *)MALLOC(dim*sizeof(DP));
  for ( i = 0, t = mblist; t; t = NEXT(t), i++ )
    mb[dim-i-1] = (DP)BDY(t);  
}

void setfield_gb(NODE gb,VL vl,struct order_spec *spec)
{
  NumberField nf;
  VL vl1,vl2;
  int n,i,dim;
  Alg *gen;
  P *defpoly;
  P p;
  Q c;
  Z iq,two;
  DP *ps,*mb;
  DP one;
  NODE t,b,b1,b2,hlist,mblist;
  struct order_spec *current_spec;

  nf = (NumberField)MALLOC(sizeof(struct oNumberField));
  current_numberfield = nf;
  for ( vl1 = vl, n = 0; vl1; vl1 = NEXT(vl1), n++ );
  nf->n = n;
  nf->psn = length(gb);
  nf->vl = vl;
  nf->defpoly = defpoly = (P *)MALLOC(nf->psn*sizeof(P));
  nf->ps = ps = (DP *)MALLOC(nf->psn*sizeof(DP));
  current_spec = dp_current_spec;
  nf->spec = spec;
  initd(nf->spec);  
  for ( b = hlist = 0, i = 0, t = gb; i < nf->psn; t = NEXT(t), i++ ) {
    ptozp((P)BDY(t),1,&c,&defpoly[i]);
    ptod(CO,vl,defpoly[i],&ps[i]);
    STOQ(i,iq); MKNODE(b1,(pointer)iq,b); b = b1;
    MKNODE(b2,(pointer)ps[i],hlist); hlist = b2;
  }
  ptod(ALG,vl,(P)ONE,&one);
  MKDAlg(one,ONE,nf->one);
  nf->ind = b;
  dp_mbase(hlist,&mblist);
  initd(current_spec);
  nf->dim = dim = length(mblist);
  nf->mb = mb = (DP *)MALLOC(dim*sizeof(DP));
  for ( i = 0, t = mblist; t; t = NEXT(t), i++ )
    mb[dim-i-1] = (DP)BDY(t);  
}

void qtodalg(Q q,DAlg *r)
{
  NumberField nf;
  Z t;
  DP nm;

  if ( !(nf=current_numberfield) )
    error("qtodalg : current_numberfield is not set");
  if ( !q )
    *r = 0;
  else if ( NID(q) == N_DA )
    *r = (DAlg)q;
  else if ( NID(q) == N_Q ) {
    if ( INT(q) ) {
      muldc(CO,nf->one->nm,(Obj)q,&nm);
      MKDAlg(nm,ONE,*r);
    } else {
      MPZTOZ(mpq_numref(BDY((Q)q)),t);
      muldc(CO,nf->one->nm,(Obj)t,&nm);
      MPZTOZ(mpq_denref(BDY((Q)q)),t);
      MKDAlg(nm,t,*r);
    }
  } else
    error("qtodalg : invalid argument");
}

void obj_algtodalg(Obj obj,Obj *r)
{
  DAlg d;
  DCP dc,dcr0,dcr;
  P c,p;
  Obj t;
  Obj nm,dn;
  NODE b,s,s0;
  R rat;
  VECT v;
  MAT mat;
  LIST list;
  pointer *a;
  pointer **m;
  int len,row,col,i,j,l;

  if ( !obj ) {
    *r = 0;
    return;
  }
  switch ( OID(obj) ) {
    case O_N:
      algtodalg((Alg)obj,&d); *r = (Obj)d;
      break;
    case O_P:
      for ( dcr0 = 0, dc = DC((P)obj); dc; dc = NEXT(dc) ) {
        obj_algtodalg((Obj)COEF(dc),&t);
        if ( t ) {
          NEXTDC(dcr0,dcr);
          COEF(dcr) = (P)t;
          DEG(dcr) = DEG(dc);
        }
      }
      if ( dcr0 ) {
        MKP(VR((P)obj),dcr0,p);
        *r = (Obj)p;
      } else
        *r = 0;
      break;
    case O_R:
      obj_algtodalg((Obj)NM((R)obj),&nm);
      obj_algtodalg((Obj)DN((R)obj),&dn);
      if ( !dn )
        error("obj_algtodalg : division by 0");
      if ( !nm )
        *r = 0;
      else {
        MKRAT((P)nm,(P)dn,0,rat); *r = (Obj)rat;
      }
      break;
    case O_LIST:
      s0 = 0;
      for ( b = BDY((LIST)obj); b; b = NEXT(b) ) {
        NEXTNODE(s0,s);
        obj_algtodalg((Obj)BDY(b),&t);
        BDY(s) = (pointer)t;
      }
      NEXT(s) = 0;
      MKLIST(list,s0);
      *r = (Obj)list;
      break;
    case O_VECT:
      l = ((VECT)obj)->len;
      a = BDY((VECT)obj);
      MKVECT(v,l);
      for ( i = 0; i < l; i++ ) {
        obj_algtodalg((Obj)a[i],&t);
        BDY(v)[i] = (pointer)t;
      }
      *r = (Obj)v;
      break;
    case O_MAT:
      row = ((MAT)obj)->row; col = ((MAT)obj)->col;
      m = BDY((MAT)obj);
      MKMAT(mat,row,col);
      for ( i = 0; i < row; i++ )
        for ( j = 0; j < col; j++ ) {
          obj_algtodalg((Obj)m[i][j],&t);
          BDY(mat)[i][j] = (pointer)t;
        }
      *r = (Obj)mat;
      break;
    default:
      *r = obj;
      break;
  }
}

void obj_dalgtoalg(Obj obj,Obj *r)
{
  Alg d;
  DCP dc,dcr0,dcr;
  P c,p;
  Obj t;
  Obj nm,dn;
  NODE b,s,s0;
  R rat;
  VECT v;
  MAT mat;
  LIST list;
  pointer *a;
  pointer **m;
  int len,row,col,i,j,l;

  if ( !obj ) {
    *r = 0;
    return;
  }
  switch ( OID(obj) ) {
    case O_N:
      dalgtoalg((DAlg)obj,&d); *r = (Obj)d;
      break;
    case O_P:
      for ( dcr0 = 0, dc = DC((P)obj); dc; dc = NEXT(dc) ) {
        obj_dalgtoalg((Obj)COEF(dc),&t);
        if ( t ) {
          NEXTDC(dcr0,dcr);
          COEF(dcr) = (P)t;
          DEG(dcr) = DEG(dc);
        }
      }
      if ( dcr0 ) {
        MKP(VR((P)obj),dcr0,p);
        *r = (Obj)p;
      } else
        *r = 0;
      break;
    case O_R:
      obj_dalgtoalg((Obj)NM((R)obj),&nm);
      obj_dalgtoalg((Obj)DN((R)obj),&dn);
      if ( !dn )
        error("obj_dalgtoalg : division by 0");
      if ( !nm )
        *r = 0;
      else {
        MKRAT((P)nm,(P)dn,0,rat); *r = (Obj)rat;
      }
      break;
    case O_LIST:
      s0 = 0;
      for ( b = BDY((LIST)obj); b; b = NEXT(b) ) {
        NEXTNODE(s0,s);
        obj_dalgtoalg((Obj)BDY(b),&t);
        BDY(s) = (pointer)t;
      }
      NEXT(s) = 0;
      MKLIST(list,s0);
      *r = (Obj)list;
      break;
    case O_VECT:
      l = ((VECT)obj)->len;
      a = BDY((VECT)obj);
      MKVECT(v,l);
      for ( i = 0; i < l; i++ ) {
        obj_dalgtoalg((Obj)a[i],&t);
        BDY(v)[i] = (pointer)t;
      }
      *r = (Obj)v;
      break;
    case O_MAT:
      row = ((MAT)obj)->row; col = ((MAT)obj)->col;
      m = BDY((MAT)obj);
      MKMAT(mat,row,col);
      for ( i = 0; i < row; i++ )
        for ( j = 0; j < col; j++ ) {
          obj_dalgtoalg((Obj)m[i][j],&t);
          BDY(mat)[i][j] = (pointer)t;
        }
      *r = (Obj)mat;
      break;
    default:
      *r = obj;
      break;
  }
}

void algtodalg(Alg a,DAlg *r)
{
  P ap,p,p1;
  Q c;
  Z c1,d1,nm,dn;
  DP dp;
  DAlg da;
  NumberField nf;
  struct order_spec *current_spec;
  VL vl,tvl,svl;
  V v;

  if ( !(nf=current_numberfield) )
    error("algtodalg : current_numberfield is not set");
  if ( !a ) {
    *r = 0;
    return;
  }
  switch (NID((Num)a) ) {
    case N_Q:
      c = (Q)a;
      if ( INT(c) ) {
        muldc(CO,nf->one->nm,(Obj)c,&dp);
        MKDAlg(dp,ONE,*r);
      } else {
        MPZTOZ(mpq_numref(BDY((Q)c)),c1);
        MPZTOZ(mpq_denref(BDY((Q)c)),d1);
        muldc(CO,nf->one->nm,(Obj)c1,&dp);
        MKDAlg(dp,d1,*r);
      }
      break;
    case N_A:
      ap = (P)BDY(a);
      ptozp(ap,1,&c,&p);
      if ( INT(c) ) {
        p = ap;
        dn = ONE;
      } else {
        MPZTOZ(mpq_numref(BDY((Q)c)),nm);
        MPZTOZ(mpq_denref(BDY((Q)c)),dn);
        mulpq(p,(P)nm,&p1); p = p1;
      }
      current_spec = dp_current_spec; initd(nf->spec);
      get_vars((Obj)p,&vl);
      for ( tvl = vl; tvl; tvl = NEXT(tvl) ) {
        v = tvl->v;
        for ( svl = nf->vl; svl; svl = NEXT(svl) )
          if ( v == svl->v )
            break;
        if ( !svl )
          error("algtodalg : incompatible numberfield");
      }
      ptod(ALG,nf->vl,p,&dp);
      MKDAlg(dp,dn,da);
      simpdalg(da,r);
      break;
    default:
      error("algtodalg : invalid argument");
      break;
  }
}

void dalgtoalg(DAlg da,Alg *r)
{
  NumberField nf;
  P p,p1;
  Q inv;

  if ( !(nf=current_numberfield) )
    error("dalgtoalg : current_numberfield is not set");
  if ( !da ) *r = 0;
  else {
    dtop(ALG,nf->vl,da->nm,(Obj *)&p);
    invq((Q)da->dn,&inv);
    mulpq(p,(P)inv,&p1);
    MKAlg(p1,*r);
  }
}

void simpdalg(DAlg da,DAlg *r)
{
  NumberField nf;
  DP nm;
  DAlg d;
  Z dn,dn1;
  struct order_spec *current_spec;

  if ( !(nf=current_numberfield) )
    error("simpdalg : current_numberfield is not set");
  if ( !da ) {
    *r = 0;
    return;
  }
  current_spec = dp_current_spec; initd(nf->spec);
  dp_true_nf(nf->ind,da->nm,nf->ps,1,&nm,(P *)&dn);
  if ( !nm ) *r = 0;
  else {
    initd(current_spec);
    mulz(da->dn,dn,&dn1);
    MKDAlg(nm,dn1,d);
    rmcontdalg(d,r);
  }
}

void adddalg(DAlg a,DAlg b,DAlg *c)
{
  NumberField nf;
  Z dna,dnb,a1,b1,dn,g,an,bn,gn;
  DAlg t;
  DP ta,tb,nm;
  struct order_spec *current_spec;

  if ( !(nf=current_numberfield) )
    error("adddalg : current_numberfield is not set");
  if ( !a )
    *c = b;
  else if ( !b )
    *c = a;
  else {
    qtodalg((Q)a,&t); a = t; qtodalg((Q)b,&t); b = t;
    dna = a->dn;
    dnb = b->dn;
    gcdz(dna,dnb,&gn);
    divz(dna,gn,&a1); divz(dnb,gn,&b1);
    /* nma/dna+nmb/dnb = (nma*b1+nmb*a1)/(dna*b1) */
    muldc(CO,a->nm,(Obj)b1,&ta); muldc(CO,b->nm,(Obj)a1,&tb);
    current_spec = dp_current_spec; initd(nf->spec);
    addd(CO,ta,tb,&nm);
    initd(current_spec);
    if ( !nm )
      *c = 0;
    else {
      mulz(dna,b1,&dn);
      MKDAlg(nm,dn,*c);
    }
  }
}

void subdalg(DAlg a,DAlg b,DAlg *c)
{
  NumberField nf;
  Z dna,dnb,a1,b1,dn,gn;
  DP ta,tb,nm;
  DAlg t;
  struct order_spec *current_spec;

  if ( !(nf=current_numberfield) )
    error("subdalg : current_numberfield is not set");
  if ( !a )
    *c = b;
  else if ( !b )
    *c = a;
  else {
    qtodalg((Q)a,&t); a = t; qtodalg((Q)b,&t); b = t;
    dna = a->dn;
    dnb = b->dn;
    gcdz(dna,dnb,&gn);
    divz(dna,gn,&a1); divz(dnb,gn,&b1);
    /* nma/dna-nmb/dnb = (nma*b1-nmb*a1)/(dna*b1) */
    muldc(CO,a->nm,(Obj)b1,&ta); muldc(CO,b->nm,(Obj)a1,&tb);
    current_spec = dp_current_spec; initd(nf->spec);
    subd(CO,ta,tb,&nm);
    initd(current_spec);
    if ( !nm )
      *c = 0;
    else {
      mulz(dna,b1,&dn);
      MKDAlg(nm,dn,*c);
    }
  }
}

void muldalg(DAlg a,DAlg b,DAlg *c)
{
  NumberField nf;
  DP nm;
  Z dn;
  DAlg t;
  struct order_spec *current_spec;

  if ( !(nf=current_numberfield) )
    error("muldalg : current_numberfield is not set");
  if ( !a || !b )
    *c = 0;
  else {
    qtodalg((Q)a,&t); a = t; qtodalg((Q)b,&t); b = t;
    current_spec = dp_current_spec; initd(nf->spec);
    muld(CO,a->nm,b->nm,&nm);
    initd(current_spec);
    mulz(a->dn,b->dn,&dn);
    MKDAlg(nm,dn,t);
    simpdalg(t,c);
  }
}


void divdalg(DAlg a,DAlg b,DAlg *c)
{
  DAlg inv,t;
  int ret;

  if ( !current_numberfield )
    error("divdalg : current_numberfield is not set");
  if ( !b )
    error("divdalg : division by 0");
  if ( !a )
    c = 0;
  else {
    qtodalg((Q)a,&t); a = t; qtodalg((Q)b,&t); b = t;
    ret = invdalg(b,&inv);
    if ( !ret ) {
      error("divdalg : the denominator is not invertible");
    }
    muldalg(a,inv,c);
  }
}

void rmcontdalg(DAlg a, DAlg *r)
{
  DP u,u1;
  Z cont,c,d,gn;

  if ( !a )
    *r = a;
  else {
    dp_ptozp(a->nm,&u);
    divz((Z)BDY(a->nm)->c,(Z)BDY(u)->c,&cont);
    gcdz(cont,a->dn,&gn);
    divsz(cont,gn,&c);
    divsz(a->dn,gn,&d);
    muldc(CO,u,(Obj)c,&u1);
    MKDAlg(u1,d,*r);
  }
}

int invdalg(DAlg a,DAlg *c)
{
  NumberField nf;
  int dim,n,i,j,k,l;
  DP *mb;
  DP m,d,u;
  Z ln,gn,qn,dnsol;
  Q dn,mul,nmc,dn1;
  DAlg *simp;
  DAlg t,a0,r;
  MAT mobj,sol;
  Z **mat,**solmat;
  MP mp0,mp;
  int *rinfo,*cinfo;
  int rank,nparam;
  NODE nd0,nd,ndt;
  struct order_spec *current_spec;
  struct oEGT eg0,eg1;
  extern struct oEGT eg_le;

  if ( !(nf=current_numberfield) )
    error("invdalg : current_numberfield is not set");
  if ( !a )
    error("invdalg : division by 0");
  else if ( NID(a) == N_Q ) {
    invq((Q)a,&dn); *c = (DAlg)dn;
    return 1;
  }
  dim = nf->dim;
  mb = nf->mb;
  n = nf->n;
  ln = ONE;
  dp_ptozp(a->nm,&u); divq((Q)BDY(a->nm)->c,(Q)BDY(u)->c,&nmc);
  MKDAlg(u,ONE,a0);
  simp = (DAlg *)ALLOCA(dim*sizeof(DAlg));
  current_spec = dp_current_spec; initd(nf->spec);
  for ( i = 0; i < dim; i++ ) {
    m = mb[i];
    for ( j = i-1; j >= 0; j-- )
      if ( dp_redble(m,mb[j]) )
        break;
    if ( j >= 0 ) {
      dp_subd(m,mb[j],&d);
      muld(CO,d,simp[j]->nm,&u);
      MKDAlg(u,simp[j]->dn,t);
      simpdalg(t,&simp[i]);
    } else {
      MKDAlg(m,ONE,t);
      muldalg(t,a0,&simp[i]);
    }
    gcdz(simp[i]->dn,ln,&gn); divz(ln,gn,&qn);
    mulz(simp[i]->dn,qn,&ln);
  }
  initd(current_spec);
  absq((Q)ln,&dn);
  MKMAT(mobj,dim,dim+1);
  mat = (Z **)BDY(mobj);
  mulq((Q)dn,(Q)a->dn,(Q *)&mat[0][dim]);
  for ( j = 0; j < dim; j++ ) {
    divq((Q)dn,(Q)simp[j]->dn,&mul);
    for ( i = dim-1, mp = BDY(simp[j]->nm); mp && i >= 0; i-- )
      if ( dl_equal(n,BDY(mb[i])->dl,mp->dl) ) {
        mulq(mul,(Q)mp->c,(Q *)&mat[i][j]);
        mp = NEXT(mp);
      }
  }
  get_eg(&eg0);
  rank = generic_gauss_elim_hensel(mobj,&sol,&dnsol,&rinfo,&cinfo);
  get_eg(&eg1); add_eg(&eg_le,&eg0,&eg1);
  if ( cinfo[0] == dim ) {
    /* the input is invertible */
    solmat = (Z **)BDY(sol);
    for ( i = dim-1, mp0 = 0; i >= 0; i-- )
      if ( solmat[i][0] ) {
        NEXTMP(mp0,mp);
        mp->c = (Obj)solmat[i][0];
        mp->dl = BDY(mb[i])->dl;
      }
    NEXT(mp) = 0; MKDP(n,mp0,u);
    mulq((Q)dnsol,nmc,(Q *)&dn1);
    MKDAlg(u,(Z)dn1,r);
    rmcontdalg(r,c);
    return 1;
  } else
    return 0;
}

NODE inv_or_split_dalg(DAlg a,DAlg *c)
{
  NumberField nf;
  int dim,n,i,j,k,l;
  DP *mb;
  DP m,d,u;
  Z ln,gn,qn;
  DAlg *simp;
  DAlg t,a0,r;
  Q dn,mul;
  Z dnsol,dn1,nmc;
  MAT mobj,sol;
  Z **mat,**solmat;
  MP mp0,mp;
  int *rinfo,*cinfo;
  int rank,nparam;
  NODE nd0,nd,ndt;
  struct order_spec *current_spec;
  struct oEGT eg0,eg1;
  extern struct oEGT eg_le;
  extern int DP_Print;

  if ( !(nf=current_numberfield) )
    error("invdalg : current_numberfield is not set");
  if ( !a )
    error("invdalg : division by 0");
  else if ( NID(a) == N_Q ) {
    invq((Q)a,&dn); *c = (DAlg)dn;
    return 0;
  }
  dim = nf->dim;
  mb = nf->mb;
  n = nf->n;
  ln = ONE;
  dp_ptozp(a->nm,&u); divz((Z)BDY(a->nm)->c,(Z)BDY(u)->c,&nmc);
  MKDAlg(u,ONE,a0);
  simp = (DAlg *)MALLOC(dim*sizeof(DAlg));
  current_spec = dp_current_spec; initd(nf->spec);
  for ( i = 0; i < dim; i++ ) {
    if ( DP_Print ) { fprintf(asir_out,"."); fflush(asir_out); }
    m = mb[i];
    for ( j = i-1; j >= 0; j-- )
      if ( dp_redble(m,mb[j]) )
        break;
    if ( j >= 0 ) {
      dp_subd(m,mb[j],&d);
      if ( simp[j] ) {
        muld(CO,d,simp[j]->nm,&u);
        MKDAlg(u,simp[j]->dn,t);
        simpdalg(t,&simp[i]);
      } else
        simp[i] = 0;
    } else {
      MKDAlg(m,ONE,t);
      muldalg(t,a0,&simp[i]);
    }
    if ( simp[i] ) {
      gcdz(simp[i]->dn,ln,&gn); divz(ln,gn,&qn);
      mulz(simp[i]->dn,qn,&ln);
    }
  }
  initd(current_spec);
  absq((Q)ln,&dn);
  MKMAT(mobj,dim,dim+1);
  mat = (Z **)BDY(mobj);
  mulq(dn,(Q)a->dn,(Q *)&mat[0][dim]);
  for ( j = 0; j < dim; j++ ) {
    if ( simp[j] ) {
      divq(dn,(Q)simp[j]->dn,&mul);
      for ( i = dim-1, mp = BDY(simp[j]->nm); mp && i >= 0; i-- )
        if ( dl_equal(n,BDY(mb[i])->dl,mp->dl) ) {
          mulq(mul,(Q)mp->c,(Q *)&mat[i][j]);
          mp = NEXT(mp);
        }
    }
  }
  get_eg(&eg0);
  rank = generic_gauss_elim_hensel_dalg(mobj,mb,&sol,&dnsol,&rinfo,&cinfo);
  get_eg(&eg1); add_eg(&eg_le,&eg0,&eg1);
  if ( cinfo[0] == dim ) {
    /* the input is invertible */
    solmat = (Z **)BDY(sol);
    for ( i = dim-1, mp0 = 0; i >= 0; i-- )
      if ( solmat[i][0] ) {
        NEXTMP(mp0,mp);
        mp->c = (Obj)solmat[i][0];
        mp->dl = BDY(mb[i])->dl;
      }
    NEXT(mp) = 0; MKDP(n,mp0,u);
    mulz(dnsol,nmc,&dn1);
    MKDAlg(u,dn1,r);
    rmcontdalg(r,c);
    return 0;
  } else {
    /* the input is not invertible */
    nparam = sol->col;
    solmat = (Z **)BDY(sol);
    nd0 = 0;
    for ( k = 0; k < nparam; k++ ) {
      /* construct a new basis element */
      m = mb[cinfo[k]];
      mp0 = 0;
      NEXTMP(mp0,mp);
      chsgnz(dnsol,&dn1); mp->c = (Obj)dn1;
      mp->dl = BDY(m)->dl;
      /* skip the last parameter */
      for ( l = rank-2; l >= 0; l-- ) {
        if ( solmat[l][k] ) {
          NEXTMP(mp0,mp);
          mp->c = (Obj)solmat[l][k];
          mp->dl = BDY(mb[rinfo[l]])->dl;
        }
      }
      NEXT(mp) = 0; MKDP(n,mp0,u);
      NEXTNODE(nd0,nd);
      BDY(nd) = (pointer)u;
      NEXT(nd) = 0;
    }
    NEXT(nd) = 0;
    return nd0;
  }
}

NODE dp_inv_or_split(NODE gb,DP f,struct order_spec *spec, DP *inv)
{
  int dim,n,i,j,k,l,nv;
  DP *mb,*ps;
  DP m,d,u,nm;
  Z ln,gn,qn;
  DAlg *simp;
  DAlg a0,r;
  Q mul,nmc;
  Z iq,dn,dn1,dnsol;
  MAT mobj,sol;
  Z **mat,**solmat;
  MP mp0,mp;
  int *rinfo,*cinfo;
  int rank,nparam;
  NODE nd0,nd,ndt,ind,indt,t,mblist;
  struct oEGT eg0,eg1;
  extern struct oEGT eg_le;
  extern int DP_Print;
  initd(spec);
  dp_ptozp(f,&u); f = u;

  n = length(gb);
  ps = (DP *)MALLOC(n*sizeof(DP));
  for ( ind = 0, i = 0, t = gb; i < n; i++, t = NEXT(t) ) {
    ps[i] = (DP)BDY(t);
    NEXTNODE(ind,indt);
    STOQ(i,iq); BDY(indt) = iq;
  }
  if ( ind ) NEXT(indt) = 0;
  dp_true_nf(ind,f,ps,1,&nm,(P *)&dn);
  if ( !nm ) error("dp_inv_or_split : input is 0");
  f = nm;

  dp_mbase(gb,&mblist);
  dim = length(mblist);
  mb = (DP *)MALLOC(dim*sizeof(DP));
  for ( i = 0, t = mblist; i < dim; i++, t = NEXT(t) )
    mb[dim-i-1] = (DP)BDY(t);
  nv = mb[0]->nv;
  ln = ONE;
  simp = (DAlg *)MALLOC(dim*sizeof(DAlg));
  for ( i = 0; i < dim; i++ ) {
    if ( DP_Print ) { fprintf(asir_out,"."); fflush(asir_out); }
    m = mb[i];
    for ( j = i-1; j >= 0; j-- )
      if ( dp_redble(m,mb[j]) )
        break;
    if ( j >= 0 ) {
      dp_subd(m,mb[j],&d);
      if ( simp[j] ) {
        muld(CO,d,simp[j]->nm,&u);
        dp_true_nf(ind,u,ps,1,&nm,(P *)&dn);
        mulz(simp[j]->dn,dn,&dn1);
        MKDAlg(nm,dn1,simp[i]);
      } else
        simp[i] = 0;
    } else {
      dp_true_nf(ind,f,ps,1,&nm,(P *)&dn);
      MKDAlg(nm,dn,simp[i]);
    }
    if ( simp[i] ) {
      gcdz(simp[i]->dn,ln,&gn); divz(ln,gn,&qn);
      mulz(simp[i]->dn,qn,&ln);
    }
  }
  absz(ln,&dn);
  MKMAT(mobj,dim,dim+1);
  mat = (Z **)BDY(mobj);
  mat[0][dim] = dn;
  for ( j = 0; j < dim; j++ ) {
    if ( simp[j] ) {
      divq((Q)dn,(Q)simp[j]->dn,&mul);
      for ( i = dim-1, mp = BDY(simp[j]->nm); mp && i >= 0; i-- )
        if ( dl_equal(nv,BDY(mb[i])->dl,mp->dl) ) {
          mulq(mul,(Q)mp->c,(Q *)&mat[i][j]);
          mp = NEXT(mp);
        }
    }
  }
  get_eg(&eg0);
  rank = generic_gauss_elim_hensel_dalg(mobj,mb,&sol,&dnsol,&rinfo,&cinfo);
  get_eg(&eg1); add_eg(&eg_le,&eg0,&eg1);
  if ( cinfo[0] == dim ) {
    /* the input is invertible */
    solmat = (Z **)BDY(sol);
    for ( i = dim-1, mp0 = 0; i >= 0; i-- )
      if ( solmat[i][0] ) {
        NEXTMP(mp0,mp);
        mp->c = (Obj)solmat[i][0];
        mp->dl = BDY(mb[i])->dl;
      }
    NEXT(mp) = 0; MKDP(nv,mp0,*inv);
    return 0;
  } else {
    /* the input is not invertible */
    nparam = sol->col;
    solmat = (Z **)BDY(sol);
    nd0 = 0;
    for ( k = 0; k < nparam; k++ ) {
      /* construct a new basis element */
      m = mb[cinfo[k]];
      mp0 = 0;
      NEXTMP(mp0,mp);
      chsgnz(dnsol,&dn1); mp->c = (Obj)dn1;
      mp->dl = BDY(m)->dl;
      /* skip the last parameter */
      for ( l = rank-2; l >= 0; l-- ) {
        if ( solmat[l][k] ) {
          NEXTMP(mp0,mp);
          mp->c = (Obj)solmat[l][k];
          mp->dl = BDY(mb[rinfo[l]])->dl;
        }
      }
      NEXT(mp) = 0; MKDP(nv,mp0,u);
      NEXTNODE(nd0,nd);
      BDY(nd) = (pointer)u;
      NEXT(nd) = 0;
    }
    NEXT(nd) = 0;
    return nd0;
  }
}

void chsgndalg(DAlg a,DAlg *c)
{
  DP nm;
  Q t;

  if ( !a ) *c = 0;
  else if ( NID(a) == N_Q ) {
    chsgnq((Q)a,&t); *c = (DAlg)t;
  } else {
    chsgnd(a->nm,&nm);
    MKDAlg(nm,a->dn,*c);
  }
}

void pwrdalg(DAlg a,Z e,DAlg *c)
{
  NumberField nf;
  DAlg t,z,y;
  Q q;
  Z en,qn,rn,two;
  int ret;

  if ( !(nf=current_numberfield) )
    error("pwrdalg : current_numberfield is not set");
  if ( !a )
    *c = !e ? (DAlg)ONE : 0;
  else if ( NID(a) == N_Q ) {
    pwrq((Q)a,(Q)e,&q); *c = (DAlg)q;
  } else if ( !e )
    *c = nf->one;
  else if ( UNIZ(e) )
    *c = a;
  else {
    if ( sgnz(e) < 0 ) {
      ret = invdalg(a,&t); 
      if ( !ret )
        error("pwrdalg : the denominator is not invertible");
      a = t;
    }
    absz(e,&en);
    y = nf->one;
    z = a;
    STOQ(2,two);
    while ( 1 ) {
      divqrz(en,two,&qn,&rn); en = qn;
      if ( rn ) {
        muldalg(z,y,&t); y = t;
        if ( !en ) {
          *c = y;
          return;
        }
      }
      muldalg(z,z,&t); z = t;
    }
  }
}

int cmpdalg(DAlg a,DAlg b)
{
  DAlg c;

  subdalg(a,b,&c);
  if ( !c ) return 0;
  else
    return sgnq((Q)BDY(c->nm)->c);
}

/* convert da to a univariate poly; return the position of variable */

int dalgtoup(DAlg da,P *up,Z *dn)
{
  int nv,i,hi,current_d;
  DCP dc0,dc;
  MP h,mp0,mp,t;
  DL hd,d;
  DP c;
  DAlg cc;
  P v;

  nv = da->nm->nv;
  h = BDY(da->nm);
  *dn = da->dn;
  hd = h->dl;
  for ( i = 0; i < nv; i++ )
    if ( hd->d[i] ) break;
  hi = i;
  current_d = hd->d[i];
  dc0 = 0;
  mp0 = 0;
  for ( t = h; t; t = NEXT(t) ) {
    NEWDL(d,nv);
    for ( i = 0; i <= hi; i++ ) d->d[i] = 0;
    for ( ; i < nv; i++ ) d->d[i] = t->dl->d[i];
    d->td = t->dl->td - t->dl->d[hi];
    if ( t->dl->d[hi] != current_d ) {
      NEXT(mp) = 0; MKDP(nv,mp0,c); MKDAlg(c,ONE,cc);
      NEXTDC(dc0,dc); STOQ(current_d,DEG(dc)); COEF(dc) = (P)cc;
      current_d = t->dl->d[hi];
      mp0 = 0;
    }
    NEXTMP(mp0,mp);
    mp->c = t->c; mp->dl = d;
  }
  NEXT(mp) = 0; MKDP(nv,mp0,c); MKDAlg(c,ONE,cc);
  NEXTDC(dc0,dc); STOQ(current_d,DEG(dc)); COEF(dc) = (P)cc;
  NEXT(dc) = 0;
  makevar("x",&v);
  MKP(VR(v),dc0,*up);
  return hi;
}

