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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/strobj.c,v 1.43 2004/03/15 06:44:50 noro Exp $
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

struct TeXSymbol {
	char *text;
	char *symbol;
};

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
void Pquotetotex_env();
void fnodetotex_tb(FNODE f,TB tb);
char *symbol_name(char *name);
char *conv_rule(char *name);
char *conv_subs(char *name);
char *call_convfunc(char *name);
void tb_to_string(TB tb,STRING *rp);
void fnodenodetotex_tb(NODE n,TB tb);
void fargstotex_tb(char *opname,FNODE f,TB tb);
int top_is_minus(FNODE f);

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
	{"quotetotex_env",Pquotetotex_env,-99999999},
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

int register_symbol_table(Obj arg);
int register_conv_rule(Obj arg);
int register_conv_func(Obj arg);
int register_dp_vars(Obj arg);
int register_dp_vars_prefix(Obj arg);
int register_dp_vars_hweyl(Obj arg);
int register_show_lt(Obj arg);
char *objtostr(Obj obj);
static struct TeXSymbol *user_texsymbol;
static char **dp_vars;
static int dp_vars_len;
static char *dp_vars_prefix;
static int show_lt;
static FUNC convfunc;
static int is_lt;
static int conv_flag;
static int dp_vars_hweyl;

#define CONV_TABLE (1U<<0)
#define CONV_SUBS (1U<<1)
#define CONV_DMODE (1U<<2)

static struct {
	char *name;
	Obj value;
	int (*reg)();
} qtot_env[] = {
	{"symbol_table",0,register_symbol_table},
	{"conv_rule",0,register_conv_rule},
	{"conv_func",0,register_conv_func},
	{"dp_vars",0,register_dp_vars},
	{"dp_vars_prefix",0,register_dp_vars_prefix},
	{"dp_vars_hweyl",0,register_dp_vars_hweyl},
	{"show_lt",0,register_show_lt},
	{0,0,0},
};

#define PARTIAL "\\partial"

char *conv_rule(char *name)
{
	char *body,*r;
	int len;

	if ( convfunc )
		name = call_convfunc(name);
	if ( conv_flag & CONV_TABLE ) {
		r = symbol_name(name);
		if ( r ) return r;
	}
	if ( (conv_flag & CONV_DMODE) && *name == 'd' ) {
		body = conv_rule(name+1);
		r = MALLOC_ATOMIC((strlen(PARTIAL)+strlen(body)+5)*sizeof(char));
		if ( !body || !(len=strlen(body)) )
			strcpy(r,PARTIAL);
		else if ( len == 1 )
			sprintf(r,"%s_%s",PARTIAL,body);
		else
			sprintf(r,"%s_{%s}",PARTIAL,body);
		return r;
	} else
		return conv_subs(name);
}

int _is_delimiter(char c)
{
	if ( (c == ' ' || c == '_' || c == ',') ) return 1;
	else return 0;
}

int _is_alpha(char c)
{
	if ( isdigit(c) || c == '{' || _is_delimiter(c) ) return 0;
	else return 1;
}

char *conv_subs(char *name)
{
	int i,j,k,len,clen,slen,start,level;
	char *buf,*head,*r,*h,*brace,*buf_conv;
	char **subs;

	if ( !name || !(len=strlen(name)) ) return "";
	if ( !(conv_flag&CONV_SUBS) ) return name;
	subs = (char **)ALLOCA(len*sizeof(char* ));
	for ( i = 0, j = 0, start = i; ; j++ ) {
		while ( (i < len) && _is_delimiter(name[i]) ) i++;
		start = i;
		if ( i == len ) break;
		if ( name[i] == '{' ) {
			for ( level = 1, i++; i < len && level; i++ ) {
				if ( name[i] == '{' ) level++;
				else if ( name[i] == '}' ) level--;
			}
			slen = i-start;	
			if ( slen >= 3 ) {
				brace = (char *)ALLOCA((slen+1)*sizeof(char));
				strncpy(brace,name+start+1,slen-2);
				brace[slen-2] = 0;
				buf = conv_subs(brace);
				subs[j] = (char *)ALLOCA((strlen(buf)+3)*sizeof(char));
				if ( strlen(buf) == 1 )
					strcpy(subs[j],buf);
				else	
					sprintf(subs[j],"{%s}",buf);
			} else
				subs[j] = "{}";
		} else {
			if ( isdigit(name[i]) )
				while ( i < len && isdigit(name[i]) ) i++;
			else
				while ( i < len && _is_alpha(name[i]) ) i++;
			slen = i-start;	
			buf = (char *)ALLOCA((slen+1)*sizeof(char));
			strncpy(buf,name+start,slen); buf[slen] = 0;
			buf_conv = symbol_name(buf);
			subs[j] = buf_conv?buf_conv:buf;
		}
	}
	for ( k = 0, clen = 0; k < j; k++ ) clen += strlen(subs[k]);
	/* {subs(0)}_{{subs(1)},...,{subs(j-1)}} => {}:j+1 _:1 ,:j-2 */
	h = r = MALLOC_ATOMIC((clen+(j+1)*2+1+(j-2)+1)*sizeof(char));
	if ( j == 1 )
		sprintf(h,"{%s}",subs[0]);
	else {
		sprintf(h,"{%s}_{%s",subs[0],subs[1]);
		h += strlen(h);
		for ( k = 2; k < j; k++ ) {
			sprintf(h,",%s",subs[k]);
			h += strlen(h);
		}
		strcpy(h,"}");
	}
	return r;
}

char *call_convfunc(char *name)
{
	STRING str,r;
	NODE arg;

	MKSTR(str,name);
	arg = mknode(1,str);
	r = (STRING)bevalf(convfunc,arg);
	if ( !r || OID(r) != O_STR )
		error("call_convfunc : invalid result");
	return BDY(r);
}

int register_symbol_table(Obj arg)
{
	NODE n,t;
	Obj b;
	STRING a0,a1;
	struct TeXSymbol *uts;
	int i,len;

	/* check */
	if ( !arg ) {
		user_texsymbol = 0;
		return 1;
	}
	if ( OID(arg) != O_LIST ) return 0;

	n = BDY((LIST)arg);
	len = length(n);
	uts = (struct TeXSymbol *)MALLOC((len+1)*sizeof(struct TeXSymbol));
	for ( i = 0; n; n = NEXT(n), i++ ) {
		b = (Obj)BDY(n);
		if ( !b || OID(b) != O_LIST ) return 0;
		t = BDY((LIST)b);
		if ( !t || !NEXT(t) ) return 0;
		a0 = (STRING)BDY(t);
		a1 = (STRING)BDY(NEXT(t));
		if ( !a0 ) return 0;
		if ( OID(a0) == O_STR )
			uts[i].text = BDY(a0);	
		else if ( OID(a0) == O_P )
			uts[i].text = NAME(VR((P)a0));	
		else
			return 0;
		if ( !a1 ) return 0;
		if ( OID(a1) == O_STR )
			uts[i].symbol = BDY(a1);	
		else if ( OID(a1) == O_P )
			uts[i].symbol = NAME(VR((P)a1));	
		else
			return 0;
	}
	uts[i].text = 0;
	uts[i].symbol = 0;
	user_texsymbol = uts;	
	return 1;
}

int register_dp_vars_hweyl(Obj arg)
{
	if ( INT(arg) ) {
		dp_vars_hweyl = QTOS((Q)arg);
		return 1;
	} else return 0;
}

int register_show_lt(Obj arg)
{
	if ( INT(arg) ) {
		show_lt = QTOS((Q)arg);
		return 1;
	} else return 0;
}

int register_conv_rule(Obj arg)
{
	if ( INT(arg) ) {
		conv_flag = QTOS((Q)arg);
		convfunc = 0;
		return 1;
	} else return 0;
}

int register_conv_func(Obj arg)
{
	if ( !arg ) {
		convfunc = 0;
		return 1;
	} else if ( OID(arg) == O_P && (int)(VR((P)arg))->attr == V_SR ) {
		convfunc = (FUNC)(VR((P)arg)->priv);
		/* f must be a function which takes single argument */
		return 1;
	} else return 0;
}

int register_dp_vars(Obj arg)
{
	int l,i;
	char **r;
	NODE n;
	STRING a;

	if ( !arg ) {
		dp_vars = 0;
		dp_vars_len = 0;
		return 1;
	} else if ( OID(arg) != O_LIST )
		return 0;
	else {
		n = BDY((LIST)arg);
		l = length(n);
		r = (char **)MALLOC_ATOMIC(l*sizeof(char *));
		for ( i = 0; i < l; i++, n = NEXT(n) ) {
			a = (STRING)BDY(n);
			if ( !a ) return 0;
			if ( OID(a) == O_STR )
				r[i] = BDY(a);	
			else if ( OID(a) == O_P )
				r[i] = NAME(VR((P)a));	
			else
				return 0;
		}
		dp_vars = r;
		dp_vars_len = l;
		return 1;
	}
}

int register_dp_vars_prefix(Obj arg)
{
	if ( !arg ) {
		dp_vars_prefix = 0;
		return 1;
	} else if ( OID(arg) == O_STR ) {
		dp_vars_prefix = BDY((STRING)arg);
		return 1;
	} else if ( OID(arg) == O_P ) {
		dp_vars_prefix = NAME(VR((P)arg));
		return 1;
	} else return 0;
}

void Pquotetotex_env(NODE arg,Obj *rp)
{
	int ac,i;
	char *name;
	NODE n,n0;
	STRING s;
	LIST l;

	ac = argc(arg);
	if ( !ac ) {
		n0 = 0;
		for ( i = 0; qtot_env[i].name; i++ ) {
			NEXTNODE(n0,n); MKSTR(s,qtot_env[i].name); BDY(n) = (pointer)s;
			NEXTNODE(n0,n); BDY(n) = (Q)qtot_env[i].value;
		}
		NEXT(n) = 0;
		MKLIST(l,n0);
		*rp = (Obj)l;
	} else if ( ac == 1 && !ARG0(arg) ) {
		/* set to default */
		for ( i = 0; qtot_env[i].name; i++ ) {
			(qtot_env[i].reg)(0);
			qtot_env[i].value = 0;
		}
		*rp = 0;
	} else if ( ac == 1 || ac == 2 ) {
		asir_assert(ARG0(arg),O_STR,"quotetotex_env");
		name = BDY((STRING)ARG0(arg));
		for ( i = 0; qtot_env[i].name; i++ )
			if ( !strcmp(qtot_env[i].name,name) ) {
				if ( ac == 2 ) {
					if ( (qtot_env[i].reg)((Obj)ARG1(arg)) )
						qtot_env[i].value = (Obj)ARG1(arg);
					else
						error("quotetotex_env : invalid argument");
				}
				*rp = qtot_env[i].value;
				return;
			}
		*rp = 0;
	} else
		*rp = 0;
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
	/* XXX for DP */
	is_lt = 1;
	fnodetotex_tb(BDY((QUOTE)ARG0(arg)),tb);
	tb_to_string(tb,rp);
}

void Pquotetotex_tb(NODE arg,Q *rp)
{
	int i;
	TB tb;

	asir_assert(ARG1(arg),O_TB,"quotetotex_tb");
	/* XXX for DP */
	is_lt = 1;
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
 {0,0}
};

char *symbol_name(char *name)
{
	int i;

	if ( !name || strlen(name) == 0 )
		return "";
	if ( !(conv_flag & CONV_TABLE) )
		return name;

	if ( user_texsymbol )
		for ( i = 0; user_texsymbol[i].text; i++ )
			if ( !strcmp(user_texsymbol[i].text,name) )
				return user_texsymbol[i].symbol;
	for ( i = 0; texsymbol[i].text; i++ )
		if ( !strcmp(texsymbol[i].text,name) )
			return texsymbol[i].symbol;
	return 0;
}

void fnodetotex_tb(FNODE f,TB tb)
{
	NODE n,t,t0;
	char vname[BUFSIZ],prefix[BUFSIZ];
	char *opname,*vname_conv,*prefix_conv;
	Obj obj;
	int i,len,allzero,elen,elen2;
	C cplx;
	char *r;
	FNODE fi,f2;

	write_tb(" ",tb);
	if ( !f ) {
		write_tb("0",tb);
		return;
	}
	switch ( f->id ) {
		/* unary operators */
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

		/* binary operators */
		/* arg list */
		/* I_AND, I_OR => FA0(f), FA1(f) */
		/* otherwise   => FA1(f), FA2(f) */
		case I_BOP:
			opname = ((ARF)FA0(f))->name;
			if ( !strcmp(opname,"+") ) {
				fnodetotex_tb((FNODE)FA1(f),tb);
				if ( !top_is_minus((FNODE)FA2(f)) ) write_tb(opname,tb);
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
			break;

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
		case I_FUNC:
			if ( !strcmp(((FUNC)FA0(f))->name,"@pi") )
				write_tb("\\pi",tb);
			else if ( !strcmp(((FUNC)FA0(f))->name,"@e") )
				write_tb("e",tb);
			else {
				opname = conv_rule(((FUNC)FA0(f))->name);
				write_tb(opname,tb);
				write_tb("(",tb);
				fargstotex_tb(opname,FA1(f),tb);
				write_tb(")",tb);
			}
			break;

		/* XXX */
		case I_CAR:
			opname = conv_rule("car");
			write_tb(opname,tb);
			write_tb("(",tb);
			fargstotex_tb(opname,FA0(f),tb);
			write_tb(")",tb);
			break;

		case I_CDR:
			opname = conv_rule("cdr");
			write_tb(opname,tb);
			write_tb("(",tb);
			fargstotex_tb(opname,FA0(f),tb);
			write_tb(")",tb);
			break;

		/* exponent vector */
		case I_EV:
			n = (NODE)FA0(f);
			if ( dp_vars_hweyl ) {
				elen = length(n);
				elen2 = elen>>1;
				elen = elen2<<1;
			}
			allzero = 1;
			if ( show_lt && is_lt )
				write_tb("\\underline{",tb);
			for ( t0 = 0, i = 0; n; n = NEXT(n), i++ ) {
				fi = (FNODE)BDY(n);
				if ( fi->id == I_FORMULA && !FA0(fi) ) continue;
				allzero = 0;
				if ( dp_vars && i < dp_vars_len ) {
					strcpy(vname,dp_vars[i]);
					vname_conv = conv_rule(vname);
				} else {
					if ( dp_vars_hweyl ) {
						if ( i < elen2 ) {
							strcpy(prefix,dp_vars_prefix?dp_vars_prefix:"x");
							prefix_conv = conv_rule(prefix);
							vname_conv = (char *)ALLOCA(strlen(prefix_conv)+50);
							sprintf(vname_conv,i<10?"%s_%d":"%s_{%d}",
								prefix_conv,i);
						} else if ( i < elen ) {
							strcpy(prefix,"\\partial");
							prefix_conv = conv_rule(prefix);
							vname_conv = (char *)ALLOCA(strlen(prefix_conv)+50);
							sprintf(vname_conv,i<10?"%s_%d":"%s_{%d}",
								prefix_conv,i-elen2);
						} else {
							strcpy(prefix,"h");
							vname_conv = conv_rule(prefix);
						}
					} else {
						strcpy(prefix,dp_vars_prefix?dp_vars_prefix:"x");
						prefix_conv = conv_rule(prefix);
						vname_conv = (char *)ALLOCA(strlen(prefix_conv)+50);
						sprintf(vname_conv,i<10?"%s_%d":"%s_{%d}",
							prefix_conv,i);
					}
				}
				if ( fi->id == I_FORMULA && UNIQ(FA0(fi)) ) {
					len = strlen(vname_conv);
					opname = MALLOC_ATOMIC(len+2);
					sprintf(opname,"%s ",vname_conv);
					write_tb(opname,tb);
				} else {
					len = strlen(vname_conv);
					/* 2: ^{ */
					opname = MALLOC_ATOMIC(len+1+2);
					sprintf(opname,"%s^{",vname_conv);
					write_tb(opname,tb);
					fnodetotex_tb((FNODE)BDY(n),tb);
					write_tb("} ",tb);
				}
			}
			/* XXX */
			if ( allzero )
				write_tb(" 1 ",tb);
			if ( show_lt && is_lt ) {
				write_tb("}",tb);	
				is_lt = 0;
			}
			break;

		/* string */
		case I_STR:
			write_tb((char *)FA0(f),tb);
			break;

		/* internal object */
		case I_FORMULA:
			obj = (Obj)FA0(f);
			if ( !obj )
				write_tb("0",tb);
			else if ( OID(obj) == O_N && NID(obj) == N_C ) {
				cplx = (C)obj;
				write_tb("(",tb);
				if ( cplx->r ) {
					r = objtostr((Obj)cplx->r); write_tb(r,tb);
				}
				if ( cplx->i ) {
					if ( cplx->r && compnum(0,cplx->i,0) > 0 ) {
						write_tb("+",tb);
						if ( !UNIQ(cplx->i) ) {
							r = objtostr((Obj)cplx->i); write_tb(r,tb);
						}
					} else if ( MUNIQ(cplx->i) )
						write_tb("-",tb);
					else if ( !UNIQ(cplx->i) ) {
						r = objtostr((Obj)cplx->i); write_tb(r,tb);
					}
					write_tb("\\sqrt{-1}",tb);
				}
				write_tb(")",tb);
			} else if ( OID(obj) == O_P )
				write_tb(conv_rule(VR((P)obj)->name),tb);
			else
				write_tb(objtostr(obj),tb);
			break;

		/* program variable */
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

char *objtostr(Obj obj)
{
	int len;
	char *r;

	len = estimate_length(CO,obj);
	r = (char *)MALLOC_ATOMIC(len+1);
	soutput_init(r);
	sprintexpr(CO,obj);
	return r;
}

void fnodenodetotex_tb(NODE n,TB tb)
{
	for ( ; n; n = NEXT(n) ) {
		is_lt = 1;
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

int top_is_minus(FNODE f)
{
	char *opname;
	int len;
	Obj obj;

	if ( !f )
		return 0;
	switch ( f->id ) {
		case I_MINUS:
			return 1;
		case I_BOP:
			opname = ((ARF)FA0(f))->name;
			switch ( opname[0] ) {
				case '+': case '*': case '/': case '^': case '%': 
					return top_is_minus((FNODE)FA1(f));
				case '-':
					if ( FA1(f) )
						return top_is_minus((FNODE)FA1(f));
					else
						return 1;
				default:
					return 0;
			}
			break;
		case I_COP:
			return top_is_minus((FNODE)FA1(f));
		case I_LOP:
			if ( (lid)FA0(f) == L_NOT ) return 0;
			else return top_is_minus((FNODE)FA1(f));
		case I_AND: case I_OR:
			return top_is_minus((FNODE)FA0(f));
		case I_FORMULA:
			obj = (Obj)FA0(f);
			if ( !obj )
				return 0;
			else {
				switch ( OID(obj) ) {
					case O_N:
						return mmono((P)obj);
					case O_P:
						/* must be a variable */
						opname = conv_rule(VR((P)obj)->name);
						return opname[0]=='-';
					default:
						/* ??? */
						len = estimate_length(CO,obj);
						opname = (char *)MALLOC_ATOMIC(len+1);
						soutput_init(opname);
						sprintexpr(CO,obj);
						return opname[0]=='-';
				}
			}
		default:
			return 0;
	}
}
