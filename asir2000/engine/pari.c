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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/pari.c,v 1.6 2003/02/14 22:29:09 ohara Exp $ 
*/
#include "ca.h"
#if defined(PARI)
#include "base.h"
#include <math.h>
#include "genpari.h"

void patori(GEN,Obj *);
void patori_i(GEN,N *);
void ritopa(Obj,GEN *);
void ritopa_i(N,int,GEN *);

extern long prec;
extern int paristack;

void risa_pari_init() {
	pari_init(paristack,2);
	prec = 4;
}

void create_pari_variable(index)
int index;
{
	static int max_varn;
	int i;
	char name[BUFSIZ];

	if ( index > max_varn ) {
		for ( i = max_varn+1; i <= index; i++ ) {
			sprintf(name,"x%d",i);
#if (PARI_VERSION_CODE < 131594)
			fetch_named_var(name,0);
#else
			fetch_named_var(name);
#endif
		}
		max_varn = index;
	}
}

int get_lg(a)
GEN a;
{
	return lg(a);
}

void ritopa(a,rp)
Obj a;
GEN *rp;
{
	long ltop;
	GEN pnm,z,w;
	DCP dc;
	int i,j,l,row,col;
	VL vl;
	V v;

	if ( !a ) {
		*rp = gzero; return;
	}
	switch ( OID(a) ) {
		case O_N:
			switch ( NID(a) ) {
				case N_Q:
					ltop = avma; ritopa_i(NM((Q)a),SGN((Q)a),&pnm);
					if ( INT((Q)a) )
						*rp = pnm;
					else {
						*rp = z = cgetg(3,4); z[1] = (long)pnm;
						ritopa_i(DN((Q)a),1,(GEN *)&z[2]);
					}
					break;
				case N_R:
					*rp = dbltor(BDY((Real)a)); break;
				case N_B:
					*rp = gcopy((GEN)BDY(((BF)a))); break;
				case N_C:
					z = cgetg(3,6);
					ritopa((Obj)((C)a)->r,(GEN *)&z[1]); ritopa((Obj)((C)a)->i,(GEN *)&z[2]);
					*rp = z;
					break;
				default:
					error("ritopa : not implemented"); break;
			}
			break;
		case O_P:
			l = UDEG((P)a); *rp = z = cgetg(l+3,10);
			setsigne(z,1);
			for ( i = 0, vl = CO, v = VR((P)a); vl->v != v;
				vl = NEXT(vl), i++ );
			create_pari_variable(i);
			setvarn(z,i);
			setlgef(z,l+3);
			for ( i = l+2; i >= 2; i-- )
				z[i] = (long)gzero;
			for ( dc = DC((P)a); dc; dc = NEXT(dc) )
				ritopa((Obj)COEF(dc),(GEN *)&z[QTOS(DEG(dc))+2]);
			break;
		case O_VECT:
			l = ((VECT)a)->len; z = cgetg(l+1,17);
			for ( i = 0; i < l; i++ )
				ritopa((Obj)BDY((VECT)a)[i],(GEN *)&z[i+1]);
			*rp = z;
			break;
		case O_MAT:
			row = ((MAT)a)->row; col = ((MAT)a)->col; z = cgetg(col+1,19);
			for ( j = 0; j < col; j++ ) {
				w = cgetg(row+1,18);
				for ( i = 0; i < row; i++ )
					ritopa((Obj)BDY((MAT)a)[i][j],(GEN *)&w[i+1]);
				z[j+1] = (long)w;
			}
			*rp = z;
			break;
		default:
			error("ritopa : not implemented"); break;
	}
}

void patori(a,rp)
GEN a;
Obj *rp;
{
	Q q,qnm,qdn;
	BF r;
	C c;
	VECT v;
	MAT m;
	N n,nm,dn;
	DCP dc0,dc;
	P t;
	int s,i,j,l,row,col;
	GEN b;
	VL vl;

	if ( gcmp0(a) )
		*rp = 0;
	else { 
		switch ( typ(a) ) {
			case 1: /* integer */
				patori_i(a,&n); NTOQ(n,(char)signe(a),q); *rp = (Obj)q;
				break;
			case 2: /* real */
				NEWBF(r,lg(a)); bcopy((char *)a,(char *)BDY(r),lg(a)*sizeof(long));
				*rp = (Obj)r;
				break;
			case 4: /* rational; reduced */
				patori_i((GEN)a[1],&nm); patori_i((GEN)a[2],&dn);
				s = signe(a[1])*signe(a[2]);
				if ( UNIN(dn) )
					NTOQ(nm,s,q);
				else
					NDTOQ(nm,dn,s,q);
				*rp = (Obj)q;
				break;
			case 5: /* rational; not necessarily reduced */
				patori_i((GEN)a[1],&nm); patori_i((GEN)a[2],&dn);
				s = signe(a[1])*signe(a[2]);
				NTOQ(nm,s,qnm); NTOQ(dn,1,qdn); divq(qnm,qdn,(Q *)rp);
				break;
			case 6: /* complex */
				if ( gcmp0((GEN)a[2]) )
					patori((GEN)a[1],rp);
				else {
					NEWC(c); patori((GEN)a[1],(Obj *)&c->r); patori((GEN)a[2],(Obj *)&c->i);
					*rp = (Obj)c;
				}
				break;
			case 10: /* polynomial */
				for ( i = lgef(a)-1, dc0 = 0; i >= 2; i-- )
					if ( !gcmp0((GEN)a[i]) ) {
						NEXTDC(dc0,dc); 
						patori((GEN)a[i],(Obj *)&COEF(dc)); STOQ(i-2,DEG(dc));
					}
				if ( !dc0 )
					*rp = 0;
				else {
					/* assuming that CO has not changed. */
					for ( s = varn(a), i = 0, vl = CO; i != s;
						i++, vl = NEXT(vl) );
					NEXT(dc) = 0; MKP(vl->v,dc0,t); *rp = (Obj)t;
				}
				break;
			case 17: /* row vector */
			case 18: /* column vector */
				l = lg(a)-1; MKVECT(v,l);
				for ( i = 0; i < l; i++ )
					patori((GEN)a[i+1],(Obj *)&BDY(v)[i]);
				*rp = (Obj)v;
				break;
			case 19: /* matrix */
				col = lg(a)-1; row = lg(a[1])-1; MKMAT(m,row,col);
				for ( j = 0; j < col; j++ )
					for ( i = 0, b = (GEN)a[j+1]; i < row; i++ )
						patori((GEN)b[i+1],(Obj *)&BDY(m)[i][j]);
				*rp = (Obj)m;
				break;
			default:
				error("patori : not implemented");
				break;
		}
	}
}

#if defined(LONG_IS_32BIT)
void ritopa_i(a,s,rp)
N a;
int s;
GEN *rp;
{
	int j,l;
	unsigned int *b;
	GEN z;

	l = PL(a); b = (unsigned int *)BD(a);
	z = cgeti(l+2);
	bzero((char *)&z[2],l*sizeof(int));
	for ( j = 0; j < l; j++ )
		z[l-j+1] = b[j];
	s = s>0?1:-1;
	setsigne(z,s);
	setlgefint(z,lg(z));
	*rp = z;
}

void patori_i(g,rp)
GEN g;
N *rp;
{
	int j,l;
	unsigned int *a,*b;
	N z;

	l = lgef(g)-2;
	a = (unsigned int *)g;
	*rp = z = NALLOC(l); PL(z) = l;
	for ( j = 0, b = (unsigned int *)BD(z); j < l; j++ )
		b[l-j-1] = ((unsigned int *)g)[j+2];
}
#endif

#if defined(LONG_IS_64BIT)
void ritopa_i(a,s,rp)
N a;
int s;
GEN *rp;
{
	int j,l,words;
	unsigned int *b;
	GEN z;

	l = PL(a); b = BD(a);
	words = (l+1)/2;
	z = cgeti(words+2);
	bzero((char *)&z[2],words*sizeof(long));
	for ( j = 0; j < words; j++ )
		z[words-j+1] = ((unsigned long)b[2*j])
			|(((unsigned long)(2*j+1<l?b[2*j+1]:0))<<32);
	s = s>0?1:-1;
	setsigne(z,s);
	setlgefint(z,lg(z));
	*rp = z;
}

void patori_i(g,rp)
GEN g;
N *rp;
{
	int j,l,words;
	unsigned long t;
	unsigned long *a;
	unsigned int *b;
	N z;

	words = lgef(g)-2;
	l = 2*words;
	a = (unsigned long *)g;
	*rp = z = NALLOC(l); PL(z) = l;
	for ( j = 0, b = BD(z); j < words; j++ ) {
		t = a[words+1-j];
		b[2*j] = t&0xffffffff;
		b[2*j+1] = t>>32;
	}
	PL(z) = b[l-1] ? l : l-1;
}
#endif

void strtobf(s,rp)
char *s;
BF *rp;
{
	GEN z;

	z = lisexpr(s); patori(z,(Obj *)rp); cgiv(z);
}
#endif
