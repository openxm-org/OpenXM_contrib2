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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/list.c,v 1.6 2003/01/20 17:44:51 saito Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Pcar(), Pcdr(), Pcons(), Pappend(), Preverse(), Plength();
void Pnconc(), Preplcd(), Preplca();

struct ftab list_tab[] = {
	{"car",Pcar,1},
	{"cdr",Pcdr,1},
	{"cons",Pcons,2},
	{"append",Pappend,2},
	{"reverse",Preverse,1},
	{"length",Plength,1},
	{"nconc",Pnconc,2},
	{"replcd",Preplcd,2},
	{"replca",Preplca,2},
	{0,0,0},
};

void Pcar(arg,rp)
NODE arg;
pointer *rp;
{
	asir_assert(ARG0(arg),O_LIST,"car");
	if ( !BDY((LIST)ARG0(arg)) )
		*rp = ARG0(arg);
	else
		*rp = (pointer)BDY(BDY((LIST)ARG0(arg)));
}

void Pcdr(arg,rp)
NODE arg;
LIST *rp;
{
	asir_assert(ARG0(arg),O_LIST,"cdr");
	if ( !BDY((LIST)ARG0(arg)) )
		*rp = (LIST)ARG0(arg);
	else 
		MKLIST(*rp,NEXT(BDY((LIST)ARG0(arg))));
}

void Pcons(arg,rp)
NODE arg;
LIST *rp;
{
	NODE t;

	asir_assert(ARG1(arg),O_LIST,"cons");
	MKNODE(t,ARG0(arg),BDY((LIST)ARG1(arg))); MKLIST(*rp,t);
}

void Pappend(arg,rp)
NODE arg;
LIST *rp;
{
	NODE t,t0,n;

	asir_assert(ARG0(arg),O_LIST,"append");
	asir_assert(ARG1(arg),O_LIST,"append");
	if ( !(n = BDY((LIST)ARG0(arg))) )
		*rp = (LIST)ARG1(arg);
	else {
		for ( t0 = 0; n; n = NEXT(n) ) {
			NEXTNODE(t0,t); BDY(t) = BDY(n);
		}
		NEXT(t) = BDY((LIST)ARG1(arg));
		MKLIST(*rp,t0);
	}
}

void Preverse(arg,rp)
NODE arg;
LIST *rp;
{
	NODE t,t1,n;

	asir_assert(ARG0(arg),O_LIST,"reverse");
	if ( !(n = BDY((LIST)ARG0(arg))) )
		*rp = (LIST)ARG0(arg);
	else {
		for ( t = 0; n; n = NEXT(n) ) {
			MKNODE(t1,BDY(n),t); t = t1;
		}
		MKLIST(*rp,t);
	}
}

#if 0
void Plength(arg,rp)
NODE arg;
Q *rp;
{
	NODE n;
	int i;

	asir_assert(ARG0(arg),O_LIST,"length");
	n = BDY((LIST)ARG0(arg));
	for ( i = 0; n; i++, n = NEXT(n) );
	STOQ(i,*rp);
}
#endif

void Plength(arg,rp)
NODE arg;
Q *rp;
{
    NODE n;
    int i;

    switch (OID(ARG0(arg))) {
    case O_VECT:
        i = ((VECT)ARG0(arg))->len;
        break;
    case O_LIST:
        n = BDY((LIST)ARG0(arg));
        for ( i = 0; n; i++, n = NEXT(n) );
        break;
    default:
        error("length : invalid argument"); break;
    }
    STOQ(i,*rp);
}

void Pnconc(arg,rp)
NODE arg;
LIST *rp;
{
	NODE a1,a2,n;

	asir_assert(ARG0(arg),O_LIST,"nconc");
	asir_assert(ARG1(arg),O_LIST,"nconc");
	a1 = BDY((LIST)ARG0(arg));
	if ( !a1 )
		*rp = (LIST)ARG1(arg);
	else {
		for ( n = a1; n; n = NEXT(n) ) {
			a2 = n;
		}
		NEXT(a2) = BDY((LIST)ARG1(arg));
		*rp = (LIST)ARG0(arg);
	}
}

void Preplcd(arg,rp)
NODE arg;
LIST *rp;
{
	NODE a0,a1;
	
	asir_assert(ARG0(arg),O_LIST,"replcd");
	asir_assert(ARG1(arg),O_LIST,"replcd");
	a0 = BDY((LIST)ARG0(arg));
	if ( !a0 )
		*rp = (LIST)ARG0(arg);
	else {
		NEXT(a0) = BDY((LIST)ARG1(arg));
		*rp = (LIST)ARG0(arg);
	}
}

void Preplca(arg,rp)
NODE arg;
LIST *rp;
{
	NODE n;
	
	asir_assert(ARG0(arg),O_LIST,"replca");
	if ( !(n = BDY((LIST)ARG0(arg)) ) )
		*rp = (LIST)ARG0(arg);
	else {
		BDY(BDY((LIST)ARG0(arg))) = (LIST)ARG1(arg);
		*rp = (LIST)ARG0(arg);
	}
}
