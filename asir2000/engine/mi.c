/* $OpenXM: OpenXM/src/asir99/engine/mi.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

extern int current_mod;

void addmi(a,b,c)
MQ a,b;
MQ *c;
{
	if ( !current_mod )
		error("addmi : invalid modulus");
	if ( (a && NID(a) != N_M) || (b && NID(b) != N_M) )
		error("addmi : invalid argument");
	addmq(current_mod,a,b,c);
}

void submi(a,b,c)
MQ a,b;
MQ *c;
{
	if ( !current_mod )
		error("submi : invalid modulus");
	if ( (a && NID(a) != N_M) || (b && NID(b) != N_M) )
		error("submi : invalid argument");
	submq(current_mod,a,b,c);
}

void mulmi(a,b,c)
MQ a,b;
MQ *c;
{
	if ( !current_mod )
		error("mulmi : invalid modulus");
	if ( (a && NID(a) != N_M) || (b && NID(b) != N_M) )
		error("mulmi : invalid argument");
	mulmq(current_mod,a,b,c);
}

void divmi(a,b,c)
MQ a,b;
MQ *c;
{
	if ( !current_mod )
		error("divmi : invalid modulus");
	if ( (a && NID(a) != N_M) || (b && NID(b) != N_M) )
		error("divmi : invalid argument");
	divmq(current_mod,a,b,c);
}

void chsgnmi(a,c)
MQ a,*c;
{
	if ( !current_mod )
		error("chsgnmi : invalid modulus");
	if ( a && NID(a) != N_M )
		error("chsgnmi : invalid argument");
	submq(current_mod,0,a,c);
}

void pwrmi(a,b,c)
MQ a;
Q b;
MQ *c;
{
	if ( !current_mod )
		error("pwrmi : invalid modulus");
	if ( a && NID(a) != N_M )
		error("pwrmi : invalid argument");
	pwrmq(current_mod,a,b,c);
}

int cmpmi(a,b)
MQ a,b;
{
	int c;

	if ( !current_mod )
		error("cmpmi : invalid modulus");
	if ( (a && NID(a) != N_M) || (b && NID(b) != N_M) )
		error("cmpmi : invalid argument");
	if ( !a )
		if ( !b )
			return 0;
		else
			return CONT(b) < 0;
	else
		if ( !b )
			return CONT(a) > 0;
		else {
			c = CONT(a)-CONT(b);
			if ( c % current_mod )
				return c > 0;
			else
				return 0;
		}
}
