/* $OpenXM: OpenXM/src/asir99/parse/arith.c,v 1.2 1999/11/18 05:42:02 noro Exp $ */
#include "ca.h"
#include "parse.h"

struct oAFUNC {
	void (*add)();
	void (*sub)();
	void (*mul)();
	void (*div)();
	void (*pwr)();
	void (*chsgn)();
	int (*comp)();
};

struct oARF arf[6];
ARF addfs, subfs, mulfs, divfs, remfs, pwrfs;

void divsdc();

struct oAFUNC afunc[] = {
/* ??? */	{0,0,0,0,0,0,0},
/* O_N */	{addnum,subnum,mulnum,divnum,pwrnum,chsgnnum,compnum},
/* O_P */	{addp,subp,mulp,divr,pwrp,chsgnp,compp},
/* O_R */	{addr,subr,mulr,divr,pwrr,chsgnr,compr},
/* O_LIST */	{notdef,notdef,notdef,notdef,notdef,notdef,complist},
/* O_VECT */	{addvect,subvect,mulvect,divvect,notdef,chsgnvect,compvect},
/* O_MAT */	{addmat,submat,mulmat,divmat,pwrmat,chsgnmat,compmat},
/* O_STR */	{addstr,notdef,notdef,notdef,notdef,notdef,compstr},
/* O_COMP */	{addcomp,subcomp,mulcomp,divcomp,pwrcomp,chsgncomp,compcomp},
/* O_DP */   {addd,subd,muld,divsdc,notdef,chsgnd,compd},
/* O_UI */	{notdef,notdef,notdef,notdef,notdef,notdef,compui},
/* O_GF2MAT */	{notdef,notdef,notdef,notdef,notdef,notdef,(int(*)())notdef},
/* O_ERR */	{notdef,notdef,notdef,notdef,notdef,notdef,(int(*)())notdef},
/* O_GFMMAT */	{notdef,notdef,notdef,notdef,notdef,notdef,(int(*)())notdef},
};

void arf_init() {
	addfs = &arf[0]; addfs->name = "+"; addfs->fp = arf_add;
	subfs = &arf[1]; subfs->name = "-"; subfs->fp = arf_sub;
	mulfs = &arf[2]; mulfs->name = "*"; mulfs->fp = arf_mul;
	divfs = &arf[3]; divfs->name = "/"; divfs->fp = arf_div;
	remfs = &arf[4]; remfs->name = "%"; remfs->fp = arf_remain;
	pwrfs = &arf[5]; pwrfs->name = "^"; pwrfs->fp = arf_pwr;
}

void arf_add(vl,a,b,r)
VL vl;
Obj a,b,*r;
{
	int mid;

	if ( !a )
		*r = b;
	else if ( !b )
		*r = a;
	else if ( OID(a) == OID(b) )
		(*afunc[OID(a)].add)(vl,a,b,r);
	else if ( (mid = MAX(OID(a),OID(b))) <= O_R )
		(*afunc[mid].add)(vl,a,b,r);
	else
		notdef(vl,a,b,r);
}

void arf_sub(vl,a,b,r)
VL vl;
Obj a,b,*r;
{
	int mid;

	if ( !a )
		if ( !b )
			*r = 0;
		else
			(*afunc[OID(b)].chsgn)(b,r);
	else if ( !b )
		*r = a;
	else if ( OID(a) == OID(b) )
		(*afunc[OID(a)].sub)(vl,a,b,r);
	else if ( (mid = MAX(OID(a),OID(b))) <= O_R )
		(*afunc[mid].sub)(vl,a,b,r);
	else
		notdef(vl,a,b,r);
}

void arf_mul(vl,a,b,r)
VL vl;
Obj a,b,*r;
{
	int mid;

	if ( !a || !b )
		*r = 0;
	else if ( OID(a) == OID(b) )
		(*(afunc[OID(a)].mul))(vl,a,b,r);
	else if ( (mid = MAX(OID(a),OID(b))) <= O_R || 
		(mid == O_MAT) || (mid == O_VECT) || (mid == O_DP) )
		(*afunc[mid].mul)(vl,a,b,r);
	else
		notdef(vl,a,b,r);
}

void arf_div(vl,a,b,r)
VL vl;
Obj a,b,*r;
{
	int mid;

	if ( !b )
		error("div : division by 0");
	if ( !a )
		*r = 0;
	else if ( (OID(a) == OID(b)) )
		(*(afunc[OID(a)].div))(vl,a,b,r);
	else if ( (mid = MAX(OID(a),OID(b))) <= O_R || 
		(mid == O_MAT) || (mid == O_VECT) || (mid == O_DP) )
		(*afunc[mid].div)(vl,a,b,r);
	else
		notdef(vl,a,b,r);
}

void arf_remain(vl,a,b,r)
VL vl;
Obj a,b,*r;
{
	if ( !b )
		error("rem : division by 0");
	else if ( !a )
		*r = 0;
	else if ( MAX(OID(a),OID(b)) <= O_P )
		cmp((Q)b,(P)a,(P *)r);
	else
		notdef(vl,a,b,r);
}

void arf_pwr(vl,a,e,r)
VL vl;
Obj a,e,*r;
{
	R t;

	if ( !a )
		*r = 0;
	else if ( !e )
		*r = (pointer)ONE;
	else if ( (OID(e) <= O_N) && INT(e) ) {
		if ( (OID(a) == O_P) && (SGN((Q)e) < 0) ) {
			MKRAT((P)a,(P)ONE,1,t);
			(*(afunc[O_R].pwr))(vl,t,e,r);
		} else
			(*(afunc[OID(a)].pwr))(vl,a,e,r);
	} else if ( OID(a) <= O_R )
		mkpow(vl,a,e,r);
	else
		notdef(vl,a,e,r);
}

void arf_chsgn(a,r)
Obj a,*r;
{
	if ( !a )
		*r = 0;
	else
		(*(afunc[OID(a)].chsgn))(a,r);
}

int arf_comp(vl,a,b)
VL vl;
Obj a,b;
{
	if ( !a )
		if ( !b )
			return 0;
		else
			return (*afunc[OID(b)].comp)(vl,a,b);
	else if ( !b )
			return (*afunc[OID(a)].comp)(vl,a,b);
	else if ( OID(a) != OID(b) )
		return OID(a)>OID(b) ? 1 : -1;
	else
		return (*afunc[OID(a)].comp)(vl,a,b);
}

int complist(vl,a,b)
VL vl;
LIST a,b;
{
	int i,t;
	NODE an,bn;

	if ( !a )
		if ( !b )
			return 0;
		else
			return -1;
	else if ( !b )
		return 1;
	for ( i = 0, an = BDY(a); an; i++, an = NEXT(an) );
	for ( an = BDY(b); an; i--, an = NEXT(an) );
	if ( i )
		return i > 0 ? 1 : -1;
	for ( an = BDY(a), bn = BDY(b); an; an = NEXT(an), bn = NEXT(bn) )
		if ( t = arf_comp(vl,BDY(an),BDY(bn)) )
			return t;
	return 0;
}
