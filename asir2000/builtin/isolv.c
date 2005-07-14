/*
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/isolv.c,v 1.4 2005/02/08 18:06:05 saito Exp $
 */

#include "ca.h"
#include "parse.h"
#include "version.h"

#if defined(INTERVAL)

static void Solve(NODE, Obj *);
static void NSolve(NODE, Obj *);

void Solve1(P, Q, pointer *);
void Sturm(P, VECT *);
void boundbody(P, Q *);
void binary(int , MAT);
void separate(Q, Q, VECT, Q, Q, int, int, MAT, int *);
void ueval(P, Q, Q *);

struct ftab isolv_tab[] = {
	{"solve", Solve, 2},
	{"nsolve", NSolve, 2},
	{0,0,0},
};

static void
Solve(arg, rp)
NODE arg;
Obj  *rp;
{
	pointer p, Eps;
	pointer    root;
	V       v;
	Q       eps;

	p = (pointer)ARG0(arg);
	if ( !p ) {
		*rp = 0;
		return;
	}
	Eps = (pointer)ARG1(arg);
	asir_assert(Eps, O_N, "solve");
	if ( NID(Eps) != N_Q ) {
		fprintf(stderr,"solve arg 2 is required a rational number");
		error(" : invalid argument");
		return;
	}
	DUPQ((Q)Eps, eps);
	SGN(eps) = 1;
	switch (OID(p)) {
		case O_N:
			*rp = 0;
			break;
		case O_P:
			Pvars(arg, &root);
			if (NEXT(BDY((LIST)root)) != 0) {
				fprintf(stderr,"solve arg 1 is univariate polynormial");
				error(" : invalid argument");
				break;
			}
			Solve1((P)p, eps, &root);
			*rp = (Obj)root;
			break;
		case O_LIST:
			fprintf(stderr,"solve,");
			error(" : Sorry, not yet implement of multivars");
			break;
		default:
			*rp = 0;
	}
}

static void
NSolve(arg, rp)
NODE arg;
Obj  *rp;
{
	pointer	p, Eps;
	pointer	root;
	LIST		listp;
	V			v;
	Q			eps;
	NODE		n, n0, m0, m, ln0;
	Num		r;
	Itv		iv;
	BF			breal;

	p = (pointer)ARG0(arg);
	if ( !p ) {
		*rp = 0;
		return;
	}
	Eps = (pointer)ARG1(arg);
	asir_assert(Eps, O_N, "solve");
	if ( NID(Eps) != N_Q ) {
		fprintf(stderr,"solve arg 2 is required a rational number");
		error(" : invalid argument");
		return;
	}
	DUPQ((Q)Eps, eps);
	SGN(eps) = 1;
	switch (OID(p)) {
		case O_N:
			*rp = 0;
			break;
		case O_P:
			Pvars(arg, &root);
			if (NEXT(BDY((LIST)root)) != 0) {
				fprintf(stderr,"solve arg 1 is univariate polynormial");
				error(" : invalid argument");
				break;
			}
			Solve1((P)p, eps, &root);
			for (m0 = BDY((LIST)root), n0 = 0; m0; m0 = NEXT(m0)) {
				m = BDY((LIST)BDY(m0));
				miditvp(BDY(m), &r);
				ToBf(r, &breal);
				NEXTNODE( n0, n );
				MKNODE(ln0, breal, NEXT(m));
				MKLIST((LIST)listp, ln0);
				BDY(n) = (pointer)listp;
			}
			NEXT(n) = 0;
			MKLIST((LIST)listp,n0);
			*rp = (pointer)listp;
			break;
		case O_LIST:
			fprintf(stderr,"solve,");
			error(" : Sorry, not yet implement of multivars");
			break;
		default:
			*rp = 0;
	}
}

void
Solve1(inp, eps, rt)
P    inp;
Q    eps;
pointer *rt;
{
	P    p;
	Q    up, low, a;
	DCP fctp, onedeg, zerodeg;
	LIST listp;
	VECT sseq;
	MAT  root;
	int  chvu, chvl, pad, tnumb, numb, i, j;
	Itv  iv;
	NODE n0, n, ln0, ln;

	boundbody(inp, &up);
	if (!up) {
		*rt = 0;
		return;
	}
	Sturm(inp, &sseq);
	DUPQ(up,low);
	SGN(low) = -1;
	chvu = stumq(sseq, up);
	chvl = stumq(sseq, low);
	tnumb = abs(chvu - chvl);
	MKMAT(root, tnumb, 4);
	pad = -1;

	fctrp(CO,inp,&fctp);
	for (fctp = NEXT(fctp), i = 0; fctp; fctp = NEXT(fctp)) {
		p = COEF(fctp);
		onedeg = DC(p);
		if ( !cmpq(DEG(onedeg), ONE) ) {
			pad++;
			if ( !NEXT(onedeg) ) {
				BDY(root)[pad][0] = 0;
				BDY(root)[pad][1] = 0;
				BDY(root)[pad][2] = DEG(fctp);
				BDY(root)[pad][3] = p;
			} else {
				divq((Q)COEF(NEXT(onedeg)),(Q)COEF(onedeg),&a);
				BDY(root)[pad][0] = a;
				BDY(root)[pad][1] = BDY(root)[pad][0];
				BDY(root)[pad][2] = DEG(fctp);
				BDY(root)[pad][3] = p;
			}
			continue;
		}
		boundbody(p, &up);
		Sturm(p, &sseq);
		DUPQ(up,low);
		SGN(low) = -1;
		chvu = stumq(sseq, up);
		chvl = stumq(sseq, low);
		numb = abs(chvu - chvl);
		separate(DEG(fctp), eps, sseq, up, low, chvu, chvl, root, &pad);
	}
	for (i = 0; i < pad; i++) {
		for (j = i; j <= pad; j++) {
			if (cmpq(BDY(root)[i][0], BDY(root)[j][0]) > 0) {
				a = BDY(root)[i][0];
				BDY(root)[i][0] = BDY(root)[j][0];
				BDY(root)[j][0] = a;
				a = BDY(root)[i][1];
				BDY(root)[i][1] = BDY(root)[j][1];
				BDY(root)[j][1] = a;
				a = BDY(root)[i][2];
				BDY(root)[i][2] = BDY(root)[j][2];
				BDY(root)[j][2] = a;
				a = BDY(root)[i][3];
				BDY(root)[i][3] = BDY(root)[j][3];
				BDY(root)[j][3] = a;
			}
		}
	}
	for (i = 0; i < pad; i++) {
		while(cmpq(BDY(root)[i][1], BDY(root)[i+1][0]) > 0 ) {
			binary(i, root);
			binary(i+1, root);
			if ( cmpq(BDY(root)[i][0], BDY(root)[i+1][1]) > 0 ) {
				a = BDY(root)[i][0];
				BDY(root)[i][0] = BDY(root)[i+1][0];
				BDY(root)[i+1][0] = a;
				a = BDY(root)[i][1];
				BDY(root)[i][1] = BDY(root)[i+1][1];
				BDY(root)[i+1][1] = a;
				a = BDY(root)[i][2];
				BDY(root)[i][2] = BDY(root)[i+1][2];
				BDY(root)[i+1][2] = a;
				a = BDY(root)[i][3];
				BDY(root)[i][3] = BDY(root)[i+1][3];
				BDY(root)[i+1][3] = a;
				break;
			}
		}
	}
	for (i = 0, n0 = 0; i <= pad; i++) {
		istoitv(BDY(root)[i][0], BDY(root)[i][1], &iv);
		NEXTNODE(n0,n);
		MKNODE(ln, BDY(root)[i][2], 0); MKNODE(ln0, iv, ln);
		MKLIST(listp, ln0);BDY(n) = (pointer)listp;
	}
	NEXT(n) = 0;
	MKLIST(listp,n0);
	*rt = (pointer)listp;
}

void
separate(mult, eps, sseq, up, low, upn, lown, root, padp)
VECT sseq;
Q		mult, eps, up, low;
int	upn, lown;
MAT	root;
int	*padp;
{
	int de, midn;
	Q   mid, e;
	P   p;

	de = abs(lown - upn);
	if (de == 0) return;
	if (de == 1) {
		(*padp)++;
		BDY(root)[*padp][0] = up;
		BDY(root)[*padp][1] = low;
		BDY(root)[*padp][3] = (P *)sseq->body[0];
		subq( BDY(root)[*padp][1], BDY(root)[*padp][0], &e );
		SGN(e) = 1;
		while (cmpq(e, eps) > 0) {
			binary(*padp, root);
			subq( BDY(root)[*padp][1], BDY(root)[*padp][0], &e);
			SGN(e) = 1;
		}
		BDY(root)[*padp][2] = mult;
		return;
	}
	addq(up, low, &mid);
	divq(mid, TWO, &mid);
	midn = stumq(sseq, mid);
	separate(mult, eps, sseq, low, mid, lown, midn, root, padp);
	separate(mult, eps, sseq, mid, up, midn, upn, root, padp);
}

void
binary(indx, root)
int indx;
MAT root;
{
	Q	a, b, c, d, e;
	P	p;
	p = (P)BDY(root)[indx][3];
	addq(BDY(root)[indx][0], BDY(root)[indx][1], &c);
	divq(c, TWO, &d);
	ueval(p, BDY(root)[indx][1], &a);
	ueval(p, d, &b);
	if (SGN(a) == SGN(b)){
		BDY(root)[indx][1] = d;
	} else {
		BDY(root)[indx][0] = d;
	}
}

void
Sturm(p, ret)
P    p;
VECT *ret;
{
	P    g1,g2,q,r,s, *t;
	Q    a,b,c,d,h,l,m,x;
	V    v;
	VECT seq;
	int  i,j;

	v = VR(p);
	t = (P *)ALLOCA((deg(v,p)+1)*sizeof(P));
	g1 = t[0] = p; diffp(CO,p,v,(P *)&a); ptozp((P)a,1,&c,&g2); t[1] = g2;
	for ( i = 1, h = ONE, x = ONE; ; ) {
		if ( NUM(g2) ) break;
		subq(DEG(DC(g1)),DEG(DC(g2)),&d);
		l = (Q)LC(g2);
		if ( SGN(l) < 0 ) {
			chsgnq(l,&a); l = a;
		}
		addq(d,ONE,&a); pwrq(l,a,&b); mulp(CO,(P)b,g1,(P *)&a);
		divsrp(CO,(P)a,g2,&q,&r);
		if ( !r ) break;
		chsgnp(r,&s); r = s; i++;
		if ( NUM(r) ) {
			t[i] = r; break;
		}
		pwrq(h,d,&m); g1 = g2;
		mulq(m,x,&a); divsp(CO,r,(P)a,&g2); t[i] = g2;
		x = (Q)LC(g1);
		if ( SGN(x) < 0 ) {
			chsgnq(x,&a); x = a;
		}
		pwrq(x,d,&a); mulq(a,h,&b); divq(b,m,&h);
	}
	MKVECT(seq,i+1);
	for ( j = 0; j <= i; j++ ) seq->body[j] = (pointer)t[j];
	*ret = seq;
}

int
stumq(s, val)
VECT s;
Q val;
{
	int len, i, j, c;
	P   *ss;
	Q   a, a0;

	len = s->len;
	ss = (P *)s->body;
	for ( j = 0; j < len; j++ ){
		ueval(ss[j],val,&a0);
		if (a0) break;
	}
	for ( i = j++, c =0; i < len; i++) {
		ueval( ss[i], val, &a);
		if ( a ) {
			if( (SGN(a) > 0 && SGN(a0) < 0) || (SGN(a) < 0 && SGN(a0) > 0) ){
				c++;
				a0 = a;
			}
		}
	}
	return c;
}

void
boundbody(p, q)
P  p;
Q *q;
{
	Q		t, max, tmp;
	DCP	dc;

	if ( !p )
		*q = 0;
	else if ( p->id == O_N )
		*q = 0;
	else {
		NEWQ(tmp);
		SGN(tmp)=1;
		for ( dc = DC(p), max=0; dc; dc = NEXT(dc) ) {
			t = (Q)COEF(dc);
			NM(tmp)=NM(t);
			DN(tmp)=DN(t);
			if ( cmpq(tmp, max) > 0 ) DUPQ(tmp, max);
		}
		addq(ONE, max, q);
	}
}

void
ueval(p, q, rp)
P p;
Q q;
Q *rp;
{
	Q   d, d1, a, b, t;
	Q   deg, da;
	Q   nm, dn;
	DCP dc;

	if ( !p ) *rp = 0;
	else if ( NUM(p) ) *rp = (Q)p;
	else {
		if ( q ) {
			NTOQ( DN(q), 1, dn );
			NTOQ( NM(q), SGN(q), nm );
		} else {
			dn = 0;
			nm = 0;
		}
		if ( !dn ) {
			dc = DC(p); t = (Q)COEF(dc);
			for ( d = DEG(dc), dc = NEXT(dc); dc; d = DEG(dc), dc= NEXT(dc) ) {
				subq(d, DEG(dc), &d1); pwrq(nm, d1, &a);
				mulq(t,a,&b); addq(b,(Q)COEF(dc),&t);
			}
			if ( d ) {
				pwrq(nm,d,&a); mulq(t,a,&b); t = b;
			}
			*rp = t;
		} else {
			dc = DC(p); t = (Q)COEF(dc);
			for ( d=deg= DEG(dc), dc = NEXT(dc); dc; d = DEG(dc), dc= NEXT(dc) ) {
				subq(d, DEG(dc), &d1); pwrq(nm, d1, &a);
				mulq(t,a,&b);
				subq(deg, DEG(dc), &d1); pwrq(dn, d1, &a);
				mulq(a, (Q)COEF(dc), &da);
				addq(b,da,&t);
			}
			if ( d ) {
				pwrq(nm,d,&a); mulq(t,a,&b); t = b;
			}
			*rp = t;
		}
	}
}
#endif
