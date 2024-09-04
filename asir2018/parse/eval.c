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
 * $OpenXM: OpenXM_contrib2/asir2018/parse/eval.c,v 1.4 2019/11/12 10:53:23 kondoh Exp $
*/
#include <ctype.h>
#include "ca.h"
#include "al.h"
#include "base.h"
#include "parse.h"
#if defined(GC7)
#include "gc.h"
#endif
#include <sys/types.h>
#include <sys/stat.h>

extern JMP_BUF timer_env;
extern FUNC cur_binf;
extern NODE PVSS;
extern int evalef;

int f_break,f_return,f_continue;
int evalstatline;
int show_crossref;
int at_root;
void gen_searchf_searchonly(char *name,FUNC *r,int global);
LIST eval_arg(FNODE a,unsigned int quote);

pointer eval(FNODE f)
{
  LIST t;
  STRING str;
  pointer val = 0;
  pointer a,a1,a2;
  NODE tn,tn1,ind,match;
  R u;
  DP dp;
  DPM dpm;
  unsigned int pv;
  int c,ret;
  Obj pos;
  FNODE f1;
  UP2 up2;
  UP up;
  UM um;
  Obj obj;
  GF2N gf2n;
  GFPN gfpn;
  GFSN gfsn;
  RANGE range;
  QUOTE expr,pattern;
  Q q;

#if defined(VISUAL) || defined(__MINGW32__)
  check_intr();
#endif
  if ( !f )
    return ( 0 );
  switch ( f->id ) {
    case I_PAREN:
      val = eval((FNODE)(FA0(f)));
      break;
    case I_MINUS:
      a1 = eval((FNODE)(FA0(f)));
      arf_chsgn((Obj)a1,&obj);
      val = (pointer)obj;
      break;
    case I_BOP:
      a1 = eval((FNODE)FA1(f)); a2 = eval((FNODE)FA2(f));
      (*((ARF)FA0(f))->fp)(CO,(Obj)a1,(Obj)a2,(Obj *)&val); 
      break;
    case I_NARYOP:
      tn = (NODE)FA1(f);
      a = eval((FNODE)BDY(tn));
      for ( tn = NEXT(tn); tn; tn = NEXT(tn) ) {
        a1 = eval((FNODE)BDY(tn));
        (*((ARF)FA0(f))->fp)(CO,(Obj)a,(Obj)a1,(Obj *)&a2); 
        a = a2;
      }
      val = a;
      break;
    case I_COP:
      a1 = eval((FNODE)FA1(f)); a2 = eval((FNODE)FA2(f));
      c = arf_comp(CO,a1,a2);
      switch ( (long)FA0(f) ) {
        case C_EQ:
          c = (c == 0); break;
        case C_NE:
          c = (c != 0); break;
        case C_GT:
          c = (c > 0); break;
        case C_LT:
          c = (c < 0); break;
        case C_GE:
          c = (c >= 0); break;
        case C_LE:
          c = (c <= 0); break;
        default:
          c = 0; break;
      }
      if ( c )
        val = (pointer)ONE; 
      break;
    case I_AND:
      if ( eval((FNODE)FA0(f)) && eval((FNODE)FA1(f)) ) 
        val = (pointer)ONE; 
      break;
    case I_OR:
      if ( eval((FNODE)FA0(f)) || eval((FNODE)FA1(f)) ) 
        val = (pointer)ONE; 
      break;
    case I_NOT:
      if ( eval((FNODE)FA0(f)) )
        val = 0;
      else
        val = (pointer)ONE; 
      break;
    case I_LOP:
      a1 = eval((FNODE)FA1(f)); a2 = eval((FNODE)FA2(f));
      val = evall((long)FA0(f),a1,a2);
      break;
    case I_CE:
      if ( eval((FNODE)FA0(f)) )
        val = eval((FNODE)FA1(f));
      else
        val = eval((FNODE)FA2(f));
      break;
    case I_EV:
      evalnodebody((NODE)FA0(f),&tn); nodetod(tn,&dp); val = (pointer)dp;
      break;
    case I_EVM:
      evalnodebody((NODE)FA0(f),&tn); pos = (Obj)eval((FNODE)FA1(f)); 
      if ( !pos )
        error("position of a module element must be positive.");
      nodetodpm(tn,pos,&dpm); val = (pointer)dpm;
      break;
    case I_FUNC:
      val = evalf((FUNC)FA0(f),(FNODE)FA1(f),0); break;
    case I_FUNC_OPT:
      val = evalf((FUNC)FA0(f),(FNODE)FA1(f),(FNODE)FA2(f)); break;
    case I_FUNC_QARG:
      tn = BDY(eval_arg((FNODE)FA1(f),(unsigned int)0xffffffff));
      val = bevalf((FUNC)FA0(f),tn,0); break;
    case I_PFDERIV:
      val = evalf_deriv((FUNC)FA0(f),(FNODE)FA1(f),(FNODE)FA2(f)); break;
    case I_MAP:
      val = evalmapf((FUNC)FA0(f),(FNODE)FA1(f),(FNODE)FA2(f)); break;
    case I_RECMAP:
      val = eval_rec_mapf((FUNC)FA0(f),(FNODE)FA1(f),(FNODE)FA2(f)); break;
    case I_IFUNC:
      val = evalif((FNODE)FA0(f),(FNODE)FA1(f),(FNODE)FA2(f)); break;
#if !defined(VISUAL) && !defined(__MINGW32__)
    case I_TIMER:
      {
        int interval;
        Obj expired;

        interval = ZTOS((Q)eval((FNODE)FA0(f)));
        expired = (Obj)eval((FNODE)FA2(f));
        set_timer(interval);
        savepvs();
        if ( !SETJMP(timer_env) )
          val = eval((FNODE)FA1(f));
        else {
          val = (pointer)expired;
          restorepvs();
        }
        reset_timer();
      }
      break;
#endif
    case I_PRESELF:
      f1 = (FNODE)FA1(f);
      if ( ID(f1) == I_PVAR ) {
        pv = (unsigned long)FA0(f1); ind = (NODE)FA1(f1); GETPV(pv,a); 
        if ( !ind ) {
          (*((ARF)FA0(f))->fp)(CO,(Obj)a,(Obj)ONE,(Obj *)&val); ASSPV(pv,val); 
        } else if ( a ) {
          evalnodebody(ind,&tn); getarray(a,tn,(pointer *)&u); 
          (*((ARF)FA0(f))->fp)(CO,(Obj)u,(Obj)ONE,(Obj *)&val); putarray(a,tn,val);
        }
      } else
        error("++ : not implemented yet");
      break;
    case I_POSTSELF:
      f1 = (FNODE)FA1(f);
      if ( ID(f1) == I_PVAR ) {
        pv = (unsigned long)FA0(f1); ind = (NODE)FA1(f1); GETPV(pv,val); 
        if ( !ind ) {
          (*((ARF)FA0(f))->fp)(CO,(Obj)val,(Obj)ONE,(Obj *)&u); ASSPV(pv,u);
        } else if ( val ) {
          evalnodebody(ind,&tn); getarray(val,tn,&a);
          (*((ARF)FA0(f))->fp)(CO,(Obj)a,(Obj)ONE,(Obj *)&u); putarray(val,tn,(pointer)u);
          val = a;
        }
      } else
        error("-- : not implemented yet");
      break;
    case I_PVAR:
      pv = (unsigned long)FA0(f);
      ind = (NODE)FA1(f);
      GETPV(pv,a); 
      if ( !ind )
        val = a;
      else {
        evalnodebody(ind,&tn); getarray(a,tn,&val); 
      }
      break;
    case I_ASSPVAR:
      f1 = (FNODE)FA0(f);
      if ( ID(f1) == I_PVAR ) {
        pv = (unsigned long)FA0(f1); ind = (NODE)FA1(f1);
        if ( !ind ) {
          val = eval((FNODE)FA1(f)); ASSPV(pv,val);
        } else {
          GETPV(pv,a);
          evalnodebody(ind,&tn); 
          putarray(a,tn,val = eval((FNODE)FA1(f))); 
        }
      } else if ( ID(f1) == I_POINT ) {
        /* f1 <-> FA0(f1)->FA1(f1) */
        a = eval(FA0(f1));
        assign_to_member(a,(char *)FA1(f1),val = eval((FNODE)FA1(f)));
      } else if ( ID(f1) == I_INDEX ) {
        /* f1 <-> FA0(f1)[FA1(f1)] */
        a = eval((FNODE)FA0(f1)); ind = (NODE)FA1(f1);
        evalnodebody(ind,&tn);
        putarray(a,tn,val = eval((FNODE)FA1(f)));
      } else if ( ID(f1) == I_LIST ) {
        val = eval((FNODE)FA1(f));
        if ( val != 0 && OID((Obj)val) == O_LIST ) {
           tn = (NODE)FA0(f1);
           tn1 = (NODE)BDY((LIST)val);
           if ( length(tn) != length(tn1) )
             error("eval : len(lhs) != len(rhs) in assignment");
           for ( ; tn != 0 && tn1 != 0; tn = NEXT(tn), tn1 = NEXT(tn1) ) {
             eval(mkfnode(2,I_ASSPVAR,BDY(tn),mkfnode(1,I_FORMULA,BDY(tn1))));
           }
        } else 
          error("eval : invalid assignment");
      } else {
        error("eval : invalid assignment");
      }
      break;
    case I_ANS:
      if ( (pv =(long)FA0(f)) < (long)APVS->n ) 
        val = APVS->va[pv].priv;
      break;
    case I_GF2NGEN:
      NEWUP2(up2,1);
      up2->w=1;
      up2->b[0] = 2; /* @ */
      MKGF2N(up2,gf2n);
      val = (pointer)gf2n;
      break;
    case I_GFPNGEN:
      up = UPALLOC(1);
      DEG(up)=1;
      COEF(up)[0] = 0;
      COEF(up)[1] = (Num)ONELM;
      MKGFPN(up,gfpn);
      val = (pointer)gfpn;
      break;
    case I_GFSNGEN:
      um = UMALLOC(1);
      DEG(um) = 1;
      COEF(um)[0] = 0;
      COEF(um)[1] = _onesf();
      MKGFSN(um,gfsn);
      val = (pointer)gfsn;
      break;
    case I_STR:
      MKSTR(str,FA0(f)); val = (pointer)str; break;
    case I_FORMULA:
      val = FA0(f); 
      break;
    case I_LIST:
      evalnodebody((NODE)FA0(f),&tn); MKLIST(t,tn); val = (pointer)t; break;
    case I_CONS:
      evalnodebody((NODE)FA0(f),&tn); a2 = eval(FA1(f));
      if ( !a2 || OID(a2) != O_LIST )
          error("cons : invalid argument");
      for ( tn1 = tn; NEXT(tn1); tn1 = NEXT(tn1) );
      NEXT(tn1) = BDY((LIST)a2);
      MKLIST(t,tn); val = (pointer)t;
      break;
    case I_NEWCOMP:
      newstruct((long)FA0(f),(struct oCOMP **)&val); break;
    case I_CAR:
      if ( !(a = eval((FNODE)FA0(f))) || (OID(a) != O_LIST) )
        val = 0;
      else if ( !BDY((LIST)a) )
        val = a;
      else
        val = (pointer)BDY(BDY((LIST)a));
      break;
    case I_CDR:
      if ( !(a = eval((FNODE)FA0(f))) || (OID(a) != O_LIST) )
        val = 0;
      else if ( !BDY((LIST)a) )
        val = a;
      else {
        MKLIST(t,NEXT(BDY((LIST)a))); val = (pointer)t;
      }
      break;
    case I_INDEX:
      a = eval((FNODE)FA0(f)); ind = (NODE)FA1(f);
      evalnodebody(ind,&tn); getarray(a,tn,&val); 
      break;
    case I_OPT:
      MKSTR(str,(char *)FA0(f));
      a = (pointer)eval(FA1(f));
      tn = mknode(2,str,a);
      MKLIST(t,tn); val = (pointer)t;
      break;
    case I_GETOPT:
      val = (pointer)getopt_from_cpvs((char *)FA0(f));
      break;
    case I_POINT:
      a = (pointer)eval(FA0(f));
      val = (pointer)memberofstruct(a,(char *)FA1(f));
      break;
    default:
      error("eval : unknown id");
      break;
  }
  return ( val );
}

NODE fnode_to_nary_node(NODE);
NODE fnode_to_bin_node(NODE,int);

FNODE fnode_to_nary(FNODE f)
{
  FNODE a0,a1,a2;
  NODE n,t,t0;
  pointer val;
  char *op;

  if ( !f )
    return f;
  switch ( f->id ) {
    case I_NARYOP:
        n = fnode_to_nary_node((NODE)FA1(f));
        return mkfnode(2,I_NARYOP,FA0(f),n);

    case I_BOP: 
      a1 = fnode_to_nary((FNODE)FA1(f));
      a2 = fnode_to_nary((FNODE)FA2(f));
      op = ((ARF)FA0(f))->name;
      if ( !strcmp(op,"+") || !strcmp(op,"*") ) {
        if ( a1->id == I_NARYOP && !strcmp(op,((ARF)FA0(a1))->name) ) {
          for ( n = (NODE)FA1(a1); NEXT(n); n = NEXT(n) );
          if ( a2->id == I_NARYOP && !strcmp(op,((ARF)FA0(a2))->name) )
            NEXT(n) = (NODE)FA1(a2);
          else
            MKNODE(NEXT(n),a2,0);
          return a1;
        } else if ( a2->id == I_NARYOP && !strcmp(op,((ARF)FA0(a2))->name) ) {
          MKNODE(t,a1,(NODE)FA1(a2));
          return mkfnode(2,I_NARYOP,FA0(f),t);
        } else {
          t = mknode(2,a1,a2);
          return mkfnode(2,I_NARYOP,FA0(f),t);
        }
      } else
        return mkfnode(3,f->id,FA0(f),a1,a2);

    case I_NOT: case I_PAREN: case I_MINUS:
    case I_CAR: case I_CDR:
      a0 = fnode_to_nary((FNODE)FA0(f));
      return mkfnode(1,f->id,a0);

    case I_COP: case I_LOP:
      a1 = fnode_to_nary((FNODE)FA1(f));
      a2 = fnode_to_nary((FNODE)FA2(f));
      return mkfnode(3,f->id,FA0(f),a1,a2);

    case I_AND: case I_OR:
      a0 = fnode_to_nary((FNODE)FA0(f));
      a1 = fnode_to_nary((FNODE)FA1(f));
      return mkfnode(2,f->id,a0,a1);

    /* ternary operators */
    case I_CE:
      a0 = fnode_to_nary((FNODE)FA0(f));
      a1 = fnode_to_nary((FNODE)FA1(f));
      a2 = fnode_to_nary((FNODE)FA2(f));
      return mkfnode(3,f->id,a0,a1,a2);
      break;

    /* function */
    case I_FUNC:
      a1 = fnode_to_nary((FNODE)FA1(f));
      return mkfnode(2,f->id,FA0(f),a1);

    case I_LIST: case I_EV:
      n = fnode_to_nary_node((NODE)FA0(f));
      return mkfnode(1,f->id,n);

    case I_STR: case I_FORMULA: case I_PVAR:
      return f;

    default:
      error("fnode_to_nary : not implemented yet");
      return 0;
  }
}

FNODE fnode_to_bin(FNODE f,int dir)
{
  FNODE a0,a1,a2;
  NODE n,t;
  pointer val;
  ARF fun;
  int len,i;
  FNODE *arg;

  if ( !f )
    return f;
  switch ( f->id ) {
    case I_NARYOP: 
      fun = (ARF)FA0(f);
      len = length((NODE)FA1(f));
      if ( len==1 ) return BDY((NODE)(FA1(f)));

      arg = (FNODE *)ALLOCA(len*sizeof(FNODE));
      for ( i = 0, t = (NODE)FA1(f); i < len; i++, t = NEXT(t) )
        arg[i] = fnode_to_bin((FNODE)BDY(t),dir);
      if ( dir ) {
        a2 = mkfnode(3,I_BOP,fun,arg[len-2],arg[len-1]);
        for ( i = len-3; i >= 0; i-- )
          a2 = mkfnode(3,I_BOP,fun,arg[i],a2);
      } else {
        a2 = mkfnode(3,I_BOP,fun,arg[0],arg[1]);
        for ( i = 2; i < len; i++ )
          a2 = mkfnode(3,I_BOP,fun,a2,arg[i]);
      }
      return a2;

    case I_NOT: case I_PAREN: case I_MINUS:
    case I_CAR: case I_CDR:
      a0 = fnode_to_bin((FNODE)FA0(f),dir);
      return mkfnode(1,f->id,a0);

    case I_BOP: case I_COP: case I_LOP:
      a1 = fnode_to_bin((FNODE)FA1(f),dir);
      a2 = fnode_to_bin((FNODE)FA2(f),dir);
      return mkfnode(3,f->id,FA0(f),a1,a2);

    case I_AND: case I_OR:
      a0 = fnode_to_bin((FNODE)FA0(f),dir);
      a1 = fnode_to_bin((FNODE)FA1(f),dir);
      return mkfnode(2,f->id,a0,a1);

    /* ternary operators */
    case I_CE:
      a0 = fnode_to_bin((FNODE)FA0(f),dir);
      a1 = fnode_to_bin((FNODE)FA1(f),dir);
      a2 = fnode_to_bin((FNODE)FA2(f),dir);
      return mkfnode(3,f->id,a0,a1,a2);
      break;

    /* function */
    case I_FUNC:
      a1 = fnode_to_bin((FNODE)FA1(f),dir);
      return mkfnode(2,f->id,FA0(f),a1);

    case I_LIST: case I_EV:
      n = fnode_to_bin_node((NODE)FA0(f),dir);
      return mkfnode(1,f->id,n);

    case I_STR: case I_FORMULA: case I_PVAR:
      return f;

    default:
      error("fnode_to_bin : not implemented yet");
      return 0;
  }
}

NODE partial_eval_node(NODE n);
FNODE partial_eval(FNODE f);

FNODE partial_eval(FNODE f)
{
  FNODE a0,a1,a2;
  NODE n;
  Obj obj;
  QUOTE q;
  pointer val;
  FUNC func;

  if ( !f )
    return f;
  switch ( f->id ) {
    case I_NOT: case I_PAREN: case I_MINUS:
    case I_CAR: case I_CDR:
      a0 = partial_eval((FNODE)FA0(f));
      return mkfnode(1,f->id,a0);

    case I_BOP: case I_COP: case I_LOP:
      a1 = partial_eval((FNODE)FA1(f));
      a2 = partial_eval((FNODE)FA2(f));
      return mkfnode(3,f->id,FA0(f),a1,a2);

    case I_NARYOP:
      n = partial_eval_node((NODE)FA1(f));
      return mkfnode(2,f->id,FA0(f),n);

    case I_AND: case I_OR:
      a0 = partial_eval((FNODE)FA0(f));
      a1 = partial_eval((FNODE)FA1(f));
      return mkfnode(2,f->id,a0,a1);

    /* ternary operators */
    case I_CE:
      a0 = partial_eval((FNODE)FA0(f));
      a1 = partial_eval((FNODE)FA1(f));
      a2 = partial_eval((FNODE)FA2(f));
      return mkfnode(3,f->id,a0,a1,a2);
      break;

    /* XXX : function is evaluated with QUOTE args */
    case I_FUNC:
      a1 = partial_eval((FNODE)FA1(f));
      func = (FUNC)FA0(f);
      if ( func->id == A_UNDEF || func->id != A_USR ) {
        a1 =  mkfnode(2,I_FUNC,func,a1);
        return a1;
      } else {
        n = BDY(eval_arg(a1,(unsigned int)0xffffffff));
        obj = bevalf(func,n,0);
        objtoquote(obj,&q);
        return BDY(q);
      }
      break;

    case I_LIST: case I_EV:
      n = partial_eval_node((NODE)FA0(f));
      return mkfnode(1,f->id,n);

    case I_STR: case I_FORMULA:
      return f;

    /* program variable */
    case I_PVAR:
      val = eval(f);
      if ( val && OID((Obj)val) == O_QUOTE )
        return partial_eval((FNODE)BDY((QUOTE)val));
      else
        return mkfnode(1,I_FORMULA,val);

    default:
      error("partial_eval : not implemented yet");
      return 0;
  }
}

NODE partial_eval_node(NODE n)
{
  NODE r0,r,t;

  for ( r0 = 0, t = n; t; t = NEXT(t) ) {
    NEXTNODE(r0,r);
    BDY(r) = partial_eval((FNODE)BDY(t));
  }
  if ( r0 ) NEXT(r) = 0;
  return r0;
}

NODE rewrite_fnode_node(NODE n,NODE arg,int qarg);
FNODE rewrite_fnode(FNODE f,NODE arg,int qarg);

FNODE rewrite_fnode(FNODE f,NODE arg,int qarg)
{
  FNODE a0,a1,a2,value;
  NODE n,t,pair;
  pointer val;
  int pv,ind;

  if ( !f )
    return f;
  switch ( f->id ) {
    case I_NOT: case I_PAREN: case I_MINUS:
    case I_CAR: case I_CDR:
      a0 = rewrite_fnode((FNODE)FA0(f),arg,qarg);
      return mkfnode(1,f->id,a0);

    case I_BOP: case I_COP: case I_LOP:
      a1 = rewrite_fnode((FNODE)FA1(f),arg,qarg);
      a2 = rewrite_fnode((FNODE)FA2(f),arg,qarg);
      return mkfnode(3,f->id,FA0(f),a1,a2);

    case I_AND: case I_OR:
      a0 = rewrite_fnode((FNODE)FA0(f),arg,qarg);
      a1 = rewrite_fnode((FNODE)FA1(f),arg,qarg);
      return mkfnode(2,f->id,a0,a1);

    /* ternary operators */
    case I_CE:
      a0 = rewrite_fnode((FNODE)FA0(f),arg,qarg);
      a1 = rewrite_fnode((FNODE)FA1(f),arg,qarg);
      a2 = rewrite_fnode((FNODE)FA2(f),arg,qarg);
      return mkfnode(3,f->id,a0,a1,a2);
      break;

    /* nary operators */
    case I_NARYOP:
      n = rewrite_fnode_node((NODE)FA1(f),arg,qarg);
      return mkfnode(2,f->id,FA0(f),n);

    /* and function */
    case I_FUNC:
      a1 = rewrite_fnode((FNODE)FA1(f),arg,qarg);
      return mkfnode(2,qarg?I_FUNC_QARG:f->id,FA0(f),a1);

    case I_LIST: case I_EV:
      n = rewrite_fnode_node((NODE)FA0(f),arg,qarg);
      return mkfnode(1,f->id,n);

    case I_STR: case I_FORMULA:
      return f;

    /* program variable */
    case I_PVAR:
      pv = (long)FA0(f);
      for ( t = arg; t; t = NEXT(t) ) {
        pair = (NODE)BDY(t);
        ind = (long)BDY(pair);
        value = (FNODE)BDY(NEXT(pair));
        if ( pv == ind )
          return value;
      }
      return f;
      break;

    default:
      error("rewrite_fnode : not implemented yet");
      return 0;
  }
}

NODE rewrite_fnode_node(NODE n,NODE arg,int qarg)
{
  NODE r0,r,t;

  for ( r0 = 0, t = n; t; t = NEXT(t) ) {
    NEXTNODE(r0,r);
    BDY(r) = rewrite_fnode((FNODE)BDY(t),arg,qarg);
  }
  if ( r0 ) NEXT(r) = 0;
  return r0;
}

NODE fnode_to_nary_node(NODE n)
{
  NODE r0,r,t;

  for ( r0 = 0, t = n; t; t = NEXT(t) ) {
    NEXTNODE(r0,r);
    BDY(r) = fnode_to_nary((FNODE)BDY(t));
  }
  if ( r0 ) NEXT(r) = 0;
  return r0;
}

NODE fnode_to_bin_node(NODE n,int dir)
{
  NODE r0,r,t;

  for ( r0 = 0, t = n; t; t = NEXT(t) ) {
    NEXTNODE(r0,r);
    BDY(r) = fnode_to_bin((FNODE)BDY(t),dir);
  }
  if ( r0 ) NEXT(r) = 0;
  return r0;
}

V searchvar(char *name);

pointer evalstat(SNODE f)
{
  pointer val = 0,t,s,s1;
  P u;
  NODE tn;
  int i,ac;
  V v;
  V *a;
  char *buf;
  FUNC func;

  if ( !f )
    return ( 0 );
  if ( nextbp && nextbplevel <= 0 && f->id != S_CPLX ) {
    nextbp = 0;
    bp(f);
  }
  evalstatline = f->ln;
  if ( !PVSS ) at_root = evalstatline;

  switch ( f->id ) {
    case S_BP:
      if ( !nextbp && (!FA1(f) || eval((FNODE)FA1(f))) ) {
        if ( (FNODE)FA2(f) ) {
          asir_out = stderr;
          printexpr(CO,eval((FNODE)FA2(f)));
          putc('\n',asir_out); fflush(asir_out);
          asir_out = stdout;
        } else {
          nextbp = 1; nextbplevel = 0; 
        }
      }
      val = evalstat((SNODE)FA0(f)); 
      break;
    case S_PFDEF:
      ac = argc(FA1(f)); a = (V *)MALLOC(ac*sizeof(V));
      s = eval((FNODE)FA2(f));
      buf = (char *)ALLOCA(BUFSIZ);
      for ( i = 0, tn = (NODE)FA1(f); tn; tn = NEXT(tn), i++ ) {
        t = eval((FNODE)tn->body); sprintf(buf,"_%s",NAME(VR((P)t)));
        makevar(buf,&u); a[i] = VR(u);
        substr(CO,0,(Obj)s,VR((P)t),(Obj)u,(Obj *)&s1); s = s1;
      }
#if defined(INTERVAL)
      mkpf((char *)FA0(f),(Obj)s,ac,a,0,0,0,0,(PF *)&val); val = 0;
#else
      mkpf((char *)FA0(f),(Obj)s,ac,a,0,0,0,(PF *)&val); val = 0;
#endif
      v = searchvar((char *)FA0(f));
      if ( v ) {
        searchpf((char *)FA0(f),&func);
        makesrvar(func,&u);
      }
      break;
    case S_SINGLE:
      val = eval((FNODE)FA0(f)); break;
    case S_CPLX:
      for ( tn = (NODE)FA0(f); tn; tn = NEXT(tn) ) {
        if ( BDY(tn) )
          val = evalstat((SNODE)BDY(tn));
        if ( f_break || f_return || f_continue )
          break;
      }
      break;
    case S_BREAK:
      if ( 1 || GPVS != CPVS )
        f_break = 1; 
      break;
    case S_CONTINUE:
      if ( 1 || GPVS != CPVS )
        f_continue = 1; 
      break;
    case S_RETURN:
      if ( 1 || GPVS != CPVS ) {
        val = eval((FNODE)FA0(f)); f_return = 1;
      }
      break;
    case S_IFELSE:
      if ( evalnode((NODE)FA1(f)) )
        val = evalstat((SNODE)FA2(f)); 
      else if ( FA3(f) )
        val = evalstat((SNODE)FA3(f));
      break;
    case S_FOR:
      evalnode((NODE)FA1(f));
      while ( 1 ) {
        if ( !evalnode((NODE)FA2(f)) )
          break;
        val = evalstat((SNODE)FA4(f)); 
        if ( f_break || f_return )
          break;
        f_continue = 0;
        evalnode((NODE)FA3(f));
      }
      f_break = 0; break;
    case S_DO:
      while ( 1 ) {
        val = evalstat((SNODE)FA1(f)); 
        if ( f_break || f_return )
          break;
        f_continue = 0;
        if ( !evalnode((NODE)FA2(f)) )
          break;
      }
      f_break = 0; break;
    case S_MODULE:
      CUR_MODULE = (MODULE)FA0(f);
      if ( CUR_MODULE )
          MPVS = CUR_MODULE->pvs;
      else
          MPVS = 0;
      break;
    default:
      error("evalstat : unknown id");
      break;
  }
  return ( val );
}

pointer evalnode(NODE node)
{
  NODE tn;
  pointer val;

  for ( tn = node, val = 0; tn; tn = NEXT(tn) )
    if ( BDY(tn) )
      val = eval((FNODE)BDY(tn));
  return ( val );
}


LIST eval_arg(FNODE a,unsigned int quote)
{
  LIST l;
  FNODE fn;
  NODE n,n0,tn;
  QUOTE q;
  int i;

  for ( tn = (NODE)FA0(a), n0 = 0, i = 0; tn; tn = NEXT(tn), i++ ) {
    NEXTNODE(n0,n);
    if ( quote & (1<<i) ) {
      fn = (FNODE)(BDY(tn));
      if ( fn->id == I_FORMULA && FA0(fn) 
        && OID((Obj)FA0(fn))== O_QUOTE )
         BDY(n) = FA0(fn);    
      else {
        MKQUOTE(q,(FNODE)BDY(tn));
        BDY(n) = (pointer)q;
      }
    } else
      BDY(n) = eval((FNODE)BDY(tn));
  }
  if ( n0 ) NEXT(n) = 0;
  MKLIST(l,n0);
  return l;
}

pointer evalf(FUNC f,FNODE a,FNODE opt)
{
  LIST args;
  pointer val;
  int i,n,level;
  NODE tn,sn,opts,opt1,dmy;
      VS pvs,prev_mpvs;
  char errbuf[BUFSIZ];
  static unsigned int stack_size;
  static void *stack_base;
  FUNC f1;

  if ( f->id == A_UNDEF ) {
    gen_searchf_searchonly(f->fullname,&f1,0);
    if ( f1->id == A_UNDEF ) {
      sprintf(errbuf,"evalf : %s undefined",NAME(f));
      error(errbuf);
    } else
      *f = *f1;
  }
  if ( getsecuremode() && !PVSS && !f->secure ) {
    sprintf(errbuf,"evalf : %s not permitted",NAME(f));
    error(errbuf);
  }
  if ( f->id != A_PARI ) {
    for ( i = 0, tn = a?(NODE)FA0(a):0; tn; i++, tn = NEXT(tn) );
    if ( ((n = f->argc)>= 0 && i != n) || (n < 0 && i > -n) ) {
      sprintf(errbuf,"evalf : argument mismatch in %s()",NAME(f));
      error(errbuf);
    }
  }
  switch ( f->id ) {
    case A_BIN:
      if ( opt ) {
        opts = BDY((LIST)eval(opt));
        /* opts = ["opt1",arg1],... */
        opt1 = BDY((LIST)BDY(opts));
        if ( !strcmp(BDY((STRING)BDY(opt1)),"option_list") ) {
          /*
           * the special option specification:
           *  option_list=[["o1","a1"],...]
           */
          asir_assert(BDY(NEXT(opt1)),O_LIST,"evalf");
          opts = BDY((LIST)BDY(NEXT(opt1)));
        }
      } else
        opts = 0;
      if ( !n ) {
        current_option = opts;
        cur_binf = f;
        (*f->f.binf)(&val);
      } else {
        args = (LIST)eval_arg(a,f->quote);
        current_option = opts;
        cur_binf = f;
        (*f->f.binf)(args?BDY(args):0,&val);
      }
      cur_binf = 0;
      break;
    case A_PARI:
      args = (LIST)eval(a);
      cur_binf = f;
      val = evalparif(f,args?BDY(args):0);
      cur_binf = 0;
      break;
    case A_USR:
      /* stack check */
#if !defined(VISUAL) && !defined(__MINGW32__) && !defined(__CYGWIN__)
      if ( !stack_size ) {
        struct rlimit rl;
        getrlimit(RLIMIT_STACK,&rl);
        stack_size = rl.rlim_cur;
      }
            if ( !stack_base ) {
#if defined(GC7)
                stack_base = (void *)GC_get_main_stack_base();
#else
                stack_base = (void *)GC_get_stack_base();
#endif
            }
      if ( (stack_base - (void *)&args) +0x100000 > stack_size )
        error("stack overflow");
#endif
      args = (LIST)eval_arg(a,f->quote);
      if ( opt ) {
        opts = BDY((LIST)eval(opt));
        /* opts = ["opt1",arg1],... */
        opt1 = BDY((LIST)BDY(opts));
        if ( !strcmp(BDY((STRING)BDY(opt1)),"option_list") ) {
          /*
           * the special option specification:
           *  option_list=[["o1","a1"],...]
           */
          asir_assert(BDY(NEXT(opt1)),O_LIST,"evalf");
          opts = BDY((LIST)BDY(NEXT(opt1)));
        }
      } else
        opts = 0;
        pvs = f->f.usrf->pvs;
        if ( PVSS ) {
            ((VS)BDY(PVSS))->at = evalstatline;
        level = ((VS)BDY(PVSS))->level+1;
      } else
        level = 1;
        MKNODE(tn,pvs,PVSS); PVSS = tn;
        CPVS = (VS)ALLOCA(sizeof(struct oVS)); BDY(PVSS) = (pointer)CPVS;
        CPVS->usrf = f; CPVS->n = CPVS->asize = pvs->n;
      CPVS->level = level;
      CPVS->opt = opts;
        if ( CPVS->n ) {
            CPVS->va = (struct oPV *)ALLOCA(CPVS->n*sizeof(struct oPV));
            bcopy((char *)pvs->va,(char *)CPVS->va,
          (int)(pvs->n*sizeof(struct oPV)));
        }
        if ( nextbp )
            nextbplevel++;
      for ( tn = f->f.usrf->args, sn = BDY(args); 
        sn; tn = NEXT(tn), sn = NEXT(sn) )
        ASSPV((long)FA0((FNODE)BDY(tn)),BDY(sn));
      f_return = f_break = f_continue = 0;
      if ( f->f.usrf->module ) {
        prev_mpvs = MPVS;
        MPVS = f->f.usrf->module->pvs;
        val = evalstat((SNODE)BDY(f->f.usrf)); 
        MPVS = prev_mpvs;
      } else
        val = evalstat((SNODE)BDY(f->f.usrf)); 
      f_return = f_break = f_continue = 0; poppvs(); 
      if ( PVSS )
            evalstatline = ((VS)BDY(PVSS))->at;
      break;
    case A_PURE:
      args = (LIST)eval(a);
      val = evalpf(f->f.puref,args?BDY(args):0,0);
      break;
    default:
      sprintf(errbuf,"evalf : %s undefined",NAME(f));
      error(errbuf);
      break;
  }
  return val;
}

pointer evalf_deriv(FUNC f,FNODE a,FNODE deriv)
{
  LIST args,dargs;
  pointer val;
  char errbuf[BUFSIZ];

  switch ( f->id ) {
    case A_PURE:
      args = (LIST)eval(a);
      dargs = (LIST)eval(deriv);
      val = evalpf(f->f.puref,
        args?BDY(args):0,dargs?BDY(dargs):0);
      break;
    default:
      sprintf(errbuf,
        "evalf : %s is not a pure function",NAME(f));
      error(errbuf);
      break;
  }
  return val;
}

pointer evalmapf(FUNC f,FNODE a,FNODE opt)
{
  LIST args;
  NODE node,rest,t,n,r,r0,opts,opt1;
  Obj head;
  VECT v,rv;
  MAT m,rm;
  LIST rl;
  int len,row,col,i,j;
  pointer val;

  args = (LIST)eval_arg(a,f->quote);
  node = BDY(args); head = (Obj)BDY(node); rest = NEXT(node);
  if ( opt ) {
    opts = BDY((LIST)eval(opt));
    /* opts = ["opt1",arg1],... */
    opt1 = BDY((LIST)BDY(opts));
    if ( !strcmp(BDY((STRING)BDY(opt1)),"option_list") ) {
      /*
       * the special option specification:
       *  option_list=[["o1","a1"],...]
       */
      asir_assert(BDY(NEXT(opt1)),O_LIST,"evalf");
      opts = BDY((LIST)BDY(NEXT(opt1)));
    }
  } else
    opts = 0;
  if ( !head ) {
    val = bevalf(f,node,opts);
    return val;
  }
  switch ( OID(head) ) {
    case O_VECT:
      v = (VECT)head; len = v->len; MKVECT(rv,len);
      for ( i = 0; i < len; i++ ) {
        MKNODE(t,BDY(v)[i],rest); BDY(rv)[i] = bevalf(f,t,opts);
      }
      val = (pointer)rv;
      break;
    case O_MAT:
      m = (MAT)head; row = m->row; col = m->col; MKMAT(rm,row,col);
      for ( i = 0; i < row; i++ )
        for ( j = 0; j < col; j++ ) {
          MKNODE(t,BDY(m)[i][j],rest); BDY(rm)[i][j] = bevalf(f,t,opts);
        }
      val = (pointer)rm;
      break;
    case O_LIST:
      n = BDY((LIST)head);
      for ( r0 = r = 0; n; n = NEXT(n) ) {
        NEXTNODE(r0,r); MKNODE(t,BDY(n),rest); BDY(r) = bevalf(f,t,opts);
      }
      if ( r0 )
        NEXT(r) = 0;
      MKLIST(rl,r0);
      val = (pointer)rl;
      break;
    default:
      val = bevalf(f,node,opts);
      break;
  }
  return val;
}

pointer eval_rec_mapf(FUNC f,FNODE a,FNODE opt)
{
  LIST args;
  NODE opts,opt1;

  args = (LIST)eval_arg(a,f->quote);
  if ( opt ) {
    opts = BDY((LIST)eval(opt));
    /* opts = ["opt1",arg1],... */
    opt1 = BDY((LIST)BDY(opts));
    if ( !strcmp(BDY((STRING)BDY(opt1)),"option_list") ) {
      /*
       * the special option specification:
       *  option_list=[["o1","a1"],...]
       */
      asir_assert(BDY(NEXT(opt1)),O_LIST,"evalf");
      opts = BDY((LIST)BDY(NEXT(opt1)));
    }
  } else
    opts = 0;
  return beval_rec_mapf(f,BDY(args),opts);
}

pointer beval_rec_mapf(FUNC f,NODE node,NODE opts)
{
  NODE rest,t,n,r,r0;
  Obj head;
  VECT v,rv;
  MAT m,rm;
  LIST rl;
  int len,row,col,i,j;
  pointer val;

  head = (Obj)BDY(node); rest = NEXT(node);
  if ( !head ) {
    val = bevalf(f,node,opts);
    return val;
  }
  switch ( OID(head) ) {
    case O_VECT:
      v = (VECT)head; len = v->len; MKVECT(rv,len);
      for ( i = 0; i < len; i++ ) {
        MKNODE(t,BDY(v)[i],rest); BDY(rv)[i] = beval_rec_mapf(f,t,opts);
      }
      val = (pointer)rv;
      break;
    case O_MAT:
      m = (MAT)head; row = m->row; col = m->col; MKMAT(rm,row,col);
      for ( i = 0; i < row; i++ )
        for ( j = 0; j < col; j++ ) {
          MKNODE(t,BDY(m)[i][j],rest);
          BDY(rm)[i][j] = beval_rec_mapf(f,t,opts);
        }
      val = (pointer)rm;
      break;
    case O_LIST:
      n = BDY((LIST)head);
      for ( r0 = r = 0; n; n = NEXT(n) ) {
        NEXTNODE(r0,r); MKNODE(t,BDY(n),rest);
        BDY(r) = beval_rec_mapf(f,t,opts);
      }
      if ( r0 )
        NEXT(r) = 0;
      MKLIST(rl,r0);
      val = (pointer)rl;
      break;
    default:
      val = bevalf(f,node,opts);
      break;
  }
  return val;
}

pointer bevalf(FUNC f,NODE a,NODE opts)
{
  pointer val;
  int i,n;
  NODE tn,sn;
  VS pvs,prev_mpvs;
  char errbuf[BUFSIZ];

  if ( f->id == A_UNDEF ) {
    sprintf(errbuf,"bevalf : %s undefined",NAME(f));
    error(errbuf);
  }
  if ( getsecuremode() && !PVSS && !f->secure ) {
    sprintf(errbuf,"bevalf : %s not permitted",NAME(f));
    error(errbuf);
  }
  if ( f->id != A_PARI ) {
    for ( i = 0, tn = a; tn; i++, tn = NEXT(tn) );
    if ( ((n = f->argc)>= 0 && i != n) || (n < 0 && i > -n) ) {
      sprintf(errbuf,"bevalf : argument mismatch in %s()",NAME(f));
      error(errbuf);
    }
  }
  switch ( f->id ) {
    case A_BIN:
      current_option = opts;
      if ( !n ) {
        cur_binf = f;
        (*f->f.binf)(&val);
      } else {
        cur_binf = f;
        (*f->f.binf)(a,&val);
      }
      cur_binf = 0;
      break;
    case A_PARI:
      cur_binf = f;
      val = evalparif(f,a);
      cur_binf = 0;
      break;
    case A_USR:
        pvs = f->f.usrf->pvs;
        if ( PVSS )
            ((VS)BDY(PVSS))->at = evalstatline;
        MKNODE(tn,pvs,PVSS); PVSS = tn;
        CPVS = (VS)ALLOCA(sizeof(struct oVS)); BDY(PVSS) = (pointer)CPVS;
        CPVS->usrf = f; CPVS->n = CPVS->asize = pvs->n;
      CPVS->opt = opts;
        if ( CPVS->n ) {
            CPVS->va = (struct oPV *)ALLOCA(CPVS->n*sizeof(struct oPV));
            bcopy((char *)pvs->va,(char *)CPVS->va,
          (int)(pvs->n*sizeof(struct oPV)));
        }
        if ( nextbp )
            nextbplevel++;
      for ( tn = f->f.usrf->args, sn = a; 
        sn; tn = NEXT(tn), sn = NEXT(sn) )
        ASSPV((long)FA0((FNODE)BDY(tn)),BDY(sn));
      f_return = f_break = f_continue = 0;
      if ( f->f.usrf->module ) {
        prev_mpvs = MPVS;
        MPVS = f->f.usrf->module->pvs;
        val = evalstat((SNODE)BDY(f->f.usrf)); 
        MPVS = prev_mpvs;
      } else
        val = evalstat((SNODE)BDY(f->f.usrf)); 
      f_return = f_break = f_continue = 0; poppvs(); 
      break;
    case A_PURE:
      val = evalpf(f->f.puref,a,0);
      break;
    default:
      sprintf(errbuf,"bevalf : %s undefined",NAME(f));
      error(errbuf);
      break;
  }
  return val;
}

pointer evalif(FNODE f,FNODE a,FNODE opt)
{
  Obj g;
  QUOTE q;
  FNODE t;
  LIST l;

  g = (Obj)eval(f);
  if ( g && (OID(g) == O_P) && (VR((P)g)->attr == (pointer)V_SR) )
    return evalf((FUNC)VR((P)g)->priv,a,opt);
  else if ( g && OID(g) == O_QUOTEARG && ((QUOTEARG)g)->type == A_func ) {
    t = mkfnode(2,I_FUNC,((QUOTEARG)g)->body,a);
    MKQUOTE(q,t);
    return q;
  } else {
    error("invalid function pointer");
    /* NOTREACHED */
    return (pointer)-1;
  }
}

pointer evalpf(PF pf,NODE args,NODE dargs)
{
  Obj s,s1;
  int i,di,j;
  NODE node,dnode;
  PFINS ins;
  PFAD ad;
  
  if ( !pf->body ) {
    ins = (PFINS)CALLOC(1,sizeof(PF)+pf->argc*sizeof(struct oPFAD));
    ins->pf = pf;
    for ( i = 0, node = args, dnode = dargs, ad = ins->ad;
      node; i++ ) {
      ad[i].arg = (Obj)node->body;  
      if ( !dnode ) ad[i].d = 0;
      else
        ad[i].d = ZTOS((Q)dnode->body);
      node = NEXT(node);
      if ( dnode ) dnode = NEXT(dnode);
    }
    simplify_ins(ins,&s);
  } else {
    s = pf->body;
    if ( dargs ) {
      for ( i = 0, dnode = dargs; dnode; dnode = NEXT(dnode), i++ ) {
        di = ZTOS((Q)dnode->body);
        for ( j = 0; j < di; j++ ) {
          derivr(CO,s,pf->args[i],&s1); s = s1;
        }
      }
    }
    for ( i = 0, node = args; node; node = NEXT(node), i++ ) {
      substr(CO,0,s,pf->args[i],(Obj)node->body,&s1); s = s1;
    }
  }
  return (pointer)s;
}

void evalnodebody(NODE sn,NODE *dnp)
{
  NODE n,n0,tn;
  int line;

  if ( !sn ) {
    *dnp = 0;
    return;
  }
  line = evalstatline;
  for ( tn = sn, n0 = 0; tn; tn = NEXT(tn) ) {
    NEXTNODE(n0,n);
    BDY(n) = eval((FNODE)BDY(tn));
    evalstatline = line;
  }
  NEXT(n) = 0; *dnp = n0;
}

MODULE searchmodule(char *name)
{
  MODULE mod;
  NODE m;

  for ( m = MODULE_LIST; m; m = NEXT(m) ) {
    mod = (MODULE)BDY(m);
    if ( !strcmp(mod->name,name) )
      return mod;
  }
  return 0;
}
/*
 * xxx.yyy() is searched in the flist 
 * of the module xxx.
 * yyy() is searched in the global flist.
 */

void searchuf(char *name,FUNC *r)
{
  MODULE mod;
  char *name0,*dot;

  if ( ( dot = strchr(name,'.') ) != 0 ) {
    name0 = (char *)ALLOCA(strlen(name)+1);
    strcpy(name0,name);
    dot = strchr(name0,'.');
    *dot = 0;
    mod = searchmodule(name0);
    if ( mod )
      searchf(mod->usrf_list,dot+1,r);
  } else
    searchf(usrf,name,r);
}

void gen_searchf(char *name,FUNC *r)
{
  FUNC val = 0;
  int global = 0;
  if ( *name == ':' ) {
    global = 1;
    name += 2;
  }
  if ( CUR_MODULE && !global )
    searchf(CUR_MODULE->usrf_list,name,&val);
  if ( !val )
    searchf(sysf,name,&val);
  if ( !val )
    searchf(ubinf,name,&val);
  if ( !val )
    searchpf(name,&val);
  if ( !val )
    searchuf(name,&val);
  if ( !val )
    appenduf(name,&val);
  *r = val;
}

void gen_searchf_searchonly(char *name,FUNC *r,int global)
{
  FUNC val = 0;
  if ( *name == ':' ) {
    global = 1;
    name += 2;
  }
  if ( CUR_MODULE && !global )
    searchf(CUR_MODULE->usrf_list,name,&val);
  if ( !val )
    searchf(sysf,name,&val);
  if ( !val )
    searchf(ubinf,name,&val);
  if ( !val )
    searchpf(name,&val);
  if ( !val )
    searchuf(name,&val);
  *r = val;
}

void searchf(NODE fn,char *name,FUNC *r)
{
  NODE tn;

  for ( tn = fn; tn && strcmp(NAME((FUNC)BDY(tn)),name); tn = NEXT(tn) )
    ;
  if ( tn ) {
    *r = (FUNC)BDY(tn);
    return;
  }
  *r = 0;
}

MODULE mkmodule(char *);

void appenduf(char *name,FUNC *r)
{
  NODE tn;
  FUNC f;
  int len;
  MODULE mod;
  char *modname,*fname,*dot;

  f=(FUNC)MALLOC(sizeof(struct oFUNC)); 
  f->id = A_UNDEF; f->argc = 0; f->f.binf = 0;
  if ( ( dot = strchr(name,'.') ) != 0 ) {
    /* undefined function in a module */
    len = dot-name;
    modname = (char *)MALLOC_ATOMIC(len+1);
    strncpy(modname,name,len); modname[len] = 0;
    fname = (char *)MALLOC_ATOMIC(strlen(name)-len+1);
    strcpy(fname,dot+1);
    f->name = fname;
    f->fullname = name;
    mod = searchmodule(modname);
    if ( !mod )
      mod = mkmodule(modname);
    MKNODE(tn,f,mod->usrf_list); mod->usrf_list = tn;
  } else {
    f->name = name; 
    f->fullname = name;
    MKNODE(tn,f,usrf); usrf = tn;
  }
  *r = f;
}

void appenduf_local(char *name,FUNC *r)
{
  NODE tn;
  FUNC f;
  MODULE mod;

  for ( tn = CUR_MODULE->usrf_list; tn; tn = NEXT(tn) )
    if ( !strcmp(((FUNC)BDY(tn))->name,name) )
      break;
  if ( tn )
    return;

  f=(FUNC)MALLOC(sizeof(struct oFUNC)); 
  f->id = A_UNDEF; f->argc = 0; f->f.binf = 0;
  f->name = name;
  f->fullname =
    (char *)MALLOC_ATOMIC(strlen(CUR_MODULE->name)+strlen(name)+1);
  sprintf(f->fullname,"%s.%s",CUR_MODULE->name,name);
  MKNODE(tn,f,CUR_MODULE->usrf_list); CUR_MODULE->usrf_list = tn;
  *r = f;
}

void appenduflist(NODE n)
{
  NODE tn;
  FUNC f;

  for ( tn = n; tn; tn = NEXT(tn) )
    appenduf_local((char *)BDY(tn),&f);
}

void mkparif(char *name,FUNC *r)
{
  FUNC f;

  *r = f =(FUNC)MALLOC(sizeof(struct oFUNC)); 
  f->name = name; f->id = A_PARI; f->argc = 0; f->f.binf = 0;
  f->fullname = name;
}

void mkuf(char *name,char *fname,NODE args,SNODE body,int startl,int endl,char *desc,MODULE module)
{
  FUNC f;
  USRF t;
  NODE usrf_list,sn,tn;
  FNODE fn;
  char *longname;
  int argc;

  if ( getsecuremode() ) {
    error("defining function is not permitted in the secure mode");
  }
  if ( *name == ':' )
    name += 2;
  if ( !module ) {
    searchf(sysf,name,&f);
    if ( f ) {
      fprintf(stderr,"def : builtin function %s() cannot be redefined.\n",name);
      CPVS = GPVS; return;
    }
  }
  for ( argc = 0, sn = args; sn; argc++, sn = NEXT(sn) ) {
    fn = (FNODE)BDY(sn);
    if ( !fn || ID(fn) != I_PVAR ) {
      fprintf(stderr,"illegal  argument in %s()\n",name);
      CPVS = GPVS; return;
    }
  }
  usrf_list = module ? module->usrf_list : usrf;
  for ( sn = usrf_list; sn && strcmp(NAME((FUNC)BDY(sn)),name); sn = NEXT(sn) );
  if ( sn )
    f = (FUNC)BDY(sn);
  else {
    f=(FUNC)MALLOC(sizeof(struct oFUNC)); 
    f->name = name; 
    MKNODE(tn,f,usrf_list); usrf_list = tn;
    if ( module ) {
      f->fullname = 
        (char *)MALLOC_ATOMIC(strlen(f->name)+strlen(module->name)+1);
      sprintf(f->fullname,"%s.%s",module->name,f->name);
      module->usrf_list = usrf_list;
    } else {
      f->fullname = f->name;
      usrf = usrf_list;
    }
  }
  if ( Verbose && f->id != A_UNDEF ) {
    if ( module )
      fprintf(stderr,"Warning : %s.%s() redefined.\n",module->name,name);
    else
      fprintf(stderr,"Warning : %s() redefined.\n",name);
  }
  t=(USRF)MALLOC(sizeof(struct oUSRF));
  t->args=args; BDY(t)=body; t->pvs = CPVS; t->fname = fname; 
  t->startl = startl; t->endl = endl; t->module = module;
  t->desc = desc;
  f->id = A_USR; f->argc = argc; f->f.usrf = t;
  CPVS = GPVS;
  CUR_FUNC = 0;
  clearbp(f);
}

/*
  retrieve value of an option whose key matches 'key'
  CVS->opt is a list(node) of key-value pair (list)
  CVS->opt = BDY([[key,value],[key,value],...])
*/

Obj getopt_from_cpvs(char *key)
{
  NODE opts,opt;
  LIST r;
  extern Obj VOIDobj;

  opts = CPVS->opt;
  if ( !key ) {
    MKLIST(r,opts);
    return (Obj)r;
  } else {
    for ( ; opts; opts = NEXT(opts) ) {
      asir_assert(BDY(opts),O_LIST,"getopt_from_cvps");
      opt = BDY((LIST)BDY(opts));
      if ( !strcmp(key,BDY((STRING)BDY(opt))) )
        return (Obj)BDY(NEXT(opt));
    }
    return VOIDobj;
  }

}

MODULE mkmodule(char *name)
{
  MODULE mod;
  NODE m;
  int len;
  VS mpvs;

  for ( m = MODULE_LIST; m; m = NEXT(m) ) {
    mod = (MODULE)m->body;
    if ( !strcmp(mod->name,name) )
      break;
  }
  if ( m )
    return mod;
  else {
    mod = (MODULE)MALLOC(sizeof(struct oMODULE));
    len = strlen(name);
    mod->name = (char *)MALLOC_ATOMIC(len+1);
    strcpy(mod->name,name);
    mod->pvs = mpvs = (VS)MALLOC(sizeof(struct oVS));
    asir_reallocarray((char **)&mpvs->va,(int *)&mpvs->asize,
      (int *)&mpvs->n,(int)sizeof(struct oPV));
    mod->usrf_list = 0;
    MKNODE(m,mod,MODULE_LIST);
    MODULE_LIST = m;
    return mod;
  }
}

void print_crossref(FUNC f)
{
  FUNC r;
  if ( show_crossref && CUR_FUNC ) {
  searchuf(f->fullname,&r);
  if (r != NULL) {
    fprintf(asir_out,"%s() at line %d in %s()\n",
        f->fullname, asir_infile->ln, CUR_FUNC);
  }
  }
}
