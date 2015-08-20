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
 * $OpenXM: OpenXM_contrib2/asir2000/io/pexpr.c,v 1.40 2015/08/04 06:20:45 noro Exp $
*/
#include "ca.h"
#include "al.h"
#include "parse.h"
#include "comp.h"
#include "base.h"

#ifndef FPRINT
#define FPRINT
#endif

extern int outputstyle;

FILE *asir_out;
#define OUT asir_out
char DFORMAT[BUFSIZ];
int hex_output;
int fortran_output;
int double_output;
int real_digit;
int real_binary;
int print_quote;
int hideargs;
extern FUNC user_print_function;
#if 0
int printmode = PRINTF_E;
#else
int printmode = PRINTF_G;
#endif

#define DATA_BEGIN 2
#define DATA_END 5

#define TAIL
#define PUTS(s) fputs(s,OUT)
#define PRINTF fprintf
#define PRINTN printn
#define PRINTBF printbf
#define PRINTCPLX printcplx
#define PRINTLM printlm
#define PRINTUP2 printup2
#define PRINTV printv
#define PRINTEXPR printexpr
#define PRINTNUM printnum
#define PRINTP asir_printp
#define PRINTR printr
#define PRINTLIST printlist
#define PRINTVECT printvect
#define PRINTMAT printmat
#define PRINTSTR printstr
#define PRINTCOMP printcomp
#define PRINTDP printdp
#define PRINTUI printui
#define PRINTGF2MAT printgf2mat
#define PRINTGFMMAT printgfmmat
#define PRINTBYTEARRAY printbytearray
#define PRINTQUOTE printquote
#define PRINTQUOTEARG printquotearg
#define PRINTNBP printnbp
#define PRINTERR printerr
#define PRINTLF printlf
#define PRINTLOP printlop
#define PRINTFOP printfop
#define PRINTEOP printeop
#define PRINTQOP printqop
#define PRINTUP printup
#define PRINTUM printum
#define PRINTSF printsf
#define PRINTSYMBOL printsymbol
#define PRINTRANGE printrange
#define PRINTTB printtb
#define PRINTDPV printdpv
#define PRINTFNODE printfnode
#define PRINTFNODENODE printfnodenode
#define PRINTFARGS printfargs

#include "pexpr_body.c"

/* spetial functions for file output */

void output_init() {
	OUT = stdout;
	sprintf(DFORMAT,"%%0%dd",DLENGTH);
}

int mmono(p)
P p;
{
	if ( NUM(p) )
#if defined(INTERVAL)
		if ( NID(p) != N_IP && NID(p) != N_IntervalDouble && NID(p) != N_IntervalQuad && NID(p) != N_IntervalBigFloat
			&& compnum(CO,(Num)p,0) < 0 ) 
#else
		if ( compnum(CO,(Num)p,0) < 0 ) 
#endif
			return ( 1 );
		else
			return ( 0 );
	else if ( NEXT(DC(p)) )
		return ( 0 );
	else
		return (mmono(COEF(DC(p))));
}

void printbf(BF a)
{
	int dprec;
	char fbuf[BUFSIZ];
	char *s;
	dprec = (a->body->_mpfr_prec)*0.30103;
  if ( !dprec ) dprec = 1;
	sprintf(fbuf,"%%.%dR%c",dprec,double_output?'f':'g');
	mpfr_asprintf(&s,fbuf,a->body);
	TAIL PUTS(s);
	mpfr_free_str(s);
}

