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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/comp.c,v 1.4 2000/12/05 01:24:56 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "comp.h"
#if defined(VISUAL) || (defined(__MACH__) && defined(__ppc__))
#include <stdarg.h>
#else
#include <varargs.h>
#endif

extern f_return;

#if defined(VISUAL) || (defined(__MACH__) && defined(__ppc__))
void call_usrf(FUNC f,...)
{
	va_list ap;
	int ac,i;
	pointer *c;
	NODE tn;

	va_start(ap,f);
	if ( !f )
		notdef(0,0,0,0);
	else {
		pushpvs(f);
		ac = va_arg(ap,int);
		for ( i = 0, tn = f->f.usrf->args; i < ac;
			i++, tn = NEXT(tn) )
			ASSPV((int)FA0((FNODE)BDY(tn)),va_arg(ap,pointer));
		c = va_arg(ap,pointer *); *c = evalstat(BDY(f->f.usrf));
		va_end(ap);
		f_return = 0; poppvs();
	}
}
#else
void call_usrf(va_alist)
va_dcl
{
	va_list ap;
	int ac,i;
	FUNC f;
	pointer a,b,*c;
	NODE tn;

	va_start(ap); f = va_arg(ap,FUNC);
	if ( !f )
		notdef(0,0,0,0);
	else {
		pushpvs(f);
		ac = va_arg(ap,int);
		for ( i = 0, tn = f->f.usrf->args; i < ac;
			i++, tn = NEXT(tn) )
			ASSPV((int)FA0((FNODE)BDY(tn)),va_arg(ap,pointer));
		c = va_arg(ap,pointer *); *c = evalstat(BDY(f->f.usrf));
		f_return = 0; poppvs();
	}
}
#endif

void addcomp(vl,a,b,c)
VL vl;
COMP a,b,*c;
{
	if ( a->type != b->type )
		error("addcomp : types different");
	else
		call_usrf(LSS->sa[a->type].arf.add,2,a,b,c);
}

void subcomp(vl,a,b,c)
VL vl;
COMP a,b,*c;
{
	if ( a->type != b->type )
		error("subcomp : types different");
	else
		call_usrf(LSS->sa[a->type].arf.sub,2,a,b,c);
}

void mulcomp(vl,a,b,c)
VL vl;
COMP a,b,*c;
{
	if ( a->type != b->type )
		error("mulcomp : types different");
	else
		call_usrf(LSS->sa[a->type].arf.mul,2,a,b,c);
}	

void divcomp(vl,a,b,c)
VL vl;
COMP a,b,*c;
{
	if ( a->type != b->type )
		error("divcomp : types different");
	else
		call_usrf(LSS->sa[a->type].arf.div,2,a,b,c);
}	

void chsgncomp(a,b)
COMP a,*b;
{
	call_usrf(LSS->sa[a->type].arf.chsgn,1,a,b);
}

void pwrcomp(vl,a,r,c)
VL vl;
COMP a;
Obj r;
COMP *c;
{
	call_usrf(LSS->sa[a->type].arf.pwr,2,a,r,c);
}

int compcomp(vl,a,b)
VL vl;
COMP a,b;
{
	Q c;
	int s;

	if ( a->type > b->type )
		return 1;
	else if ( a->type < b->type )
		return -1;
	else {
		call_usrf(LSS->sa[a->type].arf.comp,2,a,b,&c);
		s = QTOS(c);
		if ( s > 0 )
			return 1;
		else if ( s < 0 )
			return -1;
		else
			return 0;
	}
}
