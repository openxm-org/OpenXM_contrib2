/* $OpenXM: OpenXM/src/asir99/asm/port.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
#include "base.h"

int dsab(base,a1,a2,up)
int base,a1,a2,*up;
{
	return dmab(base,a1,BASE,a2,up);
}

int dqr(a,b,qp)
int a,b,*qp;
{
	*qp = a/b;
	return a - b * (*qp);
}
