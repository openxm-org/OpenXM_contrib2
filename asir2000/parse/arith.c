/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 *
 * $OpenXM: OpenXM_contrib2/asir2000/parse/arith.c,v 1.1.1.1 1999/12/03 07:39:11 noro Exp $ 
*/
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
