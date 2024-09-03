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
 * $OpenXM: OpenXM_contrib2/asir2018/parse/parse.y,v 1.2 2019/08/21 00:37:47 noro Exp $
*/
%{
#define malloc(x) Risa_GC_malloc(x)
#define realloc(x,y) Risa_GC_realloc(x,y)
#define free(x) Risa_GC_free(x)

#if defined(TOWNS)
#include <alloca.h>
#endif
#include <ctype.h>
#include "ca.h"
#include <sys/types.h>
#include <sys/stat.h>
#include "parse.h"

#define NOPR (prresult=0)

extern int gdef,mgdef,ldef;
extern SNODE parse_snode;
extern int main_parser, allow_create_var;

int prresult,saveresult;

static int ind;
static FNODE t;
static NODE a,b;
static NODE2 a2;
static pointer val;
static QUOTE quote;
extern jmp_buf env;
%}

%start start

%union {
	FNODE f;
	SNODE s;
	NODE n;
	NODE2 n2;
	int i;
	pointer p;
}

%token <i> STRUCT POINT NEWSTRUCT ANS FDEF PFDEF MODDEF MODEND
%token <i> GLOBAL MGLOBAL LOCAL LOCALF CMP OR AND CAR CDR QUOTED COLONCOLON SEMISEMI
%token <i> DO WHILE FOR IF ELSE BREAK RETURN CONTINUE PARIF MAP RECMAP TIMER GF2NGEN GFPNGEN GFSNGEN GETOPT
%token <i> FOP_AND FOP_OR FOP_IMPL FOP_REPL FOP_EQUIV FOP_NOT LOP
%token <p> FORMULA UCASE LCASE STR SELF BOPASS
%token <p> '+' '-' '*' '/' '^' '%'

%type <p> desc rawstr
%type <f> expr pexpr opt
%type <s> stat complex
%type <n> stats node _node pvars vars members optlist

%right '=' BOPASS
%right '?' ':'
%left '`'
%right FOP_NOT
%left FOP_EQUIV
%left FOP_REPL
%left FOP_IMPL
%left FOP_OR
%left FOP_AND
%left LOP
%left LCASE
%left OR
%left AND
%left '&'
%left CMP 
%left '+' '-' 
%left PLUS
%left MINUS
%left '*' '/' '%' 
%right '^'
%right '!' 
%right SELF
%left POINT

%%

start 	: stat
			{ 
				parse_snode = $1; 
				if ( yychar >= 0 ) 
					fprintf(stderr,
						"Warning: a token was wasted after an 'if' statement without 'else'.\n");
				YYACCEPT; 
			}
	  	;
stat 	: tail
			{ $$ = 0; }
		| GLOBAL { gdef=1; } pvars { gdef=0; } tail
			{ $$ = 0; NOPR; }
		| MGLOBAL { mgdef=1; } pvars { mgdef=0; } tail
			{ $$ = 0; NOPR; }
		| LOCAL { ldef=1; } pvars { ldef=0; } tail
			{ $$ = 0; NOPR; }
		| LOCALF vars tail
			{ appenduflist($2); $$ = 0; NOPR; }
		| STRUCT rawstr '{' members '}' tail
			{ structdef($2,$4); $$ = 0; NOPR; }
		| expr tail
			{ $$ = mksnode(1,S_SINGLE,$1); }
		| complex
			{ $$ = $1; }
		| BREAK tail
			{ $$ = mksnode(0,S_BREAK); }
		| CONTINUE tail
			{ $$ = mksnode(0,S_CONTINUE); }
		| RETURN tail
			{ $$ = mksnode(1,S_RETURN,NULLP); }
		| RETURN expr tail
			{ $$ = mksnode(1,S_RETURN,$2); }
		| IF '(' node ')' stat
			{ $$ = mksnode(4,S_IFELSE,$1,$3,$5,NULLP); $5?$$->ln=$5->ln:0; NOPR; }
		| IF '(' node ')' stat ELSE stat
			{ $$ = mksnode(4,S_IFELSE,$1,$3,$5,$7); $7?$$->ln=$7->ln:0; NOPR; }
		| FOR '(' node ';' node ';' node ')' stat
			{ $$ = mksnode(5,S_FOR,$1,$3,$5?$5:ONENODE,$7,$9); $9?$$->ln=$9->ln:0; NOPR; }
		| WHILE '(' node ')' stat
			{ $$ = mksnode(5,S_FOR,$1,NULLP,$3,NULLP,$5); $5?$$->ln=$5->ln:0; NOPR; }
		| DO stat WHILE '(' node ')' tail
			{ $$ = mksnode(3,S_DO,$1,$2,$5); NOPR; }
		| LCASE '(' node ')' ':' '=' expr tail
			{ $$ = mksnode(3,S_PFDEF,$1,$3,$7); NOPR; }
		| PFDEF LCASE '(' node ')' tail
			{ $$ = mksnode(3,S_PFDEF,$2,$4,NULLP); NOPR; }
		| FDEF LCASE { mkpvs($2); } '(' { ldef = 1; } node { ldef = -1; } ')' desc '{' stats '}'
			{
				mkuf($2,asir_infile->name,$6,
					mksnode(1,S_CPLX,$11),$1,asir_infile->ln,$9,CUR_MODULE); 
				$$ = 0; NOPR;
			}
		| MODDEF LCASE tail
			{ 
				CUR_MODULE = mkmodule($2);
				MPVS = CUR_MODULE->pvs; 
				$$ = mksnode(1,S_MODULE,CUR_MODULE); NOPR;
			}
		| MODEND tail
			{ CUR_MODULE = 0; MPVS = 0; $$ = mksnode(1,S_MODULE,NULLP); NOPR; }
	  	| error tail
			{ yyerrok; $$ = 0; }
		;
tail	: ';' 
			{ if ( main_parser ) prresult = 1; } 
		| '$' 
			{ if ( main_parser ) prresult = 0; } 
		| SEMISEMI
			{ if ( main_parser ) prresult = 0; } 
;
desc	:
			{ $$ = 0; }
		| STR
			{ $$ = $1; }
		;
complex : '{' stats '}'
			{ $$ = mksnode(1,S_CPLX,$2); }
		;
members	: rawstr
			{ MKNODE($$,$1,0); }
		| members ',' rawstr
			{ appendtonode($1,$3,&$$); }
		;
vars	: LCASE
			{ MKNODE($$,$1,0); }
		| vars ',' LCASE
			{ appendtonode($1,$3,&$$); }
		;
pvars	: UCASE
			{ val = (pointer)((long)makepvar($1)); MKNODE($$,val,0); }
		| pvars ',' UCASE
			{ appendtonode($1,(pointer)((long)makepvar($3)),&$$); }
		;
stats	:
			{ $$ = 0; }
		| stats stat
			{ appendtonode($1,(pointer)$2,&$$); }
		;
node 	: 
			{ $$ = 0; }
		| _node
			{ $$ = $1; }
		;
_node	: expr
			{ MKNODE($$,$1,0); }
		| _node ',' expr
			{ appendtonode($1,(pointer)$3,&$$); }
		;
optlist	: opt
			{ MKNODE($$,$1,0); }
		| optlist ',' opt
			{ appendtonode($1,(pointer)$3,&$$); }
		;
rawstr	: UCASE
			{ $$ = $1; }
		| LCASE
			{ $$ = $1; }
		;
opt		: rawstr '=' expr
			{ $$ = mkfnode(2,I_OPT,$1,$3); }
		;
pexpr	: STR
			{ $$ = mkfnode(1,I_STR,$1); }
	 	| FORMULA
			{ $$ = mkfnode(1,I_FORMULA,$1); }
	 	| ANS
			{ $$ = mkfnode(1,I_ANS,$1); }
		| GF2NGEN
			{ $$ = mkfnode(0,I_GF2NGEN); }
		| GFPNGEN
			{ $$ = mkfnode(0,I_GFPNGEN); }
		| GFSNGEN
			{ $$ = mkfnode(0,I_GFSNGEN); }
	 	| LCASE
			{
				FUNC f;

				searchf(noargsysf,$1,&f);
				if ( f )
					 $$ = mkfnode(2,I_FUNC,f,NULLP);
				else {
					searchc($1,&f);
					if ( f )
						$$ = mkfnode(2,I_FUNC,f,mkfnode(1,I_LIST,NULLP));
					else {
						gen_searchf_searchonly($1,(FUNC *)&f,1);
						if ( f )
							makesrvar(f,(P *)&val);
						else
							makevar($1,(P *)&val);
						$$ = mkfnode(1,I_FORMULA,val);
					}
				}
			}
		| LCASE '(' node ')' 
			{
				gen_searchf($1,(FUNC *)&val);
				print_crossref(val);
				$$ = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,$3));
			}
		| LCASE '(' node '|' optlist ')' 
			{
				gen_searchf($1,(FUNC *)&val);
				print_crossref(val);
				$$ = mkfnode(3,I_FUNC_OPT,val,
					mkfnode(1,I_LIST,$3),mkfnode(1,I_LIST,$5));
			}
		| MAP '(' LCASE ',' node ')' 
			{
				gen_searchf($3,(FUNC *)&val);
				print_crossref(val);
				$$ = mkfnode(2,I_MAP,val,mkfnode(1,I_LIST,$5));
			}
		| RECMAP '(' LCASE ',' node ')' 
			{
				gen_searchf($3,(FUNC *)&val);
				print_crossref(val);
				$$ = mkfnode(2,I_RECMAP,val,mkfnode(1,I_LIST,$5));
			}
		| LCASE '{' node '}' '(' node ')' 
			{
				searchpf($1,(FUNC *)&val);
				if ( val == 0 ) {
					fprintf(stderr,"%s : no such function.\n",(char *)$1);
                    YYABORT;
                }
				$$ = mkfnode(3,I_PFDERIV,val,mkfnode(1,I_LIST,$6),mkfnode(1,I_LIST,$3));
			}
		| GETOPT '(' rawstr ')'
			{
				$$ = mkfnode(2,I_GETOPT,$3);
			}
		| GETOPT '(' ')'
			{
				$$ = mkfnode(2,I_GETOPT,NULLP);
			}
		| TIMER '(' expr ',' expr ',' expr ')'
			{
				$$ = mkfnode(3,I_TIMER,$3,$5,$7);
			}
		| PARIF '(' LCASE ')'
			{
				searchf(parif,$3,(FUNC *)&val);
				if ( !val )
					mkparif($3,(FUNC *)&val);
				$$ = mkfnode(2,I_FUNC,val,NULLP);
			}
		| PARIF '(' LCASE ',' node ')'
			{
				searchf(parif,$3,(FUNC *)&val);
				if ( !val )
					mkparif($3,(FUNC *)&val);
				$$ = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,$5));
			}
		| '('  '*' expr ')' '(' node ')'
			{
				$$ = mkfnode(2,I_IFUNC,$3,mkfnode(1,I_LIST,$6),NULLP);
			}	
		| '('  '*' expr ')' '(' node '|' optlist ')'
			{
				$$ = mkfnode(3,I_IFUNC,$3,mkfnode(1,I_LIST,$6),
					mkfnode(1,I_LIST,$8));
			}	
		| UCASE '(' node ')'
			{
				if ( main_parser || allow_create_var )
					t = mkfnode(2,I_PVAR,makepvar($1),NULLP);
				else {
					ind = searchpvar($1);
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",(char *)$1);
						YYABORT;
					} else
						t = mkfnode(2,I_PVAR,ind,NULLP);
				}
				$$ = mkfnode(2,I_IFUNC,t,mkfnode(1,I_LIST,$3));
			}	
		| CAR '(' expr ')'
			{ $$ = mkfnode(1,I_CAR,$3); }
		| CDR '(' expr ')'
			{ $$ = mkfnode(1,I_CDR,$3); }
		| '(' expr ')'
			{ $$ = mkfnode(1,I_PAREN,$2); }
		| UCASE
			{ 
				if ( main_parser || allow_create_var )
					$$ = mkfnode(2,I_PVAR,makepvar($1),NULLP);
				else {
					ind = searchpvar($1);
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",(char *)$1);
						YYABORT;
					} else
						$$ = mkfnode(2,I_PVAR,ind,NULLP);
				}
			}
		| pexpr '[' expr ']'
			{
				if ( $1->id == I_PVAR || $1->id == I_INDEX ) {
					appendtonode((NODE)$1->arg[1],(pointer)$3,&a);
					$1->arg[1] = (pointer)a; $$ = $1;
				} else {
					MKNODE(a,$3,0);
					$$ = mkfnode(2,I_INDEX,(pointer)$1,a);
				}
			}
		| pexpr POINT rawstr
			{ $$ = mkfnode(2,I_POINT,$1,$3); }
		;
expr 	: pexpr
			{ $$ = $1; }
		| '(' STRUCT rawstr ')' pexpr
			{ $$ = mkfnode(3,I_CAST,structtoindex($3),$5,NULLP); }
		| expr '=' expr
			{ $$ = mkfnode(2,I_ASSPVAR,$1,$3); }
		| expr BOPASS expr
			{ $$ = mkfnode(2,I_ASSPVAR,$1,mkfnode(3,I_BOP,$2,$1,$3)); }
	 	| expr SELF
			{ $$ = mkfnode(2,I_POSTSELF,$2,$1); }
	 	| SELF expr
			{ $$ = mkfnode(2,I_PRESELF,$1,$2); }
		| '[' node ']'
			{ $$ = mkfnode(1,I_LIST,$2); }
	 	| '+' expr 	%prec PLUS
			{ $$ = $2; }
	 	| '-' expr 	%prec MINUS
			{ $$ = mkfnode(1,I_MINUS,$2); }
	 	| expr '+' expr
			{ $$ = mkfnode(3,I_BOP,$2,$1,$3); }
	 	| expr '-' expr
			{ $$ = mkfnode(3,I_BOP,$2,$1,$3); }
	 	| expr '*' expr
			{ $$ = mkfnode(3,I_BOP,$2,$1,$3); }
	 	| expr '/' expr
			{ $$ = mkfnode(3,I_BOP,$2,$1,$3); }
	 	| expr '%' expr
			{ $$ = mkfnode(3,I_BOP,$2,$1,$3); }
	 	| expr '^' expr
			{ $$ = mkfnode(3,I_BOP,$2,$1,$3); }
	 	| expr CMP expr
			{ $$ = mkfnode(3,I_COP,$2,$1,$3); }
	 	| '!' expr 	
			{ $$ = mkfnode(1,I_NOT,$2); }
	 	| expr '!'
			{ 
				gen_searchf("factorial",(FUNC *)&val);
				print_crossref(val);
				MKNODE(a,$1,0);
				$$ = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,a));
			}
	 	| expr OR expr
			{ $$ = mkfnode(2,I_OR,$1,$3); }
	 	| expr AND expr
			{ $$ = mkfnode(2,I_AND,$1,$3); }
		| FOP_NOT expr
			{ $$ = mkfnode(3,I_LOP,$1,$2,NULLP); }
		| expr FOP_AND expr
			{ $$ = mkfnode(3,I_LOP,$2,$1,$3); }
		| expr FOP_OR expr
			{ $$ = mkfnode(3,I_LOP,$2,$1,$3); }
		| expr FOP_IMPL expr
			{ $$ = mkfnode(3,I_LOP,$2,$1,$3); }
		| expr FOP_REPL expr
			{ $$ = mkfnode(3,I_LOP,$2,$1,$3); }
		| expr FOP_EQUIV expr
			{ $$ = mkfnode(3,I_LOP,$2,$1,$3); }
		| expr LOP expr
			{ $$ = mkfnode(3,I_LOP,$2,$1,$3); }
	 	| expr '?' expr ':' expr
			{ $$ = mkfnode(3,I_CE,$1,$3,$5); }
		| '<' node '>'
			{ $$ = mkfnode(1,I_EV,$2); }
		| '<' node ':' expr '>'
			{ $$ = mkfnode(2,I_EVM,$2,$4); }
		| NEWSTRUCT '(' rawstr ')'
			{ $$ = mkfnode(1,I_NEWCOMP,(int)structtoindex($3)); }
		| QUOTED '(' expr ')'
			{ MKQUOTE(quote,$3); $$ = mkfnode(1,I_FORMULA,(pointer)quote); }
		| '[' node '|' expr ']'
			{ $$ = mkfnode(2,I_CONS,$2,$4); }
		| '`' expr
			{ MKQUOTE(quote,$2); $$ = mkfnode(1,I_FORMULA,(pointer)quote); }
	 	;
%%
