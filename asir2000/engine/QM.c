/* $OpenXM: OpenXM/src/asir99/engine/QM.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

void addmq(m,n1,n2,nr)
int m;
MQ n1,n2,*nr;
{
	int a;

	if ( !n1 ) 
		*nr = n2;
	else if ( !n2 ) 
		*nr = n1;
	else {
		a = (CONT(n1) + CONT(n2)) % m; STOMQ(a,*nr);
	}
}

void submq(m,n1,n2,nr)
int m;
MQ n1,n2,*nr;
{
	int a;

	if ( !n1 ) {
		a = (m - CONT(n2)) % m; STOMQ(a,*nr);
	} else if ( !n2 ) 
		*nr = n1;
	else {
		a = (CONT(n1) - CONT(n2) + m) % m; STOMQ(a,*nr);
	}
}

void mulmq(m,n1,n2,nr)
int m;
MQ n1,n2,*nr;
{
	int a,b;

	if ( !n1 || !n2 ) 
		*nr = 0;
	else {
		a = dmb(m,CONT(n1),CONT(n2),&b); STOMQ(a,*nr);
	}
}

void divmq(m,n1,n2,nq)
int m;
MQ n1,n2,*nq;
{
	int a,b;

	if ( !n2 )
		error("divmq : division by 0");
	else if ( !n1 ) 
		*nq = 0;
	else if ( n1 == n2 ) 
		*nq = ONEM;
	else {
		a = dmb(m,CONT(n1),invm(CONT(n2),m),&b); STOMQ(a,*nq);
	}
}

void invmq(m,n1,nq)
int m;
MQ n1,*nq;
{
	int a;

	if ( !n1 ) 
		error("invmq : division by 0");
	else {
		a = invm(CONT(n1),m); STOMQ(a,*nq);
	}
}

void pwrmq(m,n1,n,nr)
int m;
MQ n1;
Q n;
MQ *nr;
{
	int a;

	if ( n == 0 ) 
		*nr = ONEM;
	else if ( !n1 )
		*nr = 0;
	else {
		a = pwrm(m,CONT(n1),QTOS(n)); STOMQ(a,*nr);
	}
}

void mkbcm(m,n,t)
int m,n;
MQ *t;
{
	int i,j;
	int *a,*b,*c;

	W_CALLOC(n,int,a); W_CALLOC(n,int,b);
	for ( i = 1, a[0] = 1; i <= n; i++ ) {
		for ( j = 1, b[0] = b[i] = 1; j < i; j++ )
			b[j] = (a[j-1] + a[j]) % m;
		c = a; a = b; b = c;
	}
	for ( i = 0; i <= n; i++ )
		STOMQ(a[i],t[i]);
}
