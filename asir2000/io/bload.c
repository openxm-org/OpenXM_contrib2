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
 * $OpenXM: OpenXM_contrib2/asir2000/io/bload.c,v 1.20 2019/11/12 10:52:04 kondoh Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "com.h"
#include "ox.h"

extern VL file_vl;

void loadnbp(FILE *s,NBP *p);

void (*loadf[])() = { 0, loadnum, loadp, loadr, loadlist, loadvect, loadmat,
  loadstring, 0, loaddp, loadui, loaderror,0,0,0,loadgfmmat, 
  loadbytearray, 0, 0, 0, 0, 0, 0, 0, 0,  loadnbp };

#if defined(INTERVAL)
void loaditv();
void loaditvd();
void (*nloadf[])() = { loadq, loadreal, 0, loadbf, loaditv, loaditvd, 0, loaditv, loadcplx, loadmi, loadlm, loadgf2n, loadgfpn, loadgfs, loadgfsn, loaddalg };
#else
void (*nloadf[])() = { loadq, loadreal, 0, loadbf, loadcplx, loadmi, loadlm, loadgf2n, loadgfpn, loadgfs, loadgfsn, loaddalg };
#endif

void loadobj(FILE *s,Obj *p)
{
  short id;

  read_short(s,&id);
  if ( !id )
    *p = 0;
  else if ( !loadf[id] )
    error("loadobj : not implemented");
  else
    (*loadf[id])(s,p);
}

void loadnum(FILE *s,Num *p)
{ 
  char nid;

  read_char(s,&nid);
  if ( !nloadf[nid] )
    error("loadnum : not implemented");
  else
    (*nloadf[nid])(s,p); 
}

void loadq(FILE *s,Q *p)
{
  int size[2];
  char sgn;
  int len = 2;
  N nm,dn;

  read_char(s,&sgn); read_intarray(s,size,len);
  nm = NALLOC(size[0]); PL(nm) = size[0];
  read_intarray(s,BD(nm),size[0]);
  if ( size[1] ) {
    dn = NALLOC(size[1]); PL(dn) = size[1];
    read_intarray(s,BD(dn),size[1]);
  } else
    dn = 0;
  NDTOQ(nm,dn,sgn,*p);
}

void loadreal(FILE *s,Real *p)
{
  Real q;
  char dmy;

  read_char(s,&dmy);
  NEWReal(q); read_double(s,&BDY(q));
  *p = q;
}

void loadbf(FILE *s,BF *p)
{
  BF r;
  char dmy;
  int sgn,prec;
  UL exp;

  int len;
  read_char(s,&dmy);
  NEWBF(r);
  read_int(s,&sgn);
  read_int(s,&prec);
  read_int64(s,&exp);
  read_int(s,&len);
  mpfr_init2(r->body,prec);
  MPFR_SIGN(r->body) = sgn;
  MPFR_EXP(r->body) = (int)exp;
#if defined(VISUAL)
#if !defined(_WIN64)
  read_intarray(s,(int *)r->body->_mpfr_d,len);
#else
  read_longarray(s,(long long*)r->body->_mpfr_d,len);
#endif
#else
#if SIZEOF_LONG == 4
  read_intarray(s,(int *)r->body->_mpfr_d,len);
#else /* SIZEOF_LONG == 8 */
  read_longarray(s,(long *)r->body->_mpfr_d,len);
#endif
#endif
  *p = r;
}

#if defined(INTERVAL)
void loaditv(FILE *s,Itv *p)
{
  Itv q;
  char dmy;

  read_char(s,&dmy);
  NEWItvP(q); loadobj(s,(Obj *)&INF(q)); loadobj(s,(Obj *)&SUP(q));
  *p = q;
}

void loaditvd(FILE *s,IntervalDouble *p)
{
  IntervalDouble q;
  char dmy;

  read_char(s,&dmy);
  NEWIntervalDouble(q);
  read_double(s,&INF(q));
  read_double(s,&SUP(q));
  *p = q;
}
#endif

void loadcplx(FILE *s,C *p)
{
  C q;
  char dmy;

  read_char(s,&dmy);
  NEWC(q); loadobj(s,(Obj *)&q->r); loadobj(s,(Obj *)&q->i);
  *p = q;
}

void loadmi(FILE *s,MQ *p)
{
  MQ q;
  char dmy;

  read_char(s,&dmy);
  NEWMQ(q); read_int(s,(int *)&CONT(q));
  *p = q;
}

void loadlm(FILE *s,LM *p)
{
  int size;
  char dmy;
  N body;

  read_char(s,&dmy); read_int(s,&size);
  body = NALLOC(size); PL(body) = size;
  read_intarray(s,BD(body),size);
  MKLM(body,*p);
}

void loadgf2n(FILE *s,GF2N *p)
{
  char dmy;
  int len;
  UP2 body;

  read_char(s,&dmy); read_int(s,&len);
  NEWUP2(body,len); body->w = len;
  read_intarray(s,body->b,len);
  MKGF2N(body,*p);
}

void loadgfpn(FILE *s,GFPN *p)
{
  char dmy;
  int d,i;
  UP body;

  read_char(s,&dmy); read_int(s,&d);
  body = UPALLOC(d);
  body->d = d;
  for ( i = 0; i <= d; i++ )
    loadobj(s,(Obj *)&body->c[i]);
  MKGFPN(body,*p);
}

void loadgfs(FILE *s,GFS *p)
{
  GFS q;
  char dmy;

  read_char(s,&dmy);
  NEWGFS(q); read_int(s,(int *)&CONT(q));
  *p = q;
}

void loadgfsn(FILE *s,GFSN *p)
{
  char dmy;
  int d;
  UM body;

  read_char(s,&dmy); read_int(s,&d);
  body = UMALLOC(d); DEG(body) = d;
  read_intarray(s,COEF(body),d+1);
  MKGFSN(body,*p);
}

void loaddalg(FILE *s,DAlg *p)
{
  char dmy;
  Obj nm,dn;

  read_char(s,&dmy);
  loadobj(s,&nm);
  loadobj(s,&dn);
  MKDAlg((DP)nm,(Q)dn,*p);
}

void loadp(FILE *s,P *p)
{
  V v;
  int n,vindex;
  DCP dc,dc0;
  P t;

  read_int(s,&vindex);
  if ( vindex < 0 )
  /* v is a pure function */
    v = loadpfins(s);
  else 
    v = (V)load_convv(vindex);
  read_int(s,&n);
  for ( dc0 = 0; n; n-- ) {
    NEXTDC(dc0,dc); loadobj(s,(Obj *)&DEG(dc)); loadobj(s,(Obj *)&COEF(dc));
  }
  NEXT(dc) = 0;
  MKP(v,dc0,t);
  if ( vindex < 0 || file_vl )
    reorderp(CO,file_vl,t,p);
  else
    *p = t;
}

/* |name(str)|argc(int)|darray(intarray)|args| */

V loadpfins(FILE *s)
{
  char *name;
  FUNC fp;
  int argc,i;
  V v;
  int *darray;
  Obj *args;
  PF pf;
  char *buf;
  V *a;
  P u;

  loadstr(s,&name);
  read_int(s,&argc);
  searchpf(name,&fp);
  if ( fp ) {
    pf = fp->f.puref;
    if ( pf->argc != argc )
      error("loadpfins : argument mismatch");
  } else {
    a = (V *)MALLOC(argc*sizeof(V));
    buf = (char *)ALLOCA(BUFSIZ);
    for ( i = 0; i < argc; i++ ) {
      sprintf(buf,"_%c",'a'+i);
      makevar(buf,&u); a[i] = VR(u);
    }
#if defined(INTERVAL)
    mkpf(name,0,argc,a,0,0,0,0,&pf);
#else
    mkpf(name,0,argc,a,0,0,0,&pf);
#endif
  }
  darray = (int *)ALLOCA(argc*sizeof(int));
  args = (Obj *)ALLOCA(argc*sizeof(int));
  read_intarray(s,darray,argc);
  for ( i = 0; i < argc; i++ )
    loadobj(s,&args[i]);
  _mkpfins_with_darray(pf,args,darray,&v);
  return v;
}

void loadr(FILE *s,R *p)
{
  R r;

  NEWR(r); read_short(s,&r->reduced);
  loadobj(s,(Obj *)&NM(r)); loadobj(s,(Obj *)&DN(r)); *p = r;
}

void loadlist(FILE *s,LIST *p)
{
  int n;
  NODE tn,tn0;

  read_int(s,&n);
  for ( tn0 = 0; n; n-- ) {
    NEXTNODE(tn0,tn); loadobj(s,(Obj *)&BDY(tn));
  }
  if ( tn0 )
    NEXT(tn) = 0;
  MKLIST(*p,tn0);
}

void loadvect(FILE *s,VECT *p)
{
  int i,len;
  VECT vect;

  read_int(s,&len); MKVECT(vect,len);
  for ( i = 0; i < len; i++ )
    loadobj(s,(Obj *)&BDY(vect)[i]);
  *p = vect;
}

void loadmat(FILE *s,MAT *p)
{
  int row,col,i,j;
  MAT mat;

  read_int(s,&row); read_int(s,&col); MKMAT(mat,row,col);
  for ( i = 0; i < row; i++ )
    for ( j = 0; j < col; j++ )
      loadobj(s,(Obj *)&BDY(mat)[i][j]);
  *p = mat;
}

void loadstring(FILE *s,STRING *p)
{
  char *t;

  loadstr(s,&t); MKSTR(*p,t);
}

void loadstr(FILE *s,char **p)
{
  int len;
  char *t;

  read_int(s,&len);
  if ( len ) {
    t = (char *)MALLOC(len+1); read_string(s,t,len); t[len] = 0;
  } else
    t = "";
  *p = t;
}

void loadbytearray(FILE *s,BYTEARRAY *p)
{
  int len;
  BYTEARRAY array;

  read_int(s,&len);
  MKBYTEARRAY(array,len);
  if ( len ) {
    read_string(s,array->body,len);
  }
  *p = array;
}

void loaddp(FILE *s,DP *p)
{
  int nv,n,i,sugar;
  DP dp;
  MP m,m0;
  DL dl;

  read_int(s,&nv); read_int(s,&sugar); read_int(s,&n);
  for ( i = 0, m0 = 0; i < n; i++ ) {
    NEXTMP(m0,m);
    loadobj(s,(Obj *)&(m->c));
    NEWDL(dl,nv); m->dl = dl;
    read_int(s,&dl->td); read_intarray(s,&(dl->d[0]),nv);
  }
  NEXT(m) = 0; MKDP(nv,m0,dp); dp->sugar = sugar; *p = dp;
}

void loadui(FILE *s,USINT *u)
{
  unsigned int b;

  read_int(s,&b); MKUSINT(*u,b);
}

void loaderror(FILE *s,ERR *e)
{
  Obj b;

  loadobj(s,&b); MKERR(*e,b);
}


void loadgfmmat(FILE *s,GFMMAT *p) 
{
  int i,row,col;
  unsigned int **a;
  GFMMAT mat;

  read_int(s,&row); read_int(s,&col);
  a = (unsigned int **)almat(row,col);
  TOGFMMAT(row,col,a,mat);
  for ( i = 0; i < row; i++ )
    read_intarray(s,a[i],col);
  *p = mat;
}

void loadnbp(FILE *s,NBP *p)
{
  int n,i;
  NBM m;
  NODE r0,r;

  read_int(s,&n);
  for ( i = 0, r0 = 0; i < n; i++ ) {
    NEWNBM(m);
    loadobj(s,(Obj *)&m->c);
    read_int(s,&m->d); 
    NEWNBMBDY(m,m->d); read_intarray(s,m->b,(m->d+31)/32);
    NEXTNODE(r0,r); BDY(r) = (pointer)m;
  }
  if ( r0 ) NEXT(r) = 0;
  MKNBP(*p,r0);
}
