/* $OpenXM: OpenXM/src/asir99/io/biovar.c,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "ox.h"

static V *vtab;
VL file_vl;

void savevl(s,vl)
FILE *s;
VL vl;
{
	int n,i;
	VL tvl;

	for ( tvl = vl, n = 0; tvl; tvl = NEXT(tvl))
		if ( tvl->v->attr != (pointer)V_UC && tvl->v->attr != (pointer)V_PF )
			n++;
	write_int(s,&n);
	vtab = (V *)CALLOC(n,sizeof(V));
	for ( tvl = vl, i = 0; i < n; tvl = NEXT(tvl) )
		if ( tvl->v->attr != (pointer)V_UC && tvl->v->attr != (pointer)V_PF ) {
			vtab[i] = tvl->v; savev(s,vtab[i]); i++;
		}
}

void loadvl(s)
FILE *s;
{
	int n,i,need_reorder;
	V v1,v2;
	VL t,t1;

	read_int(s,&n);
	vtab = (V *)CALLOC(n,sizeof(V));
	for ( i = 0; i < n; i++ )
		loadv(s,&vtab[i]);
	need_reorder = 0;
	for ( i = 0; i < n-1; i++ ) {
		v1 = vtab[i]; v2 = vtab[i+1];
		for ( t = CO; t->v != v1 && t->v != v2; t = NEXT(t) );
		if ( t->v == v2 ) {
			need_reorder = 1; break;
		}
	}
	if ( need_reorder ) {
		for ( i = n-1, t = 0; i >= 0; i-- ) {
			NEWVL(t1); t1->v = vtab[i]; NEXT(t1) = t; t = t1;
		}
		file_vl = t;
	} else
		file_vl = 0;
}

void skipvl(s)
FILE *s;
{
	int n,i,size,len;

	read_int(s,&n);
	for ( i = 0, size = 0; i < n; i++ ) {
		read_int(s,&len); size += len;
	}
	fseek(s,size,1L);
}

void savev(s,v)
FILE *s;
V v;
{
	savestr(s,NAME(v));
}

void loadv(s,vp)
FILE *s;
V *vp;
{
	P p;
	char *name;

	loadstr(s,&name); 
	if ( name ) {
		makevar(name,&p);
		*vp = VR(p);
	} else
		*vp = 0;
}

int save_convv(v)
V v;
{
	int i;

	if ( ox_do_count )
		return 0;
	if ( v->attr == (pointer)V_PF )
		return -1;
	for ( i = 0; vtab[i]; i++ )
		if ( vtab[i] == v )
			return i;
}

V load_convv(vindex)
int vindex;
{
	return vtab[vindex];
}

void swap_bytes(ptr,unit,size)
char *ptr;
int unit,size;
{
	char *p;
	int i,j,hunit;
	char t;
	
	for ( i = 0, p = ptr, hunit = unit/2; i < size; i++, p += unit )
		for ( j = 0; j < hunit; j++ ) {
			t = p[j]; p[j] = p[unit-j-1]; p[unit-j-1] = t;
		}
}
