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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/eval.c,v 1.8 2000/12/05 01:24:56 noro Exp $ 
*/
#include <ctype.h>
#include "ca.h"
#include "al.h"
#include "base.h"
#include "parse.h"
#include <sys/types.h>
#include <sys/stat.h>
#if PARI
#include "genpari.h"
#endif

extern jmp_buf timer_env;

int f_break,f_return,f_continue;
int evalstatline;
int recv_intr;

pointer bevalf(), evalmapf(), evall();
pointer eval_rec_mapf(), beval_rec_mapf();
Obj getopt_from_cpvs();

pointer eval(f)
FNODE f;
{
	LIST t;
	STRING str;
	pointer val = 0;
	pointer a,a1,a2;
	NODE tn,ind;
	R u;
	DP dp;
	int pv,c;
	FNODE f1;
	UP2 up2;
	UP up;
	GF2N gf2n;
	GFPN gfpn;

#if defined(VISUAL)
	if ( recv_intr ) {
#include <signal.h>
		if ( recv_intr == 1 ) {
			recv_intr = 0;
			int_handler(SIGINT);
		} else {
			recv_intr = 0;
			ox_usr1_handler(0);
		}
	}
#endif
	if ( !f )
		return ( 0 );
	switch ( f->id ) {
		case I_BOP:
			a1 = eval((FNODE)FA1(f)); a2 = eval((FNODE)FA2(f));
			(*((ARF)FA0(f))->fp)(CO,a1,a2,&val); 
		break;
		case I_COP:
			a1 = eval((FNODE)FA1(f)); a2 = eval((FNODE)FA2(f));
			c = arf_comp(CO,a1,a2);
			switch ( (cid)FA0(f) ) {
				case C_EQ:
					c = (c == 0); break;
				case C_NE:
					c = (c != 0); break;
				case C_GT:
					c = (c > 0); break;
				case C_LT:
					c = (c < 0); break;
				case C_GE:
					c = (c >= 0); break;
				case C_LE:
					c = (c <= 0); break;
				default:
					c = 0; break;
			}
			if ( c )
				val = (pointer)ONE; 
			break;
		case I_AND:
			if ( eval((FNODE)FA0(f)) && eval((FNODE)FA1(f)) ) 
				val = (pointer)ONE; 
			break;
		case I_OR:
			if ( eval((FNODE)FA0(f)) || eval((FNODE)FA1(f)) ) 
				val = (pointer)ONE; 
			break;
		case I_NOT:
			if ( eval((FNODE)FA0(f)) )
				val = 0;
			else
				val = (pointer)ONE; 
			break;
		case I_LOP:
			a1 = eval((FNODE)FA1(f)); a2 = eval((FNODE)FA2(f));
			val = evall((lid)FA0(f),a1,a2);
			break;
		case I_CE:
			if ( eval((FNODE)FA0(f)) )
				val = eval((FNODE)FA1(f));
			else
				val = eval((FNODE)FA2(f));
			break;
		case I_EV:
			evalnodebody((NODE)FA0(f),&tn); nodetod(tn,&dp); val = (pointer)dp;
			break;
		case I_FUNC:
			val = evalf((FUNC)FA0(f),(FNODE)FA1(f),0); break;
		case I_FUNC_OPT:
			val = evalf((FUNC)FA0(f),(FNODE)FA1(f),(FNODE)FA2(f)); break;
		case I_PFDERIV:
			error("eval : not implemented yet");
			break;
		case I_MAP:
			val = evalmapf((FUNC)FA0(f),(FNODE)FA1(f)); break;
		case I_RECMAP:
			val = eval_rec_mapf((FUNC)FA0(f),(FNODE)FA1(f)); break;
		case I_IFUNC:
			val = evalif((FNODE)FA0(f),(FNODE)FA1(f)); break;
#if !defined(VISUAL)
		case I_TIMER:
			{
				int interval;
				Obj expired;

				interval = QTOS((Q)eval((FNODE)FA0(f)));
				expired = (Obj)eval((FNODE)FA2(f));
				set_timer(interval);
				savepvs();
				if ( !setjmp(timer_env) )
					val = eval((FNODE)FA1(f));
				else {
					val = (pointer)expired;
					restorepvs();
				}
				reset_timer();
			}
			break;
#endif
		case I_PRESELF:
			f1 = (FNODE)FA1(f);
			if ( ID(f1) == I_PVAR ) {
				pv = (int)FA0(f1); ind = (NODE)FA1(f1); GETPV(pv,a); 
				if ( !ind ) {
					(*((ARF)FA0(f))->fp)(CO,a,ONE,&val); ASSPV(pv,val); 
				} else if ( a ) {
					evalnodebody(ind,&tn); getarray(a,tn,(pointer *)&u); 
					(*((ARF)FA0(f))->fp)(CO,u,ONE,&val); putarray(a,tn,val);
				}
			} else
				error("++ : not implemented yet");
			break;
		case I_POSTSELF:
			f1 = (FNODE)FA1(f);
			if ( ID(f1) == I_PVAR ) {
				pv = (int)FA0(f1); ind = (NODE)FA1(f1); GETPV(pv,val); 
				if ( !ind ) {
					(*((ARF)FA0(f))->fp)(CO,val,ONE,&u); ASSPV(pv,u);
				} else if ( val ) {
					evalnodebody(ind,&tn); getarray(val,tn,&a);
					(*((ARF)FA0(f))->fp)(CO,a,ONE,&u); putarray(val,tn,(pointer)u);
					val = a;
				}
			} else
				error("-- : not implemented yet");
			break;
		case I_PVAR:
			pv = (int)FA0(f); ind = (NODE)FA1(f); GETPV(pv,a); 
			if ( !ind )
				val = a;
			else {
				evalnodebody(ind,&tn); getarray(a,tn,&val); 
			}
			break;
		case I_ASSPVAR:
			f1 = (FNODE)FA0(f);
			if ( ID(f1) == I_PVAR ) {
				pv = (int)FA0(f1); ind = (NODE)FA1(f1);
				if ( !ind ) {
					val = eval((FNODE)FA1(f)); ASSPV(pv,val);
				} else {
					GETPV(pv,a);
					evalnodebody(ind,&tn); 
					putarray(a,tn,val = eval((FNODE)FA1(f))); 
				}
			} else if ( ID(f1) == I_POINT ) {
				/* f1 <-> FA0(f1)->FA1(f1) */
				a = eval(FA0(f1));
				assign_to_member(a,(char *)FA1(f1),val = eval((FNODE)FA1(f)));
			} else if ( ID(f1) == I_INDEX ) {
				/* f1 <-> FA0(f1)[FA1(f1)] */
				a = eval((FNODE)FA0(f1)); ind = (NODE)FA1(f1);
				evalnodebody(ind,&tn);
				putarray(a,tn,val = eval((FNODE)FA1(f)));
			}
			break;
		case I_ANS:
			if ( (pv =(int)FA0(f)) < (int)APVS->n ) 
				val = APVS->va[pv].priv;
			break;
		case I_GF2NGEN:
			NEWUP2(up2,1);
			up2->w=1;
			up2->b[0] = 2; /* @ */
			MKGF2N(up2,gf2n);
			val = (pointer)gf2n;
			break;
		case I_GFPNGEN:
			up = UPALLOC(1);
			up->d=1;
			up->c[0] = 0;
			up->c[1] = (Num)ONELM;
			MKGFPN(up,gfpn);
			val = (pointer)gfpn;
			break;
		case I_STR:
			MKSTR(str,FA0(f)); val = (pointer)str; break;
		case I_FORMULA:
			 val = FA0(f); break;
		case I_LIST:
			evalnodebody((NODE)FA0(f),&tn); MKLIST(t,tn); val = (pointer)t; break;
		case I_NEWCOMP:
			newstruct((int)FA0(f),(struct oCOMP **)&val); break;
		case I_CAR:
			if ( !(a = eval((FNODE)FA0(f))) || (OID(a) != O_LIST) )
				val = 0;
			else if ( !BDY((LIST)a) )
				val = a;
			else
				val = (pointer)BDY(BDY((LIST)a));
			break;
		case I_CDR:
			if ( !(a = eval((FNODE)FA0(f))) || (OID(a) != O_LIST) )
				val = 0;
			else if ( !BDY((LIST)a) )
				val = a;
			else {
				MKLIST(t,NEXT(BDY((LIST)a))); val = (pointer)t;
			}
			break;
		case I_PROC:
			val = (pointer)FA0(f); break;
		case I_INDEX:
			a = eval((FNODE)FA0(f)); ind = (NODE)FA1(f);
			evalnodebody(ind,&tn); getarray(a,tn,&val); 
			break;
		case I_OPT:
			MKSTR(str,(char *)FA0(f));
			a = (pointer)eval(FA1(f));
			tn = mknode(2,str,a);
			MKLIST(t,tn); val = (pointer)t;
			break;
		case I_GETOPT:
			val = (pointer)getopt_from_cpvs((char *)FA0(f));
			break;
		case I_POINT:
			a = (pointer)eval(FA0(f));
			val = (pointer)memberofstruct(a,(char *)FA1(f));
			break;
		default:
			error("eval : unknown id");
			break;
	}
	return ( val );
}

pointer evalstat(f)
SNODE f;
{
	pointer val = 0,t,s,s1;
	P u;
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
	evalstatline = f->ln;

	switch ( f->id ) {
		case S_BP:
			if ( !nextbp && (!FA1(f) || eval((FNODE)FA1(f))) ) {
				if ( (FNODE)FA2(f) ) {
#if PARI
					pari_outfile = stderr;
#endif
					asir_out = stderr;
					printexpr(CO,eval((FNODE)FA2(f)));
					putc('\n',asir_out); fflush(asir_out);
#if PARI
					pari_outfile = stdout;
#endif
					asir_out = stdout;
				} else {
					nextbp = 1; nextbplevel = 0; 
				}
			}
			val = evalstat((SNODE)FA0(f)); 
			break;
		case S_PFDEF:
			ac = argc(FA1(f)); a = (V *)MALLOC(ac*sizeof(V));
			s = eval((FNODE)FA2(f));
			buf = (char *)ALLOCA(BUFSIZ);
			for ( i = 0, tn = (NODE)FA1(f); tn; tn = NEXT(tn), i++ ) {
				t = eval((FNODE)tn->body); sprintf(buf,"_%s",NAME(VR((P)t)));
				makevar(buf,&u); a[i] = VR(u);
				substr(CO,0,(Obj)s,VR((P)t),(Obj)u,(Obj *)&s1); s = s1;
			}
			mkpf((char *)FA0(f),(Obj)s,ac,a,0,0,0,(PF *)&val); val = 0; break;
		case S_SINGLE:
			val = eval((FNODE)FA0(f)); break;
		case S_CPLX:
			for ( tn = (NODE)FA0(f); tn; tn = NEXT(tn) ) {
				if ( BDY(tn) )
					val = evalstat((SNODE)BDY(tn));
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
				val = eval((FNODE)FA0(f)); f_return = 1;
			}
			break;
		case S_IFELSE:
			if ( evalnode((NODE)FA1(f)) )
				val = evalstat((SNODE)FA2(f)); 
			else if ( FA3(f) )
				val = evalstat((SNODE)FA3(f));
			break;
		case S_FOR:
			evalnode((NODE)FA1(f));
			while ( 1 ) {
				if ( !evalnode((NODE)FA2(f)) )
					break;
				val = evalstat((SNODE)FA4(f)); 
				if ( f_break || f_return )
					break;
				f_continue = 0;
				evalnode((NODE)FA3(f));
			}
			f_break = 0; break;
		case S_DO:
			while ( 1 ) {
				val = evalstat((SNODE)FA1(f)); 
				if ( f_break || f_return )
					break;
				f_continue = 0;
				if ( !evalnode((NODE)FA2(f)) )
					break;
			}
			f_break = 0; break;
		default:
			error("evalstat : unknown id");
			break;
	}
	return ( val );
}

pointer evalnode(node)
NODE node;
{
	NODE tn;
	pointer val;

	for ( tn = node, val = 0; tn; tn = NEXT(tn) )
		if ( BDY(tn) )
			val = eval((FNODE)BDY(tn));
	return ( val );
}

extern FUNC cur_binf;
extern NODE PVSS;

pointer evalf(f,a,opt)
FUNC f;
FNODE a;
FNODE opt;
{
	LIST args;
	pointer val;
	int i,n,level;
	NODE tn,sn,opts;
    VS pvs;
	char errbuf[BUFSIZ];

	if ( f->id == A_UNDEF ) {
		sprintf(errbuf,"evalf : %s undefined",NAME(f));
		error(errbuf);
	}
	if ( f->id != A_PARI ) {
		for ( i = 0, tn = a?(NODE)FA0(a):0; tn; i++, tn = NEXT(tn) );
		if ( ((n = f->argc)>= 0 && i != n) || (n < 0 && i > -n) ) {
			sprintf(errbuf,"evalf : argument mismatch in %s()",NAME(f));
			error(errbuf);
		}
	}
	switch ( f->id ) {
		case A_BIN:
			if ( !n ) {
				cur_binf = f;
				(*f->f.binf)(&val);
			} else {
				args = (LIST)eval(a);
				cur_binf = f;
				(*f->f.binf)(args?BDY(args):0,&val);
			}
			cur_binf = 0;
			break;
		case A_PARI:
			args = (LIST)eval(a);
			cur_binf = f;
			val = evalparif(f,args?BDY(args):0);
			cur_binf = 0;
			break;
		case A_USR:
			args = (LIST)eval(a);
			if ( opt )
				opts = BDY((LIST)eval(opt));
			else
				opts = 0;
    		pvs = f->f.usrf->pvs;
    		if ( PVSS ) {
        		((VS)BDY(PVSS))->at = evalstatline;
				level = ((VS)BDY(PVSS))->level+1;
			} else
				level = 1;
    		MKNODE(tn,pvs,PVSS); PVSS = tn;
    		CPVS = (VS)ALLOCA(sizeof(struct oVS)); BDY(PVSS) = (pointer)CPVS;
    		CPVS->usrf = f; CPVS->n = CPVS->asize = pvs->n;
			CPVS->level = level;
			CPVS->opt = opts;
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
			val = evalstat((SNODE)BDY(f->f.usrf)); 
			f_return = f_break = f_continue = 0; poppvs(); 
			break;
		case A_PURE:
			args = (LIST)eval(a);
			val = evalpf(f->f.puref,args?BDY(args):0);
			break;
		default:
			sprintf(errbuf,"evalf : %s undefined",NAME(f));
			error(errbuf);
			break;
	}
	return val;
}

pointer evalmapf(f,a)
FUNC f;
FNODE a;
{
	LIST args;
	NODE node,rest,t,n,r,r0;
	Obj head;
	VECT v,rv;
	MAT m,rm;
	LIST rl;
	int len,row,col,i,j;
	pointer val;

	args = (LIST)eval(a);
	node = BDY(args); head = (Obj)BDY(node); rest = NEXT(node);
	if ( !head ) {
		val = bevalf(f,node);
		return val;
	}
	switch ( OID(head) ) {
		case O_VECT:
			v = (VECT)head; len = v->len; MKVECT(rv,len);
			for ( i = 0; i < len; i++ ) {
				MKNODE(t,BDY(v)[i],rest); BDY(rv)[i] = bevalf(f,t);
			}
			val = (pointer)rv;
			break;
		case O_MAT:
			m = (MAT)head; row = m->row; col = m->col; MKMAT(rm,row,col);
			for ( i = 0; i < row; i++ )
				for ( j = 0; j < col; j++ ) {
					MKNODE(t,BDY(m)[i][j],rest); BDY(rm)[i][j] = bevalf(f,t);
				}
			val = (pointer)rm;
			break;
		case O_LIST:
			n = BDY((LIST)head);
			for ( r0 = r = 0; n; n = NEXT(n) ) {
				NEXTNODE(r0,r); MKNODE(t,BDY(n),rest); BDY(r) = bevalf(f,t);
			}
			if ( r0 )
				NEXT(r) = 0;
			MKLIST(rl,r0);
			val = (pointer)rl;
			break;
		default:
			val = bevalf(f,node);
			break;
	}
	return val;
}

pointer eval_rec_mapf(f,a)
FUNC f;
FNODE a;
{
	LIST args;

	args = (LIST)eval(a);
	return beval_rec_mapf(f,BDY(args));
}

pointer beval_rec_mapf(f,node)
FUNC f;
NODE node;
{
	LIST args;
	NODE rest,t,n,r,r0;
	Obj head;
	VECT v,rv;
	MAT m,rm;
	LIST rl;
	int len,row,col,i,j;
	pointer val;

	head = (Obj)BDY(node); rest = NEXT(node);
	if ( !head ) {
		val = bevalf(f,node);
		return val;
	}
	switch ( OID(head) ) {
		case O_VECT:
			v = (VECT)head; len = v->len; MKVECT(rv,len);
			for ( i = 0; i < len; i++ ) {
				MKNODE(t,BDY(v)[i],rest); BDY(rv)[i] = beval_rec_mapf(f,t);
			}
			val = (pointer)rv;
			break;
		case O_MAT:
			m = (MAT)head; row = m->row; col = m->col; MKMAT(rm,row,col);
			for ( i = 0; i < row; i++ )
				for ( j = 0; j < col; j++ ) {
					MKNODE(t,BDY(m)[i][j],rest);
					BDY(rm)[i][j] = beval_rec_mapf(f,t);
				}
			val = (pointer)rm;
			break;
		case O_LIST:
			n = BDY((LIST)head);
			for ( r0 = r = 0; n; n = NEXT(n) ) {
				NEXTNODE(r0,r); MKNODE(t,BDY(n),rest);
				BDY(r) = beval_rec_mapf(f,t);
			}
			if ( r0 )
				NEXT(r) = 0;
			MKLIST(rl,r0);
			val = (pointer)rl;
			break;
		default:
			val = bevalf(f,node);
			break;
	}
	return val;
}

pointer bevalf(f,a)
FUNC f;
NODE a;
{
	pointer val;
	int i,n;
	NODE tn,sn;
    VS pvs;
	struct oLIST list;
	struct oFNODE fnode;
	char errbuf[BUFSIZ];

	if ( f->id == A_UNDEF ) {
		sprintf(errbuf,"bevalf : %s undefined",NAME(f));
		error(errbuf);
	}
	if ( f->id != A_PARI ) {
		for ( i = 0, tn = a; tn; i++, tn = NEXT(tn) );
		if ( ((n = f->argc)>= 0 && i != n) || (n < 0 && i > -n) ) {
			sprintf(errbuf,"bevalf : argument mismatch in %s()",NAME(f));
			error(errbuf);
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
			val = evalparif(f,a);
			cur_binf = 0;
			break;
		case A_USR:
    		pvs = f->f.usrf->pvs;
    		if ( PVSS )
        		((VS)BDY(PVSS))->at = evalstatline;
    		MKNODE(tn,pvs,PVSS); PVSS = tn;
    		CPVS = (VS)ALLOCA(sizeof(struct oVS)); BDY(PVSS) = (pointer)CPVS;
    		CPVS->usrf = f; CPVS->n = CPVS->asize = pvs->n;
			CPVS->opt = 0;
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
			val = evalstat((SNODE)BDY(f->f.usrf)); 
			f_return = f_break = f_continue = 0; poppvs(); 
			break;
		case A_PURE:
			val = evalpf(f->f.puref,a);
			break;
		default:
			sprintf(errbuf,"bevalf : %s undefined",NAME(f));
			error(errbuf);
			break;
	}
	return val;
}

pointer evalif(f,a)
FNODE f,a;
{
	Obj g;

	g = (Obj)eval(f);
	if ( g && (OID(g) == O_P) && (VR((P)g)->attr == (pointer)V_SR) )
		return evalf((FUNC)VR((P)g)->priv,a,0);
	else {
		error("invalid function pointer");
	}
}

pointer evalpf(pf,args)
PF pf;
NODE args;
{
	Obj s,s1;
	int i;
	NODE node;
	PFINS ins;
	PFAD ad;
	char errbuf[BUFSIZ];
	
	if ( !pf->body ) {
		ins = (PFINS)CALLOC(1,sizeof(PF)+pf->argc*sizeof(struct oPFAD));
		ins->pf = pf;
		for ( i = 0, node = args, ad = ins->ad;
			node; node = NEXT(node), i++ ) {
			ad[i].d = 0; ad[i].arg = (Obj)node->body;	
		}
		simplify_ins(ins,&s);
	} else {
		for ( i = 0, s = pf->body, node = args; 
			node; node = NEXT(node), i++ ) {
			substr(CO,0,s,pf->args[i],(Obj)node->body,&s1); s = s1;
		}
	}
	return (pointer)s;
}

void evalnodebody(sn,dnp)
NODE sn;
NODE *dnp;
{
	NODE n,n0,tn;
	int line;

	if ( !sn ) {
		*dnp = 0;
		return;
	}
	line = evalstatline;
	for ( tn = sn, n0 = 0; tn; tn = NEXT(tn) ) {
		NEXTNODE(n0,n);
		BDY(n) = eval((FNODE)BDY(tn));
		evalstatline = line;
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

/*
	retrieve value of an option whose key matches 'key'
	CVS->opt is a list(node) of key-value pair (list)
	CVS->opt = BDY([[key,value],[key,value],...])
*/

Obj getopt_from_cpvs(key)
char *key;
{
	NODE opts,opt;
	Obj value;
	extern Obj VOIDobj;

	opts = CPVS->opt;
	for ( ; opts; opts = NEXT(opts) ) {
		opt = BDY((LIST)BDY(opts));
		if ( !strcmp(key,BDY((STRING)BDY(opt))) )
			return (Obj)BDY(NEXT(opt));
	}
	return VOIDobj;

}
