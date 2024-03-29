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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/type.c,v 1.3 2018/10/19 23:27:38 noro Exp $
*/
#include "ca.h"
#include "parse.h"

void Ptype(NODE arg,Obj *rp);
void Pntype(NODE arg,Obj *rp);
void Preadarray(NODE arg,VECT *rp);

struct ftab type_tab[] = {
  {"type",Ptype,1},
  {"ntype",Pntype,1},
  {"readarray",Preadarray,1},
  {0,0,0},
};

void Ptype(NODE arg,Obj *rp)
{
  Obj t;
  Z q;

  if ( ( t = (Obj)ARG0(arg) ) != 0 ) {
    STOZ(OID(t),q); *rp = (Obj)q;
  } else 
    *rp = 0;
}

void Pntype(NODE arg,Obj *rp)
{
  Obj t;
  Z q;

  if ( ( t = (Obj)ARG0(arg) ) != 0 ) {
    asir_assert(t,O_N,"ntype");
    STOZ(NID(t),q); *rp = (Obj)q;
  } else 
    *rp = 0;
}

#include <sys/types.h>
#include <sys/stat.h>

void prod_array_rec(unsigned int *p,unsigned int n,mpz_t r,int level)
{
  unsigned int n2;
  mpz_t s0,s1;

  if ( n == 0 )
    mpz_set_ui(r,1);
  else if ( n == 1 )
    mpz_set_ui(r,p[0]);
  else {
    n2 = n/2;
    mpz_init(s0);
    prod_array_rec(p,n2,s0,level+1);
    mpz_init(s1);
    prod_array_rec(p+n2,n-n2,s1,level+1);
    mpz_mul(r,s0,s1);
  }
}

void Preadarray(NODE arg,VECT *rp)
{
  char *name;
  struct stat buf;
  FILE *fp;
  mpz_t z;
  Z x;
  unsigned int *p;
  unsigned int n,i;
  VECT v;

  name = BDY((STRING)ARG0(arg));
  stat(name,&buf);
  n = buf.st_size/sizeof(unsigned int);
  fp = fopen(name,"rb");
  p = (unsigned int *)MALLOC(n*sizeof(unsigned int));
  fread(p,n,sizeof(unsigned int),fp);
  fclose(fp);
  MKVECT(v,n);
  for ( i = 0; i < n; i++ ) {
    mpz_init_set_ui(z,p[i]);
    MPZTOZ(z,x);
    BDY(v)[i] = (pointer)x;
  }
  *rp = v;
}
