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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/util.c,v 1.6 2000/12/11 09:28:03 noro Exp $ 
*/
#include "ca.h"
#include "base.h"
#include "parse.h"
#if defined(VISUAL) || (defined(__MACH__) && defined(__ppc__))
#include <stdarg.h>
#else
#include <varargs.h>
#endif
#include <ctype.h>

int length(n)
NODE n;
{
	int i;

	for ( i = 0; n; n = NEXT(n), i++ );
	return i;
}

int argc(a)
NODE a;
{
	int i;

	for ( i = 0; a; i++, a = NEXT(a) );
	return ( i );
}

void stoarg(s,acp,av)
char *s;
int *acp;
char **av;
{
	int i;

	for ( i = 0; ; i++ ) {
		for ( ; *s && isspace(*s); s++ );
		if ( *s ) {
			for ( av[i] = s; *s && !isspace(*s); s++ );
			if ( *s )
				*s++ = 0;
		} else {
			*acp = i;
			return;
		}
	}
}

#if 0
unsigned int ator(addr,r)
unsigned int addr;
Obj *r;
{
	N n;
	Q q;

	if ( !addr )
		q = 0;
	if ( addr < BASE )
		STOQ(addr,q);
	else {
		n = NALLOC(2); PL(n) = 2; 
		BD(n)[0] = addr-BASE; BD(n)[1] = addr>>BSH; NTOQ(n,1,q);
	}
	*r = (Obj)q;
}
#endif

void getarray(a,ind,vp)
pointer a;
NODE ind;
pointer *vp;
{
	Obj len,row,col;
	int i,l;
	NODE n,n0;
	VECT v;
	Q q;

	if ( a && OID(a) == O_BYTEARRAY ) {
		len = (Obj)BDY(ind);
		if ( !rangecheck(len,((BYTEARRAY)a)->len) )
			error("getarray : Out of range");
		else if ( NEXT(ind) )
			error("getarray : invalid index");
		else {
			i = (int)BDY((BYTEARRAY)a)[QTOS((Q)len)];
			STOQ(i,q);
			*vp = (pointer)q;
		}
		return;
	}
	for ( ; ind; ind = NEXT(ind) ) {
		if ( !a )
			error("getarray : array or list expected");
		switch ( OID(a) ) {
			case O_VECT:
				len = (Obj)BDY(ind);
				if ( !rangecheck(len,((VECT)a)->len) )
					error("getarray : Out of range");
				else
					a = (pointer)(BDY((VECT)a)[QTOS((Q)len)]);
				break;
			case O_MAT:
				row = (Obj)BDY(ind);
				if ( !rangecheck(row,((MAT)a)->row) )
					error("getarray : Out of range");
				else if ( NEXT(ind) ) {
					ind = NEXT(ind);
					col = (Obj)BDY(ind);
					if ( !rangecheck(col,((MAT)a)->col) )
						error("getarray : Out of range");
					else
						a = (pointer)(BDY((MAT)a)[QTOS((Q)row)][QTOS((Q)col)]);
				} else {
					NEWVECT(v); v->len = ((MAT)a)->col; 
					v->body = (pointer *)BDY((MAT)a)[QTOS((Q)row)];
					a = (pointer)v;
				}
				break;
			case O_LIST:
				n0 = BDY((LIST)a); i = QTOS((Q)BDY(ind));
				if ( i < 0 )
					error("getarray : Out of range");
				for ( n = n0; i > 0 && n; n = NEXT(n), i-- );
				if ( i || !n )
					error("getarray : Out of range");
				else	
					a = (pointer)BDY(n);
				break;
			default:
				error("getarray : array or list expected");
				break;
		}
	}
	*vp = a;
}

void putarray(a,ind,b)
pointer a;
NODE ind;
pointer b;
{
	Obj len,row,col;
	int i,l;
	NODE n,n0;

	if ( a && OID(a) == O_BYTEARRAY ) {
		len = (Obj)BDY(ind);
		if ( !rangecheck(len,((BYTEARRAY)a)->len) )
			error("putarray : Out of range");
		else if ( NEXT(ind) )
			error("putarray : invalid index");
		else
			BDY((BYTEARRAY)a)[QTOS((Q)len)] = (unsigned char)QTOS((Q)b);
		return;
	}
	for ( ; ind; ind = NEXT(ind) ) {
		if ( !a )
			error("putarray : array expected");
		switch ( OID(a) ) {
			case O_VECT:
				len = (Obj)BDY(ind);
				if ( !rangecheck(len,((VECT)a)->len) )
					error("putarray : Out of range");
				else if ( NEXT(ind) )
					a = BDY((VECT)a)[QTOS((Q)len)];
				else
					BDY((VECT)a)[QTOS((Q)len)] = b;
				break;
			case O_MAT:
				row = (Obj)BDY(ind);
				if ( !rangecheck(row,((MAT)a)->row) )
					error("putarray : Out of range");
				else if ( NEXT(ind) ) {
					ind = NEXT(ind);
					col = (Obj)BDY(ind);
					if ( !rangecheck(col,((MAT)a)->col) )
						error("putarray : Out of range");
					else if ( NEXT(ind) )
						a = BDY((MAT)a)[QTOS((Q)row)][QTOS((Q)col)];
					else
						BDY((MAT)a)[QTOS((Q)row)][QTOS((Q)col)] = b;
				} else
					error("putarray : invalid assignment");
				break;
			case O_LIST:
				if ( NEXT(ind) ) {
					n0 = BDY((LIST)a); i = QTOS((Q)BDY(ind));
					if ( i < 0 )
						error("putarray : Out of range");
					for ( n = n0; i > 0 && n; n = NEXT(n), i-- );
					if ( i || !n )
						error("puarray : Out of range");
					else
						a = (pointer)BDY(n);
				} else
					error("putarray : invalid assignment");
				break;
			default:
				error("putarray : array expected");
				break;
		}
	}
}

int rangecheck(a,n)
Obj a;
int n;
{
	N m;

	if ( !a )
		return 1;
	if ( OID(a) != O_N || !RATN(a) || !INT(a) || SGN((Q)a) < 0 )
		return 0;
	m = NM((Q)a);
	if ( PL(m) > 1 || BD(m)[0] >= (unsigned int)n )
		return 0;
	return 1;
}

int zp(p)
P p;
{
	int r;
	DCP dc;

	if ( !p )
		r = 1;
	else if ( NUM(p) )
		r = INT((Q)p)?1:0;
	else
		for ( dc = DC(p), r = 1; dc && r; dc = NEXT(dc) ) 
			r &= zp(COEF(dc));
	return ( r );
}

#if defined(VISUAL) || (defined(__MACH__) && defined(__ppc__))
NODE mknode(int ac,...)
{
	va_list ap;
	int i;
	NODE n0,n;

	va_start(ap,ac);
	for ( i = 0, n0 = 0; i < ac; i++ ) {
		NEXTNODE(n0,n);
		BDY(n)=va_arg(ap, pointer);
	}
	va_end(ap);
	if ( n0 )
		NEXT(n)=0;
	return n0;
}

FNODE mkfnode(int ac,fid id,...)
{
	va_list ap;
	int i;
	FNODE r;

	va_start(ap,id);
	NEWFNODE(r,ac); ID(r) = id;
	for ( i = 0; i < ac; i++ )
		r->arg[i] = va_arg(ap, pointer);
	va_end(ap);
	return r;
}

SNODE mksnode(int ac,sid id,...)
{
	va_list ap;
	int i;
	SNODE r;

	va_start(ap,id);
	NEWSNODE(r,ac); ID(r) = id;
	for ( i = 0; i < ac; i++ )
		r->arg[i] = va_arg(ap, pointer);
	va_end(ap);
	return r;
}
#else
NODE mknode(va_alist)
va_dcl
{
	va_list ap;
	int ac,i;
	NODE n0,n;

	va_start(ap);
	ac = va_arg(ap,int);
	for ( i = 0, n0 = 0; i < ac; i++ ) {
		NEXTNODE(n0,n);
		BDY(n)=va_arg(ap, pointer);
	}
	va_end(ap);
	if ( n0 )
		NEXT(n)=0;
	return n0;
}

FNODE mkfnode(va_alist)
va_dcl
{
	va_list ap;
	int ac,i;
	FNODE r;

	va_start(ap);
	ac = va_arg(ap, int);
	NEWFNODE(r,ac); ID(r) = va_arg(ap, fid);
	for ( i = 0; i < ac; i++ )
		r->arg[i] = va_arg(ap, pointer);
	va_end(ap);
	return r;
}

SNODE mksnode(va_alist)
va_dcl
{
	va_list ap;
	int ac,i;
	SNODE r;

	va_start(ap);
	ac = va_arg(ap, int);
	NEWSNODE(r,ac); ID(r) = va_arg(ap, sid);
	for ( i = 0; i < ac; i++ )
		r->arg[i] = va_arg(ap, pointer);
	va_end(ap);
	return r;
}
#endif

void makevar(str,p)
char *str;
P *p;
{
	VL vl;
	V v;
	P t;

	for ( vl = CO; ; ) 
		if ( NAME(VR(vl)) && !strcmp(NAME(VR(vl)),str) ) {
			MKV(VR(vl),t); *p = t;
			return;
		} else if ( !NEXT(vl) ) {
			NEWV(v); v->attr = (pointer)V_IND;
			NAME(v) = (char *)CALLOC(strlen(str)+1,sizeof(char));
			strcpy(NAME(v),str);
			NEWVL(NEXT(vl)); VR(NEXT(vl)) = v; NEXT(NEXT(vl)) = 0;
			MKV(v,t); *p = t;
			return;
		} else 
			vl = NEXT(vl);
}

void makesrvar(f,p)
FUNC f;
P *p;
{
	VL vl;
	V v;
	P t;
	char *str;

	for ( vl = CO, str = NAME(f); ; )
		if ( NAME(VR(vl)) && !strcmp(NAME(VR(vl)),str) ) {
			VR(vl)->attr = (pointer)V_SR; VR(vl)->priv = (pointer)f;
			MKV(VR(vl),t); *p = t;
			return;
		} else if ( !NEXT(vl) ) {
			NEWV(v); v->attr = (pointer)V_SR; v->priv = (pointer)f;
			NAME(v) = (char *)CALLOC(strlen(str)+1,sizeof(char));
			strcpy(NAME(v),str);
			NEWVL(NEXT(vl)); VR(NEXT(vl)) = v; NEXT(NEXT(vl)) = 0;
			MKV(v,t); *p = t;
			return;
		} else
			vl = NEXT(vl);
}

void appendtonode(n,a,nrp)
NODE n;
pointer a;
NODE *nrp;
{
	NODE tn;

	if ( !n )
		MKNODE(*nrp,a,0);
	else {
		for ( tn = n; NEXT(tn); tn = NEXT(tn) );
		MKNODE(NEXT(tn),a,0); *nrp = n;
	}
}

void appendtonode2(n,a,b,nrp)
NODE2 n;
pointer a,b;
NODE2 *nrp;
{
	NODE2 tn;

	if ( !n )
		MKNODE2(*nrp,a,b,0);
	else {
		for ( tn = n; NEXT(tn); tn = NEXT(tn) );
		MKNODE2(NEXT(tn),a,b,0); *nrp = n;
	}
}

void appendvar(vl,v)
VL vl;
V v;
{
	while (1)
		if ( vl->v == v )
			return;
		else if ( !NEXT(vl) ) {
			NEWVL(NEXT(vl)); VR(NEXT(vl)) = v; NEXT(NEXT(vl)) = 0;
			return;
		} else
			vl = NEXT(vl);
}

void reallocarray(arrayp,sizep,indexp,esize)
char **arrayp;
int *sizep,*indexp;
int esize;
{
	char *new;

	if ( *arrayp ) {
		*sizep *= 2;
		new = (char *)MALLOC((*sizep)*esize);
		bcopy(*arrayp,new,*indexp*esize);
		*arrayp = new;
	} else {
		*sizep = DEFSIZE; *indexp = 0;
		new = (char *)MALLOC((*sizep)*esize);
		bzero(new,DEFSIZE*esize);
		*arrayp = new;
	}
}
