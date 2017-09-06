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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/subst.c,v 1.10 2015/09/24 04:43:13 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Psubst(), Ppsubst(), Psubstf(), Psubst_quote();
void Psubstr2np();

struct ftab subst_tab[] = {
	{"subst",Psubst,-99999999},
	{"substr2np",Psubstr2np,-3},
	{"subst_quote",Psubst_quote,-99999999},
	{"psubst",Ppsubst,-99999999},
	{"substf",Psubstf,-99999999},
	{0,0,0},
};

extern Obj VOIDobj;

/* substr2np(P,[[v,a],...]) or substr2np(P,[v1,...],[a1,...]) */

void Psubstr2np(NODE arg,Obj *rp)
{
  Obj a,b;
  P nm,dn,p,q;
  R r;
  VL vl,tvl;
  int nv,i,ac,j;
  NODE slist,t,ps,u,vlist;
  P s;
  P *svect;
  V v;
  V *vvect;

  a = (Obj)ARG0(arg);
  if ( !a || NUM(a) ) {
    *rp = a;
    return;
  }
  asir_assert(ARG0(arg),O_R,"substr2np");
  asir_assert(ARG1(arg),O_LIST,"substr2np");
  get_vars(a,&vl);
  for ( i = 0, tvl = vl; tvl; tvl = NEXT(tvl), i++ );
  nv = i;
  vvect = (V *)MALLOC((nv)*sizeof(V));
  for ( i = 0, tvl = vl; tvl; tvl = NEXT(tvl), i++ ) vvect[i] = tvl->v;
  svect = (P *)MALLOC((nv)*sizeof(P));
  ac = argc(arg);
  if ( ac == 2 ) {
    slist = BDY((LIST)ARG1(arg));
    for ( i = 0; i < nv; i++ ) svect[i] = (P)VOIDobj;
    for ( t = slist; t; t = NEXT(t) ) {
      ps = BDY((LIST)BDY(t)); p = (P)BDY(ps); s = (P)BDY(NEXT(ps));  
      asir_assert(p,O_P,"substr2np"); asir_assert(s,O_P,"substr2np");
      v = VR(p);
      for ( i = 0; i < nv; i++ ) if ( vvect[i] == v ) break;
      svect[i] = s;
    }
  } else if ( ac == 3 ) {
    asir_assert(ARG2(arg),O_LIST,"substr2np");
    vlist = BDY((LIST)ARG1(arg));
    slist = BDY((LIST)ARG2(arg));
    for ( i = 0; i < nv; i++ ) svect[i] = (P)VOIDobj;
      for ( u = vlist, t = slist; u && t; u = NEXT(u), t = NEXT(t) ) {
        v = VR((P)BDY(u));
        for ( i = 0; i < nv; i++ ) if ( vvect[i] == v ) break;
        svect[i] = (P)BDY(t);
      }
    } else
      error("substr2np : argument mismatch");
  for ( i = 0; i < nv; i++ ) {
    if ( (int)(vvect[i]->attr) == V_PF ) {
      MKV(vvect[i],p);
      for ( j = 0; j < nv; j++ )
        if ( j != i ) {
          substr(CO,0,(Obj)p,vvect[j],(Obj)svect[j],&b); p = (P)b;
        }
      if ( OID(svect[i]) == O_VOID ) svect[i] = p;
      else if ( arf_comp(CO,(Obj)p,(Obj)svect[i]) )
        error("substr2np : inconsistent values for substitution");
    }
  }
  switch ( OID(a) ) {
    case O_P:
      substpp(CO,(P)a,vvect,svect,nv,&nm); *rp = (Obj)nm;
      return;
    case O_R:
      substpp(CO,(P)NM((R)a),vvect,svect,nv,&nm);
      substpp(CO,(P)DN((R)a),vvect,svect,nv,&dn);
      if ( !dn )
        error("substr2np: division by 0");
      else if ( !nm )
        *rp = 0;
      else if ( NUM(dn) ) {
        divsp(CO,nm,dn,&p);
        *rp = (Obj)p;
      } else {
        MKRAT(nm,dn,0,r);
        *rp = (Obj)r;
      }
      return;
    default:
      error("substr2np: invalid argument");
  }
}

void Psubst(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,b,t;
	LIST l;
	V v;
	int row,col,len;
	VECT vect;
	MAT mat;
	int i,j;
	NODE n0,n,nd;
	struct oNODE arg0;
	MP m,mp,mp0;
	DP d;

	if ( !arg ) {
		*rp = 0; return;
	}
	a = (Obj)ARG0(arg);
	if ( !a ) {
		*rp = 0;
		return;
	}
	switch ( OID(a) ) {
		case O_N: case O_P: case O_R:
			reductr(CO,(Obj)ARG0(arg),&a);
			arg = NEXT(arg);
			if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
				arg = BDY(l);
			while ( arg ) {
				if ( !BDY(arg) || OID((Obj)BDY(arg)) != O_P )
					error("subst : invalid argument");
				v = VR((P)BDY(arg)); arg = NEXT(arg);
				if ( !arg )
					error("subst : invalid argument");
				asir_assert(ARG0(arg),O_R,"subst");
				reductr(CO,(Obj)BDY(arg),&b); arg = NEXT(arg);
		/*		b = (Obj)BDY(arg); arg = NEXT(arg); */
				substr(CO,0,a,v,b,&t); a = t;
			}
			*rp = a;
			break;
		case O_LIST:
			n0 = 0;
			for ( nd = BDY((LIST)a); nd; nd = NEXT(nd) ) {
				NEXTNODE(n0,n);
				arg0.body = (pointer)BDY(nd);
				arg0.next = NEXT(arg);
				Psubst(&arg0,&b);
				BDY(n) = (pointer)b;
			}
			if ( n0 )
				NEXT(n) = 0;
			MKLIST(l,n0);
			*rp = (Obj)l;
			break;
		case O_VECT:
			len = ((VECT)a)->len;
			MKVECT(vect,len);
			for ( i = 0; i < len; i++ ) {
				arg0.body = (pointer)BDY((VECT)a)[i];
				arg0.next = NEXT(arg);
				Psubst(&arg0,&b);
				BDY(vect)[i] = (pointer)b;
			}
			*rp = (Obj)vect;
			break;
		case O_MAT:
			row = ((MAT)a)->row;
			col = ((MAT)a)->col;
			MKMAT(mat,row,col);
			for ( i = 0; i < row; i++ )
				for ( j = 0; j < col; j++ ) {
					arg0.body = (pointer)BDY((MAT)a)[i][j];
					arg0.next = NEXT(arg);
					Psubst(&arg0,&b);
					BDY(mat)[i][j] = (pointer)b;
				}
			*rp = (Obj)mat;
			break;
		case O_DP:
			mp0 = 0;
			for ( m = BDY((DP)a); m; m = NEXT(m) ) {
				arg0.body = (pointer)C(m);
				arg0.next = NEXT(arg);
				Psubst(&arg0,&b);
				if ( b ) {
					NEXTMP(mp0,mp);
					C(mp) = (P)b;
					mp->dl = m->dl;
				}
			}
			if ( mp0 ) {
				MKDP(NV((DP)a),mp0,d);
				d->sugar = ((DP)a)->sugar;
				*rp = (Obj)d;
			} else
				*rp = 0;

			break;
		default:
			error("subst : invalid argument");
	}
}

FNODE subst_in_fnode();

void Psubst_quote(arg,rp)
NODE arg;
QUOTE *rp;
{
	QUOTE h;
	FNODE fn;
	Obj g;
	LIST l;
	V v;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_QUOTE,"subst_quote");
	fn = BDY((QUOTE)ARG0(arg)); arg = NEXT(arg);
	if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
		arg = BDY(l);
	while ( arg ) {
		asir_assert(BDY(arg),O_P,"subst_quote");
		v = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( !arg )
			error("subst_quote : invalid argument");
		g = (Obj)ARG0(arg); arg = NEXT(arg);
		if ( !g || OID(g) != O_QUOTE )
			objtoquote(g,&h);
		else
			h = (QUOTE)g;
		fn = subst_in_fnode(fn,v,BDY(h));
	}
	MKQUOTE(*rp,fn);
}

void Ppsubst(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,b,t;
	LIST l;
	V v;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_R,"psubst");
	reductr(CO,(Obj)ARG0(arg),&a);
/*	a = (Obj)ARG0(arg); */
	arg = NEXT(arg);
	if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
		arg = BDY(l);
	while ( arg ) {
		asir_assert(BDY(arg),O_P,"psubst");
		v = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( !arg )
			error("psubst : invalid argument");
		asir_assert(ARG0(arg),O_R,"psubst");
		reductr(CO,(Obj)BDY(arg),&b); arg = NEXT(arg);
/*		b = (Obj)BDY(arg); arg = NEXT(arg); */
		substr(CO,1,a,v,b,&t); a = t;
	}
	*rp = a;
}

void Psubstf(arg,rp)
NODE arg;
Obj *rp;
{
	Obj a,t;
	LIST l;
	V v,f;

	if ( !arg ) {
		*rp = 0; return;
	}
	asir_assert(ARG0(arg),O_R,"substf");
	reductr(CO,(Obj)ARG0(arg),&a);
/*	a = (Obj)ARG0(arg); */
	arg = NEXT(arg);
	if ( arg && (l = (LIST)ARG0(arg)) && OID(l) == O_LIST )
		arg = BDY(l);
	while ( arg ) {
		asir_assert(BDY(arg),O_P,"substf");
		v = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( !arg || (int)v->attr != V_SR )
			error("substf : invalid argument");
		f = VR((P)BDY(arg)); arg = NEXT(arg);
		if ( (int)f->attr != V_SR )
			error("substf : invalid argument\n");
		substfr(CO,a,((FUNC)v->priv)->f.puref,((FUNC)f->priv)->f.puref,&t);
		a = t;
	}
	*rp = a;
}
