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
 * $OpenXM: OpenXM_contrib2/asir2000/io/bsave.c,v 1.19 2017/08/31 09:11:04 noro Exp $ 
*/
/* saveXXX must not use GC_malloc(), GC_malloc_atomic(). */

#include "ca.h"
#include "parse.h"
#include "com.h"

void savenbp(FILE *s,NBP p);

void (*savef[])() = { 0, savenum, savep, saver, savelist, savevect,
  savemat, savestring, 0, savedp, saveui, saveerror,0,0,0,savegfmmat,
  savebytearray, 0, 0, 0, 0, 0, 0, 0, 0,  savenbp };
#if defined(INTERVAL)
void saveitv();
void saveitvd();
void (*nsavef[])() = { saveq, savereal, 0, savebf, saveitv, saveitvd, 0, saveitv, savecplx ,savemi, savelm, savegf2n, savegfpn, savegfs, savegfsn,savedalg};
#else
void (*nsavef[])() = { saveq, savereal, 0, savebf, savecplx ,savemi, savelm, savegf2n, savegfpn, savegfs, savegfsn,savedalg};
#endif

static short zeroval = 0;

void saveobj(FILE *s,Obj p)
{
  if ( !p )
    write_short(s,&zeroval);
  else if ( !savef[OID(p)] )
    error("saveobj : not implemented");
  else
    (*savef[OID(p)])(s,p);
}

void savenum(FILE *s,Num p)
{ 
  if ( !nsavef[NID(p)] )
    error("savenum : not implemented");
  else {
    write_short(s,&OID(p)); write_char(s,&NID(p));
    write_char(s,&(p->pad));
    (*nsavef[NID(p)])(s,p); 
  }
}

void saveq(FILE *s,Q p)
{
  int size[2];
  int len = 2;

  size[0] = PL(NM(p)); size[1] = DN(p) ? PL(DN(p)) : 0;
  write_intarray(s,size,len);
  write_intarray(s,BD(NM(p)),size[0]);
  if ( size[1] )
    write_intarray(s,BD(DN(p)),size[1]);
}

void savereal(FILE *s,Real p)
{ write_double(s,&BDY(p)); }

/*
 * BDY(p) = |  z[0]  |   z[1]  | z[2] | ... | z[lg(z)-1] |
 * -> | id(2) | nid(1) | sgn(1) | expo>>32 | expo&0xffffffff | len | ... |
 */

void savebf(FILE *s,BF p)
{
  unsigned int zero = 0;
  unsigned int prec;
  L exp;
  int sgn,len,t;

  prec = MPFR_PREC(p->body);
  exp = MPFR_EXP(p->body);
  sgn = MPFR_SIGN(p->body);
  len = MPFR_LIMB_SIZE(p->body);

  write_int(s,&sgn);
  write_int(s,(int *)&prec);
  write_int64(s,(UL *)&exp);
#if defined(VISUAL)
#if !defined(_WIN64)
  write_int(s,&len);
  write_intarray(s,p->body->_mpfr_d,len);
#else
  t = (prec+31)/32; 
  write_int(s,&t);
  write_longarray(s,(long long *)p->body->_mpfr_d,t);
#endif
#else
#if SIZEOF_LONG == 4
  write_int(s,&len);
  write_intarray(s,p->body->_mpfr_d,len);
#else /* SIZEOF_LONG == 8 */
  t = (prec+31)/32; 
  write_int(s,&t);
  write_longarray(s,p->body->_mpfr_d,t);
#endif
#endif
}

#if defined(INTERVAL)
void saveitv(FILE *s,Itv p)
{
  saveobj(s,(Obj)INF(p));
  saveobj(s,(Obj)SUP(p));
}

void saveitvd(FILE *s,IntervalDouble p)
{
  write_double(s,&INF(p));
  write_double(s,&SUP(p));
}
#endif

void savecplx(FILE *s,C p)
{ saveobj(s,(Obj)p->r); saveobj(s,(Obj)p->i); }

void savemi(FILE *s,MQ p)
{ write_int(s,&CONT(p)); }

void savelm(FILE *s,LM p)
{
  int size;

  size = PL(BDY(p));
  write_int(s,&size);
  write_intarray(s,BD(BDY(p)),size);
}

void savegf2n(FILE *s,GF2N p)
{
  int len;

  len = p->body->w;
  write_int(s,&len);
  write_intarray(s,p->body->b,len);
}

void savegfpn(FILE *s,GFPN p)
{
  int d,i;

  d = p->body->d;
  write_int(s,&d);
  for ( i = 0; i <= d; i++ )
    saveobj(s,(Obj)p->body->c[i]);
}

void savegfs(FILE *s,GFS p)
{ write_int(s,&CONT(p)); }

void savegfsn(FILE *s,GFSN p)
{
  int d;

  d = DEG(BDY(p));
  write_int(s,&d);
  write_intarray(s,COEF(BDY(p)),d+1);
}

void savedalg(FILE *s,DAlg p)
{
  saveobj(s,(Obj)p->nm);
  saveobj(s,(Obj)p->dn);
}

void savep(FILE *s,P p)
{
  DCP dc;
  int n;
  int vindex;

  if ( NUM(p) )
    savenum(s,(Num)p);
  else {
    vindex = save_convv(VR(p)); 
    for ( dc = DC(p), n = 0; dc; dc = NEXT(dc), n++ );
    write_short(s,&OID(p));
    write_int(s,&vindex);
    if ( vindex < 0 )
      savepfins(s,VR(p));
    write_int(s,&n);
    for ( dc = DC(p); dc; dc = NEXT(dc) ) {
      saveobj(s,(Obj)DEG(dc)); saveobj(s,(Obj)COEF(dc));
    }
  }
}

/* save a pure function (v->attr = V_PF) */
/* |name(str)|argc(int)|darray(intarray)|args| */

void savepfins(FILE *s,V v)
{
  PFINS ins;
  PF pf;
  int argc,i;
  int *darray;

  ins = (PFINS)v->priv;
  pf = ins->pf;
  savestr(s,NAME(pf));
  argc = pf->argc;
  write_int(s,&argc);
  darray = (int *)ALLOCA(argc*sizeof(int));
  for ( i = 0; i < argc; i++ )
    darray[i] = ins->ad[i].d;
  write_intarray(s,darray,argc);
  for ( i = 0; i < argc; i++ )
    saveobj(s,ins->ad[i].arg);
}

void saver(FILE *s,R p)
{
  if ( !RAT(p) )
    savep(s,(P)p);
  else {
    write_short(s,&OID(p)); write_short(s,&p->reduced);
    savep(s,NM(p)); savep(s,DN(p));
  }
}

void savelist(FILE *s,LIST p)
{
  int n;
  NODE tn;

  for ( tn = BDY(p), n = 0; tn; tn = NEXT(tn), n++ );
  write_short(s,&OID(p)); write_int(s,&n);
  for ( tn = BDY(p); tn; tn = NEXT(tn) )
    saveobj(s,(Obj)BDY(tn));
}

void savevect(FILE *s,VECT p)
{
  int i,len = 2;

  write_short(s,&OID(p)); write_int(s,&p->len);
  for ( i = 0, len = p->len; i < len; i++ )
    saveobj(s,(Obj)BDY(p)[i]);
}

void savemat(FILE *s,MAT p)
{
  int i,j,row,col;
  int len = 3;

  write_short(s,&OID(p)); write_int(s,&p->row); write_int(s,&p->col);
  for ( i = 0, row = p->row, col = p->col; i < row; i++ )
    for ( j = 0; j < col; j++ )
      saveobj(s,(Obj)BDY(p)[i][j]);
}

void savestring(FILE *s,STRING p)
{
  write_short(s,&OID(p)); savestr(s,BDY(p));
}

void savestr(FILE *s,char *p)
{
  int size;

  size = p ? strlen(p) : 0; write_int(s,&size);
  if ( size )
    write_string(s,p,size);
}

void savedp(FILE *s,DP p)
{
  int nv,n,i,sugar;
  MP m,t;

  nv = p->nv; m = p->body; sugar = p->sugar;
  for ( n = 0, t = m; t; t = NEXT(t), n++ );  
  write_short(s,&OID(p)); write_int(s,&nv); write_int(s,&sugar); write_int(s,&n);
  for ( i = 0, t = m; i < n; i++, t = NEXT(t) ) {
    saveobj(s,(Obj)t->c);
    write_int(s,&t->dl->td); write_intarray(s,&(t->dl->d[0]),nv);
  }
}

void saveui(FILE *s,USINT u)
{
  write_short(s,&OID(u)); write_int(s,&BDY(u));
}

void saveerror(FILE *s,ERR e)
{
  write_short(s,&OID(e)); saveobj(s,(Obj)BDY(e));
}

void savegfmmat(FILE *s,GFMMAT p)
{
  int i,row,col;

  write_short(s,&OID(p)); write_int(s,&p->row); write_int(s,&p->col);
  for ( i = 0, row = p->row, col = p->col; i < row; i++ )
    write_intarray(s,p->body[i],col);
}

void savebytearray(FILE *s,BYTEARRAY p)
{
  write_short(s,&OID(p)); write_int(s,&p->len);
  write_string(s,p->body,p->len);
}

void savenbp(FILE *s,NBP p)
{
  int i,n;
  NODE t;
  NBM m;

  write_short(s,&OID(p));
  for ( n = 0, t = BDY(p); t; t = NEXT(t), n++ );
  write_int(s,&n);
  for ( i = 0, t = BDY(p); i < n; t = NEXT(t), i++ ) {
    m = (NBM)BDY(t);
    saveobj(s,(Obj)m->c);
    write_int(s,&m->d);
    write_intarray(s,m->b,(m->d+31)/32);
  }
}
