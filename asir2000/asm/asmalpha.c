/* $OpenXM: OpenXM_contrib2/asir2000/asm/asmalpha.c,v 1.1.1.1 1999/12/03 07:39:06 noro Exp $ */
#if defined(__alpha)
typedef unsigned long UL;
#elif defined(mips) || defined(hpux) || defined(powerpc)
typedef unsigned long long UL;
#endif

#include "base.h"

unsigned int dm(a1,a2,u)
unsigned int a1,a2,*u;
{
	UL t;

	t = (UL)a1*(UL)a2;
	*u = t>>32;
	return (unsigned int)(t&0xffffffff);
}

unsigned int dma(a1,a2,a3,u)
unsigned int a1,a2,a3,*u;
{
	UL t;

	t = ((UL)a1*(UL)a2)+(UL)a3;
	*u = t>>32;
	return (unsigned int)(t&0xffffffff);
}

unsigned int dma2(a1,a2,a3,a4,u)
unsigned int a1,a2,a3,a4,*u;
{
	UL t;

	t = (UL)a1*(UL)a2+(UL)a3+(UL)a4;
	*u = t>>32;
	return (unsigned int)(t&0xffffffff);
}

unsigned int dmb(base,a1,a2,u)
unsigned int base,a1,a2,*u;
{
	UL t;

	t = (UL)a1*(UL)a2;
	*u = t/(UL)base;
	return (unsigned int)(t-(UL)base*(UL)(*u));
}

unsigned int dmab(base,a1,a2,a3,u)
unsigned int base,a1,a2,a3,*u;
{
	UL t;

	t = (UL)a1*(UL)a2+(UL)a3;
	*u = t/(UL)base;
	return (unsigned int)(t-(UL)base*(UL)(*u));
}

unsigned int dmar(a1,a2,a3,d)
unsigned int a1,a2,a3,d;
{
	UL t;

	t = (UL)a1*(UL)a2+(UL)a3;
	return (unsigned int)(t%(UL)d);
}

unsigned int dsab(base,a1,a2,u)
unsigned int base,a1,a2,*u;
{
	UL t;

	t = (((UL)a1)<<32)+(UL)a2;
	*u = t/(UL)base;
	return (unsigned int)(t-(UL)base*(UL)(*u));
}

unsigned int dqr(a,b,qp)
unsigned int a,b,*qp;
{
	*qp = a/b;
	return a - b * (*qp);
}
