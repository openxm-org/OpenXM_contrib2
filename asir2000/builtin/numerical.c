/* $OpenXM: OpenXM/src/asir99/builtin/numerical.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
#include "ca.h"
#include "parse.h"

#if LAPACK
#include <f2c.h>

void Pflinsolve();

struct ftab numerical_tab[] = {
	{"flinsolve",Pflinsolve,2},
	{0,0,0},
};

void Pflinsolve(arg,rp)
NODE arg;
VECT *rp;
{
	MAT mat;
	VECT b,r;
	int i,j;
	integer n,nrhs,lda,ldb,info;
	integer *ipiv;
	doublereal *m_array,*b_array;
	Num **mb;
	Num *vb;
	Real *rb;
	char trans;

	mat=(MAT)ARG0(arg); b=(VECT)ARG1(arg);
	n=mat->row;
	if ( n != mat->col )
		error("flinsolve : non-square matrix");
	if ( n != b->len )
		error("flinsolve : inconsistent rhs");
	m_array = (doublereal *)MALLOC_ATOMIC(n*n*sizeof(double));
	b_array = (doublereal *)MALLOC_ATOMIC(n*sizeof(double));
	ipiv = (integer *)MALLOC_ATOMIC(n*sizeof(integer));
	for ( i = 0, mb = (Num **)mat->body; i < n; i++ )
		for ( j = 0; j < n; j++ )
			m_array[i*n+j] = ToReal(mb[j][i]);
	for ( i = 0, vb = (Num *)b->body; i < n; i++ )
			b_array[i] = ToReal(vb[i]);
	trans = 'N';
	nrhs = 1;
	lda = n;
	ldb = n;
	dgetrf_(&n,&n,m_array,&lda,ipiv,&info);
	if ( info )
		error("flinsolve : LU factorization failed");
	dgetrs_(&trans,&n,&nrhs,m_array,&lda,ipiv,b_array,&ldb,&info);
	if ( info )
		error("flinsolve : solving failed");
	MKVECT(r,n);
	for ( i = 0, rb = (Real *)r->body; i < n; i++ )
		MKReal(b_array[i],rb[i]);
	*rp = r;		
}
#endif
