#ifdef MODULAR
void divsrmp(VL vl,int mod,P p1,P p2,P *q,P *r)
#else
void divsrp(VL vl,P p1,P p2,P *q,P *r)
#endif
{
  register int i,j;
  register DCP dc1,dc2,dc;
  P m,s,dvr;
  P *pq,*pr,*pd;
  V v1,v2;
  Z deg1,deg2;
  int d1,d2,sgn;

  if ( !p1 ) {
    *q = 0; *r = 0;
  } else if ( NUM(p2) ) 
    if ( NUM(p1) ) {
      DIVNUM(p1,p2,q); *r = 0;
    } else {
      DIVSDCP(vl,p1,p2,q); *r = 0;
    }
  else if ( NUM(p1) ) {
    *q = 0; *r = p1;
  } else if ( ( v1 = VR(p1) ) == ( v2 = VR(p2) ) ) {
    dc1 = DC(p1); dc2 = DC(p2);
    deg1 = DEG(dc1); deg2 = DEG(dc2);
    sgn = cmpz(deg1,deg2);
    if ( sgn == 0 ) {
      DIVSP(vl,COEF(dc1),COEF(dc2),q);
      MULP(vl,p2,*q,&m); SUBP(vl,p1,m,r); 
    } else if ( sgn < 0 ) {
      *q = 0; *r = p1;
    } else {  
      if ( !smallz(deg1)  ) 
        error("divsrp : invalid input");
      d1 = QTOS(deg1); d2 = QTOS(deg2); 
      W_CALLOC(d1-d2,P,pq); W_CALLOC(d1,P,pr); W_CALLOC(d2,P,pd);
      for ( dc = dc1; dc; dc = NEXT(dc) ) 
        pr[QTOS(DEG(dc))] = COEF(dc);
      for ( dc = dc2; dc; dc = NEXT(dc) ) 
        pd[QTOS(DEG(dc))] = COEF(dc);
      for ( dvr = COEF(dc2), i = d1 - d2; i >= 0; i-- ) {
        if ( !pr[i+d2] ) 
          continue;
        DIVSP(vl,pr[i+d2],dvr,&pq[i]);
        for ( j = d2; j >= 0; j-- ) {
          MULP(vl,pq[i],pd[j],&m);
          SUBP(vl,pr[i + j],m,&s); pr[i + j] = s;
        }
      }
      plisttop(pq,v1,d1 - d2,q); plisttop(pr,v1,d1 - 1,r);
    }
  } else {
    for ( ; (v1 != vl->v) && (v2 != vl->v); vl = NEXT(vl) );
    if ( v2 == vl->v ) {
      *q = 0; *r = p1;
    } else
      DIVSRDCP(vl,p1,p2,q,r);
  }
}

#ifdef MODULAR
void divsrdcmp(VL vl,int mod,P p1,P p2,P *q,P *r)
#else
void divsrdcp(VL vl,P p1,P p2,P *q,P *r)
#endif
{

  P qc,rc;
  DCP dc,dcq,dcq0,dcr,dcr0;

  for ( dc = DC(p1), dcq0 = 0, dcr0 = 0; dc; dc = NEXT(dc) ) {
    DIVSRP(vl,COEF(dc),p2,&qc,&rc);
    if ( qc ) {
      NEXTDC(dcq0,dcq); DEG(dcq) = DEG(dc); COEF(dcq) = qc;
    }
    if ( rc ) {
      NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = rc;
    }
  }
  if ( dcq0 ) {
    NEXT(dcq) = 0; MKP(VR(p1),dcq0,*q);
  } else
    *q = 0;
  if ( dcr0 ) {
    NEXT(dcr) = 0; MKP(VR(p1),dcr0,*r);
  } else
    *r = 0;
}

#ifdef MODULAR
void divsmp(VL vl,int mod,P p1,P p2,P *q)
#else
void divsp(VL vl,P p1,P p2,P *q)
#endif
{
  P t;

  DIVSRP(vl,p1,p2,q,&t);
  if ( t ) 
    error("divsp: cannot happen");
}

#ifdef MODULAR
void divsdcmp(VL vl,int mod,P p1,P p2,P *q)
#else
void divsdcp(VL vl,P p1,P p2,P *q)
#endif
{

  P m;
  register DCP dc,dcr,dcr0;

  for ( dc = DC(p1), dcr0 = 0; dc; dc = NEXT(dc) ) {
    DIVSP(vl,COEF(dc),p2,&m);
    NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = m; NEXT(dcr) = 0;
  }
  MKP(VR(p1),dcr0,*q);
}

