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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/print.c,v 1.4 2001/08/06 01:48:32 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Pprint();
void Pquotetolist();

struct ftab print_tab[] = {
	{"print",Pprint,-2},
	{"quotetolist",Pquotetolist,1},
	{0,0,0},
};

void Pprint(arg,rp)
NODE arg;
pointer *rp;
{
	printexpr(CO,ARG0(arg)); 
	if ( argc(arg) == 2 )
		switch ( QTOS((Q)ARG1(arg)) ) {
			case 0:
				break;
			case 2:
				fflush(asir_out); break;
				break;
			case 1: default:
				putc('\n',asir_out); break;
		}
	else
		putc('\n',asir_out);
	*rp = 0;
}

void fnodetotree();

void Pquotetolist(arg,rp)
NODE arg;
Obj *rp;
{
	asir_assert(ARG0(arg),O_QUOTE,"quotetolist");	
	fnodetotree((FNODE)BDY((QUOTE)(ARG0(arg))),rp);
}

void fnodetotree(f,rp)
FNODE f;
Obj *rp;
{
	Obj a1,a2;
	NODE n,t,t0;
	STRING head;
	LIST r,arg;
	char *opname;

	if ( !f ) {
		*rp = 0;
		return;
	}
	switch ( f->id ) {
		case I_BOP: case I_COP: I_LOP:
			/* arg list */
			fnodetotree((FNODE)FA1(f),&a1);
			fnodetotree((FNODE)FA2(f),&a2);
			n = mknode(2,a1,a2); MKLIST(arg,n);

			/* head */
			switch ( f->id ) {
				case I_BOP:
					MKSTR(head,((ARF)FA0(f))->name); break;
				case I_COP:
					switch( (cid)FA0(f) ) {
						case C_EQ: opname = "=="; break;
						case C_NE: opname = "!="; break;
						case C_GT: opname = ">"; break;
						case C_LT: opname = "<"; break;
						case C_GE: opname = ">="; break;
						case C_LE: opname = "<="; break;
					}
					MKSTR(head,opname); break;
				case I_LOP:
					switch( (lid)FA0(f) ) {
						case L_EQ: opname = "@=="; break;
						case L_NE: opname = "@!="; break;
						case L_GT: opname = "@>"; break;
						case L_LT: opname = "@<"; break;
						case L_GE: opname = "@>="; break;
						case L_LE: opname = "@<="; break;
						case L_AND: opname = "@&&"; break;
						case L_OR: opname = "@||"; break;
						case L_NOT: opname = "@!"; break;
					}
					MKSTR(head,opname); break;
			}
			n = mknode(2,head,arg);
			MKLIST(r,n);
			*rp = (Obj)r;
			break;
		case I_AND:
			fnodetotree((FNODE)FA0(f),&a1);
			fnodetotree((FNODE)FA1(f),&a2);
			n = mknode(2,a1,a2); MKLIST(arg,n);
			MKSTR(head,"&&");
			n = mknode(2,head,arg);
			MKLIST(r,n);
			*rp = (Obj)r;
			break;
		case I_OR:
			fnodetotree((FNODE)FA0(f),&a1);
			fnodetotree((FNODE)FA1(f),&a2);
			n = mknode(2,a1,a2); MKLIST(arg,n);
			MKSTR(head,"||");
			n = mknode(2,head,arg);
			MKLIST(r,n);
			*rp = (Obj)r;
			break;
		case I_NOT:
			fnodetotree((FNODE)FA0(f),&a1);
			n = mknode(1,a1); MKLIST(arg,n);
			MKSTR(head,"!");
			n = mknode(2,head,arg);
			MKLIST(r,n);
			*rp = (Obj)r;
			break;
		case I_CE:
			fnodetotree((FNODE)FA0(f),&a1);
			fnodetotree((FNODE)FA1(f),&a2);
			n = mknode(2,a1,a2); MKLIST(arg,n);
			MKSTR(head,"?:");
			n = mknode(2,head,arg);
			MKLIST(r,n);
			*rp = (Obj)r;
			break;
		case I_EV: case I_LIST:
			n = (NODE)FA0(f);
			for ( t0 = 0; n; n = NEXT(n) ) {
				NEXTNODE(t0,t);
				fnodetotree(BDY(n),&BDY(t));
			}
			if ( t0 )
				NEXT(t) = 0;
			MKLIST(arg,t0);
			switch ( f->id ) {
				case I_LIST:
					*rp = (Obj)arg; break;
				case I_EV:
					MKSTR(head,"exponent_vector");
					n = mknode(2,head,arg);
					MKLIST(r,n);
					*rp = (Obj)r;
					break;
			}
			break;
		case I_FUNC:
			fnodetotree((FNODE)FA1(f),&arg);
			MKSTR(head,((FUNC)FA0(f))->name);
			n = mknode(2,head,arg);
			MKLIST(r,n);
			*rp = (Obj)r;
			break;
		case I_CAR:
			fnodetotree((FNODE)FA0(f),&arg);
			MKSTR(head,"car");
			n = mknode(2,head,arg);
			MKLIST(r,n);
			*rp = (Obj)r;
			break;
		case I_CDR:
			fnodetotree((FNODE)FA0(f),&arg);
			MKSTR(head,"cdr");
			n = mknode(2,head,arg);
			MKLIST(r,n);
			*rp = (Obj)r;
			break;
		case I_PAREN:
			fnodetotree((FNODE)FA0(f),&arg);
			MKSTR(head,"()");
			n = mknode(2,head,arg);
			MKLIST(r,n);
			*rp = (Obj)r;
			break;
		case I_FORMULA:
			*rp = (Obj)FA0(f);
			break;
		default:
			error("fnodetotree : not implemented yet");
	}
}
