/* $OpenXM: OpenXM/src/asir99/engine/real.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "base.h"
#include <math.h>

#if defined(THINK_C)
double RatnToReal(a)
Q a;
{
	double nm,dn,t;
	int enm,edn;
	char buf[BUFSIZ];

	nm = NatToReal(NM(a),&enm);
	if ( INT(a) )
		if ( enm >= 1 )
			error("RatnToReal : Overflow");
		else
			return SGN(a)>0 ? nm : -nm;
	else {
		dn = NatToReal(DN(a),&edn);
		sprintf(buf,"1.0E%d",enm-edn);
		sscanf(buf,"%lf",&t);
		if ( SGN(a) < 0 )
			t = -t;
		return nm/dn*t;
	}
}

double NatToReal(a,expo)
N a;
int *expo;
{
	int *p;
	int l,all,i,j,s,tb,top,tail;
	unsigned int t,m[2];	
	
	p = BD(a); l = PL(a) - 1;
	for ( top = 0, t = p[l]; t; t >>= 1, top++ );
	all = top + BSH*l; tail = (53-top)%BSH; i = l-(53-top)/BSH-1; 
	m[1] = i < 0 ? 0 : p[i]>>(BSH-tail);
	for ( j = 1, i++, tb = tail; i <= l; i++ ) {
		s = 32-tb; t = i < 0 ? 0 : p[i];
		if ( BSH > s ) {
			m[j] |= ((t&((1<<s)-1))<<tb);
			if ( !j ) 
				break;
			else {
				j--; m[j] = t>>s; tb = BSH-s;
			}
		} else {
			m[j] |= (t<<tb); tb += BSH;
		}
	}
	s = (all-1)+1023;
	m[0] = (m[0]&((1<<20)-1))|(MIN(2046,s)<<20); *expo = MAX(s-2046,0);
#ifdef vax
	t = m[0]; m[0] = m[1]; m[1] = t; itod(m);
#endif
#if defined(MIPSEL)
	t = m[0]; m[0] = m[1]; m[1] = t;
#endif
	return (double)(*((short double *)m));
}
#else
double RatnToReal(a)
Q a;
{
	double nm,dn,t,man;
	int enm,edn,e;
	unsigned int *p,s;

	nm = NatToReal(NM(a),&enm);
	if ( INT(a) )
		if ( enm >= 1 )
			error("RatnToReal : Overflow");
		else
			return SGN(a)>0 ? nm : -nm;
	else {
		dn = NatToReal(DN(a),&edn);
		man = nm/dn;
		if ( SGN(a) < 0 )
			man = -man;
		if ( ((e = enm - edn) >= 1024) || (e <= -1023) )
			error("RatnToReal : Overflow"); /* XXX */
		else if ( !e )
			return man;
		else
			return man*pow(2,e);
	}
}

double NatToReal(a,expo)
N a;
int *expo;
{
	unsigned int *p;
	int l,all,i,j,s,tb,top,tail;
	unsigned int t,m[2];	
	
	p = BD(a); l = PL(a) - 1;
	for ( top = 0, t = p[l]; t; t >>= 1, top++ );
	all = top + BSH*l; tail = (53-top)%BSH; i = l-(53-top)/BSH-1; 
	m[1] = i < 0 ? 0 : p[i]>>(BSH-tail);
	for ( j = 1, i++, tb = tail; i <= l; i++ ) {
		s = 32-tb; t = i < 0 ? 0 : p[i];
		if ( BSH > s ) {
			m[j] |= ((t&((1<<s)-1))<<tb);
			if ( !j ) 
				break;
			else {
				j--; m[j] = t>>s; tb = BSH-s;
			}
		} else {
			m[j] |= (t<<tb); tb += BSH;
		}
	}
	s = (all-1)+1023;
	m[0] = (m[0]&((1<<20)-1))|(MIN(2046,s)<<20); *expo = MAX(s-2046,0);
#ifdef vax
	t = m[0]; m[0] = m[1]; m[1] = t; itod(m);
#endif
#if defined(i386) || defined(MIPSEL) || defined(VISUAL) || defined(linux) || defined(__alpha) || defined(__FreeBSD__) || defined(__NetBSD__)
	t = m[0]; m[0] = m[1]; m[1] = t;
#endif
	return *((double *)m);
}
#endif

void addreal(a,b,c)
Num a,b;
Real *c;
{
	double t;

	t = ToReal(a)+ToReal(b); MKReal(t,*c);
}

void subreal(a,b,c)
Num a,b;
Real *c;
{
	double t;

	t = ToReal(a)-ToReal(b); MKReal(t,*c);
}

void mulreal(a,b,c)
Num a,b;
Real *c;
{
	double t;

	if ( !a || !b )
		*c = 0;
	else {
		t = ToReal(a)*ToReal(b);
		if ( !t )
			error("mulreal : Underflow"); /* XXX */
		else
			MKReal(t,*c);
	}
}

void divreal(a,b,c)
Num a,b;
Real *c;
{
	double t;

	if ( !a )
		*c = 0;
	else {
		t = ToReal(a)/ToReal(b);
		if ( !t )
			error("divreal : Underflow"); /* XXX */
		else	
			MKReal(t,*c);
	}
}

void chsgnreal(a,c)
Num a,*c;
{
	double t;
	Real s;

	if ( !a )
		*c = 0;
	else if ( NID(a) == N_Q )
		chsgnq((Q)a,(Q *)c);
	else {
		t = -ToReal(a); MKReal(t,s); *c = (Num)s;
	}
}

void pwrreal(a,b,c)
Num a,b;
Real *c;
{
	double t;
	double pwrreal0();

	if ( !b ) 
		MKReal(1.0,*c);
	else if ( !a ) 
		*c = 0;
	else if ( !RATN(b) || !INT(b) || (PL(NM((Q)b)) > 1) ) {
		t = (double)pow((double)ToReal(a),(double)ToReal(b));
		if ( !t )
			error("pwrreal : Underflow"); /* XXX */
		else	
			MKReal(t,*c);
	} else {
		t = pwrreal0(BDY((Real)a),BD(NM((Q)b))[0]);
		t = SGN((Q)b)>0?t:1/t;
		if ( !t )
			error("pwrreal : Underflow"); /* XXX */
		else	
			MKReal(t,*c);
	}
}

double pwrreal0(n,e)
double n;
int e;
{
	double t;

	if ( e == 1 )
		return n;
	else {
		t = pwrreal0(n,e / 2);
		return e%2 ? t*t*n : t*t;
	}
}

int cmpreal(a,b)
Real a,b;
{
	double t;

	t = ToReal(a)-ToReal(b);
	return t>0.0 ? 1 : t<0.0?-1 : 0;
}
