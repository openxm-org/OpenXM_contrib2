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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/mat.c,v 1.8 2003/03/27 02:59:16 noro Exp $ 
*/
#include "ca.h"
#include "../parse/parse.h"

extern int StrassenSize;

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
    *c = 0; error("addmat : size mismatch add"); 
  } else {
    row = a->row; col = a->col;
    MKMAT(t,row,col);
    for ( i = 0; i < row; i++ )
      for ( j = 0, ab = BDY(a)[i], bb = BDY(b)[i], tb = BDY(t)[i]; 
        j < col; j++ )
        arf_add(vl,(Obj)ab[j],(Obj)bb[j],(Obj *)&tb[j]);
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
    *c = 0; error("submat : size mismatch sub");
  } else {
    row = a->row; col = a->col;
    MKMAT(t,row,col);
    for ( i = 0; i < row; i++ )
      for ( j = 0, ab = BDY(a)[i], bb = BDY(b)[i], tb = BDY(t)[i];
        j < col; j++ )
        arf_sub(vl,(Obj)ab[j],(Obj)bb[j],(Obj *)&tb[j]);
    *c = t;
  }
}

void mulmat(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	VECT vect;
	MAT mat;

	if ( !a && !b )
		*c = 0;
	else if ( !a || !b ) {
		if ( !a )
			a = b;	
		switch ( OID(a) ) {
			case O_VECT:
				MKVECT(vect,((VECT)a)->len);
				*c = (Obj)vect;
				break;
			case O_MAT:
				MKMAT(mat,((MAT)a)->row,((MAT)a)->col);
				*c = (Obj)mat;
				break;
			default:
				*c = 0;
				break;
		}
	} else if ( OID(a) <= O_R )
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
		arf_div(vl,(Obj)ONE,b,&t); mulrmat(vl,t,(MAT)a,(MAT *)c);
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
				arf_chsgn((Obj)ab[j],(Obj *)&tb[j]);
		*b = t;
	} 
}

void pwrmat(vl,a,r,c)
VL vl;
MAT a;
Obj r;
MAT *c;
{
	int n,i;
	MAT t;

	if ( !a )
		*c = 0;
	else if ( !r ) {
		if ( a->row != a->col ) {
			*c = 0; error("pwrmat : non square matrix");
		} else {
			n = a->row;
    		MKMAT(t,n,n);
			for ( i = 0; i < n; i++ )
				t->body[i][i] = ONE;
			*c = t;
		}
	} else if ( !NUM(r) || !RATN(r) || 
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
				arf_mul(vl,(Obj)a,(Obj)bb[j],(Obj *)&tb[j]);
		*c = t;
	}
}

void mulmatmat(vl,a,b,c)
VL vl;
MAT a,b,*c;
{
#if 0
	int arow,bcol,i,j,k,m;
	MAT t;
	pointer s,u,v;
	pointer *ab,*tb;

	/* Mismach col and row */
	if ( a->col != b->row ) {
		*c = 0; error("mulmat : size mismatch"); 
	} else {
		arow = a->row; m = a->col; bcol = b->col;
		MKMAt(t,arow,bcol);
		for ( i = 0; i < arow; i++ )
			for ( j = 0, ab = BDY(a)[i], tb = BDY(t)[i]; j < bcol; j++ ) {
				for ( k = 0, s = 0; k < m; k++ ) {
					arf_mul(vl,(Obj)ab[k],(Obj)BDY(b)[k][j],(Obj *)&u);
					arf_add(vl,(Obj)s,(Obj)u,(Obj *)&v);
					s = v;
				}
				tb[j] = s;
			}
		*c = t;
	}
}

void Strassen(arg, c)
NODE arg;
Obj *c;
{
  MAT a,b;
	VL vl;

	/* tomo */
	a = (MAT)ARG0(arg);
	b = (MAT)ARG1(arg);
	vl = CO;
	strassen(CO, a, b, c);
}

void strassen(vl,a,b,c)
VL vl;
MAT a,b,*c;
{
#endif
	int arow,bcol,i,j,k,m, h, arowh, bcolh;
	MAT t, a11, a12, a21, a22;
	MAT p, b11, b12, b21, b22;
	MAT ans1, ans2, ans3, c11, c12, c21, c22;
	MAT s1, s2, t1, t2, u1, v1, w1, aa, bb;
	pointer s,u,v;
	pointer *ab,*tb;
	int a1row,a2row, a3row,a4row, a1col, a2col, a3col, a4col;
	int b1row,b2row, b3row,b4row, b1col, b2col, b3col, b4col;
	int pflag1, pflag2;
	/* mismach col and row */
	if ( a->col != b->row ) {
		*c = 0; error("mulmat : size mismatch");
	}
	else {
		pflag1 = 0; pflag2 = 0;
		arow = a->row; m = a->col; bcol = b->col;
		arowh = arow/2; bcolh = bcol/2;
		MKMAT(t,arow,bcol);
		/* StrassenSize == 0 or matrix size less then StrassenSize,
		then calc cannonical algorithm. */
		if((StrassenSize == 0)||(a->row<=StrassenSize || a->col <= StrassenSize) || (b->row<=StrassenSize || b->col <= StrassenSize)) {
			for ( i = 0; i < arow; i++ )
				for ( j = 0, ab = BDY(a)[i], tb = BDY(t)[i]; j < bcol; j++ ) {
					for ( k = 0, s = 0; k < m; k++ ) {
						arf_mul(vl,(Obj)ab[k],(Obj)BDY(b)[k][j],(Obj *)&u);
						arf_add(vl,(Obj)s,(Obj)u,(Obj *)&v);
						s = v;
					}
					tb[j] = s;
				}
		*c = t;
		return;
		}
		/* padding odd col and row to even number for zero */
		i = arow/2;
		j = arow - i;
		if (i != j) {
			arow++;
			pflag1 = 1;
		}
		i = m/2;
		j = m - i;
		if (i != j) {
			m++;
			pflag2 = 1;
		}
		MKMAT(aa, arow, m);
		for (i = 0; i < a->row; i++) {
			for (j = 0; j < a->col; j++) {
				aa->body[i][j] = a->body[i][j];
			}
		}
		i = bcol/2;
		j = bcol - i;
		if (i != j) {
			bcol++;
		}
		MKMAT(bb, m, bcol);
		for (i = 0; i < b->row; i++) {
			for ( j = 0; j < b->col; j++) {
				bb->body[i][j] = b->body[i][j];
			}
		} 

		/* split matrix A and B */
		a1row = aa->row/2; a1col = aa->col/2;
		MKMAT(a11,a1row,a1col);
    MKMAT(a21,a1row,a1col);
    MKMAT(a12,a1row,a1col);
    MKMAT(a22,a1row,a1col);

		b1row = bb->row/2; b1col = bb->col/2;
		MKMAT(b11,b1row,b1col);
    MKMAT(b21,b1row,b1col);
    MKMAT(b12,b1row,b1col);
    MKMAT(b22,b1row,b1col);

		/* make a11 matrix */
		for (i = 0; i < a1row; i++) {
			for (j = 0; j < a1col; j++) {
				a11->body[i][j] = aa->body[i][j];
			}
		}

		/* make a21 matrix */
		for (i = a1row; i < aa->row; i++) {
			for (j = 0; j < a1col; j++) {
				a21->body[i-a1row][j] = aa->body[i][j];
			}
		}

		/* create a12 matrix */
		for (i = 0; i < a1row; i++) {
			for (j = a1col; j < aa->col; j++) {
				a12->body[i][j-a1col] = aa->body[i][j];
			}
		}

		/* create a22 matrix */
    for (i = a1row; i < aa->row; i++) {
      for (j = a1col; j < aa->col; j++) {
        a22->body[i-a1row][j-a1col] = aa->body[i][j];
      }
   }


		/* create b11 submatrix */
		for (i = 0; i < b1row; i++) {
			for (j = 0; j < b1col; j++) {
				b11->body[i][j] = bb->body[i][j];
			}
		}

		/* create b21 submatrix */
		for (i = b1row; i < bb->row; i++) {
			for (j = 0; j < b1col; j++) {
				b21->body[i-b1row][j] = bb->body[i][j];
			}
		}

		/* create b12 submatrix */
		for (i = 0; i < b1row; i++) {
			for (j = b1col; j < bb->col; j++) {
				b12->body[i][j-b1col] = bb->body[i][j];
			}
		}

		/* create b22 submatrix */
		for (i = b1row; i < bb->row; i++) {
			for (j = b1col; j < bb->col; j++) {
				b22->body[i-b1row][j-b1col] = bb->body[i][j];
			}
		}
		/* expand matrix by Strassen-Winograd algorithm */
		/* s1=A21+A22 */
		addmat(vl,a21,a22,&s1);

		/* s2=s1-A11 */
		submat(vl,s1,a11,&s2);

		/* t1=B12-B11 */
		submat(vl, b12, b11, &t1);

		/* t2=B22-t1 */
		submat(vl, b22, t1, &t2);

		/* u=(A11-A21)*(B22-B12) */
		submat(vl, a11, a21, &ans1);
		submat(vl, b22, b12, &ans2);
		mulmatmat(vl, ans1, ans2, &u1);

		/* v=s1*t1 */
		mulmatmat(vl, s1, t1, &v1);

		/* w=A11*B11+s2*t2 */
		mulmatmat(vl, a11, b11, &ans1);
		mulmatmat(vl, s2, t2, &ans2);
		addmat(vl, ans1, ans2, &w1);

		/* C11 = A11*B11+A12*B21 */
		mulmatmat(vl, a12, b21, &ans2);
		addmat(vl, ans1, ans2, &c11);

		/* C12 = w1+v1+(A12-s2)*B22 */
		submat(vl, a12, s2, &ans1);
		mulmatmat(vl, ans1, b22, &ans2);
		addmat(vl, w1, v1, &ans1);
		addmat(vl, ans1, ans2, &c12);

		/* C21 = w1+u1+A22*(B21-t2) */
		submat(vl, b21, t2, &ans1);
		mulmatmat(vl, a22, ans1, &ans2);
		addmat(vl, w1, u1, &ans1);
		addmat(vl, ans1, ans2, &c21);

		/* C22 = w1 + u1 + v1 */
		addmat(vl, ans1, v1, &c22);
	}

	for(i =0; i<c11->row; i++) {
		for ( j=0; j < c11->col; j++) {
			t->body[i][j] = c11->body[i][j];
		}
	}
	if (pflag1 == 0) {
			k = c21->row;
	} else {
			k = c21->row - 1;
	}
	for(i =0; i<k; i++) {
		for ( j=0; j < c21->col; j++) {
			t->body[i+c11->row][j] = c21->body[i][j];
		}
	}
	if (pflag2 == 0) {
		h = c12->col;
	} else {
		h = c12->col -1;
	}
	for(i =0; i<c12->row; i++) {
		for ( j=0; j < k; j++) {
			t->body[i][j+c11->col] = c12->body[i][j];
		}
	}
	if (pflag1 == 0) {
		k = c22->row;
	} else {
		k = c22->row -1;
	}
	if (pflag2 == 0) {
		h = c22->col;
	} else {
		h = c22->col - 1;
	}
	for(i =0; i<k; i++) {
		for ( j=0; j < h; j++) {
			t->body[i+c11->row][j+c11->col] = c22->body[i][j];
		}
	}
	*c = t;
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
		for ( i = 0; i < b->len; i++ )
			if ( BDY(b)[i] && OID((Obj)BDY(b)[i]) > O_R )
				error("mulmatvect : invalid argument");
		arow = a->row; m = a->col;
		MKVECT(t,arow);
		for ( i = 0; i < arow; i++ ) {
			for ( j = 0, s = 0, ab = BDY(a)[i]; j < m; j++ ) {
				arf_mul(vl,(Obj)ab[j],(Obj)BDY(b)[j],(Obj *)&u); arf_add(vl,(Obj)s,(Obj)u,(Obj *)&v); s = v;
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
		for ( i = 0; i < a->len; i++ )
			if ( BDY(a)[i] && OID((Obj)BDY(a)[i]) > O_R )
				error("mulvectmat : invalid argument");
		bcol = b->col; m = a->len;
		MKVECT(t,bcol);
		for ( j = 0; j < bcol; j++ ) {
			for ( i = 0, s = 0; i < m; i++ ) {
				arf_mul(vl,(Obj)BDY(a)[i],(Obj)BDY(b)[i][j],(Obj *)&u); arf_add(vl,(Obj)s,(Obj)u,(Obj *)&v); s = v;
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
				if ( t = arf_comp(vl,(Obj)BDY(a)[i][j],(Obj)BDY(b)[i][j]) )
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
