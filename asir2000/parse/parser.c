/* $OpenXM: OpenXM/src/asir99/parse/parser.c,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
#include <ctype.h>
#include "ca.h"
#include "parse.h"

extern int main_parser;
extern char *parse_strp;
extern SNODE parse_snode;
extern FUNC parse_targetf;

int mainparse(snodep)
SNODE *snodep;
{
	int ret;

	main_parser = 1;
	ret = yyparse();
	*snodep = parse_snode;
	return ret;
}

int exprparse(f,str,exprp)
FUNC f;
char *str;
FNODE *exprp;
{
	char buf0[BUFSIZ];
	char *buf;
	int i,n;
	char c;


	n = strlen(str);
	if ( n >= BUFSIZ )
		buf = (char *)ALLOCA(n+1);
	else
		buf = buf0;
	for ( i = 0; ; i++, str++ ) {
		c = *str;
		if ( !c || c == '\n' ) {
			buf[i] = ';'; buf[i+1] = 0; break;
		} else
			buf[i] = c;
	}
	parse_strp = buf;
	parse_targetf = f;
	main_parser = 0;
	if ( yyparse() || !parse_snode || parse_snode->id != S_SINGLE ) {
		*exprp = 0; return 0;
	} else {
		*exprp = (FNODE)FA0(parse_snode); return 1;
	}
}

void read_eval_loop() {
	struct oEGT egt0,egt1;
	extern int prtime,prresult,ox_do_copy;
	SNODE snode;
	double r0,r1;
	double get_rtime();
	extern Obj LastVal;

	LastVal = 0;
	while ( 1 ) {
		mainparse(&snode);
		nextbp = 0;
		get_eg(&egt0);
		r0 = get_rtime();
		LastVal = evalstat(snode); 
		storeans(LastVal);
		get_eg(&egt1);
		r1 = get_rtime();
		if ( !ox_do_copy ) {
			if ( prresult ) {
				printexpr(CO,LastVal); putc('\n',asir_out);
				fflush(asir_out);
			}
			if ( prtime ) {
				printtime(&egt0,&egt1,r1-r0);
			}
			prompt();
		}
	}
}
