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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/distm.c,v 1.11 2003/07/18 10:13:13 noro Exp $ 
*/
#include "ca.h"
#include "inline.h"

extern int (*cmpdl)();
extern int do_weyl;

void ptomd(VL vl,int mod,VL dvl,P p,DP *pr)
{
	P t;

	ptomp(mod,p,&t);
	mptomd(vl,mod,dvl,t,pr);
}

void mptomd(VL vl,int mod,VL dvl,P p,DP *pr)
{
	int n,i;
	VL tvl;
	V v;
	DL d;
	MP m;
	DCP dc;
	DP r,s,t,u;
	P x,c;

	if ( !p )
		*pr = 0;
	else {
		for ( n = 0, tvl = dvl; tvl; tvl = NEXT(tvl), n++ );
		if ( NUM(p) ) {
			NEWDL(d,n);
			NEWMP(m); m->dl = d; C(m) = p; NEXT(m) = 0; MKDP(n,m,*pr);
		} else {
			for ( i = 0, tvl = dvl, v = VR(p);
				tvl && tvl->v != v; tvl = NEXT(tvl), i++ );
			if ( !tvl ) {
				for ( dc = DC(p), s = 0, MKV(v,x); dc; dc = NEXT(dc) ) {
					mptomd(vl,mod,dvl,COEF(dc),&t); pwrmp(vl,mod,x,DEG(dc),&c);
					mulmdc(vl,mod,t,c,&r); addmd(vl,mod,r,s,&t); s = t;
				}
				*pr = s;
			} else {
				for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) ) {
					mptomd(vl,mod,dvl,COEF(dc),&t);
					NEWDL(d,n); d->d[i] = QTOS(DEG(dc));
					d->td = MUL_WEIGHT(d->d[i],i);
					NEWMP(m); m->dl = d; C(m) = (P)ONEM; NEXT(m) = 0; MKDP(n,m,u);
					comm_mulmd(vl,mod,t,u,&r); addmd(vl,mod,r,s,&t); s = t;
				}
				*pr = s;
			}
		}
	}
}

void mdtop(VL vl,int mod,VL dvl,DP p,P *pr)
{
	int n,i;
	DL d;
	MP m;
	P r,s,t,u,w;
	Q q;
	VL tvl;

	if ( !p )
		*pr = 0;
	else {
		for ( n = p->nv, m = BDY(p), s = 0; m; m = NEXT(m) ) {
			for ( i = 0, t = C(m), d = m->dl, tvl = dvl;
				i < n; tvl = NEXT(tvl), i++ ) {
				MKMV(tvl->v,r); STOQ(d->d[i],q); pwrmp(vl,mod,r,q,&u);
				mulmp(vl,mod,t,u,&w); t = w;
			}
			addmp(vl,mod,s,t,&u); s = u;
		}
		mptop(s,pr);
	}
}

void addmd(VL vl,int mod,DP p1,DP p2,DP *pr)
{
	int n;
	MP m1,m2,mr,mr0;
	P t;
	int tmp;
	MQ q;

	if ( !p1 )
		*pr = p2;
	else if ( !p2 )
		*pr = p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; )
			switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
				case 0:
					if ( NUM(C(m1)) && NUM(C(m2)) ) {
						tmp = (CONT((MQ)C(m1))+CONT((MQ)C(m2))) - mod;
						if ( tmp < 0 )
							tmp += mod;
						if ( tmp ) {
							STOMQ(tmp,q); t = (P)q;
						} else
							t = 0;
					} else
						addmp(vl,mod,C(m1),C(m2),&t);
					if ( t ) {
						NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = t;
					}
					m1 = NEXT(m1); m2 = NEXT(m2); break;
				case 1:
					NEXTMP(mr0,mr); mr->dl = m1->dl; C(mr) = C(m1);
					m1 = NEXT(m1); break;
				case -1:
					NEXTMP(mr0,mr); mr->dl = m2->dl; C(mr) = C(m2);
					m2 = NEXT(m2); break;
			}
		if ( !mr0 )
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else {
				*pr = 0;
				return;
			}
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		MKDP(NV(p1),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = MAX(p1->sugar,p2->sugar);
	}
}

void submd(VL vl,int mod,DP p1,DP p2,DP *pr)
{
	DP t;

	if ( !p2 )
		*pr = p1;
	else {
		chsgnmd(mod,p2,&t); addmd(vl,mod,p1,t,pr);
	}
}

void chsgnmd(int mod,DP p,DP *pr)
{
	MP m,mr,mr0;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); chsgnmp(mod,C(m),&C(mr)); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void mulmd(VL vl,int mod,DP p1,DP p2,DP *pr)
{
	if ( !do_weyl )
		comm_mulmd(vl,mod,p1,p2,pr);
	else
		weyl_mulmd(vl,mod,p1,p2,pr);
}

void comm_mulmd(VL vl,int mod,DP p1,DP p2,DP *pr)
{
	MP m;
	DP s,t,u;
	int i,l,l1;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else if ( OID(p1) <= O_P )
		mulmdc(vl,mod,p2,(P)p1,pr);
	else if ( OID(p2) <= O_P )
		mulmdc(vl,mod,p1,(P)p2,pr);
	else {
		for ( m = BDY(p1), l1 = 0; m; m = NEXT(m), l1++ );
		for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
		if ( l1 < l ) {
			t = p1; p1 = p2; p2 = t;
			l = l1;
		}
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			mulmdm(vl,mod,p1,w[i],&t); addmd(vl,mod,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

void weyl_mulmd(VL vl,int mod,DP p1,DP p2,DP *pr)
{
	MP m;
	DP s,t,u;
	int i,l;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else if ( OID(p1) <= O_P )
		mulmdc(vl,mod,p2,(P)p1,pr);
	else if ( OID(p2) <= O_P )
		mulmdc(vl,mod,p1,(P)p2,pr);
	else {
		for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			weyl_mulmdm(vl,mod,p1,w[i],&t); addmd(vl,mod,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

void mulmdm(VL vl,int mod,DP p,MP m0,DP *pr)
{
	MP m,mr,mr0;
	P c;
	DL d;
	int n,t;
	MQ q;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p), c = C(m0), d = m0->dl, n = NV(p); 
			m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			if ( NUM(C(m)) && NUM(c) ) {
				t = dmar(((MQ)(C(m)))->cont,((MQ)c)->cont,0,mod);
				STOMQ(t,q); C(mr) = (P)q;
			} else
				mulmp(vl,mod,C(m),c,&C(mr));
			adddl(n,m->dl,d,&mr->dl);
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar + m0->dl->td;
	}
}

void weyl_mulmdm(VL vl,int mod,DP p,MP m0,DP *pr)
{
	DP r,t,t1;
	MP m;
	int n,l,i;
	static MP *w;
	static int wlen;

	if ( !p )
		*pr = 0;
	else {
		for ( m = BDY(p), l = 0; m; m = NEXT(m), l++ );
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( r = 0, i = l-1, n = NV(p); i >= 0; i-- ) {
			weyl_mulmmm(vl,mod,w[i],m0,n,&t);
			addmd(vl,mod,r,t,&t1); r = t1;
		}
		bzero(w,l*sizeof(MP));
		if ( r )
			r->sugar = p->sugar + m0->dl->td;
		*pr = r;
	}
}

/* m0 = x0^d0*x1^d1*... * dx0^d(n/2)*dx1^d(n/2+1)*... */

void weyl_mulmmm(VL vl,int mod,MP m0,MP m1,int n,DP *pr)
{
	MP mr,mr0;
	MQ mq;
	DP r,t,t1;
	P c,c0,c1;
	DL d,d0,d1;
	int i,j,a,b,k,l,n2,s,min;
	static int *tab;
	static int tablen;

	if ( !m0 || !m1 )
		*pr = 0;
	else {
		c0 = C(m0); c1 = C(m1);
		mulmp(vl,mod,c0,c1,&c);
		d0 = m0->dl; d1 = m1->dl;
		n2 = n>>1;
		if ( n & 1 ) { 
			/* homogenized computation; dx-xd=h^2 */
			/* offset of h-degree */
			NEWDL(d,n);
			d->td = d->d[n-1] = d0->d[n-1]+d1->d[n-1];
			NEWMP(mr); mr->c = (P)ONEM; mr->dl = d; NEXT(mr) = 0;
			MKDP(n,mr,r); r->sugar = d->td;
		} else
			r = (DP)ONEM;
		for ( i = 0; i < n2; i++ ) {
			a = d0->d[i]; b = d1->d[n2+i];
			k = d0->d[n2+i]; l = d1->d[i];

			/* degree of xi^a*(Di^k*xi^l)*Di^b */
			a += l;
			b += k;
			s = MUL_WEIGHT(a,i)+MUL_WEIGHT(b,n2+i);

			/* compute xi^a*(Di^k*xi^l)*Di^b */
			min = MIN(k,l);

			if ( min+1 > tablen ) {
				if ( tab ) GC_free(tab);
				tab = (int *)MALLOC((min+1)*sizeof(int));
				tablen = min+1;
			}
			mkwcm(k,l,mod,tab);
			if ( n & 1 )
				for ( mr0 = 0, j = 0; j <= min; j++ ) {
					NEXTMP(mr0,mr); NEWDL(d,n);
					d->d[i] = a-j; d->d[n2+i] = b-j;
					d->td = s;
					d->d[n-1] = s-(MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i));
					STOMQ(tab[j],mq); mr->c = (P)mq; mr->dl = d;
				}
			else
				for ( mr0 = 0, s = 0, j = 0; j <= min; j++ ) {
					NEXTMP(mr0,mr); NEWDL(d,n);
					d->d[i] = a-j; d->d[n2+i] = b-j;
					d->td = MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i); /* XXX */
					s = MAX(s,d->td); /* XXX */
					STOMQ(tab[j],mq); mr->c = (P)mq; mr->dl = d;
				}
			bzero(tab,(min+1)*sizeof(int));
			if ( mr0 )
				NEXT(mr) = 0;
			MKDP(n,mr0,t);
			if ( t )
				t->sugar = s;
			comm_mulmd(vl,mod,r,t,&t1); r = t1;
		}
		mulmdc(vl,mod,r,c,pr);
	}
}

void mulmdc(VL vl,int mod,DP p,P c,DP *pr)
{
	MP m,mr,mr0;
	int t;
	MQ q;

	if ( !p || !c )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			if ( NUM(C(m)) && NUM(c) ) {
				t = dmar(((MQ)(C(m)))->cont,((MQ)c)->cont,0,mod);
				STOMQ(t,q); C(mr) = (P)q;
			} else
				mulmp(vl,mod,C(m),c,&C(mr));
			mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void divsmdc(VL vl,int mod,DP p,P c,DP *pr)
{
	MP m,mr,mr0;

	if ( !c )
		error("disvsdc : division by 0");
	else if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTMP(mr0,mr); divsmp(vl,mod,C(m),c,&C(mr)); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar;
	}
}

void _dtop_mod(VL vl,VL dvl,DP p,P *pr)
{
	int n,i;
	DL d;
	MP m;
	P r,s,t,u,w;
	Q q;
	VL tvl;

	if ( !p )
		*pr = 0;
	else {
		for ( n = p->nv, m = BDY(p), s = 0; m; m = NEXT(m) ) {
			i = ITOS(m->c); STOQ(i,q); t = (P)q;
			for ( i = 0, d = m->dl, tvl = dvl;
				i < n; tvl = NEXT(tvl), i++ ) {
				MKV(tvl->v,r); STOQ(d->d[i],q); pwrp(vl,r,q,&u);
				mulp(vl,t,u,&w); t = w;
			}
			addp(vl,s,t,&u); s = u;
		}
		*pr = s;
	}
}

void _dp_mod(DP p,int mod,NODE subst,DP *rp)
{
	MP m,mr,mr0;
	P t,s;
	NODE tn;

	if ( !p )
		*rp = 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			for ( tn = subst, s = m->c; tn; tn = NEXT(NEXT(tn)) ) {
				substp(CO,s,BDY(tn),BDY(NEXT(tn)),&t);
				s = t;
			}
			ptomp(mod,s,&t);
			if ( t ) {
				NEXTMP(mr0,mr); mr->c = STOI(CONT((MQ)t)); mr->dl = m->dl;
			}
		}
		if ( mr0 ) {
			NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
		} else
			*rp = 0;
	}
}

void _dp_monic(DP p,int mod,DP *rp)
{
	MP m,mr,mr0;
	int c,c1;

	if ( !p )
		*rp = 0;
	else {
		c = invm(ITOS(BDY(p)->c),mod);
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			c1 = dmar(ITOS(m->c),c,0,mod);
			NEXTMP(mr0,mr); mr->c = STOI(c1); mr->dl = m->dl;
		}
		NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
	}
}

void _printdp(DP d)
{
	int n,i;
	MP m;
	DL dl;

	if ( !d ) {
		printf("0"); return;
	}
	for ( n = d->nv, m = BDY(d); m; m = NEXT(m) ) {
		printf("%d*<<",ITOS(m->c));
		for ( i = 0, dl = m->dl; i < n-1; i++ )
			printf("%d,",dl->d[i]);
		printf("%d",dl->d[i]);
		printf(">>");
		if ( NEXT(m) )
			printf("+");
	}
}

/* merge p1 and p2 into pr */

void addmd_destructive(int mod,DP p1,DP p2,DP *pr)
{
	int n;
	MP m1,m2,mr,mr0,s;
	int t;

	if ( !p1 )
		*pr = p2;
	else if ( !p2 )
		*pr = p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; )
			switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
				case 0:
					t = (ITOS(C(m1))+ITOS(C(m2))) - mod;
					if ( t < 0 )
						t += mod;
					s = m1; m1 = NEXT(m1);
					if ( t ) {
						NEXTMP2(mr0,mr,s); C(mr) = STOI(t);
					}
					s = m2; m2 = NEXT(m2);
					break;
				case 1:
					s = m1; m1 = NEXT(m1); NEXTMP2(mr0,mr,s);
					break;
				case -1:
					s = m2; m2 = NEXT(m2); NEXTMP2(mr0,mr,s);
					break;
			}
		if ( !mr0 )
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else {
				*pr = 0;
				return;
			}
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		MKDP(NV(p1),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = MAX(p1->sugar,p2->sugar);
	}
}

void mulmd_dup(int mod,DP p1,DP p2,DP *pr)
{
	if ( !do_weyl )
		comm_mulmd_dup(mod,p1,p2,pr);
	else
		weyl_mulmd_dup(mod,p1,p2,pr);
}

void comm_mulmd_dup(int mod,DP p1,DP p2,DP *pr)
{
	MP m;
	DP s,t,u;
	int i,l,l1;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else {
		for ( m = BDY(p1), l1 = 0; m; m = NEXT(m), l1++ );
		for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
		if ( l1 < l ) {
			t = p1; p1 = p2; p2 = t;
			l = l1;
		}
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			mulmdm_dup(mod,p1,w[i],&t); addmd_destructive(mod,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

void weyl_mulmd_dup(int mod,DP p1,DP p2,DP *pr)
{
	MP m;
	DP s,t,u;
	int i,l;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else {
		for ( m = BDY(p1), l = 0; m; m = NEXT(m), l++ );
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p1), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			weyl_mulmdm_dup(mod,w[i],p2,&t); addmd_destructive(mod,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

void mulmdm_dup(int mod,DP p,MP m0,DP *pr)
{
	MP m,mr,mr0;
	DL d,dt,dm;
	int c,n,i;
	int *pt,*p1,*p2;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p), c = ITOS(C(m0)), d = m0->dl, n = NV(p); 
			m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			C(mr) = STOI(dmar(ITOS(C(m)),c,0,mod));
			NEWDL_NOINIT(dt,n); mr->dl = dt;
			dm = m->dl;
			dt->td = d->td + dm->td;
			for ( i = 0, pt = dt->d, p1=d->d, p2 = dm->d; i < n; i++ )
				*pt++ = *p1++ + *p2++;
		}
		NEXT(mr) = 0; MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar + m0->dl->td;
	}
}

void weyl_mulmdm_dup(int mod,MP m0,DP p,DP *pr)
{
	DP r,t,t1;
	MP m;
	DL d0;
	int n,n2,l,i,j,tlen;
	static MP *w,*psum;
	static struct cdlm *tab;
	static int wlen;
	static int rtlen;

	if ( !p )
		*pr = 0;
	else {
		for ( m = BDY(p), l = 0; m; m = NEXT(m), l++ );
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		n = NV(p); n2 = n>>1;
		d0 = m0->dl;

		for ( i = 0, tlen = 1; i < n2; i++ )
			tlen *= d0->d[n2+i]+1;
		if ( tlen > rtlen ) {
			if ( tab ) GC_free(tab);
			if ( psum ) GC_free(psum);
			rtlen = tlen;
			tab = (struct cdlm *)MALLOC(rtlen*sizeof(struct cdlm));
			psum = (MP *)MALLOC(rtlen*sizeof(MP));
		}
		bzero(psum,tlen*sizeof(MP));
		for ( i = l-1; i >= 0; i-- ) {
			bzero(tab,tlen*sizeof(struct cdlm));
			weyl_mulmmm_dup(mod,m0,w[i],n,tab,tlen);
			for ( j = 0; j < tlen; j++ ) {
				if ( tab[j].c ) {
					NEWMP(m); m->dl = tab[j].d;
					C(m) = STOI(tab[j].c); NEXT(m) = psum[j];
					psum[j] = m;
				}
			}
		}
		for ( j = tlen-1, r = 0; j >= 0; j-- ) 
			if ( psum[j] ) {
				MKDP(n,psum[j],t); addmd_destructive(mod,r,t,&t1); r = t1;
			}
		if ( r )
			r->sugar = p->sugar + m0->dl->td;
		*pr = r;
	}
}

/* m0 = x0^d0*x1^d1*... * dx0^d(n/2)*dx1^d(n/2+1)*... */

void weyl_mulmmm_dup(int mod,MP m0,MP m1,int n,struct cdlm *rtab,int rtablen)
{
	int c,c0,c1;
	DL d,d0,d1,dt;
	int i,j,a,b,k,l,n2,s,min,curlen;
	struct cdlm *p;
	static int *ctab;
	static struct cdlm *tab;
	static int tablen;
	static struct cdlm *tmptab;
	static int tmptablen;

	if ( !m0 || !m1 ) {
		rtab[0].c = 0;
		rtab[0].d = 0;
		return;
	}
	c0 = ITOS(C(m0)); c1 = ITOS(C(m1));
	c = dmar(c0,c1,0,mod);
	d0 = m0->dl; d1 = m1->dl;
	n2 = n>>1;
	curlen = 1;

	NEWDL(d,n);
	if ( n & 1 )
		/* offset of h-degree */
		d->td = d->d[n-1] = d0->d[n-1]+d1->d[n-1];
	else
		d->td = 0;
	rtab[0].c = c;
	rtab[0].d = d;

	if ( rtablen > tmptablen ) {
		if ( tmptab ) GC_free(tmptab);
		tmptab = (struct cdlm *)MALLOC(rtablen*sizeof(struct cdlm));
		tmptablen = rtablen; 
	}

	for ( i = 0; i < n2; i++ ) {
		a = d0->d[i]; b = d1->d[n2+i];
		k = d0->d[n2+i]; l = d1->d[i];

		/* degree of xi^a*(Di^k*xi^l)*Di^b */
		a += l;
		b += k;
		s = MUL_WEIGHT(a,i)+MUL_WEIGHT(b,n2+i);

		if ( !k || !l ) {
			for ( j = 0, p = rtab; j < curlen; j++, p++ ) {
				if ( p->c ) {
					dt = p->d;
					dt->d[i] = a;
					dt->d[n2+i] = b;
					dt->td += s;
				}
			}
			curlen *= k+1;
			continue;
		}
		if ( k+1 > tablen ) {
			if ( tab ) GC_free(tab);
			if ( ctab ) GC_free(ctab);
			tablen = k+1;
			tab = (struct cdlm *)MALLOC(tablen*sizeof(struct cdlm));
			ctab = (int *)MALLOC(tablen*sizeof(int));
		}
		/* compute xi^a*(Di^k*xi^l)*Di^b */
		min = MIN(k,l);
		mkwcm(k,l,mod,ctab);
		bzero(tab,(k+1)*sizeof(struct cdlm));
		/* n&1 != 0 => homogenized computation; dx-xd=h^2 */
		if ( n & 1 )
			for ( j = 0; j <= min; j++ ) {
				NEWDL(d,n);
				d->d[i] = a-j; d->d[n2+i] = b-j;
				d->td = s;
				d->d[n-1] = s-(MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i));
				tab[j].d = d;
				tab[j].c = ctab[j];
			}
		else
			for ( j = 0; j <= min; j++ ) {
				NEWDL(d,n);
				d->d[i] = a-j; d->d[n2+i] = b-j;
				d->td = MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i); /* XXX */
				tab[j].d = d;
				tab[j].c = ctab[j];
			}
		comm_mulmd_tab_destructive(mod,n,rtab,curlen,tab,k+1);
		curlen *= k+1;
	}
}

void comm_mulmd_tab_destructive(int mod,int nv,struct cdlm *t,int n,struct cdlm *t1,int n1)
{
	int i,j;
	struct cdlm *p;
	int c;
	DL d;

	for ( j = 1, p = t+n; j < n1; j++ ) {
		c = t1[j].c;
		d = t1[j].d;
		if ( !c )
			break;
		for ( i = 0; i < n; i++, p++ ) {
			if ( t[i].c ) {
				p->c = dmar(t[i].c,c,0,mod);
				adddl_dup(nv,t[i].d,d,&p->d);
			}
		}
	}
	c = t1[0].c;
	d = t1[0].d;
	for ( i = 0, p = t; i < n; i++, p++ )
		if ( t[i].c ) {
			p->c = dmar(t[i].c,c,0,mod);
			/* t[i].d += d */
			adddl_destructive(nv,t[i].d,d);
		}
}

void adddl_dup(int n,DL d1,DL d2,DL *dr)
{
	DL dt;
	int i;

	NEWDL(dt,n);
	*dr = dt;
	dt->td = d1->td + d2->td;
	for ( i = 0; i < n; i++ )
		dt->d[i] = d1->d[i]+d2->d[i];
}

/* ------------------------------------------------------ */

#if defined(__GNUC__)
#define INLINE inline
#elif defined(VISUAL)
#define INLINE __inline
#else
#define INLINE
#endif

typedef struct oPGeoBucket {
	int m;
	struct oND *body[32];
} *PGeoBucket;

typedef struct oND {
	struct oNM *body;
	int nv;
	int sugar;
} *ND;

typedef struct oNM {
	struct oNM *next;
	int td;
	int c;
	unsigned int dl[1];
} *NM;

typedef struct oND_pairs {
	struct oND_pairs *next;
	int i1,i2;
	int td,sugar;
	unsigned int lcm[1];
} *ND_pairs;

static ND *nps;
int nd_mod,nd_nvar;
int is_rlex;
int nd_epw,nd_bpe,nd_wpd;
unsigned int nd_mask[32];
unsigned int nd_mask0;
NM _nm_free_list;
ND _nd_free_list;
ND_pairs _ndp_free_list;

extern int Top,Reverse;
int nd_psn,nd_pslen;

void GC_gcollect();
NODE append_one(NODE,int);

#define HTD(d) ((d)->body->td)
#define HDL(d) ((d)->body->dl)
#define HC(d) ((d)->body->c)

#define NEWND_pairs(m) if(!_ndp_free_list)_NDP_alloc(); (m)=_ndp_free_list; _ndp_free_list = NEXT(_ndp_free_list)   
#define NEWNM(m) if(!_nm_free_list)_NM_alloc(); (m)=_nm_free_list; _nm_free_list = NEXT(_nm_free_list)   
#define MKND(n,m,d) if(!_nd_free_list)_ND_alloc(); (d)=_nd_free_list; _nd_free_list = (ND)BDY(_nd_free_list); (d)->nv=(n); BDY(d)=(m)

#define NEXTNM(r,c) \
if(!(r)){NEWNM(r);(c)=(r);}else{NEWNM(NEXT(c));(c)=NEXT(c);}
#define NEXTNM2(r,c,s) \
if(!(r)){(c)=(r)=(s);}else{NEXT(c)=(s);(c)=(s);}
#define FREENM(m) NEXT(m)=_nm_free_list; _nm_free_list=(m)
#define FREEND(m) BDY(m)=(NM)_nd_free_list; _nd_free_list=(m)

#define NEXTND_pairs(r,c) \
if(!(r)){NEWND_pairs(r);(c)=(r);}else{NEWND_pairs(NEXT(c));(c)=NEXT(c);}

ND_pairs crit_B( ND_pairs d, int s );
void nd_gr(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp);
NODE nd_setup(NODE f);
int nd_newps(ND a);
ND_pairs nd_minp( ND_pairs d, ND_pairs *prest );
NODE update_base(NODE nd,int ndp);
static ND_pairs equivalent_pairs( ND_pairs d1, ND_pairs *prest );
int crit_2( int dp1, int dp2 );
ND_pairs crit_F( ND_pairs d1 );
ND_pairs crit_M( ND_pairs d1 );
ND_pairs nd_newpairs( NODE g, int t );
ND_pairs update_pairs( ND_pairs d, NODE /* of index */ g, int t);
NODE nd_gb(NODE f);
void nd_free_private_storage();
void _NM_alloc();
void _ND_alloc();
int ndl_td(unsigned int *d);
ND nd_add(ND p1,ND p2);
ND nd_mul_nm(ND p,NM m0);
ND nd_sp(ND_pairs p);
ND nd_reducer(ND p1,ND p2);
ND nd_find_reducer(ND g);
ND nd_nf(ND g,int full);
void ndl_print(unsigned int *dl);
void nd_print(ND p);
void ndp_print(ND_pairs d);
int nd_length(ND p);
void nd_monic(ND p);

void nd_free_private_storage()
{
	_nd_free_list = 0;
	_nm_free_list = 0;
	GC_gcollect();
}

void _NM_alloc()
{
	NM p;
	int i;

	for ( i = 0; i < 10240; i++ ) {
		p = (NM)GC_malloc(sizeof(struct oNM)+(nd_wpd-1)*sizeof(unsigned int));
		p->next = _nm_free_list; _nm_free_list = p;
	}
}

void _ND_alloc()
{
	ND p;
	int i;

	for ( i = 0; i < 10240; i++ ) {
		p = (ND)GC_malloc(sizeof(struct oND));
		p->body = (NM)_nd_free_list; _nd_free_list = p;
	}
}

void _NDP_alloc()
{
	ND_pairs p;
	int i;

	for ( i = 0; i < 10240; i++ ) {
		p = (ND_pairs)GC_malloc(sizeof(struct oND_pairs)
			+(nd_wpd-1)*sizeof(unsigned int));
		p->next = _ndp_free_list; _ndp_free_list = p;
	}
}

INLINE nd_length(ND p)
{
	NM m;
	int i;

	if ( !p )
		return 0;
	else {
		for ( i = 0, m = BDY(p); m; m = NEXT(m), i++ );
		return i;
	}
}

INLINE int ndl_reducible(unsigned int *d1,unsigned int *d2)
{
	unsigned int u1,u2;
	int i,j;

	switch ( nd_bpe ) {
		case 4:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xf0000000) < (u2&0xf0000000) ) return 0;
				if ( (u1&0xf000000) < (u2&0xf000000) ) return 0;
				if ( (u1&0xf00000) < (u2&0xf00000) ) return 0;
				if ( (u1&0xf0000) < (u2&0xf0000) ) return 0;
				if ( (u1&0xf000) < (u2&0xf000) ) return 0;
				if ( (u1&0xf00) < (u2&0xf00) ) return 0;
				if ( (u1&0xf0) < (u2&0xf0) ) return 0;
				if ( (u1&0xf) < (u2&0xf) ) return 0;
			}
			return 1;
			break;
		case 8:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xff000000) < (u2&0xff000000) ) return 0;
				if ( (u1&0xff0000) < (u2&0xff0000) ) return 0;
				if ( (u1&0xff00) < (u2&0xff00) ) return 0;
				if ( (u1&0xff) < (u2&0xff) ) return 0;
			}
			return 1;
			break;
		case 16:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xffff0000) < (u2&0xffff0000) ) return 0;
				if ( (u1&0xffff) < (u2&0xffff) ) return 0;
			}
			return 1;
			break;
		case 32:
			for ( i = 0; i < nd_wpd; i++ )
				if ( d1[i] < d2[i] ) return 0;
			return 1;
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				for ( j = 0; j < nd_epw; j++ )
					if ( (u1&nd_mask[j]) < (u2&nd_mask[j]) ) return 0;
			}
			return 1;
	}
}

void ndl_lcm(unsigned int *d1,unsigned *d2,unsigned int *d)
{
	unsigned int t1,t2,u,u1,u2;
	int i,j;

	switch ( nd_bpe ) {
		case 4:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xf0000000); t2 = (u2&0xf0000000); u = t1>t2?t1:t2;
				t1 = (u1&0xf000000); t2 = (u2&0xf000000); u |= t1>t2?t1:t2;
				t1 = (u1&0xf00000); t2 = (u2&0xf00000); u |= t1>t2?t1:t2;
				t1 = (u1&0xf0000); t2 = (u2&0xf0000); u |= t1>t2?t1:t2;
				t1 = (u1&0xf000); t2 = (u2&0xf000); u |= t1>t2?t1:t2;
				t1 = (u1&0xf00); t2 = (u2&0xf00); u |= t1>t2?t1:t2;
				t1 = (u1&0xf0); t2 = (u2&0xf0); u |= t1>t2?t1:t2;
				t1 = (u1&0xf); t2 = (u2&0xf); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 8:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xff000000); t2 = (u2&0xff000000); u = t1>t2?t1:t2;
				t1 = (u1&0xff0000); t2 = (u2&0xff0000); u |= t1>t2?t1:t2;
				t1 = (u1&0xff00); t2 = (u2&0xff00); u |= t1>t2?t1:t2;
				t1 = (u1&0xff); t2 = (u2&0xff); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 16:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xffff0000); t2 = (u2&0xffff0000); u = t1>t2?t1:t2;
				t1 = (u1&0xffff); t2 = (u2&0xffff); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 32:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i]; 
				d[i] = u1>u2?u1:u2;
			}
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i]; 
				for ( j = 0, u = 0; j < nd_epw; j++ ) {
					t1 = (u1&nd_mask[j]); t2 = (u2&nd_mask[j]); u |= t1>t2?t1:t2;
				}
				d[i] = u;
			}
			break;
	}
}

int ndl_td(unsigned int *d)
{
	unsigned int t,u;
	int i,j;

	for ( t = 0, i = 0; i < nd_wpd; i++ ) {
		u = d[i];
		for ( j = 0; j < nd_epw; j++, u>>=nd_bpe )
			t += (u&nd_mask0); 
	}
	return t;
}

INLINE int ndl_compare(unsigned int *d1,unsigned int *d2)
{
	int i;

	for ( i = 0; i < nd_wpd; i++, d1++, d2++ )
		if ( *d1 > *d2 )
			return is_rlex ? -1 : 1;
		else if ( *d1 < *d2 )
			return is_rlex ? 1 : -1;
	return 0;
}

INLINE int ndl_equal(unsigned int *d1,unsigned int *d2)
{
	int i;

	for ( i = 0; i < nd_wpd; i++ )
		if ( d1[i] != d2[i] )
			return 0;
	return 1;
}

INLINE void ndl_add(unsigned int *d1,unsigned int *d2,unsigned int *d)
{
	int i;

	for ( i = 0; i < nd_wpd; i++ )
		d[i] = d1[i]+d2[i];
}

void ndl_sub(unsigned int *d1,unsigned int *d2,unsigned int *d)
{
	int i;

	for ( i = 0; i < nd_wpd; i++ )
		d[i] = d1[i]-d2[i];
}

int ndl_disjoint(unsigned int *d1,unsigned int *d2)
{
	unsigned int t1,t2,u,u1,u2;
	int i,j;

	switch ( nd_bpe ) {
		case 4:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xf0000000; t2 = u2&0xf0000000; if ( t1&&t2 ) return 0;
				t1 = u1&0xf000000; t2 = u2&0xf000000; if ( t1&&t2 ) return 0;
				t1 = u1&0xf00000; t2 = u2&0xf00000; if ( t1&&t2 ) return 0;
				t1 = u1&0xf0000; t2 = u2&0xf0000; if ( t1&&t2 ) return 0;
				t1 = u1&0xf000; t2 = u2&0xf000; if ( t1&&t2 ) return 0;
				t1 = u1&0xf00; t2 = u2&0xf00; if ( t1&&t2 ) return 0;
				t1 = u1&0xf0; t2 = u2&0xf0; if ( t1&&t2 ) return 0;
				t1 = u1&0xf; t2 = u2&0xf; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 8:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xff000000; t2 = u2&0xff000000; if ( t1&&t2 ) return 0;
				t1 = u1&0xff0000; t2 = u2&0xff0000; if ( t1&&t2 ) return 0;
				t1 = u1&0xff00; t2 = u2&0xff00; if ( t1&&t2 ) return 0;
				t1 = u1&0xff; t2 = u2&0xff; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 16:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xffff0000; t2 = u2&0xffff0000; if ( t1&&t2 ) return 0;
				t1 = u1&0xffff; t2 = u2&0xffff; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 32:
			for ( i = 0; i < nd_wpd; i++ )
				if ( d1[i] && d2[i] ) return 0;
			return 1;
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				for ( j = 0; j < nd_epw; j++ ) {
					if ( (u1&nd_mask0) && (u2&nd_mask0) ) return 0;
					u1 >>= nd_bpe; u2 >>= nd_bpe;
				}
			}
			return 1;
			break;
	}
}

ND nd_reduce(ND p1,ND p2)
{
	int c,t,td,td2,mul;
	NM m2,prev,head,cur,new;
	unsigned int *d;

	if ( !p1 )
		return 0;
	else {
		mul = ((nd_mod-HC(p1))*invm(HC(p2),nd_mod))%nd_mod;
		td = HTD(p1)-HTD(p2);
		d = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
		ndl_sub(HDL(p1),HDL(p2),d);
		prev = 0; head = cur = BDY(p1);
		NEWNM(new);
		for ( m2 = BDY(p2); m2; ) {
			td2 = new->td = m2->td+td;
			ndl_add(m2->dl,d,new->dl);
			if ( !cur ) {
				C(new) = (C(m2)*mul)%nd_mod;
				if ( !prev ) {
					prev = new;
					NEXT(prev) = 0;
					head = prev;
				} else {
					NEXT(prev) = new;
					NEXT(new) = 0;
					prev = new;
				}
				m2 = NEXT(m2);
				NEWNM(new);
				continue;
			}
			if ( cur->td > td2 )
				c = 1;
			else if ( cur->td < td2 )
				c = -1;
			else
				c = ndl_compare(cur->dl,new->dl);
			switch ( c ) {
				case 0:
					t = (C(cur)+C(m2)*mul)%nd_mod;
					if ( t )
						C(cur) = t;
					else if ( !prev ) {
						head = NEXT(cur);
						FREENM(cur);
						cur = head;
					} else {
						NEXT(prev) = NEXT(cur);
						FREENM(cur);
						cur = NEXT(prev);
					}
					m2 = NEXT(m2);
					break;
				case 1:
					prev = cur;
					cur = NEXT(cur);
					break;
				case -1:
					if ( !prev ) {
						/* cur = head */
						prev = new;
						C(prev) = (C(m2)*mul)%nd_mod;
						NEXT(prev) = head;
						head = prev;
					} else {
						C(new) = (C(m2)*mul)%nd_mod;
						NEXT(prev) = new;
						NEXT(new) = cur;
						prev = new;
					}
					NEWNM(new);
					m2 = NEXT(m2);
					break;
			}
		}
		FREENM(new);
		if ( head ) {
			BDY(p1) = head;
			p1->sugar = MAX(p1->sugar,p2->sugar+td);
			return p1;
		} else {
			FREEND(p1);
			return 0;
		}
			
	}
}

ND nd_sp(ND_pairs p)
{
	NM m;
	ND p1,p2,t1,t2;
	unsigned int *lcm;
	int td;

	p1 = nps[p->i1];
	p2 = nps[p->i2];
	lcm = p->lcm;
	td = p->td;
	NEWNM(m);
	C(m) = HC(p2); m->td = td-HTD(p1); ndl_sub(lcm,HDL(p1),m->dl); NEXT(m) = 0;
	t1 = nd_mul_nm(p1,m);
	C(m) = nd_mod-HC(p1); m->td = td-HTD(p2); ndl_sub(lcm,HDL(p2),m->dl);
	t2 = nd_mul_nm(p2,m);
	FREENM(m);
	return nd_add(t1,t2);
}

ND nd_reducer(ND p1,ND p2)
{
	NM m;
	ND r;


	NEWNM(m);
	C(m) = ((nd_mod-HC(p1))*invm(HC(p2),nd_mod))%nd_mod;
	m->td = HTD(p1)-HTD(p2);
	ndl_sub(HDL(p1),HDL(p2),m->dl);
	NEXT(m) = 0;
	r = nd_mul_nm(p2,m);
	FREENM(m);
	return r;
}

ND nd_find_reducer(ND g)
{
	NM m;
	ND r,p;
	int i;

	for ( i = 0; i < nd_psn; i++ ) {
		p = nps[i];
		if ( HTD(g) >= HTD(p) && ndl_reducible(HDL(g),HDL(p)) ) {
#if 1
			NEWNM(m);
			C(m) = ((nd_mod-HC(g))*invm(HC(p),nd_mod))%nd_mod;
			m->td = HTD(g)-HTD(p);
			ndl_sub(HDL(g),HDL(p),m->dl);
			NEXT(m) = 0;
			r = nd_mul_nm(p,m);
			FREENM(m);
			r->sugar = m->td + p->sugar;
			return r;
#else
			return p;
#endif
		}
	}
	return 0;
}

ND nd_add(ND p1,ND p2)
{
	int n,c;
	int t;
	ND r;
	NM m1,m2,mr0,mr,s;

	if ( !p1 )
		return p2;
	else if ( !p2 )
		return p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			if ( m1->td > m2->td )
				c = 1;
			else if ( m1->td < m2->td )
				c = -1;
			else
				c = ndl_compare(m1->dl,m2->dl);
			switch ( c ) {
				case 0:
					t = ((C(m1))+(C(m2))) - nd_mod;
					if ( t < 0 )
						t += nd_mod;
					s = m1; m1 = NEXT(m1);
					if ( t ) {
						NEXTNM2(mr0,mr,s); C(mr) = (t);
					} else {
						FREENM(s);
					}
					s = m2; m2 = NEXT(m2); FREENM(s);
					break;
				case 1:
					s = m1; m1 = NEXT(m1); NEXTNM2(mr0,mr,s);
					break;
				case -1:
					s = m2; m2 = NEXT(m2); NEXTNM2(mr0,mr,s);
					break;
			}
		}
		if ( !mr0 )
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else
				return 0;
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		BDY(p1) = mr0;
		p1->sugar = MAX(p1->sugar,p2->sugar);
		FREEND(p2);
		return p1;
	}
}

ND nd_mul_nm(ND p,NM m0)
{
	NM m,mr,mr0;
	unsigned int *d,*dt,*dm;
	int c,n,td;
	int *pt,*p1,*p2;
	ND r;

	if ( !p )
		return 0;
	else {
		n = NV(p); m = BDY(p);
		d = m0->dl; td = m0->td; c = C(m0);
		mr0 = 0;
		for ( ; m; m = NEXT(m) ) {
			NEXTNM(mr0,mr);
			C(mr) = (C(m)*c)%nd_mod;
			mr->td = m->td+td;
			ndl_add(m->dl,d,mr->dl);
		}
		NEXT(mr) = 0; 
		MKND(NV(p),mr0,r);
		r->sugar = p->sugar + td;
		return r;
	}
}

#if 1
ND nd_nf(ND g,int full)
{
	ND p,d,red;
	NM m,mrd,tail;
	int n,sugar,psugar;

	if ( !g )
		return 0;
	sugar = g->sugar;
	n = NV(g);
	for ( d = 0; g; ) {
		red = nd_find_reducer(g);
		if ( red ) {
#if 1
			g = nd_add(g,red);
			sugar = MAX(sugar,red->sugar);
#else
			psugar = (HTD(g)-HTD(red))+red->sugar;
			g = nd_reduce(g,red);
			sugar = MAX(sugar,psugar);
#endif
		} else if ( !full )
			return g;
		else {
			m = BDY(g); 
			if ( NEXT(m) ) {
				BDY(g) = NEXT(m); NEXT(m) = 0;
			} else {
				FREEND(g); g = 0;
			}
			if ( d ) {
				NEXT(tail)=m;
				tail=m;
			} else {
				MKND(n,m,d);
				tail = BDY(d);
			}
		}
	}
	if ( d )
		d->sugar = sugar;
	return d;
}
#else

ND nd_remove_head(ND p)
{
	NM m;

	m = BDY(p);
	if ( !NEXT(m) ) {
		FREEND(p);
		p = 0;
	} else
		BDY(p) = NEXT(m);
	FREENM(m);
	return p;
}

PGeoBucket create_pbucket()
{
    PGeoBucket g;
	 
	g = CALLOC(1,sizeof(struct oPGeoBucket));
	g->m = -1;
	return g;
}

void add_pbucket(PGeoBucket g,ND d)
{
	int l,k,m;

	l = nd_length(d);
	for ( k = 0, m = 1; l > m; k++, m <<= 2 );
	/* 4^(k-1) < l <= 4^k */
	d = nd_add(g->body[k],d);
	for ( ; d && nd_length(d) > 1<<(2*k); k++ ) {
		g->body[k] = 0;
		d = nd_add(g->body[k+1],d);
	}
	g->body[k] = d;
	g->m = MAX(g->m,k);
}

int head_pbucket(PGeoBucket g)
{
	int j,i,c,k,nv,sum;
	unsigned int *di,*dj;
	ND gi,gj;

	k = g->m;
	while ( 1 ) {
		j = -1;
		for ( i = 0; i <= k; i++ ) {
			if ( !(gi = g->body[i]) )
				continue;
			if ( j < 0 ) {
				j = i;
				gj = g->body[j];
				dj = HDL(gj);
				sum = HC(gj);
			} else {
				di = HDL(gi);
				nv = NV(gi);
				if ( HTD(gi) > HTD(gj) )
					c = 1;
				else if ( HTD(gi) < HTD(gj) )
					c = -1;
				else
					c = ndl_compare(di,dj);
				if ( c > 0 ) {
					if ( sum )
						HC(gj) = sum;
					else
						g->body[j] = nd_remove_head(gj);
					j = i;
					gj = g->body[j];
					dj = HDL(gj);
					sum = HC(gj);
				} else if ( c == 0 ) {
					sum = sum+HC(gi)-nd_mod;
					if ( sum < 0 )
						sum += nd_mod;
					g->body[i] = nd_remove_head(gi);
				}
			}
		}
		if ( j < 0 )
			return -1;
		else if ( sum ) {
			HC(gj) = sum;
			return j;
		} else
			g->body[j] = nd_remove_head(gj);
	}
}

ND normalize_pbucket(PGeoBucket g)
{
	int i;
	ND r,t;

	r = 0;
	for ( i = 0; i <= g->m; i++ )
		r = nd_add(r,g->body[i]);
	return r;
}

ND nd_nf(ND g,int full)
{
	ND u,p,d,red;
	NODE l;
	NM m,mrd;
	int sugar,psugar,n,h_reducible,h;
	PGeoBucket bucket;

	if ( !g ) {
		return 0;
	}
	sugar = g->sugar;
	n = g->nv;
	bucket = create_pbucket();
	add_pbucket(bucket,g);
	d = 0;
	while ( 1 ) {
		h = head_pbucket(bucket);
		if ( h < 0 ) {
			if ( d )
				d->sugar = sugar;
			return d;
		}
		g = bucket->body[h];
		red = nd_find_reducer(g);
		if ( red ) {
			bucket->body[h] = nd_remove_head(g);
			red = nd_remove_head(red);
			add_pbucket(bucket,red);
			sugar = MAX(sugar,red->sugar);
		} else if ( !full ) {
			g = normalize_pbucket(bucket);
			if ( g )
				g->sugar = sugar;
			return g;
		} else {
			m = BDY(g); 
			if ( NEXT(m) ) {
				BDY(g) = NEXT(m); NEXT(m) = 0;
			} else {
				FREEND(g); g = 0;
			}
			bucket->body[h] = g;
			NEXT(m) = 0;
			if ( d ) {
				for ( mrd = BDY(d); NEXT(mrd); mrd = NEXT(mrd) );
				NEXT(mrd) = m;
			} else {
				MKND(n,m,d);
			}
		}
	}
}
#endif

NODE nd_gb(NODE f)
{
	int i,nh;
	NODE r,g,gall;
	ND_pairs d;
	ND_pairs l;
	ND h,nf;

	for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
		i = (int)BDY(r);
		d = update_pairs(d,g,i);
		g = update_base(g,i);
		gall = append_one(gall,i);
	}
	while ( d ) {
#if 0
		ndp_print(d);
#endif
		l = nd_minp(d,&d);
		h = nd_sp(l);
		nf = nd_nf(h,!Top);
		if ( nf ) {
			printf("+"); fflush(stdout);
#if 0
			ndl_print(HDL(nf)); fflush(stdout);
#endif
			nh = nd_newps(nf);
			d = update_pairs(d,g,nh);
			g = update_base(g,nh);
			gall = append_one(gall,nh);
		} else {
			printf("."); fflush(stdout);
		}
	}
	return g;
}

ND_pairs update_pairs( ND_pairs d, NODE /* of index */ g, int t)
{
	ND_pairs d1,nd,cur,head,prev;

	if ( !g ) return d;
	d = crit_B(d,t);
	d1 = nd_newpairs(g,t);
	d1 = crit_M(d1);
	d1 = crit_F(d1);
	prev = 0; cur = head = d1;
	while ( cur ) {
		if ( crit_2( cur->i1,cur->i2 ) ) {
			if ( !prev ) {
				head = cur = NEXT(cur);
			} else {
				cur = NEXT(prev) = NEXT(cur);
			}
		} else {
			prev = cur;
			cur = NEXT(cur);
		}
	}
	if ( !d )
		return head;
	else {
		nd = d;
		while ( NEXT(nd) )
			nd = NEXT(nd);
		NEXT(nd) = head;
		return d;
	}
}

ND_pairs nd_newpairs( NODE g, int t )
{
	NODE h;
	unsigned int *dl;
	int td,ts,s;
	ND_pairs r,r0;

	dl = HDL(nps[t]);
	td = HTD(nps[t]);
	ts = nps[t]->sugar - td;
	for ( r0 = 0, h = g; h; h = NEXT(h) ) {
		NEXTND_pairs(r0,r);
		r->i1 = (int)BDY(h);
		r->i2 = t;
		ndl_lcm(HDL(nps[r->i1]),dl,r->lcm);
		r->td = ndl_td(r->lcm);
		s = nps[r->i1]->sugar-HTD(nps[r->i1]);
		r->sugar = MAX(s,ts) + r->td;
	}
	NEXT(r) = 0;
	return r0;
}

ND_pairs crit_B( ND_pairs d, int s )
{
	ND_pairs cur,head,prev;
	unsigned int *t,*tl,*lcm;
	int td,tdl;

	if ( !d ) return 0;
	t = HDL(nps[s]);
	prev = 0;
	head = cur = d;
	lcm = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
	while ( cur ) {
		tl = cur->lcm;
		if ( ndl_reducible(tl,t)
			&& (ndl_lcm(HDL(nps[cur->i1]),t,lcm),!ndl_equal(lcm,tl))
			&& (ndl_lcm(HDL(nps[cur->i2]),t,lcm),!ndl_equal(lcm,tl)) ) {
			if ( !prev ) {
				head = cur = NEXT(cur);
			} else {
				cur = NEXT(prev) = NEXT(cur);
			}
		} else {
			prev = cur;
			cur = NEXT(cur);
		}
	}
	return head;
}

ND_pairs crit_M( ND_pairs d1 )
{
	ND_pairs e,d2,d3,dd,p;
	unsigned int *id,*jd;
	int itd,jtd;

	for ( dd = 0, e = d1; e; e = d3 ) {
		if ( !(d2 = NEXT(e)) ) {
			NEXT(e) = dd;
			return e;
		}
		id = e->lcm;
		itd = e->td;
		for ( d3 = 0; d2; d2 = p ) {
			p = NEXT(d2),
			jd = d2->lcm;
			jtd = d2->td;
			if ( jtd == itd  )
				if ( id == jd );
				else if ( ndl_reducible(jd,id) ) continue;
				else if ( ndl_reducible(id,jd) ) goto delit;
				else ;
			else if ( jtd > itd )
				if ( ndl_reducible(jd,id) ) continue;
				else ;
			else if ( ndl_reducible(id,jd ) ) goto delit;
			NEXT(d2) = d3;
			d3 = d2;
		}
		NEXT(e) = dd;
		dd = e;
		continue;
		/**/
	delit:	NEXT(d2) = d3;
		d3 = d2;
		for ( ; p; p = d2 ) {
			d2 = NEXT(p);
			NEXT(p) = d3;
			d3 = p;
		}
	}
	return dd;
}

ND_pairs crit_F( ND_pairs d1 )
{
	ND_pairs rest, head;
	ND_pairs last, p, r, w;
	int s;

	for ( head = last = 0, p = d1; NEXT(p); ) {
		r = w = equivalent_pairs(p,&rest);
		s = r->sugar;
		while ( w = NEXT(w) )
			if ( crit_2(w->i1,w->i2) ) {
				r = w;
				break;
			} else if ( w->sugar < s ) {
				r = w;
				s = r->sugar;
			}
		if ( last ) NEXT(last) = r;
		else head = r;
		NEXT(last = r) = 0;
		p = rest;
		if ( !p ) return head;
	}
	if ( !last ) return p;
	NEXT(last) = p;
	return head;
}

int crit_2( int dp1, int dp2 )
{
	return ndl_disjoint(HDL(nps[dp1]),HDL(nps[dp2]));
}

static ND_pairs equivalent_pairs( ND_pairs d1, ND_pairs *prest )
{
	ND_pairs w,p,r,s;
	unsigned int *d;
	int td;

	w = d1;
	d = w->lcm;
	td = w->td;
	s = NEXT(w);
	NEXT(w) = 0;
	for ( r = 0; s; s = p ) {
		p = NEXT(s);
		if ( td == s->td && ndl_equal(d,s->lcm) ) {
			NEXT(s) = w;
			w = s;
		} else {
			NEXT(s) = r;
			r = s;
		}
	}
	*prest = r;
	return w;
}

NODE update_base(NODE nd,int ndp)
{
	unsigned int *dl, *dln;
	NODE last, p, head;
	int td,tdn;

	dl = HDL(nps[ndp]);
	td = HTD(nps[ndp]);
	for ( head = last = 0, p = nd; p; ) {
		dln = HDL(nps[(int)BDY(p)]);
		tdn = HTD(nps[(int)BDY(p)]);
		if ( tdn >= td && ndl_reducible( dln, dl ) ) {
			p = NEXT(p);
			if ( last ) NEXT(last) = p;
		} else {
			if ( !last ) head = p;
			p = NEXT(last = p);
		}
	}
	head = append_one(head,ndp);
	return head;
}

ND_pairs nd_minp( ND_pairs d, ND_pairs *prest )
{
	ND_pairs m,ml,p,l;
	unsigned int *lcm;
	int s,td,len,tlen,c;

	if ( !(p = NEXT(m = d)) ) {
		*prest = p;
		NEXT(m) = 0;
		return m;
	}
	lcm = m->lcm;
	s = m->sugar;
	td = m->td;
	len = nd_length(nps[m->i1])+nd_length(nps[m->i2]);
	for ( ml = 0, l = m; p; p = NEXT(l = p) ) {
		if (p->sugar < s)
			goto find;
		else if ( p->sugar == s ) {
			if ( p->td < td )
				goto find;
			else if ( p->td == td ) {
				c = ndl_compare(p->lcm,lcm);
				if ( c < 0 )
					goto find;
				else if ( c == 0 ) {
					tlen = nd_length(nps[p->i1])+nd_length(nps[p->i2]);
					if ( tlen < len )
						goto find;
				}
			}
		}
		continue;
find:
		ml = l;
		m = p;
		lcm = m->lcm;
		s = m->sugar;
		td = m->td;
		len = tlen;
	}
	if ( !ml ) *prest = NEXT(m);
	else {
		NEXT(ml) = NEXT(m);
		*prest = d;
	}
	NEXT(m) = 0;
	return m;
}

int nd_newps(ND a)
{
	if ( nd_psn == nd_pslen ) {
		nd_pslen *= 2;
		nps = (ND *)REALLOC((char *)nps,nd_pslen*sizeof(ND));
	}
	nd_monic(a);
	nps[nd_psn] = a;
	return nd_psn++;
}

NODE NODE_sortb(NODE f,int);
ND dptond(DP);
DP ndtodp(ND);

NODE nd_setup(NODE f)
{
	int i;
	NODE s,s0,f0;

#if 0
	f0 = f = NODE_sortb(f,1);
#endif
	nd_psn = length(f); nd_pslen = 2*nd_psn;
	nps = (ND *)MALLOC(nd_pslen*sizeof(ND));
	nd_bpe = 4;
	nd_epw = (sizeof(unsigned int)*8)/nd_bpe;
	nd_wpd = nd_nvar/nd_epw+(nd_nvar%nd_epw?1:0);
	nd_mask0 = (1<<nd_bpe)-1;
	bzero(nd_mask,sizeof(nd_mask));
	for ( i = 0; i < nd_epw; i++ )
		nd_mask[nd_epw-i-1] = (nd_mask0<<(i*nd_bpe));
	nd_free_private_storage();
	for ( i = 0; i < nd_psn; i++, f = NEXT(f) ) {
		nps[i] = dptond((DP)BDY(f));
		nd_monic(nps[i]);
	}
	for ( s0 = 0, i = 0; i < nd_psn; i++ ) {
		NEXTNODE(s0,s); BDY(s) = (pointer)i;
	}
	if ( s0 ) NEXT(s) = 0;
	return s0;
}

void nd_gr(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp)
{
	struct order_spec ord1;
	VL fv,vv,vc;
	NODE fd,fd0,r,r0,t,x,s,xx;
	DP a,b,c;

	get_vars((Obj)f,&fv); pltovl(v,&vv);
	nd_nvar = length(vv);
	if ( ord->id )
		error("nd_gr : unsupported order");
	switch ( ord->ord.simple ) {
		case 0:
			is_rlex = 1;
			break;
		case 1:
			is_rlex = 0;
			break;
		default:
			error("nd_gr : unsupported order");
	}
	initd(ord);
	nd_mod = m;
	for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
		ptod(CO,vv,(P)BDY(t),&b);
		_dp_mod(b,m,0,&c);
		if ( c ) {
			NEXTNODE(fd0,fd); BDY(fd) = (pointer)c;
		}
	}
	if ( fd0 ) NEXT(fd) = 0;
	s = nd_setup(fd0);
	x = nd_gb(s);
#if 0
	x = nd_reduceall(x,m);
#endif
	for ( r0 = 0; x; x = NEXT(x) ) {
		NEXTNODE(r0,r); 
		a = ndtodp(nps[(int)BDY(x)]);
		_dtop_mod(CO,vv,a,(P *)&BDY(r));
	}
	if ( r0 ) NEXT(r) = 0;
	MKLIST(*rp,r0);
}

void dltondl(int n,DL dl,unsigned int *r)
{
	unsigned int *d;
	int i;

	d = dl->d;
	bzero(r,nd_wpd*sizeof(unsigned int));
	if ( is_rlex )
		for ( i = 0; i < n; i++ )
			r[(n-1-i)/nd_epw] |= (d[i]<<((nd_epw-((n-1-i)%nd_epw)-1)*nd_bpe));
	else
		for ( i = 0; i < n; i++ )
			r[i/nd_epw] |= d[i]<<((nd_epw-(i%nd_epw)-1)*nd_bpe);
}

DL ndltodl(int n,int td,unsigned int *ndl)
{
	DL dl;
	int *d;
	int i;

	NEWDL(dl,n);
	dl->td = td;
	d = dl->d;
	if ( is_rlex )
		for ( i = 0; i < n; i++ )
			d[i] = (ndl[(n-1-i)/nd_epw]>>((nd_epw-((n-1-i)%nd_epw)-1)*nd_bpe))
				&((1<<nd_bpe)-1);
	else
		for ( i = 0; i < n; i++ )
			d[i] = (ndl[i/nd_epw]>>((nd_epw-(i%nd_epw)-1)*nd_bpe))
				&((1<<nd_bpe)-1);
	return dl;
}

ND dptond(DP p)
{
	ND d;
	NM m0,m;
	MP t;
	int n;

	if ( !p )
		return 0;
	n = NV(p);
	m0 = 0;
	for ( t = BDY(p); t; t = NEXT(t) ) {
		NEXTNM(m0,m);
		m->c = ITOS(t->c);
		m->td = t->dl->td;
		dltondl(n,t->dl,m->dl);
	}
	NEXT(m) = 0;
	MKND(n,m0,d);
	d->nv = n;
	d->sugar = p->sugar;
	return d;
}

DP ndtodp(ND p)
{
	DP d;
	MP m0,m;
	NM t;
	int n;

	if ( !p )
		return 0;
	n = NV(p);
	m0 = 0;
	for ( t = BDY(p); t; t = NEXT(t) ) {
		NEXTMP(m0,m);
		m->c = STOI(t->c);
		m->dl = ndltodl(n,t->td,t->dl);
	}
	NEXT(m) = 0;
	MKDP(n,m0,d);
	d->sugar = p->sugar;
	return d;
}

void ndl_print(unsigned int *dl)
{
	int n;
	int i;

	n = nd_nvar;
	printf("<<");
	if ( is_rlex )
		for ( i = 0; i < n; i++ )
			printf(i==n-1?"%d":"%d,",
				(dl[(n-1-i)/nd_epw]>>((nd_epw-((n-1-i)%nd_epw)-1)*nd_bpe))
					&((1<<nd_bpe)-1));
	else
		for ( i = 0; i < n; i++ )
			printf(i==n-1?"%d":"%d,",
				(dl[i/nd_epw]>>((nd_epw-(i%nd_epw)-1)*nd_bpe))
					&((1<<nd_bpe)-1));
	printf(">>");
}

void nd_print(ND p)
{
	NM m;

	if ( !p )
		printf("0\n");
	else {
		for ( m = BDY(p); m; m = NEXT(m) ) {
			printf("+%d*",m->c);
			ndl_print(m->dl);
		}
		printf("\n");
	}
}

void ndp_print(ND_pairs d)
{
	ND_pairs t;

	for ( t = d; t; t = NEXT(t) ) {
		printf("%d,%d ",t->i1,t->i2);
	}
	printf("\n");
}

void nd_monic(ND p)
{
	int mul;
	NM m;

	if ( !p )
		return;
	mul = invm(HC(p),nd_mod);
	for ( m = BDY(p); m; m = NEXT(m) )
		C(m) = (C(m)*mul)%nd_mod;
}
