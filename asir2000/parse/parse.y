/* $OpenXM: OpenXM/src/asir99/parse/parse.y,v 1.2 1999/11/18 05:42:03 noro Exp $ */
%{
#define malloc(x) GC_malloc(x)
#define realloc(x,y) GC_realloc(x,y)
#define free(x) GC_free(x)

#if defined(THINK_C)
#define alloca(x) GC_malloc(x)
#endif
#if defined(TOWNS)
#include <alloca.h>
#endif
#include <ctype.h>
#include "ca.h"
#if !defined(THINK_C)
#include <sys/types.h>
#include <sys/stat.h>
#endif
#include "parse.h"

#define NOPR (prresult=0)

extern int gdef;
extern SNODE parse_snode;
extern int main_parser;

int prresult;

static int ind;
static FNODE t;
static NODE a,b;
static NODE2 a2;
static pointer val;
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

%token <i> STRUCT POINT NEWSTRUCT ANS FDEF PFDEF GLOBAL CMP OR AND CAR CDR
%token <i> DO WHILE FOR IF ELSE BREAK RETURN CONTINUE PARIF MAP TIMER GF2NGEN GFPNGEN GETOPT
%token <i> FOP_AND FOP_OR FOP_IMPL FOP_REPL FOP_EQUIV FOP_NOT LOP
%token <p> FORMULA UCASE LCASE STR SELF BOPASS
%token <p> '+' '-' '*' '/' '^' '%'

%type <p> desc rawstr
%type <f> expr pexpr opt
%type <s> stat complex
%type <n> stats node _node pvars fields members optlist

%right '=' BOPASS
%right '?' ':'
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
			{ parse_snode = $1; YYACCEPT; }
	  	;
stat 	: tail
			{ $$ = 0; }
		| GLOBAL { gdef=1; } pvars { gdef=0; } tail
			{ $$ = 0; }
		| STRUCT rawstr '{' fields '}' tail
			{ structdef($2,$4); $$ = 0; }
		| STRUCT rawstr '{' fields '}' pvars tail
			{ structdef($2,$4); setstruct($2,$6); $$ = 0; }
		| STRUCT rawstr pvars tail
			{ setstruct($2,$3); $$ = 0; }
		| expr tail
			{ $$ = mksnode(1,S_SINGLE,$1); }
		| complex
			{ $$ = $1; }
		| BREAK tail
			{ $$ = mksnode(0,S_BREAK); }
		| CONTINUE tail
			{ $$ = mksnode(0,S_CONTINUE); }
		| RETURN tail
			{ $$ = mksnode(1,S_RETURN,0); }
		| RETURN expr tail
			{ $$ = mksnode(1,S_RETURN,$2); }
		| IF '(' node ')' stat
			{ $$ = mksnode(4,S_IFELSE,$1,$3,$5,0); $5?$$->ln=$5->ln:0; NOPR; }
		| IF '(' node ')' stat ELSE stat
			{ $$ = mksnode(4,S_IFELSE,$1,$3,$5,$7); $7?$$->ln=$7->ln:0; NOPR; }
		| FOR '(' node ';' node ';' node ')' stat
			{ $$ = mksnode(5,S_FOR,$1,$3,$5?$5:ONENODE,$7,$9); $9?$$->ln=$9->ln:0; NOPR; }
		| WHILE '(' node ')' stat
			{ $$ = mksnode(5,S_FOR,$1,0,$3,0,$5); $5?$$->ln=$5->ln:0; NOPR; }
		| DO stat WHILE '(' node ')' tail
			{ $$ = mksnode(3,S_DO,$1,$2,$5); }
		| LCASE '(' node ')' ':' '=' expr tail
			{ $$ = mksnode(3,S_PFDEF,$1,$3,$7); NOPR; }
		| PFDEF LCASE '(' node ')'
			{ $$ = mksnode(3,S_PFDEF,$2,$4,0); NOPR; }
		| FDEF LCASE { mkpvs(); } '(' node ')' desc '{' stats '}'
			{
				mkuf($2,asir_infile->name,$5,
					mksnode(1,S_CPLX,$9),$1,asir_infile->ln,$7); 
				$$ = 0; NOPR; 
			}
	  	| error tail
			{ yyerrok; $$ = 0; }
		;
tail	: ';' 
			{ if ( main_parser ) prresult = 1; } 
		| '$' 
			{ if ( main_parser ) prresult = 0; } 
desc	:
			{ $$ = 0; }
		| STR
			{ $$ = $1; }
		;
complex : '{' stats '}'
			{ $$ = mksnode(1,S_CPLX,$2); }
		;
fields	:
			{ $$ = 0; }
		| fields members tail
			{ MKNODE(a,0,$2); appendtonode($1,(pointer)a,&$$); }
		| fields STRUCT rawstr members tail
			{ MKNODE(a,$3,$4); appendtonode($1,(pointer)a,&$$); }
		;
members	: rawstr
			{ MKNODE($$,$1,0); }
		| members ',' rawstr
			{ appendtonode($1,$3,&$$); }
		;
pvars	: UCASE
			{ val = (pointer)makepvar($1); MKNODE($$,val,0); }
		| pvars ',' UCASE
			{ appendtonode($1,(pointer)makepvar($3),&$$); }
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
	 	| LCASE
			{
				FUNC f;

				searchf(noargsysf,$1,&f);
				if ( f )
					 $$ = mkfnode(2,I_FUNC,f,0);
				else {
					searchc($1,&f);
					if ( f )
						$$ = mkfnode(2,I_FUNC,f,mkfnode(1,I_LIST,0));
					else {
						searchf(sysf,$1,&f); 
						if ( !f )
							searchf(ubinf,$1,&f);
						if ( !f )
							searchpf($1,&f);
						if ( !f )
							searchf(usrf,$1,&f);
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
				searchf(sysf,$1,(FUNC *)&val); 
				if ( !val )
					searchf(ubinf,$1,(FUNC *)&val);
				if ( !val )
					searchpf($1,(FUNC *)&val);
				if ( !val )
					searchf(usrf,$1,(FUNC *)&val);
				if ( !val )
					appenduf($1,(FUNC *)&val);
				$$ = mkfnode(2,I_FUNC,val,mkfnode(1,I_LIST,$3));
			}

		| LCASE '(' node '|' optlist ')' 
			{
				searchf(sysf,$1,(FUNC *)&val); 
				if ( !val )
					searchf(ubinf,$1,(FUNC *)&val);
				if ( !val )
					searchpf($1,(FUNC *)&val);
				if ( !val )
					searchf(usrf,$1,(FUNC *)&val);
				if ( !val )
					appenduf($1,(FUNC *)&val);
				$$ = mkfnode(3,I_FUNC_OPT,val,
					mkfnode(1,I_LIST,$3),mkfnode(1,I_LIST,$5));
			}
		| LCASE '{' node '}' '(' node ')' 
			{
				searchpf($1,(FUNC *)&val);
				$$ = mkfnode(2,I_PFDERIV,val,mkfnode(1,I_LIST,$6),mkfnode(1,I_LIST,$3));
			}
		| GETOPT '(' rawstr ')'
			{
				$$ = mkfnode(2,I_GETOPT,$3);
			}
		| MAP '(' LCASE ',' node ')' 
			{
				searchf(sysf,$3,(FUNC *)&val); 
				if ( !val )
					searchf(ubinf,$3,(FUNC *)&val);
				if ( !val )
					searchpf($3,(FUNC *)&val);
				if ( !val )
					searchf(usrf,$3,(FUNC *)&val);
				if ( !val )
					appenduf($3,(FUNC *)&val);
				$$ = mkfnode(2,I_MAP,val,mkfnode(1,I_LIST,$5));
			}
		| TIMER '(' expr ',' expr ',' expr ')'
			{
				$$ = mkfnode(3,I_TIMER,$3,$5,$7);
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
				$$ = mkfnode(2,I_IFUNC,$3,mkfnode(1,I_LIST,$6));
			}	
		| CAR '(' expr ')'
			{ $$ = mkfnode(1,I_CAR,$3); }
		| CDR '(' expr ')'
			{ $$ = mkfnode(1,I_CDR,$3); }
		| '(' expr ')'
			{ $$ = $2; }
		| UCASE
			{ 
				if ( main_parser )
					$$ = mkfnode(2,I_PVAR,makepvar($1),0);
				else {
					ind = searchpvar($1);
					if ( ind == -1 ) {
						fprintf(stderr,"%s : no such variable.\n",$1);
						YYABORT;
					} else
						$$ = mkfnode(2,I_PVAR,ind,0);
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
			{ memberofstruct($1,$3,&$$); }
		;
expr 	: pexpr
			{ $$ = $1; }
		| '(' STRUCT rawstr ')' pexpr
			{ $$ = mkfnode(3,I_CAST,structtoindex($3),$5,0); }
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
			{ $$ = mkfnode(3,I_BOP,$1,0,$2); }
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
	 	| expr OR expr
			{ $$ = mkfnode(2,I_OR,$1,$3); }
	 	| expr AND expr
			{ $$ = mkfnode(2,I_AND,$1,$3); }
		| FOP_NOT expr
			{ $$ = mkfnode(3,I_LOP,$1,$2,0); }
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
		| NEWSTRUCT '(' rawstr ')'
			{ $$ = mkfnode(1,I_NEWCOMP,(int)structtoindex($3)); }
	 	;
%%
