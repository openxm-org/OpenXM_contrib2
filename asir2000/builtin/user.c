/* $OpenXM$ */

/* a sample file for adding builtin functions */

#include "ca.h"
#include "parse.h"

void Ppartial_derivative();
void partial_derivative(VL vl,P p,V v,P *r);

struct ftab user_tab[] = {
/*
  {"partial_derivative",Ppartial_derivative,2},
*/
  {0,0,0},
};

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