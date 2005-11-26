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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/strobj.c,v 1.100 2005/11/25 07:18:31 noro Exp $
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

#if defined(__GNUC__)
#define INLINE inline
#elif defined(VISUAL)
#define INLINE __inline
#else
#define INLINE
#endif

struct TeXSymbol {
	char *text;
	char *symbol;
};

#define OPNAME(f) (((ARF)FA0(f))->name[0])
#define IS_ZERO(f) (((f)->id==I_FORMULA) && FA0(f)==0 )
#define IS_BINARYPWR(f) (((f)->id==I_BOP) &&(OPNAME(f)=='^'))
#define IS_NARYADD(f) (((f)->id==I_NARYOP) &&(OPNAME(f)=='+'))
#define IS_NARYMUL(f) (((f)->id==I_NARYOP) &&(OPNAME(f)=='*'))

extern char *parse_strp;

void Psprintf();
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
void Pflatten_quote();

void Pquote_is_integer(),Pquote_is_rational(),Pquote_is_number();
void Pquote_is_dependent(),Pquote_is_function();
void Pquote_normalize();
void Pnquote_comp();
void Pnquote_match();

void Pquote_to_nbp();
void Pshuffle_mul(), Pharmonic_mul();
void Pnbp_hm(), Pnbp_ht(), Pnbp_hc(), Pnbp_rest(), Pnbm_hp(), Pnbm_rest();

void Pquote_to_funargs(),Pfunargs_to_quote(),Pget_function_name();
void Pquote_match(),Pget_quote_id(),Pquote_match_rewrite();
void Pquote_to_nary(),Pquote_to_bin();
void fnode_do_assign(NODE arg);
void do_assign(NODE arg);
void fnodetotex_tb(FNODE f,TB tb);
char *symbol_name(char *name);
char *conv_rule(char *name);
char *conv_subs(char *name);
char *call_convfunc(char *name);
void tb_to_string(TB tb,STRING *rp);
void fnodenodetotex_tb(NODE n,TB tb);
void fargstotex_tb(char *opname,FNODE f,TB tb);
int top_is_minus(FNODE f);
int quote_match(Obj f,Obj pat,NODE *rp);

struct ftab str_tab[] = {
	{"sprintf",Psprintf,-99999999},
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
	{"get_quote_id",Pget_quote_id,1},

	{"quote_is_number",Pquote_is_number,1},
	{"quote_is_rational",Pquote_is_rational,1},
	{"quote_is_integer",Pquote_is_integer,1},
	{"quote_is_function",Pquote_is_function,1},
	{"quote_is_dependent",Pquote_is_dependent,2},

	{"quote_normalize",Pquote_normalize,-2},
	{"quote_match",Pquote_match,2},
	{"quote_match_rewrite",Pquote_match_rewrite,-4},

	{"nquote_comp",Pnquote_comp,2},
	{"nquote_match",Pnquote_match,2},
	{"quote_to_nbp",Pquote_to_nbp,1},
	{"shuffle_mul",Pshuffle_mul,2},
	{"harmonic_mul",Pharmonic_mul,2},

	{"nbp_hm", Pnbp_hm,1},
	{"nbp_ht", Pnbp_ht,1},
	{"nbp_hc", Pnbp_hc,1},
	{"nbp_rest", Pnbp_rest,1},
	{"nbm_hp", Pnbm_hp,1},
	{"nbm_rest", Pnbm_rest,1},

	{"quote_to_nary",Pquote_to_nary,1},
	{"quote_to_bin",Pquote_to_bin,2},

	{"quotetotex_tb",Pquotetotex_tb,2},
	{"quotetotex",Pquotetotex,1},
	{"quotetotex_env",Pquotetotex_env,-99999999},
	{"flatten_quote",Pflatten_quote,-2},
	{"quote_to_funargs",Pquote_to_funargs,1},
	{"funargs_to_quote",Pfunargs_to_quote,1},
	{"get_function_name",Pget_function_name,1},
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
int register_dp_vars_origin(Obj arg);
int register_dp_dvars_origin(Obj arg);
int register_dp_dvars_prefix(Obj arg);
int register_dp_vars_prefix(Obj arg);
int register_dp_vars_hweyl(Obj arg);
int register_show_lt(Obj arg);
char *objtostr(Obj obj);
static struct TeXSymbol *user_texsymbol;
static char **dp_vars;
static int dp_vars_len;
static char *dp_vars_prefix;
static char *dp_dvars_prefix;
static int dp_vars_origin;
static int dp_dvars_origin;
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
	{"dp_dvars_prefix",0,register_dp_dvars_prefix},
	{"dp_vars_origin",0,register_dp_vars_origin},
	{"dp_dvars_origin",0,register_dp_dvars_origin},
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

int register_dp_vars_origin(Obj arg)
{
	if ( INT(arg) ) {
		dp_vars_origin = QTOS((Q)arg);
		return 1;
	} else return 0;
}

int register_dp_dvars_origin(Obj arg)
{
	if ( INT(arg) ) {
		dp_dvars_origin = QTOS((Q)arg);
		return 1;
	} else return 0;
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

int register_dp_dvars_prefix(Obj arg)
{
	if ( !arg ) {
		dp_dvars_prefix = 0;
		return 1;
	} else if ( OID(arg) == O_STR ) {
		dp_dvars_prefix = BDY((STRING)arg);
		return 1;
	} else if ( OID(arg) == O_P ) {
		dp_dvars_prefix = NAME(VR((P)arg));
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

FNODE partial_eval(FNODE), fnode_to_nary(FNODE), fnode_to_bin(FNODE,int);

void Pquote_to_nary(NODE arg,QUOTE *rp)
{
	FNODE f;

	f = fnode_to_nary(BDY((QUOTE)ARG0(arg)));
	MKQUOTE(*rp,f);	
}

void Pquote_to_bin(NODE arg,QUOTE *rp)
{
	FNODE f;
	int direction;

	direction = QTOS((Q)ARG1(arg));
	f = fnode_to_bin(BDY((QUOTE)ARG0(arg)),direction);

	MKQUOTE(*rp,f);	
}

void Pquote_is_number(NODE arg,Q *rp)
{
	QUOTE q;
	int ret;

	q = (QUOTE)ARG0(arg);
	asir_assert(q,O_QUOTE,"quote_is_number");
	ret = fnode_is_number(BDY(q));
	STOQ(ret,*rp);
}

void Pquote_is_rational(NODE arg,Q *rp)
{
	QUOTE q;
	int ret;

	q = (QUOTE)ARG0(arg);
	asir_assert(q,O_QUOTE,"quote_is_rational");
	ret = fnode_is_rational(BDY(q));
	STOQ(ret,*rp);
}

void Pquote_is_integer(NODE arg,Q *rp)
{
	QUOTE q;
	int ret;

	q = (QUOTE)ARG0(arg);
	asir_assert(q,O_QUOTE,"quote_is_integer");
	ret = fnode_is_integer(BDY(q));
	STOQ(ret,*rp);
}

void Pquote_is_function(NODE arg,Q *rp)
{
	QUOTE q;
	int ret;

	q = (QUOTE)ARG0(arg);
	asir_assert(q,O_QUOTE,"quote_is_function");
	if ( q->id == I_FUNC || q->id == I_IFUNC )
		ret = 1;
	else
		ret = 0;
	STOQ(ret,*rp);
}

void Pquote_is_dependent(NODE arg,Q *rp)
{
	P x;
	QUOTE q,v;
	int ret;
	V var;

	q = (QUOTE)ARG0(arg);
	v = (QUOTE)ARG1(arg);
	asir_assert(q,O_QUOTE,"quote_is_dependent");
	asir_assert(v,O_QUOTE,"quote_is_dependent");
	x = (P)eval(BDY(v));
	if ( !x || OID(x) != O_P )
		*rp = 0;
	var = VR(x);
	ret = fnode_is_dependent(BDY(q),var);
	STOQ(ret,*rp);
}


void Pquote_match(NODE arg,Q *rp)
{
	FNODE f,g;
	Obj obj;
	QUOTE q;
	NODE r;
	int ret;

#if 0
	g = partial_eval(BDY(((QUOTE)ARG0(arg))));
	MKQUOTE(q,g);
	ret = quote_match((Obj)q,(Obj)ARG1(arg),&r);
#else
	obj = (Obj)ARG0(arg);
	ret = quote_match(obj,(Obj)ARG1(arg),&r);
#endif
	if ( ret ) {
		do_assign(r);
		*rp = ONE;
	} else
		*rp = 0;
}

void Pnquote_match(NODE arg,Q *rp)
{
	QUOTE fq,pq;
	FNODE f,p;
	int ret;
	NODE r;

	fq = (QUOTE)ARG0(arg); Pquote_normalize(mknode(2,fq,0),&fq); f = (FNODE)BDY(fq);
	pq = (QUOTE)ARG1(arg); Pquote_normalize(mknode(2,pq,0),&pq); p = (FNODE)BDY(pq);
	ret = nfnode_match(f,p,&r);
	if ( ret ) {
		fnode_do_assign(r);
		*rp = ONE;
	} else
		*rp = 0;
}

FNODE rewrite_fnode(FNODE,NODE);

extern Obj VOIDobj;

void Pquote_match_rewrite(NODE arg,Obj *rp)
{
	FNODE f,g,h,c,value;
	Obj obj;
	QUOTE q;
	NODE r,t,s,s0,pair;
	int ret,ind,ac;

	obj = (Obj)ARG0(arg);
	ret = quote_match(obj,(Obj)ARG1(arg),&r);
	if ( ret ) {
		for ( t = r, s0 = 0; t; t = NEXT(t) ) {
			NEXTNODE(s0,s);
			pair = BDY((LIST)BDY(t));
			ind = (int)FA0((FNODE)BDY((QUOTE)BDY(pair)));
			value = BDY((QUOTE)(BDY(NEXT(pair))));
			BDY(s) = mknode(2,ind,value);
		}
		if ( s0 ) NEXT(s) = 0;
		switch ( ac = argc(arg) ) {
			case 3:
				h = rewrite_fnode(BDY((QUOTE)ARG2(arg)),s0);
				MKQUOTE(q,h); *rp = (Obj)q;
				break;
			case 4:
				c = rewrite_fnode(BDY((QUOTE)ARG2(arg)),s0);
				if ( eval(c) ) {
					h = rewrite_fnode(BDY((QUOTE)ARG3(arg)),s0);
					MKQUOTE(q,h); *rp = (Obj)q;
				} else
					*rp = VOIDobj;
				break;
			default:
				error("quote_match_rewrite : invalid argument");
		}
	} else
		*rp = VOIDobj;
}

void do_assign(NODE arg)
{
	NODE t,pair;
	int pv;

	QUOTE value;

	for ( t = arg; t; t = NEXT(t) ) {
		pair = BDY((LIST)BDY(t));
		pv = (int)FA0((FNODE)BDY((QUOTE)BDY(pair)));
		value = (QUOTE)(BDY(NEXT(pair)));
		ASSPV(pv,value);
	}
}

/* [[index,fnode],...] */

void fnode_do_assign(NODE arg)
{
	NODE t,pair;
	int pv;
	FNODE f;
	QUOTE value;
	QUOTEARG qa;

	for ( t = arg; t; t = NEXT(t) ) {
		pair = (NODE)BDY(t);
		pv = (int)BDY(pair);
		f = (FNODE)(BDY(NEXT(pair)));
		if ( f->id == I_FUNC_HEAD ) {
			/* XXX : I_FUNC_HEAD is a dummy id to pass FUNC */
			MKQUOTEARG(qa,A_func,FA0(f));
			value = (QUOTE)qa;
		} else
			MKQUOTE(value,f);
		ASSPV(pv,value);
	}
}

/* 
/* consistency check and merge
 */

int merge_matching_node(NODE n,NODE a,NODE *rp)
{
	NODE ta,ba,tn,bn;
	QUOTE pa,va,pn,vn;

	if ( !n ) {
		*rp = a;
		return 1;
	}
	for ( ta = a; ta; ta = NEXT(ta) ) {
		ba = BDY((LIST)BDY(ta));
		if ( !ba ) continue;
		pa = (QUOTE)BDY(ba); va = (QUOTE)BDY(NEXT(ba));
		for ( tn = n; tn; tn = NEXT(tn) ) {
			bn = BDY((LIST)BDY(tn));	
			if ( !bn ) continue;
			pn = (QUOTE)BDY(bn); vn = (QUOTE)BDY(NEXT(bn));
			if ( !compquote(CO,pa,pn) ) {
				if ( !compquote(CO,va,vn) ) break;
				else return 0;
			}
		}
		if ( !tn ) {
			MKNODE(tn,(pointer)BDY(ta),n);
			n = tn;
		}
	}
	*rp = n;
	return 1;
}

int quote_match_node(NODE f,NODE pat,NODE *rp) {
	NODE r,a,tf,tp,r1;
	int ret;

	if ( length(f) != length(pat) ) return 0;
	r = 0;
	for ( tf = f, tp = pat; tf; tf = NEXT(tf), tp = NEXT(tp) ) {
		ret = quote_match((Obj)BDY(tf),(Obj)BDY(tp),&a);
		if ( !ret ) return 0;
		ret = merge_matching_node(r,a,&r1);
		if ( !ret ) return 0;
		else r = r1;
	}
	*rp = r;
	return 1;
}

/* f = [a,b,c,...] pat = [X,Y,...] rpat matches the rest of f */

int quote_match_cons(NODE f,NODE pat,Obj rpat,NODE *rp) {
	QUOTE q;
	Q id;
	FNODE fn;
	NODE r,a,tf,tp,r1,arg;
	int ret;
	LIST list,alist;

	/* matching of the head part */
	if ( length(f) < length(pat) ) return 0;
	r = 0;
	for ( tf = f, tp = pat; tp; tf = NEXT(tf), tp = NEXT(tp) ) {
		ret = quote_match((Obj)BDY(tf),(Obj)BDY(tp),&a);
		if ( !ret ) return 0;
		ret = merge_matching_node(r,a,&r1);
		if ( !ret ) return 0;
		else r = r1;
	}
	/* matching of the rest */
	MKLIST(list,tf);
	STOQ(I_LIST,id); a = mknode(2,id,list);
	MKLIST(alist,a);
	arg = mknode(1,alist);
	Pfunargs_to_quote(arg,&q);
	ret = quote_match((Obj)q,rpat,&a);
	if ( !ret ) return 0;
	ret = merge_matching_node(r,a,&r1);
	if ( !ret ) return 0;
	*rp = r1;
	return 1;
}

void get_quote_id_arg(QUOTE f,int *id,NODE *r)
{
	LIST fa;
	NODE arg,fab;

	arg = mknode(1,f); Pquote_to_funargs(arg,&fa); fab = BDY((LIST)fa);
	*id = QTOS((Q)BDY(fab)); *r = NEXT(fab);
}

/* *rp : [[quote(A),quote(1)],...] */

int quote_match(Obj f, Obj pat, NODE *rp)
{
	NODE tf,tp,head,body;
	NODE parg,farg,r;
	Obj rpat;
	LIST fa,l;
	int pid,id;
	FUNC ff,pf;
	int ret;
	QUOTE q;
	FNODE g;

	if ( !f )
		if ( !pat ) {
			*rp = 0; return 1;
		} else
			return 0;
	else if ( OID(pat) == O_LIST ) {
		if ( OID(f) == O_LIST )
			return quote_match_node(BDY((LIST)f),BDY((LIST)pat),rp);
		else
			return 0;
	} else if ( OID(pat) == O_QUOTE ) {
		pid = ((FNODE)BDY((QUOTE)pat))->id;
		switch ( pid ) {
			case I_FORMULA:
				if ( compquote(CO,f,pat) )
					return 0;
				else {
					*rp = 0; return 1;
				}
				break;

			case I_LIST: case I_CONS:
				get_quote_id_arg((QUOTE)pat,&pid,&parg);
				if ( OID(f) == O_LIST )
					tf = BDY((LIST)f);
				else if ( OID(f) == O_QUOTE 
					&& ((FNODE)BDY((QUOTE)f))->id == pid ) {
					get_quote_id_arg((QUOTE)f,&id,&farg);
					tf = BDY((LIST)BDY(farg));
				} else
					return 0;

				tp = BDY((LIST)BDY(parg));
				if ( pid == I_LIST )
					return quote_match_node(tf,tp,rp);
				else {
					rpat = (Obj)BDY(NEXT(parg));
					return quote_match_cons(tf,tp,rpat,rp);
				}

			case I_PVAR:
				/* [[pat,f]] */
				r = mknode(2,pat,f); MKLIST(l,r);
				*rp =  mknode(1,l);
				return 1;

			case I_IFUNC:
				/* F(X,Y,...) = ... */
				get_quote_id_arg((QUOTE)f,&id,&farg);
				get_quote_id_arg((QUOTE)pat,&pid,&parg);
				if ( id == I_FUNC ) {
					r = mknode(2,BDY(parg),BDY(farg)); MKLIST(l,r);
					head = mknode(1,l);
					ret = quote_match(BDY(NEXT(farg)),
								BDY(NEXT(parg)),&body);
					if ( !ret ) return 0;
					else return merge_matching_node(head,body,rp);
				} else
					return 0;

			case I_NARYOP: case I_BOP: case I_FUNC:
				/* X+Y = ... */
				/* f(...) = ... */
				if ( OID(f) != O_QUOTE ) return 0;
				id = ((FNODE)BDY((QUOTE)f))->id;
				if ( pid == I_FUNC )
					;
				else {
					/* XXX converting to I_BOP */
					if ( pid == I_NARYOP ) {
						g = fnode_to_bin(BDY((QUOTE)pat),1);
						MKQUOTE(q,g); pat = (Obj)q;
					}
					if ( id == I_NARYOP ) {
						g = fnode_to_bin(BDY((QUOTE)f),1);
						MKQUOTE(q,g); f = (Obj)q;
					}
				}
				get_quote_id_arg((QUOTE)pat,&pid,&parg);
				get_quote_id_arg((QUOTE)f,&id,&farg);
				if ( compqa(CO,BDY(farg),BDY(parg)) ) return 0;
				return quote_match_node(NEXT(farg),NEXT(parg),rp);

			default:
				if ( OID(f) != O_QUOTE ) return 0;
				id = ((FNODE)BDY((QUOTE)f))->id;
				if ( id != pid ) return 0;
				get_quote_id_arg((QUOTE)pat,&pid,&parg);
				get_quote_id_arg((QUOTE)f,&id,&farg);
				return quote_match_node(farg,parg,rp);
		}
	}
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

void Pget_function_name(NODE arg,STRING *rp)
{
		QUOTEARG qa;
		ARF f;
		char *opname;

		qa = (QUOTEARG)BDY(arg);
		if ( !qa || OID(qa) != O_QUOTEARG || qa->type != A_arf )
			*rp = 0;
		else {
			f = (ARF)BDY(qa);
			opname = f->name;
			MKSTR(*rp,opname);
		}
}

FNODE strip_paren(FNODE);

void fnodetotex_tb(FNODE f,TB tb)
{
	NODE n,t,t0,args;
	char vname[BUFSIZ],prefix[BUFSIZ];
	char *opname,*vname_conv,*prefix_conv;
	Obj obj;
	int i,len,allzero,elen,elen2,si;
	C cplx;
	char *r;
	FNODE fi,f2,f1;

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
			switch ( opname[0] ) {
				case '+':
					fnodetotex_tb((FNODE)FA1(f),tb);
					if ( !top_is_minus((FNODE)FA2(f)) ) write_tb(opname,tb);
					fnodetotex_tb((FNODE)FA2(f),tb);
					break;
				case '-':
					if ( FA1(f) ) fnodetotex_tb((FNODE)FA1(f),tb);
					write_tb(opname,tb);
					fnodetotex_tb((FNODE)FA2(f),tb);
					break;
				case '*':
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
					break;
				case '/':
					write_tb("\\frac{",tb);
					fnodetotex_tb((FNODE)FA1(f),tb);
					write_tb("} {",tb);
					fnodetotex_tb((FNODE)FA2(f),tb);
					write_tb("}",tb);
					break;
				case '^':
					fnodetotex_tb((FNODE)FA1(f),tb);
					write_tb("^{",tb);
					fnodetotex_tb(strip_paren((FNODE)FA2(f)),tb);
					write_tb("} ",tb);
					break;
				case '%':
					fnodetotex_tb((FNODE)FA1(f),tb);
					write_tb(" {\\rm mod}\\, ",tb);
					fnodetotex_tb((FNODE)FA2(f),tb);
					break;
				default:
					error("invalid binary operator");
					break;
			}
			break;
		case I_NARYOP:
			args = (NODE)FA1(f);
			write_tb("(",tb);
			switch ( OPNAME(f) ) {
				case '+':
					fnodetotex_tb((FNODE)BDY(args),tb);
					for ( args = NEXT(args); args; args = NEXT(args) ) {
						write_tb("+",tb);
						fnodetotex_tb((FNODE)BDY(args),tb);
					}
					break;
				case '*':
					f1 = (FNODE)BDY(args);
					if ( f1->id == I_FORMULA && MUNIQ(FA0(f1)) )
						write_tb("-",tb);
					else
						fnodetotex_tb(f1,tb);
					write_tb(" ",tb);
					for ( args = NEXT(args); args; args = NEXT(args) ) {
						/* XXX special care for DP */
						f2 = (FNODE)BDY(args);
						if ( f2->id == I_EV ) {
							n = (NODE)FA0(f2);
							for ( i = 0; n; n = NEXT(n), i++ ) {
								fi = (FNODE)BDY(n);
								if ( fi->id != I_FORMULA || FA0(fi) )
									break;
							}
							if ( n )
								fnodetotex_tb(f2,tb);
						} else
							fnodetotex_tb(f2,tb);
					}
					break;
				default:
					error("invalid nary op");
					break;
			}
			write_tb(")",tb);
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
					write_tb(" > ",tb);
					fnodetotex_tb((FNODE)FA2(f),tb);
					break;
				case C_LT:
					fnodetotex_tb((FNODE)FA1(f),tb);
					write_tb(" < ",tb);
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
					write_tb(" > ",tb);
					fnodetotex_tb((FNODE)FA2(f),tb);
					break;
				case L_LT:
					fnodetotex_tb((FNODE)FA1(f),tb);
					write_tb(" < ",tb);
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
							si = i+dp_vars_origin;
							sprintf(vname_conv,(si>=0&&si<10)?"%s_%d":"%s_{%d}",
								prefix_conv,si);
						} else if ( i < elen ) {
							strcpy(prefix,
								dp_dvars_prefix?dp_dvars_prefix:"\\partial");
							prefix_conv = conv_rule(prefix);
							vname_conv = (char *)ALLOCA(strlen(prefix_conv)+50);
							si = i+dp_dvars_origin-elen2;
							sprintf(vname_conv,(si>=0&&si<10)?"%s_%d":"%s_{%d}",
								prefix_conv,si);
						} else {
							strcpy(prefix,"h");
							vname_conv = conv_rule(prefix);
						}
					} else {
						strcpy(prefix,dp_vars_prefix?dp_vars_prefix:"x");
						prefix_conv = conv_rule(prefix);
						vname_conv = (char *)ALLOCA(strlen(prefix_conv)+50);
						si = i+dp_vars_origin;
						sprintf(vname_conv,(si>=0&&si<10)?"%s_%d":"%s_{%d}",
							prefix_conv,si);
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

void Psprintf(NODE arg,STRING *rp)
{
    STRING string;
    char *s,*t,*r;
    int argc,n,len;
    NODE node;

    string = (STRING)ARG0(arg);
    asir_assert(string,O_STR,"sprintf");
    s = BDY(string);
    for(n = 0, t = s; *t; t++) {
        if (*t=='%' && *(t+1)=='a') {
            n++;
        }
    }
    for(node = NEXT(arg), argc = 0, len = strlen(s); node; node = NEXT(node), argc++) {
        len += estimate_length(CO,BDY(node));
    }
    if (argc < n) {
        error("sprintf: invalid argument");
    }
    r = (char *)MALLOC_ATOMIC(len);
    for(node = NEXT(arg), t = r; *s; s++) {
        if (*s=='%' && *(s+1)=='a') {
            strcpy(t,objtostr(BDY(node)));
            node = NEXT(node);
            t = strchr(t,0);
            s++;
        }else {
            *t++ = *s;
        }
    }
    *t = 0;
    MKSTR(*rp,r);
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
		case I_NARYOP:
			return top_is_minus((FNODE)BDY((NODE)FA1(f)));

		default:
			return 0;
	}
}

FNODE flatten_fnode(FNODE,char *);

void Pflatten_quote(NODE arg,Obj *rp)
{
	FNODE f;
	QUOTE q;

	if ( !ARG0(arg) || OID((Obj)ARG0(arg)) != O_QUOTE )
		*rp = (Obj)ARG0(arg);
	else if ( argc(arg) == 1 ) {
		f = flatten_fnode(BDY((QUOTE)ARG0(arg)),"+");
		f = flatten_fnode(f,"*");
		MKQUOTE(q,f);
		*rp = (Obj)q;
	} else {
		f = flatten_fnode(BDY((QUOTE)ARG0(arg)),BDY((STRING)ARG1(arg)));
		MKQUOTE(q,f);
		*rp = (Obj)q;
	}
}

void Pget_quote_id(NODE arg,Q *rp)
{
	FNODE f;
	QUOTE q;

	q = (QUOTE)ARG0(arg);
	if ( !q || OID(q) != O_QUOTE )
		error("get_quote_id : invalid argument");
	f = BDY(q);
	STOQ((int)f->id,*rp);
}

void Pquote_to_funargs(NODE arg,LIST *rp)
{
	fid_spec_p spec;
	QUOTE q;
	QUOTEARG qa;
	FNODE f;
	STRING s;
	QUOTE r;
	int i;
	Q id,a;
	LIST l;
	NODE t0,t,w,u,u0;

	q = (QUOTE)ARG0(arg);
	if ( !q || OID(q) != O_QUOTE )
		error("quote_to_funargs : invalid argument");
	f = BDY(q);
	if ( !f ) {
		MKLIST(*rp,0);
		return;
	}
	get_fid_spec(f->id,&spec);
	if ( !spec )
		error("quote_to_funargs : not supported yet");
	t0 = 0;
	STOQ((int)f->id,id);
	NEXTNODE(t0,t);
	BDY(t) = (pointer)id;
	for ( i = 0; spec->type[i] != A_end; i++ ) {
		NEXTNODE(t0,t);
		switch ( spec->type[i] ) {
			case A_fnode:
				MKQUOTE(r,(FNODE)f->arg[i]);
				BDY(t) = (pointer)r;
				break;
			case A_int:
				STOQ((int)f->arg[i],a);
				BDY(t) = (pointer)a;
				break;
			case A_str:
				MKSTR(s,(char *)f->arg[i]);
				BDY(t) = (pointer)s;
				break;
			case A_internal:
				BDY(t) = (pointer)f->arg[i];
				break;
			case A_node:
				w = (NODE)f->arg[i];
				for ( u0 = 0; w; w = NEXT(w) ){
					NEXTNODE(u0,u);
					MKQUOTE(r,(FNODE)BDY(w));
					BDY(u) = (pointer)r;
				}
				if ( u0 ) NEXT(u) = 0;
				MKLIST(l,u0);
				BDY(t) = (pointer)l;
				break;
			default:
				MKQUOTEARG(qa,spec->type[i],f->arg[i]);	
				BDY(t) = (pointer)qa;
				break;
		}
	}
	if ( t0 ) NEXT(t) = 0;
	MKLIST(*rp,t0);
}

void Pfunargs_to_quote(NODE arg,QUOTE *rp)
{
	fid_spec_p spec;
	QUOTE q;
	QUOTEARG qa;
	FNODE f;
	STRING s;
	QUOTE r,b;
	int i;
	LIST l;
	fid id;
	Obj a;
	NODE t0,t,u0,u,w;

	l = (LIST)ARG0(arg);
	if ( !l || OID(l) != O_LIST || !(t=BDY(l)) )
		error("funargs_to_quote : invalid argument");
	t = BDY(l);
	id = (fid)QTOS((Q)BDY(t)); t = NEXT(t);
	get_fid_spec(id,&spec);
	if ( !spec )
		error("funargs_to_quote : not supported yet");
	for ( i = 0; spec->type[i] != A_end; i++ );
	NEWFNODE(f,i);
	f->id = id;
	for ( i = 0; spec->type[i] != A_end; i++, t = NEXT(t) ) {
		if ( !t )
			error("funargs_to_quote : argument mismatch");
		a = (Obj)BDY(t);
		switch ( spec->type[i] ) {
			case A_fnode:
				if ( !a || OID(a) != O_QUOTE )
					error("funargs_to_quote : invalid argument");
				f->arg[i] = BDY((QUOTE)a);
				break;
			case A_int:
				if ( !INT(a) )
					error("funargs_to_quote : invalid argument");
				f->arg[i] = (pointer)QTOS((Q)a);
				break;
			case A_str:
				if ( !a || OID(a) != O_STR )
					error("funargs_to_quote : invalid argument");
				f->arg[i] = (pointer)BDY((STRING)a);
				break;
			case A_internal:
				f->arg[i] = (pointer)a;
				break;
			case A_node:
				if ( !a || OID(a) != O_LIST )
					error("funargs_to_quote : invalid argument");
				u0 = 0;
				for ( w = BDY((LIST)a); w; w = NEXT(w) ) {
					NEXTNODE(u0,u);
					b = (QUOTE)BDY(w);
					if ( !b || OID(b) != O_QUOTE )
						error("funargs_to_quote : invalid argument");
					BDY(u) = BDY(b);
				}
				if ( u0 ) NEXT(u) = 0;
				f->arg[i] = (pointer)u0;
				break;
			default:
				if ( !a || OID(a) != O_QUOTEARG || 
					((QUOTEARG)a)->type != spec->type[i] )
					error("funargs_to_quote : invalid argument");
				f->arg[i] = BDY((QUOTEARG)a);
				break;
		}
	}
	MKQUOTE(*rp,f);
}

FNODE fnode_apply(FNODE f,FNODE (*func)(),int expand);
FNODE fnode_normalize(FNODE f,int expand);

void Pquote_normalize(NODE arg,QUOTE *rp)
{
	QUOTE q,r;
	FNODE f;
	int expand,ac;

	ac = argc(arg);
	if ( !ac ) error("quote_normalize : invalid argument");
	q = (QUOTE)ARG0(arg);
	expand = ac==2 && ARG1(arg);
	if ( !q || OID(q) != O_QUOTE )
		*rp = q;
	else {
		f = fnode_normalize(BDY(q),expand);
		MKQUOTE(r,f);
		*rp = r;
	}
}

NBP fnode_to_nbp(FNODE f);

void Pquote_to_nbp(NODE arg,NBP *rp)
{
	QUOTE q;
	FNODE f;

	q = (QUOTE)ARG0(arg); f = (FNODE)BDY(q);
	f = fnode_normalize(f,0);
	*rp = fnode_to_nbp(f);
}

void Pshuffle_mul(NODE arg,NBP *rp)
{
	NBP p1,p2;

	p1 = (NBP)ARG0(arg);
	p2 = (NBP)ARG1(arg);
	shuffle_mulnbp(CO,p1,p2,rp);
}

void Pharmonic_mul(NODE arg,NBP *rp)
{
	NBP p1,p2;

	p1 = (NBP)ARG0(arg);
	p2 = (NBP)ARG1(arg);
	harmonic_mulnbp(CO,p1,p2,rp);
}

void Pnbp_hm(NODE arg, NBP *rp)
{
	NBP p;
	NODE n;
	NBM m;

	p = (NBP)ARG0(arg);
	if ( !p ) *rp = 0;
	else {
		m = (NBM)BDY(BDY(p));
		MKNODE(n,m,0);
		MKNBP(*rp,n);
	}
}
	
void Pnbp_ht(NODE arg, NBP *rp)
{
	NBP p;
	NODE n;
	NBM m,m1;

	p = (NBP)ARG0(arg);
	if ( !p ) *rp = 0;
	else {
		m = (NBM)BDY(BDY(p));
		NEWNBM(m1);
		m1->d = m->d; m1->c = ONE; m1->b = m->b;
		MKNODE(n,m1,0);
		MKNBP(*rp,n);
	}
}

void Pnbp_hc(NODE arg, Q *rp)
{
	NBP p;
	NBM m;

	p = (NBP)ARG0(arg);
	if ( !p ) *rp = 0;
	else {
		m = (NBM)BDY(BDY(p));
		*rp = m->c;
	}
}

void Pnbp_rest(NODE arg, NBP *rp)
{
	NBP p;
	NODE n;

	p = (NBP)ARG0(arg);
	if ( !p ) *rp = 0;
	else {
		n = BDY(p);
		if ( !NEXT(n) ) *rp = 0;
		else
			MKNBP(*rp,NEXT(n));
	}
}

void Pnbm_hp(NODE arg, LIST *rp)
{
	NBP p;
	NBM m;
	int d,i,xy;
	int *b;
	Q qxy,qi;
	NODE n;

	p = (NBP)ARG0(arg);
	if ( !p ) {
		MKLIST(*rp,0);
	} else {
		m = (NBM)BDY(BDY(p));
		b = m->b;
		d = m->d;
		if ( !d )
			MKLIST(*rp,0);
		else {
			xy = NBM_GET(b,0);
			for ( i = 1; i < d; i++ )
				if ( NBM_GET(b,i) != xy ) break;
			STOQ(xy,qxy);
			STOQ(i,qi);
			n = mknode(2,qxy,qi);	
			MKLIST(*rp,n);
		}
	}
}

void Pnbm_rest(NODE arg,NBP *rp)
{
	NBP p;
	NBM m,m1;
	int d,xy,i,d1,i1;
	int *b,*b1;
	NODE n;

	p = (NBP)ARG0(arg);
	if ( !p )
		*rp = 0;
	else {
		m = (NBM)BDY(BDY(p));
		b = m->b;
		d = m->d;
		if ( !d )
			*rp = p;
		else {
			xy = NBM_GET(b,0);
			for ( i = 1; i < d; i++ )
				if ( NBM_GET(b,i) != xy ) break;
			d1 = d-i;
			NEWNBM(m1);
			m1->d = d1; m1->c = m->c;
			NEWNBMBDY(m1,d1);
			b1 = m1->b;
			for ( i1 = 0; i < d; i++, i1++ )
				if ( NBM_GET(b,i) ) NBM_SET(b1,i1);
				else NBM_CLR(b1,i1);
			MKNODE(n,m1,0);
			MKNBP(*rp,n);
		}
	}
}

NBP fnode_to_nbp(FNODE f)
{
	Q r;
	int n,i;
	NBM m;
	V v;
	NBP u,u1,u2;
	NODE t,b;

	if ( f->id == I_FORMULA ) {
		r = eval(f);
		NEWNBM(m);
		if ( OID(r) == O_N ) {
			m->d = 0; m->c = (Q)r; m->b = 0;
		} else {
			v = VR((P)r);
			m->d = 1; m->c = ONE; NEWNBMBDY(m,1);
			if ( !strcmp(NAME(v),"x") ) NBM_SET(m->b,0);
			else NBM_CLR(m->b,0);
		}
		MKNODE(b,m,0); MKNBP(u,b);
		return u;
	} else if ( IS_NARYADD(f) ) {
		t = (NODE)FA1(f); u = fnode_to_nbp((FNODE)BDY(t));
		for ( t = NEXT(t); t; t = NEXT(t) ) {
			u1 = fnode_to_nbp((FNODE)BDY(t));
			addnbp(CO,u,u1,&u2); u = u2;
		}
		return u;
	} else if ( IS_NARYMUL(f) ) {
		t = (NODE)FA1(f); u = fnode_to_nbp((FNODE)BDY(t));
		for ( t = NEXT(t); t; t = NEXT(t) ) {
			u1 = fnode_to_nbp((FNODE)BDY(t));
			mulnbp(CO,u,u1,&u2); u = u2;
		}
		return u;
	} else if ( IS_BINARYPWR(f) ) {
		u = fnode_to_nbp((FNODE)FA1(f));
		r = eval((FNODE)FA2(f));
		pwrnbp(CO,u,r,&u1);
		return u1;
	}
}

void Pnquote_comp(NODE arg,Q *rp)
{
	QUOTE q1,q2;
	FNODE f1,f2;
	int r;

	q1 = (QUOTE)ARG0(arg); f1 = (FNODE)BDY(q1);
	q2 = (QUOTE)ARG1(arg); f2 = (FNODE)BDY(q2);
	f1 = fnode_normalize(f1,0);
	f2 = fnode_normalize(f2,0);
	r = nfnode_comp(f1,f2);
	STOQ(r,*rp);
}

INLINE int fnode_is_number(FNODE f)
{
	Obj obj;

	switch ( f->id ) {
		case I_MINUS: case I_PAREN:
			return fnode_is_number(FA0(f));
				
		case I_FORMULA:
			obj = FA0(f);
			if ( !obj ) return 1;
			else if ( OID(obj) == O_QUOTE ) 
				return fnode_is_number(BDY((QUOTE)obj));
			else if ( NUM(obj) ) return 1;
			else return 0;
		
		case I_BOP:
			return fnode_is_number(FA1(f)) && fnode_is_number(FA2(f));
		
		default:
			return 0;
	}
}

int fnode_is_rational(FNODE f)
{
	Obj obj;

	switch ( f->id ) {
		case I_MINUS: case I_PAREN:
			return fnode_is_number(FA0(f));
				
		case I_FORMULA:
			obj = FA0(f);
			if ( !obj ) return 1;
			else if ( OID(obj) == O_QUOTE ) 
				return fnode_is_rational(BDY((QUOTE)obj));
			else if ( NUM(obj) && RATN(obj) ) return 1;
			else return 0;
		
		case I_BOP:
			if ( !strcmp(((ARF)FA0(f))->name,"^")  )
				return fnode_is_rational(FA1(f)) && fnode_is_integer(FA2(f));
			else
				return fnode_is_rational(FA1(f)) && fnode_is_rational(FA2(f));
		
		default:
			return 0;
	}
}

int fnode_is_integer(FNODE f)
{
	Obj obj;

	switch ( f->id ) {
		case I_MINUS: case I_PAREN:
			return fnode_is_integer(FA0(f));
				
		case I_FORMULA:
			obj = FA0(f);
			if ( !obj ) return 1;
			else if ( OID(obj) == O_QUOTE ) 
				return fnode_is_integer(BDY((QUOTE)obj));
			else if ( INT(obj)) return 1;
			else return 0;
		
		case I_BOP:
			switch ( ((ARF)FA0(f))->name[0] ) {
				case '^':
					return fnode_is_integer(FA1(f)) 
						&& fnode_is_nonnegative_integer(FA2(f));
				case '/':
					return fnode_is_integer(FA1(f)) && 
						( fnode_is_one(FA2(f)) || fnode_is_minusone(FA2(f)) );
				default:
					return fnode_is_integer(FA1(f)) && fnode_is_integer(FA2(f));
			}
			break;

		default:
			return 0;
	}
}

int fnode_is_nonnegative_integer(FNODE f)
{
	Q n;

	n = eval(f);
	if ( !n || (INT(n) && SGN(n) > 0) ) return 1;
	else return 0;
}

int fnode_is_one(FNODE f)
{
	Q n;

	n = eval(f);
	if ( UNIQ(n) ) return 1;
	else return 0;
}

int fnode_is_minusone(FNODE f)
{
	Q n;

	n = eval(f);
	if ( MUNIQ(n) ) return 1;
	else return 0;
}

int fnode_is_dependent(FNODE f,V v)
{
	Obj obj;
	FNODE arg;
	NODE t;

	switch ( f->id ) {
		case I_MINUS: case I_PAREN:
			return fnode_is_dependent(FA0(f),v);
				
		case I_FORMULA:
			obj = FA0(f);
			if ( !obj ) return 0;
			else if ( OID(obj) == O_QUOTE ) 
				return fnode_is_dependent(BDY((QUOTE)obj),v);
			else if ( obj_is_dependent(obj,v) ) return 1;
			else return 0;
		
		case I_BOP:
			return fnode_is_dependent(FA1(f),v) || fnode_is_dependent(FA2(f),v);
	
		case I_FUNC:
			arg = (FNODE)FA1(f);
			for ( t = FA0(arg); t; t = NEXT(t) )
				if ( fnode_is_dependent(BDY(t),v) ) return 1;
			return 0;

		default:
			return 0;
	}
}

FNODE nfnode_add(FNODE a1,FNODE a2,int expand);
FNODE nfnode_mul(FNODE a1,FNODE a2,int expand);
FNODE nfnode_pwr(FNODE a1,FNODE a2,int expand);
FNODE nfnode_mul_coef(Num c,FNODE f,int expand);
FNODE fnode_expand_pwr(FNODE f,int n);
FNODE to_narymul(FNODE f);
FNODE to_naryadd(FNODE f);
FNODE fnode_node_to_naryadd(NODE n);
FNODE fnode_node_to_narymul(NODE n);
void fnode_base_exp(FNODE f,FNODE *bp,FNODE *ep);
void fnode_coef_body(FNODE f,Num *cp,FNODE *bp);


FNODE fnode_normalize(FNODE f,int expand)
{
	FNODE a1,a2,mone,r,b2;
	NODE n;
	Q q;

	if ( f->normalized && (f->expanded || !expand) ) return f;
	STOQ(-1,q);
	mone = mkfnode(1,I_FORMULA,q);
	switch ( f->id ) {
		case I_PAREN:
			r = fnode_normalize(FA0(f),expand);
			break;
		
		case I_MINUS:
			r = nfnode_mul_coef((Num)q,
				fnode_normalize(FA0(f),expand),expand);
			break;

		case I_BOP:
			/* arf fnode fnode */
			a1 = fnode_normalize(FA1(f),expand);
			a2 = fnode_normalize(FA2(f),expand);
			switch ( OPNAME(f) ) {
				case '+':
					r = nfnode_add(a1,a2,expand);
					break;
				case '-':
					a2 = nfnode_mul_coef((Num)q,a2,expand);
					r = nfnode_add(a1,a2,expand);
					break;
				case '*':
					r = nfnode_mul(a1,a2,expand);
					break;
				case '/':
					a2 = nfnode_pwr(a2,mone,expand);
					r = nfnode_mul(a1,a2,expand);
					break;
				case '^':
					r = nfnode_pwr(a1,a2,expand);
					break;
				default:
					r = mkfnode(3,I_BOP,FA0(f),a1,a2);
					break;
			}
			break;

		case I_NARYOP:
			switch ( OPNAME(f) ) {
				case '+':
					n = (NODE)FA1(f);
					r = fnode_normalize(BDY(n),expand); n = NEXT(n);
					for ( ; n; n = NEXT(n) ) {
						a1 = fnode_normalize(BDY(n),expand);
						r = nfnode_add(r,a1,expand);
					}
					break;
				case '*':
					n = (NODE)FA1(f);
					r = fnode_normalize(BDY(n),expand); n = NEXT(n);
					for ( ; n; n = NEXT(n) ) {
						a1 = fnode_normalize(BDY(n),expand);
						r = nfnode_mul(r,a1,expand);
					}
					break;
				default:
					error("fnode_normallize : cannot happen");
			}
			break;

		default:
			return fnode_apply(f,fnode_normalize,expand);
	}
	r->normalized = 1;
	r->expanded = expand;
	return r;
}

FNODE fnode_apply(FNODE f,FNODE (*func)(),int expand)
{
	fid_spec_p spec;
	FNODE r;
	int i,n;
	NODE t,t0,s;

	get_fid_spec(f->id,&spec);
	for ( n = 0; spec->type[n] != A_end; n++ );
	NEWFNODE(r,n); r->id = f->id;
	for ( i = 0; i < n; i++ ) {
		switch ( spec->type[i] ) {
			case A_fnode:
				r->arg[i] = func(f->arg[i],expand);
				break;
			case A_node:
				s = (NODE)f->arg[i];
				for ( t0 = 0; s; s = NEXT(s) ) {
					NEXTNODE(t0,t);
					BDY(t) = (pointer)func((FNODE)BDY(s),expand);
				}
				if ( t0 ) NEXT(t) = 0;
				r->arg[i] = t0;
				break;
			default:
				r->arg[i] = f->arg[i];
				break;
		}
	}
	return r;
}

FNODE nfnode_add(FNODE f1,FNODE f2,int expand)
{
	NODE n1,n2,r0,r;
	FNODE b1,b2;
	int s;
	Num c1,c2,c;

	if ( IS_ZERO(f1) ) return f2;
	else if ( IS_ZERO(f2) ) return f1;
	f1 = to_naryadd(f1); f2 = to_naryadd(f2);
	n1 = (NODE)FA1(f1); n2 = (NODE)FA1(f2);
	r0 = 0;
	while ( n1 && n2 ) {
		fnode_coef_body(BDY(n1),&c1,&b1); fnode_coef_body(BDY(n2),&c2,&b2);
		if ( (s = nfnode_comp(b1,b2)) > 0 ) {
			NEXTNODE(r0,r); BDY(r) = BDY(n1); n1 = NEXT(n1);
		} else if ( s < 0 ) {
			NEXTNODE(r0,r); BDY(r) = BDY(n2); n2 = NEXT(n2);
		} else {
			addnum(0,c1,c2,&c);
			if ( c ) {
				NEXTNODE(r0,r); BDY(r) = nfnode_mul_coef(c,b1,expand);
			}
			n1 = NEXT(n1); n2 = NEXT(n2);
		}
	}
	if ( n1 )
		if ( r0 ) NEXT(r) = n1;
		else r0 = n1;
	else if ( n2 )
		if ( r0 ) NEXT(r) = n2;
		else r0 = n2;
	else if ( r0 )
		NEXT(r) = 0;

	return fnode_node_to_naryadd(r0);
}

FNODE fnode_node_to_naryadd(NODE n)
{
	if ( !n ) return mkfnode(1,I_FORMULA,0);
	else if ( !NEXT(n) ) return BDY(n);
	else return mkfnode(2,I_NARYOP,addfs,n);
}

FNODE fnode_node_to_narymul(NODE n)
{
	if ( !n ) return mkfnode(1,I_FORMULA,ONE);
	else if ( !NEXT(n) ) return BDY(n);
	else return mkfnode(2,I_NARYOP,mulfs,n);
}

FNODE nfnode_mul(FNODE f1,FNODE f2,int expand)
{
	NODE n1,n2,r0,r,r1;
	FNODE b1,b2,e1,e2,cc,t,t1;
	FNODE *m;
	int s;
	Num c1,c2,c,e;
	int l1,l,i,j;

	if ( IS_ZERO(f1) || IS_ZERO(f2) ) return mkfnode(1,I_FORMULA,0);
	else if ( fnode_is_number(f1) ) 
		return nfnode_mul_coef((Num)eval(f1),f2,expand);
	else if ( fnode_is_number(f2) ) 
		return nfnode_mul_coef((Num)eval(f2),f1,expand);

	if ( expand && IS_NARYADD(f1) ) {
		t = mkfnode(1,I_FORMULA,0);
		for ( n1 = (NODE)FA1(f1); n1; n1 = NEXT(n1) ) {
			t1 = nfnode_mul(BDY(n1),f2,expand);
			t = nfnode_add(t,t1,expand);
		}
		return t;
	}
	if ( expand && IS_NARYADD(f2) ) {
		t = mkfnode(1,I_FORMULA,0);
		for ( n2 = (NODE)FA1(f2); n2; n2 = NEXT(n2) ) {
			t1 = nfnode_mul(f1,BDY(n2),expand);
			t = nfnode_add(t,t1,expand);
		}
		return t;
	}

	fnode_coef_body(f1,&c1,&b1); fnode_coef_body(f2,&c2,&b2);
	mulnum(0,c1,c2,&c);
	if ( !c ) return mkfnode(1,I_FORMULA,0);


	n1 = (NODE)FA1(to_narymul(b1)); n2 = (NODE)FA1(to_narymul(b2));
	l1 = length(n1); l = l1+length(n2);
	m = (FNODE *)ALLOCA(l*sizeof(FNODE));
	for ( r = n1, i = 0; i < l1; r = NEXT(r), i++ ) m[i] = BDY(r);
	for ( r = n2; r; r = NEXT(r) ) {
		if ( i == 0 )
			m[i++] = BDY(r);
		else {
			fnode_base_exp(m[i-1],&b1,&e1); fnode_base_exp(BDY(r),&b2,&e2);
			if ( compfnode(b1,b2) ) break;
			addnum(0,eval(e1),eval(e2),&e);
			if ( !e ) i--;
			else if ( UNIQ(e) )
				m[i-1] = b1;
			else
				m[i-1] = mkfnode(3,I_BOP,pwrfs,b1,mkfnode(1,I_FORMULA,e));
		}
	}
	for ( j = i-1; j >= 0; j-- ) {
		MKNODE(r1,m[j],r); r = r1;
	}
	if ( !UNIQ(c) ) {
		cc = mkfnode(1,I_FORMULA,c); MKNODE(r1,cc,r); r = r1;
	}
	return fnode_node_to_narymul(r);
}

FNODE nfnode_pwr(FNODE f1,FNODE f2,int expand)
{
	FNODE b,b1,e1,e,cc,r,mf2,mone,inv;
	Num c,c1,nf2;
	int ee;
	NODE arg,n,t0,t1;
	Q q;

	if ( IS_ZERO(f2) ) return mkfnode(1,I_FORMULA,ONE);
	else if ( IS_ZERO(f1) ) return mkfnode(1,I_FORMULA,0);
	else if ( fnode_is_number(f1) ) {
		if ( fnode_is_integer(f2) ) {
			if ( fnode_is_one(f2) ) return f1;
			else {
				pwrnum(0,(Num)eval(f1),(Num)eval(f2),&c);
				return mkfnode(1,I_FORMULA,c);
			}
		} else
			return mkfnode(3,I_BOP,pwrfs,f1,f2);
	} else if ( IS_BINARYPWR(f1) ) {
		b1 = FA1(f1); e1 = FA2(f1);
		e = nfnode_mul(e1,f2,expand);
		if ( fnode_is_one(e) )
			return b1;
		else
			return mkfnode(3,I_BOP,FA0(f1),b1,e);
	} else if ( expand && IS_NARYMUL(f1) && fnode_is_number(f2) 
		&& fnode_is_integer(f2) ) {
		fnode_coef_body(f1,&c1,&b1);
		nf2 = (Num)eval(f2);
		pwrnum(0,(Num)c1,nf2,&c);
		ee = QTOS((Q)nf2);
		cc = mkfnode(1,I_FORMULA,c);
		if ( fnode_is_nonnegative_integer(f2) )
			b = fnode_expand_pwr(b1,ee);
		else {
			STOQ(-1,q);
			mone = mkfnode(1,I_FORMULA,q);
			b1 = to_narymul(b1);
			for ( t0 = 0, n = (NODE)FA1(b1); n; n = NEXT(n) ) {
				inv = mkfnode(3,I_BOP,pwrfs,BDY(n),mone);
				MKNODE(t1,inv,t0); t0 = t1;
			}
			b1 = fnode_node_to_narymul(t0);
			b = fnode_expand_pwr(b1,-ee);
		}
		if ( fnode_is_one(cc) )
			return b;
		else
			return fnode_node_to_narymul(mknode(2,cc,b));
	} else if ( expand && fnode_is_integer(f2) 
			&& fnode_is_nonnegative_integer(f2) ) {
		q = (Q)eval(f2);
		if ( PL(NM(q)) > 1 ) error("nfnode_pwr : exponent too large");
		return fnode_expand_pwr(f1,QTOS(q));
	} else
		return mkfnode(3,I_BOP,pwrfs,f1,f2);
}

FNODE fnode_expand_pwr(FNODE f,int n)
{
	int n1;
	FNODE f1,f2;

	if ( !n ) return mkfnode(1,I_FORMULA,ONE);
	else if ( IS_ZERO(f) ) return mkfnode(1,I_FORMULA,0);
	else if ( n == 1 ) return f;
	else {
		n1 = n/2;
		f1 = fnode_expand_pwr(f,n1);
		f2 = nfnode_mul(f1,f1,1);
		if ( n%2 ) f2 = nfnode_mul(f2,f,1);
		return f2;
	}
}

/* f = b^e */
void fnode_base_exp(FNODE f,FNODE *bp,FNODE *ep)
{
	if ( IS_BINARYPWR(f) ) {
		*bp = FA1(f); *ep = FA2(f);
	} else {
		*bp = f; *ep = mkfnode(1,I_FORMULA,ONE);		
	}
}

FNODE to_naryadd(FNODE f)
{
	FNODE r;
	NODE n;

	if ( IS_NARYADD(f) ) return f;

	NEWFNODE(r,2); r->id = I_NARYOP;
	FA0(r) = addfs; MKNODE(n,f,0); FA1(r) = n;
	return r;
}

FNODE to_narymul(FNODE f)
{
	FNODE r;
	NODE n;

	if ( IS_NARYMUL(f) ) return f;

	NEWFNODE(r,2); r->id = I_NARYOP;
	FA0(r) = mulfs; MKNODE(n,f,0); FA1(r) = n;
	return r;
}

FNODE nfnode_mul_coef(Num c,FNODE f,int expand)
{
	FNODE b1,cc;
	Num c1,c2;
	NODE n,r,r0;

	if ( !c )
		return mkfnode(I_FORMULA,0);
	else {
		fnode_coef_body(f,&c1,&b1);
		mulnum(0,c,c1,&c2);
		if ( UNIQ(c2) ) return b1;
		else {
			cc = mkfnode(1,I_FORMULA,c2);
			if ( fnode_is_number(b1) ) {
				if ( !fnode_is_one(b1) )
					error("nfnode_mul_coef : cannot happen");
				else
					return cc;
			} else if ( IS_NARYMUL(b1) ) {
				MKNODE(n,cc,FA1(b1));
				return fnode_node_to_narymul(n);
			} else if ( expand && IS_NARYADD(b1) ) {
				for ( r0 = 0, n = (NODE)FA1(b1); n; n = NEXT(n) ) {
					NEXTNODE(r0,r);
					BDY(r) = nfnode_mul_coef(c2,BDY(n),expand);
				}
				if ( r0 ) NEXT(r) = 0;
				return fnode_node_to_naryadd(r0);
			} else 
				return fnode_node_to_narymul(mknode(2,cc,b1));
		}
	}
}

void fnode_coef_body(FNODE f,Num *cp,FNODE *bp)
{
	FNODE c;

	if ( fnode_is_number(f) ) {
		*cp = eval(f); *bp = mkfnode(1,I_FORMULA,ONE);
	} else if ( IS_NARYMUL(f) ) {
		c=(FNODE)BDY((NODE)FA1(f));
		if ( fnode_is_number(c) ) {
			*cp = eval(c);
			*bp = fnode_node_to_narymul(NEXT((NODE)FA1(f)));
		} else {
			*cp = (Num)ONE; *bp = f;
		}
	} else {
		*cp = (Num)ONE; *bp = f;
	}
}

int nfnode_comp_pwr(FNODE f1,FNODE f2);

int nfnode_comp(FNODE f1,FNODE f2)
{
	NODE n1,n2;
	int r,i1,i2,ret;
	char *nm1,*nm2;
	FNODE b1,b2,e1,e2,g,a1,a2,fn1,fn2;
	Num ee,ee1,c1,c2;

	if ( IS_NARYADD(f1) || IS_NARYADD(f2) ) {
		f1 = to_naryadd(f1); f2 = to_naryadd(f2);
		n1 = (NODE)FA1(f1); n2 = (NODE)FA1(f2);
		while ( n1 && n2 )
			if ( r = nfnode_comp(BDY(n1),BDY(n2)) ) return r;
			else {
				n1 = NEXT(n1); n2 = NEXT(n2);
			}
		return n1?1:(n2?-1:0);
	}
	if ( IS_NARYMUL(f1) || IS_NARYMUL(f2) ) {
		fnode_coef_body(f1,&c1,&b1);
		fnode_coef_body(f2,&c2,&b2);
		if ( !compfnode(b1,b2) ) return compnum(0,c1,c2);
		b1 = to_narymul(b1); b2 = to_narymul(b2);
		n1 = (NODE)FA1(b1); n2 = (NODE)FA1(b2);
		while ( 1 ) {
			while ( n1 && n2 && !compfnode(BDY(n1),BDY(n2)) ) {
				n1 = NEXT(n1); n2 = NEXT(n2);
			}
			if ( !n1 || !n2 ) {
				return n1?1:(n2?-1:0);
			}
			fnode_base_exp(BDY(n1),&b1,&e1);
			fnode_base_exp(BDY(n2),&b2,&e2);

			if ( r = nfnode_comp(b1,b2) ) {
				if ( r > 0 )
					return nfnode_comp(e1,mkfnode(1,I_FORMULA,0));
				else if ( r < 0 )
					return nfnode_comp(mkfnode(1,I_FORMULA,0),e2);
			} else {
				n1 = NEXT(n1); n2 = NEXT(n2);
				if ( fnode_is_number(e1) && fnode_is_number(e2) ) {
					/* f1 = t b^e1 ... , f2 = t b^e2 ... */
					subnum(0,eval(e1),eval(e2),&ee);
					r = compnum(0,ee,0);
					if ( r > 0 ) {
						g = mkfnode(3,I_BOP,pwrfs,b1,mkfnode(1,I_FORMULA,ee));
						MKNODE(n1,g,n1);
					} else if ( r < 0 ) {
						chsgnnum(ee,&ee1);
						g = mkfnode(3,I_BOP,pwrfs,b1,mkfnode(1,I_FORMULA,ee1));
						MKNODE(n2,g,n2);
					}
				} else {
					r = nfnode_comp(e1,e2);
					if ( r > 0 ) return 1;
					else if ( r < 0 ) return -1;
				}
			}
		}
	}
	if ( IS_BINARYPWR(f1) || IS_BINARYPWR(f2) )
		return nfnode_comp_pwr(f1,f2);
				
	/* now, IDs of f1 and f2 must be I_FORMULA, I_FUNC, I_IFUNC or I_PVAR */
	/* I_IFUNC > I_PVAR > I_FUNC > I_FORMULA */
	switch ( f1->id ) {
		case I_FORMULA:
			switch ( f2->id ) {
				case I_FORMULA:
					return arf_comp(CO,FA0(f1),FA0(f2));
				case I_FUNC: case I_IFUNC: case I_PVAR:
					return -1;
				default:
					error("nfnode_comp : undefined");
			}
			break;
		case I_FUNC:
			switch ( f2->id ) {
				case I_FORMULA:
					return 1;
				case I_PVAR: case I_IFUNC:
					return -1;
				case I_FUNC:
					nm1 = ((FUNC)FA0(f1))->name; nm2 = ((FUNC)FA0(f2))->name;
					r = strcmp(nm1,nm2);
					if ( r > 0 ) return 1;
					else if ( r < 0 ) return -1;
					else {
						/* compare args */
						n1 = FA0((FNODE)FA1(f1)); n2 = FA0((FNODE)FA1(f2));
						while ( n1 && n2 )
							if ( r = nfnode_comp(BDY(n1),BDY(n2)) ) return r;
							else {
								n1 = NEXT(n1); n2 = NEXT(n2);
							}
						return n1?1:(n2?-1:0);
					}
					break;
				default:
					error("nfnode_comp : undefined");
			}
		case I_PVAR:
			switch ( f2->id ) {
				case I_FORMULA: case I_FUNC:
					return 1;
				case I_IFUNC:
					return -1;
				case I_PVAR:
					i1 = (int)FA0(f1); i2 = (int)FA0(f2);
					if ( i1 > i2 ) return 1;
					else if ( i1 < i2 ) return -1;
					else return 0;
				default:
					error("nfnode_comp : undefined");
			}
			break;
		case I_IFUNC:
			switch ( f2->id ) {
				case I_FORMULA: case I_FUNC: case I_PVAR:
					return 1;
				case I_IFUNC:
					i1 = (int)FA0((FNODE)FA0(f1));
					i2 = (int)FA0((FNODE)FA0(f2));
					if ( i1 > i2 ) return 1;
					else if ( i1 < i2 ) return -1;
					else {
						/* compare args */
						n1 = FA0((FNODE)FA1(f1)); n2 = FA0((FNODE)FA1(f2));
						while ( n1 && n2 )
							if ( r = nfnode_comp(BDY(n1),BDY(n2)) ) return r;
							else {
								n1 = NEXT(n1); n2 = NEXT(n2);
							}
						return n1?1:(n2?-1:0);
					}
					break;

				default:
					error("nfnode_comp : undefined");
			}
			break;
		default:
			error("nfnode_comp : undefined");
	}
}

int nfnode_comp_pwr(FNODE f1,FNODE f2)
{
	FNODE b1,b2,e1,e2;
	int r;

	fnode_base_exp(f1,&b1,&e1);
	fnode_base_exp(f2,&b2,&e2);
	if ( r = nfnode_comp(b1,b2) ) {
		if ( r > 0 )
			return nfnode_comp(e1,mkfnode(1,I_FORMULA,0));
		else if ( r < 0 )
			return nfnode_comp(mkfnode(1,I_FORMULA,0),e2);
	} else return nfnode_comp(e1,e2);
}

NODE append_node(NODE a1,NODE a2)
{
	NODE t,t0;

	if ( !a1 )
		return a2;
	else {
		for ( t0 = 0; a1; a1 = NEXT(a1) ) {
			NEXTNODE(t0,t); BDY(t) = BDY(a1);
		}
		NEXT(t) = a2;
		return t0;
	}
}

int nfnode_match(FNODE f,FNODE pat,NODE *rp)
{
	NODE m,m1,m2,base,exp,fa,pa,n;
	LIST l;
	QUOTE qp,qf;
	FNODE fbase,fexp,a,fh;
	FUNC ff,pf;
	int r;

	if ( !pat )
		if ( !f ) {
			*rp = 0;
			return 1;
		} else
			return 0;
	else if ( !f )
		return 0;
	switch ( pat->id ) {
		case I_PVAR:
			/* [[pat,f]] */
			*rp = mknode(1,mknode(2,(int)FA0(pat),f),0);
			return 1;

		case I_FORMULA:
			if ( f->id == I_FORMULA && !arf_comp(CO,(Obj)FA0(f),(Obj)FA0(pat)) ) {
				*rp = 0; return 1;
			} else
				return 0;

		case I_BOP:
			/* OPNAME should be "^" */
			if ( !IS_BINARYPWR(pat) )
				error("nfnode_match : invalid BOP");
			if ( IS_BINARYPWR(f) ) {
				fbase = FA1(f); fexp = FA2(f);
			} else {
				fbase = f; fexp = mkfnode(1,I_FORMULA,ONE);
			}
			if ( !nfnode_match(fbase,FA1(pat),&base) ) return 0;
			a = rewrite_fnode(FA2(pat),base);
			if ( !nfnode_match(fexp,a,&exp) ) return 0;
			else {
				*rp = append_node(base,exp);
				return 1;
			}
			break;
		
		case I_FUNC: case I_IFUNC:
			if ( f->id != I_FUNC ) return 0;
			ff = (FUNC)FA0(f); 
			if ( pat->id == I_FUNC ) {
				pf = (FUNC)FA0(pat);
				if ( strcmp(ff->fullname,pf->fullname) ) return 0;
				m = 0;
			} else {
				/* XXX : I_FUNC_HEAD is a dummy id to pass FUNC */
				fh = mkfnode(1,I_FUNC_HEAD,FA0(f));
				m = mknode(1,mknode(2,FA0((FNODE)FA0(pat)),fh),0);
			}
			/* FA1(f) and FA1(pat) are I_LIST */
			fa = (NODE)FA0((FNODE)FA1(f));
			pa = (NODE)FA0((FNODE)FA1(pat));
			while ( fa && pa ) {
				a = rewrite_fnode(BDY(pa),m);
				if ( !nfnode_match(BDY(fa),a,&m1) ) return 0;
				m = append_node(m1,m);
				fa = NEXT(fa); pa = NEXT(pa);
			}
			if ( fa || pa ) return 0;
			else {
				*rp = m;
				return 1;
			}

		case I_NARYOP:
			if ( IS_NARYADD(pat) )
				return nfnode_match_naryadd(f,pat,rp);
			else if ( IS_NARYMUL(pat) )
				return nfnode_match_narymul(f,pat,rp);
			else
				error("nfnode_match : invalid NARYOP");
			break;

		default:
			error("nfnode_match : invalid pattern");
	}
}

/* remove i-th element */

FNODE fnode_removeith_naryadd(FNODE p,int i)
{
	int k,l;
	NODE t,r0,r,a;

	a = (NODE)FA1(p);
	l = length(a);
	if ( i < 0 || i >= l ) error("fnode_removeith_naryadd: invalid index");
	else if ( i == 0 )
		return fnode_node_to_naryadd(NEXT(a));
	else {
		for ( r0 = 0, k = 0, t = a; k < i; k++, t = NEXT(t) ) {
			NEXTNODE(r0,r);
			BDY(r) = BDY(t);
		}
		NEXT(r) = NEXT(t);
		return fnode_node_to_naryadd(r0);
	}
	
}

/* a0,...,a(i-1) */
FNODE fnode_left_narymul(FNODE p,int i)
{
	int k,l;
	NODE t,r0,r,a;

	a = (NODE)FA1(p);
	l = length(a);
	if ( i < 0 || i >= l ) error("fnode_left_narymul : invalid index");
	if ( i == 0 ) return 0;
	else if ( i == 1 ) return (FNODE)BDY(a);
	else {
		for ( r0 = 0, k = 0, t = a; k < i; k++, t = NEXT(t) ) {
			NEXTNODE(r0,r);
			BDY(r) = BDY(t);
		}
		NEXT(r) = 0;
		return fnode_node_to_narymul(r0);
	}
}

/* a(i+1),...,a(l-1) */
FNODE fnode_right_narymul(FNODE p,int i)
{
	NODE a,t;
	int l,k;

	a = (NODE)FA1(p);
	l = length(a);
	if ( i < 0 || i >= l ) error("fnode_right_narymul : invalid index");
	if ( i == l-1 ) return 0;
	else {
		for ( k = 0, t = a; k <= i; k++, t = NEXT(t) );
		return fnode_node_to_narymul(t);
	}
}

int nfnode_match_naryadd(FNODE f,FNODE p,NODE *rp)
{
	int fl,pl,fi,pi;
	NODE fa,pa,t,s,m,m1;
	FNODE fr,pr,prr,pivot;

	f = to_naryadd(f);
	fa = (NODE)FA1(f); fl = length(fa);
	pa = (NODE)FA1(p); pl = length(pa);
	if ( fl < pl ) return 0;
	else if ( pl == 1 ) {
		if ( fl == 1 )
			return nfnode_match(BDY(fa),BDY(pa),rp);
		else
			return 0;
	} else {
		for ( t = pa, pi = 0; t; t = NEXT(t), pi++ )
			if ( ((FNODE)BDY(t))->id != I_PVAR ) break;
		if ( !t ) {
			/* all are I_PVAR */
			m = 0;
			for ( t = pa, s = fa; NEXT(t); t = NEXT(t), s = NEXT(s) ) {
				nfnode_match(BDY(s),BDY(t),&m1);
				m = append_node(m1,m);
			}
			if ( !NEXT(s) )
				fr = (FNODE)BDY(s);
			else
				fr = mkfnode(2,I_NARYOP,FA0(f),s);
			nfnode_match(fr,BDY(t),&m1);
			*rp = append_node(m1,m);
			return 1;
		} else {
			pivot = (FNODE)BDY(t);
			pr = fnode_removeith_naryadd(p,pi);
			for ( s = fa, fi = 0; s; s = NEXT(s), fi++ ) {
				if ( nfnode_match(BDY(s),pivot,&m) ) {
					fr = fnode_removeith_naryadd(f,fi);
					prr = rewrite_fnode(pr,m);
					if ( nfnode_match(fr,prr,&m1) ) {
						*rp = append_node(m,m1);
						return 1;
					}
				}
			}
			return 0;
		}
	}
}

int nfnode_match_narymul(FNODE f,FNODE p,NODE *rp)
{
	int fl,pl,fi,pi;
	NODE fa,pa,t,s,m,m1;
	FNODE fr,pr,pleft,pleft1,pright,pright1,fleft,fright,pivot;

	f = to_narymul(f);
	fa = (NODE)FA1(f); fl = length(fa);
	pa = (NODE)FA1(p); pl = length(pa);
	if ( fl < pl ) return 0;
	else if ( pl == 1 ) {
		if ( fl == 1 )
			return nfnode_match(BDY(fa),BDY(pa),rp);
		else
			return 0;
	} else {
		for ( t = pa, pi = 0; t; t = NEXT(t), pi++ )
			if ( ((FNODE)BDY(t))->id != I_PVAR ) break;
		if ( !t ) {
			/* all are I_PVAR */
			m = 0;
			for ( t = pa, s = fa; NEXT(t); t = NEXT(t), s = NEXT(s) ) {
				pr = rewrite_fnode(BDY(t),m);
				if ( !nfnode_match(BDY(s),pr,&m1) ) return 0;
				m = append_node(m1,m);
			}
			if ( !NEXT(s) )
				fr = (FNODE)BDY(s);
			else
				fr = mkfnode(2,I_NARYOP,FA0(f),s);
			pr = rewrite_fnode(BDY(t),m);
			if ( !nfnode_match(fr,pr,&m1) ) return 0;
			*rp = append_node(m1,m);
			return 1;
		} else {
			pivot = (FNODE)BDY(t);
			pleft = fnode_left_narymul(p,pi);
			pright = fnode_right_narymul(p,pi);
			/* XXX : incomplete */
			for ( s = fa, fi = 0; s; s = NEXT(s), fi++ ) {
				if ( fi < pi ) continue;
				if ( nfnode_match(BDY(s),pivot,&m) ) {
					fleft = fnode_left_narymul(f,fi);
					pleft1 = rewrite_fnode(pleft,m);
					if ( nfnode_match(fleft,pleft1,&m1) ) {
						m = append_node(m1,m);
						fright = fnode_right_narymul(f,fi);
						pright1 = rewrite_fnode(pright,m);
						if ( nfnode_match(fright,pright1,&m1) ) {
							*rp = append_node(m1,m);
							return 1;
						}
					}
				}
			}
			return 0;
		}
	}
}

NODE nfnode_pvars(FNODE pat,NODE found)
{
	int ind;
	NODE prev,t;
	int *pair;

	switch ( pat->id ) {
		case I_PVAR:
			ind = (int)FA0(pat);
			for ( prev = 0, t = found; t; prev = t, t = NEXT(t) ) {
				pair = (int *)BDY(t);
				if ( pair[0] == ind ) {
					pair[1]++;
					return found;
				}
			}
			pair = (int *)MALLOC_ATOMIC(sizeof(int)*2);
			pair[0] = ind; pair[1] = 1;
			if ( !prev )
				MKNODE(found,pair,0);
			else
				MKNODE(NEXT(prev),pair,0);
			return found;

		case I_FORMULA:
			return found;

		case I_BOP:
			/* OPNAME should be "^" */
			if ( !IS_BINARYPWR(pat) )
				error("nfnode_pvar : invalid BOP");
			found = nfnode_pvars(FA1(pat),found);
			found = nfnode_pvars(FA2(pat),found);
			return found;

		case I_FUNC:
			t = (NODE)FA0((FNODE)FA1(pat));
			for ( ; t; t = NEXT(t) )
				found = nfnode_pvars(BDY(t),found);
			return found;

		case I_NARYOP:
			t = (NODE)FA1(pat);
			for ( ; t; t = NEXT(t) )
				found = nfnode_pvars(BDY(t),found);
			return found;

		default:
			error("nfnode_match : invalid pattern");
	}
}
