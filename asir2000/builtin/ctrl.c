/* $OpenXM: OpenXM_contrib2/asir2000/builtin/ctrl.c,v 1.5 2000/02/07 03:21:42 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Pctrl();

struct ftab ctrl_tab[] = {
	{"ctrl",Pctrl,-2},
	{0,0,0},
};

extern int prtime,nez,echoback,bigfloat;
extern int GC_free_space_numerator,GC_free_space_divisor,debug_up,no_prompt;
extern int GC_max_heap_size,Verbose,hideargs,hex_output,do_server_in_X11;
extern int do_message;
extern int ox_batch,ox_check,ox_exchange_mathcap;
extern int f4_nocheck;
extern int fortran_output;

static struct {
	char *key;
	int *val;
} ctrls[] = {
	{"cputime",&prtime},
	{"nez",&nez},
	{"echo",&echoback},
#if DO_PLOT
	{"bigfloat",&bigfloat},
#endif
	{"verbose",&Verbose},
	{"hideargs",&hideargs},
	{"hex",&hex_output},
	{"debug_window",&do_server_in_X11},
	{"message",&do_message},
	{"debug_up",&debug_up},
	{"no_prompt",&no_prompt},
	{"ox_batch",&ox_batch},
	{"ox_check",&ox_check},
	{"ox_exchange_mathcap",&ox_exchange_mathcap},
	{"f4_nocheck",&f4_nocheck},
	{"fortran_output",&fortran_output},
	{0,0},
};

void Pctrl(arg,rp)
NODE arg;
Q *rp;
{
	int t,i;
	N num,den;
	Q c;
	char *key;
	char buf[BUFSIZ];

	if ( !arg ) {
		*rp = 0;
		return;
	}
	key = BDY((STRING)ARG0(arg));
	if ( !strcmp(key,"adj") ) {
		/* special treatment is necessary for "adj" */
		if ( argc(arg) == 1 ) {
			UTON(GC_free_space_divisor,num);
			UTON(GC_free_space_numerator,den);
		} else {
			c = (Q)ARG1(arg);
			if ( !c )
				error("ctrl : adj : invalid argument");
			num = NM(c);
			den = !DN(c)?ONEN:DN(c);
			GC_free_space_divisor = BD(num)[0];
			GC_free_space_numerator = BD(den)[0];
		}
		NDTOQ(num,den,1,*rp);
		return;
	}
	for ( i = 0; ctrls[i].key; i++ )
		if ( !strcmp(key,ctrls[i].key) )
			break;
	if ( ctrls[i].key ) {
		if ( argc(arg) == 1 )
			t = *ctrls[i].val;
		else
			*ctrls[i].val = t = QTOS((Q)ARG1(arg));
		STOQ(t,*rp);
	} else {
		sprintf(buf,"ctrl : %s : no such key",key);
		error(buf);
	}
}
