/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 * $OpenXM: OpenXM_contrib2/asir2000/io/cpexpr.c,v 1.20 2004/03/17 02:23:45 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "al.h"
#include "base.h"
#if PARI
#include "genpari.h"
#endif

#ifndef CPRINT
#define CPRINT
#endif

extern int hex_output,fortran_output,double_output,real_digit;
extern int hideargs,outputstyle;
static int total_length;

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
#define PRINTBYTEARRAY length_bytearray
#define PRINTQUOTE length_QUOTE
#define PRINTSYMBOL length_SYMBOL
#define PRINTRANGE length_RANGE
#define PRINTTB length_TB
#define PRINTERR length_err
#define PRINTLF length_lf
#define PRINTLOP length_lop
#define PRINTFOP length_fop
#define PRINTEOP length_eop
#define PRINTQOP length_qop
#define PRINTUP length_up
#define PRINTUM length_um
#define PRINTSF length_sf
#define PRINTFARGS length_fargs
#define PRINTFNODENODE length_fnodenode
#define PRINTFNODE length_fnode

#include "pexpr_body.c"

/* special functions for estimating length */

static int total_length;

int estimate_length(VL vl,pointer p)
{
	total_length = 0;
	PRINTEXPR(vl,p);
	return total_length;
}

#if defined(PARI)
void PRINTBF(BF a)
{
	char *str;
	char *GENtostr0();
	void myoutbrute();

	if ( double_output ) {
		str = GENtostr0(a->body,myoutbrute);
	} else {
		str = GENtostr(a->body);
	}
	total_length += strlen(str);
	free(str);
}
#endif

void PRINTNUM(Num q)
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
			if ( double_output )
				total_length += 400+real_digit; /* XXX */
			else
				total_length += 20+real_digit; /* XXX */
			break;
		case N_A:
			PUTS("("); PRINTR(ALG,(R)BDY((Alg)q)); PUTS(")");
			break;
#if defined(PARI)
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
		case N_GFS:
			total_length += 13; /* XXX */
			break;
		case N_GFSN:
			PRINTUM(BDY((GFSN)q));
			break;
		default:
			break;
	}
}

void PRINTV(VL vl,V v)
{
	PF pf;
	PFAD ad;
	int i;

	if ( NAME(v) )
		PUTS(NAME(v));
	else if ( (vid)v->attr == V_PF ) {
		pf = ((PFINS)v->priv)->pf; ad = ((PFINS)v->priv)->ad;
		if ( !strcmp(NAME(pf),"pow") ) {
			PUTS("(("); PRINTR(vl,(R)ad[0].arg); PUTS(")"); PRINTHAT; PUTS("(");
			PRINTR(vl,(R)ad[1].arg); PUTS("))");
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

void PRINTN(N n)
{
	double ceil();

	if ( !n )
		PUTS("0");
	else if ( hex_output )
		total_length += 2+(int)(PL(n)*8);
	else
		total_length += (int)(ceil(0.31*((double)(BSH*PL(n))))+1);
}

void PRINTSTR(STRING str)
{
	char *p;

	for ( p = BDY(str); *p; p++ )
		if ( *p == '"' )
			PUTS("\"");
		else {
			total_length += 1;
		}
}

void PRINTDP(VL vl,DP d)
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

void PRINTUI(VL vl,USINT u)
{
	total_length += 10;
}

void PRINTGFMMAT(VL vl,GFMMAT mat)
{
	int row,col,i,j;
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

void PRINTBYTEARRAY(VL vl,BYTEARRAY array)
{
	/* |xx xx ... xx| */
	total_length += 1+3*array->len;
}

extern int print_quote;

void PRINTQUOTE(VL vl,QUOTE quote)
{
	LIST list;

	if ( print_quote == 2 ) {
		PRINTFNODE(BDY(quote),0);
	} else if ( print_quote == 1 ) {
		/* XXX */
		fnodetotree(BDY(quote),&list);
		PRINTEXPR(vl,(Obj)list);
	} else {
		/* <...quoted...> */
		total_length += 20;
	}
}

void PRINTSYMBOL(SYMBOL sym)
{
	total_length += strlen(sym->name);
}

void PRINTTB(VL vl,TB p)
{
	int i;

	for ( i = 0; i < p->next; i++ ) {
		total_length += strlen(p->body[i]);
	}
}

void PRINTUP2(UP2 p)
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

void PRINTQOP(VL vl,F f)
{
	char *op;

	op = FOP(f)==AL_EX?"ex":"all";
	PUTS(op); PUTS(NAME(FQVR(f)));
	total_length += 2;
	PRINTEXPR(vl,(Obj)FQMAT(f)); PUTS(")");
}

void PRINTUP(UP n)
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

void PRINTUM(UM n)
{
	int i,d;

	if ( !n )
		PUTS("0");
	else if ( !n->d )
		PRINTSF(n->c[0]);
	else {
		d = n->d;
		PUTS("(");
		if ( !d ) {
			PRINTSF(n->c[d]);
		} else if ( d == 1 ) {
			PRINTSF(n->c[d]);
			PUTS("*@s");
		} else {
			PRINTSF(n->c[d]);
			PUTS("*@s"); PRINTHAT; total_length += 13;
		}
		for ( i = d-1; i >= 0; i-- ) {
			if ( n->c[i] ) {
				PUTS("+("); PRINTSF(n->c[i]); PUTS(")");
				if ( i >= 2 ) {
					PUTS("*@s"); PRINTHAT; total_length += 13;
				} else if ( i == 1 )
					PUTS("*@s");
			}
		}
		PUTS(")");
	}
}

void PRINTSF(unsigned int i)
{
	if ( !i ) {
		PUTS("0");
	} else
		total_length += 15;

#ifndef CPRINT
#define CPRINT
#endif
}
