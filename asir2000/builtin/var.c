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
 *
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/var.c,v 1.8 2015/12/02 13:12:31 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Pvar(), Pvars(), Puc(), Pvars_recursive(),Psimple_is_eq();
void Pdelete_uc();

struct ftab var_tab[] = {
	{"var",Pvar,1},
	{"vars",Pvars,1},
	{"vars_recursive",Pvars_recursive,1},
	{"uc",Puc,0},
	{"delete_uc",Pdelete_uc,-1},
	{"simple_is_eq",Psimple_is_eq,2},
	{0,0,0},
};

void Psimple_is_eq(NODE arg,Q *rp)
{
	int ret;

	ret = is_eq(ARG0(arg),ARG1(arg));
	STOQ(ret,*rp);
}

int is_eq(Obj a0,Obj a1)
{
	P p0,p1;
	DCP dc0,dc1;

	if ( !a0 ) return a1?0:1;
	else if ( !a1 ) return 0;
	else if ( OID(a0) != OID(a1) ) return 0;
	else {
		switch ( OID(a0) ) {
			case O_P:
				p0 = (P)a0; p1 = (P)a1;
				if ( VR(p0) == VR(p1) ) {
					for ( dc0 = DC(p0), dc1 = DC(p1); dc0 && dc1; dc0 = NEXT(dc0), dc1 = NEXT(dc1) ) {
						if ( cmpq(DEG(dc0),DEG(dc1)) ) return 0;
						if ( !is_eq((Obj)COEF(dc0),(Obj)COEF(dc1)) ) return 0;
					}
					return (dc0||dc1)?0:1;
				} else return 0;
				break;
			default:
				return !arf_comp(CO,a0,a1);
				break;
		}
	}
}

void Pvar(NODE arg,Obj *rp)
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

void Pvars(NODE arg,LIST *rp)
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

void Pvars_recursive(NODE arg,LIST *rp)
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

void get_vars_recursive(Obj obj,VL *vlp)
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

void get_vars(Obj t,VL *vlp)
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
			case O_NBP:
				n = BDY((NBP)t);
				for ( vl = 0; n; n = NEXT(n) ) {
					get_vars((Obj)(((NBM)BDY(n))->c),&vl1); 
					mergev(CO,vl,vl1,&vl2);
					vl = vl2;
				}
				break;
			default:
				vl = 0; break;
		}
	*vlp = vl;
}

void Puc(Obj *p)
{
	VL vl;
	V v;
	P t;
	char buf[BUFSIZ];
	char *n,*nv;
	static int UCN;

	NEWV(v); v->attr = (pointer)V_UC;
	sprintf(buf,"_%d",UCN++);
	nv = NAME(v) = (char *)CALLOC(strlen(buf)+1,sizeof(char));
	strcpy(NAME(v),buf);
	for ( vl = CO; vl; vl = NEXT(vl) )
		if ( (n=NAME(VR(vl))) && !strcmp(n,nv) ) break;
		else if ( !NEXT(vl) ) {
			NEWVL(NEXT(vl)); VR(NEXT(vl)) = v; NEXT(NEXT(vl)) = 0;
			LASTCO = NEXT(vl);
			break;
		}
	MKV(v,t); *p = (Obj)t;
}

void Pdelete_uc(NODE arg,Obj *p)
{
  VL vl,prev;
  V v;

  if ( argc(arg) == 1 ) {
    asir_assert(ARG0(arg),O_P,"delete_uc");
    v = VR((P)ARG0(arg));
  } else
    v = 0;

  for ( prev = 0, vl = CO; vl; vl = NEXT(vl) ) {
    if ( (!v || v == vl->v) && vl->v->attr == (pointer)V_UC ) {
      if ( prev == 0 )
        CO = NEXT(vl); 
      else 
        NEXT(prev) = NEXT(vl);
    } else
      prev = vl;
  }
  update_LASTCO();
  *p = 0;
}
