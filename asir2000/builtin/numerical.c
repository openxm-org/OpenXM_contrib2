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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/numerical.c,v 1.3 2000/08/22 05:03:59 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

#if defined(LAPACK)
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
