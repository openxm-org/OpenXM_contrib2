/* $OpenXM: OpenXM/src/asir99/engine/Ebug.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"

void substvp(vl,f,vn,g)
VL vl;
P f;
VN vn;
P *g;
{
	V v;
	int i;
	P h,h1;
	Q t;

	h = f;
	for ( i = 0; v = vn[i].v; i++ ) {
		STOQ(vn[i].n,t);
		substp(vl,h,v,(P)t,&h1); h = h1;
	}
	*g = h;
}
		
void affine(vl,f,vn,fr)
VL vl;
P f;
VN vn;
P *fr;
{
	int i,j,n;
	P vv,g,g1,t,u;
	Q s;
	int *dlist;
	P **plist;

	for ( n = 0; vn[n].v; n++);
	dlist = (int *)ALLOCA((n+1)*sizeof(int));	
	plist = (P **)ALLOCA((n+1)*sizeof(P *));	
	for ( i = 0; vn[i].v; i++ ) {
		if ( !vn[i].n )
			continue;
		dlist[i] = getdeg(vn[i].v,f); 
		plist[i] = (P *)ALLOCA((dlist[i]+1)*sizeof(P));

		MKV(vn[i].v,t);
		if ( vn[i].n ) {
			STOQ(vn[i].n,s); addp(vl,t,(P)s,&vv); 
		} else 
			vv = t;

		for ( j = 0, t = (P)ONE; j < dlist[i]; j++ ) {
			plist[i][j] = t;
			mulp(vl,t,vv,&u);
			t = u;
		}
		plist[i][j] = t;
	}

	g = f;
	for ( i = 0; vn[i].v; i++ ) {
		if ( !vn[i].n )
			continue;
		affinemain(vl,g,vn[i].v,dlist[i],plist[i],&g1); g = g1;
	}
	*fr = g;
}

