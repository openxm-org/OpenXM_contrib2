/* $OpenXM: OpenXM/src/asir99/builtin/print.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Pprint();

struct ftab print_tab[] = {
	{"print",Pprint,-2},
	{0,0,0},
};

void Pprint(arg,rp)
NODE arg;
pointer *rp;
{
	printexpr(CO,ARG0(arg)); 
	if ( argc(arg) == 2 )
		switch ( QTOS((Q)ARG1(arg)) ) {
			case 0:
				break;
			case 2:
				fflush(asir_out); break;
				break;
			case 1: default:
				putc('\n',asir_out); break;
		}
	else
		putc('\n',asir_out);
	*rp = 0;
}
