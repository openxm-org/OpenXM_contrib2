/* $OpenXM: OpenXM_contrib2/asir2000/engine/gfsn.c,v 1.3 2001/09/03 01:06:40 noro Exp $ */

#include "ca.h"
#include "base.h"

UM current_mod_gfsn;

void setmod_gfsn(p)
UM p;
{
	current_mod_gfsn = p;
}

void getmod_gfsn(up)
UM *up;
{
	*up = current_mod_gfsn;
}

void simpgfsn(n,r)
GFSN n;
GFSN *r;
{
	UM t,q;

	if ( !n )
		*r = 0;
	else if ( NID(n) != N_GFSN )
		*r = n;
	else {
		t = UMALLOC(DEG(BDY(n)));
		q = W_UMALLOC(DEG(BDY(n)));
		cpyum(BDY(n),t);
		DEG(t) = divsfum(t,current_mod_gfsn,q);
		MKGFSN(t,*r);
	}
}

#define NZGFSN(a) ((a)&&(OID(a)==O_N)&&(NID(a)==N_GFSN))

void ntogfsn(a,b)
Obj a;
GFSN *b;
{
	UM t;
	GFS c;

	if ( !a || (OID(a)==O_N && NID(a) == N_GFSN) )
		*b = (GFSN)a;
	else if ( OID(a) == O_N && (NID(a) == N_GFS || NID(a) == N_Q) ) {
		ntogfs((Num)a,&c);
		if ( !b )
			*b = 0;
		else {
			t = UMALLOC(0); ptosfum((P)c,t);
			MKGFSN(t,*b);
		}
	} else
		error("ntogfsn : invalid argument");
}

void addgfsn(a,b,c)
GFSN a,b;
GFSN *c;
{
	UM t,q;
	GFSN z;
	int d;
	MQ qq;

	ntogfsn((Obj)a,&z); a = z; ntogfsn((Obj)b,&z); b = z;
	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else {
		d = MAX(DEG(BDY(a)),DEG(BDY(b)));
		t = UMALLOC(d);
		q = W_UMALLOC(d);
		addsfum(BDY(a),BDY(b),t);
		DEG(t) = divsfum(t,current_mod_gfsn,q);
		MKGFSN(t,z);
		*c = (GFSN)z;
	}
}

void subgfsn(a,b,c)
GFSN a,b;
GFSN *c;
{
	UM t,q;
	GFSN z;
	int d;

	ntogfsn((Obj)a,&z); a = z; ntogfsn((Obj)b,&z); b = z;
	if ( !a )
		chsgngfsn(b,c);
	else if ( !b )
		*c = a;
	else {
		d = MAX(DEG(BDY(a)),DEG(BDY(b)));
		t = UMALLOC(d);
		q = W_UMALLOC(d);
		subsfum(BDY(a),BDY(b),t);
		DEG(t) = divsfum(t,current_mod_gfsn,q);
		MKGFSN(t,*c);
	}
}

extern int up_lazy;

void mulgfsn(a,b,c)
GFSN a,b;
GFSN *c;
{
	UM t,q;
	GFSN z;
	int d;

	ntogfsn((Obj)a,&z); a = z; ntogfsn((Obj)b,&z); b = z;
	if ( !a || !b )
		*c = 0;
	else {
		d = DEG(BDY(a))+DEG(BDY(b));
		t = UMALLOC(d);
		q = W_UMALLOC(d);
		mulsfum(BDY(a),BDY(b),t);
		DEG(t) = divsfum(t,current_mod_gfsn,q);
		MKGFSN(t,*c);
	}
}

void divgfsn(a,b,c)
GFSN a,b;
GFSN *c;
{
	GFSN z;
	int d;
	UM wb,wc,wd,we,t,q;

	ntogfsn((Obj)a,&z); a = z; ntogfsn((Obj)b,&z); b = z;
	if ( !b )
		error("divgfsn: division by 0");
	else if ( !a )
		*c = 0;
	else {
		wb = W_UMALLOC(DEG(BDY(b))); cpyum(BDY(b),wb); 
		d = DEG(current_mod_gfsn);
		wc = W_UMALLOC(d); cpyum(current_mod_gfsn,wc);
		wd = W_UMALLOC(2*d); we = W_UMALLOC(2*d);
		/* wd*wb+we*wc=1 */
		eucsfum(wb,wc,wd,we);
		d = DEG(BDY(a))+DEG(wd);
		t = UMALLOC(d);
		q = W_UMALLOC(d);
		mulsfum(BDY(a),wd,t);
		DEG(t) = divsfum(t,current_mod_gfsn,q);
		MKGFSN(t,*c);
	}
}

void invgfsn(b,c)
GFSN b;
GFSN *c;
{
	GFSN z;
	int d;
	UM wb,wc,wd,we,t;

	ntogfsn((Obj)b,&z); b = z;
	if ( !b )
		error("divgfsn: division by 0");
	else {
		wb = W_UMALLOC(DEG(BDY(b))); cpyum(BDY(b),wb); 
		d = DEG(current_mod_gfsn);
		wc = W_UMALLOC(d); cpyum(current_mod_gfsn,wc);
		wd = W_UMALLOC(2*d); we = W_UMALLOC(2*d);
		/* wd*wb+we*wc=1 */
		eucsfum(wb,wc,wd,we);
		d = DEG(wd);
		t = UMALLOC(d);
		cpyum(wd,t);
		MKGFSN(t,*c);
	}
}

void chsgngfsn(a,c)
GFSN a,*c;
{
	GFSN z;
	int d;
	struct oUM zero;
	UM t;

	ntogfsn((Obj)a,&z); a = z;
	if ( !a )
		*c = 0;
	else {
		d = DEG(BDY(a));
		t = UMALLOC(d);
		DEG(&zero) = -1;
		subsfum(&zero,BDY(a),t);
		MKGFSN(t,*c);
	}
}

void pwrgfsn(a,b,c)
GFSN a;
Q b;
GFSN *c;
{
	GFSN z;
	UM t,x,y,q;
	int d,k;
	N e;

	ntogfsn((Obj)a,&z); a = z;
	if ( !b ) {
		t = UMALLOC(0); DEG(t) = 0; COEF(t)[0] = _onesf();
		MKGFSN(t,*c);
	} else if ( !a )
		*c = 0;
	else {
		d = DEG(current_mod_gfsn);

		/* y = 1 */
		y = UMALLOC(d); DEG(y) = 0; COEF(y)[0] = _onesf();

		t = W_UMALLOC(2*d); q = W_UMALLOC(2*d);

		/* x = simplify(a) */
		x = W_UMALLOC(DEG(BDY(a))); cpyum(BDY(a),x);
		DEG(x) = divsfum(x,current_mod_gfsn,q);
		if ( DEG(x) < 0 ) {
			*c = 0;
		} else {
			e = NM(b);
			for ( k = n_bits(e)-1; k >= 0; k-- ) {
				mulsfum(y,y,t);
				DEG(t) = divsfum(t,current_mod_gfsn,q);
				cpyum(t,y);
				if ( e->b[k/32] & (1<<(k%32)) ) {
					mulsfum(y,x,t);
					DEG(t) = divsfum(t,current_mod_gfsn,q);
					cpyum(t,y);
				}
			}
			MKGFSN(y,*c);
		}
	}
}

int cmpgfsn(a,b)
GFSN a,b;
{
	GFSN z;

	ntogfsn((Obj)a,&z); a = z; ntogfsn((Obj)b,&z); b = z;
	if ( !a )
		if ( !b )
			return 0;
		else
			return -1;
	else if ( !b )
			return 1;
	else
		return compsfum(BDY(a),BDY(b));
}

void randomgfsn(r)
GFSN *r;
{
	int i,d;
	UM t;

	if ( !current_mod_gfsn )
		error("randomgfsn : current_mod_gfsn is not set");
	d = DEG(current_mod_gfsn);
	t = UMALLOC(d-1);
	randsfum(d,t);
	degum(t,d-1);
	if ( DEG(t) < 0 )
		*r = 0;
	else {
		MKGFSN(t,*r);
	}
}
