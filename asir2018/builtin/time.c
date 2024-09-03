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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/time.c,v 1.2 2018/09/27 02:39:37 noro Exp $
*/
#include "ca.h"
#include "parse.h"
#include <limits.h>

void Pcurrenttime(Z *rp);
void Pdcurrenttime(Real *rp);
void Ptstart(pointer *rp);
void Ptstop(pointer *rp);
void Pcputime(NODE arg,pointer *rp);
void Ptime(LIST *listp);

struct ftab time_tab[] = {
  {"time",Ptime,0},
  {"cputime",Pcputime,1},
  {"currenttime",Pcurrenttime,0},
  {"dcurrenttime",Pdcurrenttime,0},
  {"tstart",Ptstart,0},
  {"tstop",Ptstop,0},
  {0,0,0},
};

static struct oEGT egt0;
static double r0;
double get_rtime(), get_current_time();

void Pcurrenttime(Z *rp)
{
  int t;

  t = (int)get_current_time(); STOZ(t,*rp);
}

void Pdcurrenttime(Real *rp)
{
  double t;

  t = get_current_time(); MKReal(t,*rp);
}

void Ptstart(pointer *rp)
{
  get_eg(&egt0); r0 = get_rtime(); *rp = 0;
}

void Ptstop(pointer *rp)
{
  struct oEGT egt1;

  get_eg(&egt1); printtime(&egt0,&egt1,get_rtime()-r0); *rp = 0;
}

int prtime;

void Pcputime(NODE arg,pointer *rp)
{
  prtime = ARG0(arg) ? 1 : 0; *rp = 0;
}

void Ptime(LIST *listp)
{
  struct oEGT eg;
  Real re,rg,rr;
  NODE a,b,w,r;
  Z words;
  size_t t;
  double rtime;
  double get_rtime();

  rtime = get_rtime(); MKReal(rtime,rr);
  t = get_allocwords();
  STOZ(t,words);
  get_eg(&eg); MKReal(eg.exectime,re); MKReal(eg.gctime,rg);
  MKNODE(r,rr,0); MKNODE(w,words,r); MKNODE(a,rg,w); MKNODE(b,re,a);
  MKLIST(*listp,b);
}

void printtime(struct oEGT *egt0,struct oEGT *egt1,double r)
{
#if defined(VISUAL)
  fprintf(stderr,"%.4gsec\n",r);
#else
  double e,g;

  e = egt1->exectime - egt0->exectime;
  if ( e < 0 ) e = 0;
  g = 0;
  fprintf(stderr,"%.4gsec(%.4gsec)\n",e,r);
#endif
}
