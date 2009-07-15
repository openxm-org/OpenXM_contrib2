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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/bf.c,v 1.5 2009/03/27 14:42:29 ohara Exp $ 
*/
#include "ca.h"
#if defined(PARI)
#include "base.h"
#include <math.h>
#include "genpari.h"

void ritopa(Obj,GEN *);
void patori(GEN,Obj *);

void addbf(a,b,c)
Num a,b;
Num *c;
{
	GEN pa,pb,z;
	long ltop,lbot;

	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
		(*addnumt[MIN(NID(a),NID(b))])(a,b,c);
	else {
		ltop = avma; ritopa((Obj)a,&pa);
		ritopa((Obj)b,&pb); lbot = avma;
		z = gerepile(ltop,lbot,gadd(pa,pb));
		patori(z,(Obj *)c); cgiv(z);
	}
}

void subbf(a,b,c)
Num a,b;
Num *c;
{
	GEN pa,pb,z;
	long ltop,lbot;

	if ( !a )
		(*chsgnnumt[NID(b)])(b,c);
	else if ( !b )
		*c = a;
	else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
		(*subnumt[MIN(NID(a),NID(b))])(a,b,c);
	else {
		ltop = avma; ritopa((Obj)a,&pa); ritopa((Obj)b,&pb); lbot = avma;
		z = gerepile(ltop,lbot,gsub(pa,pb));
		patori(z,(Obj *)c); cgiv(z);
	}
}

void mulbf(a,b,c)
Num a,b;
Num *c;
{
	GEN pa,pb,z;
	long ltop,lbot;

	if ( !a || !b )
		*c = 0;
	else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
		(*mulnumt[MIN(NID(a),NID(b))])(a,b,c);
	else {
		ltop = avma; ritopa((Obj)a,&pa); ritopa((Obj)b,&pb); lbot = avma;
		z = gerepile(ltop,lbot,gmul(pa,pb));
		patori(z,(Obj *)c); cgiv(z);
	}
}

void divbf(a,b,c)
Num a,b;
Num *c;
{
	GEN pa,pb,z;
	long ltop,lbot;

	if ( !b )
		error("divbf : division by 0");
	else if ( !a )
		*c = 0;
	else if ( (NID(a) <= N_A) && (NID(b) <= N_A ) )
		(*divnumt[MIN(NID(a),NID(b))])(a,b,c);
	else {
		ltop = avma; ritopa((Obj)a,&pa); ritopa((Obj)b,&pb); lbot = avma;
		z = gerepile(ltop,lbot,gdiv(pa,pb));
		patori(z,(Obj *)c); cgiv(z);
	}
}

void pwrbf(a,e,c)
Num a,e;
Num *c;
{
	if ( !e )
		*c = (Num)ONE;
	else if ( !a )
		*c = 0;
	else {
		gpui_ri((Obj)a,(Obj)e,(Obj *)c);
	}
}

void chsgnbf(a,c)
Num a,*c;
{
	BF t;
	GEN z;
	int s;

	if ( !a )
		*c = 0;
	else if ( NID(a) <= N_A )
		(*chsgnnumt[NID(a)])(a,c);
	else {
		z = (GEN)((BF)a)->body; s = lg(z); NEWBF(t,s);
		bcopy((char *)a,(char *)t,sizeof(struct oBF)+((s-1)*sizeof(long)));
		z = (GEN)((BF)t)->body; setsigne(z,-signe(z));
		*c = (Num)t;
	}
}

int cmpbf(a,b)
Num a,b;
{
	GEN pa,pb;
	int s;

	if ( !a ) {
		if ( !b || (NID(b)<=N_A) )
			return (*cmpnumt[NID(b)])(a,b);
		else
			return -signe(((BF)b)->body);
	} else if ( !b ) {
		if ( !a || (NID(a)<=N_A) )
			return (*cmpnumt[NID(a)])(a,b);
		else
			return signe(((BF)a)->body);
	} else {
		ritopa((Obj)a,&pa); ritopa((Obj)b,&pb);
		s = gcmp(pa,pb); cgiv(pb); cgiv(pa);
		return s;
	}
}
#endif /* PARI */
