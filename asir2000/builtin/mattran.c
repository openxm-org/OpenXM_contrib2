#include "ca.h"
#include "parse.h"

static void Rowm(NODE, Obj *);
static void Rowch(NODE, Obj *);
static void Rowmadd(NODE, Obj *);
//static void mlowch(NODE, Obj *, Obj *, Obj *);
//static void mlowad(NODE, Obj *, Obj *, Obj *);

struct ftab mat_tab[] = {
	{"rowm",Rowm,3},
	{"rowch",Rowch,3},
	{"rowmadd",Rowmadd,4},
	{0,0,0},
};

static void
Rowm(arg, rp)
NODE arg;
Obj *rp;
{
	MAT a;
	Q s;
	int row, size, i;
	pointer *k, *t, *ap;

	a = (MAT)ARG0(arg);
	size = a->col;
	s = (Q)ARG1(arg);
	row = QTOS(s);
	k = (pointer)ARG2(arg);
	ap = BDY(a)[row];
	for ( i = 0; i < size; i++ ) {
		mulr(CO,(Obj)ap[i],(Obj)k,(Obj *)&t);
		ap[i]=(Obj)t;
	}

	*rp = (Obj)a;
}

static void
Rowch(arg, rp)
NODE arg;
Obj *rp;
{
	MAT a;
	Q sa, sb;
	int rowa, rowb, size, i;
	pointer *k, *t, *ap, *aq;

	a = (MAT)ARG0(arg);
	size = a->col;
	sa = (Q)ARG1(arg);
	rowa = QTOS(sa);
	sb = (Q)ARG2(arg);
	rowb = QTOS(sb);
	ap = BDY(a)[rowa];
	aq = BDY(a)[rowb];
	for ( i = 0; i < size; i++ ) {
		t = ap[i];
		ap[i] = aq[i];
		aq[i] = t;
	}

	*rp = (Obj)a;
}

static void
Rowmadd(arg, rp)
NODE arg;
Obj *rp;
{
	MAT a;
	Q sa, sb;
	int rowa, rowb, size, i;
	pointer *k, *t, *ap, *aq;

	a = (MAT)ARG0(arg);
	size = a->col;
	sa = (Q)ARG1(arg);
	rowa = QTOS(sa);
	sb = (Q)ARG2(arg);
	rowb = QTOS(sb);
	ap = BDY(a)[rowa];
	aq = BDY(a)[rowb];
	k = (pointer)ARG3(arg);

	for ( i = 0; i < size; i++ ) {
		mulr( CO, (Obj)ap[i], (Obj)k, (Obj *)&t );
		addr( CO, (Obj)aq[i], (Obj)t, (Obj *)&t );
		aq[i] = t;
	}

	*rp = (Obj)a;
}
