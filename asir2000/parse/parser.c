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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/parser.c,v 1.7 2004/02/27 09:13:04 noro Exp $ 
*/
#include <ctype.h>
#include "ca.h"
#include "parse.h"

extern int main_parser, allow_create_var;
extern char *parse_strp;
extern SNODE parse_snode;
extern FUNC parse_targetf;
extern int outputstyle;

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
	allow_create_var = 0;
	if ( yyparse() || !parse_snode || parse_snode->id != S_SINGLE ) {
		*exprp = 0; return 0;
	} else {
		*exprp = (FNODE)FA0(parse_snode); return 1;
	}
}

/* allows to create a new variable */

int exprparse_create_var(f,str,exprp)
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
	allow_create_var = 1;
	if ( yyparse() || !parse_snode || parse_snode->id != S_SINGLE ) {
		*exprp = 0; return 0;
	} else {
		*exprp = (FNODE)FA0(parse_snode); return 1;
	}
}

extern FUNC user_print_function;

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
				if ( user_print_function ) {
					bevalf(user_print_function,mknode(1,LastVal));
					fflush(asir_out);
				} else
					printexpr(CO,LastVal);
				if ( outputstyle == 1 ) {
					putc(';',asir_out);
				}
				putc('\n',asir_out);
				fflush(asir_out);
			}
			if ( prtime ) {
				printtime(&egt0,&egt1,r1-r0);
			}
			prompt();
		}
	}
}
