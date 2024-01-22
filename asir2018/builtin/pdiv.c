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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/pdiv.c,v 1.1 2018/09/19 05:45:06 noro Exp $
*/
#include "ca.h"
#include "parse.h"

void Psdiv(NODE arg, Obj *rp);
void Psrem(NODE arg, Obj *rp);
void Pprem(NODE arg, P *rp);
void Psqr(NODE arg, LIST *rp);
void Psdiv_gf2n(NODE arg, GF2N *rp);
void Psrem_gf2n(NODE arg, GF2N *rp);
void Pgcd_gf2n(NODE arg, GF2N *rp);
void Ptdiv(NODE arg, P *rp);
void Pudiv(NODE arg, LIST *rp);
void Psdivm(NODE arg, Obj *rp);
void Psremm(NODE arg, Obj *rp);
void Psqrm(NODE arg, LIST *rp);
void Pinva_mod(NODE arg, P *rp);
void Psrem_mod(NODE arg, P *rp);
void Purem(NODE arg, P *rp);
void Pugcd(NODE arg, P *rp);

struct ftab pdiv_tab[] = {
  {"sdiv",Psdiv,-3},
  {"srem",Psrem,-3},
  {"prem",Pprem,-3},
  {"sdiv_gf2n",Psdiv_gf2n,2},
  {"srem_gf2n",Psrem_gf2n,2},
  {"gcd_gf2n",Pgcd_gf2n,2},
  {"sqr",Psqr,-3},
  {"tdiv",Ptdiv,-3},
  {"udiv",Pudiv,2},
  {"sdivm",Psdivm,-4},
  {"sremm",Psremm,-4},
  {"sqrm",Psqrm,-4},
  {"inva_mod",Pinva_mod,3},
  {"srem_mod",Psrem_mod,3},
  {"ugcd",Pugcd,2},
  {"urem",Purem,2},
  {0,0,0},
};

void Psdiv(NODE arg, Obj *rp)
{
  P q,r,dnd,dnd1,dvr,dvr1;
  V v;
  VL vl;

  asir_assert(ARG0(arg),O_P,"sdiv");
  asir_assert(ARG1(arg),O_P,"sdiv");
  dnd = (P)ARG0(arg); dvr = (P)ARG1(arg);
  if ( argc(arg) == 3 ) {
    v = VR((P)ARG2(arg));
    change_mvar(CO,dnd,v,&dnd1); change_mvar(CO,dvr,v,&dvr1);
    reordvar(CO,v,&vl);
    divsrp(vl,dnd1,dvr1,&q,&r);
    restore_mvar(CO,q,v,(P *)rp);
  } else
    divsrp(CO,dnd,dvr,(P *)rp,&r);
}

void Psrem(NODE arg, Obj *rp)
{
  P q,r,dnd,dnd1,dvr,dvr1;
  V v;
  VL vl;

  asir_assert(ARG0(arg),O_P,"srem");
  asir_assert(ARG1(arg),O_P,"srem");
  dnd = (P)ARG0(arg); dvr = (P)ARG1(arg);
  if ( argc(arg) == 3 ) {
    v = VR((P)ARG2(arg));
    change_mvar(CO,dnd,v,&dnd1); change_mvar(CO,dvr,v,&dvr1);
    reordvar(CO,v,&vl);
    divsrp(vl,dnd1,dvr1,&q,&r);
    restore_mvar(CO,r,v,(P *)rp);
  } else
    divsrp(CO,dnd,dvr,&q,(P *)rp);
}

void Pprem(NODE arg, P *rp)
{
  P q,r,dnd,dnd1,dvr,dvr1;
  V v;
  VL vl;

  asir_assert(ARG0(arg),O_P,"prem");
  asir_assert(ARG1(arg),O_P,"prem");
  dnd = (P)ARG0(arg); dvr = (P)ARG1(arg);
  if ( !dvr ) error("prem : division by 0");
  if ( !dnd ) {
    *rp = 0; return;
  }
  if ( argc(arg) == 3 ) {
    v = VR((P)ARG2(arg));
    change_mvar(CO,dnd,v,&dnd1); change_mvar(CO,dvr,v,&dvr1);
    reordvar(CO,v,&vl);
    premp(vl,dnd1,dvr1,&r);
    restore_mvar(CO,r,v,rp);
  } else
    premp(CO,dnd,dvr,rp);
}

void Psqr(NODE arg, LIST *rp)
{
  P q,q1,r,r1,dnd,dnd1,dvr,dvr1;
  NODE n,tn;
  V v;
  VL vl;

  asir_assert(ARG0(arg),O_P,"sqr");
  asir_assert(ARG1(arg),O_P,"sqr");
  dnd = (P)ARG0(arg); dvr = (P)ARG1(arg);
  if ( argc(arg) == 3 ) {
    v = VR((P)ARG2(arg));
    change_mvar(CO,dnd,v,&dnd1); change_mvar(CO,dvr,v,&dvr1);
    reordvar(CO,v,&vl);
    divsrp(vl,dnd1,dvr1,&q1,&r1);
    restore_mvar(CO,q1,v,&q); restore_mvar(CO,r1,v,&r);
  } else
    divsrp(CO,dnd,dvr,&q,&r);
  MKNODE(tn,r,0); MKNODE(n,q,tn); MKLIST(*rp,n);
}

void Psdiv_gf2n(NODE arg, GF2N *rp)
{
  GF2N dnd,dvr;
  UP2 q,r;

  dnd = (GF2N)ARG0(arg); dvr = (GF2N)ARG1(arg);
  if ( !dvr )
    error("sdiv_gf2n : division by 0");
  else if ( !dnd )
    *rp = 0;
  else {
    qrup2(dnd->body,dvr->body,&q,&r);
    MKGF2N(q,*rp);    
  }
}

void Psrem_gf2n(NODE arg, GF2N *rp)
{
  GF2N dnd,dvr;
  UP2 q,r;

  dnd = (GF2N)ARG0(arg); dvr = (GF2N)ARG1(arg);
  if ( !dvr )
    error("srem_gf2n : division by 0");
  else if ( !dnd )
    *rp = 0;
  else {
    qrup2(dnd->body,dvr->body,&q,&r);
    MKGF2N(r,*rp);    
  }
}

void Pgcd_gf2n(NODE arg, GF2N *rp)
{
  GF2N p1,p2;
  UP2 gcd;

  p1 = (GF2N)ARG0(arg); p2 = (GF2N)ARG1(arg);
  if ( !p1 )
    *rp = p2;
  else if ( !p2 )
    *rp = p1;
  else {
    gcdup2(p1->body,p2->body,&gcd);
    MKGF2N(gcd,*rp);    
  }
}

void Ptdiv(NODE arg, P *rp)
{
  P p1,p2,q1,q2,q,c1,c2,c;
  int m;

  p1 = (P)ARG0(arg); p2 = (P)ARG1(arg);
  asir_assert(p1,O_P,"tdiv");
  asir_assert(p2,O_P,"tdiv");
  if ( !p1 || !p2 )
    *rp = 0;
  else if ( (OID(p1) > O_P) || (OID(p2) > O_P ) )
    *rp = 0;
  else if ( argc(arg) == 3 ) {
    m = ZTOS((Q)ARG2(arg));
    ptomp(m,p1,&q1); ptomp(m,p2,&q2);
    if ( divtmp(CO,m,q1,q2,&q) )
      mptop(q,rp);
    else
      *rp = 0;
  } else if ( qpcheck((Obj)p1) && qpcheck((Obj)p2) ) {
    ptozp(p1,1,(Q *)&c1,&q1); ptozp(p2,1,(Q *)&c2,&q2);
    if ( divtpz(CO,q1,q2,&q) ) {
      divq((Q)c1,(Q)c2,(Q *)&c); mulp(CO,q,c,rp);
    } else
      *rp = 0;
  } else {
    if ( !divtp(CO,p1,p2,rp) )
      *rp = 0;
  }
}

void Pudiv(NODE arg, LIST *rp)
{
  P q,r,dnd,dvr;
  NODE n,tn;

  asir_assert(ARG0(arg),O_P,"udiv");
  asir_assert(ARG1(arg),O_P,"udiv");
  dnd = (P)ARG0(arg); dvr = (P)ARG1(arg);
  udivpz(dnd,dvr,&q,&r);
  MKNODE(tn,r,0); MKNODE(n,q,tn); MKLIST(*rp,n);
}

void Psdivm(NODE arg, Obj *rp)
{
  P q,r,dnd,dnd1,dndm,dvr,dvr1,dvrm,t;
  V v;
  VL vl;
  int m;

  asir_assert(ARG0(arg),O_P,"sdivm");
  asir_assert(ARG1(arg),O_P,"sdivm");
  asir_assert(ARG2(arg),O_N,"sdivm");
  dnd = (P)ARG0(arg); dvr = (P)ARG1(arg); m = ZTOS((Q)ARG2(arg));
  if ( argc(arg) == 4 ) {
    v = VR((P)ARG3(arg));
    change_mvar(CO,dnd,v,&dnd1); change_mvar(CO,dvr,v,&dvr1);
    reordvar(CO,v,&vl);
    ptomp(m,dnd1,&dndm); ptomp(m,dvr1,&dvrm);
    divsrmp(vl,m,dndm,dvrm,&t,&r); mptop(t,&q);
    restore_mvar(CO,q,v,(P *)rp);
  } else {
    ptomp(m,dnd,&dndm); ptomp(m,dvr,&dvrm);
    divsrmp(CO,m,dndm,dvrm,&t,&r); mptop(t,(P *)rp);
  }
}

void Psremm(NODE arg, Obj *rp)
{
  P q,r,dnd,dnd1,dndm,dvr,dvr1,dvrm,t;
  V v;
  VL vl;
  int m;

  asir_assert(ARG0(arg),O_P,"sremm");
  asir_assert(ARG1(arg),O_P,"sremm");
  asir_assert(ARG2(arg),O_N,"sremm");
  dnd = (P)ARG0(arg); dvr = (P)ARG1(arg); m = ZTOS((Q)ARG2(arg));
  if ( argc(arg) == 4 ) {
    v = VR((P)ARG3(arg));
    change_mvar(CO,dnd,v,&dnd1); change_mvar(CO,dvr,v,&dvr1);
    reordvar(CO,v,&vl);
    ptomp(m,dnd1,&dndm); ptomp(m,dvr1,&dvrm);
    divsrmp(vl,m,dndm,dvrm,&q,&t); mptop(t,&r);
    restore_mvar(CO,r,v,(P *)rp);
  } else {
    ptomp(m,dnd,&dndm); ptomp(m,dvr,&dvrm);
    divsrmp(CO,m,dndm,dvrm,&q,&t); mptop(t,(P *)rp);
  }
}

void Psqrm(NODE arg, LIST *rp)
{
  P q,q1,r,r1,dnd,dnd1,dndm,dvr,dvr1,dvrm;
  NODE n,tn;
  V v;
  VL vl;
  int m;

  asir_assert(ARG0(arg),O_P,"sqrm");
  asir_assert(ARG1(arg),O_P,"sqrm");
  asir_assert(ARG2(arg),O_N,"sqrm");
  dnd = (P)ARG0(arg); dvr = (P)ARG1(arg); m = ZTOS((Q)ARG2(arg));
  if ( argc(arg) == 4 ) {
    v = VR((P)ARG3(arg));
    change_mvar(CO,dnd,v,&dnd1); change_mvar(CO,dvr,v,&dvr1);
    reordvar(CO,v,&vl);
    ptomp(m,dnd1,&dndm); ptomp(m,dvr1,&dvrm);
    divsrmp(vl,m,dndm,dvrm,&q,&r); mptop(q,&q1); mptop(r,&r1);
    restore_mvar(CO,q1,v,&q); restore_mvar(CO,r1,v,&r);
  } else {
    ptomp(m,dnd,&dndm); ptomp(m,dvr,&dvrm);
    divsrmp(CO,m,dndm,dvrm,&q1,&r1); mptop(q1,&q); mptop(r1,&r);
  }
  MKNODE(tn,r,0); MKNODE(n,q,tn); MKLIST(*rp,n);
}

void Pinva_mod(NODE arg, P *rp)
{
  P dp,f;
  Z q;
  int n,i;
  int mod;
  V v;
  UM wf,wdp,winv;

  asir_assert(ARG0(arg),O_P,"gcda_mod");
  asir_assert(ARG1(arg),O_N,"gcda_mod");
  asir_assert(ARG2(arg),O_P,"gcda_mod");
  dp = (P)ARG0(arg);
  mod = ZTOS((Q)ARG1(arg));
  f = (P)ARG2(arg);
  if ( NUM(f) ) {
    i = invm(remqi((Q)f,mod),mod);
    STOZ(i,q); *rp = (P)q;
  } else {
    v = VR(dp);
    n = MAX(UDEG(dp),UDEG(f));
    wf = W_UMALLOC(n); wdp = W_UMALLOC(n);
    winv = W_UMALLOC(n);
    ptoum(mod,f,wf); ptoum(mod,dp,wdp);
    invum(mod,wdp,wf,winv);
    if ( DEG(winv) < 0 )
      *rp = 0;
    else {
      umtop(v,winv,rp);
    }
  }
}

void Psrem_mod(NODE arg, P *rp)
{
  P p1,p2;
  int n,dr;
  int mod;
  V v;
  UM wp1,wp2,q;

  asir_assert(ARG0(arg),O_P,"srem_mod");
  asir_assert(ARG1(arg),O_P,"srem_mod");
  asir_assert(ARG2(arg),O_N,"srem_mod");
  p1 = (P)ARG0(arg); p2 = (P)ARG1(arg); mod = ZTOS((Q)ARG2(arg));
  if ( !p1 || NUM(p1) )
    *rp = p1;  
  else {
    v = VR(p1);
    n = MAX(UDEG(p1),UDEG(p2));
    wp1 = W_UMALLOC(n); wp2 = W_UMALLOC(n); q = W_UMALLOC(n);
    ptoum(mod,p1,wp1); ptoum(mod,p2,wp2);
    dr = divum(mod,wp1,wp2,q);
    if ( ( DEG(wp1) = dr ) == -1 )
      *rp = 0;
    else
      umtop(v,wp1,rp);
  }
}

void Purem(NODE arg, P *rp)
{ 
  asir_assert(ARG0(arg),O_P,"urem");
  asir_assert(ARG1(arg),O_P,"urem");
  uremp((P)ARG0(arg),(P)ARG1(arg),rp);
}

void Pugcd(NODE arg, P *rp)
{
  asir_assert(ARG0(arg),O_P,"ugcd");
  asir_assert(ARG1(arg),O_P,"ugcd");
  ugcdp((P)ARG0(arg),(P)ARG1(arg),rp);
}

void invum(int mod,UM dp,UM f,UM inv)
{
  UM g1,g2,a1,a2,a3,wm,q,tum;
  int d,dr;

  d = DEG(dp)+DEG(f)+10;
  g1 = W_UMALLOC(d); g2 = W_UMALLOC(d); a1 = W_UMALLOC(d);
  a2 = W_UMALLOC(d); a3 = W_UMALLOC(d); wm = W_UMALLOC(d);
  q = W_UMALLOC(d);
  DEG(a1) = 0; COEF(a1)[0] = 1; DEG(a2) = -1;
  cpyum(f,g1); cpyum(dp,g2);
  while ( 1 ) {
    dr = divum(mod,g1,g2,q); tum = g1; g1 = g2; g2 = tum;
    if ( ( DEG(g2) = dr ) == -1 )
        break;
    mulum(mod,a2,q,wm); subum(mod,a1,wm,a3); dr = divum(mod,a3,dp,q);
    tum = a1; a1 = a2; a2 = a3; a3 = tum; DEG(a3) = dr;
  }
  if ( DEG(g1) != 0 )
    DEG(inv) = -1;
  else if ( COEF(g1)[0] != 1 )
    mulsum(mod,a2,invm(COEF(g1)[0],mod),inv);
  else
    cpyum(a2,inv);
}
