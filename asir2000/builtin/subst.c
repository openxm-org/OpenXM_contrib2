/* $OpenXM: OpenXM/src/asir99/builtin/subst.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Psubst(), Ppsubst(), Psubstf();

struct ftab subst_tab[] = {
	{"subst",Psubst,-99999999},
	{"psubst",Ppsubst,-99999999},
	{"substf",Psubstf,-99999999},
	{0,0,0},
};

void Psubst(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,b,t;
	LIST l;
	V v;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_R,"subst");
	reductr(CO,(Obj)ARG0(arg),&a);
/*	a = (Obj)ARG0(arg); */
	arg = NEXT(arg);
	if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
		arg = BDY(l);
	while ( arg ) {
		asir_assert(BDY(arg),O_P,"subst");
		v = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( !arg )
			error("subst : invalid argument");
		asir_assert(ARG0(arg),O_R,"subst");
		reductr(CO,(Obj)BDY(arg),&b); arg = NEXT(arg);
/*		b = (Obj)BDY(arg); arg = NEXT(arg); */
		substr(CO,0,a,v,b,&t); a = t;
	}
	*rp = a;
}

void Ppsubst(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,b,t;
	LIST l;
	V v;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_R,"psubst");
	reductr(CO,(Obj)ARG0(arg),&a);
/*	a = (Obj)ARG0(arg); */
	arg = NEXT(arg);
	if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
		arg = BDY(l);
	while ( arg ) {
		asir_assert(BDY(arg),O_P,"psubst");
		v = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( !arg )
			error("psubst : invalid argument");
		asir_assert(ARG0(arg),O_R,"psubst");
		reductr(CO,(Obj)BDY(arg),&b); arg = NEXT(arg);
/*		b = (Obj)BDY(arg); arg = NEXT(arg); */
		substr(CO,1,a,v,b,&t); a = t;
	}
	*rp = a;
}

void Psubstf(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,t;
	LIST l;
	V v,f;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_R,"substf");
	reductr(CO,(Obj)ARG0(arg),&a);
/*	a = (Obj)ARG0(arg); */
	arg = NEXT(arg);
	if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
		arg = BDY(l);
	while ( arg ) {
		asir_assert(BDY(arg),O_P,"substf");
		v = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( !arg || (int)v->attr != V_SR )
			error("substf : invalid argument");
		f = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( (int)f->attr != V_SR )
			error("substf : invalid argument\n");
		substfr(CO,a,((FUNC)v->priv)->f.puref,((FUNC)f->priv)->f.puref,&t);
		a = t;
	}
	*rp = a;
}
