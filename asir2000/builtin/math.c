/* $OpenXM: OpenXM/src/asir99/builtin/math.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
#include "ca.h"
#include <math.h>
#include "parse.h"

void Pdsqrt(),Pdsin(),Pdcos(),Pdtan(),Pdasin(),Pdacos(),Pdatan(),Pdlog(),Pdexp();
void Pabs(),Pdfloor(),Pdceil(),Pdrint();

struct ftab math_tab[] = {
	{"dsqrt",Pdsqrt,1},
	{"dabs",Pabs,1},
	{"dsin",Pdsin,1},
	{"dcos",Pdcos,1},
	{"dtan",Pdtan,1},
	{"dlog",Pdlog,1},
	{"dexp",Pdexp,1},
	{"dasin",Pdasin,1},
	{"dacos",Pdacos,1},
	{"datan",Pdatan,1},
	{"dfloor",Pdfloor,1},
	{"dceil",Pdceil,1},
	{"drint",Pdrint,1},
	{0,0,0},
};

void Pabs(arg,rp)
NODE arg;
Real *rp;
{
	double s;

	s = fabs(ToReal(ARG0(arg)));
	MKReal(s,*rp);
}

void Pdsqrt(arg,rp)
NODE arg;
Real *rp;
{
	double s;

	s = sqrt(ToReal(ARG0(arg)));
	MKReal(s,*rp);
}

void Pdsin(arg,rp)
NODE arg;
Real *rp;
{
	double s;

	s = sin(ToReal(ARG0(arg)));
	MKReal(s,*rp);
}

void Pdcos(arg,rp)
NODE arg;
Real *rp;
{
	double s;

	s = cos(ToReal(ARG0(arg)));
	MKReal(s,*rp);
}

void Pdtan(arg,rp)
NODE arg;
Real *rp;
{
	double s;

	s = tan(ToReal(ARG0(arg)));
	MKReal(s,*rp);
}

void Pdasin(arg,rp)
NODE arg;
Real *rp;
{
	double s;

	s = asin(ToReal(ARG0(arg)));
	MKReal(s,*rp);
}

void Pdacos(arg,rp)
NODE arg;
Real *rp;
{
	double s;

	s = acos(ToReal(ARG0(arg)));
	MKReal(s,*rp);
}

void Pdatan(arg,rp)
NODE arg;
Real *rp;
{
	double s;

	s = atan(ToReal(ARG0(arg)));
	MKReal(s,*rp);
}

void Pdlog(arg,rp)
NODE arg;
Real *rp;
{
	double s;

	s = log(ToReal(ARG0(arg)));
	MKReal(s,*rp);
}

void Pdexp(arg,rp)
NODE arg;
Real *rp;
{
	double s;

	s = exp(ToReal(ARG0(arg)));
	MKReal(s,*rp);
}

void Pdfloor(arg,rp)
NODE arg;
Q *rp;
{
	L a;
	unsigned int au,al;
	int sgn;
	Q q;
	double d;

	if ( !ARG0(arg) ) {
		*rp = 0;
		return;
	}
	d = floor(ToReal(ARG0(arg)));
	if ( d < -9.223372036854775808e18 || d >= 9.223372036854775808e18 )
		error("dfloor : OverFlow");
	a = (L)d;
	if ( a < 0 ) {
		sgn = -1;
		a = -a;
	} else
		sgn = 1;
#if defined(i386) || defined(__alpha) || defined(VISUAL)
	au = ((unsigned int *)&a)[1];
	al = ((unsigned int *)&a)[0];
#else
	al = ((unsigned int *)&a)[1];
	au = ((unsigned int *)&a)[0];
#endif
	if ( au ) {
		NEWQ(q); SGN(q) = sgn; NM(q)=NALLOC(2); DN(q)=0;
		PL(NM(q))=2; BD(NM(q))[0]=al; BD(NM(q))[1] = au;
	} else {
		UTOQ(al,q); SGN(q) = sgn;
	}
	*rp = q;
}

void Pdceil(arg,rp)
NODE arg;
Q *rp;
{
	L a;
	unsigned int au,al;
	int sgn;
	Q q;
	double d;

	if ( !ARG0(arg) ) {
		*rp = 0;
		return;
	}
	d = ceil(ToReal(ARG0(arg)));
	if ( d < -9.223372036854775808e18 || d >= 9.223372036854775808e18 )
		error("dceil : OverFlow");
	a = (L)d;
	if ( a < 0 ) {
		sgn = -1;
		a = -a;
	} else
		sgn = 1;
#if defined(i386) || defined(__alpha) || defined(VISUAL)
	au = ((unsigned int *)&a)[1];
	al = ((unsigned int *)&a)[0];
#else
	al = ((unsigned int *)&a)[1];
	au = ((unsigned int *)&a)[0];
#endif
	if ( au ) {
		NEWQ(q); SGN(q) = sgn; NM(q)=NALLOC(2); DN(q)=0;
		PL(NM(q))=2; BD(NM(q))[0]=al; BD(NM(q))[1] = au;
	} else {
		UTOQ(al,q); SGN(q) = sgn;
	}
	*rp = q;
}

void Pdrint(arg,rp)
NODE arg;
Q *rp;
{
	L a;
	unsigned int au,al;
	int sgn;
	Q q;
	double d;

	if ( !ARG0(arg) ) {
		*rp = 0;
		return;
	}
#if defined(VISUAL)
	d = ToReal(ARG0(arg));
	if ( d > 0 )
		d = floor(d+0.5);
	else
		d = ceil(d-0.5);
#else
	d = rint(ToReal(ARG0(arg)));
#endif
	if ( d < -9.223372036854775808e18 || d >= 9.223372036854775808e18 )
		error("drint : OverFlow");
	a = (L)d;
	if ( a < 0 ) {
		sgn = -1;
		a = -a;
	} else
		sgn = 1;
#if defined(i386) || defined(__alpha) || defined(VISUAL)
	au = ((unsigned int *)&a)[1];
	al = ((unsigned int *)&a)[0];
#else
	al = ((unsigned int *)&a)[1];
	au = ((unsigned int *)&a)[0];
#endif
	if ( au ) {
		NEWQ(q); SGN(q) = sgn; NM(q)=NALLOC(2); DN(q)=0;
		PL(NM(q))=2; BD(NM(q))[0]=al; BD(NM(q))[1] = au;
	} else {
		UTOQ(al,q); SGN(q) = sgn;
	}
	*rp = q;
}
