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
 * $OpenXM$
*/
#include "ca.h"
#include "base.h"

extern int lm_lazy;

GEN_UP2 current_mod_gf2n;

void setmod_gf2n(P p)
{
  if ( !current_mod_gf2n ) {
    current_mod_gf2n = (GEN_UP2)MALLOC(sizeof(struct oGEN_UP2));
    current_mod_gf2n->id = UP2_SPARSE; /* use sparse rep. by default */
  }
  ptoup2(p,&current_mod_gf2n->dense);
  ptoup2_sparse(p,&current_mod_gf2n->sparse);
}

void getmod_gf2n(UP2 *p)
{
  if ( current_mod_gf2n )
    *p = current_mod_gf2n->dense;
  else
    *p = 0;
}

void simpgf2n(GF2N n,GF2N *r)
{
  UP2 rem;

  if ( !n )
    *r = 0;
  else if ( NID(n) != N_GF2N )
    *r = n;
  else {
    gen_simpup2(n->body,current_mod_gf2n,&rem);
    MKGF2N(rem,*r);
  }
}

void ptogf2n(Obj q,GF2N *l)
{
  UP2 q1;

  if ( !q || (OID(q)==O_N && NID(q)==N_GF2N) ) {
    *l = (GF2N)q;
  } else if ( (OID(q)==O_N && NID(q)==N_Q) || OID(q)==O_P ) {
    ptoup2((P)q,&q1);
    MKGF2N(q1,*l);
  } else
    error("ptogf2n : invalid argument");
}

void gf2ntop(GF2N q,P *l)
{
  if ( !q )
    *l = 0;
  else
    up2top(q->body,l);
}

void gf2ntovect(GF2N q,VECT *l)
{
  if ( !q )
    *l = 0;
  else
    up2tovect(q->body,l);
}

#define NZGF2N(a) ((a)&&(OID(a)==O_N)&&(NID(a)==N_GF2N))

void addgf2n(GF2N a,GF2N b,GF2N *c)
{
  UP2 t,t1;
  GF2N z;

  ptogf2n((Obj)a,&z); a = z; ptogf2n((Obj)b,&z); b = z;
  if ( !a )
    *c = b;
  else if ( !b )
    *c = a;
  else {
    addup2(a->body,b->body,&t);
    gen_simpup2(t,current_mod_gf2n,&t1);
    MKGF2N(t1,*c);
  }
}

void subgf2n(GF2N a,GF2N b,GF2N *c)
{
  addgf2n(a,b,c);
}

void mulgf2n(GF2N a,GF2N b,GF2N *c)
{
  UP2 t;
  GF2N z;

  ptogf2n((Obj)a,&z); a = z; ptogf2n((Obj)b,&z); b = z;
  if ( !a || !b )
    *c = 0;
  else {
    mulup2(a->body,b->body,&t);
#if 0
    gen_simpup2(t,current_mod_gf2n,&t1);
    MKGF2N(t1,*c);  
#else
    gen_simpup2_destructive(t,current_mod_gf2n);
    if ( !t || !t->w )
      *c = 0;
    else
      MKGF2N(t,*c);  
#endif
  }
}

void squaregf2n(GF2N a,GF2N *c)
{
  UP2 t;
  GF2N z;

  ptogf2n((Obj)a,&z); a = z;
  if ( !a )
    *c = 0;
  else {
    squareup2(a->body,&t);
#if 0
    gen_simpup2(t,current_mod_gf2n,&t1);
    MKGF2N(t1,*c);  
#else
    gen_simpup2_destructive(t,current_mod_gf2n);
    if ( !t || !t->w )
      *c = 0;
    else
      MKGF2N(t,*c);  
#endif
  }
}

void divgf2n(GF2N a,GF2N b,GF2N *c)
{
  UP2 t,i,s;
  GF2N z;

  ptogf2n((Obj)a,&z); a = z; ptogf2n((Obj)b,&z); b = z;
  if ( !b )
    error("divgf2n: division by 0");
  else if ( !a )
    *c = 0;
  else {
    gen_invup2(b->body,current_mod_gf2n,&i);
    mulup2(a->body,i,&t);
    gen_simpup2(t,current_mod_gf2n,&s);
    MKGF2N(s,*c);
  }
}

void invgf2n(GF2N b,GF2N *c)
{
  UP2 i;
  GF2N z;

  ptogf2n((Obj)b,&z); b = z;
  if ( !b )
    error("divgf2n: division by 0");
  else {
    gen_invup2(b->body,current_mod_gf2n,&i);
    MKGF2N(i,*c);
  }
}

void chsgngf2n(GF2N a,GF2N *c)
{
  *c = a;
}

void pwrgf2n(GF2N a,Z b,GF2N *c)
{
  UP2 t;
  GF2N r;

  if ( !b ) {
    MKGF2N(ONEUP2,*c);
  } else if ( !a )
    *c = 0;
  else {
    gen_pwrmodup2(a->body,b,current_mod_gf2n,&t);
    MKGF2N(t,r);
    if ( sgnz(b) < 0 )
      invgf2n(r,c);
    else
      *c = r;
  }
}

int cmpgf2n(GF2N a,GF2N b)
{
  GF2N z;

  ptogf2n((Obj)a,&z); a = z; ptogf2n((Obj)b,&z); b = z;
  if ( !a )
    if ( !b )
      return 0;
    else
      return -1;
  else if ( !b )
      return 1;
  else
    return compup2(a->body,b->body);
}

void randomgf2n(GF2N *r)
{
  int i,w,d;
  unsigned int *tb;
  UP2 t;

  if ( !current_mod_gf2n )
    error("randomgf2n : current_mod_gf2n is not set");
  w = current_mod_gf2n->dense->w;
  d = degup2(current_mod_gf2n->dense);
  NEWUP2(t,w);
  for ( i = 0, tb = t->b; i < w; i++ )
    tb[i] = mt_genrand();
  tb[w-1] &= (1<<(d%BSH))-1;
  for ( i = w-1; i >= 0 && !tb[i]; i-- );
  if ( i < 0 )
    *r = 0;
  else {
    t->w = i+1; MKGF2N(t,*r);
  }
}
