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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/vect.c,v 1.4 2003/05/20 07:19:41 noro Exp $ 
*/
#include "ca.h"

void addvect(vl,a,b,c)
VL vl;
VECT a,b,*c;
{
	int len,i;
	VECT t;
	pointer *ab,*bb,*tb;

	if ( !a ) 
		*c = b;
	else if ( !b )
		*c = a;
	else if ( a->len != b->len ) {
		*c = 0; error("addvect : size mismatch");
	} else {
		len = a->len;
		MKVECT(t,len);
		for ( i = 0, ab = BDY(a), bb = BDY(b), tb = BDY(t); i < len; i++ )
			arf_add(vl,(Obj)ab[i],(Obj)bb[i],(Obj *)&tb[i]);
		*c = t;
	}
}

void subvect(vl,a,b,c)
VL vl;
VECT a,b,*c;
{
	int len,i;
	VECT t;
	pointer *ab,*bb,*tb;

	if ( !a ) 
		chsgnvect(b,c);
	else if ( !b )
		*c = a;
	else if (a->len != b->len ) {
		*c = 0; error("subvect : size mismatch");
	} else {
		len = a->len;
		MKVECT(t,len);
		for ( i = 0, ab = BDY(a), bb = BDY(b), tb = BDY(t); 
				i < len; i++ )
				arf_sub(vl,(Obj)ab[i],(Obj)bb[i],(Obj *)&tb[i]);
		*c = t;
	}
}

void mulvect(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	if ( !a || !b )
		*c = 0;
	else if ( OID(a) <= O_R || OID(a) == O_DP )
		mulrvect(vl,a,(VECT)b,(VECT *)c);
	else if ( OID(b) <= O_R || OID(b) == O_DP )
		mulrvect(vl,b,(VECT)a,(VECT *)c);
	else
		notdef(vl,a,b,c);
}

void divvect(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	Obj t;

	if ( !b ) 
		error("divvect : division by 0");
	else if ( !a )
		*c = 0;
	else if ( OID(b) > O_R )
		notdef(vl,a,b,c);
	else {
		arf_div(vl,(Obj)ONE,b,&t); mulrvect(vl,(Obj)t,(VECT)a,(VECT *)c);
	}
}

void chsgnvect(a,b)
VECT a,*b;
{
	VECT t;
	int len,i;
	pointer *ab,*tb;

	if ( !a )
		*b = 0;
	else {
		len = a->len;
		MKVECT(t,len);
		for ( i = 0, ab = BDY(a), tb = BDY(t); 
			i < len; i++ )
			arf_chsgn((Obj)ab[i],(Obj *)&tb[i]);
		*b = t;
	} 
}

void mulrvect(vl,a,b,c)
VL vl;
Obj a;
VECT b,*c;
{
	int len,i;
	VECT t;
	pointer *bb,*tb;

	if ( !a || !b ) 
		*c = 0;
	else {
		len = b->len;
		MKVECT(t,len);
		for ( i = 0, bb = BDY(b), tb = BDY(t); i < len; i++ )
			arf_mul(vl,a,(Obj)bb[i],(Obj *)&tb[i]);
		*c = t;
	}
}

int compvect(vl,a,b)
VL vl;
VECT a,b;
{
	int i,len,t;

	if ( !a )
		return b?-1:0;
	else if ( !b )
		return 1;
	else if ( a->len != b->len )
		return a->len>b->len ? 1 : -1;
	else {
		for ( i = 0, len = a->len; i < len; i++ )
			if ( t = arf_comp(vl,(Obj)BDY(a)[i],(Obj)BDY(b)[i]) )
				return t;
		return 0;
	}
}
