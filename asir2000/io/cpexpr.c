/* $OpenXM: OpenXM_contrib2/asir2000/io/cpexpr.c,v 1.1.1.1 1999/12/03 07:39:11 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "al.h"
#include "base.h"

extern int hex_output,fortran_output;

#define PRINTHAT (fortran_output?PUTS("**"):PUTS("^"))

#define TAIL
#define PUTS(s) (total_length+=strlen(s))
#define PRINTN length_n
#define PRINTBF length_bf
#define PRINTCPLX length_cplx
#define PRINTLM length_lm
#define PRINTUP2 length_up2
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
#define PRINTUI length_ui
#define PRINTGF2MAT length_gf2mat
#define PRINTGFMMAT length_gfmmat
#define PRINTERR length_err
#define PRINTLF length_lf
#define PRINTLOP length_lop
#define PRINTFOP length_fop
#define PRINTEOP length_eop
#define PRINTQOP length_qop
#define PRINTUP length_up

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
void PRINTUI();
void PRINTGF2MAT();
void PRINTGFMMAT();
void PRINTERR();
void PRINTCPLX();
void PRINTLM();
void PRINTLF();
void PRINTUP2();

static int total_length;

int estimate_length(vl,p)
VL vl;
pointer p;
{
	total_length = 0;
	PRINTEXPR(vl,p);
	return total_length;
}

#if PARI
void PRINTBF(a)
BF a;
{
	char *str;
	char *GENtostr();

	str = GENtostr(a->body);
	total_length += strlen(str);
	free(str);
}
#endif

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
		case O_USINT:
			PRINTUI(vl,(USINT)p); break;
		case O_GF2MAT:
			PRINTGF2MAT(vl,(GF2MAT)p); break;
		case O_ERR:
			PRINTERR(vl,(ERR)p); break;
		case O_MATHCAP:
			PRINTLIST(vl,((MATHCAP)p)->body); break;
		case O_F:
			PRINTLF(vl,(F)p); break;
		case O_GFMMAT:
			PRINTGFMMAT(vl,(GFMMAT)p); break;
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
	else if ( hex_output )
		total_length += 2+(int)(PL(n)*8);
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
			total_length += 11; /* XXX */
			break;
		case N_LM:
			PRINTN(((LM)q)->body); break;
		case N_GF2N:
			if ( hex_output )
				PRINTN((N)(((GF2N)q)->body));
			else
				PRINTUP2(((GF2N)q)->body);
			break;
		case N_GFPN:
			PRINTUP((UP)(((GFPN)q)->body));
			break;
		default:
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
					PRINTHAT;
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

extern int hideargs;

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
			PUTS("("); PRINTR(vl,(R)ad[0].arg); PUTS(")"); PRINTHAT; PUTS("(");
			PRINTR(vl,(R)ad[1].arg); PUTS(")");
		} else if ( !pf->argc )
			PUTS(NAME(pf));
		else {
			if ( hideargs ) {
				for ( i = 0; i < pf->argc; i++ )
					if ( ad[i].d )
						break;
				if ( i < pf->argc ) {
					PUTS(NAME(pf));	
					total_length += 11; /* XXX */
					for ( i = 1; i < pf->argc; i++ ) {
						total_length += 11; /* XXX */
					}
					PUTS("}");
				} else {
					PUTS(NAME(pf));
					total_length += 1; /* XXX */
				}
			} else {
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

void PRINTUI(vl,u)
VL vl;
USINT u;
{
	total_length += 10;
}

void PRINTGF2MAT(vl,mat)
VL vl;
GF2MAT mat;
{
	int row,col,w,i,j,k,m;
	unsigned int t;
	unsigned int **b;

	row = mat->row;
	col = mat->col;
	w = (col+BSH-1)/BSH;
	b = mat->body;
	for ( i = 0; i < row; i++ ) {
		for ( j = 0, m = 0; j < w; j++ ) {
			t = b[i][j];
			for ( k = 0; m < col && k < BSH; k++, m++ )
				if ( t & (1<<k) )
					PUTS("1");
				else
					PUTS("0");
		}
		PUTS("\n");
	}
}

void PRINTGFMMAT(vl,mat)
VL vl;
GFMMAT mat;
{
	int row,col,i,j;
	unsigned int t;
	unsigned int **b;

	row = mat->row;
	col = mat->col;
	b = mat->body;
	for ( i = 0; i < row; i++ ) {
		PUTS("[");
		for ( j = 0; j < col; j++ ) {
			total_length += 10; /* XXX */
		}
		PUTS("]\n");
	}
}

void PRINTERR(vl,e)
VL vl;
ERR e;
{
	PUTS("error("); PRINTEXPR(vl,e->body); PUTS(")");
}

void PRINTUP2(p)
UP2 p;
{
	int d,i;

	if ( !p ) {
		PUTS("0");
	} else {
		d = degup2(p);
		PUTS("(");
		if ( !d ) {
			PUTS("1");
		} else if ( d == 1 ) {
			PUTS("@");
		} else {
			PRINTHAT;
			total_length += 11;
		}
		for ( i = d-1; i >= 0; i-- ) {
			if ( p->b[i/BSH] & (1<<(i%BSH)) )
				if ( !i ) {
					PUTS("+1");
				} else if ( i == 1 ) {
					PUTS("+@");
				} else {
					PRINTHAT;
					total_length += 12;
				}
		}
		PUTS(")");
	}
}

void PRINTLF(vl,f)
VL vl;
F f;
{
	switch ( FOP(f) ) {
		case AL_TRUE:
			PUTS("@true");
			break;
		case AL_FALSE:
			PUTS("@false");
			break;

		case AL_OR: case AL_AND:
			PRINTFOP(vl,f); break;
		case AL_NOT: case AL_IMPL: case AL_REPL: case AL_EQUIV:
			PRINTEOP(vl,f); break;

		case AL_EQUAL: case AL_NEQ: case AL_LESSP:
		case AL_GREATERP: case AL_LEQ: case AL_GEQ:
			PRINTLOP(vl,f); break;

		case AL_EX: case AL_ALL:
			PRINTQOP(vl,f); break;
		default:
			break;
	}
}

PRINTFOP(vl,f)
VL vl;
F f;
{
	char *op;
	NODE n;

	op = FOP(f)==AL_OR?" @|| ":" @&& ";
	n = FJARG(f);
	PUTS("("); PRINTEXPR(vl,BDY(n)); PUTS(")");
	for ( n = NEXT(n); n; n = NEXT(n) ) {
		PUTS(op); PUTS("("); PRINTEXPR(vl,BDY(n)); PUTS(")");
	}
}

PRINTEOP(vl,f)
VL vl;
F f;
{
	oFOP op;
	char *sop;

	if ( (op = FOP(f)) == AL_NOT ) {
		PUTS("(@! "); PRINTEXPR(vl,(Obj)FARG(f)); PUTS(")"); return;
	}
	switch ( op ) {
		case AL_IMPL:
			sop = " @impl "; break;
		case AL_REPL:
			sop = " @repl "; break;
		case AL_EQUIV:
			sop = " @equiv "; break;
		default:
			break;
	}
	PUTS("("); 
	PRINTEXPR(vl,(Obj)FLHS(f)); 
	PUTS(sop);
	PRINTEXPR(vl,(Obj)FRHS(f)); 
	PUTS(")");
}

PRINTLOP(vl,f)
VL vl;
F f;
{
	char *op;

	switch ( FOP(f) ) {
		case AL_EQUAL:
			op = " @== "; break;
		case AL_NEQ:
			op = " @!= "; break;
		case AL_LESSP:
			op = " @< "; break;
		case AL_GREATERP:
			op = " @> "; break;
		case AL_LEQ:
			op = " @<= "; break;
		case AL_GEQ:
			op = " @>= "; break;
		default:
			error("PRINTLOP : invalid operator");
			break;
	}
	PRINTEXPR(vl,(Obj)FPL(f)); PUTS(op); PUTS("0");
}

PRINTQOP(vl,f)
VL vl;
F f;
{
	char *op;

	op = FOP(f)==AL_EX?"ex":"all";
	PUTS(op); PUTS(NAME(FQVR(f)));
	total_length += 2;
	PRINTEXPR(vl,(Obj)FQMAT(f)); PUTS(")");
}

PRINTUP(n)
UP n;
{
	int i,d;

	if ( !n )
		PUTS("0");
	else if ( !n->d )
		PRINTNUM(n->c[0]);
	else {
		d = n->d;
		PUTS("(");
		if ( !d ) {
			PRINTNUM(n->c[d]);
		} else if ( d == 1 ) {
			PRINTNUM(n->c[d]);
			PUTS("*@p");
		} else {
			PRINTNUM(n->c[d]);
			PRINTHAT;
			total_length += 13;
		}
		for ( i = d-1; i >= 0; i-- ) {
			if ( n->c[i] ) {
				PUTS("+("); PRINTNUM(n->c[i]); PUTS(")");
				if ( i >= 2 ) {
					PRINTHAT;
					total_length += 13;
				} else if ( i == 1 )
					PUTS("*@p");
			}
		}
		PUTS(")");
	}
}
