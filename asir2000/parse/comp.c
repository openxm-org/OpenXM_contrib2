/* $OpenXM: OpenXM/src/asir99/parse/comp.c,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "comp.h"
#if defined(THINK_C) || defined(VISUAL)
#include <stdarg.h>
#else
#include <varargs.h>
#endif

extern f_return;

#if defined(THINK_C) || defined(VISUAL)
void call_usrf(FUNC f,...)
{
	va_list ap;
	int ac,i;
	pointer *c;
	NODE tn;

	va_start(ap,f);
	if ( !f )
		notdef(0,0,0,0);
	else {
		pushpvs(f);
		ac = va_arg(ap,int);
		for ( i = 0, tn = f->f.usrf->args; i < ac;
			i++, tn = NEXT(tn) )
			ASSPV((int)FA0((FNODE)BDY(tn)),va_arg(ap,pointer));
		c = va_arg(ap,pointer *); *c = evalstat(BDY(f->f.usrf));
		va_end(ap);
		f_return = 0; poppvs();
	}
}
#else
void call_usrf(va_alist)
va_dcl
{
	va_list ap;
	int ac,i;
	FUNC f;
	pointer a,b,*c;
	NODE tn;

	va_start(ap); f = va_arg(ap,FUNC);
	if ( !f )
		notdef(0,0,0,0);
	else {
		pushpvs(f);
		ac = va_arg(ap,int);
		for ( i = 0, tn = f->f.usrf->args; i < ac;
			i++, tn = NEXT(tn) )
			ASSPV((int)FA0((FNODE)BDY(tn)),va_arg(ap,pointer));
		c = va_arg(ap,pointer *); *c = evalstat(BDY(f->f.usrf));
		f_return = 0; poppvs();
	}
}
#endif

void addcomp(vl,a,b,c)
VL vl;
COMP a,b,*c;
{
	if ( a->type != b->type )
		error("addcomp : types different");
	else
		call_usrf(LSS->sa[a->type].arf.add,2,a,b,c);
}

void subcomp(vl,a,b,c)
VL vl;
COMP a,b,*c;
{
	if ( a->type != b->type )
		error("subcomp : types different");
	else
		call_usrf(LSS->sa[a->type].arf.sub,2,a,b,c);
}

void mulcomp(vl,a,b,c)
VL vl;
COMP a,b,*c;
{
	if ( a->type != b->type )
		error("mulcomp : types different");
	else
		call_usrf(LSS->sa[a->type].arf.mul,2,a,b,c);
}	

void divcomp(vl,a,b,c)
VL vl;
COMP a,b,*c;
{
	if ( a->type != b->type )
		error("divcomp : types different");
	else
		call_usrf(LSS->sa[a->type].arf.div,2,a,b,c);
}	

void chsgncomp(a,b)
COMP a,*b;
{
	call_usrf(LSS->sa[a->type].arf.chsgn,1,a,b);
}

void pwrcomp(vl,a,r,c)
VL vl;
COMP a;
Obj r;
COMP *c;
{
	call_usrf(LSS->sa[a->type].arf.pwr,2,a,r,c);
}

int compcomp(vl,a,b)
VL vl;
COMP a,b;
{
	Q c;
	int s;

	if ( a->type > b->type )
		return 1;
	else if ( a->type < b->type )
		return -1;
	else {
		call_usrf(LSS->sa[a->type].arf.comp,2,a,b,&c);
		s = QTOS(c);
		if ( s > 0 )
			return 1;
		else if ( s < 0 )
			return -1;
		else
			return 0;
	}
}
