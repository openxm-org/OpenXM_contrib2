/* $OpenXM: OpenXM/src/asir99/engine/ui.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

int compui(vl,a,b)
VL vl;
USINT a,b;
{
	if ( !a ) 
		if ( !b ) 
			return 0;
		else 
			return -1;
	else if ( !b ) 
		return 1;
	else if ( BDY(a) > BDY(b) )
		return 1;
	else if ( BDY(a) < BDY(b) ) 
		return -1;
	else
		return 0;
}
