/* $OpenXM: OpenXM/src/asir99/builtin/type.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Ptype(), Pntype();

struct ftab type_tab[] = {
	{"type",Ptype,1},
	{"ntype",Pntype,1},
	{0,0,0},
};

void Ptype(arg,rp)
NODE arg;
Obj *rp;
{
	Obj t;
	Q q;

	if ( t = (Obj)ARG0(arg) ) {
		STOQ(OID(t),q); *rp = (Obj)q;
	} else 
		*rp = 0;
}

void Pntype(arg,rp)
NODE arg;
Obj *rp;
{
	Obj t;
	Q q;

	if ( t = (Obj)ARG0(arg) ) {
		asir_assert(t,O_N,"ntype");
		STOQ(NID(t),q); *rp = (Obj)q;
	} else 
		*rp = 0;
}
