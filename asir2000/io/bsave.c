/* $OpenXM: OpenXM/src/asir99/io/bsave.c,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
/* saveXXX must not use GC_malloc(), GC_malloc_atomic(). */

#include "ca.h"
#include "parse.h"
#if INET
#include "com.h"
#endif

#if PARI
#include "genpari.h"
int get_lg(GEN);
#endif

void saveerror(FILE *,ERR);
void saveui(FILE *,USINT);
void savedp(FILE *,DP);
void savestr(FILE *,char *);
void savestring(FILE *,STRING);
void savemat(FILE *,MAT);
void savevect(FILE *,VECT);
void savelist(FILE *,LIST);
void saver(FILE *,R);
void savep(FILE *,P);
void savegf2n(FILE *,GF2N);
void savegfpn(FILE *,GFPN);
void savelm(FILE *,LM);
void savemi(FILE *,MQ);
void savecplx(FILE *,C);
void savebf(FILE *,BF);
void savereal(FILE *,Real);
void saveq(FILE *,Q);
void savenum(FILE *,Num);
void savepfins(FILE *,V);
void savegfmmat(FILE *,GFMMAT);

#define O_GF2MAT 12
#define O_MATHCAP 13
#define O_F 14
#define O_GFMMAT 15

void (*savef[])() = { 0, savenum, savep, saver, savelist, savevect,
	savemat, savestring, 0, savedp, saveui, saveerror,0,0,0,savegfmmat };
void (*nsavef[])() = { saveq, savereal, 0, savebf, savecplx ,savemi, savelm, savegf2n, savegfpn};

static short zeroval = 0;

void saveobj(s,p)
FILE *s;
Obj p;
{
	if ( !p )
		write_short(s,&zeroval);
	else if ( !savef[OID(p)] )
		error("saveobj : not implemented");
	else
		(*savef[OID(p)])(s,p);
}

void savenum(s,p)
FILE *s;
Num p;
{ 
	if ( !nsavef[NID(p)] )
		error("savenum : not implemented");
	else {
		write_short(s,&OID(p)); write_char(s,&NID(p));
		write_char(s,&(p->pad));
		(*nsavef[NID(p)])(s,p); 
	}
}

void saveq(s,p)
FILE *s;
Q p;
{
	int size[2];
	int len = 2;

	size[0] = PL(NM(p)); size[1] = DN(p) ? PL(DN(p)) : 0;
	write_intarray(s,size,len);
	write_intarray(s,BD(NM(p)),size[0]);
	if ( size[1] )
		write_intarray(s,BD(DN(p)),size[1]);
}

void savereal(s,p)
FILE *s;
Real p;
{ write_double(s,&BDY(p)); }

/*
 * BDY(p) = |  z[0]  |   z[1]  | z[2] | ... | z[lg(z)-1] |
 * -> | id(2) | nid(1) | sgn(1) | expo>>32 | expo&0xffffffff | len | ... |
 */

void savebf(s,p)
FILE *s;
BF p;
{
#if PARI
	GEN z;
	int sign;
	unsigned long expo;
	unsigned int len,t;

	z = (GEN)BDY(p);
	sign = signe(z);
	len = lg(z)-2;
	expo = expo(z);

	write_int(s,&sign);

#if defined(LONG_IS_32BIT)
	write_int(s,(int *)&zeroval); /* expo>>32 is always 0 */
	write_int(s,(int *)&expo); 
	write_int(s,&len);
	write_intarray(s,(int *)&z[2],len);
#elif defined(LONG_IS_64BIT)
	t = expo>>32; write_int(s,(int *)&t);
	t = expo&0xffffffff; write_int(s,&t);
	t = 2*len; write_int(s,&t);
	write_longarray(s,&z[2],len);
#endif
#else
	error("savebf : PARI is not combined");
#endif
}

void savecplx(s,p)
FILE *s;
C p;
{ saveobj(s,(Obj)p->r); saveobj(s,(Obj)p->i); }

void savemi(s,p)
FILE *s;
MQ p;
{ write_int(s,&CONT(p)); }

void savelm(s,p)
FILE *s;
LM p;
{
	int size;

	size = PL(BDY(p));
	write_int(s,&size);
	write_intarray(s,BD(BDY(p)),size);
}

void savegf2n(s,p)
FILE *s;
GF2N p;
{
	int len;

	len = p->body->w;
	write_int(s,&len);
	write_intarray(s,p->body->b,len);
}

void savegfpn(s,p)
FILE *s;
GFPN p;
{
	int d,i;

	d = p->body->d;
	write_int(s,&d);
	for ( i = 0; i <= d; i++ )
		saveobj(s,(Obj)p->body->c[i]);
}

void savep(s,p)
FILE *s;
P p;
{
	DCP dc;
	int n;
	int vindex;

	if ( NUM(p) )
		savenum(s,(Num)p);
	else {
		vindex = save_convv(VR(p)); 
		for ( dc = DC(p), n = 0; dc; dc = NEXT(dc), n++ );
		write_short(s,&OID(p));
		write_int(s,&vindex);
		if ( vindex < 0 )
			savepfins(s,VR(p));
		write_int(s,&n);
		for ( dc = DC(p); dc; dc = NEXT(dc) ) {
			saveobj(s,(Obj)DEG(dc)); saveobj(s,(Obj)COEF(dc));
		}
	}
}

/* save a pure function (v->attr = V_PF) */
/* |name(str)|argc(int)|darray(intarray)|args| */

void savepfins(s,v)
FILE *s;
V v;
{
	PFINS ins;
	PF pf;
	int argc,i;
	int *darray;

	ins = (PFINS)v->priv;
	pf = ins->pf;
	savestr(s,NAME(pf));
	argc = pf->argc;
	write_int(s,&argc);
	darray = (int *)ALLOCA(argc*sizeof(int));
	for ( i = 0; i < argc; i++ )
		darray[i] = ins->ad[i].d;
	write_intarray(s,darray,argc);
	for ( i = 0; i < argc; i++ )
		saveobj(s,ins->ad[i].arg);
}

void saver(s,p)
FILE *s;
R p;
{
	if ( !RAT(p) )
		savep(s,(P)p);
	else {
		write_short(s,&OID(p)); write_short(s,&p->reduced);
		savep(s,NM(p)); savep(s,DN(p));
	}
}

void savelist(s,p)
FILE *s;
LIST p;
{
	int n;
	NODE tn;

	for ( tn = BDY(p), n = 0; tn; tn = NEXT(tn), n++ );
	write_short(s,&OID(p)); write_int(s,&n);
	for ( tn = BDY(p); tn; tn = NEXT(tn) )
		saveobj(s,(Obj)BDY(tn));
}

void savevect(s,p)
FILE *s;
VECT p;
{
	int i,len = 2;

	write_short(s,&OID(p)); write_int(s,&p->len);
	for ( i = 0, len = p->len; i < len; i++ )
		saveobj(s,(Obj)BDY(p)[i]);
}

void savemat(s,p)
FILE *s;
MAT p;
{
	int i,j,row,col;
	int len = 3;

	write_short(s,&OID(p)); write_int(s,&p->row); write_int(s,&p->col);
	for ( i = 0, row = p->row, col = p->col; i < row; i++ )
		for ( j = 0; j < col; j++ )
			saveobj(s,(Obj)BDY(p)[i][j]);
}

void savestring(s,p)
FILE *s;
STRING p;
{
	write_short(s,&OID(p)); savestr(s,BDY(p));
}

void savestr(s,p)
FILE *s;
char *p;
{
	int size;

	size = p ? strlen(p) : 0; write_int(s,&size);
	if ( size )
		write_string(s,p,size);
}

void savedp(s,p)
FILE *s;
DP p;
{
	int nv,n,i,sugar;
	MP m,t;

	nv = p->nv; m = p->body; sugar = p->sugar;
	for ( n = 0, t = m; t; t = NEXT(t), n++ );	
	write_short(s,&OID(p)); write_int(s,&nv); write_int(s,&sugar); write_int(s,&n);
	for ( i = 0, t = m; i < n; i++, t = NEXT(t) ) {
		saveobj(s,(Obj)t->c);
		write_int(s,&t->dl->td); write_intarray(s,&(t->dl->d[0]),nv);
	}
}

void saveui(s,u)
FILE *s;
USINT u;
{
	write_short(s,&OID(u)); write_int(s,&BDY(u));
}

void saveerror(s,e)
FILE *s;
ERR e;
{
	write_short(s,&OID(e)); saveobj(s,(Obj)BDY(e));
}

void savegfmmat(s,p)
FILE *s;
GFMMAT p;
{
	int i,j,row,col;

	write_short(s,&OID(p)); write_int(s,&p->row); write_int(s,&p->col);
	for ( i = 0, row = p->row, col = p->col; i < row; i++ )
		write_intarray(s,p->body[i],col);
}
