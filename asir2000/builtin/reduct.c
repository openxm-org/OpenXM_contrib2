/* $OpenXM: OpenXM/src/asir99/builtin/reduct.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Pred(), Predc(), Pprim();

struct ftab reduct_tab[] = {
	{"red",Pred,1},
	{"redc",Predc,2},
	{"prim",Pprim,-2},
	{0,0,0},
};

void Pred(arg,rp)
NODE arg;
Obj *rp;
{
	asir_assert(ARG0(arg),O_R,"red");
	reductr(CO,(Obj)ARG0(arg),rp);
}

void Predc(arg,rp)
NODE arg;
P *rp;
{
	asir_assert(ARG0(arg),O_P,"redc");
	asir_assert(ARG1(arg),O_P,"redc");
	remsdcp(CO,(P)ARG0(arg),(P)ARG1(arg),rp);
}

void Pprim(arg,rp)
NODE arg;
P *rp;
{
	P t,p,p1,r;
	V v;
	VL vl;

	asir_assert(ARG0(arg),O_P,"prim");
	p = (P)ARG0(arg);
	if ( NUM(p) )
		*rp = (P)ONE;
	else {
		if ( argc(arg) == 2 ) {
			v = VR((P)ARG1(arg));
			change_mvar(CO,p,v,&p1);
			if ( VR(p1) != v ) {
				*rp = (P)ONE; return;
			} else {
				reordvar(CO,v,&vl); pcp(vl,p1,&r,&t);
				restore_mvar(CO,r,v,rp);
			}
		} else
			pcp(CO,p,rp,&t);
	}
}
