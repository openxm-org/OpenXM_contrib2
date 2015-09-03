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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/array.c,v 1.68 2015/08/14 13:51:54 fujimoto Exp $
*/
#include "ca.h"
#include "base.h"
#include "parse.h"
#include "inline.h"

#include <sys/types.h>
#include <sys/stat.h>
#if !defined(_MSC_VER)
#include <unistd.h>
#endif

#define F4_INTRAT_PERIOD 8 

#if 0
#undef DMAR
#define DMAR(a1,a2,a3,d,r) (r)=dmar(a1,a2,a3,d);
#endif

extern int DP_Print; /* XXX */


void Pnewvect(), Pnewmat(), Psepvect(), Psize(), Pdet(), Pleqm(), Pleqm1(), Pgeninvm();
void Pinvmat();
void Pnewbytearray(),Pmemoryplot_to_coord();

void Pgeneric_gauss_elim();
void Pgeneric_gauss_elim_mod();

void Pindep_rows_mod();

void Pmat_to_gfmmat(),Plu_gfmmat(),Psolve_by_lu_gfmmat();
void Pgeninvm_swap(), Premainder(), Psremainder(), Pvtol(), Pltov();
void Pgeninv_sf_swap();
void sepvect();
void Pmulmat_gf2n();
void Pbconvmat_gf2n();
void Pmul_vect_mat_gf2n();
void PNBmul_gf2n();
void Pmul_mat_vect_int();
void Psepmat_destructive();
void Px962_irredpoly_up2();
void Pirredpoly_up2();
void Pnbpoly_up2();
void Pqsort();
void Pexponent_vector();
void Pmat_swap_row_destructive();
void Pmat_swap_col_destructive();
void Pvect();
void Pmat();
void Pmatc();
void Pnd_det();
void Plu_mat();
void Pmat_col();
void Plusolve_prep();
void Plusolve_main();

struct ftab array_tab[] = {
	{"lu_mat",Plu_mat,1},
	{"solve_by_lu_gfmmat",Psolve_by_lu_gfmmat,4},
	{"lu_gfmmat",Plu_gfmmat,2},
	{"mat_to_gfmmat",Pmat_to_gfmmat,2},
	{"generic_gauss_elim",Pgeneric_gauss_elim,1},
	{"generic_gauss_elim_mod",Pgeneric_gauss_elim_mod,2},
	{"indep_rows_mod",Pindep_rows_mod,2},
	{"newvect",Pnewvect,-2},
	{"vect",Pvect,-99999999},
	{"vector",Pnewvect,-2},
	{"exponent_vector",Pexponent_vector,-99999999},
	{"newmat",Pnewmat,-3},
	{"matrix",Pnewmat,-3},
	{"mat",Pmat,-99999999},
	{"matr",Pmat,-99999999},
	{"matc",Pmatc,-99999999},
	{"newbytearray",Pnewbytearray,-2},
	{"memoryplot_to_coord",Pmemoryplot_to_coord,1},
	{"sepmat_destructive",Psepmat_destructive,2},
	{"sepvect",Psepvect,2},
	{"qsort",Pqsort,-2},
	{"vtol",Pvtol,1},
	{"ltov",Pltov,1},
	{"size",Psize,1},
	{"det",Pdet,-2},
	{"nd_det",Pnd_det,-2},
	{"invmat",Pinvmat,-2},
	{"leqm",Pleqm,2},
	{"leqm1",Pleqm1,2},
	{"geninvm",Pgeninvm,2},
	{"geninvm_swap",Pgeninvm_swap,2},
	{"geninv_sf_swap",Pgeninv_sf_swap,1},
	{"remainder",Premainder,2},
	{"sremainder",Psremainder,2},
	{"mulmat_gf2n",Pmulmat_gf2n,1},
	{"bconvmat_gf2n",Pbconvmat_gf2n,-4},
	{"mul_vect_mat_gf2n",Pmul_vect_mat_gf2n,2},
	{"mul_mat_vect_int",Pmul_mat_vect_int,2},
	{"nbmul_gf2n",PNBmul_gf2n,3},
	{"x962_irredpoly_up2",Px962_irredpoly_up2,2},
	{"irredpoly_up2",Pirredpoly_up2,2},
	{"nbpoly_up2",Pnbpoly_up2,2},
	{"mat_swap_row_destructive",Pmat_swap_row_destructive,3},
	{"mat_swap_col_destructive",Pmat_swap_col_destructive,3},
	{"mat_col",Pmat_col,2},
	{"lusolve_prep",Plusolve_prep,1},
	{"lusolve_main",Plusolve_main,1},
	{0,0,0},
};

typedef struct _ent { int j; unsigned int e; } ent;

ent *get_row(FILE *,int *l);
void put_row(FILE *out,int l,ent *a);
int lu_elim(int *l,ent **a,int k,int i,int mul,int mod);

static int *ul,*ll;
static ent **u,**l;
static int modulus;

void Plusolve_prep(NODE arg,Q *rp)
{
	char *fname;
	FILE *in;
	int len,i,rank;
	int *rhs;

	fname = BDY((STRING)ARG0(arg));
	in = fopen(fname,"r");
	modulus = getw(in);
	len = getw(in);
	ul = (int *)MALLOC_ATOMIC(len*sizeof(int));
	u = (ent **)MALLOC(len*sizeof(ent *));
	ll = (int *)MALLOC_ATOMIC(len*sizeof(int));
	l = (ent **)MALLOC(len*sizeof(ent *));
	for ( i = 0; i < len; i++ ) {
		u[i] = get_row(in,&ul[i]);
	}
	for ( i = 0; i < len; i++ ) {
		l[i] = get_row(in,&ll[i]);
	}
	fclose(in);
	*rp = ONE;
}

void Plusolve_main(NODE arg,VECT *rp)
{
	Q *d,*p;
	VECT v,r;
	int len,i;
	int *rhs;

	v = (VECT)ARG0(arg); len = v->len;
	d = (Q *)BDY(v);
	rhs = (int *)MALLOC_ATOMIC(len*sizeof(int));
	for ( i = 0; i < len; i++ ) rhs[i] = QTOS(d[i]);
	solve_l(ll,l,len,rhs,modulus);
	solve_u(ul,u,len,rhs,modulus);
	NEWVECT(r); r->len = len;
	r->body = (pointer *)MALLOC(len*sizeof(pointer));
	p = (Q *)r->body;
	for ( i = 0; i < len; i++ )
		STOQ(rhs[i],p[i]);	
	*rp = r;
}

ent *get_row(FILE *in,int *l)
{
	int len,i;
	ent *a;

	*l = len = getw(in);
	a = (ent *)MALLOC_ATOMIC(len*sizeof(ent));
	for ( i = 0; i < len; i++ ) {
		a[i].j = getw(in);
		a[i].e = getw(in);
	}
	return a;
}

int lu_gauss(int *ul,ent **u,int *ll,ent **l,int n,int mod)
{
	int i,j,k,s,mul;
	unsigned int inv;
	int *ll2;

	ll2 = (int *)MALLOC_ATOMIC(n*sizeof(int));
	for ( i = 0; i < n; i++ ) ll2[i] = 0;
	for ( i = 0; i < n; i++ ) {
		fprintf(stderr,"i=%d\n",i);
		inv = invm(u[i][0].e,mod);
		for ( k = i+1; k < n; k++ )
			if ( u[k][0].j == n-i ) {
				s = u[k][0].e;
				DMAR(s,inv,0,mod,mul);
				lu_elim(ul,u,k,i,mul,mod);
				lu_append(ll,l,ll2,k,i,mul);
			}
	}
}

#define INITLEN 10

lu_append(int *l,ent **a,int *l2,int k,int i,int mul)
{
	int len;
	ent *p;

	len = l[k]; 
	if ( !len ) {
		a[k] = p = (ent *)MALLOC_ATOMIC(INITLEN*sizeof(ent));
		p[0].j = i; p[0].e = mul;
		l[k] = 1; l2[k] = INITLEN;
	} else {
		if ( l2[k] == l[k] ) {
			l2[k] *= 2;
			a[k] = REALLOC(a[k],l2[k]*sizeof(ent));
		}
		p =a[k];
		p[l[k]].j = i; p[l[k]].e = mul;
		l[k]++;
	}
}

/* a[k] = a[k]-mul*a[i] */ 

int lu_elim(int *l,ent **a,int k,int i,int mul,int mod)
{
	ent *ak,*ai,*w;
	int lk,li,j,m,p,q,r,s,t,j0;

	ak = a[k]; ai = a[i]; lk = l[k]; li = l[i];
	w = (ent *)alloca((lk+li)*sizeof(ent));
	p = 0; q = 0; j = 0;
	mul = mod-mul;
	while ( p < lk && q < li ) {
		if ( ak[p].j > ai[q].j ) {
			w[j] = ak[p]; j++; p++;
		} else if ( ak[p].j < ai[q].j ) {
			w[j].j = ai[q].j;
			t = ai[q].e;
			DMAR(t,mul,0,mod,r);
			w[j].e = r;
			j++; q++;
		} else {
			t = ai[q].e; s = ak[p].e;
			DMAR(t,mul,s,mod,r);
			if ( r ) {
				w[j].j = ai[q].j; w[j].e = r; j++;
			}
			p++; q++;
		}
	}
	if ( q == li )
		while ( p < lk ) {
			w[j] = ak[p]; j++; p++;
		}
	else if ( p == lk )
		while ( q < li ) {
			w[j].j = ai[q].j;
			t = ai[q].e;
			DMAR(t,mul,0,mod,r);
			w[j].e = r;
			j++; q++;
		}
	if ( j <= lk ) {
		for ( m = 0; m < j; m++ ) ak[m] = w[m];
	} else {
		a[k] = ak = (ent *)MALLOC_ATOMIC(j*sizeof(ent));
		for ( m = 0; m < j; m++ ) ak[m] = w[m];
	}
	l[k] = j;
}

int solve_l(int *ll,ent **l,int n,int *rhs,int mod)
{
	int j,k,s,len;
	ent *p;

	for ( j = 0; j < n; j++ ) {
		len = ll[j]; p = l[j];
		for ( k = 0, s = 0; k < len; k++ )
			s = dmar(p[k].e,rhs[p[k].j],s,mod);
		rhs[j] -=  s;
		if ( rhs[j] < 0 ) rhs[j] += mod;
	}
}

int solve_u(int *ul,ent **u,int n,int *rhs,int mod)
{
	int j,k,s,len,inv;
	ent *p;

	for ( j = n-1; j >= 0; j-- ) {
		len = ul[j]; p = u[j];
		for ( k = 1, s = 0; k < len; k++ )
			s = dmar(p[k].e,rhs[p[k].j],s,mod);
		rhs[j] -=  s;
		if ( rhs[j] < 0 ) rhs[j] += mod;
		inv = invm((unsigned int)p[0].e,mod);
		rhs[j] = dmar(rhs[j],inv,0,mod);
	}
}

int comp_obj(Obj *a,Obj *b)
{
	return arf_comp(CO,*a,*b);
}

static FUNC generic_comp_obj_func;
static NODE generic_comp_obj_arg;
static NODE generic_comp_obj_option;

int generic_comp_obj(Obj *a,Obj *b)
{
	Q r;
	
	BDY(generic_comp_obj_arg)=(pointer)(*a);
	BDY(NEXT(generic_comp_obj_arg))=(pointer)(*b);
	r = (Q)bevalf_with_opts(generic_comp_obj_func,generic_comp_obj_arg,generic_comp_obj_option);
	if ( !r )
		return 0;
	else
		return SGN(r)>0?1:-1;
}


void Pqsort(NODE arg,LIST *rp)
{
	VECT vect;
	NODE n,n1;
	P p;
	V v;
	FUNC func;
	int len,i;
	pointer *a;
	Obj t;

	t = ARG0(arg);
    if (OID(t) == O_LIST) {
        n = (NODE)BDY((LIST)t);
        len = length(n);
        MKVECT(vect,len);
        for ( i = 0; i < len; i++, n = NEXT(n) ) {
            BDY(vect)[i] = BDY(n);
        }
        
    }else if (OID(t) != O_VECT) {
        error("qsort : invalid argument");
    }else {
        vect = (VECT)t;
    }
	if ( argc(arg) == 1 )
		qsort(BDY(vect),vect->len,sizeof(Obj),(int (*)(const void *,const void *))comp_obj);
	else {
		p = (P)ARG1(arg);
		if ( !p || OID(p)!=2 )
			error("qsort : invalid argument");
		v = VR(p);
		gen_searchf(NAME(v),&func);
		if ( !func ) {
			if ( (int)v->attr != V_SR )
				error("qsort : no such function");
			func = (FUNC)v->priv;
		}
		generic_comp_obj_func = func;
		MKNODE(n,0,0); MKNODE(generic_comp_obj_arg,0,n);
		generic_comp_obj_option = current_option;
		qsort(BDY(vect),vect->len,sizeof(Obj),(int (*)(const void *,const void *))generic_comp_obj);
	}
    if (OID(t) == O_LIST) {
        a = BDY(vect);
        for ( i = len - 1, n = 0; i >= 0; i-- ) {
            MKNODE(n1,a[i],n); n = n1;
        }
        MKLIST(*rp,n);
    }else {
        *rp = (LIST)vect;
    }
}

void PNBmul_gf2n(NODE arg,GF2N *rp)
{
	GF2N a,b;
	GF2MAT mat;
	int n,w;
	unsigned int *ab,*bb;
	UP2 r;

	a = (GF2N)ARG0(arg);
	b = (GF2N)ARG1(arg);
	mat = (GF2MAT)ARG2(arg);
	if ( !a || !b )
		*rp = 0;
	else {
		n = mat->row;
		w = (n+BSH-1)/BSH;

		ab = (unsigned int *)ALLOCA(w*sizeof(unsigned int));
		bzero((char *)ab,w*sizeof(unsigned int));
		bcopy(a->body->b,ab,(a->body->w)*sizeof(unsigned int));

		bb = (unsigned int *)ALLOCA(w*sizeof(unsigned int));
		bzero((char *)bb,w*sizeof(unsigned int));
		bcopy(b->body->b,bb,(b->body->w)*sizeof(unsigned int));

		NEWUP2(r,w);
		bzero((char *)r->b,w*sizeof(unsigned int));
		mul_nb(mat,ab,bb,r->b);
		r->w = w;
		_adjup2(r);
		if ( !r->w )
			*rp = 0;
		else
			MKGF2N(r,*rp);
	}
}

void Pmul_vect_mat_gf2n(NODE arg,GF2N *rp)
{
	GF2N a;
	GF2MAT mat;
	int n,w;
	unsigned int *b;
	UP2 r;

	a = (GF2N)ARG0(arg);
	mat = (GF2MAT)ARG1(arg);
	if ( !a )
		*rp = 0;
	else {
		n = mat->row;
		w = (n+BSH-1)/BSH;
		b = (unsigned int *)ALLOCA(w*sizeof(unsigned int));
		bzero((char *)b,w*sizeof(unsigned int));
		bcopy(a->body->b,b,(a->body->w)*sizeof(unsigned int));
		NEWUP2(r,w);
		bzero((char *)r->b,w*sizeof(unsigned int));
		mulgf2vectmat(mat->row,b,mat->body,r->b);
		r->w = w;
		_adjup2(r);
		if ( !r->w )
			*rp = 0;
		else {
			MKGF2N(r,*rp);
		}
	}
}

void Pbconvmat_gf2n(NODE arg,LIST *rp)
{
	P p0,p1;
	int to;
	GF2MAT p01,p10;
	GF2N root;
	NODE n0,n1;

	p0 = (P)ARG0(arg);
	p1 = (P)ARG1(arg);
	to = ARG2(arg)?1:0;
	if ( argc(arg) == 4 ) {
		root = (GF2N)ARG3(arg);
		compute_change_of_basis_matrix_with_root(p0,p1,to,root,&p01,&p10);
	} else
		compute_change_of_basis_matrix(p0,p1,to,&p01,&p10);
	MKNODE(n1,p10,0); MKNODE(n0,p01,n1);	
	MKLIST(*rp,n0);
}

void Pmulmat_gf2n(NODE arg,GF2MAT *rp)
{
	GF2MAT m;

	if ( !compute_multiplication_matrix((P)ARG0(arg),&m) )
		error("mulmat_gf2n : input is not a normal polynomial");
	*rp = m;
}

void Psepmat_destructive(NODE arg,LIST *rp)
{
	MAT mat,mat1;
	int i,j,row,col;
	Q **a,**a1;
	Q ent;
	N nm,mod,rem,quo;
	int sgn;
	NODE n0,n1;

	mat = (MAT)ARG0(arg); mod = NM((Q)ARG1(arg));
	row = mat->row; col = mat->col;
	MKMAT(mat1,row,col);
	a = (Q **)mat->body; a1 = (Q **)mat1->body;
	for ( i = 0; i < row; i++ )
		for ( j = 0; j < col; j++ ) {
			ent = a[i][j];
			if ( !ent )
				continue;
			nm = NM(ent);
			sgn = SGN(ent);
			divn(nm,mod,&quo,&rem);
/*			if ( quo != nm && rem != nm ) */
/*				GCFREE(nm); */
/*			GCFREE(ent); */
			NTOQ(rem,sgn,a[i][j]); NTOQ(quo,sgn,a1[i][j]);	
		}
	MKNODE(n1,mat1,0); MKNODE(n0,mat,n1);
	MKLIST(*rp,n0);
}

void Psepvect(NODE arg,VECT *rp)
{
	sepvect((VECT)ARG0(arg),QTOS((Q)ARG1(arg)),rp);
}

void sepvect(VECT v,int d,VECT *rp)
{
	int i,j,k,n,q,q1,r;
	pointer *pv,*pw,*pu;
	VECT w,u;

	n = v->len;
	if ( d > n )
		d = n;
	q = n/d; r = n%d; q1 = q+1;
	MKVECT(w,d); *rp = w;
	pv = BDY(v); pw = BDY(w); k = 0;
	for ( i = 0; i < r; i++ ) {
		MKVECT(u,q1); pw[i] = (pointer)u;
		for ( pu = BDY(u), j = 0; j < q1; j++, k++ )
			pu[j] = pv[k];
	}
	for ( ; i < d; i++ ) {
		MKVECT(u,q); pw[i] = (pointer)u;
		for ( pu = BDY(u), j = 0; j < q; j++, k++ )
			pu[j] = pv[k];
	}
}

void Pnewvect(NODE arg,VECT *rp)
{
	int len,i,r;
	VECT vect;
	pointer *vb;
	LIST list;
	NODE tn;

	asir_assert(ARG0(arg),O_N,"newvect");
	len = QTOS((Q)ARG0(arg)); 
	if ( len < 0 )
		error("newvect : invalid size");
	MKVECT(vect,len);
	if ( argc(arg) == 2 ) {
		list = (LIST)ARG1(arg);
		asir_assert(list,O_LIST,"newvect");
#if 0
		for ( r = 0, tn = BDY(list); tn; r++, tn = NEXT(tn) );
		if ( r > len ) {
			*rp = vect;
			return;
		}
#endif
		for ( i = 0, tn = BDY(list), vb = BDY(vect); tn; i++, tn = NEXT(tn) )
			vb[i] = (pointer)BDY(tn);
	}
	*rp = vect;
}

void Pvect(NODE arg,VECT *rp) {
	int len,i;
	VECT vect;
	pointer *vb;
	NODE tn;

	if ( !arg ) {
		*rp =0;
		return;
	}

	for (len = 0, tn = arg; tn; tn = NEXT(tn), len++);
	if ( len == 1 ) {
		if ( ARG0(arg) != 0 ) {
			switch ( OID(ARG0(arg)) ) {
				case O_VECT:
					*rp = ARG0(arg);
					return;
				case O_LIST:
					for ( len = 0, tn = ARG0(arg); tn; tn = NEXT(tn), len++ );
					MKVECT(vect,len-1);
					for ( i = 0, tn = BDY((LIST)ARG0(arg)), vb =BDY(vect);
							tn; i++, tn = NEXT(tn) )
						vb[i] = (pointer)BDY(tn);
					*rp=vect;
					return;
			}
		}
	}
	MKVECT(vect,len);
	for ( i = 0, tn = arg, vb = BDY(vect); tn; i++, tn = NEXT(tn) )
		vb[i] = (pointer)BDY(tn);
	*rp = vect;
}

void Pexponent_vector(NODE arg,DP *rp)
{
	nodetod(arg,rp);
}

void Pnewbytearray(NODE arg,BYTEARRAY *rp)
{
	int len,i,r;
	BYTEARRAY array;
	unsigned char *vb;
	char *str;
	LIST list;
	NODE tn;
	int ac;
	struct stat sbuf;
	char *fname;
	FILE *fp;

	ac = argc(arg);
	if ( ac == 1 ) {
		if ( !OID((Obj)ARG0(arg)) ) error("newbytearray : invalid argument");
		switch ( OID((Obj)ARG0(arg)) ) {
			case O_STR:
				fname = BDY((STRING)ARG0(arg));
				fp = fopen(fname,"rb");
				if ( !fp ) error("newbytearray : fopen failed");
				if ( stat(fname,&sbuf) < 0 ) 
					error("newbytearray : stat failed");
				len = sbuf.st_size;
				MKBYTEARRAY(array,len);
				fread(BDY(array),len,sizeof(char),fp);
				break;
			case O_N:
				if ( !RATN(ARG0(arg)) )
					error("newbytearray : invalid argument");
				len = QTOS((Q)ARG0(arg)); 
				if ( len < 0 )
					error("newbytearray : invalid size");
				MKBYTEARRAY(array,len);
				break;
			default:
				error("newbytearray : invalid argument");
		}
	} else if ( ac == 2 ) {
		asir_assert(ARG0(arg),O_N,"newbytearray");
		len = QTOS((Q)ARG0(arg)); 
		if ( len < 0 )
			error("newbytearray : invalid size");
		MKBYTEARRAY(array,len);
		if ( !ARG1(arg) )
			error("newbytearray : invalid initialization");
		switch ( OID((Obj)ARG1(arg)) ) {
			case O_LIST:
				list = (LIST)ARG1(arg);
				asir_assert(list,O_LIST,"newbytearray");
				for ( r = 0, tn = BDY(list); tn; r++, tn = NEXT(tn) );
				if ( r <= len ) {
					for ( i = 0, tn = BDY(list), vb = BDY(array); tn; 
						i++, tn = NEXT(tn) )
						vb[i] = (unsigned char)QTOS((Q)BDY(tn));
				}
				break;
			case O_STR:
				str = BDY((STRING)ARG1(arg));
				r = strlen(str);
				if ( r <= len )
					bcopy(str,BDY(array),r);
				break;
			default:
				if ( !ARG1(arg) )
					error("newbytearray : invalid initialization");
		}
	} else
		error("newbytearray : invalid argument");
	*rp = array;
}

#define MEMORY_GETPOINT(a,len,x,y) (((a)[(len)*(y)+((x)>>3)])&(1<<((x)&7)))

void Pmemoryplot_to_coord(NODE arg,LIST *rp)
{
	int len,blen,y,i,j;
	unsigned char *a;
	NODE r0,r,n;
	LIST l;
	BYTEARRAY ba;
	Q iq,jq;

	asir_assert(ARG0(arg),O_LIST,"memoryplot_to_coord");
	arg = BDY((LIST)ARG0(arg));
	len = QTOS((Q)ARG0(arg));
	blen = (len+7)/8;
	y = QTOS((Q)ARG1(arg));
	ba = (BYTEARRAY)ARG2(arg); a = ba->body;
	r0 = 0;
	for ( j = 0; j < y; j++ )
		for ( i = 0; i < len; i++ )
			if ( MEMORY_GETPOINT(a,blen,i,j) ) {
				NEXTNODE(r0,r);
				STOQ(i,iq); STOQ(j,jq);
				n = mknode(2,iq,jq);
				MKLIST(l,n);
				BDY(r) = l;
			}
	if ( r0 ) NEXT(r) = 0;
	MKLIST(*rp,r0);
}

void Pnewmat(NODE arg,MAT *rp)
{
	int row,col;
	int i,j,r,c;
	NODE tn,sn;
	MAT m;
	pointer **mb;
	LIST list;

	asir_assert(ARG0(arg),O_N,"newmat");
	asir_assert(ARG1(arg),O_N,"newmat");
	row = QTOS((Q)ARG0(arg)); col = QTOS((Q)ARG1(arg));
	if ( row < 0 || col < 0 )
		error("newmat : invalid size");
	MKMAT(m,row,col);
	if ( argc(arg) == 3 ) {
		list = (LIST)ARG2(arg);
		asir_assert(list,O_LIST,"newmat");
		for ( r = 0, c = 0, tn = BDY(list); tn; r++, tn = NEXT(tn) ) {
			for ( j = 0, sn = BDY((LIST)BDY(tn)); sn; j++, sn = NEXT(sn) );
			c = MAX(c,j);
		}
		if ( (r > row) || (c > col) ) {
			*rp = m;
			return;
		}
		for ( i = 0, tn = BDY(list), mb = BDY(m); tn; i++, tn = NEXT(tn) ) {
			asir_assert(BDY(tn),O_LIST,"newmat");
			for ( j = 0, sn = BDY((LIST)BDY(tn)); sn; j++, sn = NEXT(sn) )
				mb[i][j] = (pointer)BDY(sn);
		}
	}
	*rp = m;
}

void Pmat(NODE arg, MAT *rp)
{
	int row,col;
	int i;
	MAT m;
	pointer **mb;
	pointer *ent;
	NODE tn, sn;
	VECT v;

	if ( !arg ) {
		*rp =0;
		return;
	}

	for (row = 0, tn = arg; tn; tn = NEXT(tn), row++);
	if ( row == 1 ) {
		if ( OID(ARG0(arg)) == O_MAT ) {
			*rp=ARG0(arg);
			return;
		} else if ( !(OID(ARG0(arg)) == O_LIST || OID(ARG0(arg)) == O_VECT)) {
			error("mat : invalid argument");
		}
	}
	if ( OID(ARG0(arg)) == O_VECT ) {
		v = ARG0(arg);
		col = v->len;
	} else if ( OID(ARG0(arg)) == O_LIST ) {
		for (col = 0, tn = BDY((LIST)ARG0(arg)); tn ; tn = NEXT(tn), col++);
	} else {
		error("mat : invalid argument");
	}

	MKMAT(m,row,col);
	for (row = 0, tn = arg, mb = BDY(m); tn; tn = NEXT(tn), row++) {
		if ( BDY(tn) == 0 ) {
			error("mat : invalid argument");
		} else if ( OID(BDY(tn)) == O_VECT ) {
			v = tn->body;
			ent = BDY(v);
			for (i = 0; i < v->len; i++ ) mb[row][i] = (Obj)ent[i];
		} else if ( OID(BDY(tn)) == O_LIST ) {
			for (col = 0, sn = BDY((LIST)BDY(tn)); sn; col++, sn = NEXT(sn) )
				mb[row][col] = (pointer)BDY(sn);
		} else {
			error("mat : invalid argument");
		}
	}
	*rp = m;
}

void Pmatc(NODE arg, MAT *rp)
{
	int row,col;
	int i;
	MAT m;
	pointer **mb;
	pointer *ent;
	NODE tn, sn;
	VECT v;

	if ( !arg ) {
		*rp =0;
		return;
	}

	for (col = 0, tn = arg; tn; tn = NEXT(tn), col++);
	if ( col == 1 ) {
		if ( OID(ARG0(arg)) == O_MAT ) {
			*rp=ARG0(arg);
			return;
		} else if ( !(OID(ARG0(arg)) == O_LIST || OID(ARG0(arg)) == O_VECT)) {
			error("matc : invalid argument");
		}
	}
	if ( OID(ARG0(arg)) == O_VECT ) {
		v = ARG0(arg);
		row = v->len;
	} else if ( OID(ARG0(arg)) == O_LIST ) {
		for (row = 0, tn = BDY((LIST)ARG0(arg)); tn ; tn = NEXT(tn), row++);
	} else {
		error("matc : invalid argument");
	}

	MKMAT(m,row,col);
	for (col = 0, tn = arg, mb = BDY(m); tn; tn = NEXT(tn), col++) {
		if ( BDY(tn) == 0 ) {
			error("matc : invalid argument");
		} else if ( OID(BDY(tn)) == O_VECT ) {
			v = tn->body;
			ent = BDY(v);
			for (i = 0; i < v->len; i++ ) mb[i][col] = (Obj)ent[i];
		} else if ( OID(BDY(tn)) == O_LIST ) {
			for (row = 0, sn = BDY((LIST)BDY(tn)); sn; row++, sn = NEXT(sn) )
				mb[row][col] = (pointer)BDY(sn);
		} else {
			error("matc : invalid argument");
		}
	}
	*rp = m;
}

void Pvtol(NODE arg,LIST *rp)
{
	NODE n,n1;
	VECT v;
	pointer *a;
	int len,i;

	if ( OID(ARG0(arg)) == O_LIST ) {
		*rp = ARG0(arg);
		return;
	}
	asir_assert(ARG0(arg),O_VECT,"vtol");
	v = (VECT)ARG0(arg); len = v->len; a = BDY(v);
	for ( i = len - 1, n = 0; i >= 0; i-- ) {
		MKNODE(n1,a[i],n); n = n1;
	}
	MKLIST(*rp,n);
}

void Pltov(NODE arg,VECT *rp)
{
	NODE n;
	VECT v,v0;
	int len,i;

	if ( OID(ARG0(arg)) == O_VECT ) {
		v0 = (VECT)ARG0(arg); len = v0->len;
		MKVECT(v,len);
		for ( i = 0; i < len; i++ ) {
			BDY(v)[i] = BDY(v0)[i];
		}
		*rp = v;
		return;
	}
	asir_assert(ARG0(arg),O_LIST,"ltov");
	n = (NODE)BDY((LIST)ARG0(arg));
	len = length(n);
	MKVECT(v,len);
	for ( i = 0; i < len; i++, n = NEXT(n) )
		BDY(v)[i] = BDY(n);
	*rp = v;
}

void Premainder(NODE arg,Obj *rp)
{
	Obj a;
	VECT v,w;
	MAT m,l;
	pointer *vb,*wb;
	pointer **mb,**lb;
	int id,i,j,n,row,col,t,smd,sgn;
	Q md,q;

	a = (Obj)ARG0(arg); md = (Q)ARG1(arg);
	if ( !a )
		*rp = 0;
	else {
		id = OID(a);
		switch ( id ) {
			case O_N:
			case O_P:
				cmp(md,(P)a,(P *)rp); break;
			case O_VECT:
				smd = QTOS(md);
				v = (VECT)a; n = v->len; vb = v->body;
				MKVECT(w,n); wb = w->body;
				for ( i = 0; i < n; i++ ) {
					if ( q = (Q)vb[i] ) {
						sgn = SGN(q); t = rem(NM(q),smd);
						STOQ(t,q);
						if ( q )
							SGN(q) = sgn;
					}
					wb[i] = (pointer)q;
				}
				*rp = (Obj)w;
				break;
			case O_MAT:
				m = (MAT)a; row = m->row; col = m->col; mb = m->body;
				MKMAT(l,row,col); lb = l->body;
				for ( i = 0; i < row; i++ )
					for ( j = 0, vb = mb[i], wb = lb[i]; j < col; j++ )
						cmp(md,(P)vb[j],(P *)&wb[j]);
				*rp = (Obj)l;
				break;
			default:
				error("remainder : invalid argument");
		}
	}
}

void Psremainder(NODE arg,Obj *rp)
{
	Obj a;
	VECT v,w;
	MAT m,l;
	pointer *vb,*wb;
	pointer **mb,**lb;
	unsigned int t,smd;
	int id,i,j,n,row,col;
	Q md,q;

	a = (Obj)ARG0(arg); md = (Q)ARG1(arg);
	if ( !a )
		*rp = 0;
	else {
		id = OID(a);
		switch ( id ) {
			case O_N:
			case O_P:
				cmp(md,(P)a,(P *)rp); break;
			case O_VECT:
				smd = QTOS(md);
				v = (VECT)a; n = v->len; vb = v->body;
				MKVECT(w,n); wb = w->body;
				for ( i = 0; i < n; i++ ) {
					if ( q = (Q)vb[i] ) {
						t = (unsigned int)rem(NM(q),smd);
						if ( SGN(q) < 0 )
							t = (smd - t) % smd;
						UTOQ(t,q);
					}
					wb[i] = (pointer)q;
				}
				*rp = (Obj)w;
				break;
			case O_MAT:
				m = (MAT)a; row = m->row; col = m->col; mb = m->body;
				MKMAT(l,row,col); lb = l->body;
				for ( i = 0; i < row; i++ )
					for ( j = 0, vb = mb[i], wb = lb[i]; j < col; j++ )
						cmp(md,(P)vb[j],(P *)&wb[j]);
				*rp = (Obj)l;
				break;
			default:
				error("remainder : invalid argument");
		}
	}
}

void Psize(NODE arg,LIST *rp)
{

	int n,m;
	Q q;
	NODE t,s;

	if ( !ARG0(arg) )
		 t = 0;
	else {
		switch (OID(ARG0(arg))) {
			case O_VECT:
				n = ((VECT)ARG0(arg))->len;
				STOQ(n,q); MKNODE(t,q,0);
				break;
			case O_MAT:
				n = ((MAT)ARG0(arg))->row; m = ((MAT)ARG0(arg))->col;
				STOQ(m,q); MKNODE(s,q,0); STOQ(n,q); MKNODE(t,q,s);
				break;
			case O_IMAT:
				n = ((IMAT)ARG0(arg))->row; m = ((IMAT)ARG0(arg))->col;
				STOQ(m,q); MKNODE(s,q,0); STOQ(n,q); MKNODE(t,q,s);
				break;
			default:
				error("size : invalid argument"); break;
		}
	}
	MKLIST(*rp,t);
}

void Pdet(NODE arg,P *rp)
{
	MAT m;
	int n,i,j,mod;
	P d;
	P **mat,**w;

	m = (MAT)ARG0(arg);
	asir_assert(m,O_MAT,"det");
	if ( m->row != m->col )
		error("det : non-square matrix");
	else if ( argc(arg) == 1 )
		detp(CO,(P **)BDY(m),m->row,rp);
	else {
		n = m->row; mod = QTOS((Q)ARG1(arg)); mat = (P **)BDY(m);
		w = (P **)almat_pointer(n,n);
		for ( i = 0; i < n; i++ )
			for ( j = 0; j < n; j++ )
				ptomp(mod,mat[i][j],&w[i][j]);
		detmp(CO,mod,w,n,&d);
		mptop(d,rp);
	}
}

void Pinvmat(NODE arg,LIST *rp)
{
	MAT m,r;
	int n,i,j,mod;
	P dn;
	P **mat,**imat,**w;
	NODE nd;

	m = (MAT)ARG0(arg);
	asir_assert(m,O_MAT,"invmat");
	if ( m->row != m->col )
		error("invmat : non-square matrix");
	else if ( argc(arg) == 1 ) {
		n = m->row;
		invmatp(CO,(P **)BDY(m),n,&imat,&dn);
		NEWMAT(r); r->row = n; r->col = n; r->body = (pointer **)imat;
		nd = mknode(2,r,dn);
		MKLIST(*rp,nd);
	} else {
		n = m->row; mod = QTOS((Q)ARG1(arg)); mat = (P **)BDY(m);
		w = (P **)almat_pointer(n,n);
		for ( i = 0; i < n; i++ )
			for ( j = 0; j < n; j++ )
				ptomp(mod,mat[i][j],&w[i][j]);
#if 0
		detmp(CO,mod,w,n,&d);
		mptop(d,rp);
#else
		error("not implemented yet");
#endif
	}
}

/*
	input : a row x col matrix A
		A[I] <-> A[I][0]*x_0+A[I][1]*x_1+...

	output : [B,D,R,C]
		B : a rank(A) x col-rank(A) matrix
		D : the denominator
		R : a vector of length rank(A)
		C : a vector of length col-rank(A)
		B[I] <-> D*x_{R[I]}+B[I][0]x_{C[0]}+B[I][1]x_{C[1]}+...
*/

void Pgeneric_gauss_elim(NODE arg,LIST *rp)
{
	NODE n0,opt,p;
	MAT m,nm;
	int *ri,*ci;
	VECT rind,cind;
	Q dn,q;
	int i,row,col,t,rank;
	int is_hensel = 0;
	char *key;
	Obj value;

	if ( current_option ) {
		for ( opt = current_option; opt; opt = NEXT(opt) ) {
			p = BDY((LIST)BDY(opt));
			key = BDY((STRING)BDY(p));
			value = (Obj)BDY(NEXT(p));
			if ( !strcmp(key,"hensel") && value ) {
				is_hensel = value ? 1 : 0;
				break;
			}
		}
	}
	asir_assert(ARG0(arg),O_MAT,"generic_gauss_elim");
	m = (MAT)ARG0(arg);
	row = m->row; col = m->col;
	if ( is_hensel )
		rank = generic_gauss_elim_hensel(m,&nm,&dn,&ri,&ci);
	else
		rank = generic_gauss_elim(m,&nm,&dn,&ri,&ci);
	t = col-rank;
	MKVECT(rind,rank);
	MKVECT(cind,t);
	for ( i = 0; i < rank; i++ ) {
		STOQ(ri[i],q);
		BDY(rind)[i] = (pointer)q;
	}
	for ( i = 0; i < t; i++ ) {
		STOQ(ci[i],q);
		BDY(cind)[i] = (pointer)q;
	}
	n0 = mknode(4,nm,dn,rind,cind);
	MKLIST(*rp,n0);
}

void Pindep_rows_mod(NODE arg,VECT *rp)
{
	MAT m,mat;
	VECT rind;
	Q **tmat;
	int **wmat,**row0;
	Q *rib;
	int *rowstat,*p;
	Q q;
	int md,i,j,k,l,row,col,t,rank;

	asir_assert(ARG0(arg),O_MAT,"indep_rows_mod");
	asir_assert(ARG1(arg),O_N,"indep_rows_mod");
	m = (MAT)ARG0(arg); md = QTOS((Q)ARG1(arg));
	row = m->row; col = m->col; tmat = (Q **)m->body;
	wmat = (int **)almat(row,col);

	row0 = (int **)ALLOCA(row*sizeof(int *));
	for ( i = 0; i < row; i++ ) row0[i] = wmat[i];

	rowstat = (int *)MALLOC_ATOMIC(row*sizeof(int));
	for ( i = 0; i < row; i++ )
		for ( j = 0; j < col; j++ )
			if ( q = (Q)tmat[i][j] ) {
				t = rem(NM(q),md);
				if ( t && SGN(q) < 0 )
					t = (md - t) % md;
				wmat[i][j] = t;
			} else
				wmat[i][j] = 0;
	rank = indep_rows_mod(wmat,row,col,md,rowstat);

	MKVECT(rind,rank);
	rib = (Q *)rind->body;
	for ( j = 0; j < rank; j++ ) {
		STOQ(rowstat[j],rib[j]);
	}
    *rp = rind;
}

/*
	input : a row x col matrix A
		A[I] <-> A[I][0]*x_0+A[I][1]*x_1+...

	output : [B,R,C]
		B : a rank(A) x col-rank(A) matrix
		R : a vector of length rank(A)
		C : a vector of length col-rank(A)
		RN : a vector of length rank(A) indicating useful rows

		B[I] <-> x_{R[I]}+B[I][0]x_{C[0]}+B[I][1]x_{C[1]}+...
*/

void Pgeneric_gauss_elim_mod(NODE arg,LIST *rp)
{
	NODE n0;
	MAT m,mat;
	VECT rind,cind,rnum;
	Q **tmat;
	int **wmat,**row0;
	Q *rib,*cib,*rnb;
	int *colstat,*p;
	Q q;
	int md,i,j,k,l,row,col,t,rank;

	asir_assert(ARG0(arg),O_MAT,"generic_gauss_elim_mod");
	asir_assert(ARG1(arg),O_N,"generic_gauss_elim_mod");
	m = (MAT)ARG0(arg); md = QTOS((Q)ARG1(arg));
	row = m->row; col = m->col; tmat = (Q **)m->body;
	wmat = (int **)almat(row,col);

	row0 = (int **)ALLOCA(row*sizeof(int *));
	for ( i = 0; i < row; i++ ) row0[i] = wmat[i];

	colstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
	for ( i = 0; i < row; i++ )
		for ( j = 0; j < col; j++ )
			if ( q = (Q)tmat[i][j] ) {
				t = rem(NM(q),md);
				if ( t && SGN(q) < 0 )
					t = (md - t) % md;
				wmat[i][j] = t;
			} else
				wmat[i][j] = 0;
	rank = generic_gauss_elim_mod(wmat,row,col,md,colstat);

	MKVECT(rnum,rank);
	rnb = (Q *)rnum->body;
	for ( i = 0; i < rank; i++ )
		for ( j = 0, p = wmat[i]; j < row; j++ )
			if ( p == row0[j] )
				STOQ(j,rnb[i]);

	MKMAT(mat,rank,col-rank);
	tmat = (Q **)mat->body;
	for ( i = 0; i < rank; i++ )
		for ( j = k = 0; j < col; j++ )
			if ( !colstat[j] ) {
				UTOQ(wmat[i][j],tmat[i][k]); k++;
			}

	MKVECT(rind,rank);
	MKVECT(cind,col-rank);
	rib = (Q *)rind->body; cib = (Q *)cind->body;
	for ( j = k = l = 0; j < col; j++ )
		if ( colstat[j] ) {
			STOQ(j,rib[k]); k++;
		} else {
			STOQ(j,cib[l]); l++;
		}
	n0 = mknode(4,mat,rind,cind,rnum);
	MKLIST(*rp,n0);
}

void Pleqm(NODE arg,VECT *rp)
{
	MAT m;
	VECT vect;
	pointer **mat;
	Q *v;
	Q q;
	int **wmat;
	int md,i,j,row,col,t,n,status;

	asir_assert(ARG0(arg),O_MAT,"leqm");
	asir_assert(ARG1(arg),O_N,"leqm");
	m = (MAT)ARG0(arg); md = QTOS((Q)ARG1(arg));
	row = m->row; col = m->col; mat = m->body;
	wmat = (int **)almat(row,col);
	for ( i = 0; i < row; i++ )
		for ( j = 0; j < col; j++ )
			if ( q = (Q)mat[i][j] ) {
				t = rem(NM(q),md);
				if ( SGN(q) < 0 )
					t = (md - t) % md;
				wmat[i][j] = t;
			} else
				wmat[i][j] = 0;
	status = gauss_elim_mod(wmat,row,col,md);
	if ( status < 0 )
		*rp = 0;
	else if ( status > 0 )
		*rp = (VECT)ONE;
	else {
		n = col - 1;
		MKVECT(vect,n);
		for ( i = 0, v = (Q *)vect->body; i < n; i++ ) {
			t = (md-wmat[i][n])%md; STOQ(t,v[i]);
		}
		*rp = vect;
	}
}

int gauss_elim_mod(int **mat,int row,int col,int md)
{
	int i,j,k,inv,a,n;
	int *t,*pivot;

	n = col - 1;
	for ( j = 0; j < n; j++ ) {
		for ( i = j; i < row && !mat[i][j]; i++ );
		if ( i == row )
			return 1;
		if ( i != j ) {
			t = mat[i]; mat[i] = mat[j]; mat[j] = t;
		}
		pivot = mat[j];
		inv = invm(pivot[j],md);
		for ( k = j; k <= n; k++ ) {
/*			pivot[k] = dmar(pivot[k],inv,0,md); */
			DMAR(pivot[k],inv,0,md,pivot[k])
		}
		for ( i = 0; i < row; i++ ) {
			t = mat[i];
			if ( i != j && (a = t[j]) )
				for ( k = j, a = md - a; k <= n; k++ ) {
					unsigned int tk;
/*					t[k] = dmar(pivot[k],a,t[k],md); */
					DMAR(pivot[k],a,t[k],md,tk)
					t[k] = tk;
				}
		}
	}
	for ( i = n; i < row && !mat[i][n]; i++ );
	if ( i == row )
		return 0;
	else
		return -1;
}

struct oEGT eg_mod,eg_elim,eg_elim1,eg_elim2,eg_chrem,eg_gschk,eg_intrat,eg_symb;
struct oEGT eg_conv;

int generic_gauss_elim(MAT mat,MAT *nm,Q *dn,int **rindp,int **cindp)
{
	int **wmat;
	Q **bmat;
	N **tmat;
	Q *bmi;
	N *tmi;
	Q q;
	int *wmi;
	int *colstat,*wcolstat,*rind,*cind;
	int row,col,ind,md,i,j,k,l,t,t1,rank,rank0,inv;
	N m1,m2,m3,s,u;
	MAT r,crmat;
	struct oEGT tmp0,tmp1;
	struct oEGT eg_mod_split,eg_elim_split,eg_chrem_split;
	struct oEGT eg_intrat_split,eg_gschk_split;
	int ret;

	init_eg(&eg_mod_split); init_eg(&eg_chrem_split);
	init_eg(&eg_elim_split); init_eg(&eg_intrat_split);
	init_eg(&eg_gschk_split);
	bmat = (Q **)mat->body;
	row = mat->row; col = mat->col;
	wmat = (int **)almat(row,col);
	colstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
	wcolstat = (int *)MALLOC_ATOMIC(col*sizeof(int));
	for ( ind = 0; ; ind++ ) {
		if ( DP_Print ) {
			fprintf(asir_out,"."); fflush(asir_out);
		}
		md = get_lprime(ind);
		get_eg(&tmp0);
		for ( i = 0; i < row; i++ )
			for ( j = 0, bmi = bmat[i], wmi = wmat[i]; j < col; j++ )
				if ( q = (Q)bmi[j] ) {
					t = rem(NM(q),md);
					if ( t && SGN(q) < 0 )
						t = (md - t) % md;
					wmi[j] = t;
				} else
					wmi[j] = 0;
		get_eg(&tmp1);
		add_eg(&eg_mod,&tmp0,&tmp1);
		add_eg(&eg_mod_split,&tmp0,&tmp1);
		get_eg(&tmp0);
		rank = generic_gauss_elim_mod(wmat,row,col,md,wcolstat);
		get_eg(&tmp1);
		add_eg(&eg_elim,&tmp0,&tmp1);
		add_eg(&eg_elim_split,&tmp0,&tmp1);
		if ( !ind ) {
RESET:
			UTON(md,m1);
			rank0 = rank;
			bcopy(wcolstat,colstat,col*sizeof(int));
			MKMAT(crmat,rank,col-rank);
			MKMAT(r,rank,col-rank); *nm = r;
			tmat = (N **)crmat->body;
			for ( i = 0; i < rank; i++ )
				for ( j = k = 0, tmi = tmat[i], wmi = wmat[i]; j < col; j++ )
					if ( !colstat[j] ) {
						UTON(wmi[j],tmi[k]); k++;
					}
		} else {
			if ( rank < rank0 ) {
				if ( DP_Print ) {
					fprintf(asir_out,"lower rank matrix; continuing...\n");
					fflush(asir_out);
				}
				continue;
			} else if ( rank > rank0 ) {
				if ( DP_Print ) {
					fprintf(asir_out,"higher rank matrix; resetting...\n");
					fflush(asir_out);
				}
				goto RESET;
			} else {
				for ( j = 0; (j<col) && (colstat[j]==wcolstat[j]); j++ );
				if ( j < col ) {
					if ( DP_Print ) {
						fprintf(asir_out,"inconsitent colstat; resetting...\n");
						fflush(asir_out);
					}
					goto RESET;
				}
			}

			get_eg(&tmp0);
			inv = invm(rem(m1,md),md);
			UTON(md,m2); muln(m1,m2,&m3);
			for ( i = 0; i < rank; i++ )			
				for ( j = k = 0, tmi = tmat[i], wmi = wmat[i]; j < col; j++ )
					if ( !colstat[j] ) {
						if ( tmi[k] ) {
						/* f3 = f1+m1*(m1 mod m2)^(-1)*(f2 - f1 mod m2) */
							t = rem(tmi[k],md);
							if ( wmi[j] >= t )
								t = wmi[j]-t;
							else
								t = md-(t-wmi[j]);
							DMAR(t,inv,0,md,t1)
							UTON(t1,u);
							muln(m1,u,&s);
							addn(tmi[k],s,&u); tmi[k] = u;
						} else if ( wmi[j] ) {
						/* f3 = m1*(m1 mod m2)^(-1)*f2 */
							DMAR(wmi[j],inv,0,md,t)
							UTON(t,u);
							muln(m1,u,&s); tmi[k] = s;
						}
						k++;
					}
			m1 = m3;
			get_eg(&tmp1);
			add_eg(&eg_chrem,&tmp0,&tmp1);
			add_eg(&eg_chrem_split,&tmp0,&tmp1);

			get_eg(&tmp0);
			if ( ind % F4_INTRAT_PERIOD )
				ret = 0;
			else
				ret = intmtoratm(crmat,m1,*nm,dn);
			get_eg(&tmp1);
			add_eg(&eg_intrat,&tmp0,&tmp1);
			add_eg(&eg_intrat_split,&tmp0,&tmp1);
			if ( ret ) {
				*rindp = rind = (int *)MALLOC_ATOMIC(rank*sizeof(int));
				*cindp = cind = (int *)MALLOC_ATOMIC((col-rank)*sizeof(int));
				for ( j = k = l = 0; j < col; j++ )
					if ( colstat[j] )
						rind[k++] = j;	
					else
						cind[l++] = j;
				get_eg(&tmp0);
				if ( gensolve_check(mat,*nm,*dn,rind,cind) ) {
					get_eg(&tmp1);
					add_eg(&eg_gschk,&tmp0,&tmp1);
					add_eg(&eg_gschk_split,&tmp0,&tmp1);
					if ( DP_Print ) {
						print_eg("Mod",&eg_mod_split);
						print_eg("Elim",&eg_elim_split);
						print_eg("ChRem",&eg_chrem_split);
						print_eg("IntRat",&eg_intrat_split);
						print_eg("Check",&eg_gschk_split);
						fflush(asir_out);
					}
					return rank;
				}
			}
		}
	}
}

void lu_dec_cr(MAT mat,MAT lu,Q *dn,int **perm);

/* XXX broken */
void lu_dec_cr(MAT mat,MAT lu,Q *dn,int **perm)
{
	Q **a0,**b;
	Q *aiq;
	N **a;
	N *ai;
	Q q,q1,dn2,a1,q0,bik;
	MAT m;
	unsigned int md;
	int n,ind,i,j,rank,t,inv,t1,ret,min,k;
	int **w;
	int *wi,*rinfo0,*rinfo;
	N m1,m2,m3,u,s;

	a0 = (Q **)mat->body;
	n = mat->row;
	if ( n != mat->col )
		error("lu_dec_cr : non-square matrix");
	w = (int **)almat(n,n);
	MKMAT(m,n,n);
	a = (N **)m->body;
	UTON(1,m1);
	rinfo0 = 0;
	ind = 0;
	while ( 1 ) {
		md = get_lprime(ind);
		/* mat mod md */
		for ( i = 0; i < n; i++ )
			for ( j = 0, aiq = a0[i], wi = w[i]; j < n; j++ )
				if ( q = aiq[j] ) {
					t = rem(NM(q),md);
					if ( t && SGN(q) < 0 )
						t = (md - t) % md;
					wi[j] = t;
				} else
					wi[j] = 0;

		if ( !lu_mod((unsigned int **)w,n,md,&rinfo) ) continue;
		printf("."); fflush(stdout);
		if ( !rinfo0 )
			*perm = rinfo0 = rinfo;
		else {
			for ( i = 0; i < n; i++ )
				if ( rinfo[i] != rinfo0[i] ) break;
			if ( i < n ) continue;
		}
		if ( UNIN(m1) ) {
			for ( i = 0; i < n; i++ )
				for ( j = 0, ai = a[i], wi = w[i]; j < n; j++ ) {
					UTON(wi[j],u); ai[j] = u;
				}
			UTON(md,m1);
		} else {
			inv = invm(rem(m1,md),md);
			UTON(md,m2); muln(m1,m2,&m3);
			for ( i = 0; i < n; i++ )			
				for ( j = 0, ai = a[i], wi = w[i]; j < n; j++ )
					if ( ai[i] ) {
					/* f3 = f1+m1*(m1 mod m2)^(-1)*(f2 - f1 mod m2) */
						t = rem(ai[j],md);
						if ( wi[j] >= t )
							t = wi[j]-t;
						else
							t = md-(t-wi[j]);
						DMAR(t,inv,0,md,t1)
						UTON(t1,u);
						muln(m1,u,&s);
						addn(ai[j],s,&u); ai[j] = u;
					} else if ( wi[j] ) {
						/* f3 = m1*(m1 mod m2)^(-1)*f2 */
						DMAR(wi[j],inv,0,md,t)
						UTON(t,u);
						muln(m1,u,&s); ai[j] = s;
					}
			m1 = m3;
		}
		if ( (++ind%8) == 0 ) {
			ret = intmtoratm(m,m1,lu,dn);
			if ( ret ) {
				b = (Q **)lu->body;
				mulq(*dn,*dn,&dn2);
				for ( i = 0; i < n; i++ ) {
					for ( j = 0; j < n; j++ ) {
						q = 0;
						min = MIN(i,j);
						for ( k = 0; k <= min; k++ ) {
							bik = k==i ? *dn : b[i][k];
							mulq(bik,b[k][j],&q0);
							addq(q,q0,&q1); q = q1;
						}
						mulq(a0[rinfo0[i]][j],dn2,&q1);
						if ( cmpq(q,q1) ) break;
					}
					if ( j < n ) break;
				}
				if ( i == n )
					return;
			}
		}
	}
}

void nmat(N **m,int n)
{
	int i,j;

	for ( i = 0; i < n; i++ ) {
		for ( j = 0; j < n; j++ ) {
			printn(m[i][j]); printf(" ");
		}
		printf("\n");
	}
}

int generic_gauss_elim_hensel(MAT mat,MAT *nmmat,Q *dn,int **rindp,int **cindp)
{
	MAT bmat,xmat;
	Q **a0,**a,**b,**x,**nm;
	Q *ai,*bi,*xi;
	int row,col;
	int **w;
	int *wi;
	int **wc;
	Q mdq,q,s,u;
	N tn;
	int ind,md,i,j,k,l,li,ri,rank;
	unsigned int t;
	int *cinfo,*rinfo;
	int *rind,*cind;
	int count;
	int ret;
	struct oEGT eg_mul,eg_inv,eg_intrat,eg_check,tmp0,tmp1;
	int period;
	int *wx,*ptr;
	int wxsize,nsize;
	N wn;
	Q wq;

	a0 = (Q **)mat->body;
	row = mat->row; col = mat->col;
	w = (int **)almat(row,col);
	for ( ind = 0; ; ind++ ) {
		md = get_lprime(ind);
		STOQ(md,mdq);
		for ( i = 0; i < row; i++ )
			for ( j = 0, ai = a0[i], wi = w[i]; j < col; j++ )
				if ( q = (Q)ai[j] ) {
					t = rem(NM(q),md);
					if ( t && SGN(q) < 0 )
						t = (md - t) % md;
					wi[j] = t;
				} else
					wi[j] = 0;

		if ( DP_Print > 3 ) {
			fprintf(asir_out,"LU decomposition.."); fflush(asir_out);
		}
		rank = find_lhs_and_lu_mod((unsigned int **)w,row,col,md,&rinfo,&cinfo);
		if ( DP_Print > 3 ) {
			fprintf(asir_out,"done.\n"); fflush(asir_out);
		}
		a = (Q **)almat_pointer(rank,rank); /* lhs mat */
		MKMAT(bmat,rank,col-rank); b = (Q **)bmat->body; /* lhs mat */
		for ( j = li = ri = 0; j < col; j++ )
			if ( cinfo[j] ) {
				/* the column is in lhs */
				for ( i = 0; i < rank; i++ ) {
					w[i][li] = w[i][j];
					a[i][li] = a0[rinfo[i]][j];
				}
				li++;
			} else {
				/* the column is in rhs */
				for ( i = 0; i < rank; i++ )
					b[i][ri] = a0[rinfo[i]][j];
				ri++;
			}

			/* solve Ax+B=0; A: rank x rank, B: rank x ri */
			MKMAT(xmat,rank,ri); x = (Q **)(xmat)->body;
			MKMAT(*nmmat,rank,ri); nm = (Q **)(*nmmat)->body;
			/* use the right part of w as work area */
			/* ri = col - rank */
			wc = (int **)almat(rank,ri);
			for ( i = 0; i < rank; i++ )
				wc[i] = w[i]+rank;
			*rindp = rind = (int *)MALLOC_ATOMIC(rank*sizeof(int));
			*cindp = cind = (int *)MALLOC_ATOMIC((ri)*sizeof(int));

			init_eg(&eg_mul); init_eg(&eg_inv);
			init_eg(&eg_check); init_eg(&eg_intrat);
			period = F4_INTRAT_PERIOD;
			nsize = period;
			wxsize = rank*ri*nsize;
			wx = (int *)MALLOC_ATOMIC(wxsize*sizeof(int));
			for ( i = 0; i < wxsize; i++ ) wx[i] = 0;
			for ( q = ONE, count = 0; ; ) {
				if ( DP_Print > 3 )
					fprintf(stderr,"o");
				/* wc = -b mod md */
				get_eg(&tmp0);
				for ( i = 0; i < rank; i++ )
					for ( j = 0, bi = b[i], wi = wc[i]; j < ri; j++ )
						if ( u = (Q)bi[j] ) {
							t = rem(NM(u),md);
							if ( t && SGN(u) > 0 )
								t = (md - t) % md;
							wi[j] = t;
						} else
							wi[j] = 0;
				/* wc = A^(-1)wc; wc is not normalized */
				solve_by_lu_mod(w,rank,md,wc,ri,0);
				/* wx += q*wc */
				ptr = wx;
				for ( i = 0; i < rank; i++ )
					for ( j = 0, wi = wc[i]; j < ri; j++ ) {
						if ( wi[j] )
							muln_1(BD(NM(q)),PL(NM(q)),wi[j],ptr);
						ptr += nsize;
					}
				count++;
				get_eg(&tmp1);
				add_eg(&eg_inv,&tmp0,&tmp1);
				get_eg(&tmp0);
				for ( i = 0; i < rank; i++ )
					for ( j = 0; j < ri; j++ ) {
						inner_product_mat_int_mod(a,wc,rank,i,j,&u);
						addq(b[i][j],u,&s);
						if ( s ) {
							t = divin(NM(s),md,&tn);
							if ( t )
								error("generic_gauss_elim_hensel:incosistent");	
							NTOQ(tn,SGN(s),b[i][j]);
						} else
							b[i][j] = 0;
					}
				get_eg(&tmp1);
				add_eg(&eg_mul,&tmp0,&tmp1);
				/* q = q*md */
				mulq(q,mdq,&u); q = u;
				if ( count == period ) {
					get_eg(&tmp0);
					ptr = wx;
					for ( i = 0; i < rank; i++ )
						for ( j = 0, xi = x[i]; j < ri;
							j++, ptr += nsize ) {
							for ( k = nsize-1; k >= 0 && !ptr[k]; k-- );
							if ( k >= 0 ) {
								wn = NALLOC(k+1);
								PL(wn) = k+1;
								for ( l = 0; l <= k; l++ ) BD(wn)[l] = (unsigned int)ptr[l];
								NTOQ(wn,1,wq);
								subq(xi[j],wq,&u); xi[j] = u;
							}
						}
					ret = intmtoratm_q(xmat,NM(q),*nmmat,dn);
					get_eg(&tmp1); add_eg(&eg_intrat,&tmp0,&tmp1);
					if ( ret ) {
						rind = (int *)MALLOC_ATOMIC(rank*sizeof(int));
						cind = (int *)MALLOC_ATOMIC((col-rank)*sizeof(int));
						for ( j = k = l = 0; j < col; j++ )
							if ( cinfo[j] )
								rind[k++] = j;	
							else
								cind[l++] = j;
						get_eg(&tmp0);
						ret = gensolve_check(mat,*nmmat,*dn,rind,cind);
						get_eg(&tmp1); add_eg(&eg_check,&tmp0,&tmp1);
						if ( ret ) {
							if ( DP_Print > 3 ) {
								fprintf(stderr,"\n");
								print_eg("INV",&eg_inv);
								print_eg("MUL",&eg_mul);
								print_eg("INTRAT",&eg_intrat);
								print_eg("CHECK",&eg_check);
								fflush(asir_out);
							}
							*rindp = rind;
							*cindp = cind;
							for ( j = k = 0; j < col; j++ )
								if ( !cinfo[j] )
									cind[k++] = j;
							return rank;
						}
					} else {
						period = period*3/2;
						count = 0;
						nsize += period;
						wxsize += rank*ri*nsize;
						wx = (int *)REALLOC(wx,wxsize*sizeof(int));
						for ( i = 0; i < wxsize; i++ ) wx[i] = 0;
					}
				}
			}
	}
}

int generic_gauss_elim_hensel_dalg(MAT mat,DP *mb,MAT *nmmat,Q *dn,int **rindp,int **cindp)
{
	MAT bmat,xmat;
	Q **a0,**a,**b,**x,**nm;
	Q *ai,*bi,*xi;
	int row,col;
	int **w;
	int *wi;
	int **wc;
	Q mdq,q,s,u;
	N tn;
	int ind,md,i,j,k,l,li,ri,rank;
	unsigned int t;
	int *cinfo,*rinfo;
	int *rind,*cind;
	int count;
	int ret;
	struct oEGT eg_mul,eg_inv,eg_intrat,eg_check,tmp0,tmp1;
	int period;
	int *wx,*ptr;
	int wxsize,nsize;
	N wn;
	Q wq;
	NumberField nf;
	DP m;
	int col1;

	a0 = (Q **)mat->body;
	row = mat->row; col = mat->col;
	w = (int **)almat(row,col);
	for ( ind = 0; ; ind++ ) {
		md = get_lprime(ind);
		STOQ(md,mdq);
		for ( i = 0; i < row; i++ )
			for ( j = 0, ai = a0[i], wi = w[i]; j < col; j++ )
				if ( q = (Q)ai[j] ) {
					t = rem(NM(q),md);
					if ( t && SGN(q) < 0 )
						t = (md - t) % md;
					wi[j] = t;
				} else
					wi[j] = 0;

		if ( DP_Print ) {
			fprintf(asir_out,"LU decomposition.."); fflush(asir_out);
		}
		rank = find_lhs_and_lu_mod((unsigned int **)w,row,col,md,&rinfo,&cinfo);
		if ( DP_Print ) {
			fprintf(asir_out,"done.\n"); fflush(asir_out);
		}
		for ( i = 0; i < col-1; i++ ) {
			if ( !cinfo[i] ) {
				m = mb[i];
				for ( j = i+1; j < col-1; j++ )
					if ( dp_redble(mb[j],m) )
						cinfo[j] = -1;
			}
		}
		a = (Q **)almat_pointer(rank,rank); /* lhs mat */
		MKMAT(bmat,rank,col-rank); b = (Q **)bmat->body; /* lhs mat */
		for ( j = li = ri = 0; j < col; j++ )
			if ( cinfo[j] > 0 ) {
				/* the column is in lhs */
				for ( i = 0; i < rank; i++ ) {
					w[i][li] = w[i][j];
					a[i][li] = a0[rinfo[i]][j];
				}
				li++;
			} else if ( !cinfo[j] ) {
				/* the column is in rhs */
				for ( i = 0; i < rank; i++ )
					b[i][ri] = a0[rinfo[i]][j];
				ri++;
			}

			/* solve Ax+B=0; A: rank x rank, B: rank x ri */
			MKMAT(xmat,rank,ri); x = (Q **)(xmat)->body;
			MKMAT(*nmmat,rank,ri); nm = (Q **)(*nmmat)->body;
			/* use the right part of w as work area */
			wc = (int **)almat(rank,ri);
			for ( i = 0; i < rank; i++ )
				wc[i] = w[i]+rank;
			*rindp = rind = (int *)MALLOC_ATOMIC(rank*sizeof(int));
			*cindp = cind = (int *)MALLOC_ATOMIC((col-rank)*sizeof(int));
			init_eg(&eg_mul); init_eg(&eg_inv);
			init_eg(&eg_check); init_eg(&eg_intrat);
			period = F4_INTRAT_PERIOD;
			nsize = period;
			wxsize = rank*ri*nsize;
			wx = (int *)MALLOC_ATOMIC(wxsize*sizeof(int));
			for ( i = 0; i < wxsize; i++ ) wx[i] = 0;
			for ( q = ONE, count = 0; ; ) {
				if ( DP_Print )
					fprintf(stderr,"o");
				/* wc = -b mod md */
				get_eg(&tmp0);
				for ( i = 0; i < rank; i++ )
					for ( j = 0, bi = b[i], wi = wc[i]; j < ri; j++ )
						if ( u = (Q)bi[j] ) {
							t = rem(NM(u),md);
							if ( t && SGN(u) > 0 )
								t = (md - t) % md;
							wi[j] = t;
						} else
							wi[j] = 0;
				/* wc = A^(-1)wc; wc is not normalized */
				solve_by_lu_mod(w,rank,md,wc,ri,0);
				/* wx += q*wc */
				ptr = wx;
				for ( i = 0; i < rank; i++ )
					for ( j = 0, wi = wc[i]; j < ri; j++ ) {
						if ( wi[j] )
							muln_1(BD(NM(q)),PL(NM(q)),wi[j],ptr);
						ptr += nsize;
					}
				count++;
				get_eg(&tmp1);
				add_eg(&eg_inv,&tmp0,&tmp1);
				get_eg(&tmp0);
				for ( i = 0; i < rank; i++ )
					for ( j = 0; j < ri; j++ ) {
						inner_product_mat_int_mod(a,wc,rank,i,j,&u);
						addq(b[i][j],u,&s);
						if ( s ) {
							t = divin(NM(s),md,&tn);
							if ( t )
								error("generic_gauss_elim_hensel:incosistent");	
							NTOQ(tn,SGN(s),b[i][j]);
						} else
							b[i][j] = 0;
					}
				get_eg(&tmp1);
				add_eg(&eg_mul,&tmp0,&tmp1);
				/* q = q*md */
				mulq(q,mdq,&u); q = u;
				if ( count == period ) {
					get_eg(&tmp0);
					ptr = wx;
					for ( i = 0; i < rank; i++ )
						for ( j = 0, xi = x[i]; j < ri;
							j++, ptr += nsize ) {
							for ( k = nsize-1; k >= 0 && !ptr[k]; k-- );
							if ( k >= 0 ) {
								wn = NALLOC(k+1);
								PL(wn) = k+1;
								for ( l = 0; l <= k; l++ ) BD(wn)[l] = (unsigned int)ptr[l];
								NTOQ(wn,1,wq);
								subq(xi[j],wq,&u); xi[j] = u;
							}
						}
					ret = intmtoratm_q(xmat,NM(q),*nmmat,dn);
					get_eg(&tmp1); add_eg(&eg_intrat,&tmp0,&tmp1);
					if ( ret ) {
						for ( j = k = l = 0; j < col; j++ )
							if ( cinfo[j] > 0 )
								rind[k++] = j;	
							else if ( !cinfo[j] )
								cind[l++] = j;
						get_eg(&tmp0);
						ret = gensolve_check(mat,*nmmat,*dn,rind,cind);
						get_eg(&tmp1); add_eg(&eg_check,&tmp0,&tmp1);
						if ( ret ) {
							if ( DP_Print > 3 ) {
								fprintf(stderr,"\n");
								print_eg("INV",&eg_inv);
								print_eg("MUL",&eg_mul);
								print_eg("INTRAT",&eg_intrat);
								print_eg("CHECK",&eg_check);
								fflush(asir_out);
							}
							return rank;
						}
					} else {
						period = period*3/2;
						count = 0;
						nsize += period;
						wxsize += rank*ri*nsize;
						wx = (int *)REALLOC(wx,wxsize*sizeof(int));
						for ( i = 0; i < wxsize; i++ ) wx[i] = 0;
					}
				}
			}
	}
}

int f4_nocheck;

int gensolve_check(MAT mat,MAT nm,Q dn,int *rind,int *cind)
{
	int row,col,rank,clen,i,j,k,l;
	Q s,t;
	Q *w;
	Q *mati,*nmk;

	if ( f4_nocheck )
		return 1;
	row = mat->row; col = mat->col;
	rank = nm->row; clen = nm->col;	
	w = (Q *)MALLOC(clen*sizeof(Q));
	for ( i = 0; i < row; i++ ) {
		mati = (Q *)mat->body[i];
#if 1
		bzero(w,clen*sizeof(Q));
		for ( k = 0; k < rank; k++ )
			for ( l = 0, nmk = (Q *)nm->body[k]; l < clen; l++ ) {
				mulq(mati[rind[k]],nmk[l],&t);
				addq(w[l],t,&s); w[l] = s;
			}
		for ( j = 0; j < clen; j++ ) {
			mulq(dn,mati[cind[j]],&t);
			if ( cmpq(w[j],t) )
				break;
		}
#else
		for ( j = 0; j < clen; j++ ) {
			for ( k = 0, s = 0; k < rank; k++ ) {
				mulq(mati[rind[k]],nm->body[k][j],&t);
				addq(s,t,&u); s = u;
			}
			mulq(dn,mati[cind[j]],&t);
			if ( cmpq(s,t) )
				break;
		}
#endif
		if ( j != clen )
			break;
	}
	if ( i != row )
		return 0;
	else
		return 1;
}

/* assuming 0 < c < m */

int inttorat(N c,N m,N b,int *sgnp,N *nmp,N *dnp)
{
	Q qq,t,u1,v1,r1;
	N q,u2,v2,r2;

	u1 = 0; v1 = ONE; u2 = m; v2 = c;
	while ( cmpn(v2,b) >= 0 ) {
		divn(u2,v2,&q,&r2); u2 = v2; v2 = r2;
		NTOQ(q,1,qq); mulq(qq,v1,&t); subq(u1,t,&r1); u1 = v1; v1 = r1; 
	}
	if ( cmpn(NM(v1),b) >= 0 )
		return 0;
	else {
		*nmp = v2;
		*dnp = NM(v1);
		*sgnp = SGN(v1);
		return 1;
	}
}

/* mat->body = N ** */

int intmtoratm(MAT mat,N md,MAT nm,Q *dn)
{
	N t,s,b;
	Q dn0,dn1,nm1,q;
	int i,j,k,l,row,col;
	Q **rmat;
	N **tmat;
	N *tmi;
	Q *nmk;
	N u,unm,udn;
	int sgn,ret;

	if ( UNIN(md) )
		return 0;
	row = mat->row; col = mat->col;
	bshiftn(md,1,&t);
	isqrt(t,&s);
	bshiftn(s,64,&b);
	if ( !b )
		b = ONEN;
	dn0 = ONE;
	tmat = (N **)mat->body;
	rmat = (Q **)nm->body;
	for ( i = 0; i < row; i++ )
		for ( j = 0, tmi = tmat[i]; j < col; j++ )
			if ( tmi[j] ) {
				muln(tmi[j],NM(dn0),&s);
				remn(s,md,&u);
				ret = inttorat(u,md,b,&sgn,&unm,&udn);
				if ( !ret )
					return 0;
				else {
					NTOQ(unm,sgn,nm1);
					NTOQ(udn,1,dn1);
					if ( !UNIQ(dn1) ) {
						for ( k = 0; k < i; k++ )
							for ( l = 0, nmk = rmat[k]; l < col; l++ ) {
								mulq(nmk[l],dn1,&q); nmk[l] = q;
							}
						for ( l = 0, nmk = rmat[i]; l < j; l++ ) {
							mulq(nmk[l],dn1,&q); nmk[l] = q;
						}
					}
					rmat[i][j] = nm1;
					mulq(dn0,dn1,&q); dn0 = q;
				}
			}
	*dn = dn0;
	return 1;
}

/* mat->body = Q ** */

int intmtoratm_q(MAT mat,N md,MAT nm,Q *dn)
{
	N t,s,b;
	Q dn0,dn1,nm1,q;
	int i,j,k,l,row,col;
	Q **rmat;
	Q **tmat;
	Q *tmi;
	Q *nmk;
	N u,unm,udn;
	int sgn,ret;

	if ( UNIN(md) )
		return 0;
	row = mat->row; col = mat->col;
	bshiftn(md,1,&t);
	isqrt(t,&s);
	bshiftn(s,64,&b);
	if ( !b )
		b = ONEN;
	dn0 = ONE;
	tmat = (Q **)mat->body;
	rmat = (Q **)nm->body;
	for ( i = 0; i < row; i++ )
		for ( j = 0, tmi = tmat[i]; j < col; j++ )
			if ( tmi[j] ) {
				muln(NM(tmi[j]),NM(dn0),&s);
				remn(s,md,&u);
				ret = inttorat(u,md,b,&sgn,&unm,&udn);
				if ( !ret )
					return 0;
				else {
					if ( SGN(tmi[j])<0 )
						sgn = -sgn;
					NTOQ(unm,sgn,nm1);
					NTOQ(udn,1,dn1);
					if ( !UNIQ(dn1) ) {
						for ( k = 0; k < i; k++ )
							for ( l = 0, nmk = rmat[k]; l < col; l++ ) {
								mulq(nmk[l],dn1,&q); nmk[l] = q;
							}
						for ( l = 0, nmk = rmat[i]; l < j; l++ ) {
							mulq(nmk[l],dn1,&q); nmk[l] = q;
						}
					}
					rmat[i][j] = nm1;
					mulq(dn0,dn1,&q); dn0 = q;
				}
			}
	*dn = dn0;
	return 1;
}

#define ONE_STEP1  if ( zzz = *s ) { DMAR(zzz,hc,*tj,md,*tj) } tj++; s++;

void reduce_reducers_mod(int **mat,int row,int col,int md)
{
	int i,j,k,l,hc,zzz;
	int *t,*s,*tj,*ind;

	/* reduce the reducers */
	ind = (int *)ALLOCA(row*sizeof(int));
	for ( i = 0; i < row; i++ ) {
		t = mat[i];
		for ( j = 0; j < col && !t[j]; j++ );
		/* register the position of the head term */
		ind[i] = j;
		for ( l = i-1; l >= 0; l-- ) {
			/* reduce mat[i] by mat[l] */
			if ( hc = t[ind[l]] ) {
				/* mat[i] = mat[i]-hc*mat[l] */
				j = ind[l];
				s = mat[l]+j;
				tj = t+j;
				hc = md-hc;
				k = col-j;
				for ( ; k >= 64; k -= 64 ) {
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
					ONE_STEP1 ONE_STEP1 ONE_STEP1 ONE_STEP1
				}
				for ( ; k > 0; k-- ) {
					if ( zzz = *s ) { DMAR(zzz,hc,*tj,md,*tj) } tj++; s++;
				}
			}
		}
	}
}

/*
	mat[i] : reducers (i=0,...,nred-1)
	         spolys (i=nred,...,row-1)
	mat[0] < mat[1] < ... < mat[nred-1] w.r.t the term order
	1. reduce the reducers
	2. reduce spolys by the reduced reducers
*/

void pre_reduce_mod(int **mat,int row,int col,int nred,int md)
{
	int i,j,k,l,hc,inv;
	int *t,*s,*tk,*ind;

#if 1
	/* reduce the reducers */
	ind = (int *)ALLOCA(row*sizeof(int));
	for ( i = 0; i < nred; i++ ) {
		/* make mat[i] monic and mat[i] by mat[0],...,mat[i-1] */
		t = mat[i];
		for ( j = 0; j < col && !t[j]; j++ );
		/* register the position of the head term */
		ind[i] = j;
		inv = invm(t[j],md);
		for ( k = j; k < col; k++ )
			if ( t[k] )
				DMAR(t[k],inv,0,md,t[k])
		for ( l = i-1; l >= 0; l-- ) {
			/* reduce mat[i] by mat[l] */
			if ( hc = t[ind[l]] ) {
				/* mat[i] = mat[i]-hc*mat[l] */
				for ( k = ind[l], hc = md-hc, s = mat[l]+k, tk = t+k;
					k < col; k++, tk++, s++ )
					if ( *s )
						DMAR(*s,hc,*tk,md,*tk)
			}
		}
	}
	/* reduce the spolys */
	for ( i = nred; i < row; i++ ) {
		t = mat[i];
		for ( l = nred-1; l >= 0; l-- ) {
			/* reduce mat[i] by mat[l] */
			if ( hc = t[ind[l]] ) {
				/* mat[i] = mat[i]-hc*mat[l] */
				for ( k = ind[l], hc = md-hc, s = mat[l]+k, tk = t+k;
					k < col; k++, tk++, s++ )
					if ( *s )
						DMAR(*s,hc,*tk,md,*tk)
			}
		}
	}
#endif
}
/*
	mat[i] : reducers (i=0,...,nred-1)
	mat[0] < mat[1] < ... < mat[nred-1] w.r.t the term order
*/

void reduce_sp_by_red_mod(int *sp,int **redmat,int *ind,int nred,int col,int md)
{
	int i,j,k,hc,zzz;
	int *s,*tj;

	/* reduce the spolys by redmat */
	for ( i = nred-1; i >= 0; i-- ) {
		/* reduce sp by redmat[i] */
		if ( hc = sp[ind[i]] ) {
			/* sp = sp-hc*redmat[i] */
			j = ind[i];
			hc = md-hc;
			s = redmat[i]+j;
			tj = sp+j;
			for ( k = col-j; k > 0; k-- ) {
				if ( zzz = *s ) { DMAR(zzz,hc,*tj,md,*tj) } tj++; s++;
			}
		}
	}
}

/*
	mat[i] : compressed reducers (i=0,...,nred-1)
	mat[0] < mat[1] < ... < mat[nred-1] w.r.t the term order
*/

void red_by_compress(int m,unsigned int *p,unsigned int *r,
	unsigned int *ri,unsigned int hc,int len)
{
	unsigned int up,lo;
	unsigned int dmy;
	unsigned int *pj;

	p[*ri] = 0; r++; ri++;
	for ( len--; len; len--, r++, ri++ ) {
		pj = p+ *ri;
		DMA(*r,hc,*pj,up,lo);
		if ( up ) {
			DSAB(m,up,lo,dmy,*pj);
		} else
			*pj = lo;
	}
}

/* p -= hc*r */

void red_by_vect(int m,unsigned int *p,unsigned int *r,unsigned int hc,int len)
{
	unsigned int up,lo,dmy;

	*p++ = 0; r++; len--;
	for ( ; len; len--, r++, p++ )
		if ( *r ) {
			DMA(*r,hc,*p,up,lo);
			if ( up ) {
				DSAB(m,up,lo,dmy,*p);
			} else
				*p = lo;
		}
}

void red_by_vect_sf(int m,unsigned int *p,unsigned int *r,unsigned int hc,int len)
{
	*p++ = 0; r++; len--;
	for ( ; len; len--, r++, p++ )
		if ( *r )
			*p = _addsf(_mulsf(*r,hc),*p);
}

extern unsigned int **psca;

void reduce_sp_by_red_mod_compress (int *sp,CDP *redmat,int *ind,
	int nred,int col,int md)
{
	int i,len;
	CDP ri;
	unsigned int hc;
	unsigned int *usp;

	usp = (unsigned int *)sp;
	/* reduce the spolys by redmat */
	for ( i = nred-1; i >= 0; i-- ) {
		/* reduce sp by redmat[i] */
		usp[ind[i]] %= md;
		if ( hc = usp[ind[i]] ) {
			/* sp = sp-hc*redmat[i] */
			hc = md-hc;
			ri = redmat[i];
			len = ri->len;
			red_by_compress(md,usp,psca[ri->psindex],ri->body,hc,len);
		}
	}
	for ( i = 0; i < col; i++ )
		if ( usp[i] >= (unsigned int)md )
			usp[i] %= md;
}

#define ONE_STEP2  if ( zzz = *pk ) { DMAR(zzz,a,*tk,md,*tk) } pk++; tk++;

int generic_gauss_elim_mod(int **mat0,int row,int col,int md,int *colstat)
{
	int i,j,k,l,inv,a,rank;
	unsigned int *t,*pivot,*pk;
	unsigned int **mat;

	mat = (unsigned int **)mat0;
	for ( rank = 0, j = 0; j < col; j++ ) {
		for ( i = rank; i < row; i++ )
			mat[i][j] %= md;
		for ( i = rank; i < row; i++ )
			if ( mat[i][j] )
				break;
		if ( i == row ) {
			colstat[j] = 0;
			continue;
		} else
			colstat[j] = 1;
		if ( i != rank ) {
			t = mat[i]; mat[i] = mat[rank]; mat[rank] = t;
		}
		pivot = mat[rank];
		inv = invm(pivot[j],md);
		for ( k = j, pk = pivot+k; k < col; k++, pk++ )
			if ( *pk ) {
				if ( *pk >= (unsigned int)md )
					*pk %= md;
				DMAR(*pk,inv,0,md,*pk)
			}
		for ( i = rank+1; i < row; i++ ) {
			t = mat[i];
			if ( a = t[j] )
				red_by_vect(md,t+j,pivot+j,md-a,col-j);
		}
		rank++;
	}
	for ( j = col-1, l = rank-1; j >= 0; j-- )
		if ( colstat[j] ) {
			pivot = mat[l];
			for ( i = 0; i < l; i++ ) {
				t = mat[i];
				t[j] %= md;
				if ( a = t[j] )
					red_by_vect(md,t+j,pivot+j,md-a,col-j);
			}
			l--;
		}
	for ( j = 0, l = 0; l < rank; j++ )
		if ( colstat[j] ) {
			t = mat[l];
			for ( k = j; k < col; k++ )
				if ( t[k] >= (unsigned int)md )
					t[k] %= md;
			l++;
		}
	return rank;
}

int generic_gauss_elim_mod2(int **mat0,int row,int col,int md,int *colstat,int *rowstat)
{
	int i,j,k,l,inv,a,rank;
	unsigned int *t,*pivot,*pk;
	unsigned int **mat;

	for ( i = 0; i < row; i++ ) rowstat[i] = i;
	mat = (unsigned int **)mat0;
	for ( rank = 0, j = 0; j < col; j++ ) {
		for ( i = rank; i < row; i++ )
			mat[i][j] %= md;
		for ( i = rank; i < row; i++ )
			if ( mat[i][j] )
				break;
		if ( i == row ) {
			colstat[j] = 0;
			continue;
		} else
			colstat[j] = 1;
		if ( i != rank ) {
			t = mat[i]; mat[i] = mat[rank]; mat[rank] = t;
			k = rowstat[i]; rowstat[i] = rowstat[rank]; rowstat[rank] = k;
		}
		pivot = mat[rank];
		inv = invm(pivot[j],md);
		for ( k = j, pk = pivot+k; k < col; k++, pk++ )
			if ( *pk ) {
				if ( *pk >= (unsigned int)md )
					*pk %= md;
				DMAR(*pk,inv,0,md,*pk)
			}
		for ( i = rank+1; i < row; i++ ) {
			t = mat[i];
			if ( a = t[j] )
				red_by_vect(md,t+j,pivot+j,md-a,col-j);
		}
		rank++;
	}
	for ( j = col-1, l = rank-1; j >= 0; j-- )
		if ( colstat[j] ) {
			pivot = mat[l];
			for ( i = 0; i < l; i++ ) {
				t = mat[i];
				t[j] %= md;
				if ( a = t[j] )
					red_by_vect(md,t+j,pivot+j,md-a,col-j);
			}
			l--;
		}
	for ( j = 0, l = 0; l < rank; j++ )
		if ( colstat[j] ) {
			t = mat[l];
			for ( k = j; k < col; k++ )
				if ( t[k] >= (unsigned int)md )
					t[k] %= md;
			l++;
		}
	return rank;
}

int indep_rows_mod(int **mat0,int row,int col,int md,int *rowstat)
{
	int i,j,k,l,inv,a,rank;
	unsigned int *t,*pivot,*pk;
	unsigned int **mat;

	for ( i = 0; i < row; i++ ) rowstat[i] = i;
	mat = (unsigned int **)mat0;
	for ( rank = 0, j = 0; j < col; j++ ) {
		for ( i = rank; i < row; i++ )
			mat[i][j] %= md;
		for ( i = rank; i < row; i++ )
			if ( mat[i][j] )
				break;
		if ( i == row ) continue;
		if ( i != rank ) {
			t = mat[i]; mat[i] = mat[rank]; mat[rank] = t;
			k = rowstat[i]; rowstat[i] = rowstat[rank]; rowstat[rank] = k;
		}
		pivot = mat[rank];
		inv = invm(pivot[j],md);
		for ( k = j, pk = pivot+k; k < col; k++, pk++ )
			if ( *pk ) {
				if ( *pk >= (unsigned int)md )
					*pk %= md;
				DMAR(*pk,inv,0,md,*pk)
			}
		for ( i = rank+1; i < row; i++ ) {
			t = mat[i];
			if ( a = t[j] )
				red_by_vect(md,t+j,pivot+j,md-a,col-j);
		}
		rank++;
	}
	return rank;
}

int generic_gauss_elim_sf(int **mat0,int row,int col,int md,int *colstat)
{
	int i,j,k,l,inv,a,rank;
	unsigned int *t,*pivot,*pk;
	unsigned int **mat;

	mat = (unsigned int **)mat0;
	for ( rank = 0, j = 0; j < col; j++ ) {
		for ( i = rank; i < row; i++ )
			if ( mat[i][j] )
				break;
		if ( i == row ) {
			colstat[j] = 0;
			continue;
		} else
			colstat[j] = 1;
		if ( i != rank ) {
			t = mat[i]; mat[i] = mat[rank]; mat[rank] = t;
		}
		pivot = mat[rank];
		inv = _invsf(pivot[j]);
		for ( k = j, pk = pivot+k; k < col; k++, pk++ )
			if ( *pk )
				*pk = _mulsf(*pk,inv);
		for ( i = rank+1; i < row; i++ ) {
			t = mat[i];
			if ( a = t[j] )
				red_by_vect_sf(md,t+j,pivot+j,_chsgnsf(a),col-j);
		}
		rank++;
	}
	for ( j = col-1, l = rank-1; j >= 0; j-- )
		if ( colstat[j] ) {
			pivot = mat[l];
			for ( i = 0; i < l; i++ ) {
				t = mat[i];
				if ( a = t[j] )
					red_by_vect_sf(md,t+j,pivot+j,_chsgnsf(a),col-j);
			}
			l--;
		}
	return rank;
}

/* LU decomposition; a[i][i] = 1/U[i][i] */

int lu_gfmmat(GFMMAT mat,unsigned int md,int *perm)
{
	int row,col;
	int i,j,k;
	unsigned int *t,*pivot;
	unsigned int **a;
	unsigned int inv,m;

	row = mat->row; col = mat->col;
	a = mat->body;
	bzero(perm,row*sizeof(int));

	for ( i = 0; i < row; i++ )
		perm[i] = i;
	for ( k = 0; k < col; k++ ) {
		for ( i = k; i < row && !a[i][k]; i++ );
		if ( i == row )
			return 0;
		if ( i != k ) {
			j = perm[i]; perm[i] = perm[k]; perm[k] = j;
			t = a[i]; a[i] = a[k]; a[k] = t;
		}
		pivot = a[k];
		pivot[k] = inv = invm(pivot[k],md);
		for ( i = k+1; i < row; i++ ) {
			t = a[i];
			if ( m = t[k] ) {
				DMAR(inv,m,0,md,t[k])
				for ( j = k+1, m = md - t[k]; j < col; j++ )
					if ( pivot[j] ) {
						unsigned int tj;

						DMAR(m,pivot[j],t[j],md,tj)
						t[j] = tj;
					}
			}
		}
	}
	return 1;
}

/*
 Input
	a: a row x col matrix
	md : a modulus

 Output:
	return : d = the rank of mat
	a[0..(d-1)][0..(d-1)] : LU decomposition (a[i][i] = 1/U[i][i])
	rinfo: array of length row
	cinfo: array of length col
    i-th row in new a <-> rinfo[i]-th row in old a
	cinfo[j]=1 <=> j-th column is contained in the LU decomp.
*/

int find_lhs_and_lu_mod(unsigned int **a,int row,int col,
	unsigned int md,int **rinfo,int **cinfo)
{
	int i,j,k,d;
	int *rp,*cp;
	unsigned int *t,*pivot;
	unsigned int inv,m;

	*rinfo = rp = (int *)MALLOC_ATOMIC(row*sizeof(int));
	*cinfo = cp = (int *)MALLOC_ATOMIC(col*sizeof(int));
	for ( i = 0; i < row; i++ )
		rp[i] = i;
	for ( k = 0, d = 0; k < col; k++ ) {
		for ( i = d; i < row && !a[i][k]; i++ );
		if ( i == row ) {
			cp[k] = 0;
			continue;
		} else
			cp[k] = 1;
		if ( i != d ) {
			j = rp[i]; rp[i] = rp[d]; rp[d] = j;
			t = a[i]; a[i] = a[d]; a[d] = t;
		}
		pivot = a[d];
		pivot[k] = inv = invm(pivot[k],md);
		for ( i = d+1; i < row; i++ ) {
			t = a[i];
			if ( m = t[k] ) {
				DMAR(inv,m,0,md,t[k])
				for ( j = k+1, m = md - t[k]; j < col; j++ )
					if ( pivot[j] ) {
						unsigned int tj;
						DMAR(m,pivot[j],t[j],md,tj)
						t[j] = tj;
					}
			}
		}
		d++;
	}
	return d;
}

int lu_mod(unsigned int **a,int n,unsigned int md,int **rinfo)
{
	int i,j,k;
	int *rp;
	unsigned int *t,*pivot;
	unsigned int inv,m;

	*rinfo = rp = (int *)MALLOC_ATOMIC(n*sizeof(int));
	for ( i = 0; i < n; i++ ) rp[i] = i;
	for ( k = 0; k < n; k++ ) {
		for ( i = k; i < n && !a[i][k]; i++ );
		if ( i == n ) return 0;
		if ( i != k ) {
			j = rp[i]; rp[i] = rp[k]; rp[k] = j;
			t = a[i]; a[i] = a[k]; a[k] = t;
		}
		pivot = a[k];
		inv = invm(pivot[k],md);
		for ( i = k+1; i < n; i++ ) {
			t = a[i];
			if ( m = t[k] ) {
				DMAR(inv,m,0,md,t[k])
				for ( j = k+1, m = md - t[k]; j < n; j++ )
					if ( pivot[j] ) {
						unsigned int tj;
						DMAR(m,pivot[j],t[j],md,tj)
						t[j] = tj;
					}
			}
		}
	}
	return 1;
}

/*
  Input
	a : n x n matrix; a result of LU-decomposition
	md : modulus
	b : n x l matrix 
 Output
	b = a^(-1)b
 */

void solve_by_lu_mod(int **a,int n,int md,int **b,int l,int normalize)
{
	unsigned int *y,*c;
	int i,j,k;
	unsigned int t,m,m2;

	y = (int *)MALLOC_ATOMIC(n*sizeof(int));
	c = (int *)MALLOC_ATOMIC(n*sizeof(int));
	m2 = md>>1;
	for ( k = 0; k < l; k++ ) {
		/* copy b[.][k] to c */
		for ( i = 0; i < n; i++ )
			c[i] = (unsigned int)b[i][k];
		/* solve Ly=c */
		for ( i = 0; i < n; i++ ) {
			for ( t = c[i], j = 0; j < i; j++ )
				if ( a[i][j] ) {
					m = md - a[i][j];
					DMAR(m,y[j],t,md,t)
				}
			y[i] = t;
		}
		/* solve Uc=y */
		for ( i = n-1; i >= 0; i-- ) {
			for ( t = y[i], j =i+1; j < n; j++ )
				if ( a[i][j] ) {
					m = md - a[i][j];
					DMAR(m,c[j],t,md,t)
				}
			/* a[i][i] = 1/U[i][i] */
			DMAR(t,a[i][i],0,md,c[i])
		}
		/* copy c to b[.][k] with normalization */
		if ( normalize )
			for ( i = 0; i < n; i++ )
				b[i][k] = (int)(c[i]>m2 ? c[i]-md : c[i]);
		else
			for ( i = 0; i < n; i++ )
				b[i][k] = c[i];
	}
}

void Pleqm1(NODE arg,VECT *rp)
{
	MAT m;
	VECT vect;
	pointer **mat;
	Q *v;
	Q q;
	int **wmat;
	int md,i,j,row,col,t,n,status;

	asir_assert(ARG0(arg),O_MAT,"leqm1");
	asir_assert(ARG1(arg),O_N,"leqm1");
	m = (MAT)ARG0(arg); md = QTOS((Q)ARG1(arg));
	row = m->row; col = m->col; mat = m->body;
	wmat = (int **)almat(row,col);
	for ( i = 0; i < row; i++ )
		for ( j = 0; j < col; j++ )
			if ( q = (Q)mat[i][j] ) {
				t = rem(NM(q),md);
				if ( SGN(q) < 0 )
					t = (md - t) % md;
				wmat[i][j] = t;
			} else
				wmat[i][j] = 0;
	status = gauss_elim_mod1(wmat,row,col,md);
	if ( status < 0 )
		*rp = 0;
	else if ( status > 0 )
		*rp = (VECT)ONE;
	else {
		n = col - 1;
		MKVECT(vect,n);
		for ( i = 0, v = (Q *)vect->body; i < n; i++ ) {
			t = (md-wmat[i][n])%md; STOQ(t,v[i]);
		}
		*rp = vect;
	}
}

int gauss_elim_mod1(int **mat,int row,int col,int md)
{
	int i,j,k,inv,a,n;
	int *t,*pivot;

	n = col - 1;
	for ( j = 0; j < n; j++ ) {
		for ( i = j; i < row && !mat[i][j]; i++ );
		if ( i == row )
			return 1;
		if ( i != j ) {
			t = mat[i]; mat[i] = mat[j]; mat[j] = t;
		}
		pivot = mat[j];
		inv = invm(pivot[j],md);
		for ( k = j; k <= n; k++ )
			pivot[k] = dmar(pivot[k],inv,0,md);
		for ( i = j+1; i < row; i++ ) {
			t = mat[i];
			if ( i != j && (a = t[j]) )
				for ( k = j, a = md - a; k <= n; k++ )
					t[k] = dmar(pivot[k],a,t[k],md);
		}
	}
	for ( i = n; i < row && !mat[i][n]; i++ );
	if ( i == row ) {
		for ( j = n-1; j >= 0; j-- ) {
			for ( i = j-1, a = (md-mat[j][n])%md; i >= 0; i-- ) {
				mat[i][n] = dmar(mat[i][j],a,mat[i][n],md);
				mat[i][j] = 0;
			}
		}
		return 0;
	} else
		return -1;
}

void Pgeninvm(NODE arg,LIST *rp)
{
	MAT m;
	pointer **mat;
	Q **tmat;
	Q q;
	unsigned int **wmat;
	int md,i,j,row,col,t,status;
	MAT mat1,mat2;
	NODE node1,node2;

	asir_assert(ARG0(arg),O_MAT,"leqm1");
	asir_assert(ARG1(arg),O_N,"leqm1");
	m = (MAT)ARG0(arg); md = QTOS((Q)ARG1(arg));
	row = m->row; col = m->col; mat = m->body;
	wmat = (unsigned int **)almat(row,col+row);
	for ( i = 0; i < row; i++ ) {
		bzero((char *)wmat[i],(col+row)*sizeof(int));
		for ( j = 0; j < col; j++ )
			if ( q = (Q)mat[i][j] ) {
				t = rem(NM(q),md);
				if ( SGN(q) < 0 )
					t = (md - t) % md;
				wmat[i][j] = t;
			}
		wmat[i][col+i] = 1;
	}
	status = gauss_elim_geninv_mod(wmat,row,col,md);
	if ( status > 0 )
		*rp = 0;
	else {
		MKMAT(mat1,col,row); MKMAT(mat2,row-col,row);
		for ( i = 0, tmat = (Q **)mat1->body; i < col; i++ )
			for ( j = 0; j < row; j++ )
				UTOQ(wmat[i][j+col],tmat[i][j]);
		for ( tmat = (Q **)mat2->body; i < row; i++ )
			for ( j = 0; j < row; j++ )
				UTOQ(wmat[i][j+col],tmat[i-col][j]);
	 	MKNODE(node2,mat2,0); MKNODE(node1,mat1,node2); MKLIST(*rp,node1);
	}
}

int gauss_elim_geninv_mod(unsigned int **mat,int row,int col,int md)
{
	int i,j,k,inv,a,n,m;
	unsigned int *t,*pivot;

	n = col; m = row+col;
	for ( j = 0; j < n; j++ ) {
		for ( i = j; i < row && !mat[i][j]; i++ );
		if ( i == row )
			return 1;
		if ( i != j ) {
			t = mat[i]; mat[i] = mat[j]; mat[j] = t;
		}
		pivot = mat[j];
		inv = invm(pivot[j],md);
		for ( k = j; k < m; k++ )
			pivot[k] = dmar(pivot[k],inv,0,md);
		for ( i = j+1; i < row; i++ ) {
			t = mat[i];
			if ( a = t[j] )
				for ( k = j, a = md - a; k < m; k++ )
					t[k] = dmar(pivot[k],a,t[k],md);
		}
	}
	for ( j = n-1; j >= 0; j-- ) {
		pivot = mat[j];
		for ( i = j-1; i >= 0; i-- ) {
			t = mat[i];
			if ( a = t[j] )
				for ( k = j, a = md - a; k < m; k++ )
					t[k] = dmar(pivot[k],a,t[k],md);
		}
	}
	return 0;
}

void Psolve_by_lu_gfmmat(NODE arg,VECT *rp)
{
	GFMMAT lu;
	Q *perm,*rhs,*v;
	int n,i;
	unsigned int md;
	unsigned int *b,*sol;
	VECT r;

	lu = (GFMMAT)ARG0(arg);
	perm = (Q *)BDY((VECT)ARG1(arg));
	rhs = (Q *)BDY((VECT)ARG2(arg));
	md = (unsigned int)QTOS((Q)ARG3(arg));
	n = lu->col;
	b = (unsigned int *)MALLOC_ATOMIC(n*sizeof(int));
	sol = (unsigned int *)MALLOC_ATOMIC(n*sizeof(int));
	for ( i = 0; i < n; i++ )
		b[i] = QTOS(rhs[QTOS(perm[i])]);
	solve_by_lu_gfmmat(lu,md,b,sol);
	MKVECT(r,n);
	for ( i = 0, v = (Q *)r->body; i < n; i++ )
			UTOQ(sol[i],v[i]);
	*rp = r;
}

void solve_by_lu_gfmmat(GFMMAT lu,unsigned int md,
	unsigned int *b,unsigned int *x)
{
	int n;
	unsigned int **a;
	unsigned int *y;
	int i,j;
	unsigned int t,m;

	n = lu->col;
	a = lu->body;
	y = (unsigned int *)MALLOC_ATOMIC(n*sizeof(int));
	/* solve Ly=b */
	for ( i = 0; i < n; i++ ) {
		for ( t = b[i], j = 0; j < i; j++ )
			if ( a[i][j] ) {
				m = md - a[i][j];
				DMAR(m,y[j],t,md,t)
			}
		y[i] = t;
	}
	/* solve Ux=y */
	for ( i = n-1; i >= 0; i-- ) {
		for ( t = y[i], j =i+1; j < n; j++ )
			if ( a[i][j] ) {
				m = md - a[i][j];
				DMAR(m,x[j],t,md,t)
			}
		/* a[i][i] = 1/U[i][i] */
		DMAR(t,a[i][i],0,md,x[i])
	}
}

void Plu_mat(NODE arg,LIST *rp)
{
	MAT m,lu;
	Q dn;
	Q *v;
	int n,i;
	int *iperm;
	VECT perm;
	NODE n0;

	asir_assert(ARG0(arg),O_MAT,"lu_mat");
	m = (MAT)ARG0(arg);
	n = m->row;
	MKMAT(lu,n,n);
	lu_dec_cr(m,lu,&dn,&iperm);
	MKVECT(perm,n);
	for ( i = 0, v = (Q *)perm->body; i < n; i++ )
		STOQ(iperm[i],v[i]);
	n0 = mknode(3,lu,dn,perm);
	MKLIST(*rp,n0);
}

void Plu_gfmmat(NODE arg,LIST *rp)
{
	MAT m;
	GFMMAT mm;
	unsigned int md;
	int i,row,col,status;
	int *iperm;
	Q *v;
	VECT perm;
	NODE n0;

	asir_assert(ARG0(arg),O_MAT,"lu_gfmmat");
	asir_assert(ARG1(arg),O_N,"lu_gfmmat");
	m = (MAT)ARG0(arg); md = (unsigned int)QTOS((Q)ARG1(arg));
	mat_to_gfmmat(m,md,&mm);
	row = m->row;
	col = m->col;
	iperm = (int *)MALLOC_ATOMIC(row*sizeof(int));
	status = lu_gfmmat(mm,md,iperm);	
	if ( !status )
		n0 = 0;
	else {
		MKVECT(perm,row);
		for ( i = 0, v = (Q *)perm->body; i < row; i++ )
			STOQ(iperm[i],v[i]);
		n0 = mknode(2,mm,perm);
	}
	MKLIST(*rp,n0);
}

void Pmat_to_gfmmat(NODE arg,GFMMAT *rp)
{
	MAT m;
	unsigned int md;

	asir_assert(ARG0(arg),O_MAT,"mat_to_gfmmat");
	asir_assert(ARG1(arg),O_N,"mat_to_gfmmat");
	m = (MAT)ARG0(arg); md = (unsigned int)QTOS((Q)ARG1(arg));
	mat_to_gfmmat(m,md,rp);	
}

void mat_to_gfmmat(MAT m,unsigned int md,GFMMAT *rp)
{
	unsigned int **wmat;
	unsigned int t;
	Q **mat;
	Q q;
	int i,j,row,col;

	row = m->row; col = m->col; mat = (Q **)m->body;
	wmat = (unsigned int **)almat(row,col);
	for ( i = 0; i < row; i++ ) {
		bzero((char *)wmat[i],col*sizeof(unsigned int));
		for ( j = 0; j < col; j++ )
			if ( q = mat[i][j] ) {
				t = (unsigned int)rem(NM(q),md);
				if ( SGN(q) < 0 )
					t = (md - t) % md;
				wmat[i][j] = t;
			}
	}
	TOGFMMAT(row,col,wmat,*rp);
}

void Pgeninvm_swap(arg,rp)
NODE arg;
LIST *rp;
{
	MAT m;
	pointer **mat;
	Q **tmat;
	Q *tvect;
	Q q;
	unsigned int **wmat,**invmat;
	int *index;
	unsigned int t,md;
	int i,j,row,col,status;
	MAT mat1;
	VECT vect1;
	NODE node1,node2;

	asir_assert(ARG0(arg),O_MAT,"geninvm_swap");
	asir_assert(ARG1(arg),O_N,"geninvm_swap");
	m = (MAT)ARG0(arg); md = QTOS((Q)ARG1(arg));
	row = m->row; col = m->col; mat = m->body;
	wmat = (unsigned int **)almat(row,col+row);
	for ( i = 0; i < row; i++ ) {
		bzero((char *)wmat[i],(col+row)*sizeof(int));
		for ( j = 0; j < col; j++ )
			if ( q = (Q)mat[i][j] ) {
				t = (unsigned int)rem(NM(q),md);
				if ( SGN(q) < 0 )
					t = (md - t) % md;
				wmat[i][j] = t;
			}
		wmat[i][col+i] = 1;
	}
	status = gauss_elim_geninv_mod_swap(wmat,row,col,md,&invmat,&index);
	if ( status > 0 )
		*rp = 0;
	else {
		MKMAT(mat1,col,col);
		for ( i = 0, tmat = (Q **)mat1->body; i < col; i++ )
			for ( j = 0; j < col; j++ )
				UTOQ(invmat[i][j],tmat[i][j]);
		MKVECT(vect1,row);
		for ( i = 0, tvect = (Q *)vect1->body; i < row; i++ )
			STOQ(index[i],tvect[i]);
	 	MKNODE(node2,vect1,0); MKNODE(node1,mat1,node2); MKLIST(*rp,node1);
	}
}

gauss_elim_geninv_mod_swap(mat,row,col,md,invmatp,indexp)
unsigned int **mat;
int row,col;
unsigned int md;
unsigned int ***invmatp;
int **indexp;
{
	int i,j,k,inv,a,n,m;
	unsigned int *t,*pivot,*s;
	int *index;
	unsigned int **invmat;

	n = col; m = row+col;
	*indexp = index = (int *)MALLOC_ATOMIC(row*sizeof(int));
	for ( i = 0; i < row; i++ )
		index[i] = i;
	for ( j = 0; j < n; j++ ) {
		for ( i = j; i < row && !mat[i][j]; i++ );
		if ( i == row ) {
			*indexp = 0; *invmatp = 0; return 1;
		}
		if ( i != j ) {
			t = mat[i]; mat[i] = mat[j]; mat[j] = t;
			k = index[i]; index[i] = index[j]; index[j] = k;	
		}
		pivot = mat[j];
		inv = (unsigned int)invm(pivot[j],md);
		for ( k = j; k < m; k++ )
			if ( pivot[k] )
				pivot[k] = (unsigned int)dmar(pivot[k],inv,0,md);
		for ( i = j+1; i < row; i++ ) {
			t = mat[i];
			if ( a = t[j] )
				for ( k = j, a = md - a; k < m; k++ )
					if ( pivot[k] )
						t[k] = dmar(pivot[k],a,t[k],md);
		}
	}
	for ( j = n-1; j >= 0; j-- ) {
		pivot = mat[j];
		for ( i = j-1; i >= 0; i-- ) {
			t = mat[i];
			if ( a = t[j] )
				for ( k = j, a = md - a; k < m; k++ )
					if ( pivot[k] )
						t[k] = dmar(pivot[k],a,t[k],md);
		}
	}
	*invmatp = invmat = (unsigned int **)almat(col,col);
	for ( i = 0; i < col; i++ )
		for ( j = 0, s = invmat[i], t = mat[i]; j < col; j++ )
			s[j] = t[col+index[j]];
	return 0;
}

void Pgeninv_sf_swap(NODE arg,LIST *rp)
{
	MAT m;
	GFS **mat,**tmat;
	Q *tvect;
	GFS q;
	int **wmat,**invmat;
	int *index;
	unsigned int t;
	int i,j,row,col,status;
	MAT mat1;
	VECT vect1;
	NODE node1,node2;

	asir_assert(ARG0(arg),O_MAT,"geninv_sf_swap");
	m = (MAT)ARG0(arg);
	row = m->row; col = m->col; mat = (GFS **)m->body;
	wmat = (int **)almat(row,col+row);
	for ( i = 0; i < row; i++ ) {
		bzero((char *)wmat[i],(col+row)*sizeof(int));
		for ( j = 0; j < col; j++ )
			if ( q = (GFS)mat[i][j] )
				wmat[i][j] = FTOIF(CONT(q));
		wmat[i][col+i] = _onesf();
	}
	status = gauss_elim_geninv_sf_swap(wmat,row,col,&invmat,&index);
	if ( status > 0 )
		*rp = 0;
	else {
		MKMAT(mat1,col,col);
		for ( i = 0, tmat = (GFS **)mat1->body; i < col; i++ )
			for ( j = 0; j < col; j++ )
				if ( t = invmat[i][j] ) {
					MKGFS(IFTOF(t),tmat[i][j]);
				}
		MKVECT(vect1,row);
		for ( i = 0, tvect = (Q *)vect1->body; i < row; i++ )
			STOQ(index[i],tvect[i]);
	 	MKNODE(node2,vect1,0); MKNODE(node1,mat1,node2); MKLIST(*rp,node1);
	}
}

int gauss_elim_geninv_sf_swap(int **mat,int row,int col,
	int ***invmatp,int **indexp)
{
	int i,j,k,inv,a,n,m,u;
	int *t,*pivot,*s;
	int *index;
	int **invmat;

	n = col; m = row+col;
	*indexp = index = (int *)MALLOC_ATOMIC(row*sizeof(int));
	for ( i = 0; i < row; i++ )
		index[i] = i;
	for ( j = 0; j < n; j++ ) {
		for ( i = j; i < row && !mat[i][j]; i++ );
		if ( i == row ) {
			*indexp = 0; *invmatp = 0; return 1;
		}
		if ( i != j ) {
			t = mat[i]; mat[i] = mat[j]; mat[j] = t;
			k = index[i]; index[i] = index[j]; index[j] = k;	
		}
		pivot = mat[j];
		inv = _invsf(pivot[j]);
		for ( k = j; k < m; k++ )
			if ( pivot[k] )
				pivot[k] = _mulsf(pivot[k],inv);
		for ( i = j+1; i < row; i++ ) {
			t = mat[i];
			if ( a = t[j] )
				for ( k = j, a = _chsgnsf(a); k < m; k++ )
					if ( pivot[k] ) {
						u = _mulsf(pivot[k],a);
						t[k] = _addsf(u,t[k]);
					}
		}
	}
	for ( j = n-1; j >= 0; j-- ) {
		pivot = mat[j];
		for ( i = j-1; i >= 0; i-- ) {
			t = mat[i];
			if ( a = t[j] )
				for ( k = j, a = _chsgnsf(a); k < m; k++ )
					if ( pivot[k] ) {
						u = _mulsf(pivot[k],a);
						t[k] = _addsf(u,t[k]);
					}
		}
	}
	*invmatp = invmat = (int **)almat(col,col);
	for ( i = 0; i < col; i++ )
		for ( j = 0, s = invmat[i], t = mat[i]; j < col; j++ )
			s[j] = t[col+index[j]];
	return 0;
}

void _addn(N,N,N);
int _subn(N,N,N);
void _muln(N,N,N);

void inner_product_int(Q *a,Q *b,int n,Q *r)
{
	int la,lb,i;
	int sgn,sgn1;
	N wm,wma,sum,t;

	for ( la = lb = 0, i = 0; i < n; i++ ) {
		if ( a[i] )
			if ( DN(a[i]) )
				error("inner_product_int : invalid argument");
			else
				la = MAX(PL(NM(a[i])),la);
		if ( b[i] )
			if ( DN(b[i]) )
				error("inner_product_int : invalid argument");
			else
				lb = MAX(PL(NM(b[i])),lb);
	}
	sgn = 0;
	sum= NALLOC(la+lb+2);
	bzero((char *)sum,(la+lb+3)*sizeof(unsigned int));
	wm = NALLOC(la+lb+2);
	wma = NALLOC(la+lb+2);
	for ( i = 0; i < n; i++ ) {
		if ( !a[i] || !b[i] )
			continue;
		_muln(NM(a[i]),NM(b[i]),wm);
		sgn1 = SGN(a[i])*SGN(b[i]);
		if ( !sgn ) {
			sgn = sgn1;
			t = wm; wm = sum; sum = t;
		} else if ( sgn == sgn1 ) {
			_addn(sum,wm,wma);
			if ( !PL(wma) )
				sgn = 0;
			t = wma; wma = sum; sum = t;
		} else {
			/* sgn*sum+sgn1*wm = sgn*(sum-wm) */
			sgn *= _subn(sum,wm,wma);
			t = wma; wma = sum; sum = t;
		}
	}
	GCFREE(wm);
	GCFREE(wma);
	if ( !sgn ) {
		GCFREE(sum);
		*r = 0;
	} else
		NTOQ(sum,sgn,*r);
}

/* (k,l) element of a*b where a: .x n matrix, b: n x . integer matrix */

void inner_product_mat_int_mod(Q **a,int **b,int n,int k,int l,Q *r)
{
	int la,lb,i;
	int sgn,sgn1;
	N wm,wma,sum,t;
	Q aki;
	int bil,bilsgn;
	struct oN tn;

	for ( la = 0, i = 0; i < n; i++ ) {
		if ( aki = a[k][i] )
			if ( DN(aki) )
				error("inner_product_int : invalid argument");
			else
				la = MAX(PL(NM(aki)),la);
	}
	lb = 1;
	sgn = 0;
	sum= NALLOC(la+lb+2);
	bzero((char *)sum,(la+lb+3)*sizeof(unsigned int));
	wm = NALLOC(la+lb+2);
	wma = NALLOC(la+lb+2);
	for ( i = 0; i < n; i++ ) {
		if ( !(aki = a[k][i]) || !(bil = b[i][l]) )
			continue;
		tn.p = 1;
		if ( bil > 0 ) {
			tn.b[0] = bil; bilsgn = 1;
		} else {
			tn.b[0] = -bil; bilsgn = -1;
		}
		_muln(NM(aki),&tn,wm);
		sgn1 = SGN(aki)*bilsgn;
		if ( !sgn ) {
			sgn = sgn1;
			t = wm; wm = sum; sum = t;
		} else if ( sgn == sgn1 ) {
			_addn(sum,wm,wma);
			if ( !PL(wma) )
				sgn = 0;
			t = wma; wma = sum; sum = t;
		} else {
			/* sgn*sum+sgn1*wm = sgn*(sum-wm) */
			sgn *= _subn(sum,wm,wma);
			t = wma; wma = sum; sum = t;
		}
	}
	GCFREE(wm);
	GCFREE(wma);
	if ( !sgn ) {
		GCFREE(sum);
		*r = 0;
	} else
		NTOQ(sum,sgn,*r);
}

void Pmul_mat_vect_int(NODE arg,VECT *rp)
{
	MAT mat;
	VECT vect,r;
	int row,col,i;

	mat = (MAT)ARG0(arg);
	vect = (VECT)ARG1(arg);
	row = mat->row;
	col = mat->col;
	MKVECT(r,row);
	for ( i = 0; i < row; i++ ) {
		inner_product_int((Q *)mat->body[i],(Q *)vect->body,col,(Q *)&r->body[i]);
	}
	*rp = r;
}

void Pnbpoly_up2(NODE arg,GF2N *rp)
{
	int m,type,ret;
	UP2 r;

	m = QTOS((Q)ARG0(arg));
	type = QTOS((Q)ARG1(arg));
	ret = generate_ONB_polynomial(&r,m,type);
	if ( ret == 0 )
		MKGF2N(r,*rp);
	else
		*rp = 0;
}

void Px962_irredpoly_up2(NODE arg,GF2N *rp)
{
	int m,ret,w;
	GF2N prev;
	UP2 r;

	m = QTOS((Q)ARG0(arg));
	prev = (GF2N)ARG1(arg);
	if ( !prev ) {
		w = (m>>5)+1; NEWUP2(r,w); r->w = 0;
		bzero((char *)r->b,w*sizeof(unsigned int));
	} else {
		r = prev->body;
		if ( degup2(r) != m ) {
			w = (m>>5)+1; NEWUP2(r,w); r->w = 0;
			bzero((char *)r->b,w*sizeof(unsigned int));
		}
	}
	ret = _generate_irreducible_polynomial(r,m);
	if ( ret == 0 )
		MKGF2N(r,*rp);
	else
		*rp = 0;
}

void Pirredpoly_up2(NODE arg,GF2N *rp)
{
	int m,ret,w;
	GF2N prev;
	UP2 r;

	m = QTOS((Q)ARG0(arg));
	prev = (GF2N)ARG1(arg);
	if ( !prev ) {
		w = (m>>5)+1; NEWUP2(r,w); r->w = 0;
		bzero((char *)r->b,w*sizeof(unsigned int));
	} else {
		r = prev->body;
		if ( degup2(r) != m ) {
			w = (m>>5)+1; NEWUP2(r,w); r->w = 0;
			bzero((char *)r->b,w*sizeof(unsigned int));
		}
	}
	ret = _generate_good_irreducible_polynomial(r,m);
	if ( ret == 0 )
		MKGF2N(r,*rp);
	else
		*rp = 0;
}

void Pmat_swap_row_destructive(NODE arg, MAT *m)
{
	int i1,i2;
	pointer *t;
	MAT mat;

	asir_assert(ARG0(arg),O_MAT,"mat_swap_row_destructive");
	asir_assert(ARG1(arg),O_N,"mat_swap_row_destructive");
	asir_assert(ARG2(arg),O_N,"mat_swap_row_destructive");
	mat = (MAT)ARG0(arg);
	i1 = QTOS((Q)ARG1(arg));
	i2 = QTOS((Q)ARG2(arg));
	if ( i1 < 0 || i2 < 0 || i1 >= mat->row || i2 >= mat->row )
		error("mat_swap_row_destructive : Out of range");
	t = mat->body[i1];
	mat->body[i1] = mat->body[i2];
	mat->body[i2] = t;
	*m = mat;
}

void Pmat_swap_col_destructive(NODE arg, MAT *m)
{
	int j1,j2,i,n;
	pointer *mi;
	pointer t;
	MAT mat;

	asir_assert(ARG0(arg),O_MAT,"mat_swap_col_destructive");
	asir_assert(ARG1(arg),O_N,"mat_swap_col_destructive");
	asir_assert(ARG2(arg),O_N,"mat_swap_col_destructive");
	mat = (MAT)ARG0(arg);
	j1 = QTOS((Q)ARG1(arg));
	j2 = QTOS((Q)ARG2(arg));
	if ( j1 < 0 || j2 < 0 || j1 >= mat->col || j2 >= mat->col )
		error("mat_swap_col_destructive : Out of range");
	n = mat->row;
	for ( i = 0; i < n; i++ ) {
		mi = mat->body[i];
		t = mi[j1]; mi[j1] = mi[j2]; mi[j2] = t;
	}
	*m = mat;
}
/*
 * f = type 'type' normal polynomial of degree m if exists
 * IEEE P1363 A.7.2
 *
 * return value : 0  --- exists
 *                1  --- does not exist
 *                -1 --- failure (memory allocation error)
 */

int generate_ONB_polynomial(UP2 *rp,int m,int type)
{
	int i,r;
	int w;
	UP2 f,f0,f1,f2,t;

	w = (m>>5)+1;
	switch ( type ) {
		case 1:
			if ( !TypeT_NB_check(m,1) ) return 1;
			NEWUP2(f,w); *rp = f; f->w = w;
			/* set all the bits */
			for ( i = 0; i < w; i++ )
				f->b[i] = 0xffffffff;
			/* mask the top word if necessary */
			if ( r = (m+1)&31 )
				f->b[w-1] &= (1<<r)-1;
			return 0;
			break;
		case 2:
			if ( !TypeT_NB_check(m,2) ) return 1;
			NEWUP2(f,w); *rp = f;
			W_NEWUP2(f0,w);
			W_NEWUP2(f1,w);
			W_NEWUP2(f2,w);

			/* recursion for genrating Type II normal polynomial */

			/* f0 = 1, f1 = t+1 */
			f0->w = 1; f0->b[0] = 1; 
			f1->w = 1; f1->b[0] = 3;
			for ( i = 2; i <= m; i++ ) {
				/* f2 = t*f1+f0 */
				_bshiftup2(f1,-1,f2);
				_addup2_destructive(f2,f0);
				/* cyclic change of the variables */
				t = f0; f0 = f1; f1 = f2; f2 = t;
			}
			_copyup2(f1,f);
			return 0;
			break;
		default:
			return -1;
			break;
		}
}

/*
 * f = an irreducible trinomial or pentanomial of degree d 'after' f
 * return value : 0  --- exists
 *                1  --- does not exist (exhaustion)
 */

int _generate_irreducible_polynomial(UP2 f,int d)
{
	int ret,i,j,k,nz,i0,j0,k0;
	int w;
	unsigned int *fd;

	/*
	 * if f = x^d+x^i+1 then i0 <- i, j0 <- 0, k0 <-0.
	 * if f = x^d+x^k+x^j+x^i+1 (k>j>i) then i0 <- i, j0 <- j, k0 <-k.
	 * otherwise i0,j0,k0 is set to 0.
	 */

	fd = f->b;
	w = (d>>5)+1;
	if ( f->w && (d==degup2(f)) ) {
		for ( nz = 0, i = d; i >= 0; i-- )
			if ( fd[i>>5]&(1<<(i&31)) ) nz++;
		switch ( nz ) {
			case 3:
				for ( i0 = 1; !(fd[i0>>5]&(1<<(i0&31))) ; i0++ );
				/* reset i0-th bit */
				fd[i0>>5] &= ~(1<<(i0&31));
				j0 = k0 = 0;
				break;
			case 5:
				for ( i0 = 1; !(fd[i0>>5]&(1<<(i0&31))) ; i0++ );
				/* reset i0-th bit */
				fd[i0>>5] &= ~(1<<(i0&31));
				for ( j0 = i0+1; !(fd[j0>>5]&(1<<(j0&31))) ; j0++ );
				/* reset j0-th bit */
				fd[j0>>5] &= ~(1<<(j0&31));
				for ( k0 = j0+1; !(fd[k0>>5]&(1<<(k0&31))) ; k0++ );
				/* reset k0-th bit */
				fd[k0>>5] &= ~(1<<(k0&31));
				break;
			default:
				f->w = 0; break;
		}
	} else 
		f->w = 0;

	if ( !f->w ) {
		fd = f->b;
		f->w = w; fd[0] |= 1; fd[d>>5] |= (1<<(d&31));
		i0 = j0 = k0 = 0;
	}
	/* if j0 > 0 then f is already a pentanomial */
	if ( j0 > 0 ) goto PENTA;

	/* searching for an irreducible trinomial */

	for ( i = 1; 2*i <= d; i++ ) {
		/* skip the polynomials 'before' f */
		if ( i < i0 ) continue;
		if ( i == i0 ) { i0 = 0; continue; }
		/* set i-th bit */
		fd[i>>5] |= (1<<(i&31));
		ret = irredcheck_dddup2(f);
		if ( ret == 1 ) return 0;
		/* reset i-th bit */
		fd[i>>5] &= ~(1<<(i&31));
	}

	/* searching for an irreducible pentanomial */
PENTA:
	for ( i = 1; i < d; i++ ) {
		/* skip the polynomials 'before' f */
		if ( i < i0 ) continue;
		if ( i == i0 ) i0 = 0;
		/* set i-th bit */
		fd[i>>5] |= (1<<(i&31));
		for ( j = i+1; j < d; j++ ) {
			/* skip the polynomials 'before' f */
			if ( j < j0 ) continue;
			if ( j == j0 ) j0 = 0;
			/* set j-th bit */
			fd[j>>5] |= (1<<(j&31));
			for ( k = j+1; k < d; k++ ) {
				/* skip the polynomials 'before' f */
				if ( k < k0 ) continue;
				else if ( k == k0 ) { k0 = 0; continue; }
				/* set k-th bit */
				fd[k>>5] |= (1<<(k&31));
				ret = irredcheck_dddup2(f);
				if ( ret == 1 ) return 0;
				/* reset k-th bit */
				fd[k>>5] &= ~(1<<(k&31));
			}
			/* reset j-th bit */
			fd[j>>5] &= ~(1<<(j&31));
		}
		/* reset i-th bit */
		fd[i>>5] &= ~(1<<(i&31));
	}
	/* exhausted */
	return 1;
}

/*
 * f = an irreducible trinomial or pentanomial of degree d 'after' f
 * 
 * searching strategy:
 *   trinomial x^d+x^i+1:
 *         i is as small as possible.
 *   trinomial x^d+x^i+x^j+x^k+1: 
 *         i is as small as possible.
 *         For such i, j is as small as possible.
 *         For such i and j, 'k' is as small as possible.
 *
 * return value : 0  --- exists
 *                1  --- does not exist (exhaustion)
 */

int _generate_good_irreducible_polynomial(UP2 f,int d)
{
	int ret,i,j,k,nz,i0,j0,k0;
	int w;
	unsigned int *fd;

	/*
	 * if f = x^d+x^i+1 then i0 <- i, j0 <- 0, k0 <-0.
	 * if f = x^d+x^k+x^j+x^i+1 (k>j>i) then i0 <- i, j0 <- j, k0 <-k.
	 * otherwise i0,j0,k0 is set to 0.
	 */

	fd = f->b;
	w = (d>>5)+1;
	if ( f->w && (d==degup2(f)) ) {
		for ( nz = 0, i = d; i >= 0; i-- )
			if ( fd[i>>5]&(1<<(i&31)) ) nz++;
		switch ( nz ) {
			case 3:
				for ( i0 = 1; !(fd[i0>>5]&(1<<(i0&31))) ; i0++ );
				/* reset i0-th bit */
				fd[i0>>5] &= ~(1<<(i0&31));
				j0 = k0 = 0;
				break;
			case 5:
				for ( i0 = 1; !(fd[i0>>5]&(1<<(i0&31))) ; i0++ );
				/* reset i0-th bit */
				fd[i0>>5] &= ~(1<<(i0&31));
				for ( j0 = i0+1; !(fd[j0>>5]&(1<<(j0&31))) ; j0++ );
				/* reset j0-th bit */
				fd[j0>>5] &= ~(1<<(j0&31));
				for ( k0 = j0+1; !(fd[k0>>5]&(1<<(k0&31))) ; k0++ );
				/* reset k0-th bit */
				fd[k0>>5] &= ~(1<<(k0&31));
				break;
			default:
				f->w = 0; break;
		}
	} else 
		f->w = 0;

	if ( !f->w ) {
		fd = f->b;
		f->w = w; fd[0] |= 1; fd[d>>5] |= (1<<(d&31));
		i0 = j0 = k0 = 0;
	}
	/* if j0 > 0 then f is already a pentanomial */
	if ( j0 > 0 ) goto PENTA;

	/* searching for an irreducible trinomial */

	for ( i = 1; 2*i <= d; i++ ) {
		/* skip the polynomials 'before' f */
		if ( i < i0 ) continue;
		if ( i == i0 ) { i0 = 0; continue; }
		/* set i-th bit */
		fd[i>>5] |= (1<<(i&31));
		ret = irredcheck_dddup2(f);
		if ( ret == 1 ) return 0;
		/* reset i-th bit */
		fd[i>>5] &= ~(1<<(i&31));
	}

	/* searching for an irreducible pentanomial */
PENTA:
	for ( i = 3; i < d; i++ ) {
		/* skip the polynomials 'before' f */
		if ( i < i0 ) continue;
		if ( i == i0 ) i0 = 0;
		/* set i-th bit */
		fd[i>>5] |= (1<<(i&31));
 		for ( j = 2; j < i; j++ ) {
			/* skip the polynomials 'before' f */
			if ( j < j0 ) continue;
			if ( j == j0 ) j0 = 0;
			/* set j-th bit */
			fd[j>>5] |= (1<<(j&31));
 			for ( k = 1; k < j; k++ ) {
				/* skip the polynomials 'before' f */
				if ( k < k0 ) continue;
				else if ( k == k0 ) { k0 = 0; continue; }
				/* set k-th bit */
				fd[k>>5] |= (1<<(k&31));
				ret = irredcheck_dddup2(f);
				if ( ret == 1 ) return 0;
				/* reset k-th bit */
				fd[k>>5] &= ~(1<<(k&31));
			}
			/* reset j-th bit */
			fd[j>>5] &= ~(1<<(j&31));
		}
		/* reset i-th bit */
		fd[i>>5] &= ~(1<<(i&31));
	}
	/* exhausted */
	return 1;
}

void printqmat(Q **mat,int row,int col)
{
	int i,j;

	for ( i = 0; i < row; i++ ) {
		for ( j = 0; j < col; j++ ) {
			printnum((Num)mat[i][j]); printf(" ");
		}
		printf("\n");
	}
}

void printimat(int **mat,int row,int col)
{
	int i,j;

	for ( i = 0; i < row; i++ ) {
		for ( j = 0; j < col; j++ ) {
			printf("%d ",mat[i][j]);
		}
		printf("\n");
	}
}

void Pnd_det(NODE arg,P *rp)
{
	if ( argc(arg) == 1 )
		nd_det(0,ARG0(arg),rp);
	else
		nd_det(QTOS((Q)ARG1(arg)),ARG0(arg),rp);
}

void Pmat_col(NODE arg,VECT *rp)
{
	int i,j,n;
	MAT mat;
	VECT vect;

	asir_assert(ARG0(arg),O_MAT,"mat_col");
	asir_assert(ARG1(arg),O_N,"mat_col");
	mat = (MAT)ARG0(arg);
	j = QTOS((Q)ARG1(arg));
	if ( j < 0 || j >= mat->col) {
		error("mat_col : Out of range");
	}
	n = mat->row;
	MKVECT(vect,n);
	for(i=0; i<n; i++) {
		BDY(vect)[i] = BDY(mat)[i][j];
	}
	*rp = vect;
}
