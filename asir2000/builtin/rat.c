/* $OpenXM: OpenXM/src/asir99/builtin/rat.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Pnm(), Pdn(), Pderiv();

struct ftab rat_tab[] = {
	{"nm",Pnm,1},
	{"dn",Pdn,1},
	{"diff",Pderiv,-99999999},
	{0,0,0},
};

void Pnm(arg,rp)
NODE arg;
Obj *rp;
{
	Obj t;
	Q q;

	asir_assert(ARG0(arg),O_R,"nm");
	if ( !(t = (Obj)ARG0(arg)) )
		*rp = 0;
	else
		switch ( OID(t) ) {
			case O_N: 
				switch ( NID(t) ) {
					case N_Q:
						NTOQ(NM((Q)t),SGN((Q)t),q); *rp = (Obj)q; break;
					default:
						*rp = t; break;
				}
				break;
			case O_P:
				*rp = t; break;
			case O_R:
				*rp = (Obj)NM((R)t); break;
			default:
				*rp = 0;
		}
}

void Pdn(arg,rp)
NODE arg;
Obj *rp;
{
	Obj t;
	Q q;

	asir_assert(ARG0(arg),O_R,"dn");
	if ( !(t = (Obj)ARG0(arg)) )
		*rp = (Obj)ONE;
	else
		switch ( OID(t) ) {
			case O_N: 
				switch ( NID(t) ) {
					case N_Q:
						if ( DN((Q)t) )
							NTOQ(DN((Q)t),1,q);
						else
							q = ONE;
						*rp = (Obj)q; break;
					default:
						*rp = (Obj)ONE; break;
				}
				break;
			case O_P:
				*rp = (Obj)ONE; break;
			case O_R:
				*rp = (Obj)DN((R)t); break;
			default:
				*rp = 0;
		}
}

void Pderiv(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,t;
	LIST l;
	P v;
	NODE n;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_R,"diff");
	reductr(CO,(Obj)ARG0(arg),&a);
	n = NEXT(arg);
	if ( n && (l = (LIST)ARG0(n)) && OID(l) == O_LIST )
		n = BDY(l);
	for ( ; n; n = NEXT(n) ) {
		if ( !(v = (P)BDY(n)) || OID(v) != O_P )
			error("diff : invalid argument");
		derivr(CO,a,VR(v),&t); a = t;
	}
	*rp = a;
}
