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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/poly.c,v 1.13 2001/07/13 08:25:21 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "base.h"

void Pranp();

void Pumul(),Pumul_ff(),Pusquare(),Pusquare_ff(),Putmul(),Putmul_ff();
void Pkmul(),Pksquare(),Pktmul();
void Pord(), Pcoef0(), Pcoef(), Pdeg(), Pmindeg(), Psetmod();
void Pcoef_gf2n();
void getcoef(), getdeglist(), mergedeglist(), change_mvar(), restore_mvar();

void Pp_mag(),Pmaxblen();
void Pmergelist(), Pch_mv(), Pre_mv(), Pdeglist();
void Pptomp(),Pmptop();
void Pptolmp(),Plmptop();
void Pptosfp(),Psfptop(),Psf_galois_action(),Psf_embed(),Psf_find_root();
void Psf_minipoly(),Psf_log();
void Pptogf2n(),Pgf2ntop(),Pgf2ntovect();
void Pptogfpn(),Pgfpntop();
void Pfind_root_gf2n();
void Pumul_specialmod(),Pusquare_specialmod(),Putmul_specialmod();

void Pureverse(),Putrunc(),Pudecomp(),Purembymul(),Purembymul_precomp();
void Puinvmod(),Purevinvmod();
void Ppwrmod_ff(),Ppwrtab_ff(),Pgeneric_pwrmod_ff();
void Pkpwrmod_lm(),Pkpwrtab_lm(),Pkgeneric_pwrmod_lm();

void Pkmulum();
void Pksquareum();

void Pfmultest();
void Phfmul_lm();
void Plazy_lm();

void Psetmod_ff();
void Psimp_ff();
void Pextdeg_ff();
void Pcharacteristic_ff();
void Pfield_type_ff();
void Pfield_order_ff();
void Prandom_ff();

void Ptracemod_gf2n();
void Psparsemod_gf2n();
void Pmultest_gf2n();
void Psquaretest_gf2n();
void Pinvtest_gf2n();
void Pbininv_gf2n();
void Prinvtest_gf2n();
void Pis_irred_gf2();
void Pis_irred_ddd_gf2();
void Pget_next_fft_prime();
void Puadj_coef();
void Preorder();
void Phomogeneous_part();
void Phomogeneous_deg();
void simp_ff(Obj,Obj *);
void ranp(int,UP *);
void field_order_ff(N *);

extern int current_mod;
extern GEN_UP2 current_mod_gf2n;
extern int lm_lazy;

int current_ff;

struct ftab poly_tab[] = {
	{"homogeneous_deg",Phomogeneous_deg,-2},
	{"homogeneous_part",Phomogeneous_part,-3},
	{"reorder",Preorder,3},
	{"uadj_coef",Puadj_coef,3},
	{"ranp",Pranp,2},
	{"p_mag",Pp_mag,1},
	{"maxblen",Pmaxblen,1},
	{"ord",Pord,-1},
	{"coef0",Pcoef0,-3},
	{"coef",Pcoef,-3},
	{"coef_gf2n",Pcoef_gf2n,2},
	{"deg",Pdeg,2},
	{"mindeg",Pmindeg,2},
	{"setmod",Psetmod,-1},

	{"sparsemod_gf2n",Psparsemod_gf2n,-1},

	{"setmod_ff",Psetmod_ff,-2},
	{"simp_ff",Psimp_ff,1},
	{"extdeg_ff",Pextdeg_ff,0},
	{"characteristic_ff",Pcharacteristic_ff,0},
	{"field_type_ff",Pfield_type_ff,0},
	{"field_order_ff",Pfield_order_ff,0},
	{"random_ff",Prandom_ff,0},
	
	{"deglist",Pdeglist,2},
	{"mergelist",Pmergelist,2},
	{"ch_mv",Pch_mv,2},
	{"re_mv",Pre_mv,2},

	{"ptomp",Pptomp,2},
	{"mptop",Pmptop,1},

	{"ptolmp",Pptolmp,1},
	{"lmptop",Plmptop,1},

	{"sf_galois_action",Psf_galois_action,2},
	{"sf_find_root",Psf_find_root,1},
	{"sf_minipoly",Psf_minipoly,2},
	{"sf_embed",Psf_embed,3},
	{"sf_log",Psf_log,1},

	{"ptosfp",Pptosfp,1},
	{"sfptop",Psfptop,1},
	{"ptogf2n",Pptogf2n,1},
	{"gf2ntop",Pgf2ntop,-2},
	{"gf2ntovect",Pgf2ntovect,1},

	{"ptogfpn",Pptogfpn,1},
	{"gfpntop",Pgfpntop,-2},

	{"kmul",Pkmul,2},
	{"ksquare",Pksquare,1},
	{"ktmul",Pktmul,3},

	{"umul",Pumul,2},
	{"usquare",Pusquare,1},
	{"ureverse_inv_as_power_series",Purevinvmod,2},
	{"uinv_as_power_series",Puinvmod,2},

	{"umul_specialmod",Pumul_specialmod,3},
	{"usquare_specialmod",Pusquare_specialmod,2},
	{"utmul_specialmod",Putmul_specialmod,4},

	{"utmul",Putmul,3},
	{"umul_ff",Pumul_ff,2},
	{"usquare_ff",Pusquare_ff,1},
	{"utmul_ff",Putmul_ff,3},

	/* for historical reason */
	{"trunc",Putrunc,2},
	{"decomp",Pudecomp,2},

	{"utrunc",Putrunc,2},
	{"udecomp",Pudecomp,2},
	{"ureverse",Pureverse,-2},
	{"urembymul",Purembymul,2},
	{"urembymul_precomp",Purembymul_precomp,3},

	{"lazy_lm",Plazy_lm,1},
	{"lazy_ff",Plazy_lm,1},

	{"pwrmod_ff",Ppwrmod_ff,1},
	{"generic_pwrmod_ff",Pgeneric_pwrmod_ff,3},
	{"pwrtab_ff",Ppwrtab_ff,2},

	{"tracemod_gf2n",Ptracemod_gf2n,3},
	{"b_find_root_gf2n",Pfind_root_gf2n,1},

	{"is_irred_gf2",Pis_irred_gf2,1},
	{"is_irred_ddd_gf2",Pis_irred_ddd_gf2,1},

	{"kpwrmod_lm",Pkpwrmod_lm,1},
	{"kgeneric_pwrmod_lm",Pkgeneric_pwrmod_lm,3},
	{"kpwrtab_lm",Pkpwrtab_lm,2},

	{"kmulum",Pkmulum,3},
	{"ksquareum",Pksquareum,2},

	{"fmultest",Pfmultest,3},
	{"hfmul_lm",Phfmul_lm,2},

	{"multest_gf2n",Pmultest_gf2n,2},
	{"squaretest_gf2n",Psquaretest_gf2n,1},
	{"bininv_gf2n",Pbininv_gf2n,2},
	{"invtest_gf2n",Pinvtest_gf2n,1},
	{"rinvtest_gf2n",Prinvtest_gf2n,0},
	{"get_next_fft_prime",Pget_next_fft_prime,2},
	{0,0,0},
};

extern V up_var;

void Phomogeneous_part(arg,rp)
NODE arg;
P *rp;
{
	if ( argc(arg) == 2 )
		exthp(CO,(P)ARG0(arg),QTOS((Q)ARG1(arg)),rp);
	else
		exthpc_generic(CO,(P)ARG0(arg),QTOS((Q)ARG2(arg)),
			VR((P)ARG1(arg)),rp);
}

void Phomogeneous_deg(arg,rp)
NODE arg;
Q *rp;
{
	int d;

	if ( argc(arg) == 1 )
		d = homdeg((P)ARG0(arg));
	else
		d = getchomdeg(VR((P)ARG1(arg)),(P)ARG0(arg));
	STOQ(d,*rp);
}

/* 
	p1 = reorder(p,ovl,nvl) => p1 is 'sorted accoding to nvl.
*/

void Preorder(arg,rp)
NODE arg;
P *rp;
{
	VL ovl,nvl,tvl;
	NODE n;

	for ( ovl = 0, n = BDY((LIST)ARG1(arg)); n; n = NEXT(n) ) {
		if ( !ovl ) {
			NEWVL(ovl); tvl = ovl;
		} else {
			NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
		}
			VR(tvl) = VR((P)BDY(n));
	}
	for ( nvl = 0, n = BDY((LIST)ARG2(arg)); n; n = NEXT(n) ) {
		if ( !nvl ) {
			NEWVL(nvl); tvl = nvl;
		} else {
			NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
		}
			VR(tvl) = VR((P)BDY(n));
	}
	reorderp(nvl,ovl,(P)ARG0(arg),rp);
}

/*
	uadj_coef(F,M,M2)
	if ( F is a non-negative integer )
		return F > M2 ? F-M : M2;
	else
		F = CN*V^N+...+C0
		return uadj_coef(CN,M,M2)*V^N+...+uadj_coef(C0,M,M2);
*/

void Puadj_coef(arg,rp)
NODE arg;
P *rp;
{
	UP f,r;
	N m,m2;

	ptoup((P)ARG0(arg),&f);
	m = NM((Q)ARG1(arg));
	m2 = NM((Q)ARG2(arg));
	adj_coefup(f,m,m2,&r);
	uptop(r,rp);
}

/*
	get_next_fft_prime(StartIndex,Bits)
	tries to find smallest Index >= StartIndex s.t.
		2^(Bits-1)|FFTprime[Index]-1
	return [Index,Mod] or 0 (not exist)
*/

void Pget_next_fft_prime(arg,rp)
NODE arg;
LIST *rp;
{
	unsigned int mod,d;
	int start,bits,i;
	NODE n;
	Q q,ind;

	start = QTOS((Q)ARG0(arg));
	bits = QTOS((Q)ARG1(arg));
	for ( i = start; ; i++ ) {
		get_fft_prime(i,&mod,&d);
		if ( !mod ) {
			*rp = 0; return;
		}
		if ( bits <= d ) {
			UTOQ(mod,q);
			UTOQ(i,ind);
			n = mknode(2,ind,q);
			MKLIST(*rp,n);
			return;
		}
	}
}

void Pranp(arg,rp)
NODE arg;
P *rp;
{
	int n;
	UP c;

	n = QTOS((Q)ARG0(arg));
	ranp(n,&c);
	if ( c ) {
		up_var = VR((P)ARG1(arg));
		uptop(c,rp);
	} else
		*rp = 0;
}

void ranp(n,nr)
int n;
UP *nr;
{
	int i;
	unsigned int r;
	Q q;
	UP c;

	*nr = c = UPALLOC(n);
	for ( i = 0; i <= n; i++ ) {
		r = random();
		UTOQ(r,q);
		c->c[i] = (Num)q;
	}
	for ( i = n; i >= 0 && !c->c[i]; i-- );
	if ( i >= 0 )
		c->d = i;
	else
		*nr = 0;
}

void Pmaxblen(arg,rp)
NODE arg;
Q *rp;
{
	int l;
	l = maxblenp(ARG0(arg));
	STOQ(l,*rp);
}

void Pp_mag(arg,rp)
NODE arg;
Q *rp;
{
	int l;
	l = p_mag(ARG0(arg));
	STOQ(l,*rp);
}

void Pord(arg,listp)
NODE arg;
LIST *listp;
{
	NODE n,tn;
	LIST l;
	VL vl,tvl,svl;
	P t;
	int i,j;
	V *va;
	V v;

	if ( argc(arg) ) {
		asir_assert(ARG0(arg),O_LIST,"ord");
		for ( vl = 0, i = 0, n = BDY((LIST)ARG0(arg));
			n; n = NEXT(n), i++ ) {
			if ( !vl ) {
				NEWVL(vl); tvl = vl;
			} else {
				NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
			}
			if ( !(t = (P)BDY(n)) || (OID(t) != O_P) )
				error("ord : invalid argument");
			VR(tvl) = VR(t);
		}
		va = (V *)ALLOCA(i*sizeof(V));
		for ( j = 0, svl = vl; j < i; j++, svl = NEXT(svl) )
			va[j] = VR(svl);
		for ( svl = CO; svl; svl = NEXT(svl) ) {
			v = VR(svl);
			for ( j = 0; j < i; j++ )
				if ( v == va[j] )
					break;
			if ( j == i ) {
				if ( !vl ) {
					NEWVL(vl); tvl = vl;
				} else {
					NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
				}
				VR(tvl) = v;
			}
		}
		if ( vl )
			NEXT(tvl) = 0;
		CO = vl;
	}
	for ( n = 0, vl = CO; vl; vl = NEXT(vl) ) {
		NEXTNODE(n,tn); MKV(VR(vl),t); BDY(tn) = (pointer)t;
	}
	NEXT(tn) = 0; MKLIST(l,n); *listp = l;
}

void Pcoef0(arg,rp)
NODE arg;
Obj *rp;
{
	Obj t,n;
	P s;
	DCP dc;
	int id;
	V v;
	VL vl;

	if ( !(t = (Obj)ARG0(arg)) || ((id = OID(ARG0(arg))) > O_P) )
		*rp = 0;
	else if ( (n = (Obj)ARG1(arg)) && (OID(n) > O_N) )
		*rp = 0;
	else if ( id == O_N )
		if ( !n )
			*rp = t;
		else
			*rp = 0;
	else {
		if ( argc(arg) == 3 ) {
			if ( (v = VR((P)ARG2(arg))) != VR((P)t) ) {
				reordvar(CO,v,&vl); reorderp(vl,CO,(P)t,&s);
			} else
				s = (P)t;
			if ( VR(s) != v ) {
				if ( n )
					*rp = 0;
				else 
					*rp = t;
				return;
			}
		} else
			s = (P)t;
		for ( dc = DC(s); dc && cmpq(DEG(dc),(Q)n); dc = NEXT(dc) );
		if ( dc )
			*rp = (Obj)COEF(dc);
		else
			*rp = 0;
	}
}

void Pcoef(arg,rp)
NODE arg;
Obj *rp;
{
	Obj t,n;
	P s;
	DCP dc;
	int id;
	V v;

	if ( !(t = (Obj)ARG0(arg)) || ((id = OID(ARG0(arg))) > O_P) )
		*rp = 0;
	else if ( (n = (Obj)ARG1(arg)) && (OID(n) > O_N) )
		*rp = 0;
	else if ( id == O_N ) {
		if ( !n )
			*rp = t;
		else
			*rp = 0;
	} else {
		if ( argc(arg) == 3 ) {
			if ( (v = VR((P)ARG2(arg))) != VR((P)t) ) {
				getcoef(CO,(P)t,v,(Q)n,(P *)rp); return;
			} else
				s = (P)t;
			if ( VR(s) != v ) {
				if ( n )
					*rp = 0;
				else 
					*rp = t;
				return;
			}
		} else
			s = (P)t;
		for ( dc = DC(s); dc && cmpq(DEG(dc),(Q)n); dc = NEXT(dc) );
		if ( dc )
			*rp = (Obj)COEF(dc);
		else
			*rp = 0;
	}
}

void Pcoef_gf2n(arg,rp)
NODE arg;
Obj *rp;
{
	Obj t,n;
	int id,d;
	UP2 up2;

	if ( !(t = (Obj)ARG0(arg)) || ((id = OID(ARG0(arg))) > O_P) )
		*rp = 0;
	else if ( (n = (Obj)ARG1(arg)) && (OID(n) > O_N) )
		*rp = 0;
	else if ( id == O_N && NID((Num)t) == N_GF2N ) {
		d = QTOS((Q)n);
		up2 = ((GF2N)t)->body;
		if ( d > degup2(up2) )
			*rp = 0;
		else 
			*rp = (Obj)(up2->b[d/BSH]&(((unsigned long)1)<<(d%BSH))?ONE:0);
	} else
		*rp = 0;
}

void Pdeg(arg,rp)
NODE arg;
Q *rp;
{
	Obj t,v;
	int d;

#if 0
	if ( !(t = (Obj)ARG0(arg)) || (OID(t) != O_P) )
		*rp = 0;
	else if ( !(v = (Obj)ARG1(arg)) || (VR((P)v) != VR((P)t)) )
		*rp = 0;
	else 
		*rp = (Obj)DEG(DC((P)t));
#endif
	if ( !(t = (Obj)ARG0(arg)) )
		STOQ(-1,*rp);
	else if ( OID(t) != O_P ) {
		if ( OID(t) == O_N && NID(t) == N_GF2N 
			&& (v=(Obj)ARG1(arg)) && OID(v)== O_N && NID(v) == N_GF2N ) {
			d = degup2(((GF2N)t)->body);
			STOQ(d,*rp);
		} else
			*rp = 0;
	} else
		degp(VR((P)ARG1(arg)),(P)ARG0(arg),rp);
}

void Pmindeg(arg,rp)
NODE arg;
Q *rp;
{
	Obj t;

	if ( !(t = (Obj)ARG0(arg)) || (OID(t) != O_P) )
		*rp = 0;
	else
		getmindeg(VR((P)ARG1(arg)),(P)ARG0(arg),rp);
}

void Psetmod(arg,rp)
NODE arg;
Q *rp;
{
	if ( arg ) {
		asir_assert(ARG0(arg),O_N,"setmod");
		current_mod = QTOS((Q)ARG0(arg));
	}
	STOQ(current_mod,*rp);
}

void Psparsemod_gf2n(arg,rp)
NODE arg;
Q *rp;
{
	int id;

	if ( arg && current_mod_gf2n )
		current_mod_gf2n->id = ARG0(arg)?1:0;
	if ( !current_mod_gf2n )
		id = -1;
	else
		id = current_mod_gf2n->id;
	STOQ(id,*rp);
}

void Pmultest_gf2n(arg,rp)
NODE arg;
GF2N *rp;
{
	GF2N a,b,c;
	int i;

	a = (GF2N)ARG0(arg); b = (GF2N)ARG0(arg);	
	for ( i = 0; i < 10000; i++ )
		mulgf2n(a,b,&c);
	*rp = c;
}

void Psquaretest_gf2n(arg,rp)
NODE arg;
GF2N *rp;
{
	GF2N a,c;
	int i;

	a = (GF2N)ARG0(arg);
	for ( i = 0; i < 10000; i++ )
		squaregf2n(a,&c);
	*rp = c;
}

void Pinvtest_gf2n(arg,rp)
NODE arg;
GF2N *rp;
{
	GF2N a,c;
	int i;

	a = (GF2N)ARG0(arg);
	for ( i = 0; i < 10000; i++ )
		invgf2n(a,&c);
	*rp = c;
}

void Pbininv_gf2n(arg,rp)
NODE arg;
GF2N *rp;
{
	UP2 a,inv;
	int n;

	a = ((GF2N)ARG0(arg))->body;
	n = QTOS((Q)ARG1(arg));
	type1_bin_invup2(a,n,&inv);
	MKGF2N(inv,*rp);
}

void Prinvtest_gf2n(rp)
Real *rp;
{
	GF2N *a;
	GF2N c;
	double t0,t1,r;
	int i;
	double get_clock();

	a = (GF2N *)ALLOCA(1000*sizeof(GF2N));
	for ( i = 0; i < 1000; i++ ) {
		randomgf2n(&a[i]);
	}
	t0 = get_clock();
	for ( i = 0; i < 1000; i++ )
		invgf2n(a[i],&c);
	t1 = get_clock();
	r = (t1-t0)/1000;
	MKReal(r,*rp);
}

void Pfind_root_gf2n(arg,rp)
NODE arg;
GF2N *rp;
{

#if 0
	UP p;

	ptoup((P)ARG0(arg),&p);
	find_root_gf2n(p,rp);
#else
	UP2 p;

	ptoup2((P)ARG0(arg),&p);
	find_root_up2(p,rp);
#endif
}

void Pis_irred_gf2(arg,rp)
NODE arg;
Q *rp;
{
	UP2 t;

	ptoup2(ARG0(arg),&t);
	*rp = irredcheckup2(t) ? ONE : 0;
}

void Pis_irred_ddd_gf2(arg,rp)
NODE arg;
Q *rp;
{
	UP2 t;
	int ret;

	ptoup2(ARG0(arg),&t);
	ret = irredcheck_dddup2(t);
	STOQ(ret,*rp);
}

extern P current_gfs_ext;
extern int current_gfs_p;
extern int current_gfs_q;
extern int current_gfs_q1;
extern int *current_gfs_plus1;
extern int *current_gfs_ntoi;
extern int *current_gfs_iton;

void Psetmod_ff(arg,rp)
NODE arg;
Obj *rp;
{
	int ac;
	int d;
	Obj mod,defpoly;
	N n;
	UP up;
	UP2 up2;
	UM dp;
	Q q,r;
	P p;
	NODE n0,n1;
	LIST list;

	ac = argc(arg);
	if ( ac == 1 ) {
		mod = (Obj)ARG0(arg);
		if ( !mod )
			error("setmod_ff : invalid argument");
		switch ( OID(mod) ) {
			case O_N:
				current_ff = FF_GFP;
				setmod_lm(NM((Q)mod));
				break;
			case O_P:
				current_ff = FF_GF2N;
				setmod_gf2n((P)mod); break;
			default:
				error("setmod_ff : invalid argument");
		}
	} else if ( ac == 2 ) {
		if ( OID(ARG0(arg)) == O_N ) {
			/* small finite field; primitive root representation */
			current_ff = FF_GFS;
			setmod_sf(QTOS((Q)ARG0(arg)),QTOS((Q)ARG1(arg)));
		} else {
			mod = (Obj)ARG1(arg);
			current_ff = FF_GFPN;
			defpoly = (Obj)ARG0(arg);
			if ( !mod || !defpoly )
				error("setmod_ff : invalid argument");
			setmod_lm(NM((Q)mod));
			setmod_gfpn((P)defpoly);
		}
	} else if ( ac == 3 ) {
		/* finite extension of a small finite field */
		current_ff = FF_GFS;
		setmod_sf(QTOS((Q)ARG0(arg)),QTOS((Q)ARG1(arg)));
		d = QTOS((Q)ARG2(arg));
		generate_defpoly_sfum(d,&dp);
		setmod_gfspn(dp);
		current_ff = FF_GFSPN;
	}
	switch ( current_ff ) {
		case FF_GFP:
			getmod_lm(&n); NTOQ(n,1,q); *rp = (Obj)q; break;
		case FF_GF2N:
			getmod_gf2n(&up2); up2top(up2,&p); *rp = (Obj)p; break;
		case FF_GFPN:
			getmod_lm(&n); NTOQ(n,1,q);
			getmod_gfpn(&up); uptop(up,&p);
			MKNODE(n1,q,0); MKNODE(n0,p,n1);
			MKLIST(list,n0);
			*rp = (Obj)list; break;
		case FF_GFS:
			STOQ(current_gfs_p,q);
			if ( current_gfs_ext ) {
				enc_to_p(current_gfs_p,current_gfs_iton[1],
					VR(current_gfs_ext),&p);
				n0 = mknode(3,q,current_gfs_ext,p);
			} else {
				if ( current_gfs_p == 2 )
					r = ONE;
				else
					STOQ(current_gfs_iton[1],r);
				n0 = mknode(3,q,current_gfs_ext,r);
			}
			MKLIST(list,n0);
			*rp = (Obj)list; break;
		default:
			*rp = 0; break;
	}
}

void Pextdeg_ff(rp)
Q *rp;
{
	int d;
	UP2 up2;
	UP up;

	switch ( current_ff ) {
		case FF_GFP:
			*rp = ONE; break;
		case FF_GF2N:
			getmod_gf2n(&up2); d = degup2(up2); STOQ(d,*rp); break;
		case FF_GFPN:
			getmod_gfpn(&up); STOQ(up->d,*rp); break;
		case FF_GFS:
			if ( !current_gfs_ext )
				*rp = ONE;
			else
				*rp = DEG(DC(current_gfs_ext));
			break;
		default:
			error("extdeg_ff : current_ff is not set");
	}
}

void Pcharacteristic_ff(rp)
Q *rp;
{
	N lm;

	switch ( current_ff ) {
		case FF_GFP:
		case FF_GFPN:
			getmod_lm(&lm); NTOQ(lm,1,*rp); break;
		case FF_GF2N:
			STOQ(2,*rp); break;
		case FF_GFS:
			STOQ(current_gfs_p,*rp); break;
		default:
			error("characteristic_ff : current_ff is not set");
	}
}

void Pfield_type_ff(rp)
Q *rp;
{
	STOQ(current_ff,*rp);
}

void Pfield_order_ff(rp)
Q *rp;
{
	N n;

	field_order_ff(&n);
	NTOQ(n,1,*rp);
}

void field_order_ff(order)
N *order;
{
	UP2 up2;
	UP up;
	N m;
	int d,w;

	switch ( current_ff ) {
		case FF_GFP:
			getmod_lm(order); break;
		case FF_GF2N:
			getmod_gf2n(&up2); d = degup2(up2);
			w = (d>>5)+1;
			*order = m = NALLOC(w);
			PL(m)=w;
			bzero((char *)BD(m),w*sizeof(unsigned int));
			BD(m)[d>>5] |= 1<<(d&31);
			break;
		case FF_GFPN:
			getmod_lm(&m);
			getmod_gfpn(&up); pwrn(m,up->d,order); break;
		case FF_GFS:
			STON(current_gfs_q,*order); break;
		default:
			error("field_order_ff : current_ff is not set");
	}
}

void Prandom_ff(rp)
Obj *rp;
{
	LM l;
	GF2N g;
	GFPN p;
	GFS s;

	switch ( current_ff ) {
		case FF_GFP:
			random_lm(&l); *rp = (Obj)l; break;
		case FF_GF2N:
			randomgf2n(&g); *rp = (Obj)g; break;
		case FF_GFPN:
			randomgfpn(&p); *rp = (Obj)p; break;
		case FF_GFS:
			randomgfs(&s); *rp = (Obj)s; break;
		default:
			error("random_ff : current_ff is not set");
	}
}

void Psimp_ff(arg,rp)
NODE arg;
Obj *rp;
{
	LM r;
	GF2N rg;
	extern lm_lazy;

	simp_ff((Obj)ARG0(arg),rp);
}

void simp_ff(p,rp)
Obj p;
Obj *rp;
{
	Num n;
	LM r,s;
	DCP dc,dcr0,dcr;
	GF2N rg,sg;
	GFPN rpn,spn;
	GFS rs;
	GFSPN rspn,sspn;
	P t;
	Obj obj;

	lm_lazy = 0;
	if ( !p )
		*rp = 0;
	else if ( OID(p) == O_N ) {
		switch ( current_ff ) {
			case FF_GFP:
				ptolmp((P)p,&t); simplm((LM)t,&s); *rp = (Obj)s;
				break;
			case FF_GF2N:
				ptogf2n((Obj)p,&rg); simpgf2n((GF2N)rg,&sg); *rp = (Obj)sg;
				break;
			case FF_GFPN:
				ntogfpn((Obj)p,&rpn); simpgfpn((GFPN)rpn,&spn); *rp = (Obj)spn;
				break;
			case FF_GFS:
				if ( NID((Num)p) == N_GFS )
					*rp = p;
				else {
					ptomp(current_gfs_p,(P)p,&t); mqtogfs(t,&rs);
					*rp = (Obj)rs;
				} 
				break;
			case FF_GFSPN:
				ntogfspn((Obj)p,&rspn); simpgfspn((GFSPN)rspn,&sspn);
				*rp = (Obj)sspn;
				break;
			default:
				*rp = (Obj)p;
				break;
		}
	} else if ( OID(p) == O_P ) {
		for ( dc = DC((P)p), dcr0 = 0; dc; dc = NEXT(dc) ) {
			simp_ff((Obj)COEF(dc),&obj);
			if ( obj ) {
				NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = (P)obj;
			}
		}
		if ( !dcr0 ) 
			*rp = 0;
		else {
			NEXT(dcr) = 0; MKP(VR((P)p),dcr0,t); *rp = (Obj)t;
		}
	} else
		error("simp_ff : not implemented yet");
}

void getcoef(vl,p,v,d,r)
VL vl;
P p;
V v;
Q d;
P *r;
{
	P s,t,u,a,b,x;
	DCP dc;
	V w;

	if ( !p )
		*r = 0;
	else if ( NUM(p) )
		*r = d ? 0 : p;
	else if ( (w=VR(p)) == v ) {
		for ( dc = DC(p); dc && cmpq(DEG(dc),d); dc = NEXT(dc) );
		*r = dc ? COEF(dc) : 0;
	} else {
		MKV(w,x);
		for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) ) {
			getcoef(vl,COEF(dc),v,d,&t);
			if ( t ) {
				pwrp(vl,x,DEG(dc),&u); mulp(vl,t,u,&a);
				addp(vl,s,a,&b); s = b;
			}
		}
		*r = s;
	}
}

void Pdeglist(arg,rp)
NODE arg;
LIST *rp;
{
	NODE d;

	getdeglist((P)ARG0(arg),VR((P)ARG1(arg)),&d);
	MKLIST(*rp,d);
}

void Pch_mv(arg,rp)
NODE arg;
P *rp;
{
	change_mvar(CO,(P)ARG0(arg),VR((P)ARG1(arg)),rp);
}

void Pre_mv(arg,rp)
NODE arg;
P *rp;
{
	restore_mvar(CO,(P)ARG0(arg),VR((P)ARG1(arg)),rp);
}

void change_mvar(vl,p,v,r)
VL vl;
P p;
V v;
P *r;
{
	Q d;
	DCP dc,dc0;
	NODE dl;

	if ( !p || NUM(p) || (VR(p) == v) )
		*r = p;
	else {
		getdeglist(p,v,&dl);
		for ( dc0 = 0; dl; dl = NEXT(dl) ) {
			NEXTDC(dc0,dc); DEG(dc) = d = (Q)BDY(dl);
			getcoef(vl,p,v,d,&COEF(dc));
		}
		NEXT(dc) = 0; MKP(v,dc0,*r);
	}
}

void restore_mvar(vl,p,v,r)
VL vl;
P p;
V v;
P *r;
{
	P s,u,a,b,x;
	DCP dc;

	if ( !p || NUM(p) || (VR(p) != v) )
		*r = p;
	else {
		MKV(v,x);
		for ( dc = DC(p), s = 0; dc; dc = NEXT(dc) ) {
			pwrp(vl,x,DEG(dc),&u); mulp(vl,COEF(dc),u,&a);
			addp(vl,s,a,&b); s = b;
		}
		*r = s;
	}
}

void getdeglist(p,v,d)
P p;
V v;
NODE *d;
{
	NODE n,n0,d0,d1,d2;
	DCP dc;

	if ( !p || NUM(p) ) {
		MKNODE(n,0,0); *d = n;
	} else if ( VR(p) == v ) {
		for ( n0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			NEXTNODE(n0,n); BDY(n) = (pointer)DEG(dc);
		}
		NEXT(n) = 0; *d = n0;
	} else {
		for ( dc = DC(p), d0 = 0; dc; dc = NEXT(dc) ) {
			getdeglist(COEF(dc),v,&d1); mergedeglist(d0,d1,&d2); d0 = d2;
		}
		*d = d0;
	}
}
void Pmergelist(arg,rp)
NODE arg;
LIST *rp;
{
    NODE n;

	asir_assert(ARG0(arg),O_LIST,"mergelist");
	asir_assert(ARG1(arg),O_LIST,"mergelist");
	mergedeglist(BDY((LIST)ARG0(arg)),BDY((LIST)ARG1(arg)),&n);
	MKLIST(*rp,n);
}

void mergedeglist(d0,d1,dr)
NODE d0,d1,*dr;
{
	NODE t0,t,dt;
	Q d;
	int c;

	if ( !d0 )
		*dr = d1;
	else {
		while ( d1 ) {
			dt = d1; d1 = NEXT(d1); d = (Q)BDY(dt);
			for ( t0 = 0, t = d0; t; t0 = t, t = NEXT(t) ) {
				c = cmpq(d,(Q)BDY(t));
				if ( !c )
					break;
				else if ( c > 0 ) {
					if ( !t0 ) {
						NEXT(dt) = d0; d0 = dt;
					} else {
						NEXT(t0) = dt; NEXT(dt) = t;	
					}
					break;
				}
			}
			if ( !t ) {
				NEXT(t0) = dt; *dr = d0; return;
			}
		}
		*dr = d0;
	}
}

void Pptomp(arg,rp)
NODE arg;
P *rp;
{
	ptomp(QTOS((Q)ARG1(arg)),(P)ARG0(arg),rp);
}

void Pmptop(arg,rp)
NODE arg;
P *rp;
{
	mptop((P)ARG0(arg),rp);
}

void Pptolmp(arg,rp)
NODE arg;
P *rp;
{
	ptolmp((P)ARG0(arg),rp);
}

void Plmptop(arg,rp)
NODE arg;
P *rp;
{
	lmptop((P)ARG0(arg),rp);
}

void Psf_galois_action(arg,rp)
NODE arg;
P *rp;
{
	sf_galois_action(ARG0(arg),ARG1(arg),rp);
}

/*
  sf_embed(F,B,PM)
  F : an element of GF(pn)
  B : the image of the primitive root of GF(pn)
  PM : order of GF(pm)
*/

void Psf_embed(arg,rp)
NODE arg;
P *rp;
{
	int k,pm;

	/* GF(pn)={0,1,a,a^2,...}->GF(pm)={0,1,b,b^2,...}; a->b^k */
	k = CONT((GFS)ARG1(arg));
	pm = QTOS((Q)ARG2(arg));
	sf_embed((P)ARG0(arg),k,pm,rp);
}

void Psf_log(arg,rp)
NODE arg;
Q *rp;
{
	int k;

	if ( !ARG0(arg) )
		error("sf_log : invalid armument");
	k = CONT((GFS)ARG0(arg));
	STOQ(k,*rp);
}

void Psf_find_root(arg,rp)
NODE arg;
GFS *rp;
{
	P p;
	Obj t;
	int d;
	UM u;
	int *root;

	p = (P)ARG0(arg);
	simp_ff((Obj)p,&t); p = (P)t;
	d = getdeg(VR(p),p);
	u = W_UMALLOC(d);
	ptosfum(p,u);
	root = (int *)ALLOCA(d*sizeof(int));
	find_rootsf(u,root);
	MKGFS(IFTOF(root[0]),*rp);
}

void Psf_minipoly(arg,rp)
NODE arg;
P *rp;
{
	Obj t;
	P p1,p2;
	int d1,d2;
	UM up1,up2,m;

	p1 = (P)ARG0(arg); simp_ff((Obj)p1,&t); p1 = (P)t;
	p2 = (P)ARG1(arg); simp_ff((Obj)p2,&t); p2 = (P)t;
	d1 = getdeg(VR(p1),p1); up1 = W_UMALLOC(d1); ptosfum(p1,up1);
	d2 = getdeg(VR(p2),p2); up2 = W_UMALLOC(d2); ptosfum(p2,up2);
	m = W_UMALLOC(d2);
	minipolysf(up1,up2,m);
	sfumtop(VR(p2),m,&p1);
	sfptop(p1,rp);
}

void Pptosfp(arg,rp)
NODE arg;
P *rp;
{
	ptosfp(ARG0(arg),rp);
}

void Psfptop(arg,rp)
NODE arg;
P *rp;
{
	sfptop((P)ARG0(arg),rp);
}

void Pptogf2n(arg,rp)
NODE arg;
GF2N *rp;
{
	ptogf2n((Obj)ARG0(arg),rp);
}

void Pgf2ntop(arg,rp)
NODE arg;
P *rp;
{
	extern V up2_var;

	if ( argc(arg) == 2 )
		up2_var = VR((P)ARG1(arg));
	gf2ntop((GF2N)ARG0(arg),rp);
}

void Pgf2ntovect(arg,rp)
NODE arg;
VECT *rp;
{
	gf2ntovect((GF2N)ARG0(arg),rp);
}

void Pptogfpn(arg,rp)
NODE arg;
GF2N *rp;
{
	ptogfpn((Obj)ARG0(arg),rp);
}

void Pgfpntop(arg,rp)
NODE arg;
P *rp;
{
	extern V up_var;

	if ( argc(arg) == 2 )
		up_var = VR((P)ARG1(arg));
	gfpntop((GFPN)ARG0(arg),rp);
}

void Pureverse(arg,rp)
NODE arg;
P *rp;
{
	UP p,r;

	ptoup((P)ARG0(arg),&p);
	if ( argc(arg) == 1 )
		reverseup(p,p->d,&r);
	else 
		reverseup(p,QTOS((Q)ARG1(arg)),&r);
	uptop(r,rp);
}

void Putrunc(arg,rp)
NODE arg;
P *rp;
{
	UP p,r;

	ptoup((P)ARG0(arg),&p);
	truncup(p,QTOS((Q)ARG1(arg))+1,&r);
	uptop(r,rp);
}

void Pudecomp(arg,rp)
NODE arg;
LIST *rp;
{
	P u,l;
	UP p,up,low;
	NODE n0,n1;

	ptoup((P)ARG0(arg),&p);
	decompup(p,QTOS((Q)ARG1(arg))+1,&low,&up);
	uptop(low,&l);
	uptop(up,&u);
	MKNODE(n1,u,0); MKNODE(n0,l,n1);
	MKLIST(*rp,n0);
}

void Purembymul(arg,rp)
NODE arg;
P *rp;
{
	UP p1,p2,r;

	if ( !ARG0(arg) || !ARG1(arg) )
		*rp = 0;
	else {
		ptoup((P)ARG0(arg),&p1);
		ptoup((P)ARG1(arg),&p2);
		rembymulup(p1,p2,&r);
		uptop(r,rp);
	}
}

/*
 * d1 = deg(p1), d2 = deg(p2)
 * d1 <= 2*d2-1
 * p2*inv = 1 mod x^d2
 */

void Purembymul_precomp(arg,rp)
NODE arg;
P *rp;
{
	UP p1,p2,inv,r;

	if ( !ARG0(arg) || !ARG1(arg) )
		*rp = 0;
	else {
		ptoup((P)ARG0(arg),&p1);
		ptoup((P)ARG1(arg),&p2);
		ptoup((P)ARG2(arg),&inv);
		if ( p1->d >= 2*p2->d ) {
			error("urembymul_precomp : degree of 1st arg is too large"); 
/*			fprintf(stderr,"urembymul_precomp : degree of 1st arg is too large"); */
			remup(p1,p2,&r);
		} else
			hybrid_rembymulup_special(current_ff,p1,p2,inv,&r);
		uptop(r,rp);
	}
}

void Puinvmod(arg,rp)
NODE arg;
P *rp;
{
	UP p,r;

	ptoup((P)ARG0(arg),&p);
	invmodup(p,QTOS((Q)ARG1(arg)),&r);
	uptop(r,rp);
}

void Purevinvmod(arg,rp)
NODE arg;
P *rp;
{
	UP p,pr,r;

	ptoup((P)ARG0(arg),&p);
	reverseup(p,p->d,&pr);
	invmodup(pr,QTOS((Q)ARG1(arg)),&r);
	uptop(r,rp);
}

void Ppwrmod_ff(arg,rp)
NODE arg;
P *rp;
{
	UP p1,p2;

	ptoup((P)ARG0(arg),&p1);
	switch ( current_ff ) {
		case FF_GFP:
			hybrid_powermodup(p1,&p2); break;
		case FF_GF2N:
			powermodup_gf2n(p1,&p2); break;
		case FF_GFPN:
		case FF_GFS:
			powermodup(p1,&p2); break;
		default:
			error("pwrmod_ff : current_ff is not set");
	}
	uptop(p2,rp);
}

void Pgeneric_pwrmod_ff(arg,rp)
NODE arg;
P *rp;
{
	UP g,f,r;

	ptoup((P)ARG0(arg),&g);
	ptoup((P)ARG1(arg),&f);
	switch ( current_ff ) {
		case FF_GFP:
			hybrid_generic_powermodup(g,f,(Q)ARG2(arg),&r); break;
		case FF_GF2N:
			generic_powermodup_gf2n(g,f,(Q)ARG2(arg),&r); break;
		case FF_GFPN:
		case FF_GFS:
			generic_powermodup(g,f,(Q)ARG2(arg),&r); break;
		default:
			error("generic_pwrmod_ff : current_ff is not set");
	}
	uptop(r,rp);
}

void Ppwrtab_ff(arg,rp)
NODE arg;
VECT *rp;
{
	UP f,xp;
	UP *tab;
	VECT r;
	int i,d;

	ptoup((P)ARG0(arg),&f);
	ptoup((P)ARG1(arg),&xp);
	d = f->d;

	tab = (UP *)ALLOCA(d*sizeof(UP));
	switch ( current_ff ) {
		case FF_GFP:
			hybrid_powertabup(f,xp,tab); break;
		case FF_GF2N:
			powertabup_gf2n(f,xp,tab); break;
		case FF_GFPN:
		case FF_GFS:
			powertabup(f,xp,tab); break;
		default:
			error("pwrtab_ff : current_ff is not set");
	}
	MKVECT(r,d); *rp = r;
	for ( i = 0; i < d; i++ )
		uptop(tab[i],(P *)&BDY(r)[i]);
}

void Pkpwrmod_lm(arg,rp)
NODE arg;
P *rp;
{
	UP p1,p2;

	ptoup((P)ARG0(arg),&p1);
	powermodup(p1,&p2);
	uptop(p2,rp);
}

void Pkgeneric_pwrmod_lm(arg,rp)
NODE arg;
P *rp;
{
	UP g,f,r;

	ptoup((P)ARG0(arg),&g);
	ptoup((P)ARG1(arg),&f);
	generic_powermodup(g,f,(Q)ARG2(arg),&r);
	uptop(r,rp);
}

void Pkpwrtab_lm(arg,rp)
NODE arg;
VECT *rp;
{
	UP f,xp;
	UP *tab;
	VECT r;
	int i,d;

	ptoup((P)ARG0(arg),&f);
	ptoup((P)ARG1(arg),&xp);
	d = f->d;

	tab = (UP *)ALLOCA(d*sizeof(UP));
	powertabup(f,xp,tab);
	MKVECT(r,d); *rp = r;
	for ( i = 0; i < d; i++ )
		uptop(tab[i],(P *)&BDY(r)[i]);
}

void Plazy_lm(arg,rp)
NODE arg;
Q *rp;
{
	lm_lazy = QTOS((Q)ARG0(arg));
	*rp = 0;
}

void Pkmul(arg,rp)
NODE arg;
P *rp;
{
	P n1,n2;
	
	n1 = (P)ARG0(arg); n2 = (P)ARG1(arg);
	asir_assert(n1,O_P,"kmul");
	asir_assert(n2,O_P,"kmul");
	kmulp(CO,n1,n2,rp);
}

void Pksquare(arg,rp)
NODE arg;
P *rp;
{
	P n1;
	
	n1 = (P)ARG0(arg);
	asir_assert(n1,O_P,"ksquare");
	ksquarep(CO,n1,rp);
}

void Pktmul(arg,rp)
NODE arg;
P *rp;
{
	UP p1,p2,r;

	ptoup((P)ARG0(arg),&p1);
	ptoup((P)ARG1(arg),&p2);
	tkmulup(p1,p2,QTOS((Q)ARG2(arg))+1,&r);
	uptop(r,rp);
}

void Pumul(arg,rp)
NODE arg;
P *rp;
{
	P a1,a2;
	UP p1,p2,r;

	a1 = (P)ARG0(arg); a2 = (P)ARG1(arg);
	if ( !a1 || !a2 || NUM(a1) || NUM(a2) )
		mulp(CO,a1,a2,rp);
	else {
		if ( !uzpcheck(a1) || !uzpcheck(a2) || VR(a1) != VR(a2) )
			error("umul : invalid argument");
		ptoup(a1,&p1);
		ptoup(a2,&p2);
		hybrid_mulup(0,p1,p2,&r);
		uptop(r,rp);
	}
}

void Pusquare(arg,rp)
NODE arg;
P *rp;
{
	UP p1,p2,r;

	ptoup((P)ARG0(arg),&p1);
	hybrid_squareup(0,p1,&r);
	uptop(r,rp);
}

void Putmul(arg,rp)
NODE arg;
P *rp;
{
	UP p1,p2,r;

	ptoup((P)ARG0(arg),&p1);
	ptoup((P)ARG1(arg),&p2);
	hybrid_tmulup(0,p1,p2,QTOS((Q)ARG2(arg))+1,&r);
	uptop(r,rp);
}

void Pumul_ff(arg,rp)
NODE arg;
Obj *rp;
{
	P a1,a2;
	UP p1,p2,r;
	P p;

	a1 = (P)ARG0(arg); a2 = (P)ARG1(arg);
	ptoup(a1,&p1);
	ptoup(a2,&p2);
	hybrid_mulup(current_ff,p1,p2,&r);
	uptop(r,&p);
	simp_ff((Obj)p,rp);
}

void Pusquare_ff(arg,rp)
NODE arg;
Obj *rp;
{
	UP p1,p2,r;
	P p;

	ptoup((P)ARG0(arg),&p1);
	hybrid_squareup(current_ff,p1,&r);
	uptop(r,&p);
	simp_ff((Obj)p,rp);
}

void Putmul_ff(arg,rp)
NODE arg;
Obj *rp;
{
	UP p1,p2,r;
	P p;

	ptoup((P)ARG0(arg),&p1);
	ptoup((P)ARG1(arg),&p2);
	hybrid_tmulup(current_ff,p1,p2,QTOS((Q)ARG2(arg))+1,&r);
	uptop(r,&p);
	simp_ff((Obj)p,rp);
}

void Phfmul_lm(arg,rp)
NODE arg;
P *rp;
{
	UP p1,p2,r;
	UP hi,lo,mid,t,s,p10,p11,p20,p21;
	unsigned int m,d;
	int i;

	ptoup((P)ARG0(arg),&p1);
	ptoup((P)ARG1(arg),&p2);
	d = MAX(p1->d,p2->d);
	for ( m = 1; m < d; m <<= 1 );
	if ( m > d )
		m >>= 1;
	if ( d-m < 10000 ) {
		decompup(p1,m,&p10,&p11); /* p1 = p11*x^m+p10 */
		decompup(p2,m,&p20,&p21); /* p2 = p21*x^m+p20 */
		fft_mulup_lm(p10,p20,&lo);
		kmulup(p11,p21,&hi);
		kmulup(p11,p20,&t); kmulup(p10,p21,&s); addup(t,s,&mid);
		r = UPALLOC(2*d);
		bzero((char *)COEF(r),(2*d+1)*sizeof(Num));
		if ( lo )
			bcopy((char *)COEF(lo),(char *)COEF(r),(DEG(lo)+1)*sizeof(Num));
		if ( hi )
			bcopy((char *)COEF(hi),(char *)(COEF(r)+2*m),(DEG(hi)+1)*sizeof(Num));
		for ( i = 2*d; i >= 0 && !COEF(r)[i]; i-- );
		if ( i < 0 )
			r = 0;
		else {
			DEG(r) = i;
			t = UPALLOC(DEG(mid)+m); DEG(t) = DEG(mid)+m;
			bzero((char *)COEF(t),(DEG(mid)+m+1)*sizeof(Num));
			bcopy((char *)COEF(mid),(char *)(COEF(t)+m),(DEG(mid)+1)*sizeof(Num));
			addup(t,r,&s);
			r = s;
		}
	} else
		fft_mulup_lm(p1,p2,&r);
	uptop(r,rp);
}

void Pfmultest(arg,rp)
NODE arg;
LIST *rp;
{
	P p1,p2,r;
	int d1,d2;
	UM w1,w2,wr;
	unsigned int *f1,*f2,*fr,*w;
	int index,mod,root,d,maxint,i;
	int cond;
	Q prime;
	NODE n0,n1;

	p1 = (P)ARG0(arg); p2 = (P)ARG1(arg); index = QTOS((Q)ARG2(arg));
	FFT_primes(index,&mod,&root,&d);
	maxint = 1<<d;
	d1 = UDEG(p1); d2 = UDEG(p2);
	if ( maxint < d1+d2+1 )
		*rp = 0;
	else {
		w1 = W_UMALLOC(d1); w2 = W_UMALLOC(d2);
		wr = W_UMALLOC(d1+d2);
		ptoum(mod,p1,w1); ptoum(mod,p2,w2);
		f1 = (unsigned int *)ALLOCA(maxint*sizeof(unsigned int));
		f2 = (unsigned int *)ALLOCA(maxint*sizeof(unsigned int));
		fr = (unsigned int *)ALLOCA(maxint*sizeof(unsigned int));
		w = (unsigned int *)ALLOCA(12*maxint*sizeof(unsigned int));

		for ( i = 0; i <= d1; i++ )
			f1[i] = (unsigned int)w1->c[i];
		for ( i = 0; i <= d2; i++ )
			f2[i] = (unsigned int)w2->c[i];
	
		cond = FFT_pol_product(d1,f1,d2,f2,fr,index,w);
		if ( cond )
			error("fmultest : ???");
		wr->d = d1+d2;
		for ( i = 0; i <= d1+d2; i++ )
			wr->c[i] = (unsigned int)fr[i];
		umtop(VR(p1),wr,&r);
		STOQ(mod,prime);
		MKNODE(n1,prime,0);
		MKNODE(n0,r,n1);
		MKLIST(*rp,n0);
	}
}

void Pkmulum(arg,rp)
NODE arg;
P *rp;
{
	P p1,p2;
	int d1,d2,mod;
	UM w1,w2,wr;

	p1 = (P)ARG0(arg); p2 = (P)ARG1(arg); mod = QTOS((Q)ARG2(arg));
	d1 = UDEG(p1); d2 = UDEG(p2);
	w1 = W_UMALLOC(d1); w2 = W_UMALLOC(d2);
	wr = W_UMALLOC(d1+d2);
	ptoum(mod,p1,w1); ptoum(mod,p2,w2);
	kmulum(mod,w1,w2,wr);
	umtop(VR(p1),wr,rp);
}

void Pksquareum(arg,rp)
NODE arg;
P *rp;
{
	P p1;
	int d1,mod;
	UM w1,wr;

	p1 = (P)ARG0(arg); mod = QTOS((Q)ARG1(arg));
	d1 = UDEG(p1);
	w1 = W_UMALLOC(d1);
	wr = W_UMALLOC(2*d1);
	ptoum(mod,p1,w1);
	kmulum(mod,w1,w1,wr);
	umtop(VR(p1),wr,rp);
}

void Ptracemod_gf2n(arg,rp)
NODE arg;
P *rp;
{
	UP g,f,r;

	ptoup((P)ARG0(arg),&g);
	ptoup((P)ARG1(arg),&f);
	tracemodup_gf2n(g,f,(Q)ARG2(arg),&r);
	uptop(r,rp);
}

void Pumul_specialmod(arg,rp)
NODE arg;
P *rp;
{
	P a1,a2;
	UP p1,p2,r;
	int i,nmod;
	int *modind;
	NODE t,n;

	a1 = (P)ARG0(arg); a2 = (P)ARG1(arg);
	if ( !a1 || !a2 || NUM(a1) || NUM(a2) )
		mulp(CO,a1,a2,rp);
	else {
		if ( !uzpcheck(a1) || !uzpcheck(a2) || VR(a1) != VR(a2) )
			error("umul_specialmod : invalid argument");
		ptoup(a1,&p1);
		ptoup(a2,&p2);
		n = BDY((LIST)ARG2(arg));
		nmod = length(n);
		modind = (int *)MALLOC_ATOMIC(nmod*sizeof(int));
		for ( i = 0, t = n; i < nmod; i++, t = NEXT(t) )
			modind[i] = QTOS((Q)BDY(t));
		fft_mulup_specialmod_main(p1,p2,0,modind,nmod,&r);
		uptop(r,rp);
	}
}

void Pusquare_specialmod(arg,rp)
NODE arg;
P *rp;
{
	P a1;
	UP p1,r;
	int i,nmod;
	int *modind;
	NODE t,n;

	a1 = (P)ARG0(arg);
	if ( !a1 || NUM(a1) )
		mulp(CO,a1,a1,rp);
	else {
		if ( !uzpcheck(a1) )
			error("usquare_specialmod : invalid argument");
		ptoup(a1,&p1);
		n = BDY((LIST)ARG1(arg));
		nmod = length(n);
		modind = (int *)MALLOC_ATOMIC(nmod*sizeof(int));
		for ( i = 0, t = n; i < nmod; i++, t = NEXT(t) )
			modind[i] = QTOS((Q)BDY(t));
		fft_mulup_specialmod_main(p1,p1,0,modind,nmod,&r);
		uptop(r,rp);
	}
}

void Putmul_specialmod(arg,rp)
NODE arg;
P *rp;
{
	P a1,a2;
	UP p1,p2,r;
	int i,nmod;
	int *modind;
	NODE t,n;

	a1 = (P)ARG0(arg); a2 = (P)ARG1(arg);
	if ( !a1 || !a2 || NUM(a1) || NUM(a2) )
		mulp(CO,a1,a2,rp);
	else {
		if ( !uzpcheck(a1) || !uzpcheck(a2) || VR(a1) != VR(a2) )
			error("utmul_specialmod : invalid argument");
		ptoup(a1,&p1);
		ptoup(a2,&p2);
		n = BDY((LIST)ARG3(arg));
		nmod = length(n);
		modind = (int *)MALLOC_ATOMIC(nmod*sizeof(int));
		for ( i = 0, t = n; i < nmod; i++, t = NEXT(t) )
			modind[i] = QTOS((Q)BDY(t));
		fft_mulup_specialmod_main(p1,p2,QTOS((Q)ARG2(arg))+1,modind,nmod,&r);
		uptop(r,rp);
	}
}
