/* $OpenXM: OpenXM/src/asir99/builtin/cplxnum.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Pconj(),Preal(),Pimag();

#if defined(THINK_C)
void cplx_conj(Obj,Obj *);
void cplx_real(Obj,Obj *);
void cplx_imag(Obj,Obj *);
#else
void cplx_conj();
void cplx_real();
void cplx_imag();
#endif

struct ftab cplx_tab[] = {
	{"conj",Pconj,1},
	{"real",Preal,1},
	{"imag",Pimag,1},
	{0,0,0},
};

void Pconj(arg,rp)
NODE arg;
Obj *rp;
{
	cplx_conj((Obj)ARG0(arg),rp);
}

void Preal(arg,rp)
NODE arg;
Obj *rp;
{
	cplx_real((Obj)ARG0(arg),rp);
}

void Pimag(arg,rp)
NODE arg;
Obj *rp;
{
	cplx_imag((Obj)ARG0(arg),rp);
}

void cplx_conj(p,r)
Obj p;
Obj *r;
{
	C c;
	DCP dc,dcr,dcr0;
	P t;

	if (  !p )
		*r = 0;
	else
		switch ( OID(p) ) {
			case O_N:
				if ( NID((Num)p) <= N_B )
					*r = p;
				else {
					NEWC(c); c->r = ((C)p)->r; chsgnnum(((C)p)->i,&c->i);
					*r = (Obj)c;
				}
				break;
			case O_P:
				for ( dcr0 = 0, dc = DC((P)p); dc; dc = NEXT(dc) ) {
					NEXTDC(dcr0,dcr); cplx_conj((Obj)COEF(dc),(Obj *)&COEF(dcr));
					DEG(dcr) = DEG(dc);
				}
				NEXT(dcr) = 0; MKP(VR((P)p),dcr0,t); *r = (Obj)t;
				break;
			default:
				error("cplx_conj : not implemented"); break;
		}
}

void cplx_real(p,r)
Obj p;
Obj *r;
{
	DCP dc,dcr,dcr0;
	P t;

	if (  !p )
		*r = 0;
	else
		switch ( OID(p) ) {
			case O_N:
				if ( NID((Num)p) <= N_B )
					*r = p;
				else
					*r = (Obj)((C)p)->r;
				break;
			case O_P:
				for ( dcr0 = 0, dc = DC((P)p); dc; dc = NEXT(dc) ) {
					cplx_real((Obj)COEF(dc),(Obj *)&t);
					if ( t ) {
						NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
					}
				}
				if ( !dcr0 )
					*r = 0;
				else {
					NEXT(dcr) = 0; MKP(VR((P)p),dcr0,t); *r = (Obj)t;
				}
				break;
			default:
				error("cplx_real : not implemented"); break;
		}
}

void cplx_imag(p,r)
Obj p;
Obj *r;
{
	DCP dc,dcr,dcr0;
	P t;

	if (  !p )
		*r = 0;
	else
		switch ( OID(p) ) {
			case O_N:
				if ( NID((Num)p) <= N_B )
					*r = 0;
				else
					*r = (Obj)((C)p)->i;
				break;
			case O_P:
				for ( dcr0 = 0, dc = DC((P)p); dc; dc = NEXT(dc) ) {
					cplx_imag((Obj)COEF(dc),(Obj *)&t);
					if ( t ) {
						NEXTDC(dcr0,dcr); DEG(dcr) = DEG(dc); COEF(dcr) = t;
					}
				}
				if ( !dcr0 )
					*r = 0;
				else {
					NEXT(dcr) = 0; MKP(VR((P)p),dcr0,t); *r = (Obj)t;
				}
				break;
			default:
				error("cplx_imag : not implemented"); break;
		}
}
