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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/RU.c,v 1.3 2000/08/22 05:04:05 noro Exp $ 
*/
#include "ca.h"

int qcoefr(r)
Obj r;
{
	if ( !r )
		return 1;
	else
		switch ( OID(r) ) {
			case O_N:
				return RATN(r)?1:0; break;
			case O_P:
				return qcoefp(r); break;
			default:
				return qcoefp((Obj)NM((R)r))&&qcoefp((Obj)DN((R)r)); break;
		}
}

int qcoefp(p)
Obj p;
{
	DCP dc;

	if ( !p )
		return 1;
	else
		switch ( OID(p) ) {
			case O_N:
				return RATN(p)?1:0; break;
			default:
				for ( dc = DC((P)p); dc; dc = NEXT(dc) )
					if ( !qcoefp((Obj)COEF(dc)) )
						return 0;
				return 1; break;
		}
}

void reductr(vl,p,r)
VL vl;
Obj p,*r;
{
	P t,s,u,cnm,cdn,pnm,pdn;
	R a;

	if ( !p )
		*r = 0;
	else if ( OID(p) <= O_P )
		*r = p;
	else if ( ((R)p)->reduced )
		*r = p;
	else if ( NUM(DN((R)p)) )
		divsp(vl,NM((R)p),DN((R)p),(P *)r);
	else if ( qcoefp((Obj)NM((R)p)) && qcoefp((Obj)DN((R)p)) ) {
		ptozp(NM((R)p),1,(Q *)&cnm,&pnm); ptozp(DN((R)p),1,(Q *)&cdn,&pdn);
		ezgcdpz(vl,pnm,pdn,&t); 
		divsp(vl,NM((R)p),t,&u); divsp(vl,DN((R)p),t,&s);
		if ( NUM(s) )
			divsp(vl,u,s,(P *)r);
		else {
			divsp(vl,u,cdn,&pnm); divsp(vl,s,cdn,&pdn); 
			if ( headsgn(pdn) < 0 ) {
				chsgnp(pnm,&t); pnm = t;
				chsgnp(pdn,&t); pdn = t;
			}
			MKRAT(pnm,pdn,1,a); *r = (Obj)a;
		}
	} else {
		MKRAT(NM((R)p),DN((R)p),1,a); *r = (Obj)a;
	}
}

void pderivr(vl,a,v,b)
VL vl;
V v;
Obj a,*b;
{
	P t,s,u;
	R r;

	if ( !a )
		*b = 0;
	else if ( OID(a) <= O_P )
		diffp(vl,(P)a,v,(P *)b); 
	else {
		diffp(vl,NM((R)a),v,&t); mulp(vl,t,DN((R)a),&s);
		diffp(vl,DN((R)a),v,&t); mulp(vl,t,NM((R)a),&u);
		subp(vl,s,u,&t);
		if ( t ) {
			mulp(vl,DN((R)a),DN((R)a),&u); MKRAT(t,u,0,r); *b = (Obj)r;
		} else 
			*b = 0;
	}
}

void clctvr(vl,p,nvl)
VL vl,*nvl;
Obj p;
{
	VL vl1,vl2;

	if ( !p )
		*nvl = 0;
	else if ( OID(p) <= O_P )
		clctv(vl,(P)p,nvl);
	else {
		clctv(vl,NM((R)p),&vl1); clctv(vl,DN((R)p),&vl2); mergev(vl,vl1,vl2,nvl);
	}
}
