/*
 * $OpenXM$
*/
#include "ca.h"
#include "base.h"
#include "parse.h"
#include "inline.h"

extern int StrassenSize;

struct ftab imat_tab[] = {
	{"newimat", Pnewimat,2},
	{"m2im",  Pm2Im,1},
	{"im2m",  PIm2m,1},
	{0,0,0},
};

const IENT zent = { -1, -1, -1, 0}; /* last ient const */

void MEnt(cr, row, col, trg, ent)
int cr, row, col;
Obj trg;
IENT *ent;
{
	/* make Index Entry IENT */
	/* set:cr, row, trag */
	/* return: value ent */
	/* MEnt(cr, row, col, trg, &ent); */

	ent->cr = cr;
	ent->row = row;
	ent->col = col;
	ent->body = (pointer)trg;
}

void GetNextIent(Im, ent, c)
IMATC *Im;
IENT *ent;
int *c;
{
	/* get next IENT */
	/* set: Im, c */
	/* return: ent, c */
	/* GetNextIent(&Im, &ent, &c); */

	if( (++*c) >= IMATCH ){
		if ( ! (IMATC)(*Im)->next ) {
		/* no more IENT */
			*c = -1;
			return;
		}
		*Im = (IMATC)(*Im)->next;
		*c = 0;
	}
	*ent = (*Im)->ient[*c];
}

void GetForeIent(Im, ent, c)
IMATC *Im;
IENT *ent;
int *c;
{
	/* GetForeIent(&Im, &ent, &c); */

	/* find last IENT */
	if( (--*c) < 0 ){
		if ( !(IMATC)(*Im)->fore ){
		/* no more IENT */
			*c = -1;
			return;
		}
		/* next IENT block */
		*Im = (IMATC)(*Im)->fore;
		*c = IMATCH - 1;
	}
	*ent = (*Im)->ient[*c];
}

void AppendIent(m, row, col, body)
IMAT m;
int row, col;
Obj body;
{
	/* append row, col, body to matrix m */
	IMATC Im, Imt;
	int d;

	if ( ! body ) return;
	if ( ! m->clen ) {
		NEWIENT(Im);
		m->root = (pointer)Im;
		m->toor = (pointer)Im;
		m->clen = 1;
		MEnt(row * m->row + col, row, col, body, &Im->ient[0]);
		Im->ient[1] = zent;
	} else {
		Im = (pointer)m->toor;
		for( d = 0; Im->ient[d].cr != -1; d++);
		if ( d == IMATCH - 1 ) {
			/* append point is chank end */
			NEWIENT(Imt);
			Im->next = (pointer)Imt;
			m->toor = (pointer)Imt;
			++(m->clen);
			Imt->fore = (pointer)Im;
			Imt->next = 0;
			MEnt(row * m->row + col, row, col, body, &Im->ient[d]);
			Imt->ient[0] = zent;
		} else {
			MEnt(row * m->row + col, row, col, body, &Im->ient[d]);
			Im->ient[d + 1] = zent;
		}
	}
}

void PutIent(m, row, col, trg)
IMAT m;
int row, col;
Obj trg;
{
	/* insert or replace IENT */
	IMATC Im, Imn;
	IENT ent, tent;
	int cr, c, d;

	if ( m->row <= row || m->col <= col || row < 0 || col < 0 )
		error("putim : Out of rage");
	cr = row * m->row + col;
	if ( ! m->clen ) {
		if( trg == 0 ) return;
		AppendIent(m, row, col, trg);
		return;
	}
	MEnt(cr, row, col, trg, &tent);
	Im = (pointer)m->root;
	c = -1;
	GetNextIent(&Im, &ent, &c);
	while (ent.cr < cr) {
		GetNextIent(&Im, &ent, &c);
		if( c == -1 ) {
			/* zero insert case */
			if ( ! trg ) return;
			/* last append case */
			AppendIent(m, row, col, trg);
			return;
		}
	}
	if ( ent.cr == cr ) {
		/* same row and col (replace) case */
		if ( ! trg ) {
			/* IENT remove case */
			Imn = Im;
			while ( c != -1 ) {
				GetNextIent( &Im, &ent, &c );
				if ( ! c ) {
					Imn->ient[IMATCH-1] = ent;
					Imn = Im;
				} else {
					Im->ient[c - 1] = ent;
				}
			}
		} else {
			/* replase case */
			Im->ient[c] = tent;
		}
	} else {
		/* IENT insert case */
		while( c != -1 ) {
			Im->ient[c] = tent;
			tent = ent;
			GetNextIent(&Im, &ent, &c);
		}

	}
}

void Pnewimat(arg, rp)
NODE arg;
IMAT *rp;
{
	/* make new index type matrix for parser */
	int row,col;
	IMAT m;

	asir_assert(ARG0(arg),O_N,"newimat");
	asir_assert(ARG1(arg),O_N,"newimat");
	row = QTOS((Q)ARG0(arg)); col = QTOS((Q)ARG1(arg));
	if ( row <= 0 || col <= 0 ) error("newimat : invalid size");
	NEWIMAT(m);
	m->col = col;
	m->row = row;
	*rp = m;
}

void GetIbody(m, row, col, trg)
IMAT m;
int col, row;
Obj *trg;
{
	/* get entry body from m for parser */
	IMATC Im;
	IENT ent;
	int cr;
	int c;

	if ( m->row <= row || m->col <= col || row < 0 || col < 0 )
		error("putim : Out of rage");
	if ( ! m->clen ) {
	/* zero matrix case */
		*trg = (Obj)0;
	} else {
		cr = row * m->row + col;
		c = -1;
		Im = (pointer)m->root;
		GetNextIent( &Im, &ent, &c);
		while( ent.cr < cr ) {
			if ( ent.cr == -1 ) {
			/* not fined ent to last case */
				*trg = (Obj)0;
				return;
			}
			GetNextIent( &Im, &ent, &c);
		}
		if ( ent.cr == cr ) *trg = (Obj)ent.body;
		else *trg = (Obj)0; /* not fined end to mid case */
	}
}

void PChsgnI(arg, rp)
NODE arg;
IMAT *rp;
{
	/* index type matrix all entry sgn cheng for parser */
	VL vl;
	IMAT m, n;

	asir_assert(ARG0(arg),O_IMAT,"chsgnind");
	vl = CO;
	m = (IMAT)ARG0(arg);
	ChsgnI(m, &n);
	*rp = n;
}

void ChsgnI(a, c)
IMAT a;
IMAT *c;
{
	/* index type matrix all entry sgn chg */
	IMAT b;
	IMATC ac;
	IENT aent;
	Obj r;
	int ai;

	if( ! a->root ){
		/* zero matrix case */
		*c = a;
	} else {
		NEWIMAT(b);
		b->col = a->col;
		b->row = a->row;
		ai = -1;
		ac = (pointer)a->root;
		GetNextIent(&ac, &aent, &ai);
		while(aent.cr != -1){
			arf_chsgn((Obj)aent.body, (Obj *)&r);
			AppendIent(b, aent.row, aent.col, r);
			GetNextIent(&ac, &aent, &ai);
		}
		*c = b;
	}
}

void Pm2Im(arg, rp)
NODE arg;
IMAT *rp;
{
	/* matrix type convert from MAT to IMAT */
	int i,j, row, col;
	MAT m;
	IMAT Im;
	pointer *a;

	m = (MAT)ARG0(arg);
	row = m->row;
	col = m->col;
	NEWIMAT(Im);
	Im->col = m->col;
	Im->row = m->row;
	for (i = 0; i < row; i++)
		for (j = 0, a = BDY(m)[i]; j < col; j++) 
			if (a[j]) AppendIent(Im, i, j, a[j]);
	*rp = Im;
}

void PIm2m(arg, rp)
NODE arg;
MAT *rp;
{
	/* matrix type convert from IMAT to MAT */
	IMAT Im;
	MAT m;
	int c, i, j;
	IMATC Imc;
	IENT ent;

	Im = (IMAT)ARG0(arg);
	MKMAT(m,Im->row,Im->col);
	if ( Im->root ) {
		/* non zero matrix case */
		Imc = (pointer)Im->root;
		c = -1;
		GetNextIent(&Imc, &ent, &c);
		while (ent.cr != -1) {
			BDY(m)[ent.row][ent.col] = ent.body;
			GetNextIent(&Imc, &ent, &c);
		}
	}
	*rp = m;
}

void AddMatI(vl, a, b, c)
VL vl;
IMAT a, b, *c;
{
	/* add index type matrix */
	int ai, bi;
	IMAT m;
	IMATC ac, bc;
	IENT aent, bent;
	Obj pc;

	if ( a->col != b->col ) {
		error("addmati : colum size mismatch");
		*c = 0;
	} else if ( a->row != b->row ) {
		error("addmati : row size mismatch");
		*c = 0;
	} else if ( ! a->root ) {
		*c = b; /* a : zero matrox */
	} else if ( ! b->root ) {
		*c = a; /* b : zero matrix */
	} else {
		NEWIMAT(m);
		m->col = a->col;
		m->row = a->row;
		ai = -1;
		ac = (pointer)a->root;
		bi = -1;
		bc = (pointer)b->root;
		GetNextIent(&ac, &aent, &ai);
		GetNextIent(&bc, &bent, &bi);
		while( aent.cr != -1 ) {
			if( aent.cr == bent.cr ) {
				arf_add(vl, (Obj)aent.body, (Obj)bent.body, (Obj *)&pc);
				AppendIent( m, aent.row, aent.col, pc);
				GetNextIent( &ac, &aent, &ai );
				GetNextIent( &bc, &bent, &bi );
			} else if ( aent.cr < bent.cr ) {
				AppendIent( m, aent.row, aent.col, (Obj)aent.body);
				GetNextIent( &ac, &aent, &ai);
			} else if ( bent.cr == -1 ) {
				AppendIent( m, aent.row, aent.col, (Obj)aent.body);
				GetNextIent( &ac, &aent, &ai);
			} else {
				AppendIent( m, bent.row, bent.col, (Obj)bent.body);
				GetNextIent( &bc, &bent, &bi);
				if ( bent.cr == -1 ){
					while(aent.cr != -1){
						AppendIent(m, aent.row, aent.col, (Obj)aent.body);
						GetNextIent( &ac, &aent, &ai );
					}
					break;
				}
			}
		}
		while(bent.cr != -1) {
			AppendIent( m, bent.row, bent.col, (Obj)bent.body);
			GetNextIent( &bc, &bent, &bi );
		}
		*c = m;
	}
	return;
}

void SubMatI(vl, a, b, c)
VL vl;
IMAT a, b, *c;
{
	/* subtract index type matrix */
	int ai, bi;
	IMAT m;
	IMATC ac, bc;
	IENT aent, bent;
	Obj obj;

	if ( a->col != b->col ) {
		error("submati : colum size mismatch");
		*c = 0;
	} else if ( a->row != b->row ) {
		error("submati : row size mismatch");
		*c = 0;
	} else if ( ! b->root ) {
		*c = a;
	} else if ( ! a->root ) {
		ChsgnI(b, &m);
		*c = m;
	} else {
		vl = CO;
		NEWIMAT(m);
		m->col = a->col;
		m->row = a->row;
		ai = -1;
		ac = (pointer)a->root;
		bi = -1;
		bc = (pointer)b->root;

		GetNextIent(&ac, &aent, &ai);
		GetNextIent(&bc, &bent, &bi);
		while(aent.cr != -1) {
			if( aent.cr == bent.cr ) {
				arf_sub(vl, (Obj)aent.body, (Obj)bent.body, (Obj *)&obj);
				AppendIent(m, aent.row, aent.col, obj);
				GetNextIent(&ac, &aent, &ai);
				GetNextIent(&bc, &bent, &bi);
			} else if ( aent.cr < bent.cr ) {
				AppendIent( m, aent.row, aent.col, (Obj)aent.body);
				GetNextIent(&ac, &aent, &ai);
			} else if ( bent.cr == -1 ) {
				AppendIent( m, aent.row, aent.col, (Obj)aent.body);
				GetNextIent(&ac, &aent, &ai);
			} else {
				arf_chsgn((Obj)bent.body, (Obj *)&obj);
				AppendIent( m, bent.row, bent.col, (Obj)obj);
				GetNextIent(&bc, &bent, &bi);
				if (bent.cr == -1){
					while(aent.cr != -1){
						AppendIent(m, aent.row, aent.col, (Obj)aent.body);
						GetNextIent(&ac, &aent, &ai);
					}
					break;
				}
			}
		}
		while(bent.cr != -1) {
			arf_chsgn((Obj)bent.body, (Obj *)&obj);
			AppendIent( m, bent.row, bent.col, (Obj)obj);
			GetNextIent(&bc, &bent, &bi);
		}
		*c = m;
	}
}

void MulrMatI(vl, a, b, rp)
VL vl;
Obj a, b, *rp;
{
	/* multiply a expression and a index type matrix */
	IMAT m;
	IENT ent;
	IMATC Im;
	Obj p;
	int ia;

	NEWIMAT(m);
	m->col = ((IMAT)b)->col;
	m->row = ((IMAT)b)->row;
	Im = (IMATC)(((IMAT)b)->root);
	ia = -1;
	GetNextIent(&Im, &ent, &ia);
	while(ent.cr != -1) {
		arf_mul(vl, (Obj)a, (Obj)ent.body, (Obj *)&p);
		AppendIent(m, ent.row, ent.col, (Obj)p);
		GetNextIent(&Im, &ent, &ia);
	}
	*rp = (Obj)m;
}

void MulMatG(vl, a, b, c)
VL vl;
Obj a, b, *c;
{
	/* multiply index type matrix general procedure */
	IMAT m;

	if ( !a ) {
		NEWIMAT(m);
		m->col = ((IMAT)b)->col;
		m->row = ((IMAT)b)->row;
		*c = (Obj)m;
	} else if ( !b ) {
		NEWIMAT(m);
		m->col = ((IMAT)a)->col;
		m->row = ((IMAT)a)->row;
		*c = (Obj)m;
	} else if ( OID(a) <= O_R ) MulrMatI(vl, (Obj)a, (Obj)b, (Obj *)c);
	else if ( OID(b) <= O_R ) MulrMatI(vl, (Obj)b, (Obj)a, (Obj *)c);
	else MulMatS(vl, (IMAT)a, (IMAT)b, (IMAT *)c);
}

void MulMatS(vl, m,n,rp)
VL vl;
IMAT m, n, *rp;
{
	/* multiply index type matrix and index type matrix */
	IMAT r, a11, a12, a21, a22, b11, b12, b21, b22;
	IMAT ans1, ans2, c11, c12, c21, c22, s1, s2, t1, t2, u1, v1, w1;
	pointer u, v, *ab;
	int hmcol, hmrow, hncol, hnrow;
	IENT ent, entn, entm;
	IMATC in,im;
	int *c, ai, bi, cr, row, col, ca11,ca12,ca21,ca22;
	if ( m->col != n->row ) {
		*rp =0; error("mulmati : size mismatch");
	}
	vl = CO;
	NEWIMAT(r);
	r->row = m->row;
	r->col = n->col;
	if( m->clen == 0 || n->clen == 0){
	/* zero matrix case */
		*rp = (pointer)r;
		return;
	} else if(StrassenSize == 0) {
	/* not use Strassen algorithm */
		MulMatI(vl, m, n, &r);
		*rp = (pointer)r;
		return;
#if 0
	} else if(m->row == 1){
		ab = (pointer)MALLOC((n->col +1)*sizeof(Obj));
		ai = -1;
		im = (pointer)m->root;
		GetNextIent(&im, &entm, &ai);
		bi = -1;
		in = (pointer)n->root;
		GetNextIent(&in, &entn, &bi);
		while( entn.cr != -1 ){
			if( entn.row == entm.col ){
				arf_mul(vl,(Obj)entm.body,(Obj)entm.body,(Obj *)&u);
				arf_add(vl,(Obj)u,(Obj)ab[entn.col],(Obj *)&v);
				ab[entn.col] = (pointer)v;
				GetNextIent(&in, &entn, &bi);
			} else if( entn.row < entm.col ) {
				GetNextIent(&in, &entn, &bi);
			} else {
				GetNextIent(&im, &entm, &ai);
			}
		}
		for(ai=0; ai< m->col; ai++){
			if(ab[ai] != 0) AppendIent(r, 1, ai, (Obj)&ab[ai]);
		}
		*rp=r;
		return;
	} else if(n->col == 1){
	/* not yet */
		*rp=0;
		return;
	} else if(m->col == 1){
		ai = -1;
		im = (pointer)m->root;
		GetNextIent(&im, &entm, &ai);
		while( entm.cr != -1 ){
			bi = -1;
			in = (pointer)n->root;
			GetNextIent(&in, &entn, &bi);
			while( entn.cr != -1 ){
				arf_mul(vl,(Obj)entm.body,(Obj)entn.body,(Obj *)&u);
				AppendIent(r, entm.row, entn.col, (Obj)&u);
				GetNextIent(&in, &entn, &bi);
			}
			GetNextIent(&im, &entm, &ai);
		}
		*rp = r;
		return;
#endif
	} else if((m->row<=StrassenSize)||(m->col<=StrassenSize)|| (n->col<=StrassenSize)) {
	/* not use Strassen algorithm */
		MulMatI(vl, m, n, &r);
		*rp = (pointer)r;
		return;
	}
	/* Strassen Algorithm */
	/* calcrate matrix half size */
	hmrow = (m->row + (m->row & 1)) >> 1;
	hmcol = (m->col + (m->col & 1)) >> 1;
	hnrow = (n->row + (n->row & 1)) >> 1;
	hncol = (n->col + (n->col & 1)) >> 1;
	NEWIMAT(a11); a11->col = hmcol; a11->row = hmrow;
	NEWIMAT(a12); a12->col = hmcol; a12->row = hmrow;
	NEWIMAT(a21); a21->col = hmcol; a21->row = hmrow;
	NEWIMAT(a22); a22->col = hmcol; a22->row = hmrow;
	NEWIMAT(b11); b11->col = hncol; b11->row = hnrow;
	NEWIMAT(b12); b12->col = hncol; b12->row = hnrow;
	NEWIMAT(b21); b21->col = hncol; b21->row = hnrow;
	NEWIMAT(b22); b22->col = hncol; b22->row = hnrow;

	/* copy matrix n to 4 small matrix a11,a12,a21,a22 */
	im = (pointer)m->root;
	ai = -1;
	GetNextIent(&im, &ent, &ai);
	while( ent.cr != -1 ){
		if(ent.col < hmcol){
			if(ent.row < hmrow){
				AppendIent(a11,ent.row,ent.col,(Obj)ent.body);
			} else {
				AppendIent(a21,ent.row-hmrow,ent.col,(Obj)ent.body);
			}
		} else {
			if(ent.row < hmrow){
				AppendIent(a12,ent.row,ent.col-hmcol,(Obj)ent.body);
			} else {
				AppendIent(a22,ent.row-hmrow,ent.col-hmcol,(Obj)ent.body);
			}
		}
		GetNextIent(&im, &ent, &ai);
	}
	/* copy matrix m to 4 small matrix b11,b12,b21,b22 */
	im = (pointer)n->root;
	ai = -1;
	GetNextIent(&im, &ent, &ai);
	while( ent.cr != -1 ){
		if(ent.col < hmcol){
			if(ent.row < hnrow){
				AppendIent(b11,ent.row,ent.col,(Obj)ent.body);
			} else {
				AppendIent(b21,ent.row-hnrow,ent.col,(Obj)ent.body);
			}
		} else {
			if(ent.row < hnrow){
				AppendIent(b12,ent.row,ent.col-hncol,(Obj)ent.body);
			} else {
				AppendIent(b22,ent.row-hnrow,ent.col-hncol,(Obj)ent.body);
			}
		}
		GetNextIent(&im, &ent, &ai);
	}
	/* expand matrix by Strassen-Winograd algorithm */
	/* s1 = A21 + A22 */
	AddMatI(vl,a21,a22, &s1);

	/* s2=s1-A11 */
	SubMatI(vl,s1,a11,&s2);

	/* t1=B12-B11 */
	SubMatI(vl,b12,b11,&t1);

	/* t2=B22-t1 */
	SubMatI(vl,b22,t1,&t2);

	/* u=(A11-A21)*(B22-B12) */
	SubMatI(vl,a11,a21,&ans1);
	SubMatI(vl,b22,b12,&ans2);
	MulMatS(vl, ans1,ans2,&u1);

	/* v=s1*t1 */
	MulMatS(vl, s1,t1,&v1);

	/* w=A11*B11+s2*t2 */
	MulMatS(vl, a11,b11,&ans1);
	MulMatS(vl, s2,t2,&ans2);
	AddMatI(vl,ans1,ans2,&w1);

	/* C11 = A11*B11+A12*B21 */
	MulMatS(vl, a12,b21,&ans2);
	AddMatI(vl,ans1,ans2,&c11);

	/* C12 = w1+v1+(A12-s2)*B22 */
	SubMatI(vl,a12,s2,&ans1);
	MulMatS(vl, ans1, b22, &ans2);
	AddMatI(vl, w1, v1, &ans1);
	AddMatI(vl, ans1, ans2, &c12);

	/* C21 = w1+u1+A22*(B21-t2) */
	SubMatI(vl, b21, t2, &ans1);
	MulMatS(vl, a22, ans1, &ans2);
	AddMatI(vl, w1, u1, &ans1);
	AddMatI(vl, ans1, ans2, &c21);

	/* C22 = w1 + u1 + v1 */
	AddMatI(vl, ans1, v1, &c22);

	/* create return i matrix */
	r->col = m->col;
	r->row = n->row;

	/* copy c11 to r */
	ca11 = -1;
	if( c11->root ){
		im = (pointer)c11->root;
		ai = -1;
		GetNextIent(&im, &ent, &ai);
		while( ai != -1 ){
			AppendIent(r,ent.row,ent.col,(Obj)ent.body);
			GetNextIent(&im, &ent, &ai);
		}
	}
	/* copy c21 to r */
	if( c21->root ){
		im = (pointer)c21->root;
		ai = -1;
		GetNextIent(&im, &ent, &ai);
		while( ent.cr != -1 ){
			PutIent(r, ent.row + hmrow, ent.col, (Obj)ent.body);
			GetNextIent(&im, &ent, &ai);
		}
	}
	/* copy c12 to r */
	if( c12->root ){
		im = (pointer)c12->root;
		ai = -1;
		GetNextIent(&im, &ent, &ai);
		while( ent.cr != -1 ){
			PutIent(r, ent.row, ent.col + hncol, (Obj)ent.body);
			GetNextIent(&im, &ent, &ai);
		}
	}
	/* copy c22 to r */
	if( c22->root ){
		im = (pointer)c22->root;
		ai = -1;
		GetNextIent(&im, &ent, &ai);
		while( ent.cr != -1 ){
			PutIent(r, ent.row + hmrow, ent.col + hncol, (Obj)ent.body);
			GetNextIent(&im, &ent, &ai);
		}
	}
	*rp = (pointer)r;
	return;
}

void MulMatI(vl,m,n,r)
VL vl;
IMAT m, n, *r;
{
	/* inner prodocut algorithm for index type multiply procedure */
	IMAT l;
	IMATC im, imt;
	IENT ent, tent;
	int bc, bend, ac, aend;
	int col, row, tcol, trow;
	int i, j, k, ai, bi;
	int bj, ik,kj;
	pointer s, u, v;

	typedef struct oIND {
		int row, col;
		pointer body;
	} *IND;
	IND a,b;
	/* make trans(n) */
	a = (IND)MALLOC(IMATCH * m->clen *sizeof(struct oIND)+1);
	b = (IND)MALLOC(IMATCH * n->clen *sizeof(struct oIND)+1);

	bend = -1;
	row = n->row;
	col = n->col;
	bc = -1;
	im = (pointer)n->root;
	GetNextIent(&im, &ent, &bc);
	b[++bend].body = ent.body;
	b[bend].row = ent.col;
	b[bend].col = ent.row;
	GetNextIent(&im, &ent, &bc);
	while ( ent.cr != -1 ) {
		trow = ent.col;
		tcol = ent.row;
		for( i = bend; i >= 0; i--) {
			if( b[i].row > trow ) {
				b[i + 1] = b[i];
			} else if ( b[i].col > tcol ) {
				b[i + 1] = b[i];
			} else {
				b[i+1].col = tcol;
				b[i+1].row = trow;
				b[i+1].body = ent.body;
				break;
			}
		}
		if ( i == -1 ) {
			b[0].col = tcol;
			b[0].row = trow;
			b[0].body = ent.body;
		}
		bend++;
		GetNextIent(&im, &ent, &bc);
	}

	im = (pointer)m->root;
	ac = -1;
	GetNextIent(&im, &ent, &ac);
	aend = -1;
	while( ent.cr != -1 ){
		a[++aend].col = ent.col;
		a[aend].row = ent.row;
		a[aend].body = ent.body;
		GetNextIent(&im, &ent, &ac);
	}
	/* make return matrix */
	NEWIMAT(l);
	l->row = m->row;
	l->col = n->col;
	ac = -1;
	for( i = 0; i<= aend;) {
		ai = a[i].row;
		bj = b[0].row;
		s = 0;
		ik=i;
		for(j=0; j<= bend;){
			if( a[ik].col == b[j].col) {
				arf_mul(CO,(Obj)a[ik].body, (Obj)b[j].body, (Obj *)&u);
				arf_add(CO,(Obj)s,(Obj)u,(Obj *)&v);
				s = v;
				j++;
				if ( bj != b[j].row ) {
					AppendIent(l, ai, bj, (Obj)s);
					ik = i;
					bj = b[j].row;
					s = 0;
				} else {
					ik++;
					if ( ai != a[ik].row ) {
						AppendIent(l, ai, bj, (Obj)s);
						s = 0;
						while ( bj == b[j].row ) {
							j++;
							if ( j > bend ) break;
						}
						ik = i;
						bj = b[j].row;
					}
				}
			} else if ( a[ik].col > b[j].col ) {
				j++;
				if ( bj != b[j].row ) {
					AppendIent(l, ai, bj, (Obj)s);
					s = 0;
					ik = i;
					bj = b[j].row;
				}
			} else /* if ( a[ik].col < b[j].col ) */ {
				ik++;
				if ( ai != a[ik].row ) {
					AppendIent(l, ai, bj, (Obj)s);
					s = 0;
					while ( bj == b[j].row ) {
						j++;
						if ( j > bend ) break;
					}
					bj = b[j].row;
					ik = i;
				}
			}
		}
		i = ik;
		while ( ai == a[i].row ) {
			i++;
			if( i > aend) break;
		}
	}
	*r = (IMAT)l;
	FREE(a);
	FREE(b);
	return;
}
