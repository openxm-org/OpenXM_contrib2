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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/rat.c,v 1.1.1.1 1999/12/03 07:39:07 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Pnm(), Pdn(), Pderiv();

struct ftab rat_tab[] = {
	{"nm",Pnm,1},
	{"dn",Pdn,1},
	{"diff",Pderiv,-99999999},
	{0,0,0},
};

void Pnm(arg,rp)
NODE arg;
Obj *rp;
{
	Obj t;
	Q q;

	asir_assert(ARG0(arg),O_R,"nm");
	if ( !(t = (Obj)ARG0(arg)) )
		*rp = 0;
	else
		switch ( OID(t) ) {
			case O_N: 
				switch ( NID(t) ) {
					case N_Q:
						NTOQ(NM((Q)t),SGN((Q)t),q); *rp = (Obj)q; break;
					default:
						*rp = t; break;
				}
				break;
			case O_P:
				*rp = t; break;
			case O_R:
				*rp = (Obj)NM((R)t); break;
			default:
				*rp = 0;
		}
}

void Pdn(arg,rp)
NODE arg;
Obj *rp;
{
	Obj t;
	Q q;

	asir_assert(ARG0(arg),O_R,"dn");
	if ( !(t = (Obj)ARG0(arg)) )
		*rp = (Obj)ONE;
	else
		switch ( OID(t) ) {
			case O_N: 
				switch ( NID(t) ) {
					case N_Q:
						if ( DN((Q)t) )
							NTOQ(DN((Q)t),1,q);
						else
							q = ONE;
						*rp = (Obj)q; break;
					default:
						*rp = (Obj)ONE; break;
				}
				break;
			case O_P:
				*rp = (Obj)ONE; break;
			case O_R:
				*rp = (Obj)DN((R)t); break;
			default:
				*rp = 0;
		}
}

void Pderiv(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,t;
	LIST l;
	P v;
	NODE n;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_R,"diff");
	reductr(CO,(Obj)ARG0(arg),&a);
	n = NEXT(arg);
	if ( n && (l = (LIST)ARG0(n)) && OID(l) == O_LIST )
		n = BDY(l);
	for ( ; n; n = NEXT(n) ) {
		if ( !(v = (P)BDY(n)) || OID(v) != O_P )
			error("diff : invalid argument");
		derivr(CO,a,VR(v),&t); a = t;
	}
	*rp = a;
}
