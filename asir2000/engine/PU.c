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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/PU.c,v 1.11 2004/09/14 07:23:34 noro Exp $ 
*/
#include "ca.h"

void reorderp(VL nvl,VL ovl,P p,P *pr)
{
	DCP dc;
	P x,m,s,t,c;
	
	if ( !p ) 
		*pr = 0;
	else if ( NUM(p) ) 
		*pr = p;
	else {
		MKV(VR(p),x);
		for ( s = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			reorderp(nvl,ovl,COEF(dc),&c);
			if ( DEG(dc) ) {
				pwrp(nvl,x,DEG(dc),&t); mulp(nvl,c,t,&m); 
				addp(nvl,m,s,&t); 
			} else 
				addp(nvl,s,c,&t);
			s = t;
		}
		*pr = s;
	}
}
			
void substp(VL vl,P p,V v0,P p0,P *pr)
{
	P x,t,m,c,s,a;
	DCP dc;
	Q d;

	if ( !p ) 
		*pr = 0;
	else if ( NUM(p) ) 
		*pr = p;
	else if ( VR(p) != v0 ) {
		MKV(VR(p),x);
		for ( c = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			substp(vl,COEF(dc),v0,p0,&t);
			if ( DEG(dc) ) {
				pwrp(vl,x,DEG(dc),&s); mulp(vl,s,t,&m); 
				addp(vl,m,c,&a); 
				c = a;
			} else {
				addp(vl,t,c,&a); 
				c = a;
			}
		}
		*pr = c;
	} else {
		dc = DC(p);
		c = COEF(dc);
		for ( d = DEG(dc), dc = NEXT(dc); 
			dc; d = DEG(dc), dc = NEXT(dc) ) {
				subq(d,DEG(dc),(Q *)&t); pwrp(vl,p0,(Q)t,&s); 
				mulp(vl,s,c,&m); 
				addp(vl,m,COEF(dc),&c); 
		}
		if ( d ) {
			pwrp(vl,p0,d,&t); mulp(vl,t,c,&m); 
			c = m;
		}
		*pr = c;
	}
}

void detp(VL vl,P **rmat,int n,P *dp)
{
	int i,j,k,l,sgn,nmin,kmin,lmin,ntmp;
	P mjj,mij,t,s,u,d;
	P **mat;
	P *mi,*mj;

	mat = (P **)almat_pointer(n,n);
	for ( i = 0; i < n; i++ )
		for ( j = 0; j < n; j++ )
			mat[i][j] = rmat[i][j];
	for ( j = 0, d = (P)ONE, sgn = 1; j < n; j++ ) {
		for ( i = j; (i < n) && !mat[i][j]; i++ );
		if ( i == n ) {
			*dp = 0; return;
		}
		nmin = nmonop(mat[i][j]);
		kmin=i; lmin=j;
		for ( k = j; k < n; k++ )
			for ( l = j; l < n; l++ )
				if ( mat[k][l] && ((ntmp=nmonop(mat[k][l])) < nmin) ) {
					kmin = k; lmin = l; nmin = ntmp;
				}
		if ( kmin != j ) {
			mj = mat[j]; mat[j] = mat[kmin]; mat[kmin] = mj; sgn = -sgn;
		}
		if ( lmin != j ) {
			for ( k = j; k < n; k++ ) {
				t = mat[k][j]; mat[k][j] = mat[k][lmin]; mat[k][lmin] = t;
			}
			sgn = -sgn;
		}
		for ( i = j + 1, mj = mat[j], mjj = mj[j]; i < n; i++ )
			for ( k = j + 1, mi = mat[i], mij = mi[j]; k < n; k++ ) {
				mulp(vl,mi[k],mjj,&t); mulp(vl,mj[k],mij,&s);
				subp(vl,t,s,&u); divsp(vl,u,d,&mi[k]);
			}
		d = mjj;
	}
	if ( sgn < 0 )
		chsgnp(d,dp);
	else
		*dp = d;
}

void invmatp(VL vl,P **rmat,int n,P ***imatp,P *dnp)
{
	int i,j,k,l,n2;
	P mjj,mij,t,s,u,d;
	P **mat,**imat;
	P *mi,*mj,*w;
	
	n2 = n<<1;
	mat = (P **)almat_pointer(n,n2);
	for ( i = 0; i < n; i++ ) {
		for ( j = 0; j < n; j++ )
			mat[i][j] = rmat[i][j];
		mat[i][i+n] = (P)ONE;
	}
	for ( j = 0, d = (P)ONE; j < n; j++ ) {
		for ( i = j; (i < n) && !mat[i][j]; i++ );
		if ( i == n ) {
			error("invmatp : input is singular");
		}
		for ( k = i; k < n; k++ )
			if ( mat[k][j] && (nmonop(mat[k][j]) < nmonop(mat[i][j]) ) )
				i = k;
		if ( j != i ) {
			mj = mat[j]; mat[j] = mat[i]; mat[i] = mj;
		}
		for ( i = j + 1, mj = mat[j], mjj = mj[j]; i < n; i++ )
			for ( k = j + 1, mi = mat[i], mij = mi[j]; k < n2; k++ ) {
				mulp(vl,mi[k],mjj,&t); mulp(vl,mj[k],mij,&s);
				subp(vl,t,s,&u); divsp(vl,u,d,&mi[k]);
			}
		d = mjj;
	}
	/* backward substitution */
	w = (P *)ALLOCA(n2*sizeof(P));
	for ( i = n-2; i >= 0; i-- ) {
		bzero(w,n2*sizeof(P));
		for ( k = i+1; k < n; k++ )
			for ( l = k, u = mat[i][l]; l < n2; l++ ) {
				mulp(vl,mat[k][l],u,&t); addp(vl,w[l],t,&s); w[l] = s;
			}
		for ( j = i, u = mat[i][j]; j < n2; j++ ) {
			mulp(vl,mat[i][j],d,&t); subp(vl,t,w[j],&s);
			divsp(vl,s,u,&mat[i][j]);
		}
	}
	imat = (P **)almat_pointer(n,n);
	for ( i = 0; i < n; i++ )
		for ( j = 0; j < n; j++ )
			imat[i][j] = mat[i][j+n];
	*imatp = imat;
	*dnp = d;
}

void reordvar(VL vl,V v,VL *nvlp)
{
	VL nvl,nvl0;

	for ( NEWVL(nvl0), nvl0->v = v, nvl = nvl0; 
		vl; vl = NEXT(vl) )
		if ( vl->v == v ) 
			continue;
		else {
			NEWVL(NEXT(nvl));
			nvl = NEXT(nvl);
			nvl->v = vl->v;
		}
	NEXT(nvl) = 0;
	*nvlp = nvl0;
}

void gcdprsp(VL vl,P p1,P p2,P *pr)
{
	P g1,g2,gc1,gc2,gp1,gp2,g,gc,gp,gcr;
	V v1,v2;

	if ( !p1 ) 
		ptozp0(p2,pr);	
	else if ( !p2 ) 
		ptozp0(p1,pr);	
	else if ( NUM(p1) || NUM(p2) ) 
		*pr = (P)ONE;
	else {
		ptozp0(p1,&g1); ptozp0(p2,&g2);
		if ( ( v1 = VR(g1) ) == ( v2 = VR(g2) ) ) {
			gcdcp(vl,g1,&gc1); divsp(vl,g1,gc1,&gp1); 
			gcdcp(vl,g2,&gc2); divsp(vl,g2,gc2,&gp2); 
			gcdprsp(vl,gc1,gc2,&gcr); 
			sprs(vl,v1,gp1,gp2,&g); 

			if ( VR(g) == v1 ) {
				ptozp0(g,&gp);
				gcdcp(vl,gp,&gc); divsp(vl,gp,gc,&gp1);
				mulp(vl,gp1,gcr,pr); 

			} else
				*pr = gcr;
		} else {
			while ( v1 != vl->v && v2 != vl->v ) 
				vl = NEXT(vl);
			if ( v1 == vl->v ) {
				gcdcp(vl,g1,&gc1); gcdprsp(vl,gc1,g2,pr);
			} else {
				gcdcp(vl,g2,&gc2); gcdprsp(vl,gc2,g1,pr);
			}
		}
	}
}

void gcdcp(VL vl,P p,P *pr)
{
	P g,g1;
	DCP dc;

	if ( NUM(p) ) 
		*pr = (P)ONE;
	else {
		dc = DC(p);
		g = COEF(dc);
		for ( dc = NEXT(dc); dc; dc = NEXT(dc) ) {
			gcdprsp(vl,g,COEF(dc),&g1); 
			g = g1;
		}
		*pr = g;
	}
}

void sprs(VL vl,V v,P p1,P p2,P *pr)
{
	P q1,q2,m,m1,m2,x,h,r,g1,g2;
	int d;
	Q dq;
	VL nvl;

	reordvar(vl,v,&nvl);
	reorderp(nvl,vl,p1,&q1); reorderp(nvl,vl,p2,&q2);

	if ( ( VR(q1) != v ) || ( VR(q2) != v ) ) {
		*pr = 0;
		return;
	}

	if ( deg(v,q1) >= deg(v,q2) ) {
		g1 = q1; g2 = q2;
	} else {
		g2 = q1; g1 = q2;
	}
	
	for ( h = (P)ONE, x = (P)ONE; ; ) {
		if ( !deg(v,g2) ) 
			break;

		premp(nvl,g1,g2,&r);
		if ( !r ) 
			break;

		d = deg(v,g1) - deg(v,g2); STOQ(d,dq);
		pwrp(nvl,h,dq,&m); mulp(nvl,m,x,&m1); g1 = g2;
		divsp(nvl,r,m1,&g2); x = LC(g1); /* g1 is not const w.r.t v */
		pwrp(nvl,x,dq,&m1); mulp(nvl,m1,h,&m2); 
		divsp(nvl,m2,m,&h); 
	}
	*pr = g2;
}

void resultp(VL vl,V v,P p1,P p2,P *pr)
{
	P q1,q2,m,m1,m2,lc,q,r,t,g1,g2,adj;
	int d,d1,d2,j,k;
	VL nvl;
	Q dq;

	if ( !p1 || !p2 ) {
		*pr = 0; return;
	}
	reordvar(vl,v,&nvl);
	reorderp(nvl,vl,p1,&q1); reorderp(nvl,vl,p2,&q2);

	if ( VR(q1) != v )
		if ( VR(q2) != v ) {
			*pr = 0;
			return;
		} else {
			d = deg(v,q2); STOQ(d,dq);
			pwrp(vl,q1,dq,pr); 
			return;
		}
	else if ( VR(q2) != v ) {
		d = deg(v,q1); STOQ(d,dq);
		pwrp(vl,q2,dq,pr); 
		return;
	} 
		
	if ( ( VR(q1) != v ) || ( VR(q2) != v ) ) {
		*pr = 0;
		return;
	} 

	d1 = deg(v,q1); d2 = deg(v,q2);
	if ( d1 > d2 ) {
		g1 = q1; g2 = q2; adj = (P)ONE;
	} else if ( d1 < d2 ) {
		g2 = q1; g1 = q2;
		if ( (d1 % 2) && (d2 % 2) ) {
			STOQ(-1,dq); adj = (P)dq;
		} else
			adj = (P)ONE;
	} else {
		premp(nvl,q1,q2,&t);
		d = deg(v,t); STOQ(d,dq); pwrp(nvl,LC(q2),dq,&adj); 
		g1 = q2; g2 = t;
		if ( d1 % 2 ) {
			chsgnp(adj,&t); adj = t;
		}
	}
	d1 = deg(v,g1); j = d1 - 1;

	for ( lc = (P)ONE; ; ) {
		if ( ( k = deg(v,g2) ) < 0 ) {
			*pr = 0;
			return;
		}

		if ( k == j )
			if ( !k ) {
				divsp(nvl,g2,adj,pr); 
				return;
			} else {
				premp(nvl,g1,g2,&r); mulp(nvl,lc,lc,&m);
				divsp(nvl,r,m,&q); 
				g1 = g2; g2 = q;
				lc = LC(g1); /* g1 is not const */
				j = k - 1;
			}
		else {
			d = j - k; STOQ(d,dq);
			pwrp(nvl,(VR(g2)==v?LC(g2):g2),dq,&m);
			mulp(nvl,g2,m,&m1); 
			pwrp(nvl,lc,dq,&m); divsp(nvl,m1,m,&t); 
			if ( k == 0 ) {
				divsp(nvl,t,adj,pr); 
				return;
			} else {
				premp(nvl,g1,g2,&r);
				mulp(nvl,lc,lc,&m1); mulp(nvl,m,m1,&m2); 
				divsp(nvl,r,m2,&q);
				g1 = t; g2 = q;
				if ( d % 2 ) {
					chsgnp(g2,&t); g2 = t;
				}
				lc = LC(g1); /* g1 is not const */
				j = k - 1;
			}
		}
	}
}

void srch2(VL vl,V v,P p1,P p2,P *pr)
{
	P q1,q2,m,m1,m2,lc,q,r,t,s,g1,g2,adj;
	int d,d1,d2,j,k;
	VL nvl;
	Q dq;

	reordvar(vl,v,&nvl);
	reorderp(nvl,vl,p1,&q1); reorderp(nvl,vl,p2,&q2);

	if ( VR(q1) != v )
		if ( VR(q2) != v ) {
			*pr = 0;
			return;
		} else {
			d = deg(v,q2); STOQ(d,dq);
			pwrp(vl,q1,dq,pr); 
			return;
		}
	else if ( VR(q2) != v ) {
		d = deg(v,q1); STOQ(d,dq);
		pwrp(vl,q2,dq,pr); 
		return;
	} 
		
	if ( ( VR(q1) != v ) || ( VR(q2) != v ) ) {
		*pr = 0;
		return;
	} 

	if ( deg(v,q1) >= deg(v,q2) ) {
		g1 = q1; g2 = q2;
	} else {
		g2 = q1; g1 = q2;
	}

	
	if ( ( d1 = deg(v,g1) ) > ( d2 = deg(v,g2) ) ) {
		j = d1 - 1;
		adj = (P)ONE;
	} else {
		premp(nvl,g1,g2,&t); 
		d = deg(v,t); STOQ(d,dq);
		pwrp(nvl,LC(g2),dq,&adj); 
		g1 = g2; g2 = t;
		j = deg(v,g1) - 1;
	}

	for ( lc = (P)ONE; ; ) {
		if ( ( k = deg(v,g2) ) < 0 ) {
			*pr = 0;
			return;
		}

		ptozp(g1,1,(Q *)&t,&s); g1 = s; ptozp(g2,1,(Q *)&t,&s); g2 = s;
		if ( k == j )
			if ( !k ) {
				divsp(nvl,g2,adj,pr); 
				return;
			} else {
				premp(nvl,g1,g2,&r); mulp(nvl,lc,lc,&m);
				divsp(nvl,r,m,&q); 
				g1 = g2; g2 = q;
				lc = LC(g1); /* g1 is not const */
				j = k - 1;
			}
		else {
			d = j - k; STOQ(d,dq);
			pwrp(nvl,(VR(g2)==v?LC(g2):g2),dq,&m);
			mulp(nvl,g2,m,&m1); 
			pwrp(nvl,lc,dq,&m); divsp(nvl,m1,m,&t); 
			if ( k == 0 ) {
				divsp(nvl,t,adj,pr); 
				return;
			} else {
				premp(nvl,g1,g2,&r);
				mulp(nvl,lc,lc,&m1); mulp(nvl,m,m1,&m2); 
				divsp(nvl,r,m2,&q);
				g1 = t; g2 = q;
				if ( d % 2 ) {
					chsgnp(g2,&t); g2 = t;
				}
				lc = LC(g1); /* g1 is not const */
				j = k - 1;
			}
		}
	}
}

void srcr(VL vl,V v,P p1,P p2,P *pr)
{
	P q1,q2,c,c1;
	P tg,tg1,tg2,resg;
	int index,mod;
	Q a,b,f,q,t,s,u,n,m;
	VL nvl;

	reordvar(vl,v,&nvl);
	reorderp(nvl,vl,p1,&q1); reorderp(nvl,vl,p2,&q2);

	if ( ( VR(q1) != v ) && ( VR(q2) != v ) ) {
		*pr = 0;
		return;
	} 
	if ( VR(q1) != v ) {
		pwrp(vl,q1,DEG(DC(q2)),pr);
		return;
	} 
	if ( VR(q2) != v ) {
		pwrp(vl,q2,DEG(DC(q1)),pr);
		return;
	} 
	norm1c(q1,&a); norm1c(q2,&b);
	n = DEG(DC(q1)); m = DEG(DC(q2));
	pwrq(a,m,&t); pwrq(b,n,&s); mulq(t,s,&u);
	factorial(QTOS(n)+QTOS(m),&t);
	mulq(u,t,&s); addq(s,s,&f);
	for ( index = 0, q = ONE, c = 0; cmpq(f,q) >= 0; ) {
		mod = get_lprime(index++);
		ptomp(mod,LC(q1),&tg);
		if ( !tg ) 
			continue;
		ptomp(mod,LC(q2),&tg);
		if ( !tg ) 
			continue;
		ptomp(mod,q1,&tg1); ptomp(mod,q2,&tg2);
		srchmp(nvl,mod,v,tg1,tg2,&resg);
		chnremp(nvl,mod,c,q,resg,&c1); c = c1;
		STOQ(mod,t); mulq(q,t,&s); q = s;
	}
	*pr = c;
}

void res_ch_det(VL vl,V v,P p1,P p2,P *pr)
{
	P q1,q2,c,c1;
	P tg,tg1,tg2,resg;
	int index,mod;
	Q a,b,f,q,t,s,u,n,m;
	VL nvl;

	reordvar(vl,v,&nvl);
	reorderp(nvl,vl,p1,&q1); reorderp(nvl,vl,p2,&q2);

	if ( ( VR(q1) != v ) && ( VR(q2) != v ) ) {
		*pr = 0;
		return;
	} 
	if ( VR(q1) != v ) {
		pwrp(vl,q1,DEG(DC(q2)),pr);
		return;
	} 
	if ( VR(q2) != v ) {
		pwrp(vl,q2,DEG(DC(q1)),pr);
		return;
	} 
	norm1c(q1,&a); norm1c(q2,&b);
	n = DEG(DC(q1)); m = DEG(DC(q2));
	pwrq(a,m,&t); pwrq(b,n,&s); mulq(t,s,&u);
	factorial(QTOS(n)+QTOS(m),&t);
	mulq(u,t,&s); addq(s,s,&f);
	for ( index = 0, q = ONE, c = 0; cmpq(f,q) >= 0; ) {
		mod = get_lprime(index++);
		ptomp(mod,LC(q1),&tg);
		if ( !tg ) 
			continue;
		ptomp(mod,LC(q2),&tg);
		if ( !tg ) 
			continue;
		ptomp(mod,q1,&tg1); ptomp(mod,q2,&tg2);
		res_detmp(nvl,mod,v,tg1,tg2,&resg);
		chnremp(nvl,mod,c,q,resg,&c1); c = c1;
		STOQ(mod,t); mulq(q,t,&s); q = s;
	}
	*pr = c;
}

void res_detmp(VL vl,int mod,V v,P p1,P p2,P *dp)
{
	int n1,n2,n,sgn;
	int i,j,k;
	P mjj,mij,t,s,u,d;
	P **mat;
	P *mi,*mj;
	DCP dc;

	n1 = UDEG(p1); n2 = UDEG(p2); n = n1+n2;
	mat = (P **)almat_pointer(n,n);
	for ( dc = DC(p1); dc; dc = NEXT(dc) )
		mat[0][n1-QTOS(DEG(dc))] = COEF(dc);
	for ( i = 1; i < n2; i++ )
		for ( j = 0; j <= n1; j++ )
			mat[i][i+j] = mat[0][j];
	for ( dc = DC(p2); dc; dc = NEXT(dc) )
		mat[n2][n2-QTOS(DEG(dc))] = COEF(dc);
	for ( i = 1; i < n1; i++ )
		for ( j = 0; j <= n2; j++ )
			mat[i+n2][i+j] = mat[n2][j];
	for ( j = 0, d = (P)ONEM, sgn = 1; j < n; j++ ) {
		for ( i = j; (i < n) && !mat[i][j]; i++ );
		if ( i == n ) {
			*dp = 0; return;
		}
		for ( k = i; k < n; k++ )
			if ( mat[k][j] && (nmonop(mat[k][j]) < nmonop(mat[i][j]) ) )
				i = k;
		if ( j != i ) {
			mj = mat[j]; mat[j] = mat[i]; mat[i] = mj; sgn = -sgn;
		}
		for ( i = j + 1, mj = mat[j], mjj = mj[j]; i < n; i++ )
			for ( k = j + 1, mi = mat[i], mij = mi[j]; k < n; k++ ) {
				mulmp(vl,mod,mi[k],mjj,&t); mulmp(vl,mod,mj[k],mij,&s);
				submp(vl,mod,t,s,&u); divsmp(vl,mod,u,d,&mi[k]);
			}
		d = mjj;
	}
	if ( sgn > 0 )
		*dp = d;
	else
		chsgnmp(mod,d,dp);
}

#if 0
showmat(VL vl,P **mat,int n)
{
	int i,j;
	P t;

	for ( i = 0; i < n; i++ ) {
		for ( j = 0; j < n; j++ ) {
			mptop(mat[i][j],&t); asir_printp(vl,t); fprintf(out," ");
		}
		fprintf(out,"\n");
	}
	fflush(out);
}

showmp(VL vl,P p)
{
	P t;

	mptop(p,&t); asir_printp(vl,t); fprintf(out,"\n");
}
#endif

void premp(VL vl,P p1,P p2,P *pr)
{
	P m,m1,m2;
	P *pw;
	DCP dc;
	V v1,v2;
	register int i,j;
	int n1,n2,d;

	if ( NUM(p1) ) 
		if ( NUM(p2) ) 
			*pr = 0;
		else 
			*pr = p1;
	else if ( NUM(p2) ) 
		*pr = 0;
	else if ( ( v1 = VR(p1) ) == ( v2 = VR(p2) ) ) {
		n1 = deg(v1,p1); n2 = deg(v1,p2);
		pw = (P *)ALLOCA((n1+1)*sizeof(P));
		bzero((char *)pw,(int)((n1+1)*sizeof(P)));

		for ( dc = DC(p1); dc; dc = NEXT(dc) )
			pw[QTOS(DEG(dc))] = COEF(dc);

		for ( i = n1; i >= n2; i-- ) {
			if ( pw[i] ) {
				m = pw[i];
				for ( j = i; j >= 0; j-- ) {
					mulp(vl,pw[j],LC(p2),&m1); pw[j] = m1;
				}

				for ( dc = DC(p2), d = i - n2; dc; dc = NEXT(dc) ) {
					mulp(vl,COEF(dc),m,&m1);
					subp(vl,pw[QTOS(DEG(dc))+d],m1,&m2); 
					pw[QTOS(DEG(dc))+d] = m2;
				}
			} else 
				for ( j = i; j >= 0; j-- ) 
					if ( pw[j] ) {
						mulp(vl,pw[j],LC(p2),&m1); pw[j] = m1;
					} 
		}
		plisttop(pw,v1,n2-1,pr);
	} else {
		while ( v1 != vl->v && v2 != vl->v ) 
			vl = NEXT(vl);
		if ( v1 == vl->v ) 
			*pr = 0;
		else 
			*pr = p1;
	}
}

void ptozp0(P p,P *pr)
{
	Q c;

	if ( qpcheck((Obj)p) )
		ptozp(p,1,&c,pr);
	else
		*pr = p;
}

void mindegp(VL vl,P p,VL *mvlp,P *pr)
{
	P t;
	VL nvl,tvl,avl;
	V v;
	int n,d;

	clctv(vl,p,&nvl); v = nvl->v; d = getdeg(nvl->v,p);
	for ( avl = NEXT(nvl); avl; avl = NEXT(avl) ) {
		n = getdeg(avl->v,p);
		if ( n < d ) {
			v = avl->v; d = n;
		} 
	}
	if ( v != nvl->v ) {
		reordvar(nvl,v,&tvl); reorderp(tvl,nvl,p,&t);
		*pr = t; *mvlp = tvl;
	} else {
		*pr = p; *mvlp = nvl;
	}
}

void maxdegp(VL vl,P p,VL *mvlp,P *pr)
{
	P t;
	VL nvl,tvl,avl;
	V v;
	int n,d;

	clctv(vl,p,&nvl); v = nvl->v; d = getdeg(nvl->v,p);
	for ( avl = NEXT(nvl); avl; avl = NEXT(avl) ) {
		n = getdeg(avl->v,p);
		if ( n > d ) {
			v = avl->v; d = n;
		} 
	}
	if ( v != nvl->v ) {
		reordvar(nvl,v,&tvl); reorderp(tvl,nvl,p,&t);
		*pr = t; *mvlp = tvl;
	} else {
		*pr = p; *mvlp = nvl;
	}
}

void min_common_vars_in_coefp(VL vl,P p,VL *mvlp,P *pr)
{
	P u,p0;
	VL tvl,cvl,svl,uvl,avl,vl0;
	int n,n0,d,d0;
	DCP dc;

	clctv(vl,p,&tvl); vl = tvl;
	for ( tvl = vl, n0 = 0; tvl; tvl = NEXT(tvl), n0++ );
	for ( avl = vl; avl; avl = NEXT(avl) ) {
		if ( VR(p) != avl->v ) {
			reordvar(vl,avl->v,&uvl); reorderp(uvl,vl,p,&u);
		} else {
			uvl = vl; u = p;
		}
		for ( cvl = NEXT(uvl), dc = DC(u); dc && cvl; dc = NEXT(dc) ) {
			clctv(uvl,COEF(dc),&tvl); intersectv(cvl,tvl,&svl); cvl = svl;
		}
		if ( !cvl ) {
			*mvlp = uvl; *pr = u; return;
		} else {
			for ( tvl = cvl, n = 0; tvl; tvl = NEXT(tvl), n++ );
			if ( n < n0 ) {
				vl0 = uvl; p0 = u; n0 = n; d0 = getdeg(uvl->v,u);
			} else if ( (n == n0) && ((d = getdeg(uvl->v,u)) < d0) ) {
					vl0 = uvl; p0 = u; n0 = n; d0 = d;
			}
		}
	}
	*pr = p0; *mvlp = vl0;
}

void minlcdegp(VL vl,P p,VL *mvlp,P *pr)
{
	P u,p0;
	VL tvl,uvl,avl,vl0;
	int d,d0;

	clctv(vl,p,&tvl); vl = tvl;
	vl0 = vl; p0 = p; d0 = homdeg(COEF(DC(p)));
	for ( avl = NEXT(vl); avl; avl = NEXT(avl) ) {
		reordvar(vl,avl->v,&uvl); reorderp(uvl,vl,p,&u);
		d = homdeg(COEF(DC(u)));
		if ( d < d0 ) {
			vl0 = uvl; p0 = u; d0 = d;
		}
	}
	*pr = p0; *mvlp = vl0;
}

void sort_by_deg(int n,P *p,P *pr)
{
	int j,k,d,k0;
	V v;

	for ( j = 0; j < n; j++ ) {
		for ( k0 = k = j, d = deg(v = VR(p[0]),p[j]); 
			k < n; k++ ) 
			if ( deg(v,p[k]) < d ) {
				k0 = k;
				d = deg(v,p[k]);
			}
		pr[j] = p[k0];
		if ( j != k0 ) 
			p[k0] = p[j];
	}
}

void sort_by_deg_rev(int n,P *p,P *pr)
{
	int j,k,d,k0;
	V v;

	for ( j = 0; j < n; j++ ) {
		for ( k0 = k = j, d = deg(v = VR(p[0]),p[j]); 
			k < n; k++ ) 
			if ( deg(v,p[k]) > d ) {
				k0 = k;
				d = deg(v,p[k]);
			}
		pr[j] = p[k0];
		if ( j != k0 ) 
			p[k0] = p[j];
	}
}


void getmindeg(V v,P p,Q *dp)
{
	Q dt,d;
	DCP dc;

	if ( !p || NUM(p) )
		*dp = 0;
	else if ( v == VR(p) ) {
		for ( dc = DC(p); NEXT(dc); dc = NEXT(dc) );
		*dp = DEG(dc);
	} else {
		dc = DC(p);
		getmindeg(v,COEF(dc),&d);
		for ( dc = NEXT(dc); dc; dc = NEXT(dc) ) {
			getmindeg(v,COEF(dc),&dt);
			if ( cmpq(dt,d) < 0 )
				d = dt;
		}
		*dp = d;
	}
}

void minchdegp(VL vl,P p,VL *mvlp,P *pr)
{
	P t;
	VL tvl,nvl,avl;
	int n,d,z;
	V v;

	if ( NUM(p) ) {
		*mvlp = vl;
		*pr = p;
		return;
	}
	clctv(vl,p,&nvl);
	v = nvl->v;
	d = getchomdeg(v,p) + getlchomdeg(v,p,&z);
	for ( avl = NEXT(nvl); avl; avl = NEXT(avl) ) {
		n = getchomdeg(avl->v,p) + getlchomdeg(avl->v,p,&z);
		if ( n < d ) {
			v = avl->v; d = n;
		} 
	}
	if ( v != nvl->v ) {
		reordvar(nvl,v,&tvl); reorderp(tvl,nvl,p,&t);
		*pr = t; *mvlp = tvl;
	} else {
		*pr = p; *mvlp = nvl;
	}
}

int getchomdeg(V v,P p)
{
	int m,m1;
	DCP dc;

	if ( !p || NUM(p) )
		return ( 0 );
	else if ( VR(p) == v ) 
		return ( dbound(v,p) );
	else {
		for ( dc = DC(p), m = 0; dc; dc = NEXT(dc) ) {
			m1 = getchomdeg(v,COEF(dc))+QTOS(DEG(dc));
			m = MAX(m,m1);
		}
		return ( m );
	}
}

int getlchomdeg(V v,P p,int *d)
{
	int m0,m1,d0,d1;
	DCP dc;

	if ( !p || NUM(p) ) {
		*d = 0;
		return ( 0 );
	} else if ( VR(p) == v ) {
		*d = QTOS(DEG(DC(p)));
		return ( homdeg(LC(p)) );
	} else {
		for ( dc = DC(p), m0 = 0, d0 = 0; dc; dc = NEXT(dc) ) {
			m1 = getlchomdeg(v,COEF(dc),&d1)+QTOS(DEG(dc));
			if ( d1 > d0 ) {
				m0 = m1;
				d0 = d1;
			} else if ( d1 == d0 ) 
				m0 = MAX(m0,m1);
		}
		*d = d0;
		return ( m0 );
	}
}

int nmonop(P p)
{
	int s;
	DCP dc;

	if ( !p )
		return 0;
	else
		switch ( OID(p) ) {
			case O_N:
				return 1; break;
			case O_P:
				for ( dc = DC((P)p), s = 0; dc; dc = NEXT(dc) )
					s += nmonop(COEF(dc));
				return s; break;
			default:
				return 0;
		}
}

int qpcheck(Obj p)
{
	DCP dc;

	if ( !p )
		return 1;
	else
		switch ( OID(p) ) {
			case O_N:
				return RATN((Num)p)?1:0;
			case O_P:
				for ( dc = DC((P)p); dc; dc = NEXT(dc) )
					if ( !qpcheck((Obj)COEF(dc)) )
						return 0;
				return 1;
			default:
				return 0;
		}
}

/* check if p is univariate and all coeffs are INT or LM */

int uzpcheck(Obj p)
{
	DCP dc;
	P c;

	if ( !p )
		return 1;
	else
		switch ( OID(p) ) {
			case O_N:
				return (RATN((Num)p)&&INT(p))||(NID((Num)p)==N_LM);
			case O_P:
				for ( dc = DC((P)p); dc; dc = NEXT(dc) ) {
					c = COEF(dc);
					if ( !NUM(c) || !uzpcheck((Obj)c) )
						return 0;
				}
				return 1;
			default:
				return 0;
		}
}

int p_mag(P p)
{
	int s;
	DCP dc;

	if ( !p )
		return 0;
	else if ( OID(p) == O_N )
		return n_bits(NM((Q)p))+(INT((Q)p)?0:n_bits(DN((Q)p)));
	else {
		for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) )
			s += p_mag(COEF(dc));
		return s;
	}
}

int maxblenp(P p)
{
	int s,t;
	DCP dc;

	if ( !p )
		return 0;
	else if ( OID(p) == O_N )
		return n_bits(NM((Q)p))+(INT((Q)p)?0:n_bits(DN((Q)p)));
	else {
		for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) ) {
			t = maxblenp(COEF(dc));
			s = MAX(t,s);
		}
		return s;
	}
}
