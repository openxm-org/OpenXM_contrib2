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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/algnum.c,v 1.11 2005/07/11 00:24:02 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Pdefpoly(), Pnewalg(), Pmainalg(), Palgtorat(), Prattoalg(), Pgetalg();
void Palg(), Palgv(), Pgetalgtree();
void Pinvalg_le();
void Pset_field(),Palgtodalg(),Pdalgtoalg();
void Pinv_or_split_dalg();
void Pdalgtoup();
void Pget_field_defpoly();
void Pget_field_generator();

void mkalg(P,Alg *);
int cmpalgp(P,P);
void algptop(P,P *);
void algtorat(Num,Obj *);
void rattoalg(Obj,Alg *);
void ptoalgp(P,P *);
void clctalg(P,VL *);
void get_algtree(Obj f,VL *r);
void Pinvalg_chrem();
void Pdalgtodp();
void Pdptodalg();

struct ftab alg_tab[] = {
	{"set_field",Pset_field,-3},
	{"get_field_defpoly",Pget_field_defpoly,1},
	{"get_field_generator",Pget_field_generator,1},
	{"algtodalg",Palgtodalg,1},
	{"dalgtoalg",Pdalgtoalg,1},
	{"dalgtodp",Pdalgtodp,1},
	{"dalgtoup",Pdalgtoup,1},
	{"dptodalg",Pdptodalg,1},
	{"inv_or_split_dalg",Pinv_or_split_dalg,1},
	{"invalg_chrem",Pinvalg_chrem,2},
	{"invalg_le",Pinvalg_le,1},
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

void Pset_field(NODE arg,Q *rp)
{
	int ac;
	NODE a0,a1;
	VL vl0,vl;
	struct order_spec *spec;

	if ( (ac = argc(arg)) == 1 )
		setfield_dalg(BDY((LIST)ARG0(arg)));
	else if ( ac == 3 ) {
		a0 = BDY((LIST)ARG0(arg));
		a1 = BDY((LIST)ARG1(arg));
		for ( vl0 = 0; a1; a1 = NEXT(a1) ) {
			NEXTVL(vl0,vl);
			vl->v = VR((P)BDY(a1));
		}
		if ( vl0 ) NEXT(vl) = 0;
		create_order_spec(0,ARG2(arg),&spec);
		setfield_gb(a0,vl0,spec);
	}
	*rp = 0;
}

void Palgtodalg(NODE arg,DAlg *rp)
{
	algtodalg((Alg)ARG0(arg),rp);
}

void Pdalgtoalg(NODE arg,Alg *rp)
{
	dalgtoalg((DAlg)ARG0(arg),rp);
}

void Pdalgtodp(NODE arg,LIST *r)
{
	NODE b;
	DP nm;
	Q dn;
	DAlg da;

	da = (DAlg)ARG0(arg);
	nm = da->nm;
	dn = da->dn;
	b = mknode(2,nm,dn);
	MKLIST(*r,b);
}

void Pdptodalg(NODE arg,DAlg *r)
{
	DP d;
	DAlg t;

	d = (DP)ARG0(arg);
	MKDAlg(d,ONE,t);
	simpdalg(t,r);
}

void Pdalgtoup(NODE arg,LIST *r)
{
	NODE b;
	int pos;
	P up;
	DP nm;
	Q dn,q;

	pos = dalgtoup((DAlg)ARG0(arg),&up,&dn);
	STOQ(pos,q);
	b = mknode(3,up,dn,q);
	MKLIST(*r,b);
}

NODE inv_or_split_dalg(DAlg,DAlg *);
NumberField	get_numberfield();

void Pget_field_defpoly(NODE arg,DAlg *r)
{
	NumberField nf;
	DP d;

	nf = get_numberfield();
	d = nf->ps[QTOS((Q)ARG0(arg))];
	MKDAlg(d,ONE,*r);
}

void Pget_field_generator(NODE arg,DAlg *r)
{
	int index,n,i;
	DL dl;
	MP m;
	DP d;

	index = QTOS((Q)ARG0(arg));
	n = get_numberfield()->n;
	NEWDL(dl,n);
	for ( i = 0; i < n; i++ ) dl->d[i] = 0;
	dl->d[index] = 1; dl->td = 1;
	NEWMP(m); m->dl = dl; m->c = (P)ONE; NEXT(m) = 0;
	MKDP(n,m,d);
	MKDAlg(d,ONE,*r);
}


void Pinv_or_split_dalg(NODE arg,Obj *rp)
{
	NODE gen,t,nd0,nd;
	LIST list;
	int l,i,j,k,n;
	DP *ps,*ps1,*psw;
	NumberField nf;
	DAlg inv;
	extern struct order_spec *dp_current_spec;
	struct order_spec *current_spec;

	gen = inv_or_split_dalg((DAlg)ARG0(arg),&inv);
	if ( !gen )
		*rp = (Obj)inv;
	else {
		nf = get_numberfield();
		current_spec = dp_current_spec; initd(nf->spec);
		l = length(gen);
		n = nf->n;
		ps = nf->ps;
		psw = (DP *)ALLOCA((n+l)*sizeof(DP));
		for ( i = j = 0; i < n; i++ ) {
			for ( t = gen; t; t = NEXT(t) )
				if ( dp_redble(ps[i],(DP)BDY(t)) ) break;
			if ( !t )
				psw[j++] = ps[i];
		}
		nd0  = 0;
		/* gen[0] < gen[1] < ... */
		/* psw[0] > psw[1] > ... */
		for ( i = j-1, t = gen; i >= 0 && t; ) {
			NEXTNODE(nd0,nd);
			if ( compd(CO,psw[i],(DP)BDY(t)) > 0 ) {
				BDY(nd) = BDY(t); t = NEXT(t);
			} else
				BDY(nd) = (pointer)psw[i--];
		}
		for ( ; i >= 0; i-- ) {
			NEXTNODE(nd0,nd); BDY(nd) = (pointer)psw[i];
		}
		for ( ; t; t = NEXT(t), k++ ) {
			NEXTNODE(nd0,nd); BDY(nd) = BDY(t);
		}
		NEXT(nd) = 0;
		MKLIST(list,nd0);
		initd(current_spec);
		*rp = (Obj)list;
	}
}

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

#if 0
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
#else
	get_algtree((Obj)ARG0(arg),&vl);
#endif
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

void Pinvalg_chrem(NODE arg,LIST *r)
{
	NODE n;

	inva_chrem((P)ARG0(arg),(P)ARG1(arg),&n);
	MKLIST(*r,n);
}

void invalg_le(Alg a,LIST *r);

void Pinvalg_le(NODE arg,LIST *r)
{
	invalg_le((Alg)ARG0(arg),r);
}

typedef struct oMono_nf {
	DP mono;
	DP nf;
	Q dn;
} *Mono_nf;

void invalg_le(Alg a,LIST *r)
{
	Alg inv;
	MAT mobj,sol;
	int *rinfo,*cinfo;
	P p,dn,dn1,ap;
	VL vl,tvl;
	Q c1,c2,c3,cont,c,two,iq,dn0,mul,dnsol;
	int i,j,n,len,k;
	MP mp,mp0;
	DP dp,nm,nm1,m,d,u,u1;
	NODE b,b1,hlist,mblist,t,s,rev0,rev,hist;
	DP *ps;
	struct order_spec *spec;
	Mono_nf h,h1;
	N nq,nr,nl,ng;
	Q **mat,**solmat;
	Q *w;
	int *wi;

	ap = (P)BDY(a);
	asir_assert(ap,O_P,"invalg_le");

	/* collecting algebraic numbers */
	clctalg(ap,&vl);

	/* setup */
	ptozp(ap,1,&c,&p);
	STOQ(2,two); create_order_spec(0,(Obj)two,&spec); initd(spec);
	for ( n = 0, tvl = vl; tvl; tvl = NEXT(tvl), n++ );
	ps = (DP *)ALLOCA(n*sizeof(DP));

	/* conversion to DP */
	for ( i = 0, tvl = vl; i < n; i++, tvl = NEXT(tvl) ) {
		ptod(ALG,vl,tvl->v->attr,&ps[i]);
	}
	ptod(ALG,vl,p,&dp);
	/* index list */
	for ( b = 0, i = 0; i < n; i++ ) {
		STOQ(i,iq); MKNODE(b1,(pointer)iq,b); b = b1;
	}
	/* simplification */
	dp_true_nf(b,dp,ps,1,&nm,&dn);

	/* construction of NF table */

	/* stdmono: <<0,...,0>> < ... < max */
	for ( hlist = 0, i = 0; i < n; i++ ) {
		MKNODE(b1,(pointer)ps[i],hlist); hlist = b1;
	}
	dp_mbase(hlist,&rev0);
	for ( mblist = 0, rev = rev0; rev; rev = NEXT(rev) ) {
		MKNODE(b1,BDY(rev),mblist); mblist = b1;
	}
	dn0 = ONE;
	for ( hist = 0, t = mblist; t; t = NEXT(t) ) {
		/* searching a predecessor */
		for ( m = (DP)BDY(t), s = hist; s; s = NEXT(s) ) {
			h = (Mono_nf)BDY(s);
			if ( dp_redble(m,h->mono) )
				break;
		}
		h1 = (Mono_nf)ALLOCA(sizeof(struct oMono_nf));
		if ( s ) {
			dp_subd(m,h->mono,&d);
			muld(CO,d,h->nf,&u);
			dp_true_nf(b,u,ps,1,&nm1,&dn1);
			mulq(h->dn,(Q)dn1,&h1->dn);
		} else {
			muld(CO,m,nm,&u);
			dp_true_nf(b,u,ps,1,&nm1,&dn1);
			h1->dn = (Q)dn1;
		}
		h1->mono = m;
		h1->nf = nm1;
		MKNODE(b1,(pointer)h1,hist); hist = b1;

		/* dn0 = LCM(dn0,h1->dn) */
		gcdn(NM(dn0),NM(h1->dn),&ng); divn(NM(dn0),ng,&nq,&nr);
		muln(nq,NM(h1->dn),&nl); NTOQ(nl,1,dn0);
	}
	/* create a matrix */
	len = length(mblist);
	MKMAT(mobj,len,len+1);
	mat = (Q **)BDY(mobj);
	mat[len-1][len] = dn0;
	for ( j = 0, t = hist; j < len; j++, t = NEXT(t) ) {
		h = (Mono_nf)BDY(t);
		nm1 = h->nf;
		divq((Q)dn0,h->dn,&mul);
		for ( i = 0, rev = rev0, mp = BDY(nm1); mp && i < len; i++, rev = NEXT(rev) )
			if ( dl_equal(n,BDY((DP)BDY(rev))->dl,mp->dl) ) {
				mulq(mul,(Q)mp->c,&mat[i][j]);
				mp = NEXT(mp);
			}
	}
#if 0
	w = (Q *)ALLOCA((len+1)*sizeof(Q));
	wi = (int *)ALLOCA((len+1)*sizeof(int));
	for ( i = 0; i < len; i++ ) {
		for ( j = 0, k = 0; j <= len; j++ )
			if ( mat[i][j] ) {
				w[k] = mat[i][j];
				wi[k] = j;
				k++;
			}
		removecont_array(w,k);
		for ( j = 0; j < k; j++ )
			mat[i][wi[j]] = w[j];
	}
#endif
	generic_gauss_elim_hensel(mobj,&sol,&dnsol,&rinfo,&cinfo);
	solmat = (Q **)BDY(sol);
	for ( i = 0, t = rev0, mp0 = 0; i < len; i++, t = NEXT(t) )
		if ( solmat[i][0] ) {
			NEXTMP(mp0,mp);
			mp->c = (P)solmat[i][0];
			mp->dl = BDY((DP)BDY(t))->dl;
		}
	NEXT(mp) = 0; MKDP(n,mp0,u);
	dp_ptozp(u,&u1);
	divq((Q)BDY(u)->c,(Q)BDY(u1)->c,&cont);
	dtop(ALG,vl,u1,&ap);
	MKAlg(ap,inv);
	mulq(dnsol,(Q)dn,&c1);
	mulq(c1,c,&c2);
	divq(c2,cont,&c3);
	b = mknode(2,inv,c3);
	MKLIST(*r,b);
}

void get_algtree(Obj f,VL *r)
{
	VL vl1,vl2,vl3;
	Obj t;
	DCP dc;
	NODE b;
	pointer *a;
	pointer **m;
	int len,row,col,i,j,l;

	if ( !f ) *r = 0;
	else
		switch ( OID(f) ) {
			case O_N:
				if ( NID((Num)f) != N_A ) *r = 0;
				else  {
					t = BDY((Alg)f);
					switch ( OID(t) ) {
						case O_P:
							clctalg((P)t,r); break;
						case O_R:
							clctalg(NM((R)t),&vl1);
							clctalg(DN((R)t),&vl2);
							mergev(ALG,vl1,vl2,r); break;
						default:
							*r = 0; break;
					}
				}
				break;
			case O_P:
				vl1 = 0;
				for ( dc = DC((P)f); dc; dc = NEXT(dc) ) {
					get_algtree((Obj)COEF(dc),&vl2);
					mergev(ALG,vl1,vl2,&vl3);
					vl1 = vl3;
				}
				*r = vl1;
				break;
			case O_R:
				get_algtree((Obj)NM((R)f),&vl1);
				get_algtree((Obj)DN((R)f),&vl2);
				mergev(ALG,vl1,vl2,r);
				break;
			case O_LIST:
				vl1 = 0;
				for ( b = BDY((LIST)f); b; b = NEXT(b) ) {
					get_algtree((Obj)BDY(b),&vl2);
					mergev(ALG,vl1,vl2,&vl3);
					vl1 = vl3;
				}
				*r = vl1;
				break;
			case O_VECT:
				vl1 = 0;
				l = ((VECT)f)->len;
				a = BDY((VECT)f);
				for ( i = 0; i < l; i++ ) {
					get_algtree((Obj)a[i],&vl2);
					mergev(ALG,vl1,vl2,&vl3);
					vl1 = vl3;
				}
				*r = vl1;
				break;
			case O_MAT:
				vl1 = 0;
				row = ((MAT)f)->row; col = ((MAT)f)->col;
				m = BDY((MAT)f);
				for ( i = 0; i < row; i++ )
					for ( j = 0; j < col; j++ ) {
						get_algtree((Obj)m[i][j],&vl2);
						mergev(ALG,vl1,vl2,&vl3);
						vl1 = vl3;
					}
				*r = vl1;
				break;
			default:
				*r = 0;
				break;
		}
}

void algobjtorat(Obj f,Obj *r)
{
	Obj t;
	DCP dc,dcr,dcr0;
	P p,nm,dn;
	R rat;
	NODE b,s,s0;
	VECT v;
	MAT mat;
	LIST list;
	pointer *a;
	pointer **m;
	int len,row,col,i,j,l;

	if ( !f ) *r = 0;
	else
		switch ( OID(f) ) {
			case O_N:
				algtorat((Num)f,r);
				break;
			case O_P:
				dcr0 = 0;
				for ( dc = DC((P)f); dc; dc = NEXT(dc) ) {
					NEXTDC(dcr0,dcr);
					algobjtorat((Obj)COEF(dc),&t);
					COEF(dcr) = (P)t;
					DEG(dcr) = DEG(dc);
				}
				NEXT(dcr) = 0; MKP(VR((P)f),dcr0,p); *r = (Obj)p;
				break;
			case O_R:
				algobjtorat((Obj)NM((R)f),&t); nm = (P)t;
				algobjtorat((Obj)DN((R)f),&t); dn = (P)t;
				MKRAT(nm,dn,0,rat); *r = (Obj)rat;
				break;
			case O_LIST:
				s0 = 0;
				for ( b = BDY((LIST)f); b; b = NEXT(b) ) {
					NEXTNODE(s0,s);
					algobjtorat((Obj)BDY(b),&t);
					BDY(s) = (pointer)t;
				}
				NEXT(s) = 0;
				MKLIST(list,s0);
				*r = (Obj)list;
				break;
			case O_VECT:
				l = ((VECT)f)->len;
				a = BDY((VECT)f);
				MKVECT(v,l);
				for ( i = 0; i < l; i++ ) {
					algobjtorat((Obj)a[i],&t);
					BDY(v)[i] = (pointer)t;
				}
				*r = (Obj)v;
				break;
			case O_MAT:
				row = ((MAT)f)->row; col = ((MAT)f)->col;
				m = BDY((MAT)f);
				MKMAT(mat,row,col);
				for ( i = 0; i < row; i++ )
					for ( j = 0; j < col; j++ ) {
						algobjtorat((Obj)m[i][j],&t);
						BDY(mat)[i][j] = (pointer)t;
					}
				*r = (Obj)mat;
				break;
			default:
				*r = f;
				break;
		}
}
