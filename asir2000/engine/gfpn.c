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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/gfpn.c,v 1.1.1.1 1999/12/03 07:39:08 noro Exp $ 
*/
#include "ca.h"
#include "base.h"

void chsgngfpn(GFPN,GFPN *);

UP current_mod_gfpn;

void setmod_gfpn(p)
P p;
{
	UP t;

	ptoup(p,&t); uptolmup(t,&current_mod_gfpn);
}

void getmod_gfpn(up)
UP *up;
{
	*up = current_mod_gfpn;
}

void ptogfpn(q,l)
Obj q;
GFPN *l;
{
	UP q1,q2;

	if ( !q || (OID(q)==O_N && NID(q)==N_GFPN) ) {
		*l = (GFPN)q;
	} else if ( (OID(q)==O_N && NID(q)==N_Q) || OID(q)==O_P ) {
		ptoup((P)q,&q1); uptolmup(q1,&q2);
		MKGFPN(q2,*l);
	} else
		error("ptogfpn : invalid argument");
}

void gfpntop(q,l)
GFPN q;
P *l;
{
	if ( !q )
		*l = 0;
	else if ( NID(q) == N_GFPN )
		uptop(q->body,l);
	else
		*l = (P)q;
}

void simpgfpn(n,r)
GFPN n;
GFPN *r;
{
	UP rem,t;

	if ( !n )
		*r = 0;
	else if ( NID(n) != N_GFPN )
		*r = n;
	else {
		simpup(n->body,&t);
		remup(t,current_mod_gfpn,&rem);
		MKGFPN(rem,*r);
	}
}

#define NZGFPN(a) ((a)&&(OID(a)==O_N)&&(NID(a)==N_GFPN))

void ntogfpn(a,b)
Obj a;
GFPN *b;
{
	UP t;
	LM lm;

	if ( !a || (OID(a)==O_N && NID(a) == N_GFPN) )
		*b = (GFPN)a;
	else if ( OID(a) == O_N && (NID(a) == N_LM || NID(a) == N_Q) ) {
		qtolm((Q)a,&lm);
		if ( !lm )
			*b = 0;
		else {
			t = UPALLOC(0); t->d = 0; t->c[0] = (Num)lm;
			MKGFPN(t,*b);
		}
	} else
		error("ntogfpn : invalid argument");
}

void addgfpn(a,b,c)
GFPN a,b;
GFPN *c;
{
	UP t,t1,t2;
	GFPN z;

	ntogfpn((Obj)a,&z); a = z; ntogfpn((Obj)b,&z); b = z;
	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else {
		addup(a->body,b->body,&t);
		simpup(t,&t1);
		remup(t1,current_mod_gfpn,&t2);
		MKGFPN(t2,*c);
	}
}

void subgfpn(a,b,c)
GFPN a,b;
GFPN *c;
{
	UP t,t1,t2;
	GFPN z;

	ntogfpn((Obj)a,&z); a = z; ntogfpn((Obj)b,&z); b = z;
	if ( !a )
		chsgngfpn(b,c);
	else if ( !b )
		*c = a;
	else {
		subup(a->body,b->body,&t);
		simpup(t,&t1);
		remup(t1,current_mod_gfpn,&t2);
		MKGFPN(t2,*c);
	}
}

extern int up_lazy;

void mulgfpn(a,b,c)
GFPN a,b;
GFPN *c;
{
	UP t,t1,t2;
	GFPN z;

	ntogfpn((Obj)a,&z); a = z; ntogfpn((Obj)b,&z); b = z;
	if ( !a || !b )
		*c = 0;
	else {
		up_lazy=1;
		mulup(a->body,b->body,&t);
		up_lazy=0;
		simpup(t,&t1);
		remup(t1,current_mod_gfpn,&t2);
		MKGFPN(t2,*c);	
	}
}

void squaregfpn(a,c)
GFPN a;
GFPN *c;
{
	UP t,t1,t2;
	GFPN z;

	ntogfpn((Obj)a,&z); a = z;
	if ( !a )
		*c = 0;
	else {
		squareup(a->body,&t);
		simpup(t,&t1);
		remup(t1,current_mod_gfpn,&t2);
		MKGFPN(t2,*c);	
	}
}

void divgfpn(a,b,c)
GFPN a,b;
GFPN *c;
{
	UP t,t1,i,s;
	GFPN z;

	ntogfpn((Obj)a,&z); a = z; ntogfpn((Obj)b,&z); b = z;
	if ( !b )
		error("divgfpn: division by 0");
	else if ( !a )
		*c = 0;
	else {
		extended_gcdup(b->body,current_mod_gfpn,&i);
		mulup(a->body,i,&t);
		simpup(t,&t1);
		remup(t1,current_mod_gfpn,&s);
		MKGFPN(s,*c);
	}
}

void invgfpn(b,c)
GFPN b;
GFPN *c;
{
	UP i,t;
	GFPN z;

	ntogfpn((Obj)b,&z); b = z;
	if ( !b )
		error("divgfpn: division by 0");
	else {
		simpup(b->body,&t);
		extended_gcdup(t,current_mod_gfpn,&i);
		MKGFPN(i,*c);
	}
}

void chsgngfpn(a,c)
GFPN a,*c;
{
	GFPN z;
	UP t,t1;

	ntogfpn((Obj)a,&z); a = z;
	if ( !a )
		*c = 0;
	else {
		simpup(a->body,&t);
		chsgnup(t,&t1);
		MKGFPN(t1,*c);
	}
}

void pwrgfpn(a,b,c)
GFPN a;
Q b;
GFPN *c;
{
	UP t,s;
	GFPN r;
	Q b0;

	if ( !b ) {
		t = UPALLOC(0); t->d = 0; t->c[0] = (Num)ONELM; MKGFPN(t,*c);
	} else if ( !a )
		*c = 0;
	else {
		DUPQ(b,b0); SGN(b0)=1;
		simpup(a->body,&s);
		hybrid_generic_powermodup(s,current_mod_gfpn,b0,&t);
		MKGFPN(t,r);
		if ( SGN(b) < 0 )
			invgfpn(r,c);
		else
			*c = r;
	}
}

int cmpgfpn(a,b)
GFPN a,b;
{
	GFPN z;

	ntogfpn((Obj)a,&z); a = z; ntogfpn((Obj)b,&z); b = z;
	if ( !a )
		if ( !b )
			return 0;
		else
			return -1;
	else if ( !b )
			return 1;
	else
		return compup(a->body,b->body);
}

void randomgfpn(r)
GFPN *r;
{
	int i,d;
	LM *tb;
	UP t;

	if ( !current_mod_gfpn )
		error("randomgfpn : current_mod_gfpn is not set");
	d = current_mod_gfpn->d;
	t = UPALLOC(d-1);
	for ( i = 0, tb = (LM *)t->c; i < d; i++ )
		random_lm(&tb[i]);
	for ( i = d-1; i >= 0 && !tb[i]; i-- );
	if ( i < 0 )
		*r = 0;
	else {
		t->d = i; MKGFPN(t,*r);
	}
}
