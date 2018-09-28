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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/strobj.c,v 1.1 2018/09/19 05:45:06 noro Exp $
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
#elif defined(VISUAL) || defined(__MINGW32__)
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
#define IS_MUL(f) (((f)->id==I_NARYOP||(f)->id==I_BOP) &&(OPNAME(f)=='*'))

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

void Pqt_is_integer(),Pqt_is_rational(),Pqt_is_number(),Pqt_is_coef();
void Pqt_is_dependent(),Pqt_is_function(),Pqt_is_var();
void Pqt_set_ord(),Pqt_set_coef(),Pqt_set_weight();
void Pqt_normalize();
void Pnqt_comp(),Pnqt_weight();
void Pnqt_match();
void Pnqt_match_rewrite();

void Pqt_to_nbp();
void Pshuffle_mul(), Pharmonic_mul();
void Pnbp_hm(), Pnbp_ht(), Pnbp_hc(), Pnbp_rest();
void Pnbp_tm(), Pnbp_tt(), Pnbp_tc(), Pnbp_trest();
void Pnbm_deg(), Pnbm_index();
void Pnbm_hp_rest();
void Pnbm_hxky(), Pnbm_xky_rest();
void Pnbm_hv(), Pnbm_tv(), Pnbm_rest(),Pnbm_trest();

void Pquote_to_funargs(),Pfunargs_to_quote(),Pget_function_name();
void Pqt_match(),Pget_quote_id();
void Pqt_to_nary(),Pqt_to_bin();
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
int qt_match(Obj f,Obj pat,NODE *rp);
FNODE partial_eval(FNODE), fnode_to_nary(FNODE), fnode_to_bin(FNODE,int);
FNODE nfnode_add(FNODE a1,FNODE a2,int expand);
FNODE nfnode_mul(FNODE a1,FNODE a2,int expand);
FNODE nfnode_pwr(FNODE a1,FNODE a2,int expand);
FNODE nfnode_mul_coef(Obj c,FNODE f,int expand);
FNODE fnode_expand_pwr(FNODE f,int n,int expand);
FNODE to_narymul(FNODE f);
FNODE to_naryadd(FNODE f);
FNODE fnode_node_to_nary(ARF op,NODE n);
void fnode_base_exp(FNODE f,FNODE *bp,FNODE *ep);
void fnode_coef_body(FNODE f,Obj *cp,FNODE *bp);
FNODE nfnode_match_rewrite(FNODE f,FNODE p,FNODE c,FNODE a,int mode);
FNODE fnode_apply(FNODE f,FNODE (*func)(),int expand);
FNODE fnode_normalize(FNODE f,int expand);
FNODE rewrite_fnode(FNODE f,NODE arg,int qarg);

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

  {"qt_is_var",Pqt_is_var,1},
  {"qt_is_coef",Pqt_is_coef,1},
  {"qt_is_number",Pqt_is_number,1},
  {"qt_is_rational",Pqt_is_rational,1},
  {"qt_is_integer",Pqt_is_integer,1},
  {"qt_is_function",Pqt_is_function,1},
  {"qt_is_dependent",Pqt_is_dependent,2},

  {"qt_set_coef",Pqt_set_coef,-1},
  {"qt_set_ord",Pqt_set_ord,-1},
  {"qt_set_weight",Pqt_set_weight,-1},
  {"qt_normalize",Pqt_normalize,-2},
  {"qt_match",Pqt_match,2},
  {"nqt_match_rewrite",Pnqt_match_rewrite,3},

  {"nqt_weight",Pnqt_weight,1},
  {"nqt_comp",Pnqt_comp,2},
  {"nqt_match",Pnqt_match,-3},
  {"qt_to_nbp",Pqt_to_nbp,1},
  {"shuffle_mul",Pshuffle_mul,2},
  {"harmonic_mul",Pharmonic_mul,2},

  {"nbp_hm", Pnbp_hm,1},
  {"nbp_ht", Pnbp_ht,1},
  {"nbp_hc", Pnbp_hc,1},
  {"nbp_rest", Pnbp_rest,1},
  {"nbp_tm", Pnbp_tm,1},
  {"nbp_tt", Pnbp_tt,1},
  {"nbp_tc", Pnbp_tc,1},
  {"nbp_trest", Pnbp_trest,1},
  {"nbm_deg", Pnbm_deg,1},
  {"nbm_index", Pnbm_index,1},
  {"nbm_hxky", Pnbm_hxky,1},
  {"nbm_xky_rest", Pnbm_xky_rest,1},
  {"nbm_hp_rest", Pnbm_hp_rest,1},
  {"nbm_hv", Pnbm_hv,1},
  {"nbm_tv", Pnbm_tv,1},
  {"nbm_rest", Pnbm_rest,1},
  {"nbm_trest", Pnbm_trest,1},

  {"qt_to_nary",Pqt_to_nary,1},
  {"qt_to_bin",Pqt_to_bin,2},

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
    dp_vars_origin = ZTOS((Q)arg);
    return 1;
  } else return 0;
}

int register_dp_dvars_origin(Obj arg)
{
  if ( INT(arg) ) {
    dp_dvars_origin = ZTOS((Q)arg);
    return 1;
  } else return 0;
}

int register_dp_vars_hweyl(Obj arg)
{
  if ( INT(arg) ) {
    dp_vars_hweyl = ZTOS((Q)arg);
    return 1;
  } else return 0;
}

int register_show_lt(Obj arg)
{
  if ( INT(arg) ) {
    show_lt = ZTOS((Q)arg);
    return 1;
  } else return 0;
}

int register_conv_rule(Obj arg)
{
  if ( INT(arg) ) {
    conv_flag = ZTOS((Q)arg);
    convfunc = 0;
    return 1;
  } else return 0;
}

int register_conv_func(Obj arg)
{
  if ( !arg ) {
    convfunc = 0;
    return 1;
  } else if ( OID(arg) == O_P && (long)(VR((P)arg))->attr == V_SR ) {
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

void Pqt_to_nary(NODE arg,QUOTE *rp)
{
  FNODE f;

  f = fnode_to_nary(BDY((QUOTE)ARG0(arg)));
  MKQUOTE(*rp,f);  
}

void Pqt_to_bin(NODE arg,QUOTE *rp)
{
  FNODE f;
  int direction;

  direction = ZTOS((Q)ARG1(arg));
  f = fnode_to_bin(BDY((QUOTE)ARG0(arg)),direction);

  MKQUOTE(*rp,f);  
}

void Pqt_is_var(NODE arg,Z *rp)
{
  QUOTE q;
  int ret;

  q = (QUOTE)ARG0(arg);
  asir_assert(q,O_QUOTE,"qt_is_var");
  ret = fnode_is_var(BDY(q));
  STOZ(ret,*rp);
}

void Pqt_is_coef(NODE arg,Z *rp)
{
  QUOTE q;
  int ret;

  q = (QUOTE)ARG0(arg);
  asir_assert(q,O_QUOTE,"qt_is_coef");
  ret = fnode_is_coef(BDY(q));
  STOZ(ret,*rp);
}

void Pqt_is_number(NODE arg,Z *rp)
{
  QUOTE q;
  int ret;

  q = (QUOTE)ARG0(arg);
  asir_assert(q,O_QUOTE,"qt_is_number");
  ret = fnode_is_number(BDY(q));
  STOZ(ret,*rp);
}

void Pqt_is_rational(NODE arg,Z *rp)
{
  QUOTE q;
  int ret;

  q = (QUOTE)ARG0(arg);
  asir_assert(q,O_QUOTE,"qt_is_rational");
  ret = fnode_is_rational(BDY(q));
  STOZ(ret,*rp);
}

void Pqt_is_integer(NODE arg,Z *rp)
{
  QUOTE q;
  int ret;

  q = (QUOTE)ARG0(arg);
  asir_assert(q,O_QUOTE,"qt_is_integer");
  ret = fnode_is_integer(BDY(q));
  STOZ(ret,*rp);
}

void Pqt_is_function(NODE arg,Z *rp)
{
  QUOTE q;
  int ret;

  q = (QUOTE)ARG0(arg);
  asir_assert(q,O_QUOTE,"qt_is_function");
  if ( q->id == I_FUNC || q->id == I_IFUNC )
    ret = 1;
  else
    ret = 0;
  STOZ(ret,*rp);
}

void Pqt_is_dependent(NODE arg,Z *rp)
{
  P x;
  QUOTE q,v;
  int ret;
  V var;

  q = (QUOTE)ARG0(arg);
  v = (QUOTE)ARG1(arg);
  asir_assert(q,O_QUOTE,"qt_is_dependent");
  asir_assert(v,O_QUOTE,"qt_is_dependent");
  x = (P)eval(BDY(v));
  if ( !x || OID(x) != O_P )
    *rp = 0;
  var = VR(x);
  ret = fnode_is_dependent(BDY(q),var);
  STOZ(ret,*rp);
}


void Pqt_match(NODE arg,Z *rp)
{
  FNODE f,g;
  Obj obj;
  QUOTE q;
  NODE r;
  int ret;

  obj = (Obj)ARG0(arg);
  ret = qt_match(obj,(Obj)ARG1(arg),&r);
  if ( ret ) {
    do_assign(r);
    *rp = ONE;
  } else
    *rp = 0;
}

void Pnqt_match(NODE arg,Z *rp)
{
  QUOTE fq,pq;
  FNODE f,p;
  int ret;
  Z mode;
  NODE r;

  mode = argc(arg)==3 ? (Z)ARG2(arg) : 0;
  fq = (QUOTE)ARG0(arg); Pqt_normalize(mknode(2,fq,mode),&fq); f = (FNODE)BDY(fq);
  pq = (QUOTE)ARG1(arg); Pqt_normalize(mknode(2,pq,mode),&pq); p = (FNODE)BDY(pq);
  ret = nfnode_match(f,p,&r);
  if ( ret ) {
    fnode_do_assign(r);
    *rp = ONE;
  } else
    *rp = 0;
}

void Pnqt_match_rewrite(NODE arg,Obj *rp)
{
  FNODE f,p,c,a,r;
  Obj obj,pat,cond,action;
  NODE rule;
  QUOTE q;
  Z mode;
  int m;

  obj = (Obj)ARG0(arg);
  rule = BDY((LIST)ARG1(arg));
  mode = (Z)ARG2(arg);
  if ( length(rule) == 2 ) {
    pat = ARG0(rule);
    cond = (Obj)ONE;
    action = (Obj)ARG1(rule);
  } else {
    pat = ARG0(rule);
    cond = ARG1(rule);
    action = (Obj)ARG2(rule);
  }
  Pqt_normalize(mknode(2,obj,mode),&q); f = (FNODE)BDY(q);
  Pqt_normalize(mknode(2,pat,mode),&q); p = (FNODE)BDY(q);
  Pqt_normalize(mknode(2,action,mode),&q);
  a = (FNODE)BDY(q);
  if ( OID(cond) == O_QUOTE ) c = BDY((QUOTE)cond);
  else c = mkfnode(1,I_FORMULA,ONE);

  m = ZTOS(mode);
  r = nfnode_match_rewrite(f,p,c,a,m);
  if ( r ) {
    MKQUOTE(q,r);
    *rp = (Obj)q;
  } else
    *rp = obj;
}

/* f is NARYOP => do submatch */

#define PV_ANY 99999999

FNODE nfnode_match_rewrite(FNODE f,FNODE p,FNODE c,FNODE a,int mode)
{
  ARF op;
  NODE arg,h0,t,h,valuen;
  NODE r,s0,s,pair;
  FNODE any,pany,head,tail,a1,a2;
  QUOTE q;
  int ret;
  FNODE value;
  int ind;

  if ( f->id == I_NARYOP ) {
    op = (ARF)FA0(f);
    arg = (NODE)FA1(f);
    pany = 0;
    for ( h0 = 0, t = arg; t; t = NEXT(t) ) {
      tail = fnode_node_to_nary(op,t);
      ret = nfnode_match(tail,p,&r) && eval(rewrite_fnode(c,r,1));
      if ( ret ) break;

      /* append a variable to the pattern */
      if ( !pany ) {
        any = mkfnode(1,I_PVAR,PV_ANY);
        pany = mkfnode(3,I_BOP,op,p,any);
        pany = fnode_normalize(pany,mode);
      }
      ret = nfnode_match(tail,pany,&r) && eval(rewrite_fnode(c,r,1));
      if ( ret ) {
        a = fnode_normalize(mkfnode(3,I_BOP,op,a,any),mode);
        break;
      }

      NEXTNODE(h0,h);
      BDY(h) = BDY(t);
    }
    if ( t ) {
      if ( h0 ) NEXT(h) = 0;
      head = fnode_node_to_nary(op,h0);
      a = fnode_normalize(mkfnode(3,I_BOP,op,head,a),mode);
      ret = 1;
    } else
      ret = 0;
  } else
    ret = nfnode_match(f,p,&r) && eval(rewrite_fnode(c,r,1));

  if ( ret ) {
    a1 = rewrite_fnode(a,r,0);
    a2 = partial_eval(a1);
    return fnode_normalize(a2,mode);
  } else
    return 0;
}

void do_assign(NODE arg)
{
  NODE t,pair;
  int pv;

  QUOTE value;

  for ( t = arg; t; t = NEXT(t) ) {
    pair = BDY((LIST)BDY(t));
    pv = (long)FA0((FNODE)BDY((QUOTE)BDY(pair)));
    value = (QUOTE)(BDY(NEXT(pair)));
    ASSPV(pv,value);
  }
}

/* [[index,fnode],...] */

void fnode_do_assign(NODE arg)
{
  NODE t,pair;
  long pv;
  FNODE f;
  QUOTE value;
  QUOTEARG qa;

  for ( t = arg; t; t = NEXT(t) ) {
    pair = (NODE)BDY(t);
    pv = (long)BDY(pair);
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

int qt_match_node(NODE f,NODE pat,NODE *rp) {
  NODE r,a,tf,tp,r1;
  int ret;

  if ( length(f) != length(pat) ) return 0;
  r = 0;
  for ( tf = f, tp = pat; tf; tf = NEXT(tf), tp = NEXT(tp) ) {
    ret = qt_match((Obj)BDY(tf),(Obj)BDY(tp),&a);
    if ( !ret ) return 0;
    ret = merge_matching_node(r,a,&r1);
    if ( !ret ) return 0;
    else r = r1;
  }
  *rp = r;
  return 1;
}

/* f = [a,b,c,...] pat = [X,Y,...] rpat matches the rest of f */

int qt_match_cons(NODE f,NODE pat,Obj rpat,NODE *rp) {
  QUOTE q;
  Z id;
  FNODE fn;
  NODE r,a,tf,tp,r1,arg;
  int ret;
  LIST list,alist;

  /* matching of the head part */
  if ( length(f) < length(pat) ) return 0;
  r = 0;
  for ( tf = f, tp = pat; tp; tf = NEXT(tf), tp = NEXT(tp) ) {
    ret = qt_match((Obj)BDY(tf),(Obj)BDY(tp),&a);
    if ( !ret ) return 0;
    ret = merge_matching_node(r,a,&r1);
    if ( !ret ) return 0;
    else r = r1;
  }
  /* matching of the rest */
  MKLIST(list,tf);
  STOZ(I_LIST,id); a = mknode(2,id,list);
  MKLIST(alist,a);
  arg = mknode(1,alist);
  Pfunargs_to_quote(arg,&q);
  ret = qt_match((Obj)q,rpat,&a);
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
  *id = ZTOS((Q)BDY(fab)); *r = NEXT(fab);
}

/* *rp : [[quote(A),quote(1)],...] */

int qt_match(Obj f, Obj pat, NODE *rp)
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
      return qt_match_node(BDY((LIST)f),BDY((LIST)pat),rp);
    else
      return 0;
  } else if ( OID(pat) == O_QUOTE ) {
    pid = ((FNODE)BDY((QUOTE)pat))->id;
    switch ( pid ) {
      case I_FORMULA:
        if ( compquote(CO,(QUOTE)f,(QUOTE)pat) )
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
          return qt_match_node(tf,tp,rp);
        else {
          rpat = (Obj)BDY(NEXT(parg));
          return qt_match_cons(tf,tp,rpat,rp);
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
          ret = qt_match(BDY(NEXT(farg)),
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
        return qt_match_node(NEXT(farg),NEXT(parg),rp);

      default:
        if ( OID(f) != O_QUOTE ) return 0;
        id = ((FNODE)BDY((QUOTE)f))->id;
        if ( id != pid ) return 0;
        get_quote_id_arg((QUOTE)pat,&pid,&parg);
        get_quote_id_arg((QUOTE)f,&id,&farg);
        return qt_match_node(farg,parg,rp);
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
Z *rp;
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
  STOZ(r,*rp);
}

void Pstr_chr(arg,rp)
NODE arg;
Z *rp;
{
  STRING str,terminator;
  Z start;
  char *p,*ind;
  int chr,spos,r;

  str = (STRING)ARG0(arg);
  start = (Z)ARG1(arg);
  terminator = (STRING)ARG2(arg);
  asir_assert(str,O_STR,"str_chr");
  asir_assert(start,O_N,"str_chr");
  asir_assert(terminator,O_STR,"str_chr");
  p = BDY(str);
  spos = ZTOS(start);
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
  STOZ(r,*rp);
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
  spos = ZTOS(head);
  epos = ZTOS(tail);
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
  Z q;

  str = (STRING)ARG0(arg);
  asir_assert(str,O_STR,"strtoascii");
  p = BDY(str);
  len = strlen(p);
  for ( i = len-1, n = 0; i >= 0; i-- ) {
    UTOZ((unsigned int)p[i],q);
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
  Z q;

  list = (LIST)ARG0(arg);
  asir_assert(list,O_LIST,"asciitostr");
  n = BDY(list);
  len = length(n);
  p = MALLOC_ATOMIC(len+1);
  for ( i = 0; i < len; i++, n = NEXT(n) ) {
    q = (Z)BDY(n);
    asir_assert(q,O_N,"asciitostr");
    j = ZTOS(q);
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
  SNODE snode;
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
#if 0
  exprparse_create_var(0,cmd,&fnode);
  *rp = eval(fnode);
#else
  exprparse_create_var(0,cmd,&snode);
  *rp = evalstat(snode);
#endif
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
  FUNC f;

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
  /* search global variables */
  gen_searchf_searchonly(p,&f,1);
  if ( f )
    makesrvar(f,rp);
  else
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

  qa = (QUOTEARG)BDY(arg);
  if ( !qa || OID(qa) != O_QUOTEARG ) {
    *rp = 0; return;
  }
  switch ( qa->type ) {
    case A_arf:
      MKSTR(*rp,((ARF)BDY(qa))->name);
      break;
    case A_func:
      MKSTR(*rp,((FUNC)BDY(qa))->name);
      break;
    default:
      *rp = 0;
      break;
  }
}

FNODE strip_paren(FNODE);
void objtotex_tb(Obj obj,TB tb);

void fnodetotex_tb(FNODE f,TB tb)
{
  NODE n,t,t0,args;
  char vname[BUFSIZ],prefix[BUFSIZ];
  char *opname,*vname_conv,*prefix_conv;
  Obj obj;
  int i,len,allzero,elen,elen2,si;
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
          f1 = (FNODE)FA1(f);
          if ( fnode_is_var(f1) )
            fnodetotex_tb(f1,tb);
          else {
            write_tb("(",tb);
            fnodetotex_tb(f1,tb);
            write_tb(")",tb);
          }
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
      switch ( OPNAME(f) ) {
        case '+':
          fnodetotex_tb((FNODE)BDY(args),tb);
          for ( args = NEXT(args); args; args = NEXT(args) ) {
            write_tb("+",tb);
            f1 = (FNODE)BDY(args);
            /* if ( fnode_is_var(f1) || IS_MUL(f1) )
              fnodetotex_tb(f1,tb);
            else */ {
              write_tb("(",tb);
              fnodetotex_tb(f1,tb);
              write_tb(")",tb);
            }
          }
          break;
        case '*':
          f1 = (FNODE)BDY(args);
          if ( f1->id == I_FORMULA && MUNIQ(FA0(f1)) ) {
            write_tb("- ",tb); args = NEXT(args);
          }
          for ( ; args; args = NEXT(args) ) {
            f2 = (FNODE)BDY(args);
            if ( fnode_is_var(f2) || IS_BINARYPWR(f2) )
              fnodetotex_tb(f2,tb);
            else {
              write_tb("(",tb);
              fnodetotex_tb(f2,tb);
              write_tb(")",tb);
            }
          }
          break;
        default:
          error("invalid nary op");
          break;
      }
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
      objtotex_tb((Obj)FA0(f),tb);
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

void objtotex_tb(Obj obj,TB tb)
{
  C cplx;
  char *r;
  P t;
  DCP dc;
  char *v;

  if ( !obj ) {
    write_tb("0",tb);
    return;
  }
  switch ( OID(obj) ) {
    case O_N:
      switch ( NID(obj) ) {
        case N_C:
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
          break;
        default:
          write_tb(objtostr(obj),tb);
          break;
      }
      break;
    case O_P:
      v = conv_rule(VR((P)obj)->name);
      for ( dc = DC((P)obj); dc; dc = NEXT(dc) ) {
        if ( !DEG(dc) )
          objtotex_tb((Obj)COEF(dc),tb);
        else {
          if ( NUM(COEF(dc)) && UNIQ((Q)COEF(dc)) )
            ;
          else if ( NUM(COEF(dc)) && MUNIQ((Q)COEF(dc)) )
            write_tb("-",tb);
          else if ( NUM(COEF(dc)) || !NEXT(DC(COEF(dc)))) 
            objtotex_tb((Obj)COEF(dc),tb); 
          else {
            write_tb("(",tb); objtotex_tb((Obj)COEF(dc),tb);
            write_tb(")",tb);
          }
          write_tb(v,tb);
          if ( cmpz(DEG(dc),ONE) ) {
            write_tb("^",tb);
            if ( INT(DEG(dc)) && sgnz(DEG(dc))>0 ) {
              write_tb("{",tb);
              objtotex_tb((Obj)DEG(dc),tb);
              write_tb("}",tb);
            } else {
              write_tb("{",tb); objtotex_tb((Obj)DEG(dc),tb);
              write_tb("}",tb);
            }
          }
        }
        if ( NEXT(dc) ) {
          t = COEF(NEXT(dc));
          if ( !DEG(NEXT(dc)) ) {
            if ( NUM(t) ) {
              if ( !mmono(t) ) write_tb("+",tb);
            } else {
              if ( !mmono(COEF(DC(t))) ) write_tb("+",tb);
            }
          } else {
            if ( !mmono(t) ) write_tb("+",tb);
          }
        }
      }
      break;
    case O_R:
      write_tb("\\frac{",tb);
      objtotex_tb((Obj)NM((R)obj),tb);
      write_tb("}{",tb);
      objtotex_tb((Obj)DN((R)obj),tb);
      write_tb("}",tb);
      break;
    default:
      write_tb(objtostr(obj),tb);
      break;
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
#if 0
            /* must be a variable */
            opname = conv_rule(VR((P)obj)->name);
            return opname[0]=='-';
#else
            return mmono((P)obj);
#endif
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

void Pget_quote_id(NODE arg,Z *rp)
{
  FNODE f;
  QUOTE q;

  q = (QUOTE)ARG0(arg);
  if ( !q || OID(q) != O_QUOTE )
    error("get_quote_id : invalid argument");
  f = BDY(q);
  STOZ((long)f->id,*rp);
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
  Z id,a;
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
  STOZ((int)f->id,id);
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
        STOZ((long)f->arg[i],a);
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
  id = (fid)ZTOS((Q)BDY(t)); t = NEXT(t);
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
        f->arg[i] = (pointer)ZTOS((Q)a);
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

VL reordvars(VL vl0,NODE head)
{
  VL vl,svl,tvl;
  int i,j;
  NODE n;
  P t;
  V *va;
  V v;

  for ( vl = 0, i = 0, n = head; n; n = NEXT(n), i++ ) {
    NEXTVL(vl,tvl);
    if ( !(t = (P)BDY(n)) || (OID(t) != O_P) )
      error("reordvars : invalid argument");
    VR(tvl) = VR(t);
  }
  va = (V *)ALLOCA(i*sizeof(V));
  for ( j = 0, svl = vl; j < i; j++, svl = NEXT(svl) )
    va[j] = VR(svl);
  for ( svl = vl0; svl; svl = NEXT(svl) ) {
    v = VR(svl);
    for ( j = 0; j < i; j++ )
      if ( v == va[j] )
        break;
    if ( j == i ) {
      NEXTVL(vl,tvl);
      VR(tvl) = v;
    }
  }
  if ( vl )
    NEXT(tvl) = 0;
  return vl;
}

struct wtab *qt_weight_tab;
VL qt_current_ord, qt_current_coef;
LIST qt_current_ord_obj,qt_current_coef_obj,qt_current_weight_obj;
LIST qt_current_weight_obj;

void Pqt_set_ord(NODE arg,LIST *rp)
{
  NODE r0,r;
  VL vl;
  P v;

  if ( !argc(arg) ) 
    *rp = qt_current_ord_obj;
  else if ( !ARG0(arg) ) {
    qt_current_ord_obj = 0;
    qt_current_ord = 0;
  } else {
    qt_current_ord = reordvars(CO,BDY((LIST)ARG0(arg)));
    for ( r0 = 0, vl = qt_current_ord; vl; vl = NEXT(vl) ) {
      NEXTNODE(r0,r); MKV(vl->v,v); BDY(r) = v;
    }
    if ( r0 ) NEXT(r) = 0;
    MKLIST(*rp,r0);
    qt_current_ord_obj = *rp;
  }
}

void Pqt_set_weight(NODE arg,LIST *rp)
{
  NODE n,pair;
  int l,i;
  struct wtab *tab;

  if ( !argc(arg) ) 
    *rp = qt_current_weight_obj;
  else if ( !ARG0(arg) ) {
    qt_current_weight_obj = 0;
    qt_weight_tab = 0;
  } else {
    n = BDY((LIST)ARG0(arg));
    l = length(n);
    tab = qt_weight_tab = (struct wtab *)MALLOC((l+1)*sizeof(struct wtab));
    for ( i = 0; i < l; i++, n = NEXT(n) ) {
      pair = BDY((LIST)BDY(n));
      tab[i].v = VR((P)ARG0(pair));
      tab[i].w = ZTOS((Q)ARG1(pair));
    }
    tab[i].v = 0;
    qt_current_weight_obj = (LIST)ARG0(arg);
    *rp = qt_current_weight_obj;
  }
}

void Pqt_set_coef(NODE arg,LIST *rp)
{
  NODE r0,r,n;
  VL vl0,vl;
  P v;

  if ( !argc(arg) ) 
    *rp = qt_current_coef_obj;
  else if ( !ARG0(arg) ) {
    qt_current_coef_obj = 0;
    qt_current_coef = 0;
  } else {
    n = BDY((LIST)ARG0(arg));
    for ( vl0 = 0, r0 = 0; n; n = NEXT(n) ) {
      NEXTNODE(r0,r);
      NEXTVL(vl0,vl);
      vl->v = VR((P)BDY(n));
      MKV(vl->v,v); BDY(r) = v;
    }
    if ( r0 ) NEXT(r) = 0;
    if ( vl0 ) NEXT(vl) = 0;
    qt_current_coef = vl0;
    MKLIST(*rp,r0);
    qt_current_coef_obj = *rp;
  }
}

void Pqt_normalize(NODE arg,QUOTE *rp)
{
  QUOTE q,r;
  FNODE f;
  int expand,ac;

  ac = argc(arg);
  if ( !ac ) error("qt_normalize : invalid argument");
  q = (QUOTE)ARG0(arg);
  if ( ac == 2 )
    expand = ZTOS((Q)ARG1(arg));
  if ( !q || OID(q) != O_QUOTE )
    *rp = q;
  else {
    f = fnode_normalize(BDY(q),expand);
    MKQUOTE(r,f);
    *rp = r;
  }
}

NBP fnode_to_nbp(FNODE f);

void Pqt_to_nbp(NODE arg,NBP *rp)
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
    m1->d = m->d; m1->c = (P)ONE; m1->b = m->b;
    MKNODE(n,m1,0);
    MKNBP(*rp,n);
  }
}

void Pnbp_hc(NODE arg, P *rp)
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

void Pnbp_tm(NODE arg, NBP *rp)
{
  NBP p;
  NODE n;
  NBM m;

  p = (NBP)ARG0(arg);
  if ( !p ) *rp = 0;
  else {
    for ( n = BDY(p); NEXT(n); n = NEXT(n) );
    m = (NBM)BDY(n);
    MKNODE(n,m,0);
    MKNBP(*rp,n);
  }
}
  
void Pnbp_tt(NODE arg, NBP *rp)
{
  NBP p;
  NODE n;
  NBM m,m1;

  p = (NBP)ARG0(arg);
  if ( !p ) *rp = 0;
  else {
    for ( n = BDY(p); NEXT(n); n = NEXT(n) );
    m = (NBM)BDY(n);
    NEWNBM(m1);
    m1->d = m->d; m1->c = (P)ONE; m1->b = m->b;
    MKNODE(n,m1,0);
    MKNBP(*rp,n);
  }
}

void Pnbp_tc(NODE arg, P *rp)
{
  NBP p;
  NBM m;
  NODE n;

  p = (NBP)ARG0(arg);
  if ( !p ) *rp = 0;
  else {
    for ( n = BDY(p); NEXT(n); n = NEXT(n) );
    m = (NBM)BDY(n);
    *rp = m->c;
  }
}

void Pnbp_trest(NODE arg, NBP *rp)
{
  NBP p;
  NODE n,r,r0;

  p = (NBP)ARG0(arg);
  if ( !p ) *rp = 0;
  else {
    n = BDY(p);
    for ( r0 = 0; NEXT(n); n = NEXT(n) ) {
      NEXTNODE(r0,r);
      BDY(r) = (pointer)BDY(n);
    }
    if ( r0 ) {
      NEXT(r) = 0;
      MKNBP(*rp,r0);
    } else
      *rp = 0;
  }
}

void Pnbm_deg(NODE arg, Z *rp)
{
  NBP p;
  NBM m;

  p = (NBP)ARG0(arg);
  if ( !p )
    STOZ(-1,*rp);
  else {
    m = (NBM)BDY(BDY(p));  
    STOZ(m->d,*rp);
  }
}

void Pnbm_index(NODE arg, Z *rp)
{
  NBP p;
  NBM m;
  unsigned int *b;
  int d,i,r;

  p = (NBP)ARG0(arg);
  if ( !p )
    STOZ(0,*rp);
  else {
    m = (NBM)BDY(BDY(p));  
    d = m->d;
    if ( d > 32 )
      error("nbm_index : weight too large");
    b = m->b;
    for ( r = 0, i = d-2; i > 0; i-- )
      if ( !NBM_GET(b,i) ) r |= (1<<(d-2-i));
    STOZ(r,*rp);
  }
}

void Pnbm_hp_rest(NODE arg, LIST *rp)
{
  NBP p,h,r;
  NBM m,m1;
  NODE n;
  int *b,*b1;
  int d,d1,v,i,j,k;

  p = (NBP)ARG0(arg);
  if ( !p )
    MKLIST(*rp,0);
  else {
    m = (NBM)BDY(BDY(p));  
    b = m->b; d = m->d;
    if ( !d )
      MKLIST(*rp,0);
    else {
      v = NBM_GET(b,0);
      for ( i = 1; i < d; i++ )
        if ( NBM_GET(b,i) != v ) break;
      NEWNBM(m1); NEWNBMBDY(m1,i); 
      b1 = m1->b; m1->d = i; m1->c = (P)ONE;
      if ( v ) for ( j = 0; j < i; j++ ) NBM_SET(b1,j);
      else for ( j = 0; j < i; j++ ) NBM_CLR(b1,j);
      MKNODE(n,m1,0); MKNBP(h,n);

      d1 = d-i;
      NEWNBM(m1); NEWNBMBDY(m1,d1);
      b1 = m1->b; m1->d = d1; m1->c = (P)ONE;
      for ( j = 0, k = i; j < d1; j++, k++ )
        if ( NBM_GET(b,k) ) NBM_SET(b1,j);
        else NBM_CLR(b1,j);
      MKNODE(n,m1,0); MKNBP(r,n);
      n = mknode(2,h,r);
      MKLIST(*rp,n);
    }
  }
}

void Pnbm_hxky(NODE arg, NBP *rp)
{
  NBP p;

  p = (NBP)ARG0(arg);
  if ( !p )
    *rp = 0;
  else
    separate_xky_nbm((NBM)BDY(BDY(p)),0,rp,0);
}

void Pnbm_xky_rest(NODE arg,NBP *rp)
{
  NBP p;

  p = (NBP)ARG0(arg);
  if ( !p )
    *rp = 0;
  else
    separate_xky_nbm((NBM)BDY(BDY(p)),0,0,rp);
}

void Pnbm_hv(NODE arg, NBP *rp)
{
  NBP p;
  
  p = (NBP)ARG0(arg);
  if ( !p )
    *rp = 0;
  else
    separate_nbm((NBM)BDY(BDY(p)),0,rp,0);
}

void Pnbm_rest(NODE arg, NBP *rp)
{
  NBP p;
  
  p = (NBP)ARG0(arg);
  if ( !p )
    *rp = 0;
  else
    separate_nbm((NBM)BDY(BDY(p)),0,0,rp);
}

void Pnbm_tv(NODE arg, NBP *rp)
{
  NBP p;
  
  p = (NBP)ARG0(arg);
  if ( !p )
    *rp = 0;
  else
    separate_tail_nbm((NBM)BDY(BDY(p)),0,0,rp);
}

void Pnbm_trest(NODE arg, NBP *rp)
{
  NBP p;
  
  p = (NBP)ARG0(arg);
  if ( !p )
    *rp = 0;
  else
    separate_tail_nbm((NBM)BDY(BDY(p)),0,rp,0);
}

NBP fnode_to_nbp(FNODE f)
{
  Z r;
  int n,i;
  NBM m;
  V v;
  NBP u,u1,u2;
  NODE t,b;

  if ( f->id == I_FORMULA ) {
    r = eval(f);
    NEWNBM(m);
    if ( OID(r) == O_N ) {
      m->d = 0; m->c = (P)r; m->b = 0;
    } else {
      v = VR((P)r);
      m->d = 1; m->c = (P)ONE; NEWNBMBDY(m,1);
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

void Pnqt_weight(NODE arg,Z *rp)
{
  QUOTE q;
  FNODE f;
  int w;

  q = (QUOTE)ARG0(arg); f = (FNODE)BDY(q);
  f = fnode_normalize(f,0);
  w = nfnode_weight(qt_weight_tab,f);
  STOZ(w,*rp);
}

void Pnqt_comp(NODE arg,Z *rp)
{
  QUOTE q1,q2;
  FNODE f1,f2;
  int r;

  q1 = (QUOTE)ARG0(arg); f1 = (FNODE)BDY(q1);
  q2 = (QUOTE)ARG1(arg); f2 = (FNODE)BDY(q2);
  f1 = fnode_normalize(f1,0);
  f2 = fnode_normalize(f2,0);
  r = nfnode_comp(f1,f2);
  STOZ(r,*rp);
}

int fnode_is_var(FNODE f)
{
  Obj obj;
  VL vl,t,s;
  DCP dc;

  if ( fnode_is_coef(f) ) return 0;
  switch ( f->id ) {
    case I_PAREN:
      return fnode_is_var(FA0(f));
        
    case I_FORMULA:
      obj = FA0(f);
      if ( obj && OID(obj) == O_P ) {
        dc = DC((P)obj);
        if ( !cmpz(DEG(dc),ONE) && !NEXT(dc) 
          && !arf_comp(CO,(Obj)COEF(dc),(Obj)ONE) ) return 1;
        else return 0;
      } else return 0;
    
    default:
      return 0;
  }
}

int fnode_is_coef(FNODE f)
{
  Obj obj;
  VL vl,t,s;

  switch ( f->id ) {
    case I_MINUS: case I_PAREN:
      return fnode_is_coef(FA0(f));
        
    case I_FORMULA:
      obj = FA0(f);
      if ( !obj ) return 1;
      else if ( OID(obj) == O_QUOTE ) 
        return fnode_is_coef(BDY((QUOTE)obj));
      else if ( NUM(obj) ) return 1;
      else if ( OID(obj) == O_P || OID(obj) == O_R) {
        get_vars_recursive(obj,&vl);
        for ( t = vl; t; t = NEXT(t) ) {
          if ( t->v->attr == (pointer)V_PF ) continue;
          for ( s = qt_current_coef; s; s = NEXT(s) )
            if ( t->v == s->v ) break;
          if ( !s )
            return 0;
        }
        return 1;
      } else return 0;
    
    case I_BOP:
      return fnode_is_coef(FA1(f)) && fnode_is_coef(FA2(f));
    
    default:
      return 0;
  }
}

int fnode_is_number(FNODE f)
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
  Z n;

  n = (Z)eval(f);
  if ( !n || (INT(n) && sgnz(n) > 0) ) return 1;
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


FNODE fnode_normalize(FNODE f,int expand)
{
  FNODE a1,a2,mone,r,b2;
  NODE n;
  Z q;

  if ( f->normalized && (f->expanded == expand) ) return f;
  STOZ(-1,q);
  mone = mkfnode(1,I_FORMULA,q);
  switch ( f->id ) {
    case I_PAREN:
      r = fnode_normalize(FA0(f),expand);
      break;
    
    case I_MINUS:
      r = nfnode_mul_coef((Obj)q,
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
          a2 = nfnode_mul_coef((Obj)q,a2,expand);
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
  Obj c1,c2,c;

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
      arf_add(CO,c1,c2,&c);
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

  return fnode_node_to_nary(addfs,r0);
}

FNODE fnode_node_to_nary(ARF op,NODE n)
{
  if ( !n ) {
    if ( op->name[0] == '+' )
      return mkfnode(1,I_FORMULA,NULLP);
    else
      return mkfnode(1,I_FORMULA,ONE);
  } else if ( !NEXT(n) ) return BDY(n);
  else return mkfnode(2,I_NARYOP,op,n);
}

FNODE nfnode_mul(FNODE f1,FNODE f2,int expand)
{
  NODE n1,n2,r0,r,r1;
  FNODE b1,b2,e1,e2,cc,t,t1;
  FNODE *m;
  int s;
  Obj c1,c2,c,e;
  int l1,l,i,j;

  if ( IS_ZERO(f1) || IS_ZERO(f2) ) return mkfnode(1,I_FORMULA,NULLP);
  else if ( fnode_is_coef(f1) ) 
    return nfnode_mul_coef((Obj)eval(f1),f2,expand);
  else if ( fnode_is_coef(f2) ) 
    return nfnode_mul_coef((Obj)eval(f2),f1,expand);

  if ( expand && IS_NARYADD(f1) ) {
    t = mkfnode(1,I_FORMULA,NULLP);
    for ( n1 = (NODE)FA1(f1); n1; n1 = NEXT(n1) ) {
      t1 = nfnode_mul(BDY(n1),f2,expand);
      t = nfnode_add(t,t1,expand);
    }
    return t;
  }
  if ( expand && IS_NARYADD(f2) ) {
    t = mkfnode(1,I_FORMULA,NULLP);
    for ( n2 = (NODE)FA1(f2); n2; n2 = NEXT(n2) ) {
      t1 = nfnode_mul(f1,BDY(n2),expand);
      t = nfnode_add(t,t1,expand);
    }
    return t;
  }

  fnode_coef_body(f1,&c1,&b1); fnode_coef_body(f2,&c2,&b2);
  arf_mul(CO,c1,c2,&c);
  if ( !c ) return mkfnode(1,I_FORMULA,NULLP);


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
      arf_add(CO,eval(e1),eval(e2),&e);
      if ( !e ) i--;
      else if ( expand == 2 ) {
        if ( INT(e) && sgnz((Z)e) < 0 ) {
          t1 = mkfnode(3,I_BOP,pwrfs,b1,mkfnode(1,I_FORMULA,e));
          /* r=(r0|rest)->(r0,t1|rest) */
          t = BDY(r);
          MKNODE(r1,t1,NEXT(r));
          MKNODE(r,t,r1);
          i--;
        } else
          m[i++] = BDY(r);
      } else if ( UNIQ(e) )
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
  return fnode_node_to_nary(mulfs,r);
}

FNODE nfnode_pwr(FNODE f1,FNODE f2,int expand)
{
  FNODE b,b1,e1,e,cc,r,mf2,mone,inv;
  Obj c,c1;
  Num nf2;
  int ee;
  NODE arg,n,t0,t1;
  Z q;

  if ( IS_ZERO(f2) ) return mkfnode(1,I_FORMULA,ONE);
  else if ( IS_ZERO(f1) ) return mkfnode(1,I_FORMULA,NULLP);
  else if ( fnode_is_coef(f1) ) {
    if ( fnode_is_integer(f2) ) {
      if ( fnode_is_one(f2) ) return f1;
      else {
        arf_pwr(CO,eval(f1),(Obj)eval(f2),&c);
        return mkfnode(1,I_FORMULA,c);
      }
    } else {
      f1 = mkfnode(1,I_FORMULA,eval(f1));
      return mkfnode(3,I_BOP,pwrfs,f1,f2);
    }
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
    arf_pwr(CO,c1,(Obj)nf2,&c);
    ee = ZTOS((Q)nf2);
    cc = mkfnode(1,I_FORMULA,c);
    if ( fnode_is_nonnegative_integer(f2) )
      b = fnode_expand_pwr(b1,ee,expand);
    else {
      STOZ(-1,q);
      mone = mkfnode(1,I_FORMULA,q);
      b1 = to_narymul(b1);
      for ( t0 = 0, n = (NODE)FA1(b1); n; n = NEXT(n) ) {
        inv = mkfnode(3,I_BOP,pwrfs,BDY(n),mone);
        MKNODE(t1,inv,t0); t0 = t1;
      }
      b1 = fnode_node_to_nary(mulfs,t0);
      b = fnode_expand_pwr(b1,-ee,expand);
    }
    if ( fnode_is_one(cc) )
      return b;
    else
      return fnode_node_to_nary(mulfs,mknode(2,cc,b));
  } else if ( expand && fnode_is_integer(f2) 
      && fnode_is_nonnegative_integer(f2) ) {
    q = (Z)eval(f2);
    if ( !smallz(q) ) error("nfnode_pwr : exponent too large");
    return fnode_expand_pwr(f1,ZTOS(q),expand);
  } else
    return mkfnode(3,I_BOP,pwrfs,f1,f2);
}

FNODE fnode_expand_pwr(FNODE f,int n,int expand)
{
  int n1,i;
  FNODE f1,f2,fn;
  Z q;

  if ( !n ) return mkfnode(1,I_FORMULA,ONE);
  else if ( IS_ZERO(f) ) return mkfnode(1,I_FORMULA,NULLP);
  else if ( n == 1 ) return f;
  else {
    switch ( expand ) {
      case 1:
        n1 = n/2;
        f1 = fnode_expand_pwr(f,n1,expand);
        f2 = nfnode_mul(f1,f1,expand);
        if ( n%2 ) f2 = nfnode_mul(f2,f,1);
        return f2;
      case 2:
        for ( i = 1, f1 = f; i < n; i++ )
          f1 = nfnode_mul(f1,f,expand);
        return f1;
      case 0: default:
        STOZ(n,q);
        fn = mkfnode(1,I_FORMULA,q);
        return mkfnode(3,I_BOP,pwrfs,f,fn);
    }
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

FNODE nfnode_mul_coef(Obj c,FNODE f,int expand)
{
  FNODE b1,cc;
  Obj c1,c2;
  NODE n,r,r0;

  if ( !c )
    return mkfnode(1,I_FORMULA,NULLP);
  else {
    fnode_coef_body(f,&c1,&b1);
    arf_mul(CO,c,c1,&c2);
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
        return fnode_node_to_nary(mulfs,n);
      } else if ( expand && IS_NARYADD(b1) ) {
        for ( r0 = 0, n = (NODE)FA1(b1); n; n = NEXT(n) ) {
          NEXTNODE(r0,r);
          BDY(r) = nfnode_mul_coef(c2,BDY(n),expand);
        }
        if ( r0 ) NEXT(r) = 0;
        return fnode_node_to_nary(addfs,r0);
      } else 
        return fnode_node_to_nary(mulfs,mknode(2,cc,b1));
    }
  }
}

void fnode_coef_body(FNODE f,Obj *cp,FNODE *bp)
{
  FNODE c;

  if ( fnode_is_coef(f) ) {
    *cp = (Obj)eval(f); *bp = mkfnode(1,I_FORMULA,ONE);
  } else if ( IS_NARYMUL(f) ) {
    c=(FNODE)BDY((NODE)FA1(f));
    if ( fnode_is_coef(c) ) {
      *cp = (Obj)eval(c);
      *bp = fnode_node_to_nary(mulfs,NEXT((NODE)FA1(f)));
    } else {
      *cp = (Obj)ONE; *bp = f;
    }
  } else {
    *cp = (Obj)ONE; *bp = f;
  }
}

int nfnode_weight(struct wtab *tab,FNODE f)
{
  NODE n;
  int w,w1;
  int i;
  Q a2;
  V v;

  switch ( f->id ) {
    case I_FORMULA:
      if ( fnode_is_coef(f) ) return 0;
      else if ( fnode_is_var(f) ) {
        if ( !tab ) return 0;
        v = VR((P)FA0(f));
        for ( i = 0; tab[i].v; i++ )
          if ( v == tab[i].v ) return tab[i].w;
        return 0;
      } else return 0;

    /* XXX */
    case I_PVAR: return 1;
    /* XXX */
    case I_FUNC: I_FUNC: I_FUNC_QARG:
      /* w(f) = 1 */
      /* w(f(a1,...,an)=w(a1)+...+w(an) */
      n = FA0((FNODE)FA1(f));
      for ( w = 0; n; n = NEXT(n) )
        w += nfnode_weight(tab,BDY(n));
      return w;
    case I_NARYOP:
      n = (NODE)FA1(f);
      if ( IS_NARYADD(f) )
        for ( w = nfnode_weight(tab,BDY(n)),
          n = NEXT(n); n; n = NEXT(n) ) {
          w1 = nfnode_weight(tab,BDY(n));
          w = MAX(w,w1);
        }
      else
        for ( w = 0; n; n = NEXT(n) )
          w += nfnode_weight(tab,BDY(n));
      return w;
    case I_BOP:
      /* must be binary power */
      /* XXX w(2^x)=0 ? */
      if ( fnode_is_rational(FA2(f)) ) {
        a2 = (Q)eval(FA2(f));
        w = ZTOS(a2);
      } else
        w = nfnode_weight(tab,FA2(f));
      return nfnode_weight(tab,FA1(f))*w;
    default:
      error("nfnode_weight : not_implemented");
  }
}

int nfnode_comp(FNODE f1,FNODE f2)
{
  int w1,w2;

  if ( qt_weight_tab ) {
    w1 = nfnode_weight(qt_weight_tab,f1);
    w2 = nfnode_weight(qt_weight_tab,f2);
    if ( w1 > w2 ) return 1;
    if ( w1 < w2 ) return -1;
  }
  return nfnode_comp_lex(f1,f2);
}

int nfnode_comp_lex(FNODE f1,FNODE f2)
{
  NODE n1,n2;
  int r,i1,i2,ret;
  char *nm1,*nm2;
  FNODE b1,b2,e1,e2,g,a1,a2,fn1,fn2,h1,h2;
  Num ee,ee1;
  Obj c1,c2;
  int w1,w2;

  if ( IS_NARYADD(f1) || IS_NARYADD(f2) ) {
    f1 = to_naryadd(f1); f2 = to_naryadd(f2);
    n1 = (NODE)FA1(f1); n2 = (NODE)FA1(f2);
    for ( ; n1 && n2; n1 = NEXT(n1), n2 = NEXT(n2) ) {
      r = nfnode_comp_lex(BDY(n1),BDY(n2));
      if ( r ) return r;
    }
    if ( !n1 && !n2 ) return 0;
    h1 = n1 ? (FNODE)BDY(n1) : mkfnode(1,I_FORMULA,NULLP);
    h2 = n2 ? (FNODE)BDY(n2) : mkfnode(1,I_FORMULA,NULLP);
    return nfnode_comp_lex(h1,h2);
  }
  if ( IS_NARYMUL(f1) || IS_NARYMUL(f2) ) {
    fnode_coef_body(f1,&c1,&b1);
    fnode_coef_body(f2,&c2,&b2);
    if ( !compfnode(b1,b2) ) return arf_comp(CO,c1,c2);
    b1 = to_narymul(b1); b2 = to_narymul(b2);
    n1 = (NODE)FA1(b1); n2 = (NODE)FA1(b2);
    for ( ; n1 && n2; n1 = NEXT(n1), n2 = NEXT(n2) ) {
      r = nfnode_comp_lex(BDY(n1),BDY(n2));
      if ( r ) return r;
    }
    if ( !n1 && !n2 ) return 0;
    h1 = n1 ? (FNODE)BDY(n1) : mkfnode(1,I_FORMULA,ONE);
    h2 = n2 ? (FNODE)BDY(n2) : mkfnode(1,I_FORMULA,ONE);
    return nfnode_comp_lex(h1,h2);
  }
  if ( IS_BINARYPWR(f1) || IS_BINARYPWR(f2) ) {
    fnode_base_exp(f1,&b1,&e1);
    fnode_base_exp(f2,&b2,&e2);
    if ( r = nfnode_comp_lex(b1,b2) ) {
      if ( r > 0 )
        return nfnode_comp_lex(e1,mkfnode(1,I_FORMULA,NULLP));
      else if ( r < 0 )
        return nfnode_comp_lex(mkfnode(1,I_FORMULA,NULLP),e2);
    } else return nfnode_comp_lex(e1,e2);
  }
        
  /* now, IDs of f1 and f2 must be I_FORMULA, I_FUNC, I_IFUNC or I_PVAR */
  /* I_IFUNC > I_PVAR > I_FUNC=I_FUNC_QARG > I_FORMULA */
  switch ( f1->id ) {
    case I_FORMULA:
      switch ( f2->id ) {
        case I_FORMULA:
          return arf_comp(qt_current_ord?qt_current_ord:CO,FA0(f1),FA0(f2));
        case I_FUNC: case I_IFUNC: case I_PVAR:
          return -1;
        default:
          error("nfnode_comp_lex : undefined");
      }
      break;
    case I_FUNC: case I_FUNC_QARG:
      switch ( f2->id ) {
        case I_FORMULA:
          return 1;
        case I_PVAR: case I_IFUNC:
          return -1;
        case I_FUNC: case I_FUNC_QARG:
          nm1 = ((FUNC)FA0(f1))->name; nm2 = ((FUNC)FA0(f2))->name;
          r = strcmp(nm1,nm2);
          if ( r > 0 ) return 1;
          else if ( r < 0 ) return -1;
          else {
            /* compare args */
            n1 = FA0((FNODE)FA1(f1)); n2 = FA0((FNODE)FA1(f2));
            while ( n1 && n2 )
              if ( r = nfnode_comp_lex(BDY(n1),BDY(n2)) ) return r;
              else {
                n1 = NEXT(n1); n2 = NEXT(n2);
              }
            return n1?1:(n2?-1:0);
          }
          break;
        default:
          error("nfnode_comp_lex : undefined");
      }
    case I_PVAR:
      switch ( f2->id ) {
        case I_FORMULA: case I_FUNC: case I_FUNC_QARG:
          return 1;
        case I_IFUNC:
          return -1;
        case I_PVAR:
          i1 = (long)FA0(f1); i2 = (long)FA0(f2);
          if ( i1 > i2 ) return 1;
          else if ( i1 < i2 ) return -1;
          else return 0;
        default:
          error("nfnode_comp_lex : undefined");
      }
      break;
    case I_IFUNC:
      switch ( f2->id ) {
        case I_FORMULA: case I_FUNC: case I_FUNC_QARG: case I_PVAR:
          return 1;
        case I_IFUNC:
          i1 = (long)FA0((FNODE)FA0(f1));
          i2 = (long)FA0((FNODE)FA0(f2));
          if ( i1 > i2 ) return 1;
          else if ( i1 < i2 ) return -1;
          else {
            /* compare args */
            n1 = FA0((FNODE)FA1(f1)); n2 = FA0((FNODE)FA1(f2));
            while ( n1 && n2 )
              if ( r = nfnode_comp_lex(BDY(n1),BDY(n2)) ) return r;
              else {
                n1 = NEXT(n1); n2 = NEXT(n2);
              }
            return n1?1:(n2?-1:0);
          }
          break;

        default:
          error("nfnode_comp_lex : undefined");
      }
      break;
    default:
      error("nfnode_comp_lex : undefined");
  }
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
      *rp = mknode(1,mknode(2,(long)FA0(pat),f));
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
      a = rewrite_fnode(FA2(pat),base,0);
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
        m = mknode(1,mknode(2,FA0((FNODE)FA0(pat)),fh),NULLP);
      }
      /* FA1(f) and FA1(pat) are I_LIST */
      fa = (NODE)FA0((FNODE)FA1(f));
      pa = (NODE)FA0((FNODE)FA1(pat));
      while ( fa && pa ) {
        a = rewrite_fnode(BDY(pa),m,0);
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
    return fnode_node_to_nary(addfs,NEXT(a));
  else {
    for ( r0 = 0, k = 0, t = a; k < i; k++, t = NEXT(t) ) {
      NEXTNODE(r0,r);
      BDY(r) = BDY(t);
    }
    NEXT(r) = NEXT(t);
    return fnode_node_to_nary(addfs,r0);
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
    return fnode_node_to_nary(mulfs,r0);
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
    return fnode_node_to_nary(mulfs,t);
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
          prr = rewrite_fnode(pr,m,0);
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
        pr = rewrite_fnode(BDY(t),m,0);
        if ( !nfnode_match(BDY(s),pr,&m1) ) return 0;
        m = append_node(m1,m);
      }
      if ( !NEXT(s) )
        fr = (FNODE)BDY(s);
      else
        fr = mkfnode(2,I_NARYOP,FA0(f),s);
      pr = rewrite_fnode(BDY(t),m,0);
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
          pleft1 = rewrite_fnode(pleft,m,0);
          if ( nfnode_match(fleft,pleft1,&m1) ) {
            m = append_node(m1,m);
            fright = fnode_right_narymul(f,fi);
            pright1 = rewrite_fnode(pright,m,0);
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
      ind = (long)FA0(pat);
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
