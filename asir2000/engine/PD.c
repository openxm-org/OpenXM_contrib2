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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/PD.c,v 1.3 2000/08/22 05:04:04 noro Exp $ 
*/
#ifndef FBASE
#define FBASE
#endif

#include "b.h"
#include "ca.h"

void D_DIVSRP(vl,p1,p2,q,r)
VL vl;
P p1,p2;
P *q,*r;
{
	register int i,j;
	register DCP dc1,dc2,dc;
	P m,s,dvr;
	P *pq,*pr,*pd;
	V v1,v2;
	Q deg1,deg2;
	int d1,d2,sgn;

	if ( !p1 ) {
		*q = 0; *r = 0;
	} else if ( NUM(p2) ) 
		if ( NUM(p1) ) {
			DIVNUM(p1,p2,q); *r = 0;
		} else {
			DIVSDCP(vl,p1,p2,q); *r = 0;
		}
	else if ( NUM(p1) ) {
		*q = 0; *r = p1;
	} else if ( ( v1 = VR(p1) ) == ( v2 = VR(p2) ) ) {
		dc1 = DC(p1); dc2 = DC(p2);
		deg1 = DEG(dc1); deg2 = DEG(dc2);
		sgn = cmpq(deg1,deg2);
		if ( sgn == 0 ) {
			DIVSP(vl,COEF(dc1),COEF(dc2),q);
			MULP(vl,p2,*q,&m); SUBP(vl,p1,m,r); 
		} else if ( sgn < 0 ) {
			*q = 0; *r = p1;
		} else {	
			if ( (PL(NM(deg1)) > 1) ) 
				error("divsrp : invalid input");
			d1 = QTOS(deg1); d2 = QTOS(deg2); 
			W_CALLOC(d1-d2,P,pq); W_CALLOC(d1,P,pr); W_CALLOC(d2,P,pd);
			for ( dc = dc1; dc; dc = NEXT(dc) ) 
				pr[QTOS(DEG(dc))] = COEF(dc);
			for ( dc = dc2; dc; dc = NEXT(dc) ) 
				pd[QTOS(DEG(dc))] = COEF(dc);
			for ( dvr = COEF(dc2), i = d1 - d2; i >= 0; i-- ) {
				if ( !pr[i+d2] ) 
					continue;
				DIVSP(vl,pr[i+d2],dvr,&pq[i]);
				for ( j = d2; j >= 0; j-- ) {
					MULP(vl,pq[i],pd[j],&m);
					SUBP(vl,pr[i + j],m,&s); pr[i + j] = s;
				}
			}
			plisttop(pq,v1,d1 - d2,q); plisttop(pr,v1,d1 - 1,r);
		}
	} else {
		for ( ; (v1 != vl->v) && (v2 != vl->v); vl = NEXT(vl) );
		if ( v2 == vl->v ) {
			*q = 0; *r = p1;
		} else
			DIVSRDCP(vl,p1,p2,q,r);
	}
}

void D_DIVSRDCP(vl,p1,p2,q,r)
VL vl;
P p1,p2,*q,*r;
{

	P qc,rc;
	DCP dc,dcq,dcq0,dcr,dcr0;

	for ( dc = DC(p1), dcq0 = 0, dcr0 = 0; dc; dc = NEXT(dc) ) {
		DIVSRP(vl,COEF(dc),p2,&qc,&rc);
		if ( qc ) {
			NEXTDC(dcq0,dcq); DEG(dcq) = DEG(dc); COEF(dcq) = qc;
		}
		if ( rc ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = rc;
		}
	}
	if ( dcq0 ) {
		NEXT(dcq) = 0; MKP(VR(p1),dcq0,*q);
	} else
		*q = 0;
	if ( dcr0 ) {
		NEXT(dcr) = 0; MKP(VR(p1),dcr0,*r);
	} else
		*r = 0;
}

void D_DIVSP(vl,p1,p2,q)
VL vl;
P p1,p2,*q;
{
	P t;

	DIVSRP(vl,p1,p2,q,&t);
	if ( t ) 
		error("divsp: cannot happen");
}

void D_DIVSDCP(vl,p1,p2,q)
VL vl;
P p1,p2,*q;
{

	P m;
	register DCP dc,dcr,dcr0;

	for ( dc = DC(p1), dcr0 = 0; dc; dc = NEXT(dc) ) {
		DIVSP(vl,COEF(dc),p2,&m);
		NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = m; NEXT(dcr) = 0;
	}
	MKP(VR(p1),dcr0,*q);
}

#ifdef FBASE
void plisttop(f,v,n,gp)
P *f;
V v;
int n;
P *gp;
{
	int i;
	DCP dc,dc0;

	for ( i = n; (i >= 0) && !f[i]; i-- );
	if ( i < 0 ) 
		*gp = 0;
	else if ( i == 0 )
		*gp = f[0];
	else {
		for ( dc0 = 0; i >= 0; i-- ) {
			if ( !f[i] ) 
				continue;
			NEXTDC(dc0,dc);
			if ( i ) 
				STOQ(i,DEG(dc));
			else 
				DEG(dc) = 0;
			COEF(dc) = f[i];
		}
		NEXT(dc) = 0; MKP(v,dc0,*gp);	
	}
}

/* for multivariate polynomials over fields */

int divtp(vl,p1,p2,q)
VL vl;
P p1,p2,*q;
{
	register int i,j,k;
	register DCP dc1,dc2,dc;
	P m,m1,s,dvr,t;
	P *pq,*pr,*pd;
	V v1,v2;
	Q deg1,deg2;
	int d1,d2,sgn;

	if ( !p1 ) {
		*q = 0;
		return 1;
	} else if ( NUM(p2) ) {
		divsp(vl,p1,p2,q);
		return 1;
	} else if ( NUM(p1) ) {
		*q = 0;
		return 0;
	} else if ( ( v1 = VR(p1) ) == ( v2 = VR(p2) ) ) {
		dc1 = DC(p1); dc2 = DC(p2);
		deg1 = DEG(dc1); deg2 = DEG(dc2);
		sgn = cmpq(deg1,deg2);
		if ( sgn == 0 )
			if ( !divtp(vl,COEF(dc1),COEF(dc2),&m) ) {
				*q = 0;
				return 0;
			} else {
				mulp(vl,p2,m,&m1); subp(vl,p1,m1,&s); 
				if ( !s ) {
					*q = m;
					return 1;
				} else {
					*q = 0;
					return 0;
				}
			}	
		else if ( sgn < 0 ) {
			*q = 0;
			return 0;
		} else {
			if ( (PL(NM(deg1)) > 1) ) {
				error("divtp : invalid input");
				*q = 0;
				return ( 0 );
			}
			d1 = QTOS(deg1); d2 = QTOS(deg2); 
			W_CALLOC(d1-d2,P,pq); W_CALLOC(d1,P,pr); W_CALLOC(d2,P,pd);
			for ( dc = dc1; dc; dc = NEXT(dc) ) 
				pr[QTOS(DEG(dc))] = COEF(dc);
			for ( dc = dc2; dc; dc = NEXT(dc) ) 
				pd[QTOS(DEG(dc))] = COEF(dc);
			for ( dvr = COEF(dc2), i = d1 - d2; i >= 0; i-- ) 
				if ( !pr[i+d2] ) 
					continue;
				else if ( !divtp(vl,pr[i+d2],dvr,&m) ) {
					*q = 0;
					return 0;
				} else {
					pq[i] = m;
					for ( j = d2; j >= 0; j-- ) {
						mulp(vl,pq[i],pd[j],&m);
						subp(vl,pr[i + j],m,&s); pr[i + j] = s;
					}
				}
			plisttop(pq,v1,d1 - d2,&m); plisttop(pr,v1,d1 - 1,&t);
			if ( t ) {
				*q = 0;
				return 0;
			} else {
				*q = m;
				return 1;
			}
		}
	} else {
		for ( ; (v1 != vl->v) && (v2 != vl->v); vl = NEXT(vl) );
		if ( v2 == vl->v ) {
			*q = 0;
			return 0;
		} else 
			return divtdcp(vl,p1,p2,q);
	}
}

int divtdcp(vl,p1,p2,q)
VL vl;
P p1,p2,*q;
{

	P m;
	register DCP dc,dcr,dcr0,dct;

	for ( dc = DC(p1), dcr0 = 0; dc; dc = NEXT(dc) ) 
		if ( !divtp(vl,COEF(dc),p2,&m) ) {
			*q = 0;
			return 0;
		} else {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = m; NEXT(dcr) = 0;
		}
	MKP(VR(p1),dcr0,*q);
	return 1;
}

int divtpz(vl,p1,p2,q)
VL vl;
P p1,p2,*q;
{
	register int i,j;
	register DCP dc1,dc2,dc;
	P m,m1,s,dvr,t;
	P *pq,*pr,*pd;
	V v1,v2;
	Q deg1,deg2;
	int d1,d2,sgn;

	if ( !p1 ) {
		*q = 0;
		return ( 1 );
	} else if ( NUM(p2) ) 
		if ( NUM(p1) ) {
			divq((Q)p1,(Q)p2,(Q *)&s);
			if ( INT((Q)s) ) {
				*q = s;
				return ( 1 );
			} else {
				*q = 0;
				return ( 0 );
			}
		} else 
			return ( divtdcpz(vl,p1,p2,q) );
	else if ( NUM(p1) ) {
		*q = 0;
		return ( 0 );
	} else if ( ( v1 = VR(p1) ) == ( v2 = VR(p2) ) ) {
		Q csum1,csum2;

		csump(vl,p1,&csum1); csump(vl,p2,&csum2);
		if ( csum2 && !divtpz(vl,(P)csum1,(P)csum2,&t) ) {
			*q = 0;
			return 0;
		}
		dc1 = DC(p1); dc2 = DC(p2);
		deg1 = DEG(dc1); deg2 = DEG(dc2);
		sgn = cmpq(deg1,deg2);
		if ( sgn == 0 )
			if ( !divtpz(vl,COEF(dc1),COEF(dc2),&m) ) {
				*q = 0;
				return ( 0 );
			} else {
				mulp(vl,p2,m,&m1); subp(vl,p1,m1,&s); 
				if ( !s ) {
					*q = m;
					return ( 1 );
				} else {
					*q = 0;
					return ( 0 );
				}
			}	
		else if ( sgn < 0 ) {
			*q = 0;
			return ( 0 );
		} else {
			if ( (PL(NM(deg1)) > 1) ) {
				error("divtpz : invalid input");
				*q = 0;
				return ( 0 );
			}
			d1 = QTOS(deg1); d2 = QTOS(deg2); 
			W_CALLOC(d1-d2,P,pq); W_CALLOC(d1,P,pr); W_CALLOC(d2,P,pd);
			for ( dc = dc1; dc; dc = NEXT(dc) ) 
				pr[QTOS(DEG(dc))] = COEF(dc);
			for ( dc = dc2; dc; dc = NEXT(dc) ) 
				pd[QTOS(DEG(dc))] = COEF(dc);
			for ( dvr = COEF(dc2), i = d1 - d2; i >= 0; i-- ) 
				if ( !pr[i+d2] ) 
					continue;
				else if ( !divtpz(vl,pr[i+d2],dvr,&m) ) {
					*q = 0;
					return ( 0 );
				} else {
					pq[i] = m;
					for ( j = d2; j >= 0; j-- ) {
						mulp(vl,pq[i],pd[j],&m);
						subp(vl,pr[i + j],m,&s); pr[i + j] = s;
					}
				}
			plisttop(pq,v1,d1 - d2,&m); plisttop(pr,v1,d1 - 1,&t);
			if ( t ) {
				*q = 0;
				return ( 0 );
			} else {
				*q = m;
				return ( 1 );
			}
		}
	} else {
		for ( ; (v1 != vl->v) && (v2 != vl->v); vl = NEXT(vl) );
		if ( v2 == vl->v ) {
			*q = 0;
			return ( 0 );
		} else 
			return ( divtdcpz(vl,p1,p2,q) ) ;
	}
}

int divtdcpz(vl,p1,p2,q)
VL vl;
P p1,p2,*q;
{

	P m;
	register DCP dc,dcr,dcr0;

	for ( dc = DC(p1), dcr0 = 0; dc; dc = NEXT(dc) ) 
		if ( !divtpz(vl,COEF(dc),p2,&m) ) {
			*q = 0;
			return ( 0 );
		} else {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = m; NEXT(dcr) = 0;
		}
	MKP(VR(p1),dcr0,*q);
	return ( 1 );
}

void udivpz(f1,f2,fqp,frp)
P f1,f2,*fqp,*frp;
{
	register int n1,n2,i,j;
	Q *pq,*pr,*pd,d,m,s,q;
	DCP dc;
	N qn,rn;

	if ( !f2 )
		error("udivpz: division by 0");
	else if ( !f1 ) {
		*fqp = *frp = 0;
		return;
	} else if ( NUM(f1) ) 
		if ( NUM(f2) ) {
			divn(NM((Q)f1),NM((Q)f2),&qn,&rn);
			if ( rn ) {
				*fqp = *frp = 0;
			} else {
				NTOQ(qn,SGN((Q)f1)*SGN((Q)f2),q),*fqp = (P)q; *frp = 0;
			}
			return;
		} else {
			*fqp = 0; *frp = f1;
			return;
		}
	else if ( NUM(f2) ) {
		n1 = UDEG(f1); W_CALLOC(n1,Q,pq);
		for ( dc = DC(f1); dc; dc = NEXT(dc) ) {
			divn(NM((Q)COEF(dc)),NM((Q)f2),&qn,&rn);
			if ( rn ) {
				*fqp = *frp = 0;
				return;
			} else {
				NTOQ(qn,SGN((Q)COEF(dc))*SGN((Q)f2),s); pq[QTOS(DEG(dc))] = s;
			}
		}
		plisttop((P *)pq,VR(f1),n1,fqp);
		return;
	}
	n1  = UDEG(f1); n2  = UDEG(f2);
	if ( n1 < n2 ) {
		*fqp = NULL; *frp = f1;
		return;
	}
	W_CALLOC(n1-n2,Q,pq); W_CALLOC(n1,Q,pr); W_CALLOC(n2,Q,pd);
	for ( dc = DC(f1); dc; dc = NEXT(dc) )
		pr[QTOS(DEG(dc))] = (Q)COEF(dc);
	for ( dc = DC(f2); dc; dc = NEXT(dc) )
		pd[QTOS(DEG(dc))] = (Q)COEF(dc);
	for ( d = (Q)UCOEF(f2), i = n1 - n2; i >= 0; i-- ) {
		if ( !pr[i+n2] ) 
			continue;
		divn(NM(pr[i+n2]),NM(d),&qn,&rn);
		if ( rn ) {
			*fqp = *frp = 0;
			return;
		}
		NTOQ(qn,SGN(pr[i+n2])*SGN(d),pq[i]);
		for ( j = n2; j >= 0; j-- ) {
			mulq(pq[i],pd[j],&m); subq(pr[i+j],m,&s); pr[i+j] = s;
		}
	}
	plisttop((P *)pq,VR(f1),n1-n2,fqp); plisttop((P *)pr,VR(f1),n2-1,frp);
}

void udivpwm(mod,p1,p2,q,r)
Q mod;
P p1,p2;
P *q,*r;
{
	P s,t,u,tq,tr;

	invl((Q)UCOEF(p2),mod,(Q *)&t); mulpq(p2,t,&s); cmp(mod,s,&u); 
	udivpzwm(mod,p1,u,&tq,&tr); 
	cmp(mod,tr,r); mulpq(tq,t,&s); cmp(mod,s,q); 
}

void udivpzwm(mod,f1,f2,fqp,frp)
Q mod;
P f1,f2,*fqp,*frp;
{
	register int n1,n2,i,j;
	Q *pq,*pr,*pd,d,m,s,q;
	DCP dc;
	N qn,rn;

	if ( !f2 )
		error("udivpz: division by 0");
	else if ( !f1 ) {
		*fqp = *frp = 0;
		return;
	} else if ( NUM(f1) ) 
		if ( NUM(f2) ) {
			divn(NM((Q)f1),NM((Q)f2),&qn,&rn);
			if ( rn ) {
				*fqp = *frp = 0;
			} else {
				NTOQ(qn,SGN((Q)f1)*SGN((Q)f2),q),*fqp = (P)q; *frp = 0;
			}
			return;
		} else {
			*fqp = 0; *frp = f1;
			return;
		}
	else if ( NUM(f2) ) {
		n1 = UDEG(f1); W_CALLOC(n1,Q,pq);
		for ( dc = DC(f1); dc; dc = NEXT(dc) ) {
			divn(NM((Q)COEF(dc)),NM((Q)f2),&qn,&rn);
			if ( rn ) {
				*fqp = *frp = 0;
				return;
			} else {
				NTOQ(qn,SGN((Q)COEF(dc))*SGN((Q)f2),s); pq[QTOS(DEG(dc))] = s;
			}
		}
		plisttop((P *)pq,VR(f1),n1,fqp);
		return;
	}
	n1  = UDEG(f1); n2  = UDEG(f2);
	if ( n1 < n2 ) {
		*fqp = NULL; *frp = f1;
		return;
	}
	W_CALLOC(n1-n2,Q,pq); W_CALLOC(n1,Q,pr); W_CALLOC(n2,Q,pd);
	for ( dc = DC(f1); dc; dc = NEXT(dc) )
		pr[QTOS(DEG(dc))] = (Q)COEF(dc);
	for ( dc = DC(f2); dc; dc = NEXT(dc) )
		pd[QTOS(DEG(dc))] = (Q)COEF(dc);
	for ( d = (Q)UCOEF(f2), i = n1 - n2; i >= 0; i-- ) {
		if ( !pr[i+n2] ) 
			continue;
		divn(NM(pr[i+n2]),NM(d),&qn,&rn);
		if ( rn ) {
			*fqp = *frp = 0;
			return;
		}
		NTOQ(qn,SGN(pr[i+n2])*SGN(d),pq[i]);
		for ( j = n2; j >= 0; j-- ) {
			mulq(pq[i],pd[j],&m); remq(m,mod,&s); 
			subq(pr[i+j],s,&m); remq(m,mod,&s); pr[i+j] = s;
		}
	}
	plisttop((P *)pq,VR(f1),n1-n2,fqp); plisttop((P *)pr,VR(f1),n2-1,frp);
}
#endif
