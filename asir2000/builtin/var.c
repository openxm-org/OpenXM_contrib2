/* $OpenXM: OpenXM/src/asir99/builtin/var.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Pvar(), Pvars(), Puc(), Pvars_recursive();
void get_vars(Obj,VL *);
void get_vars_recursive(Obj,VL *);

struct ftab var_tab[] = {
	{"var",Pvar,1},
	{"vars",Pvars,1},
	{"vars_recursive",Pvars_recursive,1},
	{"uc",Puc,0},
	{0,0,0},
};

void Pvar(arg,rp)
NODE arg;
Obj *rp;
{
	Obj t;
	P p;
	V vn,vd,v;
	VL vl;

	if ( !(t = (Obj)ARG0(arg)) )
		v = 0;
	else
		switch ( OID(t) ) {
			case O_P:
				v = VR((P)t); break;
			case O_R:
				vn = VR(NM((R)t)); vd = VR(DN((R)t));
				for ( vl = CO; (vl->v != vn) && (vl->v != vd); vl = NEXT(vl) );
				v = vl->v; break;
			default:
				v = 0; break;
		}
	if ( v ) {
		MKV(v,p); *rp = (Obj)p;
	} else
		*rp = 0;
}

void Pvars(arg,rp)
NODE arg;
LIST *rp;
{
	VL vl;
	NODE n,n0;
	P p;

	get_vars((Obj)ARG0(arg),&vl);
	for ( n0 = 0; vl; vl = NEXT(vl) ) {
		NEXTNODE(n0,n); MKV(vl->v,p); BDY(n) = (pointer)p;
	}
	if ( n0 )
		NEXT(n) = 0;
	MKLIST(*rp,n0);
}

void Pvars_recursive(arg,rp)
NODE arg;
LIST *rp;
{
	VL vl;
	NODE n,n0;
	P p;

	get_vars_recursive((Obj)ARG0(arg),&vl);
	for ( n0 = 0; vl; vl = NEXT(vl) ) {
		NEXTNODE(n0,n); MKV(vl->v,p); BDY(n) = (pointer)p;
	}
	if ( n0 )
		NEXT(n) = 0;
	MKLIST(*rp,n0);
}

void get_vars_recursive(obj,vlp)
Obj obj;
VL *vlp;
{
	VL vl,vl0,vl1,vl2,t;
	PFINS ins;
	int argc,i;
	PFAD ad;

	get_vars(obj,&vl);
	vl0 = 0;
	for ( t = vl; t; t = NEXT(t) )
		if ( t->v->attr == (pointer)V_PF ) {
			ins = (PFINS)t->v->priv;
			argc = ins->pf->argc;
			ad = ins->ad;
			for ( i = 0; i < argc; i++ ) {
				get_vars_recursive(ad[i].arg,&vl1);
				mergev(CO,vl0,vl1,&vl2); vl0 = vl2;
			}
		}
	mergev(CO,vl,vl0,vlp);
}

void get_vars(t,vlp)
Obj t;
VL *vlp;
{
	pointer *vb;
	pointer **mb;
	VL vl,vl1,vl2;
	NODE n;
	MP mp;
	int i,j,row,col,len;

	if ( !t )
		vl = 0;
	else
		switch ( OID(t) ) {
			case O_P: case O_R:
				clctvr(CO,t,&vl); break;
			case O_VECT:
				len = ((VECT)t)->len; vb = BDY((VECT)t);
				for ( i = 0, vl = 0; i < len; i++ ) {
					get_vars((Obj)vb[i],&vl1); mergev(CO,vl,vl1,&vl2);
					vl = vl2;
				}
				break;
			case O_MAT:
				row = ((MAT)t)->row; col = ((MAT)t)->col; mb = BDY((MAT)t);
				for ( i = 0, vl = 0; i < row; i++ )
					for ( j = 0; j < col; j++ ) {
						get_vars((Obj)mb[i][j],&vl1); mergev(CO,vl,vl1,&vl2);
						vl = vl2;
					}
				break;
			case O_LIST:
				n = BDY((LIST)t);
				for ( vl = 0; n; n = NEXT(n) ) {
					get_vars((Obj)BDY(n),&vl1); mergev(CO,vl,vl1,&vl2);
					vl = vl2;
				}
				break;
			case O_DP:
				mp = ((DP)t)->body;
				for ( vl = 0; mp; mp = NEXT(mp) ) {
					get_vars((Obj)mp->c,&vl1); mergev(CO,vl,vl1,&vl2);
					vl = vl2;
				}
				break;
			default:
				vl = 0; break;
		}
	*vlp = vl;
}

void Puc(p)
Obj *p;
{
	VL vl;
	V v;
	P t;
	char buf[BUFSIZ];
	static int UCN;

	NEWV(v); v->attr = (pointer)V_UC;
	sprintf(buf,"_%d",UCN++);
	NAME(v) = (char *)CALLOC(strlen(buf)+1,sizeof(char));
	strcpy(NAME(v),buf);
	for ( vl = CO; NEXT(vl); vl = NEXT(vl) );
	NEWVL(NEXT(vl)); VR(NEXT(vl)) = v; NEXT(NEXT(vl)) = 0;
	MKV(v,t); *p = (Obj)t;
}
