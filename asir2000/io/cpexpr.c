/* $OpenXM: OpenXM/src/asir99/io/cpexpr.c,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "base.h"

#define TAIL
#define PUTS(s) (total_length+=strlen(s))
#define PRINTN length_n
/* #define PRINTBF length_bf */
#define PRINTBF 
#define PRINTCPLX length_cplx
#define PRINTV length_v
#define PRINTEXPR length_expr
#define PRINTNUM length_num
#define PRINTP length_p
#define PRINTR length_r
#define PRINTLIST length_list
#define PRINTVECT length_vect
#define PRINTMAT length_mat
#define PRINTSTR length_str
#define PRINTCOMP length_comp
#define PRINTDP length_dp

#if defined(THINK_C)
void PRINTEXPR(VL,pointer);
void PRINTNUM(Num);
void PRINTV(VL,V);
void PRINTN(N);
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
void PRINTV();
void PRINTN();
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

static int total_length;

int estimate_length(vl,p)
VL vl;
pointer p;
{
	total_length = 0;
	PRINTEXPR(vl,p);
	return total_length;
}

void PRINTEXPR(vl,p)
VL vl;
pointer p;
{
	if ( !p ) {
		total_length++;
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
	double ceil();

	if ( !n )
		PUTS("0");
	else
		total_length += (int)(ceil(0.31*((double)(BSH*PL(n))))+1);
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
			total_length += 20; /* XXX */
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
			total_length += 10; /* XXX */
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
	int t;

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
		} else if ( !pf->argc )
			PUTS(NAME(pf));
		else {
			for ( i = 0; i < pf->argc; i++ )
				if ( ad[i].d )
					break;
			if ( i < pf->argc ) {
				PUTS(NAME(pf));
				total_length += 11; /* XXX */
				for ( i = 1; i < pf->argc; i++ ) {
					total_length += 11; /* XXX */
				}
				PUTS(")(");
			} else {
				PUTS(NAME(pf));
				total_length += 1; /* XXX */
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
			total_length += 1;
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
			total_length += 11;
		}
		total_length += 10;
		PUTS(">>");
		if ( NEXT(m) )
			PUTS("+");
	}
}
