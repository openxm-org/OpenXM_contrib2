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
 * e-mail at risa-admin@flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/strobj.c,v 1.5 2000/03/02 07:16:09 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ctype.h"
#if PARI
#include "genpari.h"
extern jmp_buf environnement;
#endif
#include <string.h>

extern char *parse_strp;

void Prtostr(), Pstrtov(), Peval_str();
void Pstrtoascii(), Pasciitostr();
void Pstr_len(), Pstr_chr(), Psub_str();

struct ftab str_tab[] = {
	{"rtostr",Prtostr,1},
	{"strtov",Pstrtov,1},
	{"eval_str",Peval_str,1},
	{"strtoascii",Pstrtoascii,1},
	{"asciitostr",Pasciitostr,1},
	{"str_len",Pstr_len,1},
	{"str_chr",Pstr_chr,3},
	{"sub_str",Psub_str,3},
	{0,0,0},
};

void Pstr_len(arg,rp)
NODE arg;
Q *rp;
{
	STRING str;
	int r;

	str = (STRING)ARG0(arg);
	asir_assert(str,O_STR,"str_chr");
	r = strlen(BDY(str));
	STOQ(r,*rp);
}

void Pstr_chr(arg,rp)
NODE arg;
Q *rp;
{
	STRING str,terminator;
	Q start;
	char *p,*ind;
	int chr,spos,r;

	str = (STRING)ARG0(arg);
	start = (Q)ARG1(arg);
	terminator = (STRING)ARG2(arg);
	asir_assert(str,O_STR,"str_chr");
	asir_assert(start,O_N,"str_chr");
	asir_assert(terminator,O_STR,"str_chr");
	p = BDY(str);
	spos = QTOS(start);
	chr = BDY(terminator)[0];
	if ( spos > strlen(p) )
		r = -1;
	else {
		ind = strchr(p+spos,chr);
		if ( ind )
			r = ind-p;
		else
			r = -1;
	}
	STOQ(r,*rp);
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
	spos = QTOS(head);
	epos = QTOS(tail);
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
	Q q;

	str = (STRING)ARG0(arg);
	asir_assert(str,O_STR,"strtoascii");
	p = BDY(str);
	len = strlen(p);
	for ( i = len-1, n = 0; i >= 0; i-- ) {
		UTOQ((unsigned int)p[i],q);
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
	Q q;

	list = (LIST)ARG0(arg);
	asir_assert(list,O_LIST,"asciitostr");
	n = BDY(list);
	len = length(n);
	p = MALLOC_ATOMIC(len+1);
	for ( i = 0; i < len; i++, n = NEXT(n) ) {
		q = (Q)BDY(n);
		asir_assert(q,O_N,"asciitostr");
		j = QTOS(q);
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
	char *cmd;
#if PARI
	recover(0);
	if ( setjmp(environnement) ) {
		avma = top; recover(1);
		resetenv("");
	}
#endif
	cmd = BDY((STRING)ARG0(arg));
	exprparse(0,cmd,&fnode);
	*rp = eval(fnode);
}

void Prtostr(arg,rp)
NODE arg;
STRING *rp;
{
	char *b;
	int len;

	len = estimate_length(CO,ARG0(arg));
	b = (char *)MALLOC(len+1);
	soutput_init(b);
	sprintexpr(CO,ARG0(arg));
	MKSTR(*rp,b);
}

void Pstrtov(arg,rp)
NODE arg;
P *rp;
{
	char *p,*t;

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
	makevar(p,rp);
#endif
}
