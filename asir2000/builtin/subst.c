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
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/subst.c,v 1.3 2000/08/22 05:04:00 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Psubst(), Ppsubst(), Psubstf(), Psubst_quote();

struct ftab subst_tab[] = {
	{"subst",Psubst,-99999999},
	{"subst_quote",Psubst_quote,-99999999},
	{"psubst",Ppsubst,-99999999},
	{"substf",Psubstf,-99999999},
	{0,0,0},
};

void Psubst(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,b,t;
	LIST l;
	V v;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_R,"subst");
	reductr(CO,(Obj)ARG0(arg),&a);
/*	a = (Obj)ARG0(arg); */
	arg = NEXT(arg);
	if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
		arg = BDY(l);
	while ( arg ) {
		asir_assert(BDY(arg),O_P,"subst");
		v = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( !arg )
			error("subst : invalid argument");
		asir_assert(ARG0(arg),O_R,"subst");
		reductr(CO,(Obj)BDY(arg),&b); arg = NEXT(arg);
/*		b = (Obj)BDY(arg); arg = NEXT(arg); */
		substr(CO,0,a,v,b,&t); a = t;
	}
	*rp = a;
}

FNODE subst_in_fnode();

void Psubst_quote(arg,rp)
NODE arg;
QUOTE *rp;
{
	QUOTE a,h;
	FNODE fn;
	Obj g;
	LIST l;
	V v;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_QUOTE,"subst_quote");
	fn = BDY((QUOTE)ARG0(arg)); arg = NEXT(arg);
	if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
		arg = BDY(l);
	while ( arg ) {
		asir_assert(BDY(arg),O_P,"subst_quote");
		v = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( !arg )
			error("subst_quote : invalid argument");
		g = (Obj)ARG0(arg); arg = NEXT(arg);
		if ( !g || OID(g) != O_QUOTE )
			objtoquote(g,&h);
		else
			h = (QUOTE)g;
		fn = subst_in_fnode(fn,v,BDY(h));
	}
	MKQUOTE(*rp,fn);
}

void Ppsubst(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,b,t;
	LIST l;
	V v;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_R,"psubst");
	reductr(CO,(Obj)ARG0(arg),&a);
/*	a = (Obj)ARG0(arg); */
	arg = NEXT(arg);
	if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
		arg = BDY(l);
	while ( arg ) {
		asir_assert(BDY(arg),O_P,"psubst");
		v = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( !arg )
			error("psubst : invalid argument");
		asir_assert(ARG0(arg),O_R,"psubst");
		reductr(CO,(Obj)BDY(arg),&b); arg = NEXT(arg);
/*		b = (Obj)BDY(arg); arg = NEXT(arg); */
		substr(CO,1,a,v,b,&t); a = t;
	}
	*rp = a;
}

void Psubstf(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,t;
	LIST l;
	V v,f;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_R,"substf");
	reductr(CO,(Obj)ARG0(arg),&a);
/*	a = (Obj)ARG0(arg); */
	arg = NEXT(arg);
	if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
		arg = BDY(l);
	while ( arg ) {
		asir_assert(BDY(arg),O_P,"substf");
		v = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( !arg || (int)v->attr != V_SR )
			error("substf : invalid argument");
		f = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( (int)f->attr != V_SR )
			error("substf : invalid argument\n");
		substfr(CO,a,((FUNC)v->priv)->f.puref,((FUNC)f->priv)->f.puref,&t);
		a = t;
	}
	*rp = a;
}
