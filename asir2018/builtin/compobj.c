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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/compobj.c,v 1.1 2018/09/19 05:45:05 noro Exp $
*/
#include "ca.h"
#include "parse.h"
#include "comp.h"

void Pget_element_at(NODE arg,Obj *rp);
void Pput_element_at(NODE arg,Obj *rp);
void Pget_element_names(NODE arg,LIST *rp);
void Pget_struct_name(NODE arg,STRING *rp);
void Pstruct_type(NODE arg,Z *rp);
void Parfreg(NODE arg,Z *rp);
void Prange(NODE arg,RANGE *rp);

struct ftab comp_tab[] = {
  {"arfreg",Parfreg,8},
  {"struct_type",Pstruct_type,1},
  {"get_element_at",Pget_element_at,2},
  {"put_element_at",Pput_element_at,3},
  {"get_element_names",Pget_element_names,1},
  {"get_struct_name",Pget_struct_name,1},
  {"range",Prange,2},
  {0,0,0},
};

void Pget_element_at(NODE arg,Obj *rp)
{
  COMP obj;
  char *name;
  int i;
  SDEF sdef;

  asir_assert(ARG0(arg),O_COMP,"get_element_at");
  asir_assert(ARG1(arg),O_STR,"get_element_at");
  name = BDY((STRING)ARG1(arg));
  obj = (COMP)ARG0(arg);
  sdef = LSS->sa+obj->type;
  for ( i = 0; i < sdef->n; i++ )
    if ( !strcmp(name,sdef->member[i]) ) break;
  if ( i < sdef->n ) {
    *rp = obj->member[i];
  } else
    error("get_element_at : no such member");
}

void Pput_element_at(NODE arg,Obj *rp)
{
  COMP obj;
  char *name;
  int i;
  SDEF sdef;

  asir_assert(ARG0(arg),O_COMP,"put_element_at");
  asir_assert(ARG1(arg),O_STR,"put_element_at");
  name = BDY((STRING)ARG1(arg));
  obj = (COMP)ARG0(arg);
  sdef = LSS->sa+obj->type;
  for ( i = 0; i < sdef->n; i++ )
    if ( !strcmp(name,sdef->member[i]) ) break;
  if ( i < sdef->n ) {
    obj->member[i] = (Obj)ARG2(arg);
    *rp = obj->member[i];
  } else
    error("put_element_at : no such member");
}

void Pget_element_names(NODE arg,LIST *rp)
{
  COMP obj;
  int i;
  SDEF sdef;
  NODE t,t1;
  STRING name;

  asir_assert(ARG0(arg),O_COMP,"get_element_names");
  obj = (COMP)ARG0(arg);
  sdef = LSS->sa+obj->type;
  t = 0;
  for ( i = sdef->n-1; i >= 0; i-- ) {
    MKSTR(name,sdef->member[i]);
    MKNODE(t1,(pointer)name,t);
    t = t1;
  }
  MKLIST(*rp,t);
}

void Pget_struct_name(NODE arg,STRING *rp)
{
  COMP obj;
  SDEF sdef;

  asir_assert(ARG0(arg),O_COMP,"get_struct_name");
  obj = (COMP)ARG0(arg);
  sdef = LSS->sa+obj->type;
  MKSTR(*rp,sdef->name);
}

void Pstruct_type(NODE arg,Z *rp)
{
  Obj obj;
  char *name;
  int ind;

/*  asir_assert(ARG0(arg),O_STR,"struct_type"); */
  obj = (Obj)ARG0(arg);
  if ( !obj )
    error("struct_type : invalid argument");
  switch ( OID(obj) ) {
    case O_STR:
      name = ((STRING)obj)->body;
      ind = structtoindex(name);
      break;
    case O_COMP:
      ind = ((COMP)obj)->type;
      break;
    default:
      error("struct_type : invalid argument");
      break;
  }
  STOZ(ind,*rp);    
}

void Parfreg(NODE arg,Z *rp)
{
  char *name;
  P t;
  SDEF s;
  int i;
  FUNC func;

  asir_assert(ARG0(arg),O_STR,"arfreg");
  name = ((STRING)ARG0(arg))->body;
  for ( s = LSS->sa, i = 0; i < LSS->n; i++ )
    if ( !strcmp(s[i].name,name) )
      break;
  if ( i == LSS->n )
    error("arfreg : no such structure");
    
  t = (P)ARG1(arg); s[i].arf.add = !t ? 0 : (gen_searchf(NAME(VR(t)),&func), func);
  t = (P)ARG2(arg); s[i].arf.sub = !t ? 0 : (gen_searchf(NAME(VR(t)),&func), func);
  t = (P)ARG3(arg); s[i].arf.mul = !t ? 0 : (gen_searchf(NAME(VR(t)),&func), func);
  t = (P)ARG4(arg); s[i].arf.div = !t ? 0 : (gen_searchf(NAME(VR(t)),&func), func);
  t = (P)ARG5(arg); s[i].arf.pwr = !t ? 0 : (gen_searchf(NAME(VR(t)),&func), func);
  t = (P)ARG6(arg); s[i].arf.chsgn = !t ? 0 : (gen_searchf(NAME(VR(t)),&func), func);
  t = (P)ARG7(arg); s[i].arf.comp = !t ? 0 : (gen_searchf(NAME(VR(t)),&func), func);
  *rp = ONE;
}

void Prange(NODE arg,RANGE *rp)
{
  RANGE range;

  NEWRANGE(range);
  range->start = (Obj)ARG0(arg);
  range->end = (Obj)ARG1(arg);
  *rp = range;
}
