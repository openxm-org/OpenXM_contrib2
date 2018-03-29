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
 * $OpenXM: OpenXM_contrib2/asir2000/builtin/cplxnum.c,v 1.4 2000/12/05 01:24:49 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

void Pconj(),Preal(),Pimag();

void cplx_conj(Obj,Obj *);
void cplx_real(Obj,Obj *);
void cplx_imag(Obj,Obj *);

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
