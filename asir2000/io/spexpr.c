/* $OpenXM: OpenXM/src/asir99/io/spexpr.c,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "comp.h"
#include "base.h"

#ifndef SPRINT
#define SPRINT
#endif

#ifdef FPRINT
FILE *asir_out;
#define OUT asir_out
char DFORMAT[BUFSIZ];

#define TAIL
#define PUTS(s) fputs(s,OUT)
#define PRINTF fprintf
#define PRINTN printn
#define PRINTBF printbf
#define PRINTCPLX printcplx
#define PRINTV printv
#define PRINTEXPR printexpr
#define PRINTNUM printnum
#define PRINTP printp
#define PRINTR printr
#define PRINTLIST printlist
#define PRINTVECT printvect
#define PRINTMAT printmat
#define PRINTSTR printstr
#define PRINTCOMP printcomp
#define PRINTDP printdp
#endif

#ifdef SPRINT
static char *buf;
#define OUT buf
extern char DFORMAT[BUFSIZ];

#define TAIL while ( *OUT ) OUT++;
#define PUTS(s) strcat(OUT,s)
#define PRINTF sprintf
#define PRINTN sprintn
#define PRINTBF sprintbf
#define PRINTCPLX sprintcplx
#define PRINTV sprintv
#define PRINTEXPR sprintexpr
#define PRINTNUM sprintnum
#define PRINTP sprintp
#define PRINTR sprintr
#define PRINTLIST sprintlist
#define PRINTVECT sprintvect
#define PRINTMAT sprintmat
#define PRINTSTR sprintstr
#define PRINTCOMP sprintcomp
#define PRINTDP sprintdp
#endif

#if defined(THINK_C)
void PRINTEXPR(VL,Obj);
void PRINTNUM(Num);
void PRINTN(N);
void PRINTV(VL,V);
void PRINTP(VL,P);
void PRINTR(VL,R);
void PRINTLIST(VL,LIST);
void PRINTVECT(VL,VECT);
void PRINTMAT(VL,MAT);
void PRINTSTR(STRING);
void PRINTCOMP(VL,COMP);
void PRINTDP(VL,DP);
void PRINTCPLX(C);
#else
void PRINTEXPR();
void PRINTNUM();
void PRINTN();
void PRINTV();
void PRINTP();
void PRINTR();
void PRINTLIST();
void PRINTVECT();
void PRINTMAT();
void PRINTSTR();
void PRINTCOMP();
void PRINTDP();
void PRINTCPLX();
#endif

#ifdef FPRINT
void output_init() {
	OUT = stdout;
	sprintf(DFORMAT,"%%0%dd",DLENGTH);
}

int mmono(p)
P p;
{
	if ( NUM(p) )
		if ( compnum(CO,(Num)p,0) < 0 ) 
			return ( 1 );
		else
			return ( 0 );
	else if ( NEXT(DC(p)) )
		return ( 0 );
	else
		return (mmono(COEF(DC(p))));
}

#if PARI
printbf(a)
BF a;
{
	sor(a->body,'g',-1,0);
}
#endif
#endif

#ifdef SPRINT
void soutput_init(s)
char *s;
{
	s[0] = 0; buf = s;
}

#if PARI
void sprintbf(a)
BF a;
{
	char *str;
	char *GENtostr();

	str = GENtostr(a->body);
	TAIL PRINTF(OUT,"%s",str);
	free(str);
}
#endif
#endif

void PRINTEXPR(vl,p)
VL vl;
Obj p;
{
	if ( !p ) {
		PRINTR(vl,(R)p);
		return;
	}

	switch ( OID(p) ) {
		case O_N:
			PRINTNUM((Num)p); break;
		case O_P:
			PRINTP(vl,(P)p); break;
		case O_R:
			PRINTR(vl,(R)p); break;
		case O_LIST:
			PRINTLIST(vl,(LIST)p); break;
		case O_VECT:
			PRINTVECT(vl,(VECT)p); break;
		case O_MAT:
			PRINTMAT(vl,(MAT)p); break;
		case O_STR:
			PRINTSTR((STRING)p); break;
		case O_COMP:
			PRINTCOMP(vl,(COMP)p); break;
		case O_DP:
			PRINTDP(vl,(DP)p); break;
		default:
			break;
	}
}

void PRINTN(n)
N n;
{
	register int i,*ptr;
	N tn;

	if ( !n ) {
		PUTS("0");
		return;
	}
	ntobn(DBASE,n,&tn);
	ptr = BD(tn);
	TAIL PRINTF(OUT,"%d",ptr[PL(tn) - 1]);
	for ( i = PL(tn) - 2; i >= 0; i-- ) {
		TAIL PRINTF(OUT,DFORMAT,ptr[i]);
	}
}

void PRINTNUM(q)
Num q;
{
	if ( !q ) {
		PUTS("0");
		return;
	}
	switch ( NID(q) ) {
		case N_Q:
			if ( SGN((Q)q) == -1 ) 
				PUTS("-");
			PRINTN(NM((Q)q));
			if ( !INT((Q)q) ) {
				PUTS("/"); PRINTN(DN((Q)q));
			}
			break;
		case N_R:
			TAIL PRINTF(OUT,"%g",BDY((Real)q));
			break;
		case N_A:
			PUTS("("); PRINTR(ALG,(R)BDY((Alg)q)); PUTS(")");
			break;
#if PARI
		case N_B:
			PRINTBF((BF)q); break;
#endif
		case N_C:
			PRINTCPLX((C)q); break;
		case N_M:
			TAIL PRINTF(OUT,"%d",CONT((MQ)q));
			break;
	}
}

void PRINTCPLX(a)
C a;
{
	PUTS("(");
	if ( a->r )
		PRINTNUM(a->r);
	if ( a->i ) {
		if ( a->r && (compnum(0,a->i,0) > 0) )
			PUTS("+");
		PRINTNUM(a->i); PUTS("*@i");
	}
	PUTS(")");
}

void PRINTP(vl,p)
VL vl;
P p;
{
	V v;
	DCP dc;

	if ( !p )
		PUTS("0");
	else if ( NUM(p) ) 
		PRINTNUM((Num)p);
	else
		for ( dc = DC(p), v = VR(p); dc; dc = NEXT(dc) ) {
			if ( !DEG(dc) )
				PRINTP(vl,COEF(dc));
			else {
				if ( NUM(COEF(dc)) && UNIQ((Q)COEF(dc)) ) {
					;
				} else if ( NUM(COEF(dc)) && MUNIQ((Q)COEF(dc)) )
					PUTS("-"); 
				else if ( NUM(COEF(dc)) || !NEXT(DC(COEF(dc)))) {
					PRINTP(vl,COEF(dc)); PUTS("*"); 
				} else {
					PUTS("("); PRINTP(vl,COEF(dc)); PUTS(")*"); 
				}
				PRINTV(vl,v); 
				if ( cmpq(DEG(dc),ONE) ) {
					PUTS("^"); 
					if ( INT(DEG(dc)) && SGN(DEG(dc))>0 )
						PRINTNUM((Num)DEG(dc));
					else {
						PUTS("("); PRINTNUM((Num)DEG(dc)); PUTS(")");
					}
				}
			}
			if ( NEXT(dc) ) {
				P t;

				t = COEF(NEXT(dc));
				if (!DEG(NEXT(dc))) {
					if ( NUM(t) ) {
						if ( !mmono(t) )
							PUTS("+");
					} else {
						if (!mmono(COEF(DC(t))))
							PUTS("+");
					}
				} else {
					if ( !mmono(t) )
						PUTS("+");
				}
			}
		}
}

void PRINTV(vl,v)
VL vl;
V v;
{
	PF pf;
	PFAD ad;
	int i;

	if ( NAME(v) )
		PUTS(NAME(v));
	else if ( (vid)v->attr == V_PF ) {
		pf = ((PFINS)v->priv)->pf; ad = ((PFINS)v->priv)->ad;
		if ( !strcmp(NAME(pf),"pow") ) {
			PUTS("("); PRINTR(vl,(R)ad[0].arg); PUTS(")^(");
			PRINTR(vl,(R)ad[1].arg); PUTS(")");
		} else if ( !pf->argc ) {
			TAIL PRINTF(OUT,"%s",NAME(pf));
		} else {
			for ( i = 0; i < pf->argc; i++ )
				if ( ad[i].d )
					break;
			if ( i < pf->argc ) {
				TAIL PRINTF(OUT,"%s(%d",NAME(pf),ad[0].d);
				for ( i = 1; i < pf->argc; i++ ) {
					TAIL PRINTF(OUT,",%d",ad[i].d);
				}
				PUTS(")(");
			} else {
				TAIL PRINTF(OUT,"%s(",NAME(pf));
			}
			PRINTR(vl,(R)ad[0].arg);
			for ( i = 1; i < pf->argc; i++ ) {
				PUTS(","); PRINTR(vl,(R)ad[i].arg);
			}
			PUTS(")");
		}
	}
}

void PRINTR(vl,a)
VL vl;
R a;
{
	if ( !a ) 
		PUTS("0");
	else
		switch (OID(a)) {
			case O_N: case O_P:
				PRINTP(vl,(P)a); break;
			default:
				PUTS("("); PRINTP(vl,NM((R)a)); PUTS(")/("); PRINTP(vl,DN((R)a)); PUTS(")");
				break;
		}
}

void PRINTVECT(vl,vect)
VL vl;
VECT vect;
{
	int i;
	pointer *ptr;

	PUTS("[ ");
	for ( i = 0, ptr = BDY(vect); i < vect->len; i++ ) {
		PRINTEXPR(vl,ptr[i]); PUTS(" ");
	}
	PUTS("]");
}

void PRINTMAT(vl,mat)
VL vl;
MAT mat;
{
	int i,j,r,c;
	pointer *ptr;

	for ( i = 0, r = mat->row, c = mat->col; i < r; i++ ) {
		PUTS("[ ");
		for ( j = 0, ptr = BDY(mat)[i]; j < c; j++ ) {
			PRINTEXPR(vl,ptr[j]); PUTS(" ");
		}
		PUTS("]");
		if ( i < r - 1 )
			PUTS("\n");
	}
}

void PRINTLIST(vl,list)
VL vl;
LIST list;
{
	NODE tnode;

	PUTS("[");
	for ( tnode = (NODE)list->body; tnode; tnode = NEXT(tnode) ) {
		PRINTEXPR(vl,tnode->body);
		if ( NEXT(tnode) )
			PUTS(",");
	}
	PUTS("]");
}

void PRINTSTR(str)
STRING str;
{
	char *p;

	for ( p = BDY(str); *p; p++ )
		if ( *p == '"' )
			PUTS("\"");
		else {
			TAIL PRINTF(OUT,"%c",*p);
		}
}

void PRINTCOMP(vl,c)
VL vl;
COMP c;
{
	int n,i;

	n = getcompsize((int)c->type);
	PUTS("{");
	for ( i = 0; i < n; i++ ) {
		PRINTEXPR(vl,(pointer)c->member[i]);
		if ( i < n-1 )
			PUTS(",");
	}
	PUTS("}");
}

void PRINTDP(vl,d)
VL vl;
DP d;
{
	int n,i;
	MP m;
	DL dl;

	for ( n = d->nv, m = BDY(d); m; m = NEXT(m) ) {
		PUTS("("); PRINTEXPR(vl,(pointer)m->c); PUTS(")*<<");
		for ( i = 0, dl = m->dl; i < n-1; i++ ) {
			TAIL PRINTF(OUT,"%d,",dl->d[i]);
		}
		TAIL PRINTF(OUT,"%d",dl->d[i]);
		PUTS(">>");
		if ( NEXT(m) )
			PUTS("+");
	}
}
