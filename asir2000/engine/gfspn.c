#include "ca.h"
#include "base.h"

UM current_mod_gfspn;

void setmod_gfspn(p)
UM p;
{
	current_mod_gfspn = p;
}

void getmod_gfspn(up)
UM *up;
{
	*up = current_mod_gfspn;
}

void simpgfspn(n,r)
GFSPN n;
GFSPN *r;
{
	UM t,q;

	if ( !n )
		*r = 0;
	else if ( NID(n) != N_GFSPN )
		*r = n;
	else {
		t = UMALLOC(DEG(BDY(n)));
		q = W_UMALLOC(DEG(BDY(n)));
		cpyum(BDY(n),t);
		divsfum(t,current_mod_gfspn,q);
		MKGFSPN(t,*r);
	}
}

#define NZGFSPN(a) ((a)&&(OID(a)==O_N)&&(NID(a)==N_GFSPN))

void ntogfspn(a,b)
Obj a;
GFSPN *b;
{
	UM t;
	GFS c;

	if ( !a || (OID(a)==O_N && NID(a) == N_GFSPN) )
		*b = (GFSPN)a;
	else if ( OID(a) == O_N && (NID(a) == N_GFS || NID(a) == N_Q) ) {
		ntogfs((Num)a,&c);
		if ( !b )
			*b = 0;
		else {
			t = UMALLOC(0); ptosfum((P)c,t);
			MKGFSPN(t,*b);
		}
	} else
		error("ntogfspn : invalid argument");
}

void addgfspn(a,b,c)
GFSPN a,b;
GFSPN *c;
{
	UM t,q;
	GFSPN z;
	int d;

	ntogfspn((Obj)a,&z); a = z; ntogfspn((Obj)b,&z); b = z;
	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else {
		d = MAX(DEG(BDY(a)),DEG(BDY(b)));
		t = UMALLOC(d);
		q = W_UMALLOC(d);
		addsfum(BDY(a),BDY(b),t);
		divsfum(t,current_mod_gfspn,q);
		MKGFSPN(t,*c);
	}
}

void subgfspn(a,b,c)
GFSPN a,b;
GFSPN *c;
{
	UM t,q;
	GFSPN z;
	int d;

	ntogfspn((Obj)a,&z); a = z; ntogfspn((Obj)b,&z); b = z;
	if ( !a )
		chsgngfspn(b,c);
	else if ( !b )
		*c = a;
	else {
		d = MAX(DEG(BDY(a)),DEG(BDY(b)));
		t = UMALLOC(d);
		q = W_UMALLOC(d);
		subsfum(BDY(a),BDY(b),t);
		divsfum(t,current_mod_gfspn,q);
		MKGFSPN(t,*c);
	}
}

extern int up_lazy;

void mulgfspn(a,b,c)
GFSPN a,b;
GFSPN *c;
{
	UM t,q;
	GFSPN z;
	int d;

	ntogfspn((Obj)a,&z); a = z; ntogfspn((Obj)b,&z); b = z;
	if ( !a || !b )
		*c = 0;
	else {
		d = DEG(BDY(a))+DEG(BDY(b));
		t = UMALLOC(d);
		q = W_UMALLOC(d);
		mulsfum(BDY(a),BDY(b),t);
		divsfum(t,current_mod_gfspn,q);
		MKGFSPN(t,*c);
	}
}

void divgfspn(a,b,c)
GFSPN a,b;
GFSPN *c;
{
	GFSPN z;
	int d;
	UM wb,wc,wd,we,t,q;

	ntogfspn((Obj)a,&z); a = z; ntogfspn((Obj)b,&z); b = z;
	if ( !b )
		error("divgfspn: division by 0");
	else if ( !a )
		*c = 0;
	else {
		wb = W_UMALLOC(DEG(BDY(b))); cpyum(BDY(b),wb); 
		d = DEG(current_mod_gfspn);
		wc = W_UMALLOC(d); cpyum(current_mod_gfspn,wc);
		wd = W_UMALLOC(2*d); we = W_UMALLOC(2*d);
		/* wd*wb+we*wc=1 */
		eucsfum(wb,wc,wd,we);
		d = DEG(BDY(a))+DEG(wd);
		t = UMALLOC(d);
		q = W_UMALLOC(d);
		mulsfum(a,wd,t);
		divsfum(t,current_mod_gfspn,q);
		MKGFSPN(t,*c);
	}
}

void invgfspn(b,c)
GFSPN b;
GFSPN *c;
{
	GFSPN z;
	int d;
	UM wb,wc,wd,we,t;

	ntogfspn((Obj)b,&z); b = z;
	if ( !b )
		error("divgfspn: division by 0");
	else {
		wb = W_UMALLOC(DEG(BDY(b))); cpyum(BDY(b),wb); 
		d = DEG(current_mod_gfspn);
		wc = W_UMALLOC(d); cpyum(current_mod_gfspn,wc);
		wd = W_UMALLOC(2*d); we = W_UMALLOC(2*d);
		/* wd*wb+we*wc=1 */
		eucsfum(wb,wc,wd,we);
		d = DEG(wd);
		t = UMALLOC(d);
		cpyum(wd,t);
		MKGFSPN(t,*c);
	}
}

void chsgngfspn(a,c)
GFSPN a,*c;
{
	GFSPN z;
	int d;
	struct oUM zero;
	UM t;

	ntogfspn((Obj)a,&z); a = z;
	if ( !a )
		*c = 0;
	else {
		d = DEG(BDY(a));
		t = UMALLOC(d);
		DEG(&zero) = -1;
		subsfum(&zero,BDY(a),t);
		MKGFSPN(t,*c);
	}
}

void pwrgfspn(a,b,c)
GFSPN a;
Q b;
GFSPN *c;
{
	GFSPN z;
	UM t,x,y,q;
	int d,k;
	N e;

	ntogfspn((Obj)a,&z); a = z;
	if ( !b ) {
		t = UMALLOC(0); DEG(t) = 0; COEF(t)[0] = _onesf();
		MKGFSPN(t,*c);
	} else if ( !a )
		*c = 0;
	else {
		d = DEG(current_mod_gfspn);

		/* y = 1 */
		y = UMALLOC(d); DEG(y) = 0; COEF(y)[0] = _onesf();

		t = W_UMALLOC(2*d); q = W_UMALLOC(2*d);

		/* x = simplify(a) */
		x = W_UMALLOC(DEG(BDY(a))); cpyum(BDY(a),x);
		divsfum(x,current_mod_gfspn,q);
		if ( DEG(x) < 0 ) {
			*c = 0;
		} else {
			e = NM(b);
			for ( k = n_bits(e)-1; k >= 0; k-- ) {
				mulsfum(y,y,t);
				divsfum(t,current_mod_gfspn,q);
				cpyum(t,y);
				if ( e->b[k/32] & (1<<(k%32)) ) {
					mulsfum(y,x,t);
					divsfum(t,current_mod_gfspn,q);
					cpyum(t,y);
				}
			}
			MKGFSPN(y,*c);
		}
	}
}

int cmpgfspn(a,b)
GFSPN a,b;
{
	GFSPN z;

	ntogfspn((Obj)a,&z); a = z; ntogfspn((Obj)b,&z); b = z;
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

void randomgfspn(r)
GFSPN *r;
{
	int i,d;
	UM t;

	if ( !current_mod_gfspn )
		error("randomgfspn : current_mod_gfspn is not set");
	d = DEG(current_mod_gfspn);
	t = UMALLOC(d-1);
	randsfum(d,t);
	degum(t,d-1);
	if ( DEG(t) < 0 )
		*r = 0;
	else {
		MKGFSPN(t,*r);
	}
}
