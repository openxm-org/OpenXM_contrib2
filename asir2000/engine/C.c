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
 * $OpenXM: OpenXM_contrib2/asir2000/engine/C.c,v 1.10 2001/06/29 09:08:53 noro Exp $ 
*/
#include "ca.h"
#include "inline.h"
#include "base.h"

V up_var;

/* binary has at least 32 leading 0 chars. */
void binaryton(char *binary,N *np)
{
	int i,w,len;
	N n;
	char buf[33];

	binary += strlen(binary)%32;
	len = strlen(binary);
	w = len/32; /* sufficient for holding binary */
	n = NALLOC(w);
	for ( i = 0; i < w; i++ ) {
		strncpy(buf,binary+len-32*(i+1),32); buf[32] = 0;
		n->b[i] = strtoul(buf,0,2);
	}
	for ( i = w-1; i >= 0 && !n->b[i]; i-- );
	if ( i < 0 )
		*np = 0;
	else {
		n->p = i+1;
		*np = n;
	}
}

/* hex has at least 8 leading 0 chars. */
void hexton(char *hex,N *np)
{
	int i,w,len;
	N n;
	char buf[9];

	hex += strlen(hex)%8;
	len = strlen(hex);
	w = len/8; /* sufficient for holding hex */
	n = NALLOC(w);
	for ( i = 0; i < w; i++ ) {
		strncpy(buf,hex+len-8*(i+1),8); buf[8] = 0;
		n->b[i] = strtoul(buf,0,16);
	}
	for ( i = w-1; i >= 0 && !n->b[i]; i-- );
	if ( i < 0 )
		*np = 0;
	else {
		n->p = i+1;
		*np = n;
	}
}

void ntobn(int base,N n,N *nrp)
{
	int i,d,plc;
	unsigned int *c,*x,*w;
	unsigned int r;
	L m;
	N nr;

	if ( !n ) {
		*nrp = NULL;
		return;
	}

	d = PL(n);
	w = BD(n);

	for ( i = 1, m = 1; m <= LBASE/(L)base; m *= base, i++ );

	c = (unsigned int *)W_ALLOC(d*i+1);
	x = (unsigned int *)W_ALLOC(d+1);
	for ( i = 0; i < d; i++ ) 
		x[i] = w[i];
	for ( plc = 0; d >= 1; plc++ ) {
		for ( i = d - 1, r = 0; i >= 0; i-- ) {
			DSAB((unsigned int)base,r,x[i],x[i],r)
		}
		c[plc] = r;
		if ( !x[d-1] ) d--;
	}

	*nrp = nr = NALLOC(plc); INITRC(nr);
	PL(nr) = plc;
	for ( i = 0; i < plc; i++ )
		BD(nr)[i] = c[i];
}

void bnton(int base,N n,N *nrp)
{
	unsigned int carry;
	unsigned int *x,*w;
	int i,j,d,plc;
	N nr;

	if ( !n ) {
		*nrp = 0;
		return;
	}

	d = PL(n);
	w = BD(n);
	x = (unsigned int *)W_ALLOC(d + 1);

	for ( plc = 0, i = d - 1; i >= 0; i-- ) {
		for ( carry = w[i],j = 0; j < plc; j++ ) {
			DMA(x[j],(unsigned int)base,carry,carry,x[j])
		}
		if ( carry ) x[plc++] = carry;
	}
	*nrp = nr = NALLOC(plc); INITRC(nr);
	PL(nr) = plc;
	for ( i = 0; i < plc; i++ )
		BD(nr)[i] = x[i];
}

void ptomp(int m,P p,P *pr)
{
	DCP dc,dcr,dcr0;
	Q q;
	unsigned int a,b;
	P t;
	MQ s;

	if ( !p )
		*pr = 0;
	else if ( NUM(p) ) {
		q = (Q)p;
		a = rem(NM(q),m);
		if ( a && (SGN(q) < 0) )
			a = m-a;
		b = !DN(q)?1:rem(DN(q),m);
		if ( !b )
			error("ptomp : denominator = 0");
		a = dmar(a,invm(b,m),0,m); STOMQ(a,s); *pr = (P)s;
	} else {
		for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
			ptomp(m,COEF(dc),&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
			}
		}
		if ( !dcr0 ) 
			*pr = 0;
		else {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		}
	}
}
		
void mptop(P f,P *gp)
{
	DCP dc,dcr,dcr0;
	Q q;

	if ( !f ) 
		*gp = 0;
	else if ( NUM(f) )
		STOQ(CONT((MQ)f),q),*gp = (P)q;
	else {
		for ( dc = DC(f), dcr0 = 0; dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); mptop(COEF(dc),&COEF(dcr));
		}
		NEXT(dcr) = 0; MKP(VR(f),dcr0,*gp);
	}
}

void ptosfp(P p,P *pr)
{
	DCP dc,dcr,dcr0;
	GFS a;
	P t;

	if ( !p )
		*pr = 0;
	else if ( NUM(p) ) {
		qtogfs((Q)p,&a); *pr = (P)a;
	} else {
		for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
			ptosfp(COEF(dc),&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
			}
		}
		if ( !dcr0 ) 
			*pr = 0;
		else {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		}
	}
}

void sfptop(P f,P *gp)
{
	DCP dc,dcr,dcr0;
	Q q;
	MQ fq;

	if ( !f ) 
		*gp = 0;
	else if ( NUM(f) ) {
		gfstomq((GFS)f,&fq);
		STOQ(CONT(fq),q);
		*gp = (P)q;
	} else {
		for ( dc = DC(f), dcr0 = 0; dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); sfptop(COEF(dc),&COEF(dcr));
		}
		NEXT(dcr) = 0; MKP(VR(f),dcr0,*gp);
	}
}

void sf_galois_action(P p,Q e,P *pr)
{
	DCP dc,dcr,dcr0;
	GFS a;
	P t;

	if ( !p )
		*pr = 0;
	else if ( NUM(p) ) {
		gfs_galois_action((GFS)p,e,&a); *pr = (P)a;
	} else {
		for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
			sf_galois_action(COEF(dc),e,&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
			}
		}
		if ( !dcr0 ) 
			*pr = 0;
		else {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		}
	}
}

/* GF(pn)={0,1,a,a^2,...} -> GF(pm)={0,1,b,b^2,..} ; a -> b^k */

void sf_embed(P p,int k,int pm,P *pr)
{
	DCP dc,dcr,dcr0;
	GFS a;
	P t;

	if ( !p )
		*pr = 0;
	else if ( NUM(p) ) {
		gfs_embed((GFS)p,k,pm,&a); *pr = (P)a;
	} else {
		for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
			sf_embed(COEF(dc),k,pm,&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
			}
		}
		if ( !dcr0 ) 
			*pr = 0;
		else {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		}
	}
}

void ptolmp(P p,P *pr)
{
	DCP dc,dcr,dcr0;
	LM a;
	P t;

	if ( !p )
		*pr = 0;
	else if ( NUM(p) ) {
		qtolm((Q)p,&a); *pr = (P)a;
	} else {
		for ( dc = DC(p), dcr0 = 0; dc; dc = NEXT(dc) ) {
			ptolmp(COEF(dc),&t);
			if ( t ) {
				NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
			}
		}
		if ( !dcr0 ) 
			*pr = 0;
		else {
			NEXT(dcr) = 0; MKP(VR(p),dcr0,*pr);
		}
	}
}
		
void lmptop(P f,P *gp)
{
	DCP dc,dcr,dcr0;
	Q q;

	if ( !f ) 
		*gp = 0;
	else if ( NUM(f) ) {
		NTOQ(((LM)f)->body,1,q); *gp = (P)q;
	} else {
		for ( dc = DC(f), dcr0 = 0; dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); lmptop(COEF(dc),&COEF(dcr));
		}
		NEXT(dcr) = 0; MKP(VR(f),dcr0,*gp);
	}
}

void ptoum(int m,P f,UM wf)
{
	unsigned int r;
	int i;
	DCP dc;

	for ( i = UDEG(f); i >= 0; i-- ) 
		COEF(wf)[i] = 0;

	for ( dc = DC(f); dc; dc = NEXT(dc) ) {
		r = rem(NM((Q)COEF(dc)),m);
		if ( r && (SGN((Q)COEF(dc)) < 0) )
			r = m-r;
		COEF(wf)[QTOS(DEG(dc))] = r;
	}
	degum(wf,UDEG(f));
}

void umtop(V v,UM w,P *f)
{
	int *c;
	DCP dc,dc0;
	int i;
	Q q;

	if ( DEG(w) < 0 )
		*f = 0;
	else if ( DEG(w) == 0 ) 
		STOQ(COEF(w)[0],q), *f = (P)q;
	else {
		for ( i = DEG(w), c = COEF(w), dc0 = 0; i >= 0; i-- ) 
			if ( c[i] ) {
				NEXTDC(dc0,dc);
				STOQ(i,DEG(dc));
				STOQ(c[i],q), COEF(dc) = (P)q;
			}
		NEXT(dc) = 0;
		MKP(v,dc0,*f);
	}
}

void ptosfum(P f,UM wf)
{
	GFS c;
	int i;
	DCP dc;

	if ( OID(f) == O_N ) {
		DEG(wf) = 0;
		COEF(wf)[0] = FTOIF(CONT((GFS)f));
		return;
	}

	for ( i = UDEG(f); i >= 0; i-- ) 
		COEF(wf)[i] = 0;

	for ( dc = DC(f); dc; dc = NEXT(dc) ) {
		c = (GFS)COEF(dc);
		if ( c )
			COEF(wf)[QTOS(DEG(dc))] = FTOIF(CONT(c));
	}
	degum(wf,UDEG(f));
}

void sfumtop(V v,UM w,P *f)
{
	int *c;
	DCP dc,dc0;
	int i,t;
	GFS q;

	if ( DEG(w) < 0 )
		*f = 0;
	else if ( DEG(w) == 0 ) {
		t = COEF(w)[0];
		t = IFTOF(t);
		MKGFS(t,q);
		*f = (P)q;
	} else {
		for ( i = DEG(w), c = COEF(w), dc0 = 0; i >= 0; i-- ) 
			if ( c[i] ) {
				NEXTDC(dc0,dc);
				STOQ(i,DEG(dc));
				t = COEF(w)[i];
				t = IFTOF(t);
				MKGFS(t,q);
				COEF(dc) = (P)q;
			}
		NEXT(dc) = 0;
		MKP(v,dc0,*f);
	}
}

void ptoup(P n,UP *nr)
{
	DCP dc;
	UP r;
	int d;

	if ( !n )
		*nr = 0;
	else if ( OID(n) == O_N ) {
		*nr = r = UPALLOC(0);
		DEG(r) = 0; COEF(r)[0] = (Num)n;
	} else {
		d = UDEG(n);
		up_var = VR(n);
		*nr = r = UPALLOC(d); DEG(r) = d;
		for ( dc = DC(n); dc; dc = NEXT(dc) ) {
			COEF(r)[QTOS(DEG(dc))] = (Num)COEF(dc);
		}
	}
}

void uptop(UP n,P *nr)
{
	int i;
	DCP dc0,dc;

	if ( !n )
		*nr = 0;
	else if ( !DEG(n) )
		*nr = (P)COEF(n)[0];
	else {
		for ( i = DEG(n), dc0 = 0; i >= 0; i-- )
			if ( COEF(n)[i] ) {
				NEXTDC(dc0,dc); STOQ(i,DEG(dc)); COEF(dc) = (P)COEF(n)[i];
			}
		if ( !up_var )
			up_var = CO->v;
		MKP(up_var,dc0,*nr);
	}
}

void ulmptoum(int m,UP f,UM wf)
{
	int i,d;
	LM *c;

	if ( !f )
		wf->d = -1;
	else {
		wf->d = d = f->d;
		c = (LM *)f->c;
		for ( i = 0, d = f->d; i <= d; i++ )
			COEF(wf)[i] = rem(c[i]->body,m);
	}
}

void objtobobj(int base,Obj p,Obj *rp)
{
	if ( !p )
		*rp = 0;
	else 
		switch ( OID(p) ) {
			case O_N:
				numtobnum(base,(Num)p,(Num *)rp); break;
			case O_P:
				ptobp(base,(P)p,(P *)rp); break;
			case O_LIST:
				listtoblist(base,(LIST)p,(LIST *)rp); break;
			case O_VECT:
				vecttobvect(base,(VECT)p,(VECT *)rp); break;
			case O_MAT:
				mattobmat(base,(MAT)p,(MAT *)rp); break;
			case O_STR:
				*rp = p; break;
			case O_COMP: default:
				error("objtobobj : not implemented"); break;
		}
}

void bobjtoobj(int base,Obj p,Obj *rp)
{
	if ( !p )
		*rp = 0;
	else 
		switch ( OID(p) ) {
			case O_N:
				bnumtonum(base,(Num)p,(Num *)rp); break;
			case O_P:
				bptop(base,(P)p,(P *)rp); break;
			case O_LIST:
				blisttolist(base,(LIST)p,(LIST *)rp); break;
			case O_VECT:
				bvecttovect(base,(VECT)p,(VECT *)rp); break;
			case O_MAT:
				bmattomat(base,(MAT)p,(MAT *)rp); break;
			case O_STR:
				*rp = p; break;
			case O_COMP: default:
				error("bobjtoobj : not implemented"); break;
		}
}

void numtobnum(int base,Num p,Num *rp)
{
	N nm,dn,body;
	Q q;
	LM l;

	if ( !p )
		*rp = 0;
	else
		switch ( NID(p) ) {
			case N_Q:
				ntobn(base,NM((Q)p),&nm);
				if ( DN((Q)p) ) {
					ntobn(base,DN((Q)p),&dn);
					NDTOQ(nm,dn,SGN((Q)p),q);
				} else
					NTOQ(nm,SGN((Q)p),q);
				*rp = (Num)q;
				break;
			case N_R:
				*rp = p; break;
			case N_LM:
				ntobn(base,((LM)p)->body,&body);
				MKLM(body,l); *rp = (Num)l;
				break;
			default:
				error("numtobnum : not implemented"); break;
		}
}

void bnumtonum(int base,Num p,Num *rp)
{
	N nm,dn,body;
	Q q;
	LM l;

	if ( !p )
		*rp = 0;
	else
		switch ( NID(p) ) {
			case N_Q:
				bnton(base,NM((Q)p),&nm);
				if ( DN((Q)p) ) {
					bnton(base,DN((Q)p),&dn);
					NDTOQ(nm,dn,SGN((Q)p),q);
				} else
					NTOQ(nm,SGN((Q)p),q);
				*rp = (Num)q;
				break;
			case N_R:
				*rp = p; break;
			case N_LM:
				bnton(base,((LM)p)->body,&body);
				MKLM(body,l); *rp = (Num)l;
				break;
			default:
				error("bnumtonum : not implemented"); break;
		}
}

void ptobp(int base,P p,P *rp)
{
	DCP dcr0,dcr,dc;

	if ( !p )
		*rp = p;
	else {
		for ( dcr0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc);
			objtobobj(base,(Obj)COEF(dc),(Obj *)&COEF(dcr));
		}
		NEXT(dcr) = 0;
		MKP(VR(p),dcr0,*rp);
	}
}

void bptop(int base,P p,P *rp)
{
	DCP dcr0,dcr,dc;

	if ( !p )
		*rp = p;
	else {
		for ( dcr0 = 0, dc = DC(p); dc; dc = NEXT(dc) ) {
			NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc);
			bobjtoobj(base,(Obj)COEF(dc),(Obj *)&COEF(dcr));
		}
		NEXT(dcr) = 0;
		MKP(VR(p),dcr0,*rp);
	}
}

void listtoblist(int base,LIST p,LIST *rp)
{
	NODE nr0,nr,n;

	if ( !p )
		*rp = p;
	else {
		for ( nr0 = 0, n = BDY(p); n; n = NEXT(n) ) {
			NEXTNODE(nr0,nr);
			objtobobj(base,BDY(n),(Obj *)&BDY(nr));
		}
		NEXT(nr) = 0;
		MKLIST(*rp,nr0);
	}
}

void blisttolist(int base,LIST p,LIST *rp)
{
	NODE nr0,nr,n;

	if ( !p )
		*rp = p;
	else {
		for ( nr0 = 0, n = BDY(p); n; n = NEXT(n) ) {
			NEXTNODE(nr0,nr);
			bobjtoobj(base,BDY(n),(Obj *)&BDY(nr));
		}
		NEXT(nr) = 0;
		MKLIST(*rp,nr0);
	}
}

void vecttobvect(int base,VECT p,VECT *rp)
{
	int i,l;
	VECT r;

	if ( !p )
		*rp = p;
	else {
		l = p->len;
		MKVECT(r,l); *rp = r;
		for ( i = 0; i < l; i++ )
			objtobobj(base,p->body[i],(Obj *)&r->body[i]);
	}
}

void bvecttovect(int base,VECT p,VECT *rp)
{
	int i,l;
	VECT r;

	if ( !p )
		*rp = p;
	else {
		l = p->len;
		MKVECT(r,l); *rp = r;
		for ( i = 0; i < l; i++ )
			bobjtoobj(base,p->body[i],(Obj *)&r->body[i]);
	}
}

void mattobmat(int base,MAT p,MAT *rp)
{
	int row,col,i,j;
	MAT r;

	if ( !p )
		*rp = p;
	else {
		row = p->row; col = p->col;
		MKMAT(r,row,col); *rp = r;
		for ( i = 0; i < row; i++ )
			for ( j = 0; i < col; j++ )
			objtobobj(base,p->body[i][j],(Obj *)&r->body[i][j]);
	}
}

void bmattomat(int base,MAT p,MAT *rp)
{
	int row,col,i,j;
	MAT r;

	if ( !p )
		*rp = p;
	else {
		row = p->row; col = p->col;
		MKMAT(r,row,col); *rp = r;
		for ( i = 0; i < row; i++ )
			for ( j = 0; i < col; j++ )
			bobjtoobj(base,p->body[i][j],(Obj *)&r->body[i][j]);
	}
}

void n32ton27(N g,N *rp)
{
	int i,j,k,l,r,bits,words;
	unsigned int t;
	unsigned int *a,*b;
	N z;

	l = PL(g); a = BD(g);
	for ( i = 31, t = a[l-1]; !(t&(1<<i)); i-- );
	bits = (l-1)*32+i+1; words = (bits+26)/27;
	*rp = z = NALLOC(words); PL(z) = words;
	bzero((char *)BD(z),words*sizeof(unsigned int));
	for ( j = 0, b = BD(z); j < words; j++ ) {
		k = (27*j)/32; r = (27*j)%32;
		if ( r > 5 )
			b[j] = (a[k]>>r)|(k==(l-1)?0:((a[k+1]&((1<<(r-5))-1))<<(32-r)));
		else
			b[j] = (a[k]>>r)&((1<<27)-1);
	}
	if ( !(r = bits%27) )
		r = 27;
	b[words-1] &= ((1<<r)-1);
}

void n27ton32(N a,N *rp)
{
	int i,j,k,l,r,bits,words;
	unsigned int t;
	unsigned int *b,*c;
	N z;

	l = PL(a); b = BD(a);
	for ( i = 26, t = b[l-1]; !(t&(1<<i)); i-- );
	bits = (l-1)*27+i+1; words = (bits+31)/32;
	*rp = z = NALLOC(words); PL(z) = words;
	bzero((char *)BD(z),words*sizeof(unsigned int));
	for ( j = 0, c = BD(z); j < l; j++ ) {
		k = (27*j)/32; r = (27*j)%32;
		if ( r > 5 ) {
			c[k] |= (b[j]&((1<<(32-r))-1))<<r;
			if ( k+1 < words )
				c[k+1] = (b[j]>>(32-r));
		} else
			c[k] |= (b[j]<<r);
	}
}

void mptoum(P p,UM pr)
{
	DCP dc;

	if ( !p ) 
		DEG(pr) = -1;
	else if ( NUM(p) ) {
		DEG(pr) = 0; COEF(pr)[0] = CONT((MQ)p);
	} else {
		bzero((char *)pr,(int)((UDEG(p)+2)*sizeof(int)));
		for ( dc = DC(p); dc; dc = NEXT(dc) )
			COEF(pr)[QTOS(DEG(dc))] = CONT((MQ)COEF(dc));
		degum(pr,UDEG(p));
	}
}

void umtomp(V v,UM p,P *pr)
{
	DCP dc,dc0;
	int i;
	MQ q;

	if ( !p || (DEG(p) < 0) ) 
		*pr = 0;
	else if ( !DEG(p) )
		STOMQ(COEF(p)[0],q), *pr = (P)q;
	else {
		for ( dc0 = 0, i = DEG(p); i >= 0; i-- )
			if ( COEF(p)[i] ) {
				NEXTDC(dc0,dc); STOQ(i,DEG(dc));
				STOMQ(COEF(p)[i],q), COEF(dc) = (P)q;
			}
		NEXT(dc) = 0; MKP(v,dc0,*pr);
	}
}

/* f(p) -> f(x) */

void enc_to_p(int p,int a,V v,P *pr)
{
	DCP dc,dct;
	int i,c;
	Q dq,cq;

	dc = 0;
	for ( i = 0; a; i++, a /= p ) {
		c = a%p;
		if ( c ) {
			STOQ(i,dq); STOQ(c,cq);
			NEWDC(dct); DEG(dct) = dq; COEF(dct) = (P)cq;
			NEXT(dct) = dc; dc = dct;
		}
	}
	MKP(v,dc,*pr);
}
