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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/compile.c,v 1.1.1.1 1999/12/03 07:39:12 noro Exp $ 
*/
#include <ctype.h>
#include "ca.h"
#include "base.h"
#include "parse.h"
#if !defined(THINK_C)
#include <sys/types.h>
#include <sys/stat.h>
#endif

extern jmp_buf timer_env;

pointer bcompilef(), compilemapf();

pointer compile(f)
FNODE f;
{
	LIST t;
	STRING str;
	pointer val = 0;
	pointer a,a1,a2;
	NODE tn,ind;
	R s,u;
	DP dp;
	int pv,c;
	FNODE f1;
	Obj expired;
	int interval;

	if ( !f )
		return ( 0 );
	switch ( f->id ) {
		case I_BOP:
			a1 = compile((FNODE)FA1(f)); a2 = compile((FNODE)FA2(f));
			val = emit_bop(((ARF)FA0(f)),a1,a2);
		break;
		case I_COP:
			a1 = compile((FNODE)FA1(f)); a2 = compile((FNODE)FA2(f));
			val = emit_cop((cid)FA0(f),a1,a2);
			break;
		case I_AND:
			a1 = compile((FNODE)FA0(f)); a2 = compile((FNODE)FA1(f));
			val = emit_and(a1,a2);
			break;
		case I_OR:
			a1 = compile((FNODE)FA0(f)); a2 = compile((FNODE)FA1(f));
			val = emit_or(a1,a2);
			break;
		case I_NOT:
			a1 = compile((FNODE)FA0(f));
			val = emit_not(a1);
			break;
		case I_CE:
			a1 = compile((FNODE)FA0(f)); a2 = compile((FNODE)FA1(f));
			a3 = compile((FNODE)FA0(f));
			emit_ce(a1,a2,a3);
			break;
		case I_FUNC:
			val = compilef((FUNC)FA0(f),(FNODE)FA1(f)); break;
		case I_PVAR:
			pv = (int)FA0(f); ind = (NODE)FA1(f); GETPV(pv,a); 
			if ( !ind )
				val = a;
			else {
				compilenodebody(ind,&tn); getarray(a,tn,&val); 
			}
			break;
		case I_ASSPVAR:
			f1 = (FNODE)FA0(f);
			if ( ID(f1) == I_PVAR ) {
				pv = (int)FA0(f1); ind = (NODE)FA1(f1);
				if ( !ind ) {
					val = compile((FNODE)FA1(f)); ASSPV(pv,val);
				} else {
					GETPV(pv,a);
					compilenodebody(ind,&tn); 
					putarray(a,tn,val = compile((FNODE)FA1(f))); 
				}
			} else
				val = compilepv(ID(f),(FNODE)FA0(f),FA1(f));
			break;
		case I_PRESELF:
			f1 = (FNODE)FA1(f);
			if ( ID(f1) == I_PVAR ) {
				pv = (int)FA0(f1); ind = (NODE)FA1(f1); GETPV(pv,a); 
				if ( !ind ) {
					(*((ARF)FA0(f))->fp)(CO,a,ONE,&val); ASSPV(pv,val); 
				} else if ( a ) {
					compilenodebody(ind,&tn); getarray(a,tn,(pointer *)&u); 
					(*((ARF)FA0(f))->fp)(CO,u,ONE,&val); putarray(a,tn,val);
				}
			} else
				val = compilepv(f->id,(FNODE)FA1(f),FA0(f));
			break;
		case I_POSTSELF:
			f1 = (FNODE)FA1(f);
			if ( ID(f1) == I_PVAR ) {
				pv = (int)FA0(f1); ind = (NODE)FA1(f1); GETPV(pv,val); 
				if ( !ind ) {
					(*((ARF)FA0(f))->fp)(CO,val,ONE,&u); ASSPV(pv,u);
				} else if ( val ) {
					compilenodebody(ind,&tn); getarray(val,tn,&a);
					(*((ARF)FA0(f))->fp)(CO,a,ONE,&u); putarray(val,tn,(pointer)u);
					val = a;
				}
			} else
				val = compilepv(f->id,(FNODE)FA1(f),FA0(f));
			break;
		case I_STR:
			MKSTR(str,FA0(f)); val = (pointer)str; break;
		case I_FORMULA:
			 val = FA0(f); break;
		case I_LIST:
			compilenodebody((NODE)FA0(f),&tn); MKLIST(t,tn); val = (pointer)t; break;
		case I_INDEX:
			a = compile((FNODE)FA0(f)); ind = (NODE)FA1(f);
			compilenodebody(ind,&tn); getarray(a,tn,&val); 
			break;
		default:
			fprintf(stderr,"compile: unknown id");
			error("");
			break;
	}
	return ( val );
}

pointer compilestat(f)
SNODE f;
{
	pointer val = 0,t,s,s1,u;
	NODE tn;
	int i,ac;
	V *a;
	char *buf;

	if ( !f )
		return ( 0 );
	if ( nextbp && nextbplevel <= 0 && f->id != S_CPLX ) {
		nextbp = 0;
		bp(f);
	}
	compilestatline = f->ln;

	switch ( f->id ) {
		case S_BP:
			if ( !nextbp && (!FA1(f) || compile((FNODE)FA1(f))) ) {
				if ( (FNODE)FA2(f) ) {
#if PARI
					extern FILE *outfile;
					outfile = stderr;
#endif
					asir_out = stderr;
					printexpr(CO,compile((FNODE)FA2(f)));
					putc('\n',asir_out); fflush(asir_out);
#if PARI
					outfile = stdout;
#endif
					asir_out = stdout;
				} else {
					nextbp = 1; nextbplevel = 0; 
				}
			}
			val = compilestat((SNODE)FA0(f)); 
			break;
		case S_PFDEF:
			ac = argc(FA1(f)); a = (V *)MALLOC(ac*sizeof(V));
			s = compile((FNODE)FA2(f));
			buf = (char *)ALLOCA(BUFSIZ);
			for ( i = 0, tn = (NODE)FA1(f); tn; tn = NEXT(tn), i++ ) {
				t = compile((FNODE)tn->body); sprintf(buf,"_%s",NAME(VR((P)t)));
				makevar(buf,&u); a[i] = VR((P)u);
				substr(CO,0,(Obj)s,VR((P)t),(Obj)u,(Obj *)&s1); s = s1;
			}
			mkpf((char *)FA0(f),(Obj)s,ac,a,0,0,(PF *)&val); val = 0; break;
		case S_SINGLE:
			val = compile((FNODE)FA0(f)); break;
		case S_CPLX:
			for ( tn = (NODE)FA0(f); tn; tn = NEXT(tn) ) {
				if ( BDY(tn) )
					val = compilestat((SNODE)BDY(tn));
				if ( f_break || f_return || f_continue )
					break;
			}
			break;
		case S_BREAK:
			if ( GPVS != CPVS )
				f_break = 1; 
			break;
		case S_CONTINUE:
			if ( GPVS != CPVS )
				f_continue = 1; 
			break;
		case S_RETURN:
			if ( GPVS != CPVS ) {
				val = compile((FNODE)FA0(f)); f_return = 1;
			}
			break;
		case S_IFELSE:
			if ( compilenode((NODE)FA1(f)) )
				val = compilestat((SNODE)FA2(f)); 
			else if ( FA3(f) )
				val = compilestat((SNODE)FA3(f));
			break;
		case S_FOR:
			compilenode((NODE)FA1(f));
			while ( 1 ) {
				if ( !compilenode((NODE)FA2(f)) )
					break;
				val = compilestat((SNODE)FA4(f)); 
				if ( f_break || f_return )
					break;
				f_continue = 0;
				compilenode((NODE)FA3(f));
			}
			f_break = 0; break;
		case S_DO:
			while ( 1 ) {
				val = compilestat((SNODE)FA1(f)); 
				if ( f_break || f_return )
					break;
				f_continue = 0;
				if ( !compilenode((NODE)FA2(f)) )
					break;
			}
			f_break = 0; break;
		default:
			fprintf(stderr,"compilestat: unknown id");
			error("");
			break;
	}
	return ( val );
}

pointer compilenode(node)
NODE node;
{
	NODE tn;
	pointer val;

	for ( tn = node, val = 0; tn; tn = NEXT(tn) )
		if ( BDY(tn) )
			val = compile((FNODE)BDY(tn));
	return ( val );
}

extern FUNC cur_binf;
extern NODE PVSS;

pointer compilef(f,a)
FUNC f;
FNODE a;
{
	LIST args;
	pointer val;
	int i,n,level;
	NODE tn,sn;
    VS pvs;

	if ( f->id == A_UNDEF ) {
		fprintf(stderr,"%s undefined",NAME(f));
		error("");
	}
	if ( f->id != A_PARI ) {
		for ( i = 0, tn = a?(NODE)FA0(a):0; tn; i++, tn = NEXT(tn) );
		if ( ((n = f->argc)>= 0 && i != n) || (n < 0 && i > -n) ) {
			fprintf(stderr,"argument mismatch in %s()",NAME(f));
			error("");
		}
	}
	switch ( f->id ) {
		case A_BIN:
			if ( !n ) {
				cur_binf = f;
				(*f->f.binf)(&val);
			} else {
				args = (LIST)compile(a);
				cur_binf = f;
				(*f->f.binf)(args?BDY(args):0,&val);
			}
			cur_binf = 0;
			break;
		case A_PARI:
			args = (LIST)compile(a);
			cur_binf = f;
			val = compileparif(f,args?BDY(args):0);
			cur_binf = 0;
			break;
		case A_USR:
			args = (LIST)compile(a);
    		pvs = f->f.usrf->pvs;
    		if ( PVSS ) {
        		((VS)BDY(PVSS))->at = compilestatline;
				level = ((VS)BDY(PVSS))->level+1;
			} else
				level = 1;
    		MKNODE(tn,pvs,PVSS); PVSS = tn;
    		CPVS = (VS)ALLOCA(sizeof(struct oVS)); BDY(PVSS) = (pointer)CPVS;
    		CPVS->usrf = f; CPVS->n = CPVS->asize = pvs->n;
			CPVS->level = level;
    		if ( CPVS->n ) {
        		CPVS->va = (struct oPV *)ALLOCA(CPVS->n*sizeof(struct oPV));
        		bcopy((char *)pvs->va,(char *)CPVS->va,
					(int)(pvs->n*sizeof(struct oPV)));
    		}
    		if ( nextbp )
        		nextbplevel++;
			for ( tn = f->f.usrf->args, sn = BDY(args); 
				sn; tn = NEXT(tn), sn = NEXT(sn) )
				ASSPV((int)FA0((FNODE)BDY(tn)),BDY(sn));
			val = compilestat((SNODE)BDY(f->f.usrf)); 
			f_return = f_break = f_continue = 0; poppvs(); 
			break;
		case A_PURE:
			val = compilepf(f->f.puref,a); break;
		default:
			fprintf(stderr,"%s undefined",NAME(f));
			error("");
			break;
	}
	return val;
}

pointer compilemapf(f,a)
FUNC f;
FNODE a;
{
	LIST args;
	NODE node,rest,t,n,l,r,r0;
	Obj head;
	VECT v,rv;
	MAT m,rm;
	LIST rl;
	int len,row,col,i,j;
	pointer val;

	args = (LIST)compile(a);
	node = BDY(args); head = (Obj)BDY(node); rest = NEXT(node);
	switch ( OID(head) ) {
		case O_VECT:
			v = (VECT)head; len = v->len; MKVECT(rv,len);
			for ( i = 0; i < len; i++ ) {
				MKNODE(t,BDY(v)[i],rest); BDY(rv)[i] = bcompilef(f,t);
			}
			val = (pointer)rv;
			break;
		case O_MAT:
			m = (MAT)head; row = m->row; col = m->col; MKMAT(rm,row,col);
			for ( i = 0; i < row; i++ )
				for ( j = 0; j < col; j++ ) {
					MKNODE(t,BDY(m)[i][j],rest); BDY(rm)[i][j] = bcompilef(f,t);
				}
			val = (pointer)rm;
			break;
		case O_LIST:
			n = BDY((LIST)head);
			for ( r0 = r = 0; n; n = NEXT(n) ) {
				NEXTNODE(r0,r); MKNODE(t,BDY(n),rest); BDY(r) = bcompilef(f,t);
			}
			if ( r0 )
				NEXT(r) = 0;
			MKLIST(rl,r0);
			val = (pointer)rl;
			break;
		default:
			val = bcompilef(f,node);
			break;
	}
	return val;
}

pointer bcompilef(f,a)
FUNC f;
NODE a;
{
	LIST args;
	pointer val;
	int i,n;
	NODE tn,sn;
    VS pvs;

	if ( f->id == A_UNDEF ) {
		fprintf(stderr,"%s undefined",NAME(f));
		error("");
	}
	if ( f->id != A_PARI ) {
		for ( i = 0, tn = a; tn; i++, tn = NEXT(tn) );
		if ( ((n = f->argc)>= 0 && i != n) || (n < 0 && i > -n) ) {
			fprintf(stderr,"argument mismatch in %s()",NAME(f));
			error("");
		}
	}
	switch ( f->id ) {
		case A_BIN:
			if ( !n ) {
				cur_binf = f;
				(*f->f.binf)(&val);
			} else {
				cur_binf = f;
				(*f->f.binf)(a,&val);
			}
			cur_binf = 0;
			break;
		case A_PARI:
			cur_binf = f;
			val = compileparif(f,a);
			cur_binf = 0;
			break;
		case A_USR:
    		pvs = f->f.usrf->pvs;
    		if ( PVSS )
        		((VS)BDY(PVSS))->at = compilestatline;
    		MKNODE(tn,pvs,PVSS); PVSS = tn;
    		CPVS = (VS)ALLOCA(sizeof(struct oVS)); BDY(PVSS) = (pointer)CPVS;
    		CPVS->usrf = f; CPVS->n = CPVS->asize = pvs->n;
    		if ( CPVS->n ) {
        		CPVS->va = (struct oPV *)ALLOCA(CPVS->n*sizeof(struct oPV));
        		bcopy((char *)pvs->va,(char *)CPVS->va,
					(int)(pvs->n*sizeof(struct oPV)));
    		}
    		if ( nextbp )
        		nextbplevel++;
			for ( tn = f->f.usrf->args, sn = a; 
				sn; tn = NEXT(tn), sn = NEXT(sn) )
				ASSPV((int)FA0((FNODE)BDY(tn)),BDY(sn));
			val = compilestat((SNODE)BDY(f->f.usrf)); 
			f_return = f_break = f_continue = 0; poppvs(); 
			break;
		default:
			fprintf(stderr,"%s undefined",NAME(f));
			error("");
			break;
	}
	return val;
}

pointer compileif(f,a)
FNODE f,a;
{
	Obj g;

	g = (Obj)compile(f);
	if ( g && (OID(g) == O_P) && (VR((P)g)->attr == (pointer)V_SR) )
		return compilef((FUNC)VR((P)g)->priv,a);
	else {
		fprintf(stderr,"invalid function pointer");
		error("");
	}
}

pointer compilepf(pf,a)
PF pf;
FNODE a;
{
	LIST args;
	pointer val;
	Obj s,s1;
	int i;
	NODE node;
	PFINS ins;
	PFAD ad;
	
	for ( i = 0, node = a?(NODE)FA0(a):0; node; i++, node = NEXT(node) );
	if ( pf->argc != i ) {
		fprintf(stderr,"argument mismatch in %s()",NAME(pf));
		error("");
	}
	args = (LIST)compile(a);
	if ( !pf->body ) {
		ins = (PFINS)CALLOC(1,sizeof(PF)+pf->argc*sizeof(struct oPFAD));
		ins->pf = pf;
		for ( i = 0, node = args->body, ad = ins->ad;
			node; node = NEXT(node), i++ ) {
			ad[i].d = 0; ad[i].arg = (Obj)node->body;	
		}
		simplify_ins(ins,&s); return (pointer)s;
	} else {
		for ( i = 0, s = pf->body, node = args->body; 
			node; node = NEXT(node), i++ ) {
			substr(CO,0,s,pf->args[i],(Obj)node->body,&s1); s = s1;
		}
		return (pointer)s;	
	}
}

void compilenodebody(sn,dnp)
NODE sn;
NODE *dnp;
{
	NODE n,n0,tn;
	int line;

	if ( !sn ) {
		*dnp = 0;
		return;
	}
	line = compilestatline;
	for ( tn = sn, n0 = 0; tn; tn = NEXT(tn) ) {
		NEXTNODE(n0,n);
		BDY(n) = compile((FNODE)BDY(tn));
		compilestatline = line;
	}
	NEXT(n) = 0; *dnp = n0;
}

void searchf(fn,name,r)
NODE fn;
char *name;
FUNC *r;
{
	NODE tn;

	for ( tn = fn; 
		tn && strcmp(NAME((FUNC)BDY(tn)),name); tn = NEXT(tn) );
		if ( tn ) {
			*r = (FUNC)BDY(tn);
			return;
		}
	*r = 0;
}

void appenduf(name,r)
char *name;
FUNC *r;
{
	NODE tn;
	FUNC f;

	f=(FUNC)MALLOC(sizeof(struct oFUNC)); 
	f->name = name; f->id = A_UNDEF; f->argc = 0; f->f.binf = 0;
	MKNODE(tn,f,usrf); usrf = tn;
	*r = f;
}

void mkparif(name,r)
char *name;
FUNC *r;
{
	NODE tn;
	FUNC f;

	*r = f =(FUNC)MALLOC(sizeof(struct oFUNC)); 
	f->name = name; f->id = A_PARI; f->argc = 0; f->f.binf = 0;
}

void mkuf(name,fname,args,body,startl,endl,desc)
char *name,*fname;
NODE args;
SNODE body;
int startl,endl;
char *desc;
{
	FUNC f;
	USRF t;
	NODE sn,tn;
	FNODE fn;
	int argc;

	searchf(sysf,name,&f);
	if ( f ) {
		fprintf(stderr,"def : builtin function %s() cannot be redefined.\n",name);
		CPVS = GPVS; return;
	}
	for ( argc = 0, sn = args; sn; argc++, sn = NEXT(sn) ) {
		fn = (FNODE)BDY(sn);
		if ( !fn || ID(fn) != I_PVAR ) {
			fprintf(stderr,"illegal	argument in %s()\n",name);
			CPVS = GPVS; return;
		}
	}
	for ( sn = usrf; sn && strcmp(NAME((FUNC)BDY(sn)),name); sn = NEXT(sn) );
	if ( sn )
		f = (FUNC)BDY(sn);
	else {
		f=(FUNC)MALLOC(sizeof(struct oFUNC)); 
		f->name = name; 
		MKNODE(tn,f,usrf); usrf = tn;
	}
	if ( Verbose && f->id != A_UNDEF )
		fprintf(stderr,"Warning : %s() redefined.\n",name);
/*	else
		fprintf(stderr,"%s() defined.\n",name); */
	t=(USRF)MALLOC(sizeof(struct oUSRF));
	t->args=args; BDY(t)=body; t->pvs = CPVS; t->fname = fname; 
	t->startl = startl; t->endl = endl; t->vol = asir_infile->vol;
	t->desc = desc;
	f->id = A_USR; f->argc = argc; f->f.usrf = t;
	CPVS = GPVS;
	clearbp(f);
}
