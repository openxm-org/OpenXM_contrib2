#define INTERVAL
#include "ca.h"
#include "parse.h"
#include "interval.h"

void Padd_pinf(),Padd_minf();
void Psub_pinf(),Psub_minf();
void Pmul_pinf(),Pmul_minf();
void Pdiv_pinf(),Pdiv_minf();

struct ftab round_tab[] = {
	{"add_pinf",Padd_pinf,2},
	{"add_minf",Padd_minf,2},
	{"sub_pinf",Psub_pinf,2},
	{"sub_minf",Psub_minf,2},
	{"mul_pinf",Pmul_pinf,2},
	{"mul_minf",Pmul_minf,2},
	{"div_pinf",Pdiv_pinf,2},
	{"div_minf",Pdiv_minf,2},
	{0,0,0},
};

void Padd_pinf(NODE arg,Real *rp)
{
	Real a,b,r;
	double c;

	a = (Real)ARG0(arg);
	b = (Real)ARG1(arg);
	if ( !a )
		*rp = b;
	else if ( !b )
		*rp = a;
	else {
		FPPLUSINF
		c = BDY(a)+BDY(b);
		FPNEAREST
		MKReal(c,r);
		*rp = r;
	}
}

void Padd_minf(NODE arg,Real *rp)
{
	Real a,b,r;
	double c;

	a = (Real)ARG0(arg);
	b = (Real)ARG1(arg);
	if ( !a )
		*rp = b;
	else if ( !b )
		*rp = a;
	else {
		FPMINUSINF
		c = BDY(a)+BDY(b);
		FPNEAREST
		MKReal(c,r);
		*rp = r;
	}
}

void Psub_pinf(NODE arg,Real *rp)
{
	Real a,b,r;
	double c;

	a = (Real)ARG0(arg);
	b = (Real)ARG1(arg);
	if ( !a ) {
		if ( !b )
			*rp = 0;
		else {
			c = -BDY(b);
			MKReal(c,r);
			*rp = r;
		}
	} else if ( !b )
		*rp = a;
	else {
		FPPLUSINF
		c = BDY(a)-BDY(b);
		FPNEAREST
		MKReal(c,r);
		*rp = r;
	}
}

void Psub_minf(NODE arg,Real *rp)
{
	Real a,b,r;
	double c;

	a = (Real)ARG0(arg);
	b = (Real)ARG1(arg);
	if ( !a ) {
		if ( !b )
			*rp = 0;
		else {
			c = -BDY(b);
			MKReal(c,r);
			*rp = r;
		}
	} else if ( !b )
		*rp = a;
	else {
		FPMINUSINF
		c = BDY(a)-BDY(b);
		FPNEAREST
		MKReal(c,r);
		*rp = r;
	}
}

void Pmul_pinf(NODE arg,Real *rp)
{
	Real a,b,r;
	double c;

	a = (Real)ARG0(arg);
	b = (Real)ARG1(arg);
	if ( !a || !b )
		*rp = 0;
	else {
		FPPLUSINF
		c = BDY(a)*BDY(b);
		FPNEAREST
		MKReal(c,r);
		*rp = r;
	}
}

void Pmul_minf(NODE arg,Real *rp)
{
	Real a,b,r;
	double c;

	a = (Real)ARG0(arg);
	b = (Real)ARG1(arg);
	if ( !a || !b )
		*rp = 0;
	else {
		FPMINUSINF
		c = BDY(a)*BDY(b);
		FPNEAREST
		MKReal(c,r);
		*rp = r;
	}
}

void Pdiv_pinf(NODE arg,Real *rp)
{
	Real a,b,r;
	double c;

	a = (Real)ARG0(arg);
	b = (Real)ARG1(arg);
	if ( !b )
		error("div_pinf : division by 0");
	else if ( !a )
		*rp = 0;
	else {
		FPPLUSINF
		c = BDY(a)/BDY(b);
		FPNEAREST
		MKReal(c,r);
		*rp = r;
	}
}

void Pdiv_minf(NODE arg,Real *rp)
{
	Real a,b,r;
	double c;

	a = (Real)ARG0(arg);
	b = (Real)ARG1(arg);
	if ( !b )
		error("div_pinf : division by 0");
	else if ( !a )
		*rp = 0;
	else {
		FPMINUSINF
		c = BDY(a)/BDY(b);
		FPNEAREST
		MKReal(c,r);
		*rp = r;
	}
}
