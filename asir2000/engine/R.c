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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/R.c,v 1.3 2000/08/22 05:04:05 noro Exp $ 
*/
#include "ca.h"

void addr(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	P t,s,u;
	R r;

	if ( !a ) 
		*c = b;
	else if ( !b )
		*c = a;
	else if ( !RAT(a) )
		if ( !RAT(b) )
			addp(vl,(P)a,(P)b,(P *)c);
		else {
			mulp(vl,(P)a,DN((R)b),&t); addp(vl,t,NM((R)b),&s);
			if ( s )
				MKRAT(s,DN((R)b),((R)b)->reduced,r);
			else
				r = 0;
			*c = (Obj)r;
		}
	else if ( !RAT(b) ) {
		mulp(vl,DN((R)a),(P)b,&t); addp(vl,NM((R)a),t,&s);
		if ( s )
			MKRAT(s,DN((R)a),((R)a)->reduced,r);
		else
			r = 0;
		*c = (Obj)r;
	} else {
		mulp(vl,NM((R)a),DN((R)b),&t); mulp(vl,NM((R)b),DN((R)a),&s); 
		addp(vl,t,s,&u);
		if ( u ) {
			mulp(vl,DN((R)a),DN((R)b),&t); MKRAT(u,t,0,r); *c = (Obj)r;
		} else
			*c = 0;
	}
}

void subr(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	P t,s,u;
	R r;

	if ( !a ) 
		chsgnr(b,c);
	else if ( !b )
		*c = a;
	else if ( !RAT(a) )
		if ( !RAT(b) )
			subp(vl,(P)a,(P)b,(P *)c);
		else {
			mulp(vl,(P)a,DN((R)b),&t); subp(vl,t,NM((R)b),&s);
			if ( s )
				MKRAT(s,DN((R)b),((R)b)->reduced,r);
			else
				r = 0;
			*c = (Obj)r;
		}
	else if ( !RAT(b) ) {
		mulp(vl,DN((R)a),(P)b,&t); subp(vl,NM((R)a),t,&s);
		if ( s )
			MKRAT(s,DN((R)a),((R)a)->reduced,r);
		else
			r = 0;
		*c = (Obj)r;
	} else {
		mulp(vl,NM((R)a),DN((R)b),&t); mulp(vl,NM((R)b),DN((R)a),&s); 
		subp(vl,t,s,&u);
		if ( u ) {
			mulp(vl,DN((R)a),DN((R)b),&t); MKRAT(u,t,0,r); *c = (Obj)r;
		} else
			*c = 0;
	}
}

void mulr(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	P t,s;
	R r;

	if ( !a || !b ) 
		*c = 0;
	else if ( !RAT(a) )
		if ( !RAT(b) )
			mulp(vl,(P)a,(P)b,(P *)c);
		else {
			mulp(vl,(P)a,NM((R)b),&t); MKRAT(t,DN((R)b),0,r); *c = (Obj)r;
		}
	else if ( !RAT(b) ) {
		mulp(vl,NM((R)a),(P)b,&t); MKRAT(t,DN((R)a),0,r); *c = (Obj)r;
	} else {
		mulp(vl,NM((R)a),NM((R)b),&t); mulp(vl,DN((R)a),DN((R)b),&s); 
		MKRAT(t,s,0,r); *c = (Obj)r;
	}
}

void divr(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	P t,s;
	R r;

	if ( !b ) 
		error("divr : division by 0");
	else if ( !a )
		*c = 0;
	else if ( !RAT(a) )
		if ( !RAT(b) )
			if ( NUM(b) )
				divsp(vl,(P)a,(P)b,(P *)c);
			else {
				MKRAT((P)a,(P)b,0,r); *c = (Obj)r;
			}
		else {
			mulp(vl,(P)a,DN((R)b),&t); MKRAT(t,NM((R)b),0,r); *c = (Obj)r;
		}
	else if ( !RAT(b) ) {
		mulp(vl,DN((R)a),(P)b,&t); MKRAT(NM((R)a),t,0,r); *c = (Obj)r;
	} else {
		mulp(vl,NM((R)a),DN((R)b),&t); mulp(vl,DN((R)a),NM((R)b),&s); 
		MKRAT(t,s,0,r); *c = (Obj)r;
	}
}

void pwrr(vl,a,q,c)
VL vl;
Obj a,q,*c;
{
	P t,s;
	R r;
	Q q1;

	if ( !q )
		*c = (Obj)ONE;
	else if ( !a )
		*c = 0;
	else if ( !RAT(a) )
		pwrp(vl,(P)a,(Q)q,(P *)c);
	else if ( !NUM(q) || !RATN(q) || !INT(q) )
		notdef(vl,a,q,c);
	else {
		if ( SGN((Q)q) < 0 ) {
			chsgnq((Q)q,&q1); pwrp(vl,DN((R)a),q1,&t); pwrp(vl,NM((R)a),q1,&s);
		} else {
			pwrp(vl,NM((R)a),(Q)q,&t); pwrp(vl,DN((R)a),(Q)q,&s);
		}
		MKRAT(t,s,((R)a)->reduced,r); *c = (Obj)r;
	}
}

void chsgnr(a,b)
Obj a,*b;
{
	P t;
	R r;

	if ( !a )
		*b = 0;
	else if ( !RAT(a) )
		chsgnp((P)a,(P *)b);
	else {
		chsgnp(NM((R)a),&t); MKRAT(t,DN((R)a),((R)a)->reduced,r); *b = (Obj)r;
	}
}

int compr(vl,a,b)
VL vl;
Obj a,b;
{
	int t;

	if ( !a )
		return b ? -1 : 0;
	else if ( !b )
		return 1;
	else if ( !RAT(a) )
		return !RAT(b) ? compp(vl,(P)a,(P)b) : -1;
	else if ( !RAT(b) )
		return 1;
	else {
		t = compp(vl,NM((R)a),NM((R)b));
		if ( !t )
			t = compp(vl,DN((R)a),DN((R)b));
		return t;
	}
}

int equalr(vl,a,b)
VL vl;
Obj a,b;
{
	if ( !a )
		return b ? 0 : 1;
	else if ( !b )
		return 0;
	else if ( !RAT(a) )
		return !RAT(b) ? equalp(vl,(P)a,(P)b) : 0;
	else if ( !RAT(b) )
		return 0;
	else
		return equalp(vl,NM((R)a),NM((R)b)) && equalp(vl,DN((R)a),DN((R)b));
}
