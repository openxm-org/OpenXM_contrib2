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
 * $OpenXM: OpenXM_contrib2/asir2018/io/spexpr.c,v 1.4 2019/12/24 10:26:39 kondoh Exp $
*/
#include "ca.h"
#include "al.h"
#include "parse.h"
#include "comp.h"
#include "base.h"

#ifndef SPRINT
#define SPRINT
#endif

extern int outputstyle;

static char *buf;
#define OUT buf
extern char DFORMAT[BUFSIZ];
extern int hex_output;
extern int fortran_output;
extern int double_output;
extern int real_digit;
extern int real_binary;
extern int print_quote;
extern int printmode;
extern int hideargs;

#define TAIL while ( *OUT ) OUT++;
#define PUTS(s) strcat(OUT,s)
#define PRINTF sprintf
#define PRINTZ sprintz
#define PRINTMPZ sprintmpz
#define PRINTBF sprintbf
#define PRINTCPLX sprintcplx
#define PRINTLM sprintlm
#define PRINTUP2 sprintup2
#define PRINTV sprintv
#define PRINTEXPR sprintexpr
#define PRINTNUM sprintnum
#define PRINTP asir_sprintp
#define PRINTR sprintr
#define PRINTLIST sprintlist
#define PRINTVECT sprintvect
#define PRINTMAT sprintmat
#define PRINTIMAT sprintimat
#define PRINTSTR sprintstr
#define PRINTCOMP sprintcomp
#define PRINTDP sprintdp
#define PRINTDPM sprintdpm
#define PRINTUI sprintui
#define PRINTGF2MAT sprintgf2mat
#define PRINTGFMMAT sprintgfmmat
#define PRINTBYTEARRAY sprintbytearray
#define PRINTQUOTE sprintquote
#define PRINTQUOTEARG sprintquotearg
#define PRINTNBP sprintnbp
#define PRINTERR sprinterr
#define PRINTLF sprintlf
#define PRINTLOP sprintlop
#define PRINTFOP sprintfop
#define PRINTEOP sprinteop
#define PRINTQOP sprintqop
#define PRINTUP sprintup
#define PRINTUM sprintum
#define PRINTSF sprintsf
#define PRINTSYMBOL sprintsymbol
#define PRINTRANGE sprintrange
#define PRINTTB sprinttb
#define PRINTDPV sprintdpv
#define PRINTFNODE sprintfnode
#define PRINTFNODENODE sprintfnodenode
#define PRINTFARGS sprintfargs

#if defined(INTERVAL)
#define PRINTBF4ITV sprintbf4itv
#endif

#include "pexpr_body.c"

/* special functions for string output */

void soutput_init(char *s)
{
  s[0] = 0; buf = s;
}

void sprintbf(BF a)
{
  int dprec;
  char fbuf[BUFSIZ];
  char *s;
  dprec = a->body->_mpfr_prec*0.30103;
  sprintf(fbuf,"%%.%dR%c",dprec,(double_output==1)?'f':(double_output==2)?'e':'g');
  mpfr_asprintf(&s,fbuf,a->body);
  TAIL PUTS(s);
  mpfr_free_str(s);
}

#if defined(INTERVAL)
void sprintbf4itv(BF a)
{
  int dprec;
  char fbuf[BUFSIZ];
  char *s;

  if ( !a ) {
    TAIL PUTS("0");
  } else {
    dprec = a->body->_mpfr_prec*0.30103;
    dprec += 1;
    sprintf(fbuf,"%%.%dR%c",dprec,(double_output==1)?'f':(double_output==2)?'e':'g');
    mpfr_asprintf(&s,fbuf,a->body);
    TAIL PUTS(s);
    mpfr_free_str(s);
  }
}
#endif

void sprintmpz(mpz_t z)
{
  char *s;
  if ( hex_output == 1 ) {
    TAIL PUTS("0x");
    s = mpz_get_str(0,16,z);
  } else if ( hex_output == 2 ) {
    TAIL PUTS("0b");
    s = mpz_get_str(0,2,z);
  } else {
    s = mpz_get_str(0,10,z);
  }
  TAIL PUTS(s);
}

void sprintz(Z n)
{
  sprintmpz(BDY(n));
}

