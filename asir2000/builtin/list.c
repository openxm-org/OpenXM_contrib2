/* $OpenXM: OpenXM/src/asir99/builtin/list.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Pcar(), Pcdr(), Pcons(), Pappend(), Preverse(), Plength();

struct ftab list_tab[] = {
	{"car",Pcar,1},
	{"cdr",Pcdr,1},
	{"cons",Pcons,2},
	{"append",Pappend,2},
	{"reverse",Preverse,1},
	{"length",Plength,1},
	{0,0,0},
};

void Pcar(arg,rp)
NODE arg;
pointer *rp;
{
	asir_assert(ARG0(arg),O_LIST,"car");
	if ( !BDY((LIST)ARG0(arg)) )
		*rp = ARG0(arg);
	else
		*rp = (pointer)BDY(BDY((LIST)ARG0(arg)));
}

void Pcdr(arg,rp)
NODE arg;
LIST *rp;
{
	asir_assert(ARG0(arg),O_LIST,"cdr");
	if ( !BDY((LIST)ARG0(arg)) )
		*rp = (LIST)ARG0(arg);
	else 
		MKLIST(*rp,NEXT(BDY((LIST)ARG0(arg))));
}

void Pcons(arg,rp)
NODE arg;
LIST *rp;
{
	NODE t;

	asir_assert(ARG1(arg),O_LIST,"cons");
	MKNODE(t,ARG0(arg),BDY((LIST)ARG1(arg))); MKLIST(*rp,t);
}

void Pappend(arg,rp)
NODE arg;
LIST *rp;
{
	NODE t,t0,n;

	asir_assert(ARG0(arg),O_LIST,"append");
	asir_assert(ARG1(arg),O_LIST,"append");
	if ( !(n = BDY((LIST)ARG0(arg))) )
		*rp = (LIST)ARG1(arg);
	else {
		for ( t0 = 0; n; n = NEXT(n) ) {
			NEXTNODE(t0,t); BDY(t) = BDY(n);
		}
		NEXT(t) = BDY((LIST)ARG1(arg));
		MKLIST(*rp,t0);
	}
}

void Preverse(arg,rp)
NODE arg;
LIST *rp;
{
	NODE t,t1,n;

	asir_assert(ARG0(arg),O_LIST,"reverse");
	if ( !(n = BDY((LIST)ARG0(arg))) )
		*rp = (LIST)ARG0(arg);
	else {
		for ( t = 0; n; n = NEXT(n) ) {
			MKNODE(t1,BDY(n),t); t = t1;
		}
		MKLIST(*rp,t);
	}
}

void Plength(arg,rp)
NODE arg;
Q *rp;
{
	NODE n;
	int i;

	asir_assert(ARG0(arg),O_LIST,"length");
	n = BDY((LIST)ARG0(arg));
	for ( i = 0; n; i++, n = NEXT(n) );
	STOQ(i,*rp);
}
