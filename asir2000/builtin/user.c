/* $OpenXM: OpenXM_contrib2/asir2000/builtin/user.c,v 1.3 2004/10/06 11:58:51 noro Exp $ */

/* a sample file for adding builtin functions */

#include "ca.h"
#include "parse.h"

void Ppartial_derivative();
void partial_derivative(VL vl,P p,V v,P *r);
void Pzadd(),Pzsub(),Pzmul();
void Pcomp_f();

struct ftab user_tab[] = {
/*
  {"partial_derivative",Ppartial_derivative,2},
*/
  {"zadd",Pzadd,2},
  {"zsub",Pzsub,2},
  {"zmul",Pzmul,2},
  {"comp_f",Pcomp_f,2},
  {0,0,0},
};

/* compare two [[F,M],...] */

void Pcomp_f(NODE arg,Q *rp)
{
  NODE l1,l2,e1,e2;
  int m1,m2,r;

  l1 = BDY((LIST)ARG0(arg));  
  l2 = BDY((LIST)ARG1(arg));  
  for ( ; l1 && l2; l1 = NEXT(l1), l2 = NEXT(l2) ) {
    e1 = BDY((LIST)BDY(l1));
    e2 = BDY((LIST)BDY(l2));
    r = compp(CO,(P)ARG0(e1),(P)ARG0(e2));
    if ( r ) { STOQ(r,*rp); return; }
    m1 = QTOS((Q)ARG1(e1));
    m2 = QTOS((Q)ARG1(e2));
	r = m1>m2?1:(m1<m2?-1:0);
	if ( r ) { STOQ(r,*rp); return; }
  }
  r = l1?1:(l2?-1:0);
  STOQ(r,*rp);
}

void Pzadd(NODE arg,Q *rp)
{
	Z z0,z1,z2;

	z0 = qtoz((Q)ARG0(arg));
	z1 = qtoz((Q)ARG1(arg));
	z2 = addz(z0,z1);
	printz(z2); printf(" ");
	*rp = ztoq(z2);
}

void Pzsub(NODE arg,Q *rp)
{
	Z z0,z1,z2;

	z0 = qtoz((Q)ARG0(arg));
	z1 = qtoz((Q)ARG1(arg));
	z2 = subz(z0,z1);
	printz(z2); printf(" ");
	*rp = ztoq(z2);
}

void Pzmul(NODE arg,Q *rp)
{
	Z z0,z1,z2;

	z0 = qtoz((Q)ARG0(arg));
	z1 = qtoz((Q)ARG1(arg));
	z2 = mulz(z0,z1);
	printz(z2); printf(" ");
	*rp = ztoq(z2);
}

/*
void Ppartial_derivative(NODE arg,P *rp)
{
  asir_assert(ARG0(arg),O_P,"partial_derivative");
  asir_assert(ARG1(arg),O_P,"partial_derivative");
  partial_derivative(CO,(P)ARG0(arg),((P)ARG1(arg))->v,rp);
}

void partial_derivative(VL vl,P p,V v,P *r)
{
  P t;
  DCP dc,dcr,dcr0;

  if ( !p || NUM(p) ) *r = 0;
  else if ( v == p->v ) {
    for ( dc = p->dc, dcr0 = 0; dc && dc->d; dc = dc->next ) {
      mulp(vl,dc->c,(P)dc->d,&t);
      if ( t ) { 
        NEXTDC(dcr0,dcr); subq(dc->d,ONE,&dcr->d); dcr->c = t; 
      }
    }
    if ( !dcr0 ) *r = 0;
    else { dcr->next = 0; MKP(v,dcr0,*r); }
  } else {
    for ( dc = p->dc, dcr0 = 0; dc; dc = dc->next ) {
      partial_derivative(vl,dc->c,v,&t);
      if ( t ) { NEXTDC(dcr0,dcr); dcr->d = dc->d; dcr->c = t; }
    }
    if ( !dcr0 ) *r = 0;
    else { dcr->next = 0; MKP(p->v,dcr0,*r); }
  }
}
*/
