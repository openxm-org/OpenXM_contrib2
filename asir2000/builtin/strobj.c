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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/strobj.c,v 1.16 2004/03/04 03:31:28 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ctype.h"
#if defined(PARI)
#include "genpari.h"
#  if !(PARI_VERSION_CODE > 131588)
extern jmp_buf environnement;
#  endif
#endif
#include <string.h>

extern char *parse_strp;

void Prtostr(), Pstrtov(), Peval_str();
void Pstrtoascii(), Pasciitostr();
void Pstr_len(), Pstr_chr(), Psub_str();
void Pwrite_to_tb();
void Ptb_to_string();
void Pclear_tb();
void Pstring_to_tb();
void Pquotetotex_tb();
void Pquotetotex();
void fnodetotex_tb(FNODE f,TB tb);
char *symbol_name(char *name);
void tb_to_string(TB tb,STRING *rp);
void fnodenodetotex_tb(NODE n,TB tb);
void fargstotex_tb(char *opname,FNODE f,TB tb);

struct ftab str_tab[] = {
	{"rtostr",Prtostr,1},
	{"strtov",Pstrtov,1},
	{"eval_str",Peval_str,1},
	{"strtoascii",Pstrtoascii,1},
	{"asciitostr",Pasciitostr,1},
	{"str_len",Pstr_len,1},
	{"str_chr",Pstr_chr,3},
	{"sub_str",Psub_str,3},
	{"write_to_tb",Pwrite_to_tb,2},
	{"clear_tb",Pclear_tb,1},
	{"tb_to_string",Ptb_to_string,1},
	{"string_to_tb",Pstring_to_tb,1},
	{"quotetotex_tb",Pquotetotex_tb,2},
	{"quotetotex",Pquotetotex,1},
	{0,0,0},
};

void write_tb(char *s,TB tb)
{
	if ( tb->next == tb->size ) {
		tb->size *= 2;
		tb->body = (char **)REALLOC(tb->body,tb->size*sizeof(char *));
	}
	tb->body[tb->next] = s;
	tb->next++;
}

void Pwrite_to_tb(NODE arg,Q *rp)
{
	int i;
	Obj obj;
	TB tb;

	asir_assert(ARG1(arg),O_TB,"write_to_tb");
	obj = ARG0(arg);
	if ( !obj )
		write_tb("",ARG1(arg));
	else if ( OID(obj) == O_STR )
		write_tb(BDY((STRING)obj),ARG1(arg));
	else if ( OID(obj) == O_TB ) {
		tb = (TB)obj;
		for ( i = 0; i < tb->next; i++ )
			write_tb(tb->body[i],ARG1(arg));
	}
	*rp = 0;	
}

void Pquotetotex(NODE arg,STRING *rp)
{
	TB tb;

	NEWTB(tb);
	fnodetotex_tb(BDY((QUOTE)ARG0(arg)),tb);
	tb_to_string(tb,rp);
}

void Pquotetotex_tb(NODE arg,Q *rp)
{
	int i;
	TB tb;

	asir_assert(ARG1(arg),O_TB,"quotetotex_tb");
	fnodetotex_tb(BDY((QUOTE)ARG0(arg)),ARG1(arg));
	*rp = 0;	
}

void Pstring_to_tb(NODE arg,TB *rp)
{
	TB tb;

	asir_assert(ARG0(arg),O_STR,"string_to_tb");
	NEWTB(tb);
	tb->body[0] = BDY((STRING)ARG0(arg));
	tb->next++;
	*rp = tb;
}

void Ptb_to_string(NODE arg,STRING *rp)
{
	TB tb;

	asir_assert(ARG0(arg),O_TB,"tb_to_string");
	tb = (TB)ARG0(arg);
	tb_to_string(tb,rp);
}

void tb_to_string(TB tb,STRING *rp)
{
	int j,len;
	char *all,*p,*q;

	for ( j = 0, len = 0; j < tb->next; j++ )
		len += strlen(tb->body[j]);
	all = (char *)MALLOC_ATOMIC((len+1)*sizeof(char));
	for ( j = 0, p = all; j < tb->next; j++ )
		for ( q = tb->body[j]; *q; *p++ = *q++ );
	*p = 0;
	MKSTR(*rp,all);
}

void Pclear_tb(NODE arg,Q *rp)
{
	TB tb;
	int j;

	asir_assert(ARG0(arg),O_TB,"clear_tb");
	tb = (TB)ARG0(arg);
	for ( j = 0; j < tb->next; j++ )
		tb->body[j] = 0;
	tb->next = 0;
	*rp = 0;
}

void Pstr_len(arg,rp)
NODE arg;
Q *rp;
{
	Obj obj;
	TB tb;
	int r,i;

	obj = (Obj)ARG0(arg);
	if ( !obj || (OID(obj) != O_STR && OID(obj) != O_TB) ) 
		error("str_len : invalid argument");
	if ( OID(obj) == O_STR)
		r = strlen(BDY((STRING)obj));
	else if ( OID(obj) == O_TB ) {
		tb = (TB)obj;
		for ( r = i = 0; i < tb->next; i++ )
			r += strlen(tb->body[i]);
	}
	STOQ(r,*rp);
}

void Pstr_chr(arg,rp)
NODE arg;
Q *rp;
{
	STRING str,terminator;
	Q start;
	char *p,*ind;
	int chr,spos,r;

	str = (STRING)ARG0(arg);
	start = (Q)ARG1(arg);
	terminator = (STRING)ARG2(arg);
	asir_assert(str,O_STR,"str_chr");
	asir_assert(start,O_N,"str_chr");
	asir_assert(terminator,O_STR,"str_chr");
	p = BDY(str);
	spos = QTOS(start);
	chr = BDY(terminator)[0];
	if ( spos > (int)strlen(p) )
		r = -1;
	else {
		ind = strchr(p+spos,chr);
		if ( ind )
			r = ind-p;
		else
			r = -1;
	}
	STOQ(r,*rp);
}

void Psub_str(arg,rp)
NODE arg;
STRING *rp;
{
	STRING str;
	Q head,tail;
	char *p,*r;
	int spos,epos,len;

	str = (STRING)ARG0(arg);
	head = (Q)ARG1(arg);
	tail = (Q)ARG2(arg);
	asir_assert(str,O_STR,"sub_str");
	asir_assert(head,O_N,"sub_str");
	asir_assert(tail,O_N,"sub_str");
	p = BDY(str);
	spos = QTOS(head);
	epos = QTOS(tail);
	len = strlen(p);
	if ( (spos >= len) || (epos < spos) ) {
		*rp = 0; return;
	}
	if ( epos >= len )
		epos = len-1;
	len = epos-spos+1;
	r = (char *)MALLOC(len+1);
	strncpy(r,p+spos,len);
	r[len] = 0;
	MKSTR(*rp,r);
}

void Pstrtoascii(arg,rp)
NODE arg;
LIST *rp;
{
	STRING str;
	unsigned char *p;
	int len,i;
	NODE n,n1;
	Q q;

	str = (STRING)ARG0(arg);
	asir_assert(str,O_STR,"strtoascii");
	p = BDY(str);
	len = strlen(p);
	for ( i = len-1, n = 0; i >= 0; i-- ) {
		UTOQ((unsigned int)p[i],q);
		MKNODE(n1,q,n);
		n = n1;
	}
	MKLIST(*rp,n);
}

void Pasciitostr(arg,rp)
NODE arg;
STRING *rp;
{
	LIST list;
	unsigned char *p;
	int len,i,j;
	NODE n;
	Q q;

	list = (LIST)ARG0(arg);
	asir_assert(list,O_LIST,"asciitostr");
	n = BDY(list);
	len = length(n);
	p = MALLOC_ATOMIC(len+1);
	for ( i = 0; i < len; i++, n = NEXT(n) ) {
		q = (Q)BDY(n);
		asir_assert(q,O_N,"asciitostr");
		j = QTOS(q);
		if ( j >= 256 || j <= 0 )
			error("asciitostr : argument out of range");
		p[i] = j;
	}
	p[i] = 0;
	MKSTR(*rp,(char *)p);
}

void Peval_str(arg,rp)
NODE arg;
Obj *rp;
{
	FNODE fnode;
	char *cmd;
#if defined(PARI)
	void recover(int);

	recover(0);
#  if !(PARI_VERSION_CODE > 131588)
	if ( setjmp(environnement) ) {
		avma = top; recover(1);
		resetenv("");
	}
#  endif
#endif
	cmd = BDY((STRING)ARG0(arg));
	exprparse_create_var(0,cmd,&fnode);
	*rp = eval(fnode);
}

void Prtostr(arg,rp)
NODE arg;
STRING *rp;
{
	char *b;
	int len;

	len = estimate_length(CO,ARG0(arg));
	b = (char *)MALLOC_ATOMIC(len+1);
	soutput_init(b);
	sprintexpr(CO,ARG0(arg));
	MKSTR(*rp,b);
}

void Pstrtov(arg,rp)
NODE arg;
P *rp;
{
	char *p;

	p = BDY((STRING)ARG0(arg));
#if 0
	if ( !islower(*p) )
		*rp = 0;
	else {
		for ( t = p+1; t && (isalnum(*t) || *t == '_'); t++ );
		if ( *t )
			*rp = 0;
		else
			makevar(p,rp);
	}
#else
	makevar(p,rp);
#endif
}

struct TeXSymbol {
	char *text;
	char *symbol;
};

static struct TeXSymbol texsymbol[] = {
 {"sin","\\sin"},
 {"cos","\\cos"},
 {"tan","\\tan"},
 {"sinh","\\sinh"},
 {"cosh","\\cosh"},
 {"tanh","\\tanh"},
 {"exp","\\exp"},
 {"log","\\log"},

/* Greek Letters (lower case) */
 {"alpha","\\alpha"},
 {"beta","\\beta"},
 {"gamma","\\gamma"},
 {"delta","\\delta"},
 {"epsilon","\\epsilon"},
 {"varepsilon","\\varepsilon"},
 {"zeta","\\zeta"},
 {"eta","\\eta"},
 {"theta","\\theta"},
 {"vartheta","\\vartheta"},
 {"iota","\\iota"},
 {"kappa","\\kappa"},
 {"lambda","\\lambda"},
 {"mu","\\mu"},
 {"nu","\\nu"},
 {"xi","\\xi"},
 {"pi","\\pi"},
 {"varpi","\\varpi"},
 {"rho","\\rho"},
 {"sigma","\\sigma"},
 {"varsigma","\\varsigma"},
 {"tau","\\tau"},
 {"upsilon","\\upsilon"},
 {"phi","\\phi"},
 {"varphi","\\varphi"},
 {"chi","\\chi"},
 {"omega","\\omega"},

/* Greek Letters, (upper case) */
 {"ggamma","\\Gamma"},
 {"ddelta","\\Delta"},
 {"ttheta","\\Theta"},
 {"llambda","\\Lambda"},
 {"xxi","\\Xi"},
 {"ppi","\\Pi"},
 {"ssigma","\\Sigma"},
 {"uupsilon","\\Upsilon"},
 {"pphi","\\Phi"},
 {"ppsi","\\Psi"},
 {"oomega","\\Omega"},

 /* Our own mathematical functions */
 {"algebra_tensor","\\otimes"},
 {"base_where","{\\rm \\ where \\ }"},
 /* Mathematical constants */
 {"c_pi","\\pi"},
 {"c_i","\\sqrt{-1}"},

 /* Temporary  */
 {"dx","\\partial"},
 {0,0}
};

char *symbol_name(char *name)
{
	int i;

	for ( i = 0; texsymbol[i].text; i++ )
		if ( !strcmp(texsymbol[i].text,name) )
			return texsymbol[i].symbol;
	return name;
}

void fnodetotex_tb(FNODE f,TB tb)
{
	NODE n,t,t0;
	char vname[BUFSIZ];
	char *opname;
	Obj obj;
	int i,len,allzero;
	FNODE fi,f2;

	write_tb(" ",tb);
	if ( !f ) {
		write_tb("0",tb);
		return;
	}
	switch ( f->id ) {
		/* unary operators */
		case I_NOT: case I_PAREN: case I_MINUS:
			switch ( f->id ) {
				case I_NOT:
					write_tb("\\neg (",tb);
					fnodetotex_tb((FNODE)FA0(f),tb);
					write_tb(")",tb);
					break;
				case I_PAREN:
					write_tb("(",tb);
					fnodetotex_tb((FNODE)FA0(f),tb);
					write_tb(")",tb);
					break;
				case I_MINUS:
					write_tb("-",tb);
					fnodetotex_tb((FNODE)FA0(f),tb);
					break;
			}
			break;

		/* binary operators */
		case I_BOP: case I_COP: case I_LOP: case I_AND: case I_OR:
			/* arg list */
			/* I_AND, I_OR => FA0(f), FA1(f) */
			/* otherwise   => FA1(f), FA2(f) */

			/* op */
			switch ( f->id ) {
				case I_BOP:
					opname = ((ARF)FA0(f))->name;
					if ( !strcmp(opname,"+") ) {
						fnodetotex_tb((FNODE)FA1(f),tb);
						write_tb(opname,tb);
						fnodetotex_tb((FNODE)FA2(f),tb);
					} else if ( !strcmp(opname,"-") ) {
						if ( FA1(f) ) fnodetotex_tb((FNODE)FA1(f),tb);
						write_tb(opname,tb);
						fnodetotex_tb((FNODE)FA2(f),tb);
					} else if ( !strcmp(opname,"*") ) {
						fnodetotex_tb((FNODE)FA1(f),tb);
						write_tb(" ",tb);
						/* XXX special care for DP */
						f2 = (FNODE)FA2(f);
						if ( f2->id == I_EV ) {
							n = (NODE)FA0(f2);
							for ( i = 0; n; n = NEXT(n), i++ ) {
								fi = (FNODE)BDY(n);
								if ( fi->id != I_FORMULA || FA0(fi) )
									break;
							}
							if ( n )
								fnodetotex_tb((FNODE)FA2(f),tb);
						} else
							fnodetotex_tb((FNODE)FA2(f),tb);
					} else if ( !strcmp(opname,"/") ) {
						write_tb("\\frac{",tb);
						fnodetotex_tb((FNODE)FA1(f),tb);
						write_tb("} {",tb);
						fnodetotex_tb((FNODE)FA2(f),tb);
						write_tb("}",tb);
					} else if ( !strcmp(opname,"^") ) {
						fnodetotex_tb((FNODE)FA1(f),tb);
						write_tb("^{",tb);
						fnodetotex_tb((FNODE)FA2(f),tb);
						write_tb("} ",tb);
					} else if ( !strcmp(opname,"%") ) {
						fnodetotex_tb((FNODE)FA1(f),tb);
						write_tb(" {\\rm mod}\\, ",tb);
						fnodetotex_tb((FNODE)FA2(f),tb);
					} else
						error("invalid binary operator");

				case I_COP:
					switch( (cid)FA0(f) ) {
						case C_EQ:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" = ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case C_NE:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" \\neq ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case C_GT:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" \\gt ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case C_LT:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" \\lt ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case C_GE:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" \\geq ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case C_LE:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" \\leq ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
					}
					break;

				case I_LOP:
					switch( (lid)FA0(f) ) {
						case L_EQ:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" = ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case L_NE:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" \\neq ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case L_GT:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" \\gt ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case L_LT:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" \\lt ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case L_GE:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" \\geq ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case L_LE:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" \\leq ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case L_AND:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" {\\rm \\ and\\ } ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case L_OR:
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(" {\\rm \\ or\\ } ",tb);
							fnodetotex_tb((FNODE)FA2(f),tb);
							break;
						case L_NOT:
							/* XXX : L_NOT is a unary operator */
							write_tb("\\neg (",tb);
							fnodetotex_tb((FNODE)FA1(f),tb);
							write_tb(")",tb);
							return;
					}
					break;

				case I_AND:
					fnodetotex_tb((FNODE)FA0(f),tb);
					write_tb(" {\\rm \\ and\\ } ",tb);
					fnodetotex_tb((FNODE)FA1(f),tb);
					break;

				case I_OR:
					fnodetotex_tb((FNODE)FA0(f),tb);
					write_tb(" {\\rm \\ or\\ } ",tb);
					fnodetotex_tb((FNODE)FA1(f),tb);
					break;
			}
			break;

		/* ternary operators */
		case I_CE:
			error("fnodetotex_tb : not implemented yet");
			break;

		/* lists */
		case I_LIST:
			write_tb(" [ ",tb);
			n = (NODE)FA0(f);
			fnodenodetotex_tb(n,tb);
			write_tb("]",tb);
			break;

		/* function */
		case I_FUNC: case I_CAR: case I_CDR: case I_EV:
			switch ( f->id ) {
				case I_FUNC:
					opname = symbol_name(((FUNC)FA0(f))->name);
					write_tb(opname,tb);
					write_tb("(",tb);
					fargstotex_tb(opname,FA1(f),tb);
					write_tb(")",tb);
					break;
				case I_CAR:
					opname = symbol_name("car");
					write_tb(opname,tb);
					write_tb("(",tb);
					fargstotex_tb(opname,FA0(f),tb);
					write_tb(")",tb);
					break;
				case I_CDR:
					opname = symbol_name("cdr");
					write_tb(opname,tb);
					write_tb("(",tb);
					fargstotex_tb(opname,FA0(f),tb);
					write_tb(")",tb);
					break;
				case I_EV:
					n = (NODE)FA0(f);
					allzero = 1;
					for ( t0 = 0, i = 0; n; n = NEXT(n), i++ ) {
						fi = (FNODE)BDY(n);
						if ( fi->id == I_FORMULA && !FA0(fi) ) continue;
						allzero = 0;
						if ( fi->id == I_FORMULA && UNIQ(FA0(fi)) ) {
							sprintf(vname,"x_{%d}",i);
							len = strlen(vname);
							opname = MALLOC_ATOMIC(len+1);
							strcpy(opname,vname);
							write_tb(opname,tb);
						} else {
							sprintf(vname,"x_{%d}^{",i);
							len = strlen(vname);
							opname = MALLOC_ATOMIC(len+1);
							strcpy(opname,vname);
							write_tb(opname,tb);
							fnodetotex_tb((FNODE)BDY(n),tb);
							write_tb("} ",tb);
						}
					}
					/* XXX */
					if ( allzero )
						write_tb(" 1 ",tb);
					break;
			}
			break;

		case I_STR:
			write_tb((char *)FA0(f),tb);
			break;

		case I_FORMULA:
			obj = (Obj)FA0(f);
			if ( obj && OID(obj) == O_P ) {
				opname = symbol_name(VR((P)obj)->name);
			} else {
				len = estimate_length(CO,obj);
				opname = (char *)MALLOC_ATOMIC(len+1);
				soutput_init(opname);
				sprintexpr(CO,obj);
			}
			write_tb(opname,tb);
			break;

		case I_PVAR:
			if ( FA1(f) )
				error("fnodetotex_tb : not implemented yet");
			GETPVNAME(FA0(f),opname);
			write_tb(opname,tb);
			break;

		default:
			error("fnodetotex_tb : not implemented yet");
	}
}

void fnodenodetotex_tb(NODE n,TB tb)
{
	for ( ; n; n = NEXT(n) ) {
		fnodetotex_tb((FNODE)BDY(n),tb);
		if ( NEXT(n) ) write_tb(", ",tb);
	}
}

void fargstotex_tb(char *name,FNODE f,TB tb)
{
	NODE n;

	if ( !strcmp(name,"matrix") ) {
		error("fargstotex_tb : not implemented yet");	
	} else if ( !strcmp(name,"vector") ) {
		error("fargstotex_tb : not implemented yet");	
	} else {
		if ( f->id == I_LIST ) {
			n = (NODE)FA0(f);
			fnodenodetotex_tb(n,tb);
		} else
			fnodetotex_tb(f,tb);
	}
}
