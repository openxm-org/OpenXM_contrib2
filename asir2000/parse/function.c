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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/function.c,v 1.2 2000/08/21 08:31:46 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void appendbinf(flistp,name,func,argc)
NODE *flistp;
char *name;
void (*func)();
int argc;
{
	FUNC t;
	NODE n;

	t = (FUNC)MALLOC(sizeof(struct oFUNC)); 
	t->name = name; t->id = A_BIN; t->argc = argc; t->f.binf = func;
	MKNODE(n,t,*flistp); *flistp = n;
}

void appendparif(flistp,name,func,type)
NODE *flistp;
char *name;
int (*func)();
int type;
{
	FUNC t;
	NODE n;

	t = (FUNC)MALLOC(sizeof(struct oFUNC)); 
	t->name = name; t->id = A_PARI; t->type = type; t->f.binf = (void (*)())func;
	MKNODE(n,t,*flistp); *flistp = n;
}

void appendsysf(name,func,argc)
char *name;
void (*func)();
int argc;
{
	appendbinf(&sysf,name,func,argc);
}

void appendubinf(name,func,argc)
char *name;
void (*func)();
int argc;
{
	appendbinf(&ubinf,name,func,argc);
}

void dcptolist(dc,listp)
DCP dc;
LIST *listp;
{
	NODE node,tnode,ln0,ln1;
	LIST l;

	for ( node = 0; dc; dc = NEXT(dc) ) {
		NEXTNODE(node,tnode);
		MKNODE(ln1,DEG(dc),0); MKNODE(ln0,COEF(dc),ln1);
		MKLIST(l,ln0); BDY(tnode) = (pointer)l;
	}
	NEXT(tnode) = 0; MKLIST(l,node); *listp = l;
}
