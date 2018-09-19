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

extern int current_mod;

void addmi(MQ a,MQ b,MQ *c)
{
  if ( !current_mod ) error("addmi : current_mod is not set");
  if ( a && RATN(a) ) ptomp(current_mod,(P)a,(P *)&a);
  if ( b && RATN(b) ) ptomp(current_mod,(P)b,(P *)&b);
  if ( (a && !SFF(a)) || (b && !SFF(b)) ) error("addmi : invalid argument");
  addmq(current_mod,a,b,c);
}

void submi(MQ a,MQ b,MQ *c)
{
  if ( !current_mod ) error("submi : current_mod is not set");
  if ( a && RATN(a) ) ptomp(current_mod,(P)a,(P *)&a);
  if ( b && RATN(b) ) ptomp(current_mod,(P)b,(P *)&b);
  if ( (a && !SFF(a)) || (b && !SFF(b)) ) error("submi : invalid argument");
  submq(current_mod,a,b,c);
}

void mulmi(MQ a,MQ b,MQ *c)
{
  if ( !current_mod ) error("mulmi : current_mod is not set");
  if ( a && RATN(a) ) ptomp(current_mod,(P)a,(P *)&a);
  if ( b && RATN(b) ) ptomp(current_mod,(P)b,(P *)&b);
  if ( (a && !SFF(a)) || (b && !SFF(b)) ) error("mulmi : invalid argument");
  mulmq(current_mod,a,b,c);
}

void divmi(MQ a,MQ b,MQ *c)
{
  if ( !current_mod ) error("divmi : current_mod is not set");
  if ( a && RATN(a) ) ptomp(current_mod,(P)a,(P *)&a);
  if ( b && RATN(b) ) ptomp(current_mod,(P)b,(P *)&b);
  if ( (a && !SFF(a)) || (b && !SFF(b)) ) error("divmi : invalid argument");
  divmq(current_mod,a,b,c);
}

void chsgnmi(MQ a,MQ *c)
{
  if ( !current_mod ) error("chsgnmi : current_mod is not set");
  if ( a && RATN(a) ) ptomp(current_mod,(P)a,(P *)&a);
  if ( a && !SFF(a) ) error("chsgnmi : invalid argument");
  submq(current_mod,0,a,c);
}

void pwrmi(MQ a,Q b,MQ *c)
{
  if ( !current_mod ) error("pwrnmi : current_mod is not set");
  if ( a && RATN(a) ) ptomp(current_mod,(P)a,(P *)&a);
  if ( a && !SFF(a) ) error("pwrmi : invalid argument");
  pwrmq(current_mod,a,b,c);
}

int cmpmi(MQ a,MQ b)
{
  int c;

  if ( !current_mod ) error("cmpmi : current_mod is not set");
  if ( a && RATN(a) ) ptomp(current_mod,(P)a,(P *)&a);
  if ( b && RATN(b) ) ptomp(current_mod,(P)b,(P *)&b);
  if ( (a && !SFF(a)) || (b && !SFF(b)) ) error("cmpmi : invalid argument");
  if ( !a ) return !b ? 0 : -1;
  else if ( !b ) return 1;
  else {
    c = CONT(a)-CONT(b);
    return c==0 ? 0 : c>0;
  }
}
