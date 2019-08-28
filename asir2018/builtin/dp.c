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
 * $OpenXM: OpenXM_contrib2/asir2018/builtin/dp.c,v 1.9 2019/08/21 00:37:47 noro Exp $
*/
#include "ca.h"
#include "base.h"
#include "parse.h"

extern int dp_fcoeffs;
extern int dp_nelim;
extern int dp_order_pair_length;
extern struct order_pair *dp_order_pair;
extern struct order_spec *dp_current_spec;
extern struct modorder_spec *dp_current_modspec;
extern int nd_rref2;

int do_weyl;

void Pdp_monomial_hilbert_poincare();
void Pdp_sort();
void Pdp_mul_trunc(),Pdp_quo();
void Pdp_ord(), Pdp_ptod(), Pdp_dtop(), Phomogenize();
void Pdp_ptozp(), Pdp_ptozp2(), Pdp_red(), Pdp_red2(), Pdp_lcm(), Pdp_redble();
void Pdp_sp(), Pdp_hm(), Pdp_ht(), Pdp_hc(), Pdp_rest(), Pdp_td(), Pdp_sugar();
void Pdp_set_sugar();
void Pdp_cri1(),Pdp_cri2(),Pdp_subd(),Pdp_mod(),Pdp_red_mod(),Pdp_tdiv();
void Pdp_prim(),Pdp_red_coef(),Pdp_mag(),Pdp_set_kara(),Pdp_rat();
void Pdp_nf(),Pdp_true_nf(),Pdp_true_nf_marked(),Pdp_true_nf_marked_mod();

void Pdp_true_nf_and_quotient(),Pdp_true_nf_and_quotient_mod();
void Pdp_true_nf_and_quotient_marked(),Pdp_true_nf_and_quotient_marked_mod();

void Pdp_nf_mod(),Pdp_true_nf_mod();
void Pdp_criB(),Pdp_nelim();
void Pdp_minp(),Pdp_sp_mod();
void Pdp_homo(),Pdp_dehomo();
void Pdp_gr_mod_main(),Pdp_gr_f_main();
void Pdp_gr_main(),Pdp_gr_hm_main(),Pdp_gr_d_main(),Pdp_gr_flags();
void Pdp_interreduce();
void Pdp_f4_main(),Pdp_f4_mod_main(),Pdp_f4_f_main();
void Pdp_gr_print();
void Pdp_mbase(),Pdp_lnf_mod(),Pdp_nf_tab_mod(),Pdp_mdtod(), Pdp_nf_tab_f();
void Pdp_vtoe(), Pdp_etov(), Pdp_dtov(), Pdp_idiv(), Pdp_sep();
void Pdp_cont();
void Pdp_gr_checklist();
void Pdp_ltod(),Pdpv_ord(),Pdpv_ht(),Pdpv_hm(),Pdpv_hc();
void Pdpm_ltod(),Pdpm_dtol(),Pdpm_set_schreyer(),Pdpm_nf(),Pdpm_weyl_nf(),Pdpm_sp(),Pdpm_weyl_sp(),Pdpm_nf_and_quotient();
void Pdpm_hm(),Pdpm_ht(),Pdpm_hc(),Pdpm_hp(),Pdpm_rest(),Pdpm_shift(),Pdpm_split(),Pdpm_sort(),Pdpm_dptodpm(),Pdpm_redble();

void Pdp_weyl_red();
void Pdp_weyl_sp();

void Pdp_weyl_nf(),Pdp_weyl_nf_mod();
void Pdp_weyl_true_nf_and_quotient(),Pdp_weyl_true_nf_and_quotient_mod();
void Pdp_weyl_true_nf_and_quotient_marked(),Pdp_weyl_true_nf_and_quotient_marked_mod();

void Pdp_weyl_gr_main(),Pdp_weyl_gr_mod_main(),Pdp_weyl_gr_f_main();
void Pdp_weyl_f4_main(),Pdp_weyl_f4_mod_main(),Pdp_weyl_f4_f_main();
void Pdp_weyl_mul(),Pdp_weyl_mul_mod(),Pdp_weyl_act();
void Pdp_weyl_set_weight();
void Pdp_set_weight(),Pdp_set_top_weight(),Pdp_set_module_weight();
void Pdp_nf_f(),Pdp_weyl_nf_f();
void Pdpm_nf_f(),Pdpm_weyl_nf_f();
void Pdp_lnf_f();
void Pnd_gr(),Pnd_gr_trace(),Pnd_f4(),Pnd_f4_trace();
void Pnd_gr_postproc(), Pnd_weyl_gr_postproc();
void Pnd_gr_recompute_trace(), Pnd_btog();
void Pnd_weyl_gr(),Pnd_weyl_gr_trace();
void Pnd_nf(),Pnd_weyl_nf();
void Pdp_initial_term();
void Pdp_order();
void Pdp_inv_or_split();
void Pdp_compute_last_t();
void Pdp_compute_last_w();
void Pdp_compute_essential_df();
void Pdp_get_denomlist();
void Pdp_symb_add();
void Pdp_mono_raddec();
void Pdp_mono_reduce();
void Pdp_rref2(),Psumi_updatepairs(),Psumi_symbolic();

LIST dp_initial_term();
LIST dp_order();
void parse_gr_option(LIST f,NODE opt,LIST *v,Num *homo,
  int *modular,struct order_spec **ord);
NODE dp_inv_or_split(NODE gb,DP f,struct order_spec *spec, DP *inv);

LIST remove_zero_from_list(LIST);

struct ftab dp_tab[] = {
  /* content reduction */
  {"dp_ptozp",Pdp_ptozp,1},
  {"dp_ptozp2",Pdp_ptozp2,2},
  {"dp_prim",Pdp_prim,1},
  {"dp_red_coef",Pdp_red_coef,2},
  {"dp_cont",Pdp_cont,1},

/* polynomial ring */
  /* special operations */
  {"dp_mul_trunc",Pdp_mul_trunc,3},
  {"dp_quo",Pdp_quo,2},

  /* s-poly */
  {"dp_sp",Pdp_sp,2},
  {"dp_sp_mod",Pdp_sp_mod,3},

  /* m-reduction */
  {"dp_red",Pdp_red,3},
  {"dp_red_mod",Pdp_red_mod,4},

  /* normal form */
  {"dp_nf",Pdp_nf,4},
  {"dp_nf_mod",Pdp_nf_mod,5},
  {"dp_nf_f",Pdp_nf_f,4},
  {"dpm_nf_and_quotient",Pdpm_nf_and_quotient,-3},
  {"dpm_nf_f",Pdpm_nf_f,4},
  {"dpm_weyl_nf_f",Pdpm_weyl_nf_f,4},
  {"dpm_nf",Pdpm_nf,4},
  {"dpm_sp",Pdpm_sp,2},
  {"dpm_weyl_sp",Pdpm_weyl_sp,2},

  {"dp_true_nf",Pdp_true_nf,4},
  {"dp_true_nf_mod",Pdp_true_nf_mod,5},
  {"dp_true_nf_marked",Pdp_true_nf_marked,4},
  {"dp_true_nf_marked_mod",Pdp_true_nf_marked_mod,5},

  {"dp_true_nf_and_quotient",Pdp_true_nf_and_quotient,3},
  {"dp_true_nf_and_quotient_mod",Pdp_true_nf_and_quotient_mod,4},
  {"dp_true_nf_and_quotient_marked",Pdp_true_nf_and_quotient_marked,4},
  {"dp_true_nf_and_quotient_marked_mod",Pdp_true_nf_and_quotient_marked_mod,5},

  {"dp_lnf_mod",Pdp_lnf_mod,3},
  {"dp_nf_tab_f",Pdp_nf_tab_f,2},
  {"dp_nf_tab_mod",Pdp_nf_tab_mod,3},
  {"dp_lnf_f",Pdp_lnf_f,2},

  /* Buchberger algorithm */
  {"dp_gr_main",Pdp_gr_main,-5},
  {"dp_interreduce",Pdp_interreduce,3},
  {"dp_gr_mod_main",Pdp_gr_mod_main,5},
  {"dp_gr_f_main",Pdp_gr_f_main,4},
  {"dp_gr_checklist",Pdp_gr_checklist,2},
  {"nd_f4",Pnd_f4,-4},
  {"nd_gr",Pnd_gr,-4},
  {"nd_gr_trace",Pnd_gr_trace,-5},
  {"nd_f4_trace",Pnd_f4_trace,-5},
  {"nd_gr_postproc",Pnd_gr_postproc,5},
  {"nd_gr_recompute_trace",Pnd_gr_recompute_trace,5},
  {"nd_btog",Pnd_btog,-6},
  {"nd_weyl_gr_postproc",Pnd_weyl_gr_postproc,5},
  {"nd_weyl_gr",Pnd_weyl_gr,-4},
  {"nd_weyl_gr_trace",Pnd_weyl_gr_trace,-5},
  {"nd_nf",Pnd_nf,5},
  {"nd_weyl_nf",Pnd_weyl_nf,5},

  /* F4 algorithm */
  {"dp_f4_main",Pdp_f4_main,3},
  {"dp_f4_mod_main",Pdp_f4_mod_main,4},

/* weyl algebra */
  /* multiplication */
  {"dp_weyl_mul",Pdp_weyl_mul,2},
  {"dp_weyl_mul_mod",Pdp_weyl_mul_mod,3},
  {"dp_weyl_act",Pdp_weyl_act,2},

  /* s-poly */
  {"dp_weyl_sp",Pdp_weyl_sp,2},

  /* m-reduction */
  {"dp_weyl_red",Pdp_weyl_red,3},

  /* normal form */
  {"dp_weyl_nf",Pdp_weyl_nf,4},
  {"dpm_weyl_nf",Pdpm_weyl_nf,4},
  {"dp_weyl_nf_mod",Pdp_weyl_nf_mod,5},
  {"dp_weyl_nf_f",Pdp_weyl_nf_f,4},

  {"dp_weyl_true_nf_and_quotient",Pdp_weyl_true_nf_and_quotient,3},
  {"dp_weyl_true_nf_and_quotient_mod",Pdp_weyl_true_nf_and_quotient_mod,4},
  {"dp_weyl_true_nf_and_quotient_marked",Pdp_weyl_true_nf_and_quotient_marked,4},
  {"dp_weyl_true_nf_and_quotient_marked_mod",Pdp_weyl_true_nf_and_quotient_marked_mod,5},


  /* Buchberger algorithm */
  {"dp_weyl_gr_main",Pdp_weyl_gr_main,-5},
  {"dp_weyl_gr_mod_main",Pdp_weyl_gr_mod_main,5},
  {"dp_weyl_gr_f_main",Pdp_weyl_gr_f_main,4},

  /* F4 algorithm */
  {"dp_weyl_f4_main",Pdp_weyl_f4_main,3},
  {"dp_weyl_f4_mod_main",Pdp_weyl_f4_mod_main,4},

  /* Hilbert function */
  {"dp_monomial_hilbert_poincare",Pdp_monomial_hilbert_poincare,2},

  /* misc */
  {"dp_inv_or_split",Pdp_inv_or_split,3},
  {"dp_set_weight",Pdp_set_weight,-1},
  {"dp_set_module_weight",Pdp_set_module_weight,-1},
  {"dp_set_top_weight",Pdp_set_top_weight,-1},
  {"dp_weyl_set_weight",Pdp_weyl_set_weight,-1},

  {"dp_get_denomlist",Pdp_get_denomlist,0},
  {0,0,0},
};

struct ftab dp_supp_tab[] = {
  /* setting flags */
  {"dp_sort",Pdp_sort,1},
  {"dp_ord",Pdp_ord,-1},
  {"dpm_set_schreyer",Pdpm_set_schreyer,-1},
  {"dpv_ord",Pdpv_ord,-2},
  {"dp_set_kara",Pdp_set_kara,-1},
  {"dp_nelim",Pdp_nelim,-1},
  {"dp_gr_flags",Pdp_gr_flags,-1},
  {"dp_gr_print",Pdp_gr_print,-1},

  /* converters */
  {"homogenize",Phomogenize,3},
  {"dp_ptod",Pdp_ptod,-2},
  {"dp_dtop",Pdp_dtop,2},
  {"dp_homo",Pdp_homo,1},
  {"dp_dehomo",Pdp_dehomo,1},
  {"dp_etov",Pdp_etov,1},
  {"dp_vtoe",Pdp_vtoe,1},
  {"dp_dtov",Pdp_dtov,1},
  {"dp_mdtod",Pdp_mdtod,1},
  {"dp_mod",Pdp_mod,3},
  {"dp_rat",Pdp_rat,1},
  {"dp_ltod",Pdp_ltod,-2},

  {"dpm_ltod",Pdpm_ltod,2},
  {"dpm_dptodpm",Pdpm_dptodpm,2},
  {"dpm_dtol",Pdpm_dtol,3},

  /* criteria */
  {"dp_cri1",Pdp_cri1,2},
  {"dp_cri2",Pdp_cri2,2},
  {"dp_criB",Pdp_criB,3},

  /* simple operation */
  {"dp_subd",Pdp_subd,2},
  {"dp_lcm",Pdp_lcm,2},
  {"dp_hm",Pdp_hm,1},
  {"dp_ht",Pdp_ht,1},
  {"dp_hc",Pdp_hc,1},
  {"dpv_hm",Pdpv_hm,1},
  {"dpv_ht",Pdpv_ht,1},
  {"dpv_hc",Pdpv_hc,1},
  {"dpm_hm",Pdpm_hm,1},
  {"dpm_ht",Pdpm_ht,1},
  {"dpm_hc",Pdpm_hc,1},
  {"dpm_hp",Pdpm_hp,1},
  {"dpm_rest",Pdpm_rest,1},
  {"dpm_shift",Pdpm_shift,2},
  {"dpm_split",Pdpm_split,2},
  {"dpm_sort",Pdpm_sort,1},
  {"dp_rest",Pdp_rest,1},
  {"dp_initial_term",Pdp_initial_term,1},
  {"dp_order",Pdp_order,1},
  {"dp_symb_add",Pdp_symb_add,2},

  /* degree and size */
  {"dp_td",Pdp_td,1},
  {"dp_mag",Pdp_mag,1},
  {"dp_sugar",Pdp_sugar,1},
  {"dp_set_sugar",Pdp_set_sugar,2},

  /* misc */
  {"dp_mbase",Pdp_mbase,1},
  {"dp_redble",Pdp_redble,2},
  {"dpm_redble",Pdpm_redble,2},
  {"dp_sep",Pdp_sep,2},
  {"dp_idiv",Pdp_idiv,2},
  {"dp_tdiv",Pdp_tdiv,2},
  {"dp_minp",Pdp_minp,2},
  {"dp_compute_last_w",Pdp_compute_last_w,5},
  {"dp_compute_last_t",Pdp_compute_last_t,5},
  {"dp_compute_essential_df",Pdp_compute_essential_df,2},
  {"dp_mono_raddec",Pdp_mono_raddec,2},
  {"dp_mono_reduce",Pdp_mono_reduce,2},

  {"dp_rref2",Pdp_rref2,2},
  {"sumi_updatepairs",Psumi_updatepairs,3},
  {"sumi_symbolic",Psumi_symbolic,5},

  {0,0,0}
};

NODE compute_last_w(NODE g,NODE gh,int n,int **v,int row1,int **m1,int row2,int **m2);
Q compute_last_t(NODE g,NODE gh,Q t,VECT w1,VECT w2,NODE *homo,VECT *wp);

int comp_by_tdeg(DP *a,DP *b)
{
  int da,db;

  da = BDY(*a)->dl->td;
  db = BDY(*b)->dl->td;
  if ( da>db ) return 1;
  else if ( da<db ) return -1;
  else return 0;
}

void dl_print(DL d,int n)
{
  int i;

  printf("<<");
  for ( i = 0; i < n; i++ )
    printf("%d ",d->d[i]);
  printf(">>\n");
}

int simple_check(VECT b,int nv)
{
  int n,i,j;
  DL *p;

  n = b->len; p = (DL *)b->body;
  for ( i = 0; i < n; i++ ) {
    for ( j = 0; j < nv; j++ ) {
      if ( p[i]->d[j] ) break;
    }
    if ( p[i]->d[j] != p[i]->td ) return 0;
  }
  return 1;
}

void make_reduced(VECT b,int nv)
{
  int n,i,j;
  DL *p;
  DL pi;

  n = b->len;
  p = (DL *)BDY(b);
  for ( i = 0; i < n; i++ ) {
    pi = p[i];
    if ( !pi ) continue;
    for ( j = 0; j < n; j++ )
      if ( i != j && p[j] && _dl_redble(pi,p[j],nv) ) p[j] = 0;
  }
  for ( i = j = 0; i < n; i++ )
    if ( p[i] ) p[j++] = p[i];
  b->len = j;
}

void make_reduced2(VECT b,int k,int nv)
{
  int n,i,j,l;
  DL *p;
  DL pi;

  n = b->len;
  p = (DL *)BDY(b);
  for ( i = l = k; i < n; i++ ) {
    pi = p[i];
    for ( j = 0; j < k; j++ )
      if ( _dl_redble(p[j],pi,nv) ) break;
    if ( j == k )
     p[l++] = pi;
  }
  b->len = l;
}
 
int i_all,i_simple;

P mhp_simple(VECT b,VECT x,P t)
{
  int n,i,j,nv;
  DL *p;
  P hp,mt,s,w;
  Z z;

  n = b->len; nv = x->len; p = (DL *)BDY(b);
  hp = (P)ONE;
  for ( i = 0; i < n; i++ ) {
    for ( j = 0; j < nv; j++ )
      if ( p[i]->d[j] ) break;
    STOZ(p[i]->d[j],z); 
    chsgnp(t,&mt); mt->dc->d =z;
    addp(CO,mt,(P)ONE,&s); mulp(CO,hp,s,&w); hp = w;
  }
  return hp;
}

struct oEGT eg_comp;

void mhp_rec(VECT b,VECT x,P t,P *r)
{
  int n,i,j,k,l,i2,nv,len;
  int *d;
  Z mone,z;
  DCP dc,dc1;
  P s;
  P *r2;
  DL *p,*q;
  DL pi,xj,d1;
  VECT c;
struct oEGT eg0,eg1;

  i_all++;
  n = b->len; nv = x->len; p = (DL *)BDY(b);
  if ( !n ) {
    r[0] = (P)ONE;
    return;
  }
  if ( n == 1 && p[0]->td == 0 )
    return;
  for ( i = 0; i < n; i++ )
    if ( p[i]->td > 1 ) break;
  if ( i == n ) {
    r[n] = (P)ONE;
    return;
  }
#if 0
  if ( simple_check(b,nv) ) {
    i_simple++;
    r[0] = mhp_simple(b,x,t);
    return;
  }
#endif
  for ( j = 0, d = p[i]->d; j < nv; j++ )
    if ( d[j] ) break;
  xj = BDY(x)[j];
  MKVECT(c,n); q = (DL *)BDY(c);
  for ( i = k = l = 0; i < n; i++ )
    if ( p[i]->d[j] ) {
      pi = p[i];
      NEWDL(d1,nv); d1->td =pi->td - 1;
      memcpy(d1->d,pi->d,nv*sizeof(int));
      d1->d[j]--;
      p[k++] = d1;
    } else
      q[l++] = p[i];
  for ( i = k, i2 = 0; i2 < l; i++, i2++ ) 
    p[i] = q[i2];
  /* b=(b[0]/xj,...,b[k-1]/xj,b[k],...b[n-1]) where
    b[0],...,b[k-1] are divisible by k */
  make_reduced2(b,k,nv);
  mhp_rec(b,x,t,r);
  /* c = (b[0],...,b[l-1],xj) */
  q[l] = xj; c->len = l+1;
  r2 = (P *)CALLOC(nv+1,sizeof(P));
  mhp_rec(c,x,t,r2);
// get_eg(&eg0);
  for ( i = 0; i <= nv; i++ ) {
    mulp(CO,r[i],t,&s); addp(CO,s,r2[i],&r[i]);
  }
// get_eg(&eg1); add_eg(&eg_comp,&eg0,&eg1);
}

/* (n+a)Cb as a polynomial of n; return (n+a)*...*(n+a-b+1) */

P binpoly(P n,int a,int b)
{
  Z z;
  P s,r,t;
  int i;

  STOZ(a,z); addp(CO,n,(P)z,&s); r = (P)ONE;
  for ( i = 0; i < b; i++ ) {
    mulp(CO,r,s,&t); r = t;
    subp(CO,s,(P)ONE,&t); s = t;
  }
  return r;
}

void ibin(unsigned long int n,unsigned long int k,Z *r);

void mhp_to_hf(VL vl,P hp,int n,P *plist,VECT *head,P *hf)
{
  P tv,gcd,q,h,hphead,tt,ai,hpoly,nv,bp,w;
  Z d,z;
  DCP dc,topdc;
  VECT hfhead;
  int i,s,qd;

  if ( !hp ) {
    MKVECT(hfhead,0); *head = hfhead;
    *hf = 0;
  } else {
    makevar("t",&tv);
    ezgcdp(CO,hp,plist[n],&gcd);
    if ( NUM(gcd) ) {
      s = n;
      q = hp;
    } else {
      s = n-ZTOS(DEG(DC(gcd)));
      divsp(CO,hp,plist[n-s],&q);
    }
    if ( NUM(q) ) qd = 0;
    else qd = ZTOS(DEG(DC(q)));
    if ( s == 0 ) {
      MKVECT(hfhead,qd+1);
      for ( i = 0; i <= qd; i++ ) {
        coefp(q,i,(P *)&BDY(hfhead)[i]);
      }
      *head = hfhead;
      *hf = 0;
    } else {
      if ( qd ) { 
        topdc = 0;
        for ( i = 0; i < qd; i++ ) {
          NEWDC(dc); NEXT(dc) = topdc;
          ibin(i+s-1,s-1,(Z *)&COEF(dc));
          STOZ(i,d); DEG(dc) = d;
          topdc = dc;
        }
        MKP(VR(tv),topdc,h);
        mulp(CO,h,q,&hphead);
      }
      MKVECT(hfhead,qd); 
      for ( i = 0; i < qd; i++ )
        coefp(hphead,i,(P *)&BDY(hfhead)[i]);
      *head = hfhead;
      hpoly = 0;
      makevar("n",&nv);
      for ( i = 0; i <= qd; i++ ) {
        coefp(q,i,&ai);
        bp = binpoly(nv,s-i-1,s-1);
        mulp(CO,ai,bp,&tt);
        addp(CO,hpoly,tt,&w);
        hpoly = w;
      }
      if ( s > 2 ) {
        factorialz(s-1,&z);
        divsp(CO,hpoly,(P)z,&tt); hpoly = tt;
      }
      *hf = hpoly;
      for ( i = qd-1; i >= 0; i-- ) {
        UTOZ(i,z);
        substp(CO,hpoly,VR(nv),(P)z,&tt);
        if ( cmpz((Z)tt,(Z)BDY(hfhead)[i]) ) break;
      }
      hfhead->len = i+1;
    }
  }
}

/* create (1,1-t,...,(1-t)^n) */

P *mhp_prep(int n,P *tv) {
  P *plist;
  P mt,t1;
  int i;

  plist = (P *)MALLOC((n+1)*sizeof(P));
  /* t1 = 1-t */
  makevar("t",tv); chsgnp(*tv,&mt); addp(CO,mt,(P)ONE,&t1);
  for ( plist[0] = (P)ONE, i = 1; i <= n; i++ )
    mulp(CO,plist[i-1],t1,&plist[i]);
  return plist;
}

P mhp_ctop(P *r,P *plist,int n)
{
  int i;
  P hp,u,w;

  for ( hp = 0, i = 0; i <= n; i++ ) {
    mulp(CO,plist[i],r[i],&u); addp(CO,u,hp,&w); hp = w;
  }
  return hp;
}

void Pdp_monomial_hilbert_poincare(NODE arg,LIST *rp)
{
  LIST g,v;
  VL vl;
  int m,n,i;
  VECT b,x,hfhead;
  NODE t,nd;
  Z z,den;
  P hp,tv,mt,t1,u,w,hpoly;
  DP a;
  DL *p;
  P *plist,*r;
  Obj val;

  i_simple = i_all = 0;
  g = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  pltovl(v,&vl);
  m = length(BDY(g)); MKVECT(b,m); p = (DL *)BDY(b);
  for ( t = BDY(g), i = 0; t; t = NEXT(t), i++ ) {
    if ( !BDY(t) )
      p[i] = 0;
    else {
      ptod(CO,vl,(P)BDY(t),&a); p[i] = BDY(a)->dl;
    }
  }
  n = length(BDY(v)); MKVECT(x,n); p = (DL *)BDY(x);
  for ( t = BDY(v), i = 0; t; t = NEXT(t), i++ ) {
    ptod(CO,vl,(P)BDY(t),&a); p[i] = BDY(a)->dl;
  }
  
  r = (P *)CALLOC(n+1,sizeof(P));
  plist = mhp_prep(n,&tv);
  make_reduced(b,n);
  mhp_rec(b,x,tv,r);
  hp = mhp_ctop(r,plist,n);
  mhp_to_hf(CO,hp,n,plist,&hfhead,&hpoly);
  UTOZ(n,z);
  nd = mknode(4,hp,z,hfhead,hpoly);
  MKLIST(*rp,nd);
}

void Pdp_compute_last_t(NODE arg,LIST *rp)
{
  NODE g,gh,homo,n;
  LIST hlist;
  VECT v1,v2,w;
  Q t;

  g = (NODE)BDY((LIST)ARG0(arg));
  gh = (NODE)BDY((LIST)ARG1(arg));
  t = (Q)ARG2(arg);
  v1 = (VECT)ARG3(arg);
  v2 = (VECT)ARG4(arg);
  t = compute_last_t(g,gh,t,v1,v2,&homo,&w);
  MKLIST(hlist,homo);
  n = mknode(3,t,w,hlist);
  MKLIST(*rp,n);
}

void Pdp_compute_last_w(NODE arg,LIST *rp)
{
  NODE g,gh,r;
  VECT w,rv;
  LIST l;
  MAT w1,w2;
  int row1,row2,i,j,n;
  int *v;
  int **m1,**m2;
  Z q;

  g = (NODE)BDY((LIST)ARG0(arg));
  gh = (NODE)BDY((LIST)ARG1(arg));
  w = (VECT)ARG2(arg);
  w1 = (MAT)ARG3(arg);
  w2 = (MAT)ARG4(arg);
  n = w1->col;
  row1 = w1->row;
  row2 = w2->row;
  if ( w ) {
    v = W_ALLOC(n);
    for ( i = 0; i < n; i++ ) v[i] = ZTOS((Q)w->body[i]);
  } else v = 0;
  m1 = almat(row1,n);
  for ( i = 0; i < row1; i++ )
    for ( j = 0; j < n; j++ ) m1[i][j] = ZTOS((Q)w1->body[i][j]);
  m2 = almat(row2,n);
  for ( i = 0; i < row2; i++ )
    for ( j = 0; j < n; j++ ) m2[i][j] = ZTOS((Q)w2->body[i][j]);
  r = compute_last_w(g,gh,n,&v,row1,m1,row2,m2);
  if ( !r ) *rp = 0;
  else {
    MKVECT(rv,n);
    for ( i = 0; i < n; i++ ) {
      STOZ(v[i],q); rv->body[i] = (pointer)q;
    }
    MKLIST(l,r);
    r = mknode(2,rv,l);
    MKLIST(*rp,r);
  }
}

NODE compute_essential_df(DP *g,DP *gh,int n);

void Pdp_compute_essential_df(NODE arg,LIST *rp)
{
  VECT g,gh;
  NODE r;

  g = (VECT)ARG0(arg);
  gh = (VECT)ARG1(arg);
  r = (NODE)compute_essential_df((DP *)BDY(g),(DP *)BDY(gh),g->len);
  MKLIST(*rp,r);
}

void Pdp_inv_or_split(NODE arg,Obj *rp)
{
  NODE gb,newgb;
  DP f,inv;
  struct order_spec *spec;
  LIST list;

  do_weyl = 0; dp_fcoeffs = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_inv_or_split");
  asir_assert(ARG1(arg),O_DP,"dp_inv_or_split");
  if ( !create_order_spec(0,(Obj)ARG2(arg),&spec) )
    error("dp_inv_or_split : invalid order specification");
  gb = BDY((LIST)ARG0(arg));
  f = (DP)ARG1(arg);
  newgb = (NODE)dp_inv_or_split(gb,f,spec,&inv);
  if ( !newgb ) {
    /* invertible */
    *rp = (Obj)inv;
  } else {
    MKLIST(list,newgb);
    *rp = (Obj)list;
  }
}

void Pdp_sort(NODE arg,DP *rp)
{
  dp_sort((DP)ARG0(arg),rp);
}

void Pdp_mdtod(NODE arg,DP *rp)
{
  MP m,mr,mr0;
  DP p;
  P t;

  p = (DP)ARG0(arg);
  if ( !p )
    *rp = 0;
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      mptop((P)m->c,&t); NEXTMP(mr0,mr); mr->c = (Obj)t; mr->dl = m->dl;
    }
    NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
  }
}

void Pdp_sep(NODE arg,VECT *rp)
{
  DP p,r;
  MP m,t;
  MP *w0,*w;
  int i,n,d,nv,sugar;
  VECT v;
  pointer *pv;

  p = (DP)ARG0(arg); m = BDY(p);
  d = ZTOS((Q)ARG1(arg));
  for ( t = m, n = 0; t; t = NEXT(t), n++ );
  if ( d > n )
    d = n;
  MKVECT(v,d); *rp = v;
  pv = BDY(v); nv = p->nv; sugar = p->sugar;
  w0 = (MP *)MALLOC(d*sizeof(MP)); bzero(w0,d*sizeof(MP));
  w = (MP *)MALLOC(d*sizeof(MP)); bzero(w,d*sizeof(MP));
  for ( t = BDY(p), i = 0; t; t = NEXT(t), i++, i %= d  ) {
    NEXTMP(w0[i],w[i]); w[i]->c = t->c; w[i]->dl = t->dl;
  }
  for ( i = 0; i < d; i++ ) {
    NEXT(w[i]) = 0; MKDP(nv,w0[i],r); r->sugar = sugar;
    pv[i] = (pointer)r;
  }
}

void Pdp_idiv(NODE arg,DP *rp)
{
  dp_idiv((DP)ARG0(arg),(Z)ARG1(arg),rp);
}

void Pdp_cont(NODE arg,Z *rp)
{
  dp_cont((DP)ARG0(arg),rp);
}

void Pdp_dtov(NODE arg,VECT *rp)
{
  dp_dtov((DP)ARG0(arg),rp);
}

void Pdp_mbase(NODE arg,LIST *rp)
{
  NODE mb;

  asir_assert(ARG0(arg),O_LIST,"dp_mbase");
  dp_mbase(BDY((LIST)ARG0(arg)),&mb);
  MKLIST(*rp,mb);
}

void Pdp_etov(NODE arg,VECT *rp)
{
  DP dp;
  int n,i;
  int *d;
  VECT v;
  Z t;

  dp = (DP)ARG0(arg);
  asir_assert(dp,O_DP,"dp_etov");
  n = dp->nv; d = BDY(dp)->dl->d;
  MKVECT(v,n);
  for ( i = 0; i < n; i++ ) {
    STOZ(d[i],t); v->body[i] = (pointer)t;
  }
  *rp = v;
}

void Pdp_vtoe(NODE arg,DP *rp)
{
  DP dp;
  DL dl;
  MP m;
  int n,i,td;
  int *d;
  VECT v;

  v = (VECT)ARG0(arg);
  asir_assert(v,O_VECT,"dp_vtoe");
  n = v->len;
  NEWDL(dl,n); d = dl->d;
  for ( i = 0, td = 0; i < n; i++ ) {
    d[i] = ZTOS((Q)(v->body[i])); td += MUL_WEIGHT(d[i],i);
  }
  dl->td = td;
  NEWMP(m); m->dl = dl; m->c = (Obj)ONE; NEXT(m) = 0;
  MKDP(n,m,dp); dp->sugar = td;
  *rp = dp;
}

void Pdp_lnf_mod(NODE arg,LIST *rp)
{
  DP r1,r2;
  NODE b,g,n;
  int mod;

  asir_assert(ARG0(arg),O_LIST,"dp_lnf_mod");
  asir_assert(ARG1(arg),O_LIST,"dp_lnf_mod");
  asir_assert(ARG2(arg),O_N,"dp_lnf_mod");
  b = BDY((LIST)ARG0(arg)); g = BDY((LIST)ARG1(arg));
  mod = ZTOS((Q)ARG2(arg));
  dp_lnf_mod((DP)BDY(b),(DP)BDY(NEXT(b)),g,mod,&r1,&r2);
  NEWNODE(n); BDY(n) = (pointer)r1;
  NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)r2;
  NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_lnf_f(NODE arg,LIST *rp)
{
  DP r1,r2;
  NODE b,g,n;

  asir_assert(ARG0(arg),O_LIST,"dp_lnf_f");
  asir_assert(ARG1(arg),O_LIST,"dp_lnf_f");
  b = BDY((LIST)ARG0(arg)); g = BDY((LIST)ARG1(arg));
  dp_lnf_f((DP)BDY(b),(DP)BDY(NEXT(b)),g,&r1,&r2);
  NEWNODE(n); BDY(n) = (pointer)r1;
  NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)r2;
  NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_nf_tab_mod(NODE arg,DP *rp)
{
  asir_assert(ARG0(arg),O_DP,"dp_nf_tab_mod");
  asir_assert(ARG1(arg),O_VECT,"dp_nf_tab_mod");
  asir_assert(ARG2(arg),O_N,"dp_nf_tab_mod");
  dp_nf_tab_mod((DP)ARG0(arg),(LIST *)BDY((VECT)ARG1(arg)),
    ZTOS((Q)ARG2(arg)),rp);
}

void Pdp_nf_tab_f(NODE arg,DP *rp)
{
  asir_assert(ARG0(arg),O_DP,"dp_nf_tab_f");
  asir_assert(ARG1(arg),O_VECT,"dp_nf_tab_f");
  dp_nf_tab_f((DP)ARG0(arg),(LIST *)BDY((VECT)ARG1(arg)),rp);
}

extern int dpm_ordtype;

void Pdp_ord(NODE arg,Obj *rp)
{
  struct order_spec *spec;
  LIST v;
  struct oLIST f;
  Num homo;
  int modular;
  
  f.id = O_LIST; f.body = 0;
  if ( !arg && !current_option )
    *rp = dp_current_spec->obj;
  else {
    if ( current_option )
      parse_gr_option(&f,current_option,&v,&homo,&modular,&spec);
    else if ( !create_order_spec(0,(Obj)ARG0(arg),&spec) )
      error("dp_ord : invalid order specification");
    initd(spec); *rp = spec->obj;
    if ( spec->id >= 256 ) dpm_ordtype = spec->ispot; 
  }
}

void Pdp_ptod(NODE arg,DP *rp)
{
  P p;
  NODE n;
  VL vl,tvl;
  struct oLIST f;
  int ac;
  LIST v;
  Num homo;
  int modular;
  struct order_spec *ord;

  asir_assert(ARG0(arg),O_P,"dp_ptod");
  p = (P)ARG0(arg);
  ac = argc(arg);
  if ( ac == 1 ) {
    if ( current_option ) {
      f.id = O_LIST; f.body = mknode(1,p);
      parse_gr_option(&f,current_option,&v,&homo,&modular,&ord);
      initd(ord);
    } else
      error("dp_ptod : invalid argument");
  } else {
    asir_assert(ARG1(arg),O_LIST,"dp_ptod");
    v = (LIST)ARG1(arg);
  }
  for ( vl = 0, n = BDY(v); n; n = NEXT(n) ) {
    if ( !vl ) {
      NEWVL(vl); tvl = vl;
    } else {
      NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
    }
    VR(tvl) = VR((P)BDY(n));
  }
  if ( vl )
    NEXT(tvl) = 0;
  ptod(CO,vl,p,rp);
}

void Phomogenize(NODE arg,Obj *rp)
{
  P p;
  DP d,h;
  NODE n;
  V hv;
  VL vl,tvl,last;
  struct oLIST f;
  LIST v;

  asir_assert(ARG0(arg),O_P,"homogenize");
  p = (P)ARG0(arg);
  asir_assert(ARG1(arg),O_LIST,"homogenize");
  v = (LIST)ARG1(arg);
  asir_assert(ARG2(arg),O_P,"homogenize");
  hv = VR((P)ARG2(arg));
  for ( vl = 0, n = BDY(v); n; n = NEXT(n) ) {
    if ( !vl ) {
      NEWVL(vl); tvl = vl;
    } else {
      NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
    }
    VR(tvl) = VR((P)BDY(n));
  }
  if ( vl ) {
    last = tvl;
    NEXT(tvl) = 0;
  }
  ptod(CO,vl,p,&d);
  dp_homo(d,&h);
  NEWVL(NEXT(last)); last = NEXT(last);
  VR(last) = hv; NEXT(last) = 0;
  dtop(CO,vl,h,rp);
}

void Pdp_ltod(NODE arg,DPV *rp)
{
  NODE n;
  VL vl,tvl;
  LIST f,v;
  int sugar,i,len,ac,modular;
  Num homo;
  struct order_spec *ord;
  DP *e;
  NODE nd,t;

  ac = argc(arg);
  asir_assert(ARG0(arg),O_LIST,"dp_ptod");
  f = (LIST)ARG0(arg);
  if ( ac == 1 ) {
    if ( current_option ) {
      parse_gr_option(f,current_option,&v,&homo,&modular,&ord);
      initd(ord);
    } else
      error("dp_ltod : invalid argument");
  } else {
    asir_assert(ARG1(arg),O_LIST,"dp_ptod");
    v = (LIST)ARG1(arg);
  }
  for ( vl = 0, n = BDY(v); n; n = NEXT(n) ) {
    if ( !vl ) {
      NEWVL(vl); tvl = vl;
    } else {
      NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
    }
    VR(tvl) = VR((P)BDY(n));
  }
  if ( vl )
    NEXT(tvl) = 0;

  nd = BDY(f);
  len = length(nd);
  e = (DP *)MALLOC(len*sizeof(DP));
  sugar = 0;
  for ( i = 0, t = nd; i < len; i++, t = NEXT(t) ) {
    ptod(CO,vl,(P)BDY(t),&e[i]);
    if ( e[i] )
      sugar = MAX(sugar,e[i]->sugar);
  }
  MKDPV(len,e,*rp);
}

void Pdpm_ltod(NODE arg,DPM *rp)
{
  NODE n;
  VL vl,tvl;
  LIST f,v;
  int i,len;
  NODE nd;
  NODE t;
  DP d;
  DPM s,u,w;

  f = (LIST)ARG0(arg);
  v = (LIST)ARG1(arg);
  for ( vl = 0, n = BDY(v); n; n = NEXT(n) ) {
    if ( !vl ) {
      NEWVL(vl); tvl = vl;
    } else {
      NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
    }
    VR(tvl) = VR((P)BDY(n));
  }
  if ( vl )
    NEXT(tvl) = 0;

  nd = BDY(f);
  len = length(nd);
  for ( i = 1, t = nd, s = 0; i <= len; i++, t = NEXT(t) ) {
    ptod(CO,vl,(P)BDY(t),&d);
    dtodpm(d,i,&u);
    adddpm(CO,s,u,&w); s = w;
  }
  *rp = s;
}

// c*[monomial,i]+... -> c*<<monomial:i>>+...

void Pdpm_dptodpm(NODE arg,DPM *rp)
{
  DP p;
  MP mp;
  int pos;
  DMM m0,m;

  p = (DP)ARG0(arg);
  pos = ZTOS((Z)ARG1(arg));
  if ( pos <= 0 )
    error("dpm_mtod : position must be positive");
  if ( !p ) *rp = 0;
  else {
    for ( m0 = 0, mp = BDY(p); mp; mp = NEXT(mp) ) {
      NEXTDMM(m0,m); m->dl = mp->dl; m->c = mp->c; m->pos = pos;
    }
    MKDPM(p->nv,m0,*rp); (*rp)->sugar = p->sugar;
  }
}

void Pdpm_dtol(NODE arg,LIST *rp)
{
  DPM a;
  NODE nd,nd1;
  VL vl,tvl;
  int n,len,i,pos,nv;
  MP *w;
  DMM t;
  DMM *wa;
  MP m;
  DP u;
  Obj s;

  a = (DPM)ARG0(arg);
  for ( vl = 0, nd = BDY((LIST)ARG1(arg)), nv = 0; nd; nd = NEXT(nd), nv++ ) {
    if ( !vl ) {
      NEWVL(vl); tvl = vl;
    } else {
      NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
    }
    VR(tvl) = VR((P)BDY(nd));
  }
  if ( vl )
    NEXT(tvl) = 0;
   n = ZTOS((Q)ARG2(arg));
   w = (MP *)CALLOC(n,sizeof(MP));
   for ( t = BDY(a), len = 0; t; t = NEXT(t) ) len++;
   wa = (DMM *)MALLOC(len*sizeof(DMM));
   for ( t = BDY(a), i = 0; t; t = NEXT(t), i++ ) wa[i] = t;
   for ( i = len-1; i >= 0; i-- ) {
     NEWMP(m); m->dl = wa[i]->dl; C(m) = C(wa[i]);
     pos = wa[i]->pos;
     NEXT(m) = w[pos];
     w[pos] = m;
   }
  nd = 0;
  for ( i = n-1; i >= 0; i-- ) {
    MKDP(nv,w[i],u); u->sugar = a->sugar; /* XXX */
    dtop(CO,vl,u,&s);
    MKNODE(nd1,s,nd); nd = nd1;
  }
  MKLIST(*rp,nd);
}

void Pdp_dtop(NODE arg,Obj *rp)
{
  NODE n;
  VL vl,tvl;

  asir_assert(ARG0(arg),O_DP,"dp_dtop");
  asir_assert(ARG1(arg),O_LIST,"dp_dtop");
  for ( vl = 0, n = BDY((LIST)ARG1(arg)); n; n = NEXT(n) ) {
    if ( !vl ) {
      NEWVL(vl); tvl = vl;
    } else {
      NEWVL(NEXT(tvl)); tvl = NEXT(tvl);
    }
    VR(tvl) = VR((P)BDY(n));
  }
  if ( vl )
    NEXT(tvl) = 0;
  dtop(CO,vl,(DP)ARG0(arg),rp);
}

extern LIST Dist;

void Pdp_ptozp(NODE arg,Obj *rp)
{
  Z t;
  NODE tt,p;
  NODE n,n0;
  char *key;
  DP pp;
  LIST list;
  int get_factor=0;

  asir_assert(ARG0(arg),O_DP,"dp_ptozp");

    /* analyze the option */
    if ( current_option ) {
      for ( tt = current_option; tt; tt = NEXT(tt) ) {
        p = BDY((LIST)BDY(tt));
        key = BDY((STRING)BDY(p));
        /*  value = (Obj)BDY(NEXT(p)); */
        if ( !strcmp(key,"factor") )  get_factor=1;
        else {
          error("ptozp: unknown option.");
        }
      }
    }

  dp_ptozp3((DP)ARG0(arg),&t,&pp);

    /* printexpr(NULL,t); */
  /* if the option factor is given, then it returns the answer
       in the format [zpoly, num] where num*zpoly is equal to the argument.*/
    if (get_factor) {
    n0 = mknode(2,pp,t);
      MKLIST(list,n0);
    *rp = (Obj)list;
    } else
      *rp = (Obj)pp;
}
  
void Pdp_ptozp2(NODE arg,LIST *rp)
{
  DP p0,p1,h,r;
  NODE n0;

  p0 = (DP)ARG0(arg); p1 = (DP)ARG1(arg);
  asir_assert(p0,O_DP,"dp_ptozp2");
  asir_assert(p1,O_DP,"dp_ptozp2");
  dp_ptozp2(p0,p1,&h,&r);
  NEWNODE(n0); BDY(n0) = (pointer)h;
  NEWNODE(NEXT(n0)); BDY(NEXT(n0)) = (pointer)r;
  NEXT(NEXT(n0)) = 0;
  MKLIST(*rp,n0);
}

void Pdp_prim(NODE arg,DP *rp)
{
  DP t;

  asir_assert(ARG0(arg),O_DP,"dp_prim");
  dp_prim((DP)ARG0(arg),&t); dp_ptozp(t,rp);
}
  
void Pdp_mod(NODE arg,DP *rp)
{
  DP p;
  int mod;
  NODE subst;

  asir_assert(ARG0(arg),O_DP,"dp_mod");
  asir_assert(ARG1(arg),O_N,"dp_mod");
  asir_assert(ARG2(arg),O_LIST,"dp_mod");
  p = (DP)ARG0(arg); mod = ZTOS((Q)ARG1(arg));
  subst = BDY((LIST)ARG2(arg));
  dp_mod(p,mod,subst,rp);
}

void Pdp_rat(NODE arg,DP *rp)
{
  asir_assert(ARG0(arg),O_DP,"dp_rat");
  dp_rat((DP)ARG0(arg),rp);
}

extern int DP_Multiple;

void Pdp_nf(NODE arg,DP *rp)
{
  NODE b;
  DP *ps;
  DP g;
  int full;

  do_weyl = 0; dp_fcoeffs = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_nf");
  asir_assert(ARG1(arg),O_DP,"dp_nf");
  asir_assert(ARG2(arg),O_VECT,"dp_nf");
  asir_assert(ARG3(arg),O_N,"dp_nf");
  if ( !(g = (DP)ARG1(arg)) ) {
    *rp = 0; return;
  }
  b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
  full = (Q)ARG3(arg) ? 1 : 0;
  dp_nf_z(b,g,ps,full,DP_Multiple,rp);
}

void Pdp_weyl_nf(NODE arg,DP *rp)
{
  NODE b;
  DP *ps;
  DP g;
  int full;

  asir_assert(ARG0(arg),O_LIST,"dp_weyl_nf");
  asir_assert(ARG1(arg),O_DP,"dp_weyl_nf");
  asir_assert(ARG2(arg),O_VECT,"dp_weyl_nf");
  asir_assert(ARG3(arg),O_N,"dp_weyl_nf");
  if ( !(g = (DP)ARG1(arg)) ) {
    *rp = 0; return;
  }
  b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
  full = (Q)ARG3(arg) ? 1 : 0;
  do_weyl = 1;
  dp_nf_z(b,g,ps,full,DP_Multiple,rp);
  do_weyl = 0;
}

void Pdpm_nf(NODE arg,DPM *rp)
{
  NODE b;
  DPM *ps;
  DPM g;
  int full;

  if ( !(g = (DPM)ARG1(arg)) ) {
    *rp = 0; return;
  }
  do_weyl = 0; dp_fcoeffs = 0;
  asir_assert(ARG0(arg),O_LIST,"dpm_nf");
  asir_assert(ARG1(arg),O_DPM,"dpm_nf");
  asir_assert(ARG2(arg),O_VECT,"dpm_nf");
  asir_assert(ARG3(arg),O_N,"dpm_nf");
  b = BDY((LIST)ARG0(arg)); ps = (DPM *)BDY((VECT)ARG2(arg));
  full = (Q)ARG3(arg) ? 1 : 0;
  dpm_nf_z(b,g,ps,full,DP_Multiple,rp);
}

DP *dpm_nf_and_quotient(NODE b,DPM g,VECT ps,DPM *rp,P *dnp);

void Pdpm_nf_and_quotient(NODE arg,LIST *rp)
{
  NODE b;
  VECT ps;
  DPM g,nm;
  P dn;
  VECT quo;
  NODE n;
  int ac;

  do_weyl = 0; dp_fcoeffs = 0;
  ac = argc(arg);
  if ( ac < 2 )
    error("dpm_nf_and_quotient : invalid arguments");
  else if ( ac == 2 ) {
    asir_assert(ARG1(arg),O_VECT,"dpm_nf");
    b = 0; g = (DPM)ARG0(arg); ps = (VECT)ARG1(arg);
  } else if ( ac == 3 ) {
    asir_assert(ARG0(arg),O_LIST,"dpm_nf");
    asir_assert(ARG2(arg),O_VECT,"dpm_nf");
    b = BDY((LIST)ARG0(arg)); g = (DPM)ARG1(arg); ps = (VECT)ARG2(arg);
  }
  NEWVECT(quo); quo->len = ps->len;
  if ( g ) {
    quo->body = (pointer *)dpm_nf_and_quotient(b,g,ps,&nm,&dn);
  } else {
    quo->body = (pointer *)MALLOC(quo->len*sizeof(pointer));
    nm = 0; dn = (P)ONE;
  }
  n = mknode(3,nm,dn,quo);
  MKLIST(*rp,n);
}

void Pdpm_weyl_nf(NODE arg,DPM *rp)
{
  NODE b;
  DPM *ps;
  DPM g;
  int full;

  if ( !(g = (DPM)ARG1(arg)) ) {
    *rp = 0; return;
  }
  asir_assert(ARG0(arg),O_LIST,"dpm_weyl_nf");
  asir_assert(ARG1(arg),O_DPM,"dpm_weyl_nf");
  asir_assert(ARG2(arg),O_VECT,"dpm_weyl_nf");
  asir_assert(ARG3(arg),O_N,"dpm_weyl_nf");
  b = BDY((LIST)ARG0(arg)); ps = (DPM *)BDY((VECT)ARG2(arg));
  full = (Q)ARG3(arg) ? 1 : 0;
  do_weyl = 1;
  dpm_nf_z(b,g,ps,full,DP_Multiple,rp);
  do_weyl = 0;
}

/* nf computation using field operations */

void Pdp_nf_f(NODE arg,DP *rp)
{
  NODE b;
  DP *ps;
  DP g;
  int full;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_nf_f");
  asir_assert(ARG1(arg),O_DP,"dp_nf_f");
  asir_assert(ARG2(arg),O_VECT,"dp_nf_f");
  asir_assert(ARG3(arg),O_N,"dp_nf_f");
  if ( !(g = (DP)ARG1(arg)) ) {
    *rp = 0; return;
  }
  b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
  full = (Q)ARG3(arg) ? 1 : 0;
  dp_nf_f(b,g,ps,full,rp);
}

void Pdp_weyl_nf_f(NODE arg,DP *rp)
{
  NODE b;
  DP *ps;
  DP g;
  int full;

  asir_assert(ARG0(arg),O_LIST,"dp_weyl_nf_f");
  asir_assert(ARG1(arg),O_DP,"dp_weyl_nf_f");
  asir_assert(ARG2(arg),O_VECT,"dp_weyl_nf_f");
  asir_assert(ARG3(arg),O_N,"dp_weyl_nf_f");
  if ( !(g = (DP)ARG1(arg)) ) {
    *rp = 0; return;
  }
  b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
  full = (Q)ARG3(arg) ? 1 : 0;
  do_weyl = 1;
  dp_nf_f(b,g,ps,full,rp);
  do_weyl = 0;
}

void Pdpm_nf_f(NODE arg,DPM *rp)
{
  NODE b;
  DPM *ps;
  DPM g;
  int full;

  if ( !(g = (DPM)ARG1(arg)) ) {
    *rp = 0; return;
  }
  asir_assert(ARG0(arg),O_LIST,"dpm_nf_f");
  asir_assert(ARG1(arg),O_DPM,"dpm_nf_f");
  asir_assert(ARG2(arg),O_VECT,"dpm_nf_f");
  asir_assert(ARG3(arg),O_N,"dpm_nf_f");
  b = BDY((LIST)ARG0(arg)); ps = (DPM *)BDY((VECT)ARG2(arg));
  full = (Q)ARG3(arg) ? 1 : 0;
  dpm_nf_f(b,g,ps,full,rp);
}

void Pdpm_weyl_nf_f(NODE arg,DPM *rp)
{
  NODE b;
  DPM *ps;
  DPM g;
  int full;

  if ( !(g = (DPM)ARG1(arg)) ) {
    *rp = 0; return;
  }
  asir_assert(ARG0(arg),O_LIST,"dpm_weyl_nf_f");
  asir_assert(ARG1(arg),O_DP,"dpm_weyl_nf_f");
  asir_assert(ARG2(arg),O_VECT,"dpm_weyl_nf_f");
  asir_assert(ARG3(arg),O_N,"dpm_weyl_nf_f");
  b = BDY((LIST)ARG0(arg)); ps = (DPM *)BDY((VECT)ARG2(arg));
  full = (Q)ARG3(arg) ? 1 : 0;
  do_weyl = 1;
  dpm_nf_f(b,g,ps,full,rp);
  do_weyl = 0;
}


void Pdp_nf_mod(NODE arg,DP *rp)
{
  NODE b;
  DP g;
  DP *ps;
  int mod,full,ac;
  NODE n,n0;

  do_weyl = 0;
  ac = argc(arg);
  asir_assert(ARG0(arg),O_LIST,"dp_nf_mod");
  asir_assert(ARG1(arg),O_DP,"dp_nf_mod");
  asir_assert(ARG2(arg),O_VECT,"dp_nf_mod");
  asir_assert(ARG3(arg),O_N,"dp_nf_mod");
  asir_assert(ARG4(arg),O_N,"dp_nf_mod");
  if ( !(g = (DP)ARG1(arg)) ) {
    *rp = 0; return;
  }
  b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
  full = ZTOS((Q)ARG3(arg)); mod = ZTOS((Q)ARG4(arg));
  for ( n0 = n = 0; b; b = NEXT(b) ) {
    NEXTNODE(n0,n);
    BDY(n) = (pointer)ZTOS((Q)BDY(b));
  }
  if ( n0 )
    NEXT(n) = 0;
  dp_nf_mod(n0,g,ps,mod,full,rp);
}

void Pdp_true_nf(NODE arg,LIST *rp)
{
  NODE b,n;
  DP *ps;
  DP g;
  DP nm;
  P dn;
  int full;

  do_weyl = 0; dp_fcoeffs = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_true_nf");
  asir_assert(ARG1(arg),O_DP,"dp_true_nf");
  asir_assert(ARG2(arg),O_VECT,"dp_true_nf");
  asir_assert(ARG3(arg),O_N,"dp_nf");
  if ( !(g = (DP)ARG1(arg)) ) {
    nm = 0; dn = (P)ONE;
  } else {
    b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
    full = (Q)ARG3(arg) ? 1 : 0;
    dp_true_nf(b,g,ps,full,&nm,&dn);
  }
  NEWNODE(n); BDY(n) = (pointer)nm;
  NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)dn;
  NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

DP *dp_true_nf_and_quotient_marked(NODE b,DP g,DP *ps,DP *hps,DP *rp,P *dnp);

void Pdp_true_nf_and_quotient_marked(NODE arg,LIST *rp)
{
  NODE b,n;
  DP *ps,*hps;
  DP g;
  DP nm;
  VECT quo;
  P dn;
  int full;

  do_weyl = 0; dp_fcoeffs = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_true_nf_and_quotient_marked");
  asir_assert(ARG1(arg),O_DP,"dp_true_nf_and_quotient_marked");
  asir_assert(ARG2(arg),O_VECT,"dp_true_nf_and_quotient_marked");
  asir_assert(ARG3(arg),O_VECT,"dp_true_nf_and_quotient_marked");
  if ( !(g = (DP)ARG1(arg)) ) {
    nm = 0; dn = (P)ONE;
  } else {
    b = BDY((LIST)ARG0(arg)); 
    ps = (DP *)BDY((VECT)ARG2(arg));
    hps = (DP *)BDY((VECT)ARG3(arg));
    NEWVECT(quo); quo->len = ((VECT)ARG2(arg))->len;
    quo->body = (pointer *)dp_true_nf_and_quotient_marked(b,g,ps,hps,&nm,&dn);
  }
  n = mknode(3,nm,dn,quo);
  MKLIST(*rp,n);
}

void Pdp_true_nf_and_quotient(NODE arg,LIST *rp)
{
  NODE narg = mknode(4,ARG0(arg),ARG1(arg),ARG2(arg),ARG2(arg));
  Pdp_true_nf_and_quotient_marked(narg,rp);
}


DP *dp_true_nf_and_quotient_marked_mod (NODE b,DP g,DP *ps,DP *hps,int mod,DP *rp,P *dnp);

void Pdp_true_nf_and_quotient_marked_mod(NODE arg,LIST *rp)
{
  NODE b,n;
  DP *ps,*hps;
  DP g;
  DP nm;
  VECT quo;
  P dn;
  int full,mod;

  do_weyl = 0; dp_fcoeffs = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_true_nf_and_quotient_marked_mod");
  asir_assert(ARG1(arg),O_DP,"dp_true_nf_and_quotient_marked_mod");
  asir_assert(ARG2(arg),O_VECT,"dp_true_nf_and_quotient_marked_mod");
  asir_assert(ARG3(arg),O_VECT,"dp_true_nf_and_quotient_marked_mod");
  asir_assert(ARG4(arg),O_N,"dp_true_nf_and_quotient_marked_mod");
  if ( !(g = (DP)ARG1(arg)) ) {
    nm = 0; dn = (P)ONE;
  } else {
    b = BDY((LIST)ARG0(arg)); 
    ps = (DP *)BDY((VECT)ARG2(arg));
    hps = (DP *)BDY((VECT)ARG3(arg));
    mod = ZTOS((Q)ARG4(arg));
    NEWVECT(quo); quo->len = ((VECT)ARG2(arg))->len;
    quo->body = (pointer *)dp_true_nf_and_quotient_marked_mod(b,g,ps,hps,mod,&nm,&dn);
  }
  n = mknode(3,nm,dn,quo);
  MKLIST(*rp,n);
}

void Pdp_true_nf_and_quotient_mod(NODE arg,LIST *rp)
{
  NODE narg = mknode(5,ARG0(arg),ARG1(arg),ARG2(arg),ARG2(arg),ARG3(arg));
  Pdp_true_nf_and_quotient_marked_mod(narg,rp);
}

void Pdp_true_nf_marked(NODE arg,LIST *rp)
{
  NODE b,n;
  DP *ps,*hps;
  DP g;
  DP nm;
  Q cont;
  P dn;
  int full;

  do_weyl = 0; dp_fcoeffs = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_true_nf_marked");
  asir_assert(ARG1(arg),O_DP,"dp_true_nf_marked");
  asir_assert(ARG2(arg),O_VECT,"dp_true_nf_marked");
  asir_assert(ARG3(arg),O_VECT,"dp_true_nf_marked");
  if ( !(g = (DP)ARG1(arg)) ) {
    nm = 0; dn = (P)ONE;
  } else {
    b = BDY((LIST)ARG0(arg)); 
    ps = (DP *)BDY((VECT)ARG2(arg));
    hps = (DP *)BDY((VECT)ARG3(arg));
    dp_true_nf_marked(b,g,ps,hps,&nm,(P *)&cont,(P *)&dn);
  }
  n = mknode(3,nm,cont,dn);
  MKLIST(*rp,n);
}

void Pdp_true_nf_marked_mod(NODE arg,LIST *rp)
{
  NODE b,n;
  DP *ps,*hps;
  DP g;
  DP nm;
  P dn;
  int mod;

  do_weyl = 0; dp_fcoeffs = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_true_nf_marked_mod");
  asir_assert(ARG1(arg),O_DP,"dp_true_nf_marked_mod");
  asir_assert(ARG2(arg),O_VECT,"dp_true_nf_marked_mod");
  asir_assert(ARG3(arg),O_VECT,"dp_true_nf_marked_mod");
  asir_assert(ARG4(arg),O_N,"dp_true_nf_marked_mod");
  if ( !(g = (DP)ARG1(arg)) ) {
    nm = 0; dn = (P)ONE;
  } else {
    b = BDY((LIST)ARG0(arg)); 
    ps = (DP *)BDY((VECT)ARG2(arg));
    hps = (DP *)BDY((VECT)ARG3(arg));
    mod = ZTOS((Q)ARG4(arg));
    dp_true_nf_marked_mod(b,g,ps,hps,mod,&nm,&dn);
  }
  n = mknode(2,nm,dn);
  MKLIST(*rp,n);
}

void Pdp_weyl_nf_mod(NODE arg,DP *rp)
{
  NODE b;
  DP g;
  DP *ps;
  int mod,full,ac;
  NODE n,n0;

  ac = argc(arg);
  asir_assert(ARG0(arg),O_LIST,"dp_weyl_nf_mod");
  asir_assert(ARG1(arg),O_DP,"dp_weyl_nf_mod");
  asir_assert(ARG2(arg),O_VECT,"dp_weyl_nf_mod");
  asir_assert(ARG3(arg),O_N,"dp_weyl_nf_mod");
  asir_assert(ARG4(arg),O_N,"dp_weyl_nf_mod");
  if ( !(g = (DP)ARG1(arg)) ) {
    *rp = 0; return;
  }
  b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
  full = ZTOS((Q)ARG3(arg)); mod = ZTOS((Q)ARG4(arg));
  for ( n0 = n = 0; b; b = NEXT(b) ) {
    NEXTNODE(n0,n);
    BDY(n) = (pointer)ZTOS((Q)BDY(b));
  }
  if ( n0 )
    NEXT(n) = 0;
  do_weyl = 1;
  dp_nf_mod(n0,g,ps,mod,full,rp);
  do_weyl = 0;
}

void Pdp_true_nf_mod(NODE arg,LIST *rp)
{
  NODE b;
  DP g,nm;
  P dn;
  DP *ps;
  int mod,full;
  NODE n;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_nf_mod");
  asir_assert(ARG1(arg),O_DP,"dp_nf_mod");
  asir_assert(ARG2(arg),O_VECT,"dp_nf_mod");
  asir_assert(ARG3(arg),O_N,"dp_nf_mod");
  asir_assert(ARG4(arg),O_N,"dp_nf_mod");
  if ( !(g = (DP)ARG1(arg)) ) {
    nm = 0; dn = (P)ONEM;
  } else {
    b = BDY((LIST)ARG0(arg)); ps = (DP *)BDY((VECT)ARG2(arg));
    full = ZTOS((Q)ARG3(arg)); mod = ZTOS((Q)ARG4(arg));
    dp_true_nf_mod(b,g,ps,mod,full,&nm,&dn);
  }
  NEWNODE(n); BDY(n) = (pointer)nm;
  NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)dn;
  NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_weyl_true_nf_and_quotient_marked(NODE arg,LIST *rp)
{
  NODE b,n;
  DP *ps,*hps;
  DP g;
  DP nm;
  VECT quo;
  P dn;
  int full;

  do_weyl = 1; dp_fcoeffs = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_weyl_true_nf_and_quotient_marked");
  asir_assert(ARG1(arg),O_DP,"dp_weyl_true_nf_and_quotient_marked");
  asir_assert(ARG2(arg),O_VECT,"dp_weyl_true_nf_and_quotient_marked");
  asir_assert(ARG3(arg),O_VECT,"dp_weyl_true_nf_and_quotient_marked");
  if ( !(g = (DP)ARG1(arg)) ) {
    nm = 0; dn = (P)ONE;
  } else {
    b = BDY((LIST)ARG0(arg)); 
    ps = (DP *)BDY((VECT)ARG2(arg));
    hps = (DP *)BDY((VECT)ARG3(arg));
    NEWVECT(quo); quo->len = ((VECT)ARG2(arg))->len;
    quo->body = (pointer *)dp_true_nf_and_quotient_marked(b,g,ps,hps,&nm,&dn);
  }
  n = mknode(3,nm,dn,quo);
  MKLIST(*rp,n);
}

void Pdp_weyl_true_nf_and_quotient(NODE arg,LIST *rp)
{
  NODE narg = mknode(4,ARG0(arg),ARG1(arg),ARG2(arg),ARG2(arg));
  Pdp_weyl_true_nf_and_quotient_marked(narg,rp);
}


void Pdp_weyl_true_nf_and_quotient_marked_mod(NODE arg,LIST *rp)
{
  NODE b,n;
  DP *ps,*hps;
  DP g;
  DP nm;
  VECT quo;
  P dn;
  int full,mod;

  do_weyl = 1; dp_fcoeffs = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_weyl_true_nf_and_quotient_marked_mod");
  asir_assert(ARG1(arg),O_DP,"dp_weyl_true_nf_and_quotient_marked_mod");
  asir_assert(ARG2(arg),O_VECT,"dp_weyl_true_nf_and_quotient_marked_mod");
  asir_assert(ARG3(arg),O_VECT,"dp_weyl_true_nf_and_quotient_marked_mod");
  asir_assert(ARG4(arg),O_N,"dp_weyl_true_nf_and_quotient_marked_mod");
  if ( !(g = (DP)ARG1(arg)) ) {
    nm = 0; dn = (P)ONE;
  } else {
    b = BDY((LIST)ARG0(arg)); 
    ps = (DP *)BDY((VECT)ARG2(arg));
    hps = (DP *)BDY((VECT)ARG3(arg));
    mod = ZTOS((Q)ARG4(arg));
    NEWVECT(quo); quo->len = ((VECT)ARG2(arg))->len;
    quo->body = (pointer *)dp_true_nf_and_quotient_marked_mod(b,g,ps,hps,mod,&nm,&dn);
  }
  n = mknode(3,nm,dn,quo);
  MKLIST(*rp,n);
}

void Pdp_weyl_true_nf_and_quotient_mod(NODE arg,LIST *rp)
{
  NODE narg = mknode(5,ARG0(arg),ARG1(arg),ARG2(arg),ARG2(arg),ARG3(arg));
  Pdp_weyl_true_nf_and_quotient_marked_mod(narg,rp);
}


void Pdp_tdiv(NODE arg,DP *rp)
{
  MP m,mr,mr0;
  DP p;
  Z d,q,r;
  int sgn;

  asir_assert(ARG0(arg),O_DP,"dp_tdiv");
  asir_assert(ARG1(arg),O_N,"dp_tdiv");
  p = (DP)ARG0(arg); d = (Z)ARG1(arg);
  if ( !p )
    *rp = 0;
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      divqrz((Z)m->c,d,&q,&r);
      if ( r ) {
        *rp = 0; return;
      } else {
        NEXTMP(mr0,mr);
        mr->c = (Obj)q; mr->dl = m->dl;
      }
    }
    NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
  }
}

void Pdp_red_coef(NODE arg,DP *rp)
{
  MP m,mr,mr0;
  P q,r;
  DP p;
  P mod;

  p = (DP)ARG0(arg); mod = (P)ARG1(arg);
  asir_assert(p,O_DP,"dp_red_coef");
  asir_assert(mod,O_P,"dp_red_coef");
  if ( !p )
    *rp = 0;
  else {
    for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
      divsrp(CO,(P)m->c,mod,&q,&r);
      if ( r ) {
        NEXTMP(mr0,mr); mr->c = (Obj)r; mr->dl = m->dl;
      }
    }
    if ( mr0 ) {
      NEXT(mr) = 0; MKDP(p->nv,mr0,*rp); (*rp)->sugar = p->sugar;
    } else
      *rp = 0;
  }
}

void Pdp_redble(NODE arg,Z *rp)
{
  asir_assert(ARG0(arg),O_DP,"dp_redble");
  asir_assert(ARG1(arg),O_DP,"dp_redble");
  if ( dp_redble((DP)ARG0(arg),(DP)ARG1(arg)) )
    *rp = ONE;
  else
    *rp = 0;
}

void Pdpm_redble(NODE arg,Z *rp)
{
  asir_assert(ARG0(arg),O_DPM,"dpm_redble");
  asir_assert(ARG1(arg),O_DPM,"dpm_redble");
  if ( dpm_redble((DPM)ARG0(arg),(DPM)ARG1(arg)) )
    *rp = ONE;
  else
    *rp = 0;
}

void Pdp_red_mod(NODE arg,LIST *rp)
{
  DP h,r;
  P dmy;
  NODE n;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_DP,"dp_red_mod");
  asir_assert(ARG1(arg),O_DP,"dp_red_mod");
  asir_assert(ARG2(arg),O_DP,"dp_red_mod");
  asir_assert(ARG3(arg),O_N,"dp_red_mod");
  dp_red_mod((DP)ARG0(arg),(DP)ARG1(arg),(DP)ARG2(arg),ZTOS((Q)ARG3(arg)),
    &h,&r,&dmy);
  NEWNODE(n); BDY(n) = (pointer)h;
  NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)r;
  NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_subd(NODE arg,DP *rp)
{
  DP p1,p2;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_subd");
  asir_assert(p2,O_DP,"dp_subd");
  dp_subd(p1,p2,rp);
}

void Pdp_symb_add(NODE arg,DP *rp)
{
  DP p1,p2,r;
  NODE s0;
  MP mp0,mp;
  int nv;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_symb_add");
  asir_assert(p2,O_DP,"dp_symb_add");
  if ( !p1 ) { *rp = p2; return; }
  else if ( !p2 ) { *rp = p1; return; }
  if ( p1->nv != p2->nv )
    error("dp_sumb_add : invalid input");
  nv = p1->nv;
  s0 = symb_merge(dp_dllist(p1),dp_dllist(p2),nv);
  for ( mp0 = 0; s0; s0 = NEXT(s0) ) {
    NEXTMP(mp0,mp); mp->dl = (DL)BDY(s0); mp->c = (Obj)ONE;
  }
  NEXT(mp) = 0;
  MKDP(nv,mp0,r); r->sugar = MAX(p1->sugar,p2->sugar);
  *rp = r;
}

void Pdp_mul_trunc(NODE arg,DP *rp)
{
  DP p1,p2,p;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg); p = (DP)ARG2(arg);
  asir_assert(p1,O_DP,"dp_mul_trunc");
  asir_assert(p2,O_DP,"dp_mul_trunc");
  asir_assert(p,O_DP,"dp_mul_trunc");
  comm_muld_trunc(CO,p1,p2,BDY(p)->dl,rp);
}

void Pdp_quo(NODE arg,DP *rp)
{
  DP p1,p2;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_quo");
  asir_assert(p2,O_DP,"dp_quo");
  comm_quod(CO,p1,p2,rp);
}

void Pdp_weyl_mul(NODE arg,DP *rp)
{
  DP p1,p2;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_weyl_mul"); asir_assert(p2,O_DP,"dp_weyl_mul");
  do_weyl = 1;
  muld(CO,p1,p2,rp);
  do_weyl = 0;
}

void Pdp_weyl_act(NODE arg,DP *rp)
{
  DP p1,p2;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_weyl_act"); asir_assert(p2,O_DP,"dp_weyl_act");
  weyl_actd(CO,p1,p2,rp);
}


void Pdp_weyl_mul_mod(NODE arg,DP *rp)
{
  DP p1,p2;
  Q m;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg); m = (Q)ARG2(arg);
  asir_assert(p1,O_DP,"dp_weyl_mul_mod");
  asir_assert(p2,O_DP,"dp_mul_mod");
  asir_assert(m,O_N,"dp_mul_mod");
  do_weyl = 1;
  mulmd(CO,ZTOS(m),p1,p2,rp);
  do_weyl = 0;
}

void Pdp_red(NODE arg,LIST *rp)
{
  NODE n;
  DP head,rest,dmy1;
  P dmy;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_DP,"dp_red");
  asir_assert(ARG1(arg),O_DP,"dp_red");
  asir_assert(ARG2(arg),O_DP,"dp_red");
  dp_red((DP)ARG0(arg),(DP)ARG1(arg),(DP)ARG2(arg),&head,&rest,&dmy,&dmy1);
  NEWNODE(n); BDY(n) = (pointer)head;
  NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)rest;
  NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_weyl_red(NODE arg,LIST *rp)
{
  NODE n;
  DP head,rest,dmy1;
  P dmy;

  asir_assert(ARG0(arg),O_DP,"dp_weyl_red");
  asir_assert(ARG1(arg),O_DP,"dp_weyl_red");
  asir_assert(ARG2(arg),O_DP,"dp_weyl_red");
  do_weyl = 1;
  dp_red((DP)ARG0(arg),(DP)ARG1(arg),(DP)ARG2(arg),&head,&rest,&dmy,&dmy1);
  do_weyl = 0;
  NEWNODE(n); BDY(n) = (pointer)head;
  NEWNODE(NEXT(n)); BDY(NEXT(n)) = (pointer)rest;
  NEXT(NEXT(n)) = 0; MKLIST(*rp,n);
}

void Pdp_sp(NODE arg,DP *rp)
{
  DP p1,p2;

  do_weyl = 0;
  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_sp"); asir_assert(p2,O_DP,"dp_sp");
  dp_sp(p1,p2,rp);
}

void Pdp_weyl_sp(NODE arg,DP *rp)
{
  DP p1,p2;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_weyl_sp"); asir_assert(p2,O_DP,"dp_weyl_sp");
  do_weyl = 1;
  dp_sp(p1,p2,rp);
  do_weyl = 0;
}

void Pdpm_sp(NODE arg,Obj *rp)
{
  DPM  p1,p2,sp;
  DP mul1,mul2;
  Obj val;
  NODE nd;
  LIST l;

  do_weyl = 0;
  p1 = (DPM)ARG0(arg); p2 = (DPM)ARG1(arg);
  asir_assert(p1,O_DPM,"dpm_sp"); asir_assert(p2,O_DPM,"dpm_sp");
  dpm_sp(p1,p2,&sp,&mul1,&mul2);
  if ( get_opt("coef",&val) && val ) {
    nd = mknode(3,sp,mul1,mul2);
    MKLIST(l,nd);
    *rp = (Obj)l;
  } else {
    *rp = (Obj)sp;
  }
}

void Pdpm_weyl_sp(NODE arg,Obj *rp)
{
  DPM  p1,p2,sp;
  DP mul1,mul2;
  Obj val;
  NODE nd;
  LIST l;

  p1 = (DPM)ARG0(arg); p2 = (DPM)ARG1(arg);
  asir_assert(p1,O_DPM,"dpm_weyl_sp"); asir_assert(p2,O_DPM,"dpm_weyl_sp");
  do_weyl = 1;
  dpm_sp(p1,p2,&sp,&mul1,&mul2);
  do_weyl = 0;
  if ( get_opt("coef",&val) && val ) {
    nd = mknode(3,sp,mul1,mul2);
    MKLIST(l,nd);
    *rp = (Obj)l;
  } else {
    *rp = (Obj)sp;
  }
}

void Pdp_sp_mod(NODE arg,DP *rp)
{
  DP p1,p2;
  int mod;

  do_weyl = 0;
  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_sp_mod"); asir_assert(p2,O_DP,"dp_sp_mod");
  asir_assert(ARG2(arg),O_N,"dp_sp_mod");
  mod = ZTOS((Q)ARG2(arg));
  dp_sp_mod(p1,p2,mod,rp);
}

void Pdp_lcm(NODE arg,DP *rp)
{
  int i,n,td;
  DL d1,d2,d;
  MP m;
  DP p1,p2;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_lcm"); asir_assert(p2,O_DP,"dp_lcm");
  n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
  NEWDL(d,n);
  for ( i = 0, td = 0; i < n; i++ ) {
    d->d[i] = MAX(d1->d[i],d2->d[i]); td += MUL_WEIGHT(d->d[i],i);
  }
  d->td = td;
  NEWMP(m); m->dl = d; m->c = (Obj)ONE; NEXT(m) = 0;
  MKDP(n,m,*rp); (*rp)->sugar = td;  /* XXX */
}

void Pdp_hm(NODE arg,DP *rp)
{
  DP p;

  p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_hm");
  dp_hm(p,rp);
}

void Pdp_ht(NODE arg,DP *rp)
{
  DP p;
  MP m,mr;

  p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_ht");
  dp_ht(p,rp);
}

void Pdp_hc(NODE arg,Obj *rp)
{
  asir_assert(ARG0(arg),O_DP,"dp_hc");
  if ( !ARG0(arg) )
    *rp = 0;
  else
    *rp = BDY((DP)ARG0(arg))->c;
}

void Pdp_rest(NODE arg,DP *rp)
{
  asir_assert(ARG0(arg),O_DP,"dp_rest");
  if ( !ARG0(arg) )
    *rp = 0;
  else
    dp_rest((DP)ARG0(arg),rp);
}

void Pdp_td(NODE arg,Z *rp)
{
  DP p;

  p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_td");
  if ( !p )
    *rp = 0;
  else
    STOZ(BDY(p)->dl->td,*rp);
}

void Pdp_sugar(NODE arg,Z *rp)
{
  DP p;

  p = (DP)ARG0(arg); asir_assert(p,O_DP,"dp_sugar");
  if ( !p )
    *rp = 0;
  else
    STOZ(p->sugar,*rp);
}

void Pdp_initial_term(NODE arg,Obj *rp)
{
  struct order_spec *ord;
  Num homo;
  int modular,is_list;
  LIST v,f,l,initiallist;
  NODE n;

  f = (LIST)ARG0(arg);
  if ( f && OID(f) == O_LIST ) 
    is_list = 1;
  else {
    n = mknode(1,f); MKLIST(l,n); f = l;
    is_list = 0;
  }
  if ( current_option ) {
    parse_gr_option(f,current_option,&v,&homo,&modular,&ord);
    initd(ord);
  } else
    ord = dp_current_spec;
  initiallist = dp_initial_term(f,ord);    
  if ( !is_list )
    *rp = (Obj)BDY(BDY(initiallist));
  else
    *rp = (Obj)initiallist;
}

void Pdp_order(NODE arg,Obj *rp)
{
  struct order_spec *ord;
  Num homo;
  int modular,is_list;
  LIST v,f,l,ordlist;
  NODE n;

  f = (LIST)ARG0(arg);
  if ( f && OID(f) == O_LIST ) 
    is_list = 1;
  else {
    n = mknode(1,f); MKLIST(l,n); f = l;
    is_list = 0;
  }
  if ( current_option ) {
    parse_gr_option(f,current_option,&v,&homo,&modular,&ord);
    initd(ord);
  } else
    ord = dp_current_spec;
  ordlist = dp_order(f,ord);    
  if ( !is_list )
    *rp = (Obj)BDY(BDY(ordlist));
  else
    *rp = (Obj)ordlist;
}

void Pdp_set_sugar(NODE arg,Q *rp)
{
  DP p;
  Q q;
  int i;

  p = (DP)ARG0(arg);
  q = (Q)ARG1(arg);
  if ( p && q) {
    asir_assert(p,O_DP,"dp_set_sugar");
    asir_assert(q,O_N, "dp_set_sugar");
    i = ZTOS(q);
    if (p->sugar < i) {
      p->sugar = i;
    }
  }
  *rp = 0;
}

void Pdp_cri1(NODE arg,Z *rp)
{
  DP p1,p2;
  int *d1,*d2;
  int i,n;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_cri1"); asir_assert(p2,O_DP,"dp_cri1");
  n = p1->nv; d1 = BDY(p1)->dl->d; d2 = BDY(p2)->dl->d;
  for ( i = 0; i < n; i++ )
    if ( d1[i] > d2[i] )
      break;
  *rp = i == n ? ONE : 0;
}

void Pdp_cri2(NODE arg,Z *rp)
{
  DP p1,p2;
  int *d1,*d2;
  int i,n;

  p1 = (DP)ARG0(arg); p2 = (DP)ARG1(arg);
  asir_assert(p1,O_DP,"dp_cri2"); asir_assert(p2,O_DP,"dp_cri2");
  n = p1->nv; d1 = BDY(p1)->dl->d; d2 = BDY(p2)->dl->d;
  for ( i = 0; i < n; i++ )
    if ( MIN(d1[i],d2[i]) >= 1 )
      break;
  *rp = i == n ? ONE : 0;
}

void Pdp_minp(NODE arg,LIST *rp)
{
  NODE tn,tn1,d,dd,dd0,p,tp;
  LIST l,minp;
  DP lcm,tlcm;
  int s,ts;

  asir_assert(ARG0(arg),O_LIST,"dp_minp");
  d = BDY((LIST)ARG0(arg)); minp = (LIST)BDY(d);
  p = BDY(minp); p = NEXT(NEXT(p)); lcm = (DP)BDY(p); p = NEXT(p);
  if ( !ARG1(arg) ) {
    s = ZTOS((Q)BDY(p)); p = NEXT(p);
    for ( dd0 = 0, d = NEXT(d); d; d = NEXT(d) ) {
      tp = BDY((LIST)BDY(d)); tp = NEXT(NEXT(tp));
      tlcm = (DP)BDY(tp); tp = NEXT(tp);
      ts = ZTOS((Q)BDY(tp)); tp = NEXT(tp);
      NEXTNODE(dd0,dd);
      if ( ts < s ) {
        BDY(dd) = (pointer)minp;
        minp = (LIST)BDY(d); lcm = tlcm; s = ts;
      } else if ( ts == s ) {
        if ( compd(CO,lcm,tlcm) > 0 ) {
          BDY(dd) = (pointer)minp;
          minp = (LIST)BDY(d); lcm = tlcm; s = ts;
        } else
          BDY(dd) = BDY(d);
      } else
        BDY(dd) = BDY(d);
    }
  } else {
    for ( dd0 = 0, d = NEXT(d); d; d = NEXT(d) ) {
      tp = BDY((LIST)BDY(d)); tp = NEXT(NEXT(tp));
      tlcm = (DP)BDY(tp);
      NEXTNODE(dd0,dd);
      if ( compd(CO,lcm,tlcm) > 0 ) {
        BDY(dd) = (pointer)minp; minp = (LIST)BDY(d); lcm = tlcm;
      } else
        BDY(dd) = BDY(d);
    }
  }
  if ( dd0 )
    NEXT(dd) = 0;
  MKLIST(l,dd0); MKNODE(tn,l,0); MKNODE(tn1,minp,tn); MKLIST(*rp,tn1);
}

void Pdp_criB(NODE arg,LIST *rp)
{
  NODE d,ij,dd,ddd;
  int i,j,s,n;
  DP *ps;
  DL ts,ti,tj,lij,tdl;

  asir_assert(ARG0(arg),O_LIST,"dp_criB"); d = BDY((LIST)ARG0(arg));
  asir_assert(ARG1(arg),O_N,"dp_criB"); s = ZTOS((Q)ARG1(arg));
  asir_assert(ARG2(arg),O_VECT,"dp_criB"); ps = (DP *)BDY((VECT)ARG2(arg));
  if ( !d )
    *rp = (LIST)ARG0(arg);
  else {
    ts = BDY(ps[s])->dl;
    n = ps[s]->nv;
    NEWDL(tdl,n);
    for ( dd = 0; d; d = NEXT(d) ) {
      ij = BDY((LIST)BDY(d));
      i = ZTOS((Q)BDY(ij)); ij = NEXT(ij);
      j = ZTOS((Q)BDY(ij)); ij = NEXT(ij);
      lij = BDY((DP)BDY(ij))->dl;
      ti = BDY(ps[i])->dl; tj = BDY(ps[j])->dl;
      if ( lij->td != lcm_of_DL(n,lij,ts,tdl)->td
        || !dl_equal(n,lij,tdl)
        || (lij->td == lcm_of_DL(n,ti,ts,tdl)->td
          && dl_equal(n,tdl,lij))
        || (lij->td == lcm_of_DL(n,tj,ts,tdl)->td
          && dl_equal(n,tdl,lij)) ) {
        MKNODE(ddd,BDY(d),dd);
        dd = ddd;
      }
    }
    MKLIST(*rp,dd);
  }
}

void Pdp_nelim(NODE arg,Z *rp)
{
  if ( arg ) {
    asir_assert(ARG0(arg),O_N,"dp_nelim");
    dp_nelim = ZTOS((Q)ARG0(arg));
  }
  STOZ(dp_nelim,*rp);
}

void Pdp_mag(NODE arg,Z *rp)
{
  DP p;
  int s;
  MP m;

  p = (DP)ARG0(arg);
  asir_assert(p,O_DP,"dp_mag");
  if ( !p )
    *rp = 0;
  else {
    for ( s = 0, m = BDY(p); m; m = NEXT(m) )
      s += p_mag((P)m->c);
    STOZ(s,*rp);
  }
}

/* kara_mag is no longer used. */

void Pdp_set_kara(NODE arg,Z *rp)
{
  *rp = 0;
}

void Pdp_homo(NODE arg,DP *rp)
{
  asir_assert(ARG0(arg),O_DP,"dp_homo");
  dp_homo((DP)ARG0(arg),rp);
}

void Pdp_dehomo(NODE arg,DP *rp)
{
  asir_assert(ARG0(arg),O_DP,"dp_dehomo");
  dp_dehomo((DP)ARG0(arg),rp);
}

void Pdp_gr_flags(NODE arg,LIST *rp)
{
  Obj name,value;
  NODE n;

  if ( arg ) {
    asir_assert(ARG0(arg),O_LIST,"dp_gr_flags");
    n = BDY((LIST)ARG0(arg));
    while ( n ) {
      name = (Obj)BDY(n); n = NEXT(n);
      if ( !n )
        break;
      else {
        value = (Obj)BDY(n); n = NEXT(n);
      }
      dp_set_flag(name,value);
    }
  }
  dp_make_flaglist(rp);
}

extern int DP_Print, DP_PrintShort;

void Pdp_gr_print(NODE arg,Z *rp)
{
  Z q;
  int s;

  if ( arg ) {
    asir_assert(ARG0(arg),O_N,"dp_gr_print");
    q = (Z)ARG0(arg);
    s = ZTOS(q);
    switch ( s ) {
      case 0:
        DP_Print = 0; DP_PrintShort = 0;
        break;
      case 1:
        DP_Print = 1;
        break;
      case 2:
        DP_Print = 0; DP_PrintShort = 1;
        break;
      default:
        DP_Print = s; DP_PrintShort = 0;
        break;
    }
  } else {
    if ( DP_Print )  {
      STOZ(1,q);
    } else if ( DP_PrintShort ) {
      STOZ(2,q);
    } else
      q = 0;
  }
  *rp = q;
}

void parse_gr_option(LIST f,NODE opt,LIST *v,Num *homo,
  int *modular,struct order_spec **ord)
{
  NODE t,p;
  Z m,z;
  char *key;
  Obj value,dmy;
  int ord_is_set = 0;
  int modular_is_set = 0;
  int homo_is_set = 0;
  VL vl,vl0;
  LIST vars;
  char xiname[BUFSIZ];
  NODE x0,x;
  DP d;
  P xi;
  int nv,i;

  /* extract vars */
  vars = 0;
  for ( t = opt; t; t = NEXT(t) ) {
    p = BDY((LIST)BDY(t));
    key = BDY((STRING)BDY(p));
    value = (Obj)BDY(NEXT(p));
    if ( !strcmp(key,"v") ) {
      /* variable list */
      vars = (LIST)value;
      break;
    }
  }
  if ( vars ) {
    *v = vars; pltovl(vars,&vl);
  } else {
    for ( t = BDY(f); t; t = NEXT(t) )
      if ( BDY(t) && OID((Obj)BDY(t))==O_DP )
        break;
    if ( t ) {
      /* f is DP list */
      /* create dummy var list */
      d = (DP)BDY(t);
      nv = NV(d);
      for ( i = 0, vl0 = 0, x0 = 0; i < nv; i++ ) {
        NEXTVL(vl0,vl);
        NEXTNODE(x0,x);
        sprintf(xiname,"x%d",i);
        makevar(xiname,&xi);
        x->body = (pointer)xi;
        vl->v = VR((P)xi);
      }
      if ( vl0 ) {
        NEXT(vl) = 0;
        NEXT(x) = 0;
      }
      MKLIST(vars,x0);
      *v = vars;
      vl = vl0;
    } else {
      get_vars((Obj)f,&vl); vltopl(vl,v);
    }
  }

  for ( t = opt; t; t = NEXT(t) ) {
    p = BDY((LIST)BDY(t));
    key = BDY((STRING)BDY(p));
    value = (Obj)BDY(NEXT(p));
    if ( !strcmp(key,"v") ) {
      /* variable list; ignore */
    } else if ( !strcmp(key,"order") ) {
      /* order spec */
      if ( !vl )
        error("parse_gr_option : variables must be specified");
      create_order_spec(vl,value,ord);
      ord_is_set = 1;
    } else if ( !strcmp(key,"block") ) {
      create_order_spec(0,value,ord);
      ord_is_set = 1;
    } else if ( !strcmp(key,"matrix") ) {
      create_order_spec(0,value,ord);
      ord_is_set = 1;
    } else if ( !strcmp(key,"sugarweight") ) {
      /* weight */
      Pdp_set_weight(NEXT(p),&dmy);
    } else if ( !strcmp(key,"homo") ) {
      *homo = (Num)value;
      homo_is_set = 1;
    } else if ( !strcmp(key,"trace") ) {
      m = (Z)value;
      STOZ(0x80000000,z);
      if ( !m )
        *modular = 0;
      else if ( cmpz(m,z) >= 0 )
        error("parse_gr_option : too large modulus");
      else
        *modular = ZTOS(m);
      modular_is_set = 1;
    } else if ( !strcmp(key,"dp") ) {
      /* XXX : ignore */
    } else
      error("parse_gr_option : not implemented");
  }
  if ( !ord_is_set ) create_order_spec(0,0,ord);
  if ( !modular_is_set ) *modular = 0;
  if ( !homo_is_set ) *homo = 0;
}

void Pdp_gr_main(NODE arg,LIST *rp)
{
  LIST f,v;
  VL vl;
  Num homo;
  Z m,z;
  int modular,ac;
  struct order_spec *ord;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_gr_main");
  f = (LIST)ARG0(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  if ( (ac = argc(arg)) == 5 ) {
    asir_assert(ARG1(arg),O_LIST,"dp_gr_main");
    asir_assert(ARG2(arg),O_N,"dp_gr_main");
    asir_assert(ARG3(arg),O_N,"dp_gr_main");
    v = (LIST)ARG1(arg);
    homo = (Num)ARG2(arg);
    m = (Z)ARG3(arg);
    STOZ(0x80000000,z);
    if ( !m )
      modular = 0;
    else if ( cmpz(m,z) >= 0 )
      error("dp_gr_main : too large modulus");
    else
      modular = ZTOS(m);
    create_order_spec(0,ARG4(arg),&ord);
  } else if ( current_option )
    parse_gr_option(f,current_option,&v,&homo,&modular,&ord);
  else if ( ac == 1 )
    parse_gr_option(f,0,&v,&homo,&modular,&ord);
  else
    error("dp_gr_main : invalid argument");
  dp_gr_main(f,v,homo,modular,0,ord,rp);
}

void Pdp_interreduce(NODE arg,LIST *rp)
{
  LIST f,v;
  VL vl;
  int ac;
  struct order_spec *ord;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_interreduce");
  f = (LIST)ARG0(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  if ( (ac = argc(arg)) == 3 ) {
    asir_assert(ARG1(arg),O_LIST,"dp_interreduce");
    v = (LIST)ARG1(arg);
    create_order_spec(0,ARG2(arg),&ord);
  }
  dp_interreduce(f,v,0,ord,rp);
}

void Pdp_gr_f_main(NODE arg,LIST *rp)
{
  LIST f,v;
  Num homo;
  int m,field,t;
  struct order_spec *ord;
  NODE n;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_gr_f_main");
  asir_assert(ARG1(arg),O_LIST,"dp_gr_f_main");
  asir_assert(ARG2(arg),O_N,"dp_gr_f_main");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  homo = (Num)ARG2(arg);
#if 0
  asir_assert(ARG3(arg),O_N,"dp_gr_f_main");
  m = ZTOS((Q)ARG3(arg));
  if ( m )
    error("dp_gr_f_main : trace lifting is not implemented yet");
  create_order_spec(0,ARG4(arg),&ord);
#else
  m = 0;
  create_order_spec(0,ARG3(arg),&ord);
#endif
  field = 0;
  for ( n = BDY(f); n; n = NEXT(n) ) {
    t = get_field_type(BDY(n));
    if ( !t )
      continue;
    if ( t < 0 )
      error("dp_gr_f_main : incosistent coefficients");
    if ( !field )
      field = t;
    else if ( t != field )
      error("dp_gr_f_main : incosistent coefficients");
  }
  dp_gr_main(f,v,homo,m?1:0,field,ord,rp);
}

void Pdp_f4_main(NODE arg,LIST *rp)
{
  LIST f,v;
  struct order_spec *ord;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_f4_main");
  asir_assert(ARG1(arg),O_LIST,"dp_f4_main");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  create_order_spec(0,ARG2(arg),&ord);
  dp_f4_main(f,v,ord,rp);
}

/* dp_gr_checklist(list of dp) */

void Pdp_gr_checklist(NODE arg,LIST *rp)
{
  VECT g;
  LIST dp;
  NODE r;
  int n;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_gr_checklist");
  asir_assert(ARG1(arg),O_N,"dp_gr_checklist");
  n = ZTOS((Q)ARG1(arg));
  gbcheck_list(BDY((LIST)ARG0(arg)),n,&g,&dp);
  r = mknode(2,g,dp);
  MKLIST(*rp,r);
}

void Pdp_f4_mod_main(NODE arg,LIST *rp)
{
  LIST f,v;
  int m;
  struct order_spec *ord;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_f4_mod_main");
  asir_assert(ARG1(arg),O_LIST,"dp_f4_mod_main");
  asir_assert(ARG2(arg),O_N,"dp_f4_mod_main");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg); m = ZTOS((Q)ARG2(arg));
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  if ( !m )
    error("dp_f4_mod_main : invalid argument");
  create_order_spec(0,ARG3(arg),&ord);
  dp_f4_mod_main(f,v,m,ord,rp);
}

void Pdp_gr_mod_main(NODE arg,LIST *rp)
{
  LIST f,v;
  Num homo;
  int m;
  struct order_spec *ord;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"dp_gr_mod_main");
  asir_assert(ARG1(arg),O_LIST,"dp_gr_mod_main");
  asir_assert(ARG2(arg),O_N,"dp_gr_mod_main");
  asir_assert(ARG3(arg),O_N,"dp_gr_mod_main");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  homo = (Num)ARG2(arg); m = ZTOS((Q)ARG3(arg));
  if ( !m )
    error("dp_gr_mod_main : invalid argument");
  create_order_spec(0,ARG4(arg),&ord);
  dp_gr_mod_main(f,v,homo,m,ord,rp);
}

void Psetmod_ff(NODE node, Obj *val);

void Pnd_f4(NODE arg,LIST *rp)
{
  LIST f,v;
  int m,homo,retdp,ac;
  Obj val;
  Z mq,z;
  Num nhomo;
  NODE node;
  struct order_spec *ord;

  do_weyl = 0;
  nd_rref2 = 0;
  retdp = 0;
  if ( (ac = argc(arg)) == 4 ) {
    asir_assert(ARG0(arg),O_LIST,"nd_f4");
    asir_assert(ARG1(arg),O_LIST,"nd_f4");
    asir_assert(ARG2(arg),O_N,"nd_f4");
    f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
    f = remove_zero_from_list(f);
    if ( !BDY(f) ) {
      *rp = f; return;
    }
      mq = (Z)ARG2(arg);
      STOZ((unsigned long)0x40000000,z);
      if ( cmpz(mq,z) >= 0 ) {
        node = mknode(1,mq);
        Psetmod_ff(node,&val);
        m = -2;
    } else
      m = ZTOS(mq);
    create_order_spec(0,ARG3(arg),&ord);
    homo = 0;
    if ( get_opt("homo",&val) && val ) homo = 1;
    if ( get_opt("dp",&val) && val ) retdp = 1;
    if ( get_opt("rref2",&val) && val ) nd_rref2 = 1;
  } else if ( ac == 1 ) {
    f = (LIST)ARG0(arg);
    parse_gr_option(f,current_option,&v,&nhomo,&m,&ord);
    homo = ZTOS((Q)nhomo);
    if ( get_opt("dp",&val) && val ) retdp = 1;
    if ( get_opt("rref2",&val) && val ) nd_rref2 = 1;
  } else
    error("nd_f4 : invalid argument");
  nd_gr(f,v,m,homo,retdp,1,ord,rp);
}

void Pnd_gr(NODE arg,LIST *rp)
{
  LIST f,v;
  int m,homo,retdp,ac;
  Obj val;
  Z mq,z;
  Num nhomo;
  NODE node;
  struct order_spec *ord;

  do_weyl = 0;
  retdp = 0;
  if ( (ac=argc(arg)) == 4 ) {
    asir_assert(ARG0(arg),O_LIST,"nd_gr");
    asir_assert(ARG1(arg),O_LIST,"nd_gr");
    asir_assert(ARG2(arg),O_N,"nd_gr");
    f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
    f = remove_zero_from_list(f);
    if ( !BDY(f) ) {
      *rp = f; return;
    }
      mq = (Z)ARG2(arg);
      STOZ(0x40000000,z);
      if ( cmpz(mq,z) >= 0 ) {
        node = mknode(1,mq);
        Psetmod_ff(node,&val);
        m = -2;
      } else
        m = ZTOS(mq);
    create_order_spec(0,ARG3(arg),&ord);
    homo = 0;
    if ( get_opt("homo",&val) && val ) homo = 1;
    if ( get_opt("dp",&val) && val ) retdp = 1;
  } else if ( ac == 1 ) {
    f = (LIST)ARG0(arg);
    parse_gr_option(f,current_option,&v,&nhomo,&m,&ord);
    homo = ZTOS((Q)nhomo);
    if ( get_opt("dp",&val) && val ) retdp = 1;
  } else
    error("nd_gr : invalid argument");
  nd_gr(f,v,m,homo,retdp,0,ord,rp);
}

void Pnd_gr_postproc(NODE arg,LIST *rp)
{
  LIST f,v;
  int m,do_check;
  Z mq,z;
  Obj val;
  NODE node;
  struct order_spec *ord;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"nd_gr");
  asir_assert(ARG1(arg),O_LIST,"nd_gr");
  asir_assert(ARG2(arg),O_N,"nd_gr");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  mq = (Z)ARG2(arg);
  STOZ(0x40000000,z);
  if ( cmpz(mq,z) >= 0 ) {
    node = mknode(1,mq);
    Psetmod_ff(node,&val);
    m = -2;
  } else
    m = ZTOS(mq);
  create_order_spec(0,ARG3(arg),&ord);
  do_check = ARG4(arg) ? 1 : 0;
  nd_gr_postproc(f,v,m,ord,do_check,rp);
}

void Pnd_gr_recompute_trace(NODE arg,LIST *rp)
{
  LIST f,v,tlist;
  int m;
  struct order_spec *ord;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"nd_gr_recompute_trace");
  asir_assert(ARG1(arg),O_LIST,"nd_gr_recompute_trace");
  asir_assert(ARG2(arg),O_N,"nd_gr_recompute_trace");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  m = ZTOS((Q)ARG2(arg));
  create_order_spec(0,ARG3(arg),&ord);
  tlist = (LIST)ARG4(arg);
  nd_gr_recompute_trace(f,v,m,ord,tlist,rp);
}

Obj nd_btog_one(LIST f,LIST v,int m,struct order_spec *ord,LIST tlist,int pos);
Obj nd_btog(LIST f,LIST v,int m,struct order_spec *ord,LIST tlist);

void Pnd_btog(NODE arg,Obj *rp)
{
  LIST f,v,tlist;
  Z mq,z;
  int m,ac,pos;
  struct order_spec *ord;
  NODE node;
  pointer val;

  do_weyl = 0;
  asir_assert(ARG0(arg),O_LIST,"nd_btog");
  asir_assert(ARG1(arg),O_LIST,"nd_btog");
  asir_assert(ARG2(arg),O_N,"nd_btog");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  mq = (Z)ARG2(arg);
  STOZ(0x40000000,z);
  if ( cmpz(mq,z) >= 0 ) {
    node = mknode(1,mq);
    Psetmod_ff(node,(Obj *)&val);
    m = -2;
  } else 
    m = ZTOS(mq);
  create_order_spec(0,ARG3(arg),&ord);
  tlist = (LIST)ARG4(arg);
  if ( (ac = argc(arg)) == 6 ) {
    asir_assert(ARG5(arg),O_N,"nd_btog");
    pos = ZTOS((Q)ARG5(arg));
    *rp = nd_btog_one(f,v,m,ord,tlist,pos);
  } else if ( ac == 5 )
    *rp = nd_btog(f,v,m,ord,tlist);
  else
    error("nd_btog : argument mismatch");
}

void Pnd_weyl_gr_postproc(NODE arg,LIST *rp)
{
  LIST f,v;
  int m,do_check;
  struct order_spec *ord;

  do_weyl = 1;
  asir_assert(ARG0(arg),O_LIST,"nd_gr");
  asir_assert(ARG1(arg),O_LIST,"nd_gr");
  asir_assert(ARG2(arg),O_N,"nd_gr");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; do_weyl = 0; return;
  }
  m = ZTOS((Q)ARG2(arg));
  create_order_spec(0,ARG3(arg),&ord);
  do_check = ARG4(arg) ? 1 : 0;
  nd_gr_postproc(f,v,m,ord,do_check,rp);
  do_weyl = 0;
}

void Pnd_gr_trace(NODE arg,LIST *rp)
{
  LIST f,v;
  int m,homo,ac;
  Obj val;
  int retdp;
  Num nhomo;
  struct order_spec *ord;

  do_weyl = 0;
  if ( (ac = argc(arg)) == 5 ) {
    asir_assert(ARG0(arg),O_LIST,"nd_gr_trace");
    asir_assert(ARG1(arg),O_LIST,"nd_gr_trace");
    asir_assert(ARG2(arg),O_N,"nd_gr_trace");
    asir_assert(ARG3(arg),O_N,"nd_gr_trace");
    f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
    f = remove_zero_from_list(f);
    if ( !BDY(f) ) {
      *rp = f; return;
    }
    homo = ZTOS((Q)ARG2(arg));
    m = ZTOS((Q)ARG3(arg));
    create_order_spec(0,ARG4(arg),&ord);
  } else if ( ac == 1 ) {
    f = (LIST)ARG0(arg);
    parse_gr_option(f,current_option,&v,&nhomo,&m,&ord);
    homo = ZTOS((Q)nhomo);
  } else
    error("nd_gr_trace : invalid argument");
  retdp = 0;
  if ( get_opt("dp",&val) && val ) retdp = 1;
  nd_gr_trace(f,v,m,homo,retdp,0,ord,rp);
}

void Pnd_f4_trace(NODE arg,LIST *rp)
{
  LIST f,v;
  int m,homo,ac;
  int retdp;
  Obj val;
  Num nhomo;
  struct order_spec *ord;

  do_weyl = 0;
  if ( (ac = argc(arg))==5 ) {
    asir_assert(ARG0(arg),O_LIST,"nd_f4_trace");
    asir_assert(ARG1(arg),O_LIST,"nd_f4_trace");
    asir_assert(ARG2(arg),O_N,"nd_f4_trace");
    asir_assert(ARG3(arg),O_N,"nd_f4_trace");
    f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
    f = remove_zero_from_list(f);
    if ( !BDY(f) ) {
      *rp = f; return;
    }
    homo = ZTOS((Q)ARG2(arg));
    m = ZTOS((Q)ARG3(arg));
    create_order_spec(0,ARG4(arg),&ord);
  } else if ( ac == 1 ) {
    f = (LIST)ARG0(arg);
    parse_gr_option(f,current_option,&v,&nhomo,&m,&ord);
    homo = ZTOS((Q)nhomo);
  } else
    error("nd_gr_trace : invalid argument");
  retdp = 0;
  if ( get_opt("dp",&val) && val ) retdp = 1;
  nd_gr_trace(f,v,m,homo,retdp,1,ord,rp);
}

void Pnd_weyl_gr(NODE arg,LIST *rp)
{
  LIST f,v;
  int m,homo,retdp,ac;
  Obj val;
  Num nhomo;
  struct order_spec *ord;

  do_weyl = 1;
  retdp = 0;
  if ( (ac = argc(arg)) == 4 ) {
    asir_assert(ARG0(arg),O_LIST,"nd_weyl_gr");
    asir_assert(ARG1(arg),O_LIST,"nd_weyl_gr");
    asir_assert(ARG2(arg),O_N,"nd_weyl_gr");
    f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
    f = remove_zero_from_list(f);
    if ( !BDY(f) ) {
      *rp = f; do_weyl = 0; return;
    }
    m = ZTOS((Q)ARG2(arg));
    create_order_spec(0,ARG3(arg),&ord);
    homo = 0;
    if ( get_opt("homo",&val) && val ) homo = 1;
    if ( get_opt("dp",&val) && val ) retdp = 1;
  } else if ( ac == 1 ) {
    f = (LIST)ARG0(arg);
    parse_gr_option(f,current_option,&v,&nhomo,&m,&ord);
    homo = ZTOS((Q)nhomo);
    if ( get_opt("dp",&val) && val ) retdp = 1;
  } else
    error("nd_weyl_gr : invalid argument");
  nd_gr(f,v,m,homo,retdp,0,ord,rp);
  do_weyl = 0;
}

void Pnd_weyl_gr_trace(NODE arg,LIST *rp)
{
  LIST f,v;
  int m,homo,ac,retdp;
  Obj val;
  Num nhomo;
  struct order_spec *ord;

  do_weyl = 1;
  if ( (ac = argc(arg)) == 5 ) {
    asir_assert(ARG0(arg),O_LIST,"nd_weyl_gr_trace");
    asir_assert(ARG1(arg),O_LIST,"nd_weyl_gr_trace");
    asir_assert(ARG2(arg),O_N,"nd_weyl_gr_trace");
    asir_assert(ARG3(arg),O_N,"nd_weyl_gr_trace");
    f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
    f = remove_zero_from_list(f);
    if ( !BDY(f) ) {
      *rp = f; do_weyl = 0; return;
    }
    homo = ZTOS((Q)ARG2(arg));
    m = ZTOS((Q)ARG3(arg));
    create_order_spec(0,ARG4(arg),&ord);
  } else if ( ac == 1 ) {
    f = (LIST)ARG0(arg);
    parse_gr_option(f,current_option,&v,&nhomo,&m,&ord);
    homo = ZTOS((Q)nhomo);
  } else
    error("nd_weyl_gr_trace : invalid argument");
  retdp = 0;
  if ( get_opt("dp",&val) && val ) retdp = 1;
  nd_gr_trace(f,v,m,homo,retdp,0,ord,rp);
  do_weyl = 0;
}

void Pnd_nf(NODE arg,Obj *rp)
{
  Obj f;
  LIST g,v;
  struct order_spec *ord;

  do_weyl = 0;
  asir_assert(ARG1(arg),O_LIST,"nd_nf");
  asir_assert(ARG2(arg),O_LIST,"nd_nf");
  asir_assert(ARG4(arg),O_N,"nd_nf");
  f = (Obj)ARG0(arg);
  g = (LIST)ARG1(arg); g = remove_zero_from_list(g);
  if ( !BDY(g) ) {
    *rp = f; return;
  }
  v = (LIST)ARG2(arg);
  create_order_spec(0,ARG3(arg),&ord);
  nd_nf_p(f,g,v,ZTOS((Q)ARG4(arg)),ord,rp);
}

void Pnd_weyl_nf(NODE arg,Obj *rp)
{
  Obj f;
  LIST g,v;
  struct order_spec *ord;

  do_weyl = 1;
  asir_assert(ARG1(arg),O_LIST,"nd_weyl_nf");
  asir_assert(ARG2(arg),O_LIST,"nd_weyl_nf");
  asir_assert(ARG4(arg),O_N,"nd_weyl_nf");
  f = (Obj)ARG0(arg);
  g = (LIST)ARG1(arg); g = remove_zero_from_list(g);
  if ( !BDY(g) ) {
    *rp = f; return;
  }
  v = (LIST)ARG2(arg);
  create_order_spec(0,ARG3(arg),&ord);
  nd_nf_p(f,g,v,ZTOS((Q)ARG4(arg)),ord,rp);
}

/* for Weyl algebra */

void Pdp_weyl_gr_main(NODE arg,LIST *rp)
{
  LIST f,v;
  Num homo;
  Z m,z;
  int modular,ac;
  struct order_spec *ord;


  asir_assert(ARG0(arg),O_LIST,"dp_weyl_gr_main");
  f = (LIST)ARG0(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  if ( (ac = argc(arg)) == 5 ) {  
    asir_assert(ARG1(arg),O_LIST,"dp_weyl_gr_main");
    asir_assert(ARG2(arg),O_N,"dp_weyl_gr_main");
    asir_assert(ARG3(arg),O_N,"dp_weyl_gr_main");
    v = (LIST)ARG1(arg);
    homo = (Num)ARG2(arg);
    m = (Z)ARG3(arg);
    STOZ(0x80000000,z);
    if ( !m )
      modular = 0;
    else if ( cmpz(m,z) >= 0 )
      error("dp_weyl_gr_main : too large modulus");
    else
      modular = ZTOS(m);
    create_order_spec(0,ARG4(arg),&ord);
  } else if ( current_option )
    parse_gr_option(f,current_option,&v,&homo,&modular,&ord);
  else if ( ac == 1 )
    parse_gr_option(f,0,&v,&homo,&modular,&ord);
  else
    error("dp_weyl_gr_main : invalid argument");
  do_weyl = 1;
  dp_gr_main(f,v,homo,modular,0,ord,rp);
  do_weyl = 0;
}

void Pdp_weyl_gr_f_main(NODE arg,LIST *rp)
{
  LIST f,v;
  Num homo;
  struct order_spec *ord;

  asir_assert(ARG0(arg),O_LIST,"dp_weyl_gr_main");
  asir_assert(ARG1(arg),O_LIST,"dp_weyl_gr_main");
  asir_assert(ARG2(arg),O_N,"dp_weyl_gr_main");
  asir_assert(ARG3(arg),O_N,"dp_weyl_gr_main");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  homo = (Num)ARG2(arg);
  create_order_spec(0,ARG3(arg),&ord);
  do_weyl = 1;
  dp_gr_main(f,v,homo,0,1,ord,rp);
  do_weyl = 0;
}

void Pdp_weyl_f4_main(NODE arg,LIST *rp)
{
  LIST f,v;
  struct order_spec *ord;

  asir_assert(ARG0(arg),O_LIST,"dp_weyl_f4_main");
  asir_assert(ARG1(arg),O_LIST,"dp_weyl_f4_main");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  create_order_spec(0,ARG2(arg),&ord);
  do_weyl = 1;
  dp_f4_main(f,v,ord,rp);
  do_weyl = 0;
}

void Pdp_weyl_f4_mod_main(NODE arg,LIST *rp)
{
  LIST f,v;
  int m;
  struct order_spec *ord;

  asir_assert(ARG0(arg),O_LIST,"dp_weyl_f4_main");
  asir_assert(ARG1(arg),O_LIST,"dp_weyl_f4_main");
  asir_assert(ARG2(arg),O_N,"dp_f4_main");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg); m = ZTOS((Q)ARG2(arg));
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  if ( !m )
    error("dp_weyl_f4_mod_main : invalid argument");
  create_order_spec(0,ARG3(arg),&ord);
  do_weyl = 1;
  dp_f4_mod_main(f,v,m,ord,rp);
  do_weyl = 0;
}

void Pdp_weyl_gr_mod_main(NODE arg,LIST *rp)
{
  LIST f,v;
  Num homo;
  int m;
  struct order_spec *ord;

  asir_assert(ARG0(arg),O_LIST,"dp_weyl_gr_mod_main");
  asir_assert(ARG1(arg),O_LIST,"dp_weyl_gr_mod_main");
  asir_assert(ARG2(arg),O_N,"dp_weyl_gr_mod_main");
  asir_assert(ARG3(arg),O_N,"dp_weyl_gr_mod_main");
  f = (LIST)ARG0(arg); v = (LIST)ARG1(arg);
  f = remove_zero_from_list(f);
  if ( !BDY(f) ) {
    *rp = f; return;
  }
  homo = (Num)ARG2(arg); m = ZTOS((Q)ARG3(arg));
  if ( !m )
    error("dp_weyl_gr_mod_main : invalid argument");
  create_order_spec(0,ARG4(arg),&ord);
  do_weyl = 1;
  dp_gr_mod_main(f,v,homo,m,ord,rp);
  do_weyl = 0;
}

VECT current_dl_weight_vector_obj;
int *current_dl_weight_vector;
int dp_negative_weight;

void Pdp_set_weight(NODE arg,VECT *rp)
{
  VECT v;
  int i,n;
  NODE node;

  if ( !arg )
    *rp = current_dl_weight_vector_obj;
  else if ( !ARG0(arg) ) {
    current_dl_weight_vector_obj = 0;
    current_dl_weight_vector = 0;
        dp_negative_weight = 0;
    *rp = 0;
  } else {
    if ( OID(ARG0(arg)) != O_VECT && OID(ARG0(arg)) != O_LIST )
      error("dp_set_weight : invalid argument");
    if ( OID(ARG0(arg)) == O_VECT )
      v = (VECT)ARG0(arg);
    else {
      node = (NODE)BDY((LIST)ARG0(arg));
      n = length(node);
      MKVECT(v,n);
      for ( i = 0; i < n; i++, node = NEXT(node) )
        BDY(v)[i] = BDY(node);
    }
    current_dl_weight_vector_obj = v;
    n = v->len;
    current_dl_weight_vector = (int *)CALLOC(n,sizeof(int));
    for ( i = 0; i < n; i++ )
      current_dl_weight_vector[i] = ZTOS((Q)v->body[i]);
        for ( i = 0; i < n; i++ )
            if ( current_dl_weight_vector[i] < 0 ) break;
        if ( i < n )
            dp_negative_weight = 1;
        else
            dp_negative_weight = 0;
    *rp = v;
  }
}

VECT current_module_weight_vector_obj;
int *current_module_weight_vector;

void Pdp_set_module_weight(NODE arg,VECT *rp)
{
  VECT v;
  int i,n;
  NODE node;

  if ( !arg )
    *rp = current_module_weight_vector_obj;
  else if ( !ARG0(arg) ) {
    current_module_weight_vector_obj = 0;
    current_module_weight_vector = 0;
    *rp = 0;
  } else {
    if ( OID(ARG0(arg)) != O_VECT && OID(ARG0(arg)) != O_LIST )
      error("dp_module_set_weight : invalid argument");
    if ( OID(ARG0(arg)) == O_VECT )
      v = (VECT)ARG0(arg);
    else {
      node = (NODE)BDY((LIST)ARG0(arg));
      n = length(node);
      MKVECT(v,n);
      for ( i = 0; i < n; i++, node = NEXT(node) )
        BDY(v)[i] = BDY(node);
    }
    current_module_weight_vector_obj = v;
    n = v->len;
    current_module_weight_vector = (int *)CALLOC(n,sizeof(int));
    for ( i = 0; i < n; i++ )
      current_module_weight_vector[i] = ZTOS((Q)v->body[i]);
    *rp = v;
  }
}

extern Obj current_top_weight;
extern Obj nd_top_weight;

void Pdp_set_top_weight(NODE arg,Obj *rp)
{
  VECT v;
  MAT m;
  Obj obj;
  int i,j,n,id,row,col;
  Q *mi;
  NODE node;

  if ( !arg )
    *rp = current_top_weight;
  else if ( !ARG0(arg) ) {
    reset_top_weight();
    *rp = 0;
  } else {
    id = OID(ARG0(arg));
    if ( id != O_VECT && id != O_MAT && id != O_LIST )
      error("dp_set_top_weight : invalid argument");
    if ( id == O_LIST ) {
      node = (NODE)BDY((LIST)ARG0(arg));
      n = length(node);
      MKVECT(v,n);
      for ( i = 0; i < n; i++, node = NEXT(node) )
        BDY(v)[i] = BDY(node);
        obj = (Obj)v;
    } else
        obj = ARG0(arg);
    if ( OID(obj) == O_VECT ) {
      v = (VECT)obj;
        for ( i = 0; i < v->len; i++ )
          if ( !INT((Q)BDY(v)[i]) || sgnz((Z)BDY(v)[i]) < 0 )
            error("dp_set_top_weight : each element must be a non-negative integer");
    } else {
      m = (MAT)obj; row = m->row; col = m->col;
        for ( i = 0; i < row; i++ )
        for ( j = 0, mi = (Q *)BDY(m)[i]; j < col; j++ )
              if ( !INT((Q)mi[j]) || sgnz((Z)mi[j]) < 0 ) 
                error("dp_set_top_weight : each element must be a non-negative integer");
    }
        current_top_weight = obj;
    nd_top_weight = obj;
    *rp = current_top_weight;
  }
}

LIST get_denomlist();

void Pdp_get_denomlist(LIST *rp)
{
  *rp = get_denomlist();
}

static VECT current_weyl_weight_vector_obj;
int *current_weyl_weight_vector;

void Pdp_weyl_set_weight(NODE arg,VECT *rp)
{
  VECT v;
  NODE node;
  int i,n;

  if ( !arg )
    *rp = current_weyl_weight_vector_obj;
  else if ( !ARG0(arg) ) {
    current_weyl_weight_vector_obj = 0;
    current_weyl_weight_vector = 0;
    *rp = 0;
  } else {
    if ( OID(ARG0(arg)) != O_VECT && OID(ARG0(arg)) != O_LIST )
      error("dp_weyl_set_weight : invalid argument");
    if ( OID(ARG0(arg)) == O_VECT )
      v = (VECT)ARG0(arg);
    else {
      node = (NODE)BDY((LIST)ARG0(arg));
      n = length(node);
      MKVECT(v,n);
      for ( i = 0; i < n; i++, node = NEXT(node) )
        BDY(v)[i] = BDY(node);
    }
    current_weyl_weight_vector_obj = v;
    n = v->len;
    current_weyl_weight_vector = (int *)CALLOC(n,sizeof(int));
    for ( i = 0; i < n; i++ )
      current_weyl_weight_vector[i] = ZTOS((Q)v->body[i]);
    *rp = v;
  }
}

NODE mono_raddec(NODE ideal);

void Pdp_mono_raddec(NODE arg,LIST *rp)
{
  NODE ideal,rd,t,t1,r,r1,u;
  VL vl0,vl;
  int nv,i,bpi;
  int *s;
  DP dp;
  P *v;
  LIST l;

  ideal = BDY((LIST)ARG0(arg));
  if ( !ideal ) *rp = (LIST)ARG0(arg);
  else {
    t = BDY((LIST)ARG1(arg));
    nv = length(t);
    v = (P *)MALLOC(nv*sizeof(P));
    for ( vl0 = 0, i = 0; t; t = NEXT(t), i++ ) {
      NEXTVL(vl0,vl); VR(vl) = VR((P)BDY(t));
      MKV(VR(vl),v[i]);
    }
    if ( vl0 ) NEXT(vl) = 0;
    for ( t = 0, r = ideal; r; r = NEXT(r) ) {
      ptod(CO,vl0,BDY(r),&dp); MKNODE(t1,dp,t); t = t1;
    }
    rd = mono_raddec(t);
    r = 0;
    bpi = (sizeof(int)/sizeof(char))*8;
    for ( u = rd; u; u = NEXT(u) ) {
      s = (int *)BDY(u);
      for ( i = nv-1, t = 0; i >= 0; i-- )
        if ( s[i/bpi]&(1<<(i%bpi)) ) {
          MKNODE(t1,v[i],t); t = t1;
        }
      MKLIST(l,t); MKNODE(r1,l,r); r = r1;
    }
    MKLIST(*rp,r);
  }
}

void Pdp_mono_reduce(NODE arg,LIST *rp)
{
  NODE t,t0,t1,r0,r;
  int i,n;
  DP m;
  DP *a;

  t0 = BDY((LIST)ARG0(arg));
  t1 = BDY((LIST)ARG1(arg));
  n = length(t0);
  a = (DP *)MALLOC(n*sizeof(DP));
  for ( i = 0; i < n; i++, t0 = NEXT(t0) ) a[i] = (DP)BDY(t0);
  for ( t = t1; t; t = NEXT(t) ) {
    m = (DP)BDY(t);
    for ( i = 0; i < n; i++ )
      if ( a[i] && dp_redble(a[i],m) ) a[i] = 0;
  }
  for ( i = n-1, r0 = 0; i >= 0; i-- )
    if ( a[i] ) { NEXTNODE(r0,r); BDY(r) = a[i]; }
  if ( r0 ) NEXT(r) = 0;
  MKLIST(*rp,r0);
}

#define BLEN (8*sizeof(unsigned long))

void showmat2(unsigned long **a,int row,int col)
{
  int i,j;

  for ( i = 0; i < row; i++, putchar('\n') )
    for ( j = 0; j < col; j++ )
      if ( a[i][j/BLEN] & (1L<<(j%BLEN)) ) putchar('1');
      else putchar('0');
}

int rref2(unsigned long **a,int row,int col)
{
  int i,j,k,l,s,wcol,wj;
  unsigned long bj;
  unsigned long *ai,*ak,*as,*t;
  int *pivot;

  wcol = (col+BLEN-1)/BLEN; 
  pivot = (int *)MALLOC_ATOMIC(row*sizeof(int));
  i = 0;
  for ( j = 0; j < col; j++ ) {
    wj = j/BLEN; bj = 1L<<(j%BLEN);
    for ( k = i; k < row; k++ )
      if ( a[k][wj] & bj ) break;
    if ( k == row ) continue;
    pivot[i] = j;
    if ( k != i ) {
     t = a[i]; a[i] = a[k]; a[k] = t;
    }
    ai = a[i];
    for ( k = i+1; k < row; k++ ) {
      ak = a[k];
      if ( ak[wj] & bj ) {
        for ( l = wj; l < wcol; l++ )
          ak[l] ^= ai[l];
      }
    }
    i++;
  }
  for ( k = i-1; k >= 0; k-- ) {
    j = pivot[k]; wj = j/BLEN; bj = 1L<<(j%BLEN);
    ak = a[k];
    for ( s = 0; s < k; s++ ) {
      as = a[s];
      if ( as[wj] & bj ) {
        for ( l = wj; l < wcol; l++ )
          as[l] ^= ak[l];
      }
    }
  }
  return i;
}

void Pdp_rref2(NODE arg,VECT *rp)
{
  VECT f,term,ret;
  int row,col,wcol,size,nv,i,j,rank,td;
  unsigned long **mat;
  unsigned long *v;
  DL d;
  DL *t;
  DP dp;
  MP m,m0;

  f = (VECT)ARG0(arg);
  row = f->len;
  term = (VECT)ARG1(arg);
  col = term->len; 
  mat = (unsigned long **)MALLOC(row*sizeof(unsigned long *));
  size = sizeof(unsigned long)*((col+BLEN-1)/BLEN);
  nv = ((DP)term->body[0])->nv;
  t = (DL *)MALLOC(col*sizeof(DL));
  for ( i = 0; i < col; i++ ) t[i] = BDY((DP)BDY(term)[i])->dl;
  for ( i = 0; i < row; i++ ) {
    v = mat[i] = (unsigned long *)MALLOC_ATOMIC_IGNORE_OFF_PAGE(size);
  bzero(v,size);
  for ( j = 0, m = BDY((DP)BDY(f)[i]); m; m = NEXT(m) ) {
    d = m->dl;
    for ( ; !dl_equal(nv,d,t[j]); j++ );
    v[j/BLEN] |= 1L <<(j%BLEN);
  }
  }
  rank = rref2(mat,row,col);
  MKVECT(ret,rank);
  *rp = ret;
  for ( i = 0; i < rank; i++ ) {
    v = mat[i];
  m0 = 0;
  td = 0;
    for ( j = 0; j < col; j++ ) {
    if ( v[j/BLEN] & (1L<<(j%BLEN)) ) {
      NEXTMP(m0,m);
    m->dl = t[j];
    m->c = (Obj)ONE;
      td = MAX(td,m->dl->td);
    }
  }
  NEXT(m) = 0;
  MKDP(nv,m0,dp);
  dp->sugar = td;
    BDY(ret)[i] = (pointer)dp;
  }
}

#define HDL(f) (BDY(f)->dl)

NODE sumi_criB(int nv,NODE d,DP *f,int m)
{
 LIST p;
 NODE r0,r;
 int p0,p1;
 DL p2,lcm;

 NEWDL(lcm,nv);
 r0 = 0;
 for ( ; d; d = NEXT(d) ) {
  p = (LIST)BDY(d);
  p0 = ZTOS((Q)ARG0(BDY(p)));
  p1 = ZTOS((Q)ARG1(BDY(p)));
  p2 = HDL((DP)ARG2(BDY(p)));
    if(!_dl_redble(HDL((DP)f[m]),p2,nv) ||
     dl_equal(nv,lcm_of_DL(nv,HDL(f[p0]),HDL(f[m]),lcm),p2) ||
     dl_equal(nv,lcm_of_DL(nv,HDL(f[p1]),HDL(f[m]),lcm),p2) ) {
    NEXTNODE(r0,r);
    BDY(r) = p;
  }
 }
 if ( r0 ) NEXT(r) = 0;
 return r0;
}

NODE sumi_criFMD(int nv,DP *f,int m)
{
  DL *a;
  DL l1,dl1,dl2;
  int i,j,k,k2;
  NODE r,r1,nd;
  MP mp;
  DP u;
  Z iq,mq;
  LIST list;

  /* a[i] = lcm(LT(f[i]),LT(f[m])) */
  a = (DL *)ALLOCA(m*sizeof(DL));
  for ( i = 0; i < m; i++ ) {
   a[i] = lcm_of_DL(nv,HDL(f[i]),HDL(f[m]),0);
  }
  r = 0;
  for( i = 0; i < m; i++) {
   l1 = a[i];
   if ( !l1 ) continue;
   /* Tkm = Tim (k<i) */
   for( k = 0; k < i; k++)
     if( dl_equal(nv,l1,a[k]) ) break;
   if( k == i ){
     /* Tk|Tim && Tkm != Tim (k<m) */
   for ( k2 = 0; k2 < m; k2++ )
     if ( _dl_redble(HDL(f[k2]),l1,nv) &&
       !dl_equal(nv,l1,a[k2]) ) break;
   if ( k2 == m ) {
       dl1 = HDL(f[i]); dl2 = HDL(f[m]);
       for ( k2 = 0; k2 < nv; k2++ )
         if ( dl1->d[k2] && dl2->d[k2] ) break;
       if ( k2 < nv ) {
         NEWMP(mp); mp->dl = l1; C(mp) = (Obj)ONE;
         NEXT(mp) = 0; MKDP(nv,mp,u); u->sugar = l1->td;
       STOZ(i,iq); STOZ(m,mq);
       nd = mknode(3,iq,mq,u);
       MKLIST(list,nd);
       MKNODE(r1,list,r);
       r = r1;
    }
  }
   }
 } 
 return r;
}

LIST sumi_updatepairs(LIST d,DP *f,int m)
{
  NODE old,new,t;
  LIST l;
  int nv;

  nv = f[0]->nv;
  old = sumi_criB(nv,BDY(d),f,m);
  new = sumi_criFMD(nv,f,m);
  if ( !new ) new = old;
  else {
    for ( t = new ; NEXT(t); t = NEXT(t) );
  NEXT(t) = old;
  }
  MKLIST(l,new);
  return l;
}

VECT ltov(LIST l)
{
  NODE n;
  int i,len;
  VECT v;

  n = BDY(l);
  len = length(n);
  MKVECT(v,len);
  for ( i = 0; i < len; i++, n = NEXT(n) )
    BDY(v)[i] = BDY(n);
  return v;
}

DL subdl(int nv,DL d1,DL d2)
{
  int i;
  DL d;

  NEWDL(d,nv);
  d->td = d1->td-d2->td;
  for ( i = 0; i < nv; i++ )
    d->d[i] = d1->d[i]-d2->d[i];
  return d;
}

DP dltodp(int nv,DL d)
{
  MP mp;
  DP dp;

  NEWMP(mp); mp->dl = d; C(mp) = (Obj)ONE;
  NEXT(mp) = 0; MKDP(nv,mp,dp); dp->sugar = d->td;
  return dp;
}

LIST sumi_simplify(int nv,DL t,DP p,NODE f2,int simp)
{
  DL d,h,hw;
  DP u,w,dp;
  int n,i,last;
  LIST *v;
  LIST list;
  NODE s,r;

  d = t; u = p;
  /* only the last history is used */
  if ( f2 && simp && t->td != 0 ) {
    adddl(nv,t,HDL(p),&h);
    n = length(f2);
    last = 1;
    if ( simp > 1 ) last = n;
    v = (LIST *)ALLOCA(n*sizeof(LIST));
    for ( r = f2, i = 0; r; r = NEXT(r), i++ ) v[n-i-1] = BDY(r);
    for ( i = 0; i < last; i++ ) {
      for ( s = BDY((LIST)v[i]); s; s = NEXT(s) ) {
      w = (DP)BDY(s); hw = HDL(w);
        if ( _dl_redble(hw,h,nv) ) {
      u = w;
      d = subdl(nv,h,hw);
      goto fin;
      }
      }
    }
  }
fin:
  dp = dltodp(nv,d);
  r = mknode(2,dp,u);
  MKLIST(list,r);
  return list;
}

LIST sumi_symbolic(NODE l,int q,NODE f2,DP *g,int simp)
{
   int nv;
   NODE t,r;
   NODE f0,f,fd0,fd,done0,done,red0,red;
   DL h,d;
   DP mul;
   int m;
   LIST tp,l0,l1,l2,l3,list;
   VECT v0,v1,v2,v3;

   nv = ((DP)BDY(l))->nv;
   t = 0;

   f0 = 0; fd0 = 0; done0 = 0; red0 = 0;

   for ( ; l; l = NEXT(l) ) {
     t = symb_merge(t,dp_dllist((DP)BDY(l)),nv);
     NEXTNODE(fd0,fd); BDY(fd) = BDY(l);
   }

   while ( t ) {
   h = (DL)BDY(t);
   NEXTNODE(done0,done); BDY(done) = dltodp(nv,h);
   t = NEXT(t);
     for(m = 0; m < q; m++)
     if ( _dl_redble(HDL(g[m]),h,nv) ) break;
     if ( m == q ) { 
     } else {
     d = subdl(nv,h,HDL(g[m]));
       tp = sumi_simplify(nv,d,g[m],f2,simp);

     muldm(CO,ARG1(BDY(tp)),BDY((DP)ARG0(BDY(tp))),&mul);
       t = symb_merge(t,NEXT(dp_dllist(mul)),nv);

     NEXTNODE(f0,f); BDY(f) = tp;
     NEXTNODE(fd0,fd); BDY(fd) = mul;
     NEXTNODE(red0,red); BDY(red) = mul;
     }
   }
   if ( fd0 ) NEXT(fd) = 0; MKLIST(l0,fd0);
   v0 = ltov(l0);
   if ( done0 ) NEXT(done) = 0; MKLIST(l1,done0);
   v1 = ltov(l1);
   if ( f0 ) NEXT(f) = 0; MKLIST(l2,f0);
   v2 = ltov(l2);
   if ( red0 ) NEXT(red) = 0; MKLIST(l3,red0);
   v3 = ltov(l3);
   r = mknode(4,v0,v1,v2,v3);
   MKLIST(list,r);
   return list;
}

void Psumi_symbolic(NODE arg,LIST *rp)
{
  NODE l,f2;
  DP *g;
  int q,simp;

  l = BDY((LIST)ARG0(arg));
  q = ZTOS((Q)ARG1(arg));
  f2 = BDY((LIST)ARG2(arg));
  g = (DP *)BDY((VECT)ARG3(arg));
  simp = ZTOS((Q)ARG4(arg));
  *rp = sumi_symbolic(l,q,f2,g,simp);
}

void Psumi_updatepairs(NODE arg,LIST *rp)
{
   LIST d,l;
   DP *f;
   int m;

   d = (LIST)ARG0(arg); 
   f = (DP *)BDY((VECT)ARG1(arg)); 
   m = ZTOS((Q)ARG2(arg)); 
   *rp = sumi_updatepairs(d,f,m);
}

LIST remove_zero_from_list(LIST l)
{
  NODE n,r0,r;
  LIST rl;

  asir_assert(l,O_LIST,"remove_zero_from_list");
  n = BDY(l);
  for ( r0 = 0; n; n = NEXT(n) )
    if ( BDY(n) ) {
      NEXTNODE(r0,r);
      BDY(r) = BDY(n);
    }
  if ( r0 )
    NEXT(r) = 0;
  MKLIST(rl,r0);
  return rl;
}

int get_field_type(P p)
{
  int type,t;
  DCP dc;

  if ( !p )
    return 0;
  else if ( NUM(p) )
    return NID((Num)p);
  else {
    type = 0;
    for ( dc = DC(p); dc; dc = NEXT(dc) ) {
      t = get_field_type(COEF(dc));
      if ( !t )
        continue;
      if ( t < 0 )
        return t;
      if ( !type )
        type = t;
      else if ( t != type )
        return -1;
    }
    return type;
  }
}

void Pdpv_ord(NODE arg,Obj *rp)
{
  int ac,id;
  LIST shift;

  ac = argc(arg);
  if ( ac ) {
    id = ZTOS((Q)ARG0(arg));
    if ( ac > 1 && ARG1(arg) && OID((Obj)ARG1(arg))==O_LIST )
      shift = (LIST)ARG1(arg);
    else
      shift = 0;
    create_modorder_spec(id,shift,&dp_current_modspec);
  }
  *rp = dp_current_modspec->obj;
}

extern int dpm_ordtype;

void set_schreyer_order(NODE n);

LIST schreyer_obj;

void Pdpm_set_schreyer(NODE arg,LIST *rp)
{
  if ( argc(arg) ) {
    schreyer_obj = (LIST)ARG0(arg);
    set_schreyer_order(schreyer_obj?BDY(schreyer_obj):0);
  }
  *rp = schreyer_obj;
}

void Pdpm_hm(NODE arg,DPM *rp)
{
  DPM p;

  p = (DPM)ARG0(arg); asir_assert(p,O_DPM,"dpm_hm");
  dpm_hm(p,rp);
}

void Pdpm_ht(NODE arg,DPM *rp)
{
  DPM p;

  p = (DPM)ARG0(arg); asir_assert(p,O_DPM,"dpm_ht");
  dpm_ht(p,rp);
}

void dpm_rest(DPM p,DPM *r);

void Pdpm_rest(NODE arg,DPM *rp)
{
  DPM p;

  p = (DPM)ARG0(arg); asir_assert(p,O_DPM,"dpm_ht");
  dpm_rest(p,rp);
}


void Pdpm_hp(NODE arg,Z *rp)
{
  DPM p;
  int pos;

  p = (DPM)ARG0(arg); asir_assert(p,O_DPM,"dpm_ht");
  pos = BDY(p)->pos;
  STOZ(pos,*rp);
}

void dpm_shift(DPM p,int s,DPM *rp);

void Pdpm_shift(NODE arg,DPM *rp)
{
  DPM p;
  int s;

  p = (DPM)ARG0(arg); asir_assert(p,O_DPM,"dpm_shift");
  s = ZTOS((Z)ARG1(arg));
  dpm_shift(p,s,rp);
}

void dpm_sort(DPM p,DPM *rp);

void Pdpm_sort(NODE arg,DPM *rp)
{
  DPM p;
  int s;

  p = (DPM)ARG0(arg); 
  if ( !p ) *rp = 0;
  dpm_sort(p,rp);
}

void dpm_split(DPM p,int s,DPM *up,DPM *lo);

void Pdpm_split(NODE arg,LIST *rp)
{
  DPM p,up,lo;
  int s;
  NODE nd;

  p = (DPM)ARG0(arg);
  s = ZTOS((Z)ARG1(arg));
  dpm_split(p,s,&up,&lo);
  nd = mknode(2,up,lo);
  MKLIST(*rp,nd);
}


void Pdpm_hc(NODE arg,Obj *rp)
{
  asir_assert(ARG0(arg),O_DPM,"dpm_hc");
  if ( !ARG0(arg) )
    *rp = 0;
  else
    *rp = BDY((DPM)ARG0(arg))->c;
}


void Pdpv_ht(NODE arg,LIST *rp)
{
  NODE n;
  DP ht;
  int pos;
  DPV p;
  Z q;

  asir_assert(ARG0(arg),O_DPV,"dpv_ht");
  p = (DPV)ARG0(arg);
  pos = dpv_hp(p);
  if ( pos < 0 )
    ht = 0;
  else
    dp_ht(BDY(p)[pos],&ht);
  STOZ(pos,q);
  n = mknode(2,q,ht);
  MKLIST(*rp,n);
}

void Pdpv_hm(NODE arg,LIST *rp)
{
  NODE n;
  DP ht;
  int pos;
  DPV p;
  Z q;

  asir_assert(ARG0(arg),O_DPV,"dpv_hm");
  p = (DPV)ARG0(arg);
  pos = dpv_hp(p);
  if ( pos < 0 )
    ht = 0;
  else
    dp_hm(BDY(p)[pos],&ht);
  STOZ(pos,q);
  n = mknode(2,q,ht);
  MKLIST(*rp,n);
}

void Pdpv_hc(NODE arg,LIST *rp)
{
  NODE n;
  P hc;
  int pos;
  DPV p;
  Z q;

  asir_assert(ARG0(arg),O_DPV,"dpv_hc");
  p = (DPV)ARG0(arg);
  pos = dpv_hp(p);
  if ( pos < 0 )
    hc = 0;
  else
    hc = (P)BDY(BDY(p)[pos])->c;
  STOZ(pos,q);
  n = mknode(2,q,hc);
  MKLIST(*rp,n);
}

int dpv_hp(DPV p)
{
  int len,i,maxp,maxw,w,slen;
  int *shift;
  DP *e;

  len = p->len;
  e = p->body;
  slen = dp_current_modspec->len;
  shift = dp_current_modspec->degree_shift;
  switch ( dp_current_modspec->id ) {
    case ORD_REVGRADLEX:
      for ( maxp = -1, i = 0; i < len; i++ )
        if ( !e[i] ) continue;
        else if ( maxp < 0 ) { 
          maxw = BDY(e[i])->dl->td+(i<slen?shift[i]:0); maxp = i; 
        } else {
          w = BDY(e[i])->dl->td+(i<slen?shift[i]:0);
          if ( w >= maxw ) {
            maxw = w; maxp = i;
          }
        }
      return maxp;
    case ORD_GRADLEX:
      for ( maxp = -1, i = 0; i < len; i++ )
        if ( !e[i] ) continue;
        else if ( maxp < 0 ) { 
          maxw = BDY(e[i])->dl->td+(i<slen?shift[i]:0); maxp = i; 
        } else {
          w = BDY(e[i])->dl->td+(i<slen?shift[i]:0);
          if ( w > maxw ) {
            maxw = w; maxp = i;
          }
        }
      return maxp;
      break;
    case ORD_LEX:
      for ( i = 0; i < len; i++ )
        if ( e[i] ) return i;
      return -1;
      break;
    default:
      error("dpv_hp : unsupported term ordering");
      return -1;
      break;
  }
}

int get_opt(char *key0,Obj *r) {
   NODE tt,p;
   char *key;

   if ( current_option ) {
     for ( tt = current_option; tt; tt = NEXT(tt) ) {
       p = BDY((LIST)BDY(tt));
       key = BDY((STRING)BDY(p));
       /*  value = (Obj)BDY(NEXT(p)); */
       if ( !strcmp(key,key0) )  {
       *r = (Obj)BDY(NEXT(p));
       return 1;
     }
     }
   }
   return 0;
}
