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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/P.c,v 1.7 2003/06/24 09:49:36 noro Exp $ 
*/
#ifndef FBASE
#define FBASE
#endif

#include "b.h"
#include "ca.h"

void D_ADDP(vl,p1,p2,pr)
VL vl;
P p1,p2,*pr;
{
	register DCP dc1,dc2,dcr0,dcr;
	V v1,v2;
	P t;

	if ( !p1 ) 
		*pr = p2;
	else if ( !p2 ) 
		*pr = p1;
	else if ( NUM(p1) )
		if ( NUM(p2) ) 
			ADDNUM(p1,p2,pr);
		else 
			ADDPQ(p2,p1,pr);
	else if ( NUM(p2) ) 
		ADDPQ(p1,p2,pr);
	else if ( ( v1 = VR(p1) ) ==  ( v2 = VR(p2) ) ) {
		for ( dc1 = DC(p1), dc2 = DC(p2), dcr0 = 0; dc1 && dc2; )
			switch ( cmpq(DEG(dc1),DEG(dc2)) ) {
				case 0: 
					ADDP(vl,COEF(dc1),COEF(dc2),&t);
					if ( t )  {
						NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc1); COEF(dcr) = t;
					}
					dc1 = NEXT(dc1); dc2 = NEXT(dc2); break;
				case 1:
					NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc1); COEF(dcr) = COEF(dc1);
					dc1 = NEXT(dc1); break;
				case -1:
					NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc2); COEF(dcr) = COEF(dc2);
					dc2 = NEXT(dc2); break;
			}
		if ( !dcr0 ) 
			if ( dc1 )
				dcr0 = dc1;
			else if ( dc2 ) 
				dcr0 = dc2;
			else {
				*pr = 0;
				return;
			}
		else 
			if ( dc1 ) 
				NEXT(dcr) = dc1;
			else if ( dc2 ) 
				NEXT(dcr) = dc2;
			else 
				NEXT(dcr) = 0;
		MKP(v1,dcr0,*pr);
	} else {
		while ( v1 != VR(vl) && v2 != VR(vl) ) 
			vl = NEXT(vl);
		if ( v1 == VR(vl) ) 
			ADDPTOC(vl,p1,p2,pr);
		else 
			ADDPTOC(vl,p2,p1,pr);
	}
}
	
void D_ADDPQ(p,q,pr)
P p,q,*pr;
{
	DCP dc,dcr,dcr0;
	P t;

	if ( NUM(p) ) 
		ADDNUM(p,q,pr);
	else {
		for ( dcr0 = 0, dc = DC(p); dc && DEG(dc); dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = COEF(dc);
		}
		if ( !dc ) {
			NEWDC(NEXT(dcr)); dcr = NEXT(dcr); DEG(dcr) = 0; COEF(dcr) = q;
		} else {
			ADDPQ(COEF(dc),q,&t);
			if ( t ) {
				NEWDC(NEXT(dcr)); dcr = NEXT(dcr); DEG(dcr) = 0; COEF(dcr) = t;
			} 
		}
		NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
	}
}

void D_ADDPTOC(vl,p,c,pr)
VL vl;
P p,c,*pr;
{
	DCP dc,dcr,dcr0;
	P t;

	for ( dcr0 = 0, dc = DC(p); dc && DEG(dc); dc = NEXT(dc) ) {
		NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = COEF(dc);
	}
	if ( !dc ) {
		NEWDC(NEXT(dcr)); dcr = NEXT(dcr); DEG(dcr) = 0; COEF(dcr) = c;
	} else {
		ADDP(vl,COEF(dc),c,&t);
		if ( t ) {
			NEWDC(NEXT(dcr)); dcr = NEXT(dcr); DEG(dcr) = 0; COEF(dcr) = t;
		} 
	}
	NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
}

void D_SUBP(vl,p1,p2,pr)
VL vl;
P p1,p2,*pr;
{
	P t;

	if ( !p2 ) 
		*pr = p1;
	else {
		CHSGNP(p2,&t); ADDP(vl,p1,t,pr);
	}
}

void D_MULP(vl,p1,p2,pr)
VL vl;
P p1,p2,*pr;
{
	register DCP dc,dct,dcr,dcr0;
	V v1,v2;
	P t,s,u;
	int n1,n2;

	if ( !p1 || !p2 ) *pr = 0;
	else if ( NUM(p1) ) 
		MULPQ(p2,p1,pr);
	else if ( NUM(p2) )
		MULPQ(p1,p2,pr);
	else if ( ( v1 = VR(p1) ) ==  ( v2 = VR(p2) ) ) {
		for ( dc = DC(p1), n1 = 0; dc; dc = NEXT(dc), n1++ );
		for ( dc = DC(p2), n2 = 0; dc; dc = NEXT(dc), n2++ );
		if ( n1 > n2 )
			for ( dc = DC(p2), s = 0; dc; dc = NEXT(dc) ) {
				for ( dcr0 = 0, dct = DC(p1); dct; dct = NEXT(dct) ) {
					NEXTDC(dcr0,dcr); MULP(vl,COEF(dct),COEF(dc),&COEF(dcr));
					addq(DEG(dct),DEG(dc),&DEG(dcr));
				}
				NEXT(dcr) = 0; MKP(v1,dcr0,t);
				ADDP(vl,s,t,&u); s = u; t = u = 0;
			}
		else
			for ( dc = DC(p1), s = 0; dc; dc = NEXT(dc) ) {
				for ( dcr0 = 0, dct = DC(p2); dct; dct = NEXT(dct) ) {
					NEXTDC(dcr0,dcr); MULP(vl,COEF(dct),COEF(dc),&COEF(dcr));
					addq(DEG(dct),DEG(dc),&DEG(dcr));
				}
				NEXT(dcr) = 0; MKP(v1,dcr0,t);
				ADDP(vl,s,t,&u); s = u; t = u = 0;
			}
		*pr = s;
	} else {
		while ( v1 != VR(vl) && v2 != VR(vl) ) 
			vl = NEXT(vl);
		if ( v1 == VR(vl) ) 
			MULPC(vl,p1,p2,pr);
		else 
			MULPC(vl,p2,p1,pr);
	}
}

void D_MULPQ(p,q,pr)
P p,q,*pr;
{
	DCP dc,dcr,dcr0;
	P t;

	if (!p || !q)
		*pr = 0;
	else if ( Uniq(q) ) 
		*pr = p;
	else if ( NUM(p) ) 
		MULNUM(p,q,pr);
	else {
		for ( dcr0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			MULPQ(COEF(dc),q,&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); COEF(dcr) = t; DEG(dcr) = DEG(dc);
			}
		}
		if ( dcr0 ) {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		} else
			*pr = 0;
	}
}

void D_MULPC(vl,p,c,pr)
VL vl;
P p,c,*pr;
{
	DCP dc,dcr,dcr0;
	P t;

	if ( NUM(c) ) 
		MULPQ(p,c,pr);
	else {
		for ( dcr0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			MULP(vl,COEF(dc),c,&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); COEF(dcr) = t; DEG(dcr) = DEG(dc);
			}
		}
		if ( dcr0 ) {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		} else
			*pr = 0;
	}
}

void D_PWRP(vl,p,q,pr)
VL vl;
P p,*pr;
Q q;
{
	DCP dc,dcr;
	int n,i;
	P *x,*y;
	P t,s,u;
	DCP dct;
	P *pt;

	if ( !q ) { 
		*pr = (P)One; 
	} else if ( !p ) 
		*pr = 0;
	else if ( UNIQ(q) ) 
		*pr = p;
	else if ( NUM(p) ) 
		PWRNUM(p,q,pr);
	else {
		dc = DC(p);
		if ( !NEXT(dc) ) {
			NEWDC(dcr);
			PWRP(vl,COEF(dc),q,&COEF(dcr)); mulq(DEG(dc),q,&DEG(dcr));
			NEXT(dcr) = 0; MKP(VR(p),dcr,*pr);
		} else if ( !INT(q) ) {
			error("pwrp: can't calculate fractional power."); *pr = 0;
		} else if ( PL(NM(q)) == 1 ) {
			n = QTOS(q); x = (P *)ALLOCA((n+1)*sizeof(pointer));
			NEWDC(dct); DEG(dct) = DEG(dc); COEF(dct) = COEF(dc);
			NEXT(dct) = 0; MKP(VR(p),dct,t);
			for ( i = 0, u = (P)One; i < n; i++ ) {
				x[i] = u; MULP(vl,u,t,&s); u = s;
			}
			x[n] = u; y = (P *)ALLOCA((n+1)*sizeof(pointer));
			if ( DEG(NEXT(dc)) ) {
				dct = NEXT(dc); MKP(VR(p),dct,t);
			} else 
				t = COEF(NEXT(dc));
			for ( i = 0, u = (P)One; i < n; i++ ) {
				y[i] = u; MULP(vl,u,t,&s); u = s;
			}
			y[n] = u; 
			pt = (P *)ALLOCA((n+1)*sizeof(pointer)); MKBC(n,pt);
			for ( i = 0, u = 0; i <= n; i++ ) {
				MULP(vl,x[i],y[n-i],&t); MULP(vl,t,pt[i],&s); 
				ADDP(vl,u,s,&t); u = t;
			}
			*pr = u;
		} else {
			error("exponent too big");
			*pr = 0;
		}		
	}
}

void D_CHSGNP(p,pr)
P p,*pr;
{
	register DCP dc,dcr,dcr0;
	P t;

	if ( !p ) 
		*pr = NULL;
	else if ( NUM(p) ) {
#if defined(_PA_RISC1_1) || defined(__alpha) || defined(mips) || defined(_IBMR2)
#ifdef FBASE
		chsgnnum((Num)p,(Num *)pr);
#else
		MQ mq;

		NEWMQ(mq); CONT(mq)=mod-CONT((MQ)p); *pr = (P)mq;
#endif
#else
		CHSGNNUM(p,t); *pr = t;
#endif
	} else {
		for ( dcr0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); CHSGNP(COEF(dc),&COEF(dcr)); DEG(dcr) = DEG(dc);
		}
		NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
	}
}


#ifdef FBASE
void ptozp(p,sgn,c,pr)
P p;
int sgn;
Q *c;
P *pr;
{
	N nm,dn;

	if ( !p ) {
		*c = 0; *pr = 0;
	} else {
		lgp(p,&nm,&dn);
		if ( UNIN(dn) )
			NTOQ(nm,sgn,*c);
		else 
			NDTOQ(nm,dn,sgn,*c);
		divcp(p,*c,pr);
	}
}

void lgp(p,g,l)
P p;
N *g,*l;
{
	N g1,g2,l1,l2,l3,l4,tmp;
	DCP dc;

	if ( NUM(p) ) {
		*g = NM((Q)p);
		if ( INT((Q)p) ) 
			*l = ONEN;
		else 
			*l = DN((Q)p);
	} else {
		dc = DC(p); lgp(COEF(dc),g,l);
		for ( dc = NEXT(dc); dc; dc = NEXT(dc) ) {
			lgp(COEF(dc),&g1,&l1); gcdn(*g,g1,&g2); *g = g2;
			gcdn(*l,l1,&l2); kmuln(*l,l1,&l3); divn(l3,l2,&l4,&tmp); *l = l4;
		}
	} 
}

void divcp(f,q,rp)
P f;
Q q;
P *rp;
{
	DCP dc,dcr,dcr0;

	if ( !f ) 
		*rp = 0;
	else if ( NUM(f) )
		DIVNUM(f,q,rp);
	else {
		for ( dc = DC(f), dcr0 = 0; dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc);
			divcp(COEF(dc),q,&COEF(dcr));
		}
		NEXT(dcr) = 0; MKP(VR(f),dcr0,*rp);
	}
}

void diffp(vl,p,v,r)
VL vl;
P p;
V v;
P *r;
{
	P t;
	DCP dc,dcr,dcr0;

	if ( !p || NUM(p) ) 
		*r = 0;
	else {
		if ( v == VR(p) ) {
			for ( dc = DC(p), dcr0 = 0; 
				dc && DEG(dc); dc = NEXT(dc) ) {
				MULPQ(COEF(dc),(P)DEG(dc),&t);
				if ( t ) {
					NEXTDC(dcr0,dcr); SUBQ(DEG(dc),ONE,&DEG(dcr));
					COEF(dcr) = t;
				}
			}
			if ( !dcr0 )
				*r = 0;
			else {
				NEXT(dcr) = 0; MKP(v,dcr0,*r);
			}
		} else {
			for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
				diffp(vl,COEF(dc),v,&t);
				if ( t ) {
					NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
				}
			}
			if ( !dcr0 )
				*r = 0;
			else {
				NEXT(dcr) = 0; MKP(VR(p),dcr0,*r);
			}
		}
	}
}

/* Euler derivation */
void ediffp(vl,p,v,r)
VL vl;
P p;
V v;
P *r;
{
	P t;
	DCP dc,dcr,dcr0;

	if ( !p || NUM(p) ) 
		*r = 0;
	else {
		if ( v == VR(p) ) {
			for ( dc = DC(p), dcr0 = 0; 
				  dc && DEG(dc); dc = NEXT(dc) ) {
				MULPQ(COEF(dc),(P)DEG(dc),&t);
				if ( t ) {
					NEXTDC(dcr0,dcr);
					DEG(dcr) = DEG(dc);
					COEF(dcr) = t;
				}
			}
			if ( !dcr0 )
				*r = 0;
			else {
				NEXT(dcr) = 0; MKP(v,dcr0,*r);
			}
		} else {
			for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
				ediffp(vl,COEF(dc),v,&t);
				if ( t ) {
					NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
				}
			}
			if ( !dcr0 )
				*r = 0;
			else {
				NEXT(dcr) = 0; MKP(VR(p),dcr0,*r);
			}
		}
	}
}

void coefp(p,d,pr)
P p;
int d;
P *pr;
{
	DCP dc;
	int sgn;
	Q dq;

	if ( NUM(p) )
		if ( d == 0 ) 
			*pr = p;
		else 
			*pr = 0;
	else {
		for ( STOQ(d,dq), dc = DC(p); dc; dc = NEXT(dc) ) 
			if ( (sgn = cmpq(DEG(dc),dq)) > 0 )
				continue;
			else if ( sgn == 0 ) {
				*pr = COEF(dc);
				return;
			} else {
				*pr = 0;
				break;	
			}
		*pr = 0;
	}
}

int compp(vl,p1,p2)
VL vl;
P p1,p2;
{
	DCP dc1,dc2;
	V v1,v2;

	if ( !p1 )
		return p2 ? -1 : 0;
	else if ( !p2 )
		return 1;
	else if ( NUM(p1) )
		return NUM(p2) ? (*cmpnumt[MAX(NID(p1),NID(p2))])(p1,p2) : -1;
	else if ( NUM(p2) )
		return 1;
	if ( (v1 = VR(p1)) == (v2 = VR(p2)) ) {
		for ( dc1 = DC(p1), dc2 = DC(p2); 
			dc1 && dc2; dc1 = NEXT(dc1), dc2 = NEXT(dc2) )
			switch ( cmpq(DEG(dc1),DEG(dc2)) ) {
				case 1:
					return 1; break;
				case -1:
					return -1; break;
				default:
					switch ( compp(vl,COEF(dc1),COEF(dc2)) ) {
						case 1:
							return 1; break;
						case -1:
							return -1; break;
						default:
							break;
					}
					break;
			}
		return dc1 ? 1 : (dc2 ? -1 : 0);
	} else {
		for ( ; v1 != VR(vl) && v2 != VR(vl); vl = NEXT(vl) );
		return v1 == VR(vl) ? 1 : -1;
	}
}

void csump(vl,p,c)
VL vl;
P p;
Q *c;
{
	DCP dc;
	Q s,s1,s2;

	if ( !p || NUM(p) )
		*c = (Q)p;
	else {
		for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) ) {
			csump(vl,COEF(dc),&s1); addq(s,s1,&s2); s = s2;
		}
		*c = s;
	}
}

void degp(v,p,d)
V v;
P p;
Q *d;
{
	DCP dc;
	Q m,m1;
	
	if ( !p || NUM(p) ) 
		*d = 0;
	else if ( v == VR(p) ) 
		*d = DEG(DC(p));
	else {
		for ( dc = DC(p), m = 0; dc; dc = NEXT(dc) ) {
			degp(v,COEF(dc),&m1);
			if ( cmpq(m,m1) < 0 )
				m = m1;
		}
		*d = m;
	}
}

void mulpc_trunc(VL vl,P p,P c,VN vn,P *pr);
void mulpq_trunc(P p,Q q,VN vn,P *pr);
void mulp_trunc(VL vl,P p1,P p2,VN vn,P *pr);

void mulp_trunc(VL vl,P p1,P p2,VN vn,P *pr)
{
	register DCP dc,dct,dcr,dcr0;
	V v1,v2;
	P t,s,u;
	int n1,n2,i,d,d1;

	if ( !p1 || !p2 ) *pr = 0;
	else if ( NUM(p1) ) 
		mulpq_trunc(p2,(Q)p1,vn,pr);
	else if ( NUM(p2) )
		mulpq_trunc(p1,(Q)p2,vn,pr);
	else if ( ( v1 = VR(p1) ) ==  ( v2 = VR(p2) ) ) {
		for ( ; vn->v && vn->v != v1; vn++ )
			if ( vn->n ) {
				/* p1,p2 do not contain vn->v */
				*pr = 0;
				return;
			}
		if ( !vn->v )
			error("mulp_trunc : invalid vn");
		d = vn->n;
		for ( dc = DC(p2), s = 0; dc; dc = NEXT(dc) ) {
			for ( dcr0 = 0, dct = DC(p1); dct; dct = NEXT(dct) ) {
				d1 = QTOS(DEG(dct))+QTOS(DEG(dc));
				if ( d1 >= d ) {
					mulp_trunc(vl,COEF(dct),COEF(dc),vn+1,&t);
					if ( t ) {
						NEXTDC(dcr0,dcr);
						STOQ(d1,DEG(dcr));
						COEF(dcr) = t;
					}
				}
			}
			if ( dcr0 ) {
				NEXT(dcr) = 0; MKP(v1,dcr0,t);
				addp(vl,s,t,&u); s = u; t = u = 0;
			}
		}
		*pr = s;
	} else {
		while ( v1 != VR(vl) && v2 != VR(vl) ) 
			vl = NEXT(vl);
		if ( v1 == VR(vl) ) 
			mulpc_trunc(vl,p1,p2,vn,pr);
		else 
			mulpc_trunc(vl,p2,p1,vn,pr);
	}
}

void mulpq_trunc(P p,Q q,VN vn,P *pr)
{
	DCP dc,dcr,dcr0;
	P t;
	int i,d;
	V v;

	if (!p || !q)
		*pr = 0;
	else if ( NUM(p) ) {
		for ( ; vn->v; vn++ )
			if ( vn->n ) {
				*pr = 0;
				return;
			}
		MULNUM(p,q,pr);
	} else {
		v = VR(p);
		for ( ; vn->v && vn->v != v; vn++ ) {
			if ( vn->n ) {
				/* p does not contain vn->v */
				*pr = 0;
				return;
			}
		}
		if ( !vn->v )
			error("mulpq_trunc : invalid vn");
		d = vn->n;
		for ( dcr0 = 0, dc = DC(p); dc && QTOS(DEG(dc)) >= d; dc = NEXT(dc) ) {
			mulpq_trunc(COEF(dc),q,vn+1,&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); COEF(dcr) = t; DEG(dcr) = DEG(dc);
			}
		}
		if ( dcr0 ) {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		} else
			*pr = 0;
	}
}

void mulpc_trunc(VL vl,P p,P c,VN vn,P *pr)
{
	DCP dc,dcr,dcr0;
	P t;
	V v;
	int i,d;

	if ( NUM(c) ) 
		mulpq_trunc(p,(Q)c,vn,pr);
	else {
		v = VR(p);
		for ( ; vn->v && vn->v != v; vn++ )
			if ( vn->n ) {
				/* p,c do not contain vn->v */
				*pr = 0;
				return;
			}
		if ( !vn->v )
			error("mulpc_trunc : invalid vn");
		d = vn->n;
		for ( dcr0 = 0, dc = DC(p); dc && QTOS(DEG(dc)) >= d; dc = NEXT(dc) ) {
			mulp_trunc(vl,COEF(dc),c,vn+1,&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); COEF(dcr) = t; DEG(dcr) = DEG(dc);
			}
		}
		if ( dcr0 ) {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		} else
			*pr = 0;
	}
}

void quop_trunc(VL vl,P p1,P p2,VN vn,P *pr)
{
	DCP dc,dcq0,dcq;
	P t,s,m,lc2,qt;
	V v1,v2;
	Q d2;
	VN vn1;

	if ( !p1 )
		*pr = 0;
	else if ( NUM(p2) )
		divsp(vl,p1,p2,pr);
	else if ( (v1 = VR(p1)) != (v2 = VR(p2)) ) {
		for ( dcq0 = 0, dc = DC(p1); dc; dc = NEXT(dc) ) {
			NEXTDC(dcq0,dcq);
			DEG(dcq) = DEG(dc);
			quop_trunc(vl,COEF(dc),p2,vn,&COEF(dcq));
		}
		NEXT(dcq) = 0;
		MKP(v1,dcq0,*pr);
	} else {
		d2 = DEG(DC(p2));
		lc2 = COEF(DC(p2));
		t = p1;
		dcq0 = 0;
		/* vn1 = degree list of LC(p2) */
		for ( vn1 = vn; vn1->v != v1; vn1++ );
		vn1++;
		while ( t ) {
			dc = DC(t);
			NEXTDC(dcq0,dcq);
			subq(DEG(dc),d2,&DEG(dcq));
			quop_trunc(vl,COEF(dc),lc2,vn1,&COEF(dcq));
			NEXT(dcq) = 0;
			MKP(v1,dcq,qt);
			mulp_trunc(vl,p2,qt,vn,&m);
			subp(vl,t,m,&s); t = s;
		}
		NEXT(dcq) = 0;
		MKP(v1,dcq0,*pr);
	}
}
#endif
