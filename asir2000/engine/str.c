/* $OpenXM: OpenXM/src/asir99/engine/str.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

void addstr(vl,a,b,c)
VL vl;
STRING a,b;
STRING *c;
{
	char *buf;

	buf = (char *)MALLOC(strlen(BDY(a))+strlen(BDY(b))+1);
	sprintf(buf,"%s%s",BDY(a),BDY(b));
	MKSTR(*c,buf); 
}

int compstr(vl,a,b)
VL vl;
STRING a,b;
{
	return a ? (b?strcmp(BDY(a),BDY(b)):1) : (b?-1:0);
}
