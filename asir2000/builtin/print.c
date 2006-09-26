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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/print.c,v 1.21 2006/02/01 07:29:29 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Pprint();
void Pquotetolist();
void Pobjtoquote();
void Peval_variables_in_quote();
void Pset_print_function();

struct ftab print_tab[] = {
	{"print",Pprint,-2},
	{"objtoquote",Pobjtoquote,1},
	{"quotetolist",Pquotetolist,1},
	{"eval_variables_in_quote",Peval_variables_in_quote,1},
	{"set_print_function",Pset_print_function,-1},
	{0,0,0},
};

extern int I_am_server;

int wfep_mode;

void Pprint(NODE arg,pointer *rp)
{
	Obj obj;
	STRING nl;
	Q opt;

	/* engine for wfep */
	if ( wfep_mode ) {
		if ( arg ) {
			print_to_wfep((Obj)ARG0(arg));
			if ( !NEXT(arg) || ARG1(arg) ) {
				MKSTR(nl,"\r\n");
				print_to_wfep((Obj)nl);
			}
		}
		*rp = 0;
		return;
	}
	if ( arg ) {
		obj = (Obj)ARG0(arg);
		if ( NEXT(arg) ) {
			opt = (Q)ARG1(arg);
			if ( INT(opt) ) {
				printexpr(CO,obj);
				switch ( QTOS(opt) ) {
					case 0:
						break;
					case 2:
						fflush(asir_out); break;
						break;
					case 1: default:
						putc('\n',asir_out); break;
				}
			} else
				error("print : invalid argument");
		} else {
			printexpr(CO,obj);
			putc('\n',asir_out);
		}
	}
	/* XXX : if ox_asir, we have to fflush always */
	if ( I_am_server )
		fflush(asir_out);
	*rp = 0;
}

void Pobjtoquote(NODE arg,QUOTE *rp)
{
	objtoquote(ARG0(arg),rp);
}

void Pquotetolist(NODE arg,LIST *rp)
{
	asir_assert(ARG0(arg),O_QUOTE,"quotetolist");	
	fnodetotree((FNODE)BDY((QUOTE)(ARG0(arg))),rp);
}

void Peval_variables_in_quote(NODE arg,QUOTE *rp)
{
	FNODE fn;

	asir_assert(ARG0(arg),O_QUOTE,"eval_variables_in_quote");	
	fn = eval_pvar_in_fnode((FNODE)BDY((QUOTE)(ARG0(arg))));
	MKQUOTE(*rp,fn);
}

/* fnode -> [tag,name,arg0,arg1,...] */

void fnodetotree(FNODE f,LIST *rp)
{
	LIST a1,a2,a3;
	NODE n,t,t0;
	STRING head,op,str;
	char *opname;

	if ( !f ) {
		MKSTR(head,"internal");
		n = mknode(2,head,0);
		MKLIST(*rp,n);
		return;
	}
	switch ( f->id ) {
		/* unary operators */
		case I_NOT: case I_PAREN: case I_MINUS:
			MKSTR(head,"u_op");
			switch ( f->id ) {
				case I_NOT:
					MKSTR(op,"!");
					break;
				case I_PAREN:
					MKSTR(op,"()");
					break;
				case I_MINUS:
					MKSTR(op,"-");
					break;
			}
			fnodetotree((FNODE)FA0(f),&a1);
			n = mknode(3,head,op,a1);
			MKLIST(*rp,n);
			break;

		/* binary operators */
		case I_BOP: case I_COP: case I_LOP: case I_AND: case I_OR:
			/* head */
			MKSTR(head,"b_op");

			/* arg list */
			switch ( f->id ) {
				case I_AND: case I_OR:
					fnodetotree((FNODE)FA0(f),&a1);
					fnodetotree((FNODE)FA1(f),&a2);
					break;
				default:
					fnodetotree((FNODE)FA1(f),&a1);
					fnodetotree((FNODE)FA2(f),&a2);
					break;
			}

			/* op */
			switch ( f->id ) {
				case I_BOP:
					MKSTR(op,((ARF)FA0(f))->name); break;

				case I_COP:
					switch( (cid)FA0(f) ) {
						case C_EQ: opname = "=="; break;
						case C_NE: opname = "!="; break;
						case C_GT: opname = ">"; break;
						case C_LT: opname = "<"; break;
						case C_GE: opname = ">="; break;
						case C_LE: opname = "<="; break;
					}
					MKSTR(op,opname); break;

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

						case L_NOT: opname = "@!";
							/* XXX : L_NOT is a unary operator */
							MKSTR(head,"u_op");
							MKSTR(op,opname);
							n = mknode(3,head,op,a1);
							MKLIST(*rp,n);
							return;
					}
					MKSTR(op,opname); break;

				case I_AND:
					MKSTR(op,"&&"); break;

				case I_OR:
					MKSTR(op,"||"); break;
			}
			n = mknode(4,head,op,a1,a2);
			MKLIST(*rp,n);
			break;

		case I_NARYOP: 
			/* head */
			MKSTR(head,"n_op");
			n = (NODE)FA1(f);
			for ( t0 = 0; n; n = NEXT(n) ) {
				NEXTNODE(t0,t);
				fnodetotree((FNODE)BDY(n),&a1);
				BDY(t) = (pointer)a1;
			}
			MKSTR(op,((ARF)FA0(f))->name);
			MKNODE(t,op,t0);
			MKNODE(n,head,t);
			MKLIST(*rp,n);
			break;

		/* ternary operators */
		case I_CE:
			MKSTR(head,"t_op");
			MKSTR(op,"?:");
			fnodetotree((FNODE)FA0(f),&a1);
			fnodetotree((FNODE)FA1(f),&a2);
			fnodetotree((FNODE)FA2(f),&a3);
			n = mknode(5,head,op,a1,a2,a3);
			MKLIST(*rp,n);
			break;

		/* lists */
		case I_LIST:
			n = (NODE)FA0(f);
			for ( t0 = 0; n; n = NEXT(n) ) {
				NEXTNODE(t0,t);
				fnodetotree((FNODE)BDY(n),&a1);
				BDY(t) = (pointer)a1;
			}
			if ( t0 )
				NEXT(t) = 0;
			MKSTR(head,"list");
			MKNODE(n,head,t0);
			MKLIST(*rp,n);
			break;

		/* function */
		case I_FUNC: case I_FUNC_QARG: case I_CAR: case I_CDR: case I_EV:
			MKSTR(head,"function");
			switch ( f->id ) {
				case I_FUNC: case I_FUNC_QARG:
					MKSTR(op,((FUNC)FA0(f))->fullname);
					fnodetotree((FNODE)FA1(f),&a1);
					break;
				case I_CAR:
					MKSTR(op,"car");
					fnodetotree((FNODE)FA0(f),&a1);
					break;
				case I_CDR:
					MKSTR(op,"cdr");
					fnodetotree((FNODE)FA0(f),&a1);
					break;
				case I_EV:
					/* exponent vector; should be treated as function call */
					MKSTR(op,"exponent_vector");
					fnodetotree(mkfnode(1,I_LIST,FA0(f)),&a1);
					break;
			}
			t0 = NEXT(BDY(a1)); /* XXX : skip the headers */
			MKNODE(t,op,t0);
			MKNODE(n,head,t);
			MKLIST(*rp,n);
			break;

		case I_STR:
			MKSTR(head,"internal");
			MKSTR(str,FA0(f));
			n = mknode(2,head,str);
			MKLIST(*rp,n);
			break;

		case I_FORMULA:
			MKSTR(head,"internal");
			n = mknode(2,head,FA0(f));
			MKLIST(*rp,n);
			break;

		case I_PVAR:
			if ( FA1(f) )
				error("fnodetotree : not implemented yet");
			MKSTR(head,"variable");
			GETPVNAME(FA0(f),opname);
			MKSTR(op,opname);
			n = mknode(2,head,op);
			MKLIST(*rp,n);
			break;

		default:
			error("fnodetotree : not implemented yet");
	}
}

FNODE eval_pvar_in_fnode(FNODE f)
{
	FNODE a1,a2,a3;
	pointer r;
	NODE n,t,t0;
	QUOTE q;

	if ( !f )
		return 0;

	switch ( f->id ) {
		/* unary operators */
		case I_NOT: case I_PAREN: case I_MINUS:
			a1 = eval_pvar_in_fnode((FNODE)FA0(f));
			return mkfnode(1,f->id,a1);

		/* binary operators */
		case I_AND: case I_OR:
			a1 = eval_pvar_in_fnode((FNODE)FA0(f));
			a2 = eval_pvar_in_fnode((FNODE)FA1(f));
			return mkfnode(3,f->id,a1,a2);

		case I_BOP: case I_COP: case I_LOP: 
			a1 = eval_pvar_in_fnode((FNODE)FA1(f));
			a2 = eval_pvar_in_fnode((FNODE)FA2(f));
			return mkfnode(4,f->id,FA0(f),a1,a2);

		/* ternary operators */
		case I_CE:
			a1 = eval_pvar_in_fnode((FNODE)FA0(f));
			a2 = eval_pvar_in_fnode((FNODE)FA1(f));
			a3 = eval_pvar_in_fnode((FNODE)FA2(f));
			return mkfnode(5,f->id,a1,a2,a3);

		/* lists */
		case I_LIST:
			n = (NODE)FA0(f);
			for ( t0 = 0; n; n = NEXT(n) ) {
				NEXTNODE(t0,t);
				BDY(t) = (pointer)eval_pvar_in_fnode(BDY(n));
			}
			if ( t0 )
				NEXT(t) = 0;
			return mkfnode(1,f->id,t0);

		/* function */
		case I_FUNC:
			a1 = eval_pvar_in_fnode((FNODE)FA1(f));
			return mkfnode(2,f->id,FA0(f),a1);
			break;
		case I_CAR: case I_CDR:
			a1 = eval_pvar_in_fnode((FNODE)FA0(f));
			return mkfnode(1,f->id,a1);
		case I_EV:
			/* exponent vector */
			a1 = eval_pvar_in_fnode(mkfnode(1,I_LIST,FA0(f)));
			return mkfnode(1,f->id,a1);

		case I_STR: case I_FORMULA:
			return f;

		case I_PVAR: case I_INDEX:
		case I_POSTSELF: case I_PRESELF:
			r = eval(f);
			objtoquote(r,&q);
			return BDY(q);

		default:
			error("eval_pvar_in_fnode : not implemented yet");
			/* NOTREACHED */
			return 0;
	}
}

FNODE subst_in_fnode(FNODE f,V v,FNODE g)
{
	FNODE a1,a2,a3;
	DCP dc;
	V vf;
	NODE n,t,t0;
	Obj obj;

	if ( !f )
		return 0;

	switch ( f->id ) {
		/* unary operators */
		case I_NOT: case I_PAREN: case I_MINUS:
			a1 = subst_in_fnode((FNODE)FA0(f),v,g);
			return mkfnode(1,f->id,a1);

		/* binary operators */
		case I_AND: case I_OR:
			a1 = subst_in_fnode((FNODE)FA0(f),v,g);
			a2 = subst_in_fnode((FNODE)FA1(f),v,g);
			return mkfnode(3,f->id,a1,a2);

		case I_BOP: case I_COP: case I_LOP: 
			a1 = subst_in_fnode((FNODE)FA1(f),v,g);
			a2 = subst_in_fnode((FNODE)FA2(f),v,g);
			return mkfnode(4,f->id,FA0(f),a1,a2);

		/* ternary operators */
		case I_CE:
			a1 = subst_in_fnode((FNODE)FA0(f),v,g);
			a2 = subst_in_fnode((FNODE)FA1(f),v,g);
			a3 = subst_in_fnode((FNODE)FA2(f),v,g);
			return mkfnode(5,f->id,a1,a2,a3);

		/* lists */
		case I_LIST:
			n = (NODE)FA0(f);
			for ( t0 = 0; n; n = NEXT(n) ) {
				NEXTNODE(t0,t);
				BDY(t) = (pointer)subst_in_fnode(BDY(n),v,g);
			}
			if ( t0 )
				NEXT(t) = 0;
			return mkfnode(1,f->id,t0);

		/* function */
		case I_FUNC:
			a1 = subst_in_fnode((FNODE)FA1(f),v,g);
			return mkfnode(2,f->id,FA0(f),a1);
			break;
		case I_CAR: case I_CDR:
			a1 = subst_in_fnode((FNODE)FA0(f),v,g);
			return mkfnode(1,f->id,a1);
		case I_EV:
			/* exponent vector */
			a1 = subst_in_fnode(mkfnode(1,I_LIST,FA0(f)),v,g);
			return mkfnode(1,f->id,a1);

		case I_STR:
			return f;

		case I_FORMULA:
			obj = (Obj)FA0(f);
			if ( !obj )
				return f;

			switch ( OID(obj) ) {
				case O_N:
					return f;
				case O_P:
					vf = VR((P)obj);
					dc = DC((P)obj);
					if ( vf != v )
						return f;
					else if ( UNIQ(DEG(dc)) && UNIQ((Q)COEF(dc)) )
						return g;	
					else break;
				default:
					break;
			}

		default:
			error("subst_in_fnode : not implemented yet");
			/* NOTREACHED */
			return 0;
	}
}

/* not completed yet */

#if 0
char *get_attribute(char *key,LIST attr)
{}

void treetofnode(Obj obj,FNODE *f)
{
	NODE n;
	LIST attr;
	char *prop;

	if ( obj || OID(obj) != O_LIST ) {
		/* internal object */
		*f = mkfnode(1,I_FORMULA,obj);
	} else {
		/* [attr(list),name(string),args(node)] */
		n = BDY((LIST)obj);
		attr = (LIST)BDY(n); n = NEXT(n);
		prop = get_attribute("asir",attr);
		if ( !strcmp(prop,"u_op") ) {
		} else if ( !strcmp(prop,"b_op") ) {
		} else if ( !strcmp(prop,"t_op") ) {
		} else if ( !strcmp(prop,"function") ) {
		}
			/* default will be set to P_FUNC */
	}
}
#endif

FUNC user_print_function;

void Pset_print_function(NODE arg,pointer *rp)
{
	if ( !arg )
		user_print_function = 0;		
	else {
		gen_searchf(BDY((STRING)ARG0(arg)),&user_print_function);
	}
	*rp = 0;
}
