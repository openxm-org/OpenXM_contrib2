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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/ctrl.c,v 1.11 2000/12/13 10:54:09 noro Exp $ 
*/
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
#if defined(INTERVAL)
extern int zerorewrite;
#endif
extern int double_output;

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
	{"double_output",&double_output},
	{"fortran_output",&fortran_output},
#if defined(INTERVAL)
	{"zerorewrite",&zerorewrite},
#endif
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
