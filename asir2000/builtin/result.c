/* $OpenXM: OpenXM/src/asir99/builtin/result.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Presult(), Psrcr();

struct ftab result_tab[] = {
	{"res",Presult,-4},
	{"srcr",Psrcr,3},
	{0,0,0},
};

void Presult(arg,rp)
NODE arg;
P *rp;
{
	int mod;
	P p1,p2,r;

	asir_assert(ARG0(arg),O_P,"res");
	asir_assert(ARG1(arg),O_P,"res");
	asir_assert(ARG2(arg),O_P,"res");
	if ( argc(arg) == 3 )
		resultp(CO,VR((P)ARG0(arg)),(P)ARG1(arg),(P)ARG2(arg),rp);
	else {
		mod = QTOS((Q)ARG3(arg));
		ptomp(mod,(P)ARG1(arg),&p1); ptomp(mod,(P)ARG2(arg),&p2);
		resultmp(CO,mod,VR((P)ARG0(arg)),p1,p2,&r);
		mptop(r,rp);
	}
}

void Psrcr(arg,rp)
NODE arg;
P *rp;
{
	asir_assert(ARG0(arg),O_P,"srcr");
	asir_assert(ARG1(arg),O_P,"srcr");
	asir_assert(ARG2(arg),O_P,"srcr");
	res_ch_det(CO,VR((P)ARG0(arg)),(P)ARG1(arg),(P)ARG2(arg),rp);
}
