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
 * $OpenXM$
*/
#include "ca.h"
#include "parse.h"
#include "ox.h"
#if !defined(VISUAL) && !defined(__MINGW32__)
#include <ctype.h>
#endif

#define ISIZ sizeof(int)

int valid_as_cmo(Obj obj)
{
  NODE m;
  int nid;

  if ( !obj )
    return 1;
  switch ( OID(obj) ) {
    case O_MATHCAP: case O_P: case O_R: case O_DP: case O_STR:
    case O_ERR: case O_USINT: case O_BYTEARRAY: case O_VOID:
      return 1;
    case O_N:
      nid = NID((Num)obj);
      if ( nid == N_Q || nid == N_R || nid == N_B || nid == N_C )
        return 1;
      else
        return 0;
    case O_LIST:
      for ( m = BDY((LIST)obj); m; m = NEXT(m) )
        if ( !valid_as_cmo(BDY(m)) )
          return 0;
      return 1;
    case O_QUOTE:
      return 1;
    default:
      return 0;
  }
}

void write_cmo(FILE *s,Obj obj)
{
  int r;
  char errmsg[BUFSIZ];
  LIST l;

  if ( !obj ) {
    r = CMO_NULL; write_int(s,&r);
    return;
  }
  switch ( OID(obj) ) {
    case O_N:
      switch ( NID((Num)obj) ) {
        case N_Q:
          write_cmo_q(s,(Q)obj);
          break;
        case N_R:
          write_cmo_real(s,(Real)obj);
          break;
        case N_B:
          write_cmo_bf(s,(BF)obj);
          break;
        case N_C:
          write_cmo_complex(s,(C)obj);
          break;
        default:
          sprintf(errmsg, "write_cmo : number id=%d not implemented.",
            NID((Num)obj));
          error(errmsg);
          break;
      }
      break;
    case O_P:
      write_cmo_p(s,(P)obj);
      break;
    case O_R:
      write_cmo_r(s,(R)obj);
      break;
    case O_DP:
      write_cmo_dp(s,(DP)obj);
      break;
    case O_LIST:
      write_cmo_list(s,(LIST)obj);
      break;
    case O_STR:
      write_cmo_string(s,(STRING)obj);
      break;
    case O_USINT:
      write_cmo_uint(s,(USINT)obj);
      break;
    case O_MATHCAP:
      write_cmo_mathcap(s,(MATHCAP)obj);
      break;
    case O_ERR:
      write_cmo_error(s,(ERR)obj);
      break;
    case O_BYTEARRAY:
      write_cmo_bytearray(s,(BYTEARRAY)obj);
      break;
    case O_VOID:
      r = ((USINT)obj)->body; write_int(s,&r);
      break;
    case O_QUOTE:
      fnodetotree(BDY((QUOTE)obj),&l);
      write_cmo_tree(s,l);
      break;
    case O_MAT:
      write_cmo_matrix_as_list(s,(MAT)obj);
      break;
    default:
      sprintf(errmsg, "write_cmo : id=%d not implemented.",OID(obj));
      error(errmsg);
      break;
  }
}

int cmo_tag(Obj obj,int *tag)
{
  if ( !valid_as_cmo(obj) )
    return 0;
  if ( !obj ) {
    *tag = CMO_NULL;
    return 1;
  }
  switch ( OID(obj) ) {
    case O_N:
      switch ( NID((Num)obj) ) {
        case N_Q:
          *tag = !INT((Q)obj) ? CMO_QQ : CMO_ZZ; break;
        case N_R:
          *tag = CMO_IEEE_DOUBLE_FLOAT; break;
        case N_B:
          *tag = CMO_BIGFLOAT32; break;
        case N_C:
          *tag = CMO_COMPLEX; break;
        default:
          return 0;
      }
      break;
    case O_P:
      *tag = CMO_RECURSIVE_POLYNOMIAL; break;
    case O_R:
      *tag = CMO_RATIONAL; break;
    case O_DP:
      *tag = CMO_DISTRIBUTED_POLYNOMIAL; break;
    case O_LIST:
      *tag = CMO_LIST; break;
    case O_STR:
      *tag = CMO_STRING; break;
    case O_USINT:
      *tag = CMO_INT32; break;
    case O_MATHCAP:
      *tag = CMO_MATHCAP; break;
    case O_ERR:
      *tag = CMO_ERROR2; break;
    case O_QUOTE:
      *tag = CMO_TREE; break; break;
    default:
      return 0;
  }
  return 1;
}

void write_cmo_mathcap(FILE *s,MATHCAP mc)
{
  unsigned int r;

  r = CMO_MATHCAP; write_int(s,&r);
  write_cmo(s,(Obj)BDY(mc));
}

void write_cmo_uint(FILE *s,USINT ui)
{
  unsigned int r;

  r = CMO_INT32; write_int(s,&r);
  r = ui->body; write_int(s,&r);
}

void write_cmo_q(FILE *s,Q q)
{
  int r;
  Z nm,dn;

  if ( q && !INT(q) ) {
    r = CMO_QQ; write_int(s,&r);
    nmq(q,&nm);
    write_cmo_zz(s,nm);
    nmq(q,&dn);
    write_cmo_zz(s,dn);
  } else {
    r = CMO_ZZ; write_int(s,&r);
    write_cmo_zz(s,(Z)q);  
  }
}

void write_cmo_real(FILE *s,Real real)
{
  unsigned int r;
  double dbl;

  r = CMO_IEEE_DOUBLE_FLOAT; write_int(s,&r);
  dbl = real->body; write_double(s,&dbl);
}

void write_cmo_bf(FILE *s,BF bf)
{
  unsigned int r;
  int len_r,len;
  unsigned int *ptr;

  r = CMO_BIGFLOAT32; write_int(s,&r);
  r = MPFR_PREC(bf->body); write_int(s,&r);
  r = MPFR_SIGN(bf->body); write_int(s,&r);
  r = MPFR_EXP(bf->body);  write_int(s,&r);
  len_r = MPFR_LIMB_SIZE_REAL(bf->body);
  len   = MPFR_LIMB_SIZE_BODY(bf->body);
  write_int(s,&len);
  ptr = (unsigned int *)MPFR_MANT(bf->body);
  write_intarray(s,ptr+(len_r-len),len);
}

void write_cmo_zz(FILE *s,Z n)
{
  size_t l;
  int *b;
  int bytes;

  b = (int *)mpz_export(0,&l,-1,sizeof(int),0,0,BDY(n));
  bytes = sgnz(n)*l;
  write_int(s,&bytes);
  write_intarray(s,b,l);
}

void write_cmo_p(FILE *s,P p)
{
  int r,i;
  VL t,vl;
  char *namestr;
  STRING name;

  r = CMO_RECURSIVE_POLYNOMIAL; write_int(s,&r);
  get_vars((Obj)p,&vl);

  /* indeterminate list */
  r = CMO_LIST; write_int(s,&r);
  for ( t = vl, i = 0; t; t = NEXT(t), i++ );
  write_int(s,&i);
  r = CMO_INDETERMINATE;
  for ( t = vl; t; t = NEXT(t) ) {
    write_int(s,&r);
/*    localname_to_cmoname(NAME(t->v),&namestr); */
    namestr = NAME(t->v);
    MKSTR(name,namestr);
    write_cmo(s,(Obj)name);
  }

  /* body */
  write_cmo_upoly(s,vl,p);
}

void write_cmo_upoly(FILE *s,VL vl,P p)
{
  int r,i;
  V v;
  DCP dc,dct;
  VL vlt;

  if ( NUM(p) )
    write_cmo(s,(Obj)p);
  else {
    r = CMO_UNIVARIATE_POLYNOMIAL; write_int(s,&r);
    v = VR(p);
    dc = DC(p);
    for ( i = 0, dct = dc; dct; dct = NEXT(dct), i++ );
    write_int(s,&i);
    for ( i = 0, vlt = vl; vlt->v != v; vlt = NEXT(vlt), i++ );
    write_int(s,&i);
    for ( dct = dc; dct; dct = NEXT(dct) ) {
      i = QTOS(DEG(dct)); write_int(s,&i);
      write_cmo_upoly(s,vl,COEF(dct));
    }
  }
}

void write_cmo_r(FILE *s,R f)
{
  int r;

  r = CMO_RATIONAL; write_int(s,&r);
  write_cmo(s,(Obj)NM(f));
  write_cmo(s,(Obj)DN(f));
}

void write_cmo_complex(FILE *s,C f)
{
  int r;

  r = CMO_COMPLEX; write_int(s,&r);
  write_cmo(s,(Obj)f->r);
  write_cmo(s,(Obj)f->i);
}

void write_cmo_dp(FILE *s,DP dp)
{
  int i,n,nv,r;
  MP m;

  for ( n = 0, m = BDY(dp); m; m = NEXT(m), n++ );
  r = CMO_DISTRIBUTED_POLYNOMIAL; write_int(s,&r);
  r = n; write_int(s,&r);
  r = CMO_DMS_GENERIC; write_int(s,&r);
  nv = dp->nv;
  for ( i = 0, m = BDY(dp); i < n; i++, m = NEXT(m) )
    write_cmo_monomial(s,m,nv);
}

void write_cmo_monomial(FILE *s,MP m,int n)
{
  int i,r;
  int *p;

  r = CMO_MONOMIAL32; write_int(s,&r);
  write_int(s,&n);
  for ( i = 0, p = m->dl->d; i < n; i++ ) {
    write_int(s,p++);
  }
  write_cmo_q(s,(Q)m->c);
}

void write_cmo_list(FILE *s,LIST list)
{
  NODE m;
  int i,n,r;

  for ( n = 0, m = BDY(list); m; m = NEXT(m), n++ );
  r = CMO_LIST; write_int(s,&r);
  write_int(s,&n);
  for ( i = 0, m = BDY(list); i < n; i++, m = NEXT(m) )
    write_cmo(s,BDY(m));
}

void write_cmo_string(FILE *s,STRING str)
{
  int r;

  r = CMO_STRING; write_int(s,&r);
  savestr(s,BDY(str));  
}

void write_cmo_bytearray(FILE *s,BYTEARRAY array)
{
  int r;

  r = CMO_DATUM; write_int(s,&r);
  write_int(s,&array->len);
  write_string(s,array->body,array->len);
}

void write_cmo_error(FILE *s,ERR e)
{
  int r;

  r = CMO_ERROR2; write_int(s,&r);
  write_cmo(s,BDY(e));
}

/* XXX */

/*
 * BDY(l) = treenode
 * treenode = [property,(name,)arglist]
 * arglist = list of treenode
 */

void write_cmo_tree(FILE *s,LIST l)
{
  NODE n;
  int r;
  STRING prop,name,key;

  /* (CMO_TREE (CMO_LIST,n,key1,attr1,...,keyn,attn),(CMO_LIST,m,arg1,...,argm)) */
  n = BDY(l);
  prop = (STRING)BDY(n);  n = NEXT(n);
  if ( !strcmp(BDY(prop),"internal") ) {
    write_cmo(s,(Obj)BDY(n));
  } else {
    if ( strcmp(BDY(prop),"list") ) {
      r = CMO_TREE; write_int(s,&r);
      name = (STRING)BDY(n);
      n = NEXT(n);
      /* function name */
      write_cmo(s,(Obj)name);

      /* attribute list */
      r = CMO_LIST; write_int(s,&r);
      r = 2; write_int(s,&r);
      MKSTR(key,"asir");
      write_cmo(s,(Obj)key);
      write_cmo(s,(Obj)prop);
    }

    /* argument list */
    r = CMO_LIST; write_int(s,&r);
    /* len = number of arguments */
    r = length(n); write_int(s,&r);
    while ( n ) {
      write_cmo_tree(s,BDY(n));
      n = NEXT(n);
    }
  }
}

void write_cmo_matrix_as_list(FILE *s,MAT a)
{
  int i,j,r,row,col;

  /* CMO_LIST row (CMO_LIST col a[0][0] ... a[0][col-1]) ... (CMO_LIST col a[row-1][0] ... a[row-1][col-1] */
  row = a->row; col = a->col;
  r = CMO_LIST;
  write_int(s,&r);
  write_int(s,&row);
  for ( i = 0; i < row; i++ ) {
    write_int(s,&r);
    write_int(s,&col);
    for ( j = 0; j < col; j++ )
      write_cmo(s,a->body[i][j]);
  }
}

void read_cmo(FILE *s,Obj *rp)
{
  int id;
  int sgn,dummy;
  Q q;
  Z nm,dn;
  P p,pnm,pdn;
  Real real;
  C c;
  double dbl;
  STRING str;
  USINT t;
  DP dp;
  Obj obj;
  ERR e;
  BF bf;
  MATHCAP mc;
  BYTEARRAY array;
  LIST list;

  read_int(s,&id);
  switch ( id ) {
  /* level 0 objects */
    case CMO_NULL:
      *rp = 0;
      break;
    case CMO_INT32:
      read_cmo_uint(s,&t); *rp = (Obj)t;
      break;
    case CMO_DATUM:
      loadbytearray(s,&array); *rp = (Obj)array;
      break;
    case CMO_STRING:
      loadstring(s,&str); *rp = (Obj)str;
      break;
    case CMO_MATHCAP:
      read_cmo(s,&obj); MKMATHCAP(mc,(LIST)obj);
      *rp = (Obj)mc;
      break;
    case CMO_ERROR:
      MKERR(e,0); *rp = (Obj)e;
      break;
    case CMO_ERROR2:
      read_cmo(s,&obj); MKERR(e,obj); *rp = (Obj)e;
      break;
  /* level 1 objects */
    case CMO_LIST:
      read_cmo_list(s,rp);
      break;
    case CMO_MONOMIAL32:
      read_cmo_monomial(s,&dp); *rp = (Obj)dp;
      break;
    case CMO_ZZ:
      read_cmo_zz(s,&nm); *rp = (Obj)nm;
      break;
    case CMO_QQ:
      read_cmo_zz(s,&nm);
      read_cmo_zz(s,&dn);
      divq((Q)nm,(Q)dn,&q); *rp = (Obj)q;
      break;
    case CMO_IEEE_DOUBLE_FLOAT:
      read_double(s,&dbl); MKReal(dbl,real); *rp = (Obj)real;
      break;
    case CMO_BIGFLOAT32:
      read_cmo_bf(s,&bf); *rp = (Obj)bf;
      break;
    case CMO_COMPLEX:
      NEWC(c);
      read_cmo(s,(Obj *)&c->r);
      read_cmo(s,(Obj *)&c->i);
      *rp = (Obj)c;
      break;
    case CMO_DISTRIBUTED_POLYNOMIAL:
      read_cmo_dp(s,&dp); *rp = (Obj)dp;
      break;
    case CMO_RECURSIVE_POLYNOMIAL:
      read_cmo_p(s,&p); *rp = (Obj)p;
      break;
    case CMO_UNIVARIATE_POLYNOMIAL:
      read_cmo_upoly(s,&p); *rp = (Obj)p;
      break;
    case CMO_INDETERMINATE:
      read_cmo(s,rp);
      break;
    case CMO_RATIONAL:
      read_cmo(s,&obj); pnm = (P)obj;
      read_cmo(s,&obj); pdn = (P)obj;
      divr(CO,(Obj)pnm,(Obj)pdn,rp);
      break;
    case CMO_ZERO:
      *rp = 0;
      break;
    case CMO_DMS_OF_N_VARIABLES:
      read_cmo(s,rp);
      break;
    case CMO_RING_BY_NAME:
      read_cmo(s,rp);
      break;
    case CMO_TREE:
      read_cmo_tree_as_list(s,&list);
#if 0
      treetofnode(list,&fn);
      MKQUOTE(quote,fn);
      *rp = (Obj)quote;
#else
      *rp = (Obj)list;
#endif
      break;
    default:
      MKUSINT(t,id);
      t->id = O_VOID;
      *rp = (Obj)t;
      break;
  }
}

void read_cmo_uint(FILE *s,USINT *rp)
{
  unsigned int body;

  read_int(s,&body);
  MKUSINT(*rp,body);
}

void read_cmo_zz(FILE *s,Z *rp)
{
  int l,sgn;
  int *b;
  mpz_t z;

  read_int(s,&l);
  if ( l == 0 ) {
    *rp = 0;
    return;
  }
  if ( l < 0 ) {
    sgn = -1; l = -l;
  } else
    sgn = 1; 
  b = (int *)MALLOC(l*sizeof(int));
  read_intarray(s,b,l);
  mpz_init(z);
  mpz_import(z,l,-1,sizeof(int),0,0,b);
  if ( sgn < 0 ) mpz_neg(z,z);
  MPZTOZ(z,*rp);
}

void read_cmo_bf(FILE *s,BF *bf)
{
  BF r;
  int sgn,prec,exp,len,len_r;
  unsigned int *ptr;

  NEWBF(r);
  read_int(s,&prec);
  read_int(s,&sgn);
  read_int(s,&exp);
  read_int(s,&len);
  mpfr_init2(r->body,prec);
  MPFR_SIGN(r->body) = sgn;
  MPFR_EXP(r->body) = exp;
  *(MPFR_MANT(r->body)) = 0;
  ptr = (unsigned int *)MPFR_MANT(r->body);
  len_r = MPFR_LIMB_SIZE_REAL(r->body);
  read_intarray(s,ptr+(len_r-len),len);
  *bf = r;
}

void read_cmo_list(FILE *s,Obj *rp)
{
  int len;
  Obj *w;
  int i;
  NODE n0,n1;
  LIST list;

  read_int(s,&len);
  w = (Obj *)ALLOCA(len*sizeof(Obj));
  for ( i = 0; i < len; i++ )
    read_cmo(s,&w[i]);    
  for ( i = len-1, n0 = 0; i >= 0; i-- ) {
    MKNODE(n1,w[i],n0); n0 = n1;
  }
  MKLIST(list,n0);
  *rp = (Obj)list;
}

void read_cmo_dp(FILE *s,DP *rp)
{
  int len;
  int i;
  MP mp0,mp;
  int nv,d;
  DP dp;
  Obj obj;

  read_int(s,&len);
  /* skip the ring definition */
  read_cmo(s,&obj);
  for ( mp0 = 0, i = 0, d = 0; i < len; i++ ) {
    read_cmo(s,&obj); dp = (DP)obj;
    if ( !mp0 ) {
      nv = dp->nv;
      mp0 = dp->body;
      mp = mp0;
    } else {
      NEXT(mp) = dp->body;
      mp = NEXT(mp);
    }
    d = MAX(d,dp->sugar);
  }
  MKDP(nv,mp0,dp);
  dp->sugar = d; *rp = dp;
}

void read_cmo_monomial(FILE *s,DP *rp)
{
  Obj obj;
  MP m;
  DP dp;
  int i,sugar,n;
  DL dl;

  read_int(s,&n);
  NEWMP(m); NEWDL(dl,n); m->dl = dl;
  read_intarray(s,dl->d,n);
  for ( sugar = 0, i = 0; i < n; i++ )
    sugar += dl->d[i];
  dl->td = sugar;
  read_cmo(s,&obj); m->c = obj;
  NEXT(m) = 0; MKDP(n,m,dp); dp->sugar = sugar; *rp = dp;
}

static V *remote_vtab;

void read_cmo_p(FILE *s,P *rp)
{
  Obj obj;
  LIST vlist;
  int nv,i;
  V *vtab;
  V v1,v2;
  NODE t;
  P v,p;
  VL tvl,rvl;
  char *name;
  FUNC f;

  read_cmo(s,&obj); vlist = (LIST)obj;
  nv = length(BDY(vlist));
  vtab = (V *)ALLOCA(nv*sizeof(V));
  for ( i = 0, t = BDY(vlist); i < nv; t = NEXT(t), i++ ) {
/*    cmoname_to_localname(BDY((STRING)BDY(t)),&name); */
    name = BDY((STRING)BDY(t));
    gen_searchf_searchonly(name,&f,1);
    if ( f )
      makesrvar(f,&v);
    else
      makevar(name,&v);
    vtab[i] = VR(v);
  }
  remote_vtab = vtab;
  read_cmo(s,&obj); p = (P)obj;
  for ( i = 0; i < nv-1; i++ ) {
    v1 = vtab[i]; v2 = vtab[i+1];
    for ( tvl = CO; tvl->v != v1 && tvl->v != v2; tvl = NEXT(tvl) );
    if ( tvl->v == v2 )
      break;
  }
  if ( i < nv-1 ) {
     for ( i = nv-1, rvl = 0; i >= 0; i-- ) {
      NEWVL(tvl); tvl->v = vtab[i]; NEXT(tvl) = rvl; rvl = tvl;
     }
     reorderp(CO,rvl,p,rp);
  } else
    *rp = p;
}

void read_cmo_upoly(FILE *s,P *rp)
{
  int n,ind,i,d;
  Obj obj;
  P c;
  Z q;
  DCP dc0,dc;

  read_int(s,&n);
  read_int(s,&ind);
  for ( i = 0, dc0 = 0; i < n; i++ ) {
    read_int(s,&d);
    read_cmo(s,&obj); c = (P)obj;
    if ( c ) {
      if ( OID(c) == O_USINT ) {
        UTOQ(((USINT)c)->body,q); c = (P)q;
      }
      NEXTDC(dc0,dc); 
      STOQ(d,q);
      dc->c = c; dc->d = q;
    }
  }
  if ( dc0 )
    NEXT(dc) = 0;
  MKP(remote_vtab[ind],dc0,*rp);
}

/* XXX */

extern struct oARF arf[];

struct operator_tab {
  char *name;
  fid id;  
  ARF arf;
  cid cid;
};

static struct operator_tab optab[] = {
  {"+",I_BOP,&arf[0],0}, /* XXX */
  {"-",I_BOP,&arf[1],0},
  {"*",I_BOP,&arf[2],0},
  {"/",I_BOP,&arf[3],0},
  {"%",I_BOP,&arf[4],0},
  {"^",I_BOP,&arf[5],0},
  {"==",I_COP,0,C_EQ},
  {"!=",I_COP,0,C_NE},
  {"<",I_COP,0,C_LT},
  {"<=",I_COP,0,C_LE},
  {">",I_COP,0,C_GT},
  {">=",I_COP,0,C_GE},
  {"&&",I_AND,0,0},
  {"||",I_OR,0,0},
  {"!",I_NOT,0,0},
};

static int optab_len = sizeof(optab)/sizeof(struct operator_tab);

#if 0
/* old code */
void read_cmo_tree(s,rp)
FILE *s;
FNODE *rp;
{
  int r,i,n;
  char *opname;
  STRING name,cd;
  int op;
  pointer *arg;
  QUOTE quote;
  FNODE fn;
  NODE t,t1;
  fid id;
  Obj expr;
  FUNC func;

  read_cmo(s,&name);
  read_cmo(s,&attr);
  for ( i = 0; i < optab_len; i++ )
    if ( !strcmp(optab[i].name,BDY(name)) )
      break;
  if ( i == optab_len ) {
    /* may be a function name */
    n = read_cmo_tree_arg(s,&arg);
    for ( i = n-1, t = 0; i >= 0; i-- ) {
      MKNODE(t1,arg[i],t); t = t1;    
    }
    searchf(sysf,BDY(name),&func); 
    if ( !func )
      searchf(ubinf,BDY(name),&func);
    if ( !func )
      searchpf(BDY(name),&func);
    if ( !func )
      searchf(usrf,BDY(name),&func);
    if ( !func )
      appenduf(BDY(name),&func);
    *rp = mkfnode(2,I_FUNC,func,mkfnode(1,I_LIST,t));
  } else {
    opname = optab[i].name;
    id = optab[i].id;
    switch ( id ) {
      case I_BOP:
        read_cmo_tree_arg(s,&arg);
        *rp = mkfnode(3,I_BOP,optab[i].arf,arg[0],arg[1]);
        return;
      case I_COP:
        read_cmo_tree_arg(s,&arg);
        *rp = mkfnode(3,I_COP,optab[i].cid,arg[0],arg[0]);
        return;
      case I_AND:
        read_cmo_tree_arg(s,&arg);
        *rp = mkfnode(2,I_AND,arg[0],arg[1]);
        return;
      case I_OR:
        read_cmo_tree_arg(s,&arg);
        *rp = mkfnode(2,I_OR,arg[0],arg[1]);
        return;
      case I_NOT:
        read_cmo_tree_arg(s,&arg);
        *rp = mkfnode(1,I_OR,arg[0]);
        return;
    }
  }
}

int read_cmo_tree_arg(s,argp)
FILE *s;
pointer **argp;
{
  int id,n,i;
  pointer *ap;
  Obj t;

  read_int(s,&id); /* id = CMO_LIST */
  read_int(s,&n); /* n = the number of args */
  *argp = ap = (pointer *) MALLOC(n*sizeof(pointer));
  for ( i = 0; i < n; i++ ) {
    read_cmo(s,&t);
    if ( !t || (OID(t) != O_QUOTE) )
      ap[i] = mkfnode(1,I_FORMULA,t);
    else
      ap[i] = BDY((QUOTE)t);
  }
  return n;
}
#else
void read_cmo_tree_as_list(FILE *s,LIST *rp)
{
  Obj obj;
  STRING name;
  LIST attr,args;
  NODE t0,t1;

  read_cmo(s,&obj); name = (STRING)obj;
  read_cmo(s,&obj); attr = (LIST)obj;
  read_cmo(s,&obj); args = (LIST)obj;
  MKNODE(t1,name,BDY(args));
  MKNODE(t0,attr,t1);
  MKLIST(*rp,t0);
}
#endif

void localname_to_cmoname(char *a,char **b)
{
  int l;
  char *t;

  l = strlen(a);
  if ( l >= 2 && a[0] == '@' && isupper(a[1]) ) {
    t = *b = (char *)MALLOC_ATOMIC(l);
    strcpy(t,a+1);
  } else {
    t = *b = (char *)MALLOC_ATOMIC(l+1);
    strcpy(t,a);
  }
}

void cmoname_to_localname(char *a,char **b)
{
  int l;
  char *t;

  l = strlen(a);
  if ( isupper(a[0]) ) {
    t = *b = (char *)MALLOC_ATOMIC(l+2);
    strcpy(t+1,a);
    t[0] = '@';
  } else {
    t = *b = (char *)MALLOC_ATOMIC(l+1);
    strcpy(t,a);
  }
}
