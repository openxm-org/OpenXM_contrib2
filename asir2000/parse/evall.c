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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/evall.c,v 1.4 2000/12/05 01:24:56 noro Exp $ 
*/
#include <ctype.h>
#include "ca.h"
#include "parse.h"
#include "base.h"
#include "al.h"
#include <sys/types.h>
#include <sys/stat.h>

pointer evall(lid id,Obj a1,Obj a2)
{
	F f;
	Obj d;
	oFOP op;
	NODE n1,n2;

	switch ( id ) {
		case L_EQ:
			op = AL_EQUAL; break;
		case L_NE:
			op = AL_NEQ; break;
		case L_GT:
			op = AL_GREATERP; break;
		case L_LT:
			op = AL_LESSP; break;
		case L_GE:
			op = AL_GEQ; break;
		case L_LE:
			op = AL_LEQ; break;
		case L_AND:
			op = AL_AND; break;
		case L_OR:
			op = AL_OR; break;
		case L_NOT:
			op = AL_NOT; break;
		case L_IMPL:
			op = AL_IMPL; break;
		case L_REPL:
			op = AL_REPL; break;
		case L_EQUIV:
			op = AL_EQUIV; break;
		default:
			error("evall : unknown id");
	}
	if ( AL_ATOMIC(op) ) {
		arf_sub(CO,a1,a2,&d);
		MKAF(f,op,(P)d);
	} else if ( AL_JUNCT(op) ) {
		if ( FOP((F)a1) == op ) {
			if ( FOP((F)a2) == op )
				n2 = FJARG((F)a2);
			else
				MKNODE(n2,(F)a2,0);
			node_concat_dup(FJARG((F)a1),n2,&n1);
		} else if ( FOP((F)a2) == op )
			MKNODE(n1,a1,FJARG((F)a2));
		else {
			MKNODE(n2,a2,0); MKNODE(n1,a1,n2);
		}
		MKJF(f,op,n1);
	} else if ( AL_UNI(op) )
		MKUF(f,op,(F)a1);
	else if ( AL_EXT(op) )
		MKBF(f,op,(F)a1,(F)a2);
	return (pointer)f;
}

void node_concat_dup(NODE n1,NODE n2,NODE *nr)
{
	NODE r0,r,n;

	for ( r0 = 0, n = n1; n; n = NEXT(n) ) {
		NEXTNODE(r0,r); BDY(r) = BDY(n);
	}
	if ( !r0 )
		*nr = n2;
	else {
		NEXT(r) = n2; *nr = r0;
	}
}
