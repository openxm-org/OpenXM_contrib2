/* $OpenXM: OpenXM_contrib2/asir2018/builtin/parif.c,v 1.4 2020/08/26 06:40:36 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "ox.h"

Q ox_pari_stream;
int ox_pari_stream_initialized = 0;
int ox_get_pari_result = 0;
P ox_pari_starting_function = 0;

typedef void (*mpfr_func)(NODE,Obj *);

void Pmpfr_ai();
void Pmpfr_eint(), Pmpfr_erf(),Pmpfr_li2();
void Pmpfr_zeta();
void Pmpfr_j0(), Pmpfr_j1();
void Pmpfr_y0(), Pmpfr_y1();
void Pmpfr_gamma(), Pmpfr_lngamma(), Pmpfr_digamma();
void Pmpfr_floor(), Pmpfr_round(), Pmpfr_ceil();

void Pox_shutdown(NODE arg,Q *rp);
void Pox_launch_nox(NODE arg,Obj *rp);
void Pox_launch(NODE arg,Obj *rp);
void Pox_push_cmo(NODE arg,Obj *rp);
void Pox_pop_cmo(NODE arg,Obj *rp);
void Pox_execute_function(NODE arg,Obj *rp);

int debug_pari;

struct mpfr_tab_rec {
  char *name;
  mpfr_func func;
} mpfr_tab[] = {
  {"ai",Pmpfr_ai},
  {"zeta",Pmpfr_zeta},
  {"j0",Pmpfr_j0},
  {"j1",Pmpfr_j1},
  {"y0",Pmpfr_y0},
  {"y1",Pmpfr_y1},
  {"eint",Pmpfr_eint},
  {"erf",Pmpfr_erf},
  {"li2",Pmpfr_li2},
  {"gamma",Pmpfr_gamma},
  {"lngamma",Pmpfr_lngamma},
  {"digamma",Pmpfr_digamma},
  {"floor",Pmpfr_floor},
  {"ceil",Pmpfr_ceil},
  {"round",Pmpfr_round},
};

mpfr_func mpfr_search(char *name)
{
  int i,n;

  n = sizeof(mpfr_tab)/sizeof(struct mpfr_tab_rec);
  for ( i = 0; i < n; i++ )
    if ( !strcmp(name,mpfr_tab[i].name) )
      return mpfr_tab[i].func;
  return 0;
}

Obj list_to_vect(Obj a)
{
  int len,i;
  VECT v;
  NODE nd;

  if ( !a || OID(a) != O_LIST ) return a;
  len = length(BDY((LIST)a));
  MKVECT(v,len);
  for ( i = 0, nd = BDY((LIST)a); nd; nd = NEXT(nd), i++ )
     v->body[i] = (pointer)list_to_vect((Obj)BDY(nd));
  return (Obj)v;
}

Obj vect_to_mat(VECT v)
{
  MAT m;
  int len,col,i,j;

  len = v->len;
  if ( v->body[0] && OID((Obj)v->body[0]) == O_VECT ) {
    col = ((VECT)v->body[0])->len;
  for ( i = 1; i < len; i++ ) 
    if ( !v->body[i] || OID((Obj)v->body[i]) != O_VECT
     || ((VECT)v->body[i])->len != col )
    break;
    if ( i == len ) {
    /* convert to a matrix */
    MKMAT(m,len,col);
    for ( i = 0; i < len; i++ )
      for ( j = 0; j < col; j++ )
      m->body[i][j] = ((VECT)v->body[i])->body[j];
    return (Obj)m;
  }
  }
  return (Obj)v;
}

void reset_ox_pari()
{
  NODE nd;
  Q r;

  if ( ox_get_pari_result ) {
  nd = mknode(1,ox_pari_stream);
  Pox_shutdown(nd,&r);
    ox_get_pari_result = 0;
  ox_pari_stream_initialized = 0;
  }
}

void pari_setprec(long n)
{
  struct oFUNC f;
  Z z;
  NODE arg;

  f.name = f.fullname = "pari_setprec";
  f.id = A_PARI;
  f.argc = 1;
  f.f.binf = 0;
  STOZ(n,z);
  arg = mknode(1,z);
  evalparif(&f,arg);
}

/* decimal precision */
long mpfrprec = 0;

pointer evalparif(FUNC f,NODE arg)
{
  long prec;
  int ac,intarg,opt;
  Q q,r,cmd;
  Z narg;
  Real sec;
  NODE nd,oxarg,t,t1,n;
  STRING name;
  USINT ui;
  LIST list;
  Obj ret,dmy;
  mpfr_func mpfr_function;
  V v;

  if ( arg && (!ARG0(arg) || (NUM(ARG0(arg)) && NID((Num)ARG0(arg)) != N_C))
    && (mpfr_function = mpfr_search(f->name)) ) {
    (*mpfr_function)(arg,&ret);
    return (pointer) ret;
  }

  if ( !ox_pari_stream_initialized ) {
  if ( ox_pari_starting_function && OID(ox_pari_starting_function) == O_P ) {
    v = VR(ox_pari_starting_function);
    if ( (long)v->attr != V_SR ) {
      error("pari : no handler.");
    }
    MKNODE(nd,0,0);
    r = (Q)bevalf((FUNC)v->priv,0,0);
  }else {
#if !defined(VISUAL)    
  MKSTR(name,"ox_pari");
  nd = mknode(2,NULL,name);
  if ( debug_pari )
    Pox_launch(nd,(Obj *)&r);
  else
    Pox_launch_nox(nd,(Obj *)&r);
#else
  error("Please load names.rr from latest asir-contrib library before using pari functions.");
#endif
  }
  ox_pari_stream = r;
    ox_pari_stream_initialized = 1;
  }

  prec = mpfr_get_default_prec()*0.30103+1;
  if ( prec != mpfrprec ) {
    mpfrprec = prec;
    pari_setprec(prec);
  }
  ac = argc(arg);
  /* reverse the arg list */
  for ( n = arg, t = 0; n; n = NEXT(n) ) {
    MKNODE(t1,BDY(n),t); t = t1;
  }
  /* push the reversed arg list */
  for ( ; t; t = NEXT(t) ) {
    oxarg = mknode(2,ox_pari_stream,BDY(t));
    Pox_push_cmo(oxarg,&dmy);
  }
  MKSTR(name,f->name);
  STOZ(ac,narg);
  oxarg = mknode(3,ox_pari_stream,name,narg);
  Pox_execute_function(oxarg,&dmy);
  ox_get_pari_result = 1;
#if defined(VISUAL) || defined(__MINGW32__)
#define SM_popCMO 262
  STOZ(SM_popCMO,cmd);
  oxarg = mknode(2,ox_pari_stream,cmd);
  Pox_push_cmd(oxarg,&dmy);
  nd = mknode(1,ox_pari_stream);
  MKLIST(list,nd);
  MKReal(1.0/8,sec);
  oxarg = mknode(2,list,sec);
  ret=0;
  do {
    check_intr();
    Pox_select(oxarg,&list);
    oxarg = mknode(1,list);
    Plength(oxarg,&ret);
  }while (!ret);
  oxarg = mknode(1,ox_pari_stream);
  Pox_get(oxarg,&ret);
#else
  oxarg = mknode(1,ox_pari_stream);
  Pox_pop_cmo(oxarg,&ret);
#endif
  ox_get_pari_result = 0;
  if ( ret && OID(ret) == O_ERR ) {
    char buf[BUFSIZ];
    soutput_init(buf); 
    sprintexpr(CO,((ERR)ret)->body);
    error(buf);
  }
  if ( ret && OID(ret) == O_LIST ) {
    ret = list_to_vect(ret);
  ret = vect_to_mat((VECT)ret);
  }
  return ret;
}

struct pariftab {
  char *name;
  int dmy;
  int type;
};

/*
 * type = 1 => argc = 1, second arg = precision
 * type = 2 => argc = 1, second arg = (long int)0
 *
 */
/*
{"abs",0,1},
{"adj",0,1},
*/

struct pariftab pariftab[] = {
{"arg",0,1},
{"bigomega",0,1},
{"binary",0,1},
{"ceil",0,1},
{"centerlift",0,1},
{"cf",0,1},
{"classno",0,1},
{"classno2",0,1},
{"conj",0,1},
{"content",0,1},
{"denom",0,1},
{"det",0,1},
{"det2",0,1},
{"dilog",0,1},
{"disc",0,1},
{"discf",0,1},
{"divisors",0,1},
{"eigen",0,1},
{"eintg1",0,1},
{"erfc",0,1},
{"eta",0,1},
{"floor",0,1},
{"frac",0,1},
{"galois",0,1},
{"galoisconj",0,1},
{"gamh",0,1},
{"gamma",0,1},
{"hclassno",0,1},
{"hermite",0,1},
{"hess",0,1},
{"imag",0,1},
{"image",0,1},
{"image2",0,1},
{"indexrank",0,1},
{"indsort",0,1},
{"initalg",0,1},
{"isfund",0,1},
{"ispsp",0,1},
{"isqrt",0,1},
{"issqfree",0,1},
{"issquare",0,1},
{"jacobi",0,1},
{"jell",0,1},
{"ker",0,1},
{"keri",0,1},
{"kerint",0,1},
{"kerintg1",0,1},
{"length",0,1},
{"lexsort",0,1},
{"lift",0,1},
{"lindep",0,1},
{"lll",0,1},
{"lllgen",0,1},
{"lllgram",0,1},
{"lllgramgen",0,1},
{"lllgramint",0,1},
{"lllgramkerim",0,1},
{"lllgramkerimgen",0,1},
{"lllint",0,1},
{"lllkerim",0,1},
{"lllkerimgen",0,1},
{"lngamma",0,1},
{"logagm",0,1},
{"mat",0,1},
{"matrixqz2",0,1},
{"matrixqz3",0,1},
{"matsize",0,1},
{"modreverse",0,1},
{"mu",0,1},
{"nextprime",0,1},
{"norm",0,1},
{"norml2",0,1},
{"numdiv",0,1},
{"numer",0,1},
{"omega",0,1},
{"order",0,1},
{"ordred",0,1},
{"phi",0,1},
{"pnqn",0,1},
{"polred",0,1},
{"polred2",0,1},
{"primroot",0,1},
{"psi",0,1},
{"quadgen",0,1},
{"quadpoly",0,1},
{"real",0,1},
{"recip",0,1},
{"redreal",0,1},
{"regula",0,1},
{"reorder",0,1},
{"reverse",0,1},
{"rhoreal",0,1},
{"roots",0,1},
{"round",0,1},
{"sigma",0,1},
{"signat",0,1},
{"simplify",0,1},
{"smalldiscf",0,1},
{"smallfact",0,1},
{"smallpolred",0,1},
{"smallpolred2",0,1},
{"smith",0,1},
{"smith2",0,1},
{"sort",0,1},
{"sqr",0,1},
{"sqred",0,1},
{"sqrt",0,1},
{"supplement",0,1},
{"trace",0,1},
{"trans",0,1},
{"trunc",0,1},
{"unit",0,1},
{"vec",0,1},
{"wf",0,1},
{"wf2",0,1},
{"zeta",0,1},
{"factor",0,1},

{"allocatemem",0,0},

{"factpol",0,2},
{"isprime",0,2},
{"factorint",0,2},
{0,0,0},
};

void parif_init() {
  int i;

  for ( i = 0, parif = 0; pariftab[i].name; i++ )
     appendparif(&parif,pariftab[i].name, 0,pariftab[i].type);
}
