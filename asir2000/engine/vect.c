/* $OpenXM: OpenXM/src/asir99/engine/vect.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

void addvect(vl,a,b,c)
VL vl;
VECT a,b,*c;
{
	int len,i;
	VECT t;
	pointer *ab,*bb,*tb;

	if ( !a ) 
		*c = b;
	else if ( !b )
		*c = a;
	else if ( a->len != b->len ) {
		*c = 0; error("addvect : size mismatch");
	} else {
		len = a->len;
		MKVECT(t,len);
		for ( i = 0, ab = BDY(a), bb = BDY(b), tb = BDY(t); i < len; i++ )
			addr(vl,(Obj)ab[i],(Obj)bb[i],(Obj *)&tb[i]);
		*c = t;
	}
}

void subvect(vl,a,b,c)
VL vl;
VECT a,b,*c;
{
	int len,i;
	VECT t;
	pointer *ab,*bb,*tb;

	if ( !a ) 
		chsgnvect(b,c);
	else if ( !b )
		*c = a;
	else if (a->len != b->len ) {
		*c = 0; error("subvect : size mismatch");
	} else {
		len = a->len;
		MKVECT(t,len);
		for ( i = 0, ab = BDY(a), bb = BDY(b), tb = BDY(t); 
				i < len; i++ )
				subr(vl,(Obj)ab[i],(Obj)bb[i],(Obj *)&tb[i]);
		*c = t;
	}
}

void mulvect(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	if ( !a || !b )
		*c = 0;
	else if ( OID(a) <= O_R )
		mulrvect(vl,a,(VECT)b,(VECT *)c);
	else if ( OID(b) <= O_R )
		mulrvect(vl,b,(VECT)a,(VECT *)c);
	else
		notdef(vl,a,b,c);
}

void divvect(vl,a,b,c)
VL vl;
Obj a,b,*c;
{
	Obj t;

	if ( !b ) 
		error("divvect : division by 0");
	else if ( !a )
		*c = 0;
	else if ( OID(b) > O_R )
		notdef(vl,a,b,c);
	else {
		divr(vl,(Obj)ONE,b,&t); mulrvect(vl,(Obj)t,(VECT)a,(VECT *)c);
	}
}

void chsgnvect(a,b)
VECT a,*b;
{
	VECT t;
	int len,i;
	pointer *ab,*tb;

	if ( !a )
		*b = 0;
	else {
		len = a->len;
		MKVECT(t,len);
		for ( i = 0, ab = BDY(a), tb = BDY(t); 
			i < len; i++ )
			chsgnr((Obj)ab[i],(Obj *)&tb[i]);
		*b = t;
	} 
}

void mulrvect(vl,a,b,c)
VL vl;
Obj a;
VECT b,*c;
{
	int len,i;
	VECT t;
	pointer *bb,*tb;

	if ( !a || !b ) 
		*c = 0;
	else {
		len = b->len;
		MKVECT(t,len);
		for ( i = 0, bb = BDY(b), tb = BDY(t); i < len; i++ )
			mulr(vl,a,(Obj)bb[i],(Obj *)&tb[i]);
		*c = t;
	}
}

int compvect(vl,a,b)
VL vl;
VECT a,b;
{
	int i,len,t;

	if ( !a )
		return b?-1:0;
	else if ( !b )
		return 1;
	else if ( a->len != b->len )
		return a->len>b->len ? 1 : -1;
	else {
		for ( i = 0, len = a->len; i < len; i++ )
			if ( t = compr(vl,(Obj)BDY(a)[i],(Obj)BDY(b)[i]) )
				return t;
		return 0;
	}
}
