/* $OpenXM: OpenXM/src/asir99/engine/mat.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

void addmat(vl,a,b,c)
VL vl;
MAT a,b,*c;
{
	int row,col,i,j;
	MAT t;
	pointer *ab,*bb,*tb;

	if ( !a ) 
		*c = b;
	else if ( !b )
		*c = a;
	else if ( (a->row != b->row) || (a->col != b->col) ) {
		*c = 0; error("addmat : size mismatch"); 
	} else {
		row = a->row; col = a->col;
		MKMAT(t,row,col);
		for ( i = 0; i < row; i++ )
			for ( j = 0, ab = BDY(a)[i], bb = BDY(b)[i], tb = BDY(t)[i]; 
				j < col; j++ )
				addr(vl,(Obj)ab[j],(Obj)bb[j],(Obj *)&tb[j]);
		*c = t;
	}
}

void submat(vl,a,b,c)
VL vl;
MAT a,b,*c;
{
	int row,col,i,j;
	MAT t;
	pointer *ab,*bb,*tb;

	if ( !a ) 
		chsgnmat(b,c);
	else if ( !b )
		*c = a;
	else if ( (a->row != b->row) || (a->col != b->col) ) {
		*c = 0; error("submat : size mismatch"); 
	} else {
		row = a->row; col = a->col;
		MKMAT(t,row,col);
		for ( i = 0; i < row; i++ )
			for ( j = 0, ab = BDY(a)[i], bb = BDY(b)[i], tb = BDY(t)[i]; 
				j < col; j++ )
				subr(vl,(Obj)ab[j],(Obj)bb[j],(Obj *)&tb[j]);
		*c = t;
	}
}

void mulmat(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	if ( !a || !b ) 
		*c = 0;
	else if ( OID(a) <= O_R )
		mulrmat(vl,(Obj)a,(MAT)b,(MAT *)c);
	else if ( OID(b) <= O_R )
		mulrmat(vl,(Obj)b,(MAT)a,(MAT *)c);
	else
		switch ( OID(a) ) {
			case O_VECT:
				switch ( OID(b) ) {
					case O_MAT:
						mulvectmat(vl,(VECT)a,(MAT)b,(VECT *)c); break;
					case O_VECT: default:
						notdef(vl,a,b,c); break;
				}
				break;
			case O_MAT:
				switch ( OID(b) ) {
					case O_VECT:
						mulmatvect(vl,(MAT)a,(VECT)b,(VECT *)c); break;
					case O_MAT:
						mulmatmat(vl,(MAT)a,(MAT)b,(MAT *)c); break;
					default:
						notdef(vl,a,b,c); break;
				}
				break;
			default:
				notdef(vl,a,b,c); break;
		}
}	

void divmat(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	Obj t;

	if ( !b ) 
		error("divmat : division by 0");
	else if ( !a )
		*c = 0;
	else if ( OID(b) > O_R )
		notdef(vl,a,b,c);
	else {
		divr(vl,(Obj)ONE,b,&t); mulrmat(vl,t,(MAT)a,(MAT *)c);
	}
}	

void chsgnmat(a,b)
MAT a,*b;
{
	MAT t;
	int row,col,i,j;
	pointer *ab,*tb;

	if ( !a )
		*b = 0;
	else {
		row = a->row; col = a->col;
		MKMAT(t,row,col);
		for ( i = 0; i < row; i++ )
			for ( j = 0, ab = BDY(a)[i], tb = BDY(t)[i]; 
				j < col; j++ )
				chsgnr((Obj)ab[j],(Obj *)&tb[j]);
		*b = t;
	} 
}

void pwrmat(vl,a,r,c)
VL vl;
MAT a;
Obj r;
MAT *c;
{
	if ( !a )
		*c = 0;
	else if ( !r || !NUM(r) || !RATN(r) || 
		!INT(r) || (SGN((Q)r)<0) || (PL(NM((Q)r))>1) ) {
		*c = 0; error("pwrmat : invalid exponent");
	} else if ( a->row != a->col ) {
		*c = 0; error("pwrmat : non square matrix");
	}  else
		pwrmatmain(vl,a,QTOS((Q)r),c);
}

void pwrmatmain(vl,a,e,c)
VL vl;
MAT a;
int e;
MAT *c;
{
	MAT t,s;

	if ( e == 1 ) {
		*c = a;
		return;
	}

	pwrmatmain(vl,a,e/2,&t);
	mulmat(vl,(Obj)t,(Obj)t,(Obj *)&s);
	if ( e % 2 ) 
		mulmat(vl,(Obj)s,(Obj)a,(Obj *)c);
	else
		*c = s;
}

void mulrmat(vl,a,b,c)
VL vl;
Obj a;
MAT b,*c;
{
	int row,col,i,j;
	MAT t;
	pointer *bb,*tb;

	if ( !a || !b ) 
		*c = 0;
	else {
		row = b->row; col = b->col;
		MKMAT(t,row,col);
		for ( i = 0; i < row; i++ )
			for ( j = 0, bb = BDY(b)[i], tb = BDY(t)[i]; 
				j < col; j++ )
				mulr(vl,(Obj)a,(Obj)bb[j],(Obj *)&tb[j]);
		*c = t;
	}
}

void mulmatmat(vl,a,b,c)
VL vl;
MAT a,b,*c;
{
	int arow,bcol,i,j,k,m;
	MAT t;
	pointer s,u,v;
	pointer *ab,*tb;

	if ( a->col != b->row ) {
		*c = 0; error("mulmat : size mismatch"); 
	} else {
		arow = a->row; m = a->col; bcol = b->col;
		MKMAT(t,arow,bcol);
		for ( i = 0; i < arow; i++ )
			for ( j = 0, ab = BDY(a)[i], tb = BDY(t)[i]; j < bcol; j++ ) {
				for ( k = 0, s = 0; k < m; k++ ) {
					mulr(vl,(Obj)ab[k],(Obj)BDY(b)[k][j],(Obj *)&u); addr(vl,(Obj)s,(Obj)u,(Obj *)&v); s = v;
				}
				tb[j] = s;
			}
		*c = t;
	}
}

void mulmatvect(vl,a,b,c)
VL vl;
MAT a;
VECT b;
VECT *c;
{
	int arow,i,j,m;
	VECT t;
	pointer s,u,v;
	pointer *ab;

	if ( !a || !b ) 
		*c = 0;
	else if ( a->col != b->len ) {
		*c = 0; error("mulmatvect : size mismatch");
	} else {
		arow = a->row; m = a->col;
		MKVECT(t,arow);
		for ( i = 0; i < arow; i++ ) {
			for ( j = 0, s = 0, ab = BDY(a)[i]; j < m; j++ ) {
				mulr(vl,(Obj)ab[j],(Obj)BDY(b)[j],(Obj *)&u); addr(vl,(Obj)s,(Obj)u,(Obj *)&v); s = v;
			}
			BDY(t)[i] = s;
		}		
		*c = t;
	}
}

void mulvectmat(vl,a,b,c)
VL vl;
VECT a;
MAT b;
VECT *c;
{
	int bcol,i,j,m;
	VECT t;
	pointer s,u,v;

	if ( !a || !b ) 
		*c = 0;
	else if ( a->len != b->row ) {
		*c = 0; error("mulvectmat : size mismatch");
	} else {
		bcol = b->col; m = a->len;
		MKVECT(t,bcol);
		for ( j = 0; j < bcol; j++ ) {
			for ( i = 0, s = 0; i < m; i++ ) {
				mulr(vl,(Obj)BDY(a)[i],(Obj)BDY(b)[i][j],(Obj *)&u); addr(vl,(Obj)s,(Obj)u,(Obj *)&v); s = v;
			}
			BDY(t)[j] = s;
		}
		*c = t;
	}
}

int compmat(vl,a,b)
VL vl;
MAT a,b;
{
	int i,j,t,row,col;

	if ( !a )
		return b?-1:0;
	else if ( !b )
		return 1;
	else if ( a->row != b->row )
		return a->row>b->row ? 1 : -1;
	else if (a->col != b->col )
		return a->col > b->col ? 1 : -1;
	else {
		row = a->row; col = a->col;
		for ( i = 0; i < row; i++ )
			for ( j = 0; j < col; j++ )
				if ( t = compr(vl,(Obj)BDY(a)[i][j],(Obj)BDY(b)[i][j]) )
					return t;
		return 0;
	}
}

pointer **almat_pointer(n,m)
int n,m;
{
	pointer **mat;
	int i;

	mat = (pointer **)MALLOC(n*sizeof(pointer *));
	for ( i = 0; i < n; i++ )
		mat[i] = (pointer *)CALLOC(m,sizeof(pointer));
	return mat;
}
