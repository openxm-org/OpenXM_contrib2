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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/algnum.c,v 1.4 2000/12/05 01:24:49 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Pdefpoly(), Pnewalg(), Pmainalg(), Palgtorat(), Prattoalg(), Pgetalg();
void Palg(), Palgv(), Pgetalgtree();

void mkalg(P,Alg *);
int cmpalgp(P,P);
void algptop(P,P *);
void algtorat(Num,Obj *);
void rattoalg(Obj,Alg *);
void ptoalgp(P,P *);
void clctalg(P,VL *);

struct ftab alg_tab[] = {
	{"defpoly",Pdefpoly,1},
	{"newalg",Pnewalg,1},
	{"mainalg",Pmainalg,1},
	{"algtorat",Palgtorat,1},
	{"rattoalg",Prattoalg,1},
	{"getalg",Pgetalg,1},
	{"getalgtree",Pgetalgtree,1},
	{"alg",Palg,1},
	{"algv",Palgv,1},
	{0,0,0},
};

static int UCN,ACNT;

void Pnewalg(arg,rp)
NODE arg;
Alg *rp;
{
	P p;
	VL vl;
	P c;

	p = (P)ARG0(arg);
	if ( !p || OID(p) != O_P )
		error("newalg : invalid argument");
	clctv(CO,p,&vl);
	if ( NEXT(vl) )
		error("newalg : invalid argument");
	c = COEF(DC(p));
	if ( !NUM(c) || !RATN(c) )
		error("newalg : invalid argument");
	mkalg(p,rp);
}

void mkalg(p,r)
P p;
Alg *r;
{
	VL vl,mvl,nvl;
	V a,tv;
	char buf[BUFSIZ];
	char *name;
	P x,t,s;
	Num c;
	DCP dc,dcr,dcr0;

	for ( vl = ALG; vl; vl = NEXT(vl) )
		if ( !cmpalgp(p,(P)vl->v->attr) ) {
			a = vl->v; break;
		}
	if ( !vl ) {
		NEWVL(vl); NEXT(vl) = ALG; ALG = vl;
		NEWV(a); vl->v = a;
		sprintf(buf,"#%d",ACNT++);
		name = (char *)MALLOC(strlen(buf)+1); 
		strcpy(name,buf); NAME(a) = name; 

		for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); c = (Num)COEF(dc);
			if ( NID(c) != N_A )
				COEF(dcr) = (P)c;
			else
				COEF(dcr) = (P)BDY(((Alg)c));
		}
		NEXT(dcr) = 0; MKP(a,dcr0,t); a->attr = (pointer)t;
		
		sprintf(buf,"t%s",name); makevar(buf,&s); 

		if ( NEXT(ALG) ) {
			tv = (V)NEXT(ALG)->v->priv;	
			for ( vl = CO; NEXT(NEXT(vl)); vl = NEXT(vl) );
			nvl = NEXT(vl); NEXT(vl) = 0;
			for ( vl = CO; NEXT(vl) && (NEXT(vl)->v != tv); vl = NEXT(vl) );
			mvl = NEXT(vl); NEXT(vl) = nvl; NEXT(nvl) = mvl;
		}
	
		a->priv = (pointer)VR(s); VR(s)->priv = (pointer)a;
	}
	MKV(a,x); MKAlg(x,*r);
}

int cmpalgp(p,defp)
P p,defp;
{
	DCP dc,dcd;
	P t;

	for ( dc = DC(p), dcd = DC(defp); dc && dcd; 
		dc = NEXT(dc), dcd = NEXT(dcd) ) {
		if ( cmpq(DEG(dc),DEG(dcd)) )
			break;
		t = NID((Num)COEF(dc)) == N_A ? (P)BDY((Alg)COEF(dc)) : COEF(dc);
		if ( compp(ALG,t,COEF(dcd)) )
			break;
	}
	if ( dc || dcd )
		return 1;
	else
		return 0;
}

void Pdefpoly(arg,rp)
NODE arg;
P *rp;
{
	asir_assert(ARG0(arg),O_N,"defpoly");
	algptop((P)VR((P)BDY((Alg)ARG0(arg)))->attr,rp);
}

void Pmainalg(arg,r)
NODE arg;
Alg *r;
{
	Num c;
	V v;
	P b;

	c = (Num)(ARG0(arg));
	if ( NID(c) <= N_R )
		*r = 0;
	else {
		v = VR((P)BDY((Alg)c)); MKV(v,b); MKAlg(b,*r);
	}
}

void Palgtorat(arg,rp)
NODE arg;
Obj *rp;
{
	asir_assert(ARG0(arg),O_N,"algtorat");
	algtorat((Num)ARG0(arg),rp);
}

void Prattoalg(arg,rp)
NODE arg;
Alg *rp;
{
	asir_assert(ARG0(arg),O_R,"rattoalg");
	rattoalg((Obj)ARG0(arg),rp);
}

void Pgetalg(arg,rp)
NODE arg;
LIST *rp;
{
	Obj t;
	P p;
	VL vl;
	Num a;
	Alg b;
	NODE n0,n;

	if ( !(a = (Num)ARG0(arg)) || NID(a) <= N_R )
		vl = 0;
	else {
		t = BDY((Alg)a);
		switch ( OID(t) ) {
			case O_P: case O_R:
				clctvr(ALG,t,&vl); break;
			default:
				vl = 0; break;
		}
	}
	for ( n0 = 0; vl; vl = NEXT(vl) ) {
		NEXTNODE(n0,n); MKV(vl->v,p); MKAlg(p,b); BDY(n) = (pointer)b;
	}
	if ( n0 )
		NEXT(n) = 0;
	MKLIST(*rp,n0);
}

void Pgetalgtree(arg,rp)
NODE arg;
LIST *rp;
{
	Obj t;
	P p;
	VL vl,vl1,vl2;
	Num a;
	Alg b;
	NODE n0,n;

	if ( !(a = (Num)ARG0(arg)) || NID(a) <= N_R )
		vl = 0;
	else {
		t = BDY((Alg)a);
		switch ( OID(t) ) {
			case O_P:
				clctalg((P)t,&vl); break;
			case O_R:
				clctalg(NM((R)t),&vl1);
				clctalg(DN((R)t),&vl2);
				mergev(ALG,vl1,vl2,&vl); break;
			default:
				vl = 0; break;
		}
	}
	for ( n0 = 0; vl; vl = NEXT(vl) ) {
		NEXTNODE(n0,n); MKV(vl->v,p); MKAlg(p,b); BDY(n) = (pointer)b;
	}
	if ( n0 )
		NEXT(n) = 0;
	MKLIST(*rp,n0);
}

void clctalg(p,vl)
P p;
VL *vl;
{
	int n,i;
	VL tvl;
	VN vn,vn1;
	P d;
	DCP dc;

	for ( n = 0, tvl = ALG; tvl; tvl = NEXT(tvl), n++ );
	vn = (VN) ALLOCA((n+1)*sizeof(struct oVN));
	for ( i = n-1, tvl = ALG; tvl; tvl = NEXT(tvl), i-- ) {
		vn[i].v = tvl->v;
		vn[i].n = 0;
	}
	markv(vn,n,p);
	for ( i = n-1; i >= 0; i-- ) {
		if ( !vn[i].n )
			continue;
		d = (P)vn[i].v->attr;
		for ( dc = DC(d); dc; dc = NEXT(dc) )
			markv(vn,i,COEF(dc));
	}
	vn1 = (VN) ALLOCA((n+1)*sizeof(struct oVN));
	for ( i = 0; i < n; i++ ) {
		vn1[i].v = vn[n-1-i].v; vn1[i].n = vn[n-1-i].n;
	}
	vntovl(vn1,n,vl);
}

void Palg(arg,rp)
NODE arg;
Alg *rp;
{
	Q a;
	VL vl;
	P x;
	int n;

	a = (Q)ARG0(arg);
	if ( a && (OID(a) != O_N || NID(a) != N_Q || !INT(a)) )
		*rp = 0;
	else {
		n = ACNT-QTOS(a)-1;
		for ( vl = ALG; vl && n; vl = NEXT(vl), n-- );
		if ( vl ) {
			MKV(vl->v,x); MKAlg(x,*rp);
		} else
			*rp = 0;
	}
}

void Palgv(arg,rp)
NODE arg;
Obj *rp;
{
	Q a;
	VL vl;
	P x;
	int n;
	Alg b;

	a = (Q)ARG0(arg);
	if ( a && (OID(a) != O_N || NID(a) != N_Q || !INT(a)) )
		*rp = 0;
	else {
		n = ACNT-QTOS(a)-1;
		for ( vl = ALG; vl && n; vl = NEXT(vl), n-- );
		if ( vl ) {
			MKV(vl->v,x); MKAlg(x,b); algtorat((Num)b,rp);
		} else
			*rp = 0;
	}
}

void algptop(p,r)
P p,*r;
{
	DCP dc,dcr,dcr0;

	if ( NUM(p) )
		*r = (P)p;
	else {
		for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc);
			algptop(COEF(dc),&COEF(dcr));
		}
		NEXT(dcr) = 0; MKP((V)(VR(p)->priv),dcr0,*r);
	}
}

void algtorat(n,r)
Num n;
Obj *r;
{
	Obj obj;
	P nm,dn;

	if ( !n || NID(n) <= N_R )
		*r = (Obj)n;
	else {
		obj = BDY((Alg)n);
		if ( ID(obj) <= O_P )
			algptop((P)obj,(P *)r);
		else {
			algptop(NM((R)obj),&nm); algptop(DN((R)obj),&dn);
			divr(CO,(Obj)nm,(Obj)dn,r);
		}
	}	
}

void rattoalg(obj,n)
Obj obj;
Alg *n;
{
	P nm,dn;
	Obj t;

	if ( !obj || ID(obj) == O_N )
		*n = (Alg)obj;
	else if ( ID(obj) == O_P ) {
		ptoalgp((P)obj,(P *)&t); MKAlg(t,*n);
	} else {
		ptoalgp(NM((R)obj),&nm); ptoalgp(DN((R)obj),&dn);
		divr(ALG,(Obj)nm,(Obj)dn,&t); MKAlg(t,*n);
	}
}

void ptoalgp(p,r)
P p,*r;
{
	DCP dc,dcr,dcr0;

	if ( NUM(p) )
		*r = (P)p;
	else {
		for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc);
			ptoalgp(COEF(dc),&COEF(dcr));
		}
		NEXT(dcr) = 0; MKP((V)(VR(p)->priv),dcr0,*r);
	}
}
