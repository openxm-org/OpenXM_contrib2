/* $OpenXM: OpenXM/src/asir99/engine/num.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

void addnum(vl,a,b,c)
VL vl;
Num a,b,*c;
{
	(*addnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b,c);
}

void subnum(vl,a,b,c)
VL vl;
Num a,b,*c;
{
	(*subnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b,c);
}

void mulnum(vl,a,b,c)
VL vl;
Num a,b,*c;
{
	(*mulnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b,c);
}

void divnum(vl,a,b,c)
VL vl;
Num a,b,*c;
{
	(*divnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b,c);
}

void pwrnum(vl,a,b,c)
VL vl;
Num a,b,*c;
{
	(*pwrnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b,c);
}

void chsgnnum(a,c)
Num a,*c;
{
	(*chsgnnumt[a?NID(a):0])(a,c);
}

int compnum(vl,a,b)
VL vl;
Num a,b;
{
	return (*cmpnumt[MAX(a?NID(a):0,b?NID(b):0)])(a,b);
}
