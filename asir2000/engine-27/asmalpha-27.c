/* $OpenXM: OpenXM/src/asir99/engine-27/asmalpha-27.c,v 1.1.1.1 1999/11/10 08:12:27 noro Exp $ */
#if defined(__alpha)
typedef unsigned long UL;
#elif defined(mips) || defined(hpux)
typedef unsigned long long UL;
#endif

#include "base.h"

unsigned int dm_27(a1,a2,u)
unsigned int a1,a2,*u;
{
	UL t;

	t = (UL)a1*(UL)a2;
	*u = t>>27;
	return (unsigned int)(t&0x7ffffff);
}

unsigned int dma_27(a1,a2,a3,u)
unsigned int a1,a2,a3,*u;
{
	UL t;

	t = (UL)a1*(UL)a2+(UL)a3;
	*u = t>>27;
	return (unsigned int)(t&0x7ffffff);
}

unsigned int dmb_27(base,a1,a2,u)
unsigned int base,a1,a2,*u;
{
	UL t;

	t = (UL)a1*(UL)a2;
	*u = t/(UL)base;
	return (unsigned int)(t%(UL)base);
}

unsigned int dmab_27(base,a1,a2,a3,u)
unsigned int base,a1,a2,a3,*u;
{
	UL t;

	t = (UL)a1*(UL)a2+(UL)a3;
	*u = t/(UL)base;
	return (unsigned int)(t%(UL)base);
}

unsigned int dmar_27(a1,a2,a3,d)
unsigned int a1,a2,a3,d;
{
	UL t;

	t = (UL)a1*(UL)a2+(UL)a3;
	return (unsigned int)(t%(UL)d);
}

unsigned int dsab_27(base,a1,a2,u)
unsigned int base,a1,a2,*u;
{
	UL t;

	t = (((UL)a1)<<27)+(UL)a2;
	*u = t/(UL)base;
	return (unsigned int)(t%(UL)base);
}

unsigned int dqr_27(a,b,qp)
unsigned int a,b,*qp;
{
	*qp = a/b;
	return a - b * (*qp);
}
