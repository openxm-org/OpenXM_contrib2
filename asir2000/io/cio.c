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
 * e-mail at risa-admin@flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/io/cio.c,v 1.2 2000/03/28 06:32:22 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"

#define ISIZ sizeof(int)

int write_cmo_zz(FILE *,int,N);
int read_cmo_zz(FILE *,int *,N *);

int valid_as_cmo(obj)
Obj obj;
{
	NODE m;

	if ( !obj )
		return 1;
	switch ( OID(obj) ) {
		case O_MATHCAP: case O_P: case O_R: case O_DP: case O_STR:
		case O_ERR: case O_USINT: case O_VOID:
			return 1;
		case O_N:
			if ( NID((Num)obj) == N_Q || NID((Num)obj) == N_R )
				return 1;
			else
				return 0;
		case O_LIST:
			for ( m = BDY((LIST)obj); m; m = NEXT(m) )
				if ( !valid_as_cmo(BDY(m)) )
					return 0;
			return 1;
		default:
			return 0;
	}
}

write_cmo(s,obj)
FILE *s;
Obj obj;
{
	int r;
	char errmsg[BUFSIZ];

	if ( !obj ) {
		r = CMO_NULL; write_int(s,&r);
		return;
	}
	switch ( OID(obj) ) {
		case O_N:
			switch ( NID((Num)obj) ) {
				case N_Q:
					write_cmo_q(s,(Q)obj);
					break;
				case N_R:
					write_cmo_real(s,(Real)obj);
					break;
				default:
					sprintf(errmsg, "write_cmo : number id=%d not implemented.",
						NID((Num)obj));
					error(errmsg);
					break;
			}
			break;
		case O_P:
			write_cmo_p(s,obj);
			break;
		case O_R:
			write_cmo_r(s,obj);
			break;
		case O_DP:
			write_cmo_dp(s,obj);
			break;
		case O_LIST:
			write_cmo_list(s,obj);
			break;
		case O_STR:
			write_cmo_string(s,obj);
			break;
		case O_USINT:
			write_cmo_uint(s,obj);
			break;
		case O_MATHCAP:
			write_cmo_mathcap(s,obj);
			break;
		case O_ERR:
			write_cmo_error(s,obj);
			break;
		case O_VOID:
			r = ((USINT)obj)->body; write_int(s,&r);
			break;
		default:
			sprintf(errmsg, "write_cmo : id=%d not implemented.",OID(obj));
			error(errmsg);
			break;
	}
}

write_cmo_mathcap(s,mc)
FILE *s;
MATHCAP mc;
{
	unsigned int r;

	r = CMO_MATHCAP; write_int(s,&r);
	write_cmo(s,BDY(mc));
}

write_cmo_uint(s,ui)
FILE *s;
USINT ui;
{
	unsigned int r;

	r = CMO_INT32; write_int(s,&r);
	r = ui->body; write_int(s,&r);
}

write_cmo_q(s,q)
FILE *s;
Q q;
{
	int r;

	if ( q && DN(q) ) {
		r = CMO_QQ; write_int(s,&r);
		write_cmo_zz(s,SGN(q),NM(q));	
		write_cmo_zz(s,1,DN(q));	
	} else {
		r = CMO_ZZ; write_int(s,&r);
		write_cmo_zz(s,SGN(q),NM(q));	
	}
}

write_cmo_real(s,real)
FILE *s;
Real real;
{
	unsigned int r;
	double dbl;

	r = CMO_IEEE_DOUBLE_FLOAT; write_int(s,&r);
	dbl = real->body; write_double(s,&dbl);
}

write_cmo_zz(s,sgn,n)
FILE *s;
int sgn;
N n;
{
	int i,l,bytes;
	unsigned int t;
	unsigned int *b;
	unsigned char c;

#if 1
	l = PL(n);
	bytes = sgn*l;
	write_int(s,&bytes);
	write_intarray(s,BD(n),l);
#else
	l = PL(n); b = (unsigned int *)BD(n);
	bytes = sgn*4*l;
	write_int(s,&bytes);
	for ( i = l-1; i >= 0; i-- ) {
		t = b[i];
		c = t>>24; write_char(s,&c);
		c = (t>>16)&0xff; write_char(s,&c);
		c = (t>>8)&0xff; write_char(s,&c);
		c = t&0xff; write_char(s,&c);
	}
#endif
}

write_cmo_p(s,p)
FILE *s;
P p;
{
	int r,i;
	VL t,vl;
	char *namestr;
	STRING name;
	NODE n0,n;

	r = CMO_RECURSIVE_POLYNOMIAL; write_int(s,&r);
	get_vars((Obj)p,&vl);

	/* indeterminate list */
	r = CMO_LIST; write_int(s,&r);
	for ( t = vl, i = 0; t; t = NEXT(t), i++ );
	write_int(s,&i);
	r = CMO_INDETERMINATE;
	for ( t = vl; t; t = NEXT(t) ) {
		write_int(s,&r);
/*		localname_to_cmoname(NAME(t->v),&namestr); */
		namestr = NAME(t->v);
		MKSTR(name,namestr);
		write_cmo(s,name);
	}

	/* body */
	write_cmo_upoly(s,vl,p);
}

write_cmo_upoly(s,vl,p)
FILE *s;
VL vl;
P p;
{
	int r,i;
	V v;
	DCP dc,dct;
	VL vlt;

	if ( NUM(p) )
		write_cmo(s,p);
	else {
		r = CMO_UNIVARIATE_POLYNOMIAL; write_int(s,&r);
		v = VR(p);
		dc = DC(p);
		for ( i = 0, dct = dc; dct; dct = NEXT(dct), i++ );
		write_int(s,&i);
		for ( i = 0, vlt = vl; vlt->v != v; vlt = NEXT(vlt), i++ );
		write_int(s,&i);
		for ( dct = dc; dct; dct = NEXT(dct) ) {
			i = QTOS(DEG(dct)); write_int(s,&i);
			write_cmo_upoly(s,vl,COEF(dct));
		}
	}
}

write_cmo_r(s,f)
FILE *s;
R f;
{
	int r;

	r = CMO_RATIONAL; write_int(s,&r);
	write_cmo(s,NM(f));
	write_cmo(s,DN(f));
}

write_cmo_dp(s,dp)
FILE *s;
DP dp;
{
	int i,n,nv,r;
	MP m;

	for ( n = 0, m = BDY(dp); m; m = NEXT(m), n++ );
	r = CMO_DISTRIBUTED_POLYNOMIAL; write_int(s,&r);
	r = n; write_int(s,&r);
	r = CMO_DMS_GENERIC; write_int(s,&r);
	nv = dp->nv;
	for ( i = 0, m = BDY(dp); i < n; i++, m = NEXT(m) )
		write_cmo_monomial(s,m,nv);
}

write_cmo_monomial(s,m,n)
FILE *s;
MP m;
int n;
{
	int i,r;
	int *p;

	r = CMO_MONOMIAL32; write_int(s,&r);
	write_int(s,&n);
	for ( i = 0, p = m->dl->d; i < n; i++ ) {
		write_int(s,p++);
	}
	write_cmo_q(s,m->c);
}

write_cmo_list(s,list)
FILE *s;
LIST list;
{
	NODE m;
	int i,n,r;

	for ( n = 0, m = BDY(list); m; m = NEXT(m), n++ );
	r = CMO_LIST; write_int(s,&r);
	write_int(s,&n);
	for ( i = 0, m = BDY(list); i < n; i++, m = NEXT(m) )
		write_cmo(s,BDY(m));
}

write_cmo_string(s,str)
FILE *s;
STRING str;
{
	int r;

	r = CMO_STRING; write_int(s,&r);
	savestr(s,BDY(str));	
}

write_cmo_error(s,e)
FILE *s;
ERR e;
{
	int r;

	r = CMO_ERROR2; write_int(s,&r);
	write_cmo(s,BDY(e));
}

read_cmo(s,rp)
FILE *s;
Obj *rp;
{
	int id;
	int n,sgn,dummy;
	Q q,qnm,qdn;
	N nm,dn;
	P p,pnm,pdn;
	R r;
	Real real;
	double dbl;
	STRING str;
	USINT t;
	DP dp;
	char *b;
	Obj obj;
	ERR e;
	MATHCAP mc;

	read_int(s,&id);
	switch ( id ) {
	/* level 0 objects */
		case CMO_NULL:
			*rp = 0;
			break;
		case CMO_INT32:
			read_cmo_uint(s,rp);
			break;
		case CMO_DATUM:
		case CMO_STRING:
			loadstring(s,&str); *rp = (Obj)str;
			break;
		case CMO_MATHCAP:
			read_cmo(s,&obj); MKMATHCAP(mc,(LIST)obj);
			*rp = (Obj)mc;
			break;
		case CMO_ERROR:
			MKERR(e,0); *rp = (Obj)e;
			break;
		case CMO_ERROR2:
			read_cmo(s,&obj); MKERR(e,obj); *rp = (Obj)e;
			break;
	/* level 1 objects */
		case CMO_LIST:
			read_cmo_list(s,rp);
			break;
		case CMO_MONOMIAL32:
			read_cmo_monomial(s,rp);
			break;
		case CMO_ZZ:
			read_cmo_zz(s,&sgn,&nm);
			NTOQ(nm,sgn,q); *rp = (Obj)q;
			break;
		case CMO_QQ:
			read_cmo_zz(s,&sgn,&nm);
			read_cmo_zz(s,&dummy,&dn);
			NDTOQ(nm,dn,sgn,q); *rp = (Obj)q;
			break;
		case CMO_IEEE_DOUBLE_FLOAT:
			read_double(s,&dbl); MKReal(dbl,real); *rp = (Obj)real;
			break;
		case CMO_DISTRIBUTED_POLYNOMIAL:
			read_cmo_dp(s,&dp); *rp = (Obj)dp;
			break;
		case CMO_RECURSIVE_POLYNOMIAL:
			read_cmo_p(s,&p); *rp = (Obj)p;
			break;
		case CMO_UNIVARIATE_POLYNOMIAL:
			read_cmo_upoly(s,&p); *rp = (Obj)p;
			break;
		case CMO_INDETERMINATE:
			read_cmo(s,&str); *rp = (Obj)str;
			break;
		case CMO_RATIONAL:
			read_cmo(s,&pnm); read_cmo(s,&pdn);
			divr(CO,(Obj)pnm,(Obj)pdn,rp);
			break;
		case CMO_ZERO:
			*rp = 0;
			break;
		case CMO_DMS_OF_N_VARIABLES:
			read_cmo(s,rp);
			break;
		case CMO_RING_BY_NAME:
			read_cmo(s,rp);
			break;
		default:
			MKUSINT(t,id);
			t->id = O_VOID;
			*rp = (Obj)t;
			break;
	}
}

read_cmo_uint(s,rp)
FILE *s;
USINT *rp;
{
	unsigned int body;

	read_int(s,&body);
	MKUSINT(*rp,body);
}

read_cmo_zz(s,sgn,rp)
FILE *s;
int *sgn;
N *rp;
{
	int l,i,words;
	N n;
	unsigned int *b;
	unsigned int h;
	unsigned char c;

	read_int(s,&l);
	if ( l == 0 ) {
		*sgn = 0;
		*rp = 0;
		return;
	}
	if ( l < 0 ) {
		*sgn = -1; l = -l;
	} else
		*sgn = 1; 
#if 1
	*rp = n = NALLOC(l); PL(n) = l;
	read_intarray(s,BD(n),l);
#else
	words = (l+3)/4;
	*rp = n = NALLOC(words); PL(n) = words; b = BD(n);
	h = 0;
	switch ( l % 4 ) {
		case 0:
			read_char(s,&c); h = c;
		case 3:
			read_char(s,&c); h = (h<<8)|c;
		case 2:
			read_char(s,&c); h = (h<<8)|c;
		case 1:
			read_char(s,&c); h = (h<<8)|c;
	}
	b[words-1] = h;
	for ( i = words-2; i >= 0; i-- ) {
		read_char(s,&c); h = c;
		read_char(s,&c); h = (h<<8)|c;
		read_char(s,&c); h = (h<<8)|c;
		read_char(s,&c); h = (h<<8)|c;
		b[i] = h;
	}
#endif
}

read_cmo_list(s,rp)
FILE *s;
Obj *rp;
{
	int len;
	Obj *w;
	int i;
	Obj r,r1;
	NODE n0,n1;
	LIST list;

	read_int(s,&len);
	w = (Obj *)ALLOCA(len*sizeof(Obj));
	for ( i = 0; i < len; i++ )
		read_cmo(s,&w[i]);		
	for ( i = len-1, n0 = 0; i >= 0; i-- ) {
		MKNODE(n1,w[i],n0); n0 = n1;
	}
	MKLIST(list,n0);
	*rp = (Obj)list;
}

read_cmo_dp(s,rp)
FILE *s;
DP *rp;
{
	int len;
	int i;
	NODE n0,n1;
	MP mp0,mp;
	int nv,d;
	DP dp;
	Obj obj;

	read_int(s,&len);
	/* skip the ring definition */
	read_cmo(s,&obj);
	for ( mp0 = 0, i = 0, d = 0; i < len; i++ ) {
		read_cmo(s,&dp);
		if ( !mp0 ) {
			nv = dp->nv;
			mp0 = dp->body;
			mp = mp0;
		} else {
			NEXT(mp) = dp->body;
			mp = NEXT(mp);
		}
		d = MAX(d,dp->sugar);
	}
	MKDP(nv,mp0,dp);
	dp->sugar = d; *rp = dp;
}

read_cmo_monomial(s,rp)
FILE *s;
DP *rp;
{
	MP m;
	DP dp;
	int i,sugar,n;
	DL dl;

	read_int(s,&n);
	NEWMP(m); NEWDL(dl,n); m->dl = dl;
	read_intarray(s,dl->d,n);
	for ( sugar = 0, i = 0; i < n; i++ )
		sugar += dl->d[i];
	dl->td = sugar;
	read_cmo(s,&m->c);
	NEXT(m) = 0; MKDP(n,m,dp); dp->sugar = sugar; *rp = dp;
}

static V *remote_vtab;

read_cmo_p(s,rp)
FILE *s;
P *rp;
{
	LIST vlist;
	int nv,i;
	V *vtab;
	V v1,v2;
	NODE t;
	P v,p;
	VL tvl,rvl;
	char *name;

	read_cmo(s,&vlist);
	nv = length(BDY(vlist));
	vtab = (V *)ALLOCA(nv*sizeof(V));
	for ( i = 0, t = BDY(vlist); i < nv; t = NEXT(t), i++ ) {
/*		cmoname_to_localname(BDY((STRING)BDY(t)),&name); */
		name = BDY((STRING)BDY(t));
		makevar(name,&v); vtab[i] = VR(v);
	}
	remote_vtab = vtab;
	read_cmo(s,&p);
	for ( i = 0; i < nv-1; i++ ) {
		v1 = vtab[i]; v2 = vtab[i+1];
		for ( tvl = CO; tvl->v != v1 && tvl->v != v2; tvl = NEXT(tvl) );
		if ( tvl->v == v2 )
			break;
	}
	if ( i < nv-1 ) {
		 for ( i = nv-1, rvl = 0; i >= 0; i-- ) {
			NEWVL(tvl); tvl->v = vtab[i]; NEXT(tvl) = rvl; rvl = tvl;
		 }
		 reorderp(CO,rvl,p,rp);
	} else
		*rp = p;
}

read_cmo_upoly(s,rp)
FILE *s;
P *rp;
{
	int n,ind,i,d;
	P c;
	Q q;
	DCP dc0,dc;

	read_int(s,&n);
	read_int(s,&ind);
	for ( i = 0, dc0 = 0; i < n; i++ ) {
		read_int(s,&d);
		read_cmo(s,&c);
		if ( c ) {
			if ( OID(c) == O_USINT ) {
				UTOQ(((USINT)c)->body,q); c = (P)q;
			}
			NEXTDC(dc0,dc); 
			STOQ(d,q);
			dc->c = c; dc->d = q;
		}
	}
	if ( dc0 )
		NEXT(dc) = 0;
	MKP(remote_vtab[ind],dc0,*rp);
}

localname_to_cmoname(a,b)
char *a;
char **b;
{
	int l;
	char *t;

	l = strlen(a);
	if ( l >= 2 && a[0] == '@' && isupper(a[1]) ) {
		t = *b = (char *)MALLOC_ATOMIC(l);
		strcpy(t,a+1);
	} else {
		t = *b = (char *)MALLOC_ATOMIC(l+1);
		strcpy(t,a);
	}
}

cmoname_to_localname(a,b)
char *a;
char **b;
{
	int l;
	char *t;

	l = strlen(a);
	if ( isupper(a[0]) ) {
		t = *b = (char *)MALLOC_ATOMIC(l+2);
		strcpy(t+1,a);
		t[0] = '@';
	} else {
		t = *b = (char *)MALLOC_ATOMIC(l+1);
		strcpy(t,a);
	}
}
