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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/parse.h,v 1.29 2004/03/04 01:41:32 noro Exp $ 
*/
# if defined(VISUAL)
#include <time.h>
#else
#include <sys/time.h>
#include <sys/resource.h>
#endif
#include <setjmp.h>
#ifdef ABS
#undef ABS
#define ABS(a) ((a)>0?(a):-(a))
#endif

/* identifiers for expressions */

typedef enum {
	I_BOP, I_COP, I_AND, I_OR, I_NOT, I_CE,
	I_PRESELF, I_POSTSELF,
	I_FUNC, I_FUNC_OPT, I_IFUNC, I_MAP, I_RECMAP, I_PFDERIV,
	I_ANS, I_PVAR, I_ASSPVAR, 
	I_FORMULA, I_LIST, I_STR, I_NEWCOMP, I_CAR, I_CDR, I_CAST,
	I_COM, I_PROC, I_INDEX, I_EV, I_TIMER, I_GF2NGEN, I_GFPNGEN, I_GFSNGEN,
	I_LOP, I_OPT, I_GETOPT, I_POINT, I_QUOTE, I_PAREN, I_MINUS, I_RANGE
} fid;

/* identifiers for statements */

typedef enum {
	S_BP, S_PFDEF,
	S_SINGLE, S_CPLX, 
	S_IFELSE, S_FOR, S_DO,
	S_BREAK, S_RETURN, S_CONTINUE
} sid;

/* identifiers for comparison operators */

typedef enum { C_LT, C_GT, C_LE, C_GE, C_EQ, C_NE } cid;

/* identifiers for logical operators */

typedef enum { L_LT, L_GT, L_LE, L_GE, L_EQ, L_NE, L_AND, L_OR, L_NOT, L_IMPL, L_REPL, L_EQUIV } lid;

/* identifiers for functions */

typedef enum { A_UNDEF, A_BIN, A_USR, A_PURE, A_PARI } aid;

/* identifiers for indeterminates */

typedef enum { V_IND, V_UC, V_PF, V_SR } vid;

typedef struct oVS {
	unsigned int n;
	unsigned int asize;
	unsigned int at;
	unsigned int level;
	struct oFUNC *usrf;
	struct oPV *va;
	NODE opt;
} *VS;

typedef struct oMODULE {
	char *name;
	VS pvs;
	NODE usrf_list;
} *MODULE;

typedef struct oPV {
	char *name;
	short attr,type;
	pointer priv;
} *PV;

typedef struct oINFILE {
	char *name;
	FILE *fp;
	int ln;
	struct oINFILE *next;
	char *tname;
	int encoded;
	short vol;
} *INFILE;

struct oTKWD {
	char *name;
	int token;
};

typedef struct oARF {
	char *name;
	void (*fp)();
} *ARF;

typedef struct oFUNC {
	char *name,*fullname;
	int argc;
	int type;
	aid id;
	union {
		void (*binf)();
		struct oUSRF *usrf;
		struct oPF *puref;
	} f;
} *FUNC;

typedef struct oUSRF {
	char *fname;
	MODULE module;
	int startl,endl;
	NODE args;
	VS pvs;
	char *desc;
	struct oSNODE *body;
} *USRF;

typedef struct oPF {
	char *name;
	int argc;
	Obj body;
	V *args;
	Obj *deriv;
	NODE ins;
	int (*pari)();
	double (*libm)();
	int (*simplify)();
} *PF;

typedef struct oPFAD {
	Obj arg;
	int d;
} *PFAD;

typedef struct oPFINS {
	struct oPF *pf;	
	struct oPFAD ad[1];
} *PFINS;

#define NEWPF(p) ((p)=(PF)MALLOC(sizeof(struct oPF)))

typedef struct oFNODE {
	fid id;
	pointer arg[1];
} *FNODE;

#define NEWFNODE(a,b) \
((a)=(FNODE)MALLOC(sizeof(struct oFNODE)+sizeof(pointer)*(b-1)))

typedef struct oSNODE {
	sid id;
	int ln;
	pointer arg[1];
} *SNODE;

#define NEWSNODE(a,b) \
((a)=(SNODE)MALLOC(sizeof(struct oSNODE)+sizeof(pointer)*(b-1)),(a)->ln=asir_infile->ln)

typedef struct oPVI {
	int pv;
	NODE index;
} *PVI;

typedef struct oNODE2 {
	pointer body1,body2;
	struct oNODE2 *next;
} *NODE2;

struct ftab {
	char *name;
	void (*f)();
	int argc;
};

#define MKPVI(a,b,c)\
((a)=(pointer)MALLOC(sizeof(struct oPVI)),((PVI)(a))->pv=(b),((PVI)(a))->index=(c))

#define FA0(f) ((f)->arg[0])
#define FA1(f) ((f)->arg[1])
#define FA2(f) ((f)->arg[2])
#define FA3(f) ((f)->arg[3])
#define FA4(f) ((f)->arg[4])
#define FA5(f) ((f)->arg[5])
#define FA6(f) ((f)->arg[6])

#define ARG0(a) ((a)->body)
#define ARG1(a) (NEXT(a)->body)
#define ARG2(a) (NEXT(NEXT(a))->body)
#define ARG3(a) (NEXT(NEXT(NEXT(a)))->body)
#define ARG4(a) (NEXT(NEXT(NEXT(NEXT((a)))))->body)
#define ARG5(a) (NEXT(NEXT(NEXT(NEXT(NEXT((a))))))->body)
#define ARG6(a) (NEXT(NEXT(NEXT(NEXT(NEXT(NEXT((a)))))))->body)
#define ARG7(a) (NEXT(NEXT(NEXT(NEXT(NEXT(NEXT(NEXT((a))))))))->body)
#define ARG8(a) (NEXT(NEXT(NEXT(NEXT(NEXT(NEXT(NEXT(NEXT((a)))))))))->body)
#define ARG9(a) (NEXT(NEXT(NEXT(NEXT(NEXT(NEXT(NEXT(NEXT(NEXT((a))))))))))->body)
#define ARG10(a) (NEXT(NEXT(NEXT(NEXT(NEXT(NEXT(NEXT(NEXT(NEXT(NEXT((a)))))))))))->body)

#define asir_assert(ptr,id,message)\
switch ( id ) {\
	case O_N: case O_P: case O_R:\
		if( (ptr) && (OID(ptr) > (id)) ) {\
			fprintf(stderr,message);\
			error(" : invalid argument");\
		}\
		break;\
	case O_DP:\
		if( (ptr) && (OID(ptr) != (id)) ) {\
			fprintf(stderr,message);\
			error(" : invalid argument");\
		}\
		break;\
	default:\
		if( !(ptr) || (OID(ptr) != (id)) ) {\
			fprintf(stderr,message);\
			error(" : invalid argument");\
		}\
		break;\
}

#define DEFSIZE 32
#define PVGLOBAL(i) (((unsigned int)(i))|(1<<30))
#define PVMGLOBAL(i) (((unsigned int)(i))|(3<<30))
#define PVATTR(i) (((unsigned int)(i))>>30)
#define PVIND(i) (((unsigned int)(i))&0x3fffffff)
#define GETPV(i,p) \
(PVATTR(i)==0?(int)((p)=CPVS->va[(unsigned int)(i)].priv)\
             :PVATTR(i)==1?(int)((p)=GPVS->va[PVIND(i)].priv)\
                          :(int)((p)=MPVS->va[PVIND(i)].priv))
#define GETPVREF(i,p) \
(PVATTR(i)==0?(int)((p)=&(CPVS->va[(unsigned int)(i)].priv))\
             :PVATTR(i)==1?(int)((p)=&(GPVS->va[PVIND(i)].priv))\
                          :(int)((p)=&(MPVS->va[PVIND(i)].priv)))
#define GETPVNAME(i,p) \
(PVATTR(i)==0?(int)((p)=CPVS->va[(unsigned int)(i)].name)\
             :PVATTR(i)==1?(int)((p)=GPVS->va[PVIND(i)].name)\
                          :(int)((p)=MPVS->va[PVIND(i)].name))
#define ASSPV(i,p) \
(PVATTR(i)==0?(int)(CPVS->va[(unsigned int)(i)].priv=(pointer)(p))\
             :PVATTR(i)==1?(int)(GPVS->va[PVIND(i)].priv=(pointer)(p))\
                          :(int)(MPVS->va[PVIND(i)].priv=(pointer)(p)))

#define NEWNODE2(a) ((a)=(NODE2)MALLOC(sizeof(struct oNODE2)))
#define MKNODE2(a,b,c,d) \
(NEWNODE2(a),(a)->body1=(pointer)b,(a)->body2=(pointer)c,NEXT(a)=(NODE2)(d))
#define BDY1(a) ((a)->body1)
#define BDY2(a) ((a)->body2)

extern VS GPVS,CPVS,EPVS,APVS,MPVS;
extern MODULE CUR_MODULE;
extern NODE MODULE_LIST;
extern char *CUR_FUNC;
extern ARF addfs, subfs, mulfs, divfs, remfs, pwrfs;
extern INFILE asir_infile;
extern NODE usrf,sysf,noargsysf,ubinf,parif,ONENODE;
extern int nextbp,nextbplevel;
extern int Verbose;
extern int do_quiet;
extern SYMBOL Symbol_grlex, Symbol_glex, Symbol_lex;
extern NODE current_option;

#if defined(VISUAL_LIB)
#include <w_stdio.h>
#undef getc
#define getc w_fgetc
#undef putc
#define putc w_fputc
#define fputc w_fputc
#define ungetc w_ungetc
#define fgets w_fgets
#define fputs w_fputs
#define printf w_printf
#define fprintf w_fprintf
#define fflush w_fflush
#endif

#if defined(__GNUC__) || defined(VISUAL) || (defined(__MACH__) && defined(__ppc__)) || defined(__FreeBSD__)
NODE mknode(int,...);
FNODE mkfnode(int,fid,...);
SNODE mksnode(int,sid,...);
void call_usrf(FUNC f,...);
#else
NODE mknode();
FNODE mkfnode();
SNODE mksnode();
void call_usrf();
#endif

/* prototypes */

void dp_ptozp2_d(DP,DP,DP *,DP *);
void dp_ptozp_d(DP,DP *);
void dp_vtod(Q *,DP,DP *);
void dp_idivv_hist(Q,DP *);
void dp_igcdv_hist(DP,Q *);
void dp_ptozp2(DP,DP,DP *,DP *);
void dp_ptozp(DP,DP *);
void dp_nf_tab_mod(DP,LIST *,int,DP *);
void dp_lnf_mod(DP,DP,NODE,int,DP *,DP *);
void insert_to_node(DL,NODE *,int);
int _dl_redble(DL,DL,int);
void dp_mbase(NODE,NODE *);
void dp_dtov(DP,VECT *);
void dp_cont(DP,Q *);
void dp_idiv(DP,Q,DP *);

int dp_nt(DP);
void dp_dehomo(DP,DP *);
void dp_homo(DP,DP *);
DL lcm_of_DL(int,DL,DL,DL);
void dp_rest(DP,DP *);
void dp_hm(DP,DP *);
void dp_sp_mod(DP,DP,int,DP *);
void dp_sp(DP,DP,DP *);
void dp_red(DP,DP,DP,DP *,DP *,P *,DP *);
void dp_subd(DP,DP,DP *);
void dp_red_mod(DP,DP,DP,int,DP *,DP *,P *);
int dp_redble(DP,DP);
int comp_nm(Q *,Q *);
void dp_true_nf_mod(NODE,DP,DP *,int,int,DP *,P *);
void dp_nf_mod(NODE,DP,DP *,int,int,DP *);
void dp_nf_mod_qindex(NODE,DP,DP *,int,int,DP *);
void dp_nf_ptozp(NODE,DP,DP *,int,int,DP *);
void dp_true_nf(NODE,DP,DP *,int,DP *,P *);
void dp_nf(NODE,DP,DP *,int,DP *);
void dp_rat(DP,DP *);
void dp_mod(DP,int,NODE,DP *);
void dp_prim_mod(DP,int,DP *);
void dp_prim(DP,DP *);
void heu_nezgcdnpz(VL,P *,int,P *);
void homogenize_order(struct order_spec *,int,struct order_spec **);
int create_order_spec(VL,Obj,struct order_spec **);

int dl_equal(int,DL,DL);
void qltozl(Q *,int,Q *);
int cmpdl_matrix(int,DL,DL);
int cmpdl_order_pair(int,DL,DL);
int cmpdl_elim(int,DL,DL);
int cmpdl_blexrev(int,DL,DL);
int cmpdl_bgradrev(int,DL,DL);
int cmpdl_brevrev(int,DL,DL);
int cmpdl_brevgradlex(int,DL,DL);
int cmpdl_bgradlex(int,DL,DL);
int cmpdl_blex(int,DL,DL);
int cmpdl_revgradlex(int,DL,DL);
int cmpdl_gradlex(int,DL,DL);
int cmpdl_revlex(int,DL,DL);
int cmpdl_lex(int,DL,DL);
int compd(VL,DP,DP);
void adddl(int,DL,DL,DL *);
void divsdc(VL,DP,P,DP *);
void muldc(VL,DP,P,DP *);
void muldm(VL,DP,MP,DP *);
void muld(VL,DP,DP,DP *);
void chsgnd(DP,DP *);
void subd(VL,DP,DP,DP *);
void addd(VL,DP,DP,DP *);
int sugard(MP);
void nodetod(NODE,DP *);
void dtop(VL,VL,DP,P *);
void ptod(VL,VL,P,DP *);
void initd(struct order_spec *);

int mainparse(SNODE *);
int exprparse(FUNC,char *,FNODE *);
void output_init(void);
void GC_init(void);
void env_init(void);
void soutput_init(char *);
void appendparif(NODE *,char *,int (*)(),int);
int mmono(P);
void mkpow(VL,Obj,Obj,Obj *);
void _printdp(DP);
int mt_save(char *);
int mt_load(char *);
void sortbynm(Q *,int);
void iqrv(VECT,Q,LIST *);
void igcdv(VECT,Q *);
void igcdv_estimate(VECT,Q *);
void cpp_main(int,char **);
int afternl(void);
void glob_init();
void input_init(FILE *,char *);
void asir_terminate(int);
void param_init(void);
void sprompt(char *);
void cppname_init();
void process_args(int,char **);
void sig_init(void);
void asir_save_handler(void);
void asir_set_handler(void);
void asir_reset_handler(void);
void resetenv(char *);
void fatal(int);
void restore_handler(void);
void resize_buffer(void);
void tty_init(void);
void tty_reset(void);
void set_timer(int);
void reset_timer(void);
void int_handler(int);
void int_handler(int);
void segv_handler(int);
void bus_handler(int);
void fpe_handler(int);
void ill_handler(int);
void winch_handler(int);
void pipe_handler(int);
void usr1_handler(int);
void alrm_handler(int);
void copyright(void);
void kan_init(void);
void pushpvs(FUNC);
void read_eval_loop(void);
void prompt(void);
void storeans(pointer);
int yyparse(void);
void nodetod(NODE,DP *);
void poppvs(void);
int length(NODE);
int get_heapsize(void);
int searchpvar(char *);
void mkparif(char *,FUNC *);
void makevar(char *,P *);
void mkpvs(char *);
MODULE mkmodule(char *);
void stoarg(char *,int *,char **);
void yyerror(char *);
void closecurrentinput(void);
void asir_terminate(int);
void searchasirpath(char *,char **);
void get_vars(Obj,VL *);
void appendbinf(NODE *,char *,void(*)(),int);
void appendubinf(char *,void(*)(),int);
void parif_init(void);
void sysf_init(void);
void makesrvar(FUNC, P *);
void sprintexpr(VL,Obj);
void printexpr(VL,Obj);
void appendvar(VL,V);
void change_mvar(VL,P,V,P *);
void restore_mvar(VL,P,V,P *);
int argc(NODE);
void printn(N);
void sprintn(N);
void printv(VL,V);
void sprintv(VL,V);
int arf_comp(VL, Obj, Obj);
int complist(VL, LIST, LIST);
int complist(VL, LIST, LIST);
int getcompsize(int);
int getpvar(VS,char *,int);
int getpvar(VS,char *,int);
int gettype(unsigned int);
int indextotype(int,int);
int loadfile(char *);
int loadfiles(NODE);
unsigned int makepvar(char *);
int membertoindex(int,char *);
int qcoefp(Obj);
int qcoefr(Obj);
int rangecheck(Obj,int);
int structtoindex(char *);
pointer eval(FNODE);
pointer evalf(FUNC,FNODE,FNODE);
pointer bevalf(FUNC,NODE);
pointer evalif(FNODE,FNODE);
pointer evalnode(NODE node);
pointer evalpf(PF,NODE);
pointer evalparif(FUNC,NODE);
pointer evalpv(int,FNODE,pointer);
pointer evalstat(SNODE f);
void _mkpfins(PF ,Obj *,V *);
void appendpfins(V ,V *);
void appenduf(char *,FUNC *);
void arf_add(VL, Obj, Obj, Obj *);
void arf_chsgn(Obj, Obj *);
void arf_div(VL, Obj, Obj, Obj *);
void arf_init(void);
void arf_mul(VL, Obj, Obj, Obj *);
void arf_pwr(VL, Obj, Obj, Obj *);
void arf_remain(VL, Obj, Obj, Obj *);
void arf_sub(VL, Obj, Obj, Obj *);
void bp(SNODE);
void clearbp(FUNC);
void debug(SNODE);
void debug_init(void);
void delbp(int, char **);
void derivr(VL ,Obj ,V ,Obj *);
void derivvar(VL ,V ,V ,Obj *);
void duppfins(V ,V *);
void error(char *);
void evalins(PFINS ,int ,Obj *);
void evalnodebody(NODE,NODE *);
void evalp(VL ,P ,int ,P *);
void evalr(VL ,Obj ,int ,Obj *);
void evalv(VL ,V ,int ,Obj *);
void getarrayp(Obj,NODE,Obj **);
void getmember(FNODE,Obj *);
void getmemberp(FNODE,Obj **);
void help(char *);
void instov(PFINS ,V *);
Obj memberofstruct(COMP,char *);
void mkpf(char *,Obj ,int ,V *,int (*)(),double (*)(),int (*)(),PF *);
void mkpfins(PF ,V *,V *);
void mkuf(char *,char *,NODE,SNODE,int,int,char *,MODULE);
void newstruct(int,struct oCOMP **);
void optobj(Obj *);
void println(int, char **, int);
void printvars(char *,VS);
void resetpvs(void);
void searchc(char *,FUNC *);
void searchf(NODE,char *,FUNC *);
void searchpf(char *,FUNC *);
void searchsn(SNODE *, int, SNODE **);
void setbp(char *);
void setf(int, char **);
void setstruct(char *,NODE);
void showbp(int);
void showbps(void);
void showpos(void);
void simplify_ins(PFINS ,Obj *);
void sprintvars(char *,VS);
int structdef(char *,NODE);
void substfp(VL ,Obj ,PF ,PF ,Obj *);
void substfr(VL ,Obj ,PF ,PF ,Obj *);
void substfv(VL ,V ,PF ,PF ,Obj *);
void substpr(VL ,int ,Obj ,V ,Obj ,Obj *);
void substr(VL ,int ,Obj ,V ,Obj ,Obj *);
void appendtonode(NODE,pointer, NODE *);
void getarray(pointer,NODE,pointer *);
void putarray(pointer,NODE,pointer);

void addcomp(VL,COMP,COMP,COMP *);
void subcomp(VL,COMP,COMP,COMP *);
void mulcomp(VL,COMP,COMP,COMP *);
void divcomp(VL,COMP,COMP,COMP *);
void chsgncomp(COMP,COMP *);
void pwrcomp(VL,COMP,Obj,COMP *);
int compcomp(VL,COMP,COMP);

void mergedeglist(NODE d0,NODE d1,NODE *dr);
void getdeglist(P p,V v,NODE *d);
void restore_mvar(VL vl,P p,V v,P *r);
void change_mvar(VL vl,P p,V v,P *r);
void getcoef(VL vl,P p,V v,Q d,P *r);
void ranp(int n,UP *nr);

void _print_mp(int nv,MP m);
int dp_homogeneous(DP p);
int dp_nt(DP p);
int dl_equal(int nv,DL dl1,DL dl2);
DL lcm_of_DL(int nv,DL dl1,DL dl2,DL dl);
void dp_rest(DP p,DP *rp);
void dp_hm(DP p,DP *rp);
void dltod(DL d,int n,DP *rp);
void dp_subd(DP p1,DP p2,DP *rp);
int dp_redble(DP p1,DP p2);
void sortbynm(Q *w,int n);
int comp_nm(Q *a,Q *b);
void qltozl(Q *w,int n,Q *dvr);
void dp_rat(DP p,DP *rp);
void dp_mod(DP p,int mod,NODE subst,DP *rp);
void dp_dehomo(DP p,DP *rp);
void dp_homo(DP p,DP *rp);
void dp_nf_tab_mod(DP p,LIST *tab,int mod,DP *rp);
void dp_lnf_mod(DP p1,DP p2,NODE g,int mod,DP *r1p,DP *r2p);
void dp_lnf_f(DP p1,DP p2,NODE g,DP *r1p,DP *r2p);
void _dp_nf_mod_destructive(NODE b,DP g,DP *ps,int mod,int full,DP *rp);
void dp_true_nf_mod(NODE b,DP g,DP *ps,int mod,int full,DP *rp,P *dnp);
void dp_nf_mod(NODE b,DP g,DP *ps,int mod,int full,DP *rp);
void dp_nf_f(NODE b,DP g,DP *ps,int full,DP *rp);
void dp_nf_z(NODE b,DP g,DP *ps,int full,int multiple,DP *rp);
void dp_true_nf(NODE b,DP g,DP *ps,int full,DP *rp,P *dnp);
void _dp_red_mod_destructive(DP p1,DP p2,int mod,DP *rp);
void dp_red_mod(DP p0,DP p1,DP p2,int mod,DP *head,DP *rest,P *dnp);
void dp_red_f(DP p1,DP p2,DP *rest);
void dp_red(DP p0,DP p1,DP p2,DP *head,DP *rest,P *dnp,DP *multp);
void _dp_sp_mod(DP p1,DP p2,int mod,DP *rp);
void _dp_sp_mod_dup(DP p1,DP p2,int mod,DP *rp);
void dp_sp_mod(DP p1,DP p2,int mod,DP *rp);
void _dp_sp_dup(DP p1,DP p2,DP *rp);
void dp_sp(DP p1,DP p2,DP *rp);
void dp_dtov(DP dp,VECT *rp);
void dp_cont(DP p,Q *rp);
void dp_prim_mod(DP p,int mod,DP *rp);
void heu_nezgcdnpz(VL vl,P *pl,int m,P *pr);
void dp_prim(DP p,DP *rp);
void dp_ptozp2_d(DP p0,DP p1,DP *hp,DP *rp);
void dp_ptozp_d(DP p,DP *rp);
void dp_vtod(Q *c,DP p,DP *rp);
void insert_to_node(DL d,NODE *n,int nvar);
int _dl_redble(DL d1,DL d2,int nvar);
void dp_mbase(NODE hlist,NODE *mbase);
void dp_idiv(DP p,Q c,DP *rp);
void dp_ptozp2(DP p0,DP p1,DP *hp,DP *rp);
void dp_ptozp(DP p,DP *rp);
int eqdl(int nv,DL dl1,DL dl2);
void _dpmod_to_vect(DP f,DL *at,int *b);
void _tf_to_vect_compress(NODE tf,DL *at,CDP *b);
void dp_to_vect(DP f,DL *at,Q *b);
NODE dp_dllist(DP f);
NODE mul_dllist(DL d,DP f);
void pdl(NODE f);
void dp_gr_main(LIST f,LIST v,Num homo,int modular,int field,struct order_spec *ord,LIST *rp);
void dp_gr_mod_main(LIST f,LIST v,Num homo,int m,struct order_spec *ord,LIST *rp);
void dp_f4_main(LIST f,LIST v,struct order_spec *ord,LIST *rp);
void dp_f4_mod_main(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp);
NODE gb_f4(NODE f);
NODE gb_f4_mod(NODE f,int m);
NODE gb_f4_mod_old(NODE f,int m);
int DPPlength(DP_pairs n);
void printdl(DL dl);
void pltovl(LIST l,VL *vl);
void makesubst(VL v,NODE *s);
void printsubst(NODE s);
void vlminus(VL v,VL w,VL *d);
int validhc(P a,int m,NODE s);
void setup_arrays(NODE f,int m,NODE *r);
void prim_part(DP f,int m,DP *r);
NODE /* of DP */ NODE_sortb_insert( DP newdp, NODE /* of DP */ nd, int dec );
NODE NODE_sortb( NODE node, int dec );
NODE /* of index */ NODE_sortbi_insert( int newdpi, NODE /* of index */ nd, int dec );
NODE NODE_sortbi( NODE node, int dec );
void reduceall(NODE in,NODE *h);
void reduceall_mod(NODE in,int m,NODE *h);
int newps(DP a,int m,NODE subst);
int newps_nosave(DP a,int m,NODE subst);
int newps_mod(DP a,int m);
void reducebase_dehomo(NODE f,NODE *g);
NODE append_one(NODE f,int n);
DP_pairs minp( DP_pairs d, DP_pairs *prest );
void minsugar(DP_pairs d,DP_pairs *dm,DP_pairs *dr);
NODE gb(NODE f,int m,NODE subst);
NODE gb_mod(NODE f,int m);
DP_pairs updpairs( DP_pairs d, NODE /* of index */ g, int t);
DP_pairs newpairs( NODE /* of index */ g, int t );
DP_pairs criterion_B( DP_pairs d, int s );
DP_pairs criterion_M( DP_pairs d1 );
int criterion_2( int dp1, int dp2 );
DP_pairs criterion_F( DP_pairs d1 );
NODE updbase(NODE g,int t);
NODE /* of index */ remove_reducibles(NODE /* of index */ nd, int newdp );
int dl_redble(DL dl1,DL dl2);
int dl_weyl_weight(DL dl);
int gbcheck(NODE f);
int membercheck(NODE f,NODE x);
void dp_set_flag(Obj name,Obj value);
void dp_make_flaglist(LIST *list);
void dp_save(int index,Obj p,char *prefix);
void dp_load(int index,DP *p);
int dp_load_t(int index,DP *p);
void init_stat();
void print_stat();
void dp_mulc_d(DP p,P c,DP *r);
void _dp_nf(NODE b,DP g,DP *ps,int full,DP *rp);
void _dp_nf_z(NODE b,DP g,DP *ps,int full,int multiple,DP *r);
void dp_imul_d(DP p,Q q,DP *rp);
void imulv(VECT w,Q c,VECT *rp);
void dptoca(DP p,unsigned int **rp);
int yylex();
void purge_stdin();
int afternl();
int aftercomment();
int myatoi(char *s);
void yyerror(char *s);
int Egetc(FILE *fp);
void Eungetc(int c,FILE *fp);
int readline_getc();
void readline_ungetc();
char *readline_console(char *prompt);
void ox_usr1_handler(int sig);
void env_init();
void loadasirfile(char *name0);
void execasirfile(char *name);
void load_and_execfile(char *name);
int loadfile(char *s);
int loadfiles(NODE node);
unsigned char encrypt_char(unsigned char c);
unsigned char decrypt_char(unsigned char c);
void encrypt_file(char *in,char *out);
void decrypt_file(char *in,char *out);
void get_vars(Obj t,VL *vlp);
void get_vars_recursive(Obj obj,VL *vlp);
int comp_obj(Obj *a,Obj *b);
int generic_comp_obj(Obj *a,Obj *b);
void sepvect(VECT v,int d,VECT *rp);
int gauss_elim_mod(int **mat,int row,int col,int md);
int generic_gauss_elim(MAT mat,MAT *nm,Q *dn,int **rindp,int **cindp);
int generic_gauss_elim_hensel(MAT mat,MAT *nmmat,Q *dn,int **rindp,int **cindp);
int gensolve_check(MAT mat,MAT nm,Q dn,int *rind,int *cind);
int inttorat(N c,N m,N b,int *sgnp,N *nmp,N *dnp);
int intmtoratm(MAT mat,N md,MAT nm,Q *dn);
int intmtoratm_q(MAT mat,N md,MAT nm,Q *dn);
void reduce_reducers_mod(int **mat,int row,int col,int md);
void pre_reduce_mod(int **mat,int row,int col,int nred,int md);
void reduce_sp_by_red_mod(int *sp,int **redmat,int *ind,int nred,int col,int md);
void red_by_compress(int m,unsigned int *p,unsigned int *r,
	unsigned int *ri,unsigned int hc,int len);
void red_by_vect(int m,unsigned int *p,unsigned int *r,unsigned int hc,int len);
void reduce_sp_by_red_mod_compress (int *sp,CDP *redmat,int *ind,
	int nred,int col,int md);
int generic_gauss_elim_mod(int **mat0,int row,int col,int md,int *colstat);
int lu_gfmmat(GFMMAT mat,unsigned int md,int *perm);
int find_lhs_and_lu_mod(unsigned int **a,int row,int col,
	unsigned int md,int **rinfo,int **cinfo);
void solve_by_lu_mod(int **a,int n,int md,int **b,int l);
int gauss_elim_mod1(int **mat,int row,int col,int md);
int gauss_elim_geninv_mod(unsigned int **mat,int row,int col,int md);
void solve_by_lu_gfmmat(GFMMAT lu,unsigned int md,unsigned int *b,unsigned int *x);
void mat_to_gfmmat(MAT m,unsigned int md,GFMMAT *rp);
int gauss_elim_geninv_mod_swap(unsigned int **mat,int row,int col,
	unsigned int md,unsigned int ***invmatp,int **indexp);
void inner_product_int(Q *a,Q *b,int n,Q *r);
void inner_product_mat_int_mod(Q **a,int **b,int n,int k,int l,Q *r);
int generate_ONB_polynomial(UP2 *rp,int m,int type);
int _generate_irreducible_polynomial(UP2 f,int d);
int _generate_good_irreducible_polynomial(UP2 f,int d);
void printqmat(Q **mat,int row,int col);
void printimat(int **mat,int row,int col);
void ixor(N n1,N n2,N *r);
void ior(N n1,N n2,N *r);
void iand(N n1,N n2,N *r);
void isqrt(N a,N *r);
void igcd_cofactor(Q a,Q b,Q *gcd,Q *ca,Q *cb);
int comp_n(N *a,N *b);
int TypeT_NB_check(unsigned int m, unsigned int t);
int small_jacobi(int a,int m);
void pushpvs(FUNC f);
void poppvs();
unsigned int makepvar(char *str);
int searchpvar(char *str);
int getpvar(VS pvs,char *str,int searchonly);
void closecurrentinput();
void resetpvs();
void savepvs();
void restorepvs();
void storeans(pointer p);
void get_rootdir(char *name,int len);
void set_rootdir(char *name);
int process_id();
void call_exe(char *name,char **av);
void addquote(VL vl,QUOTE a,QUOTE b,QUOTE *c);
void subquote(VL vl,QUOTE a,QUOTE b,QUOTE *c);
void mulquote(VL vl,QUOTE a,QUOTE b,QUOTE *c);
void divquote(VL vl,QUOTE a,QUOTE b,QUOTE *c);
void pwrquote(VL vl,QUOTE a,QUOTE b,QUOTE *c);
void chsgnquote(QUOTE a,QUOTE *c);
void objtoquote(Obj a,QUOTE *c);
void polytoquote(P a,QUOTE *c);
void dptoquote(DP a,QUOTE *c);
void dctoquote(DCP dc,QUOTE v,QUOTE *c,int *sgn);
void mptoquote(MP m,int n,QUOTE *c,int *sgn);
void vartoquote(V v,QUOTE *c);
void fnodetotree(FNODE f,LIST *rp);
FNODE eval_pvar_in_fnode(FNODE f);
FNODE subst_in_fnode(FNODE f,V v,FNODE g);
char *get_attribute(char *key,LIST attr);
void treetofnode(Obj obj,FNODE *f);
void glob_init() ;
void input_init(FILE *fp,char *name);
void notdef(VL vl,Obj a,Obj b,Obj *c);
void ExitAsir() ;
void asir_terminate(int status);
void param_init() ;
void prompt() ;
void sprompt(char *ptr);
void process_args(int ac,char **av);
void sig_init() ;
static void (*old_int)(int);;
void asir_save_handler() ;
void asir_set_handler() ;
void asir_reset_handler() ;
void resetenv(char *s);
void fatal(int n);
void int_handler(int sig);
void restore_handler() ;
void segv_handler(int sig);
void ill_handler(int sig);
void alrm_handler(int sig);
void bus_handler(int sig);
void fpe_handler(int sig);
void pipe_handler(int sig);
void resize_buffer();
void tty_init() ;
void tty_reset() ;
void set_lasterror(char *s);
void error(char *s);
void set_timer(int interval);
void reset_timer();
void copyright() ;
void show_debug_window(int on);
void init_cmdwin();
void searchpf(char *name,FUNC *fp);
void searchc(char *name,FUNC *fp);
void mkpf(char *name,Obj body,int argc,V *args,
	int (*parif)(),double (*libmf)(), int (*simp)(),PF *pfp);
void mkpfins(PF pf,V *args,V *vp);
void _mkpfins(PF pf,Obj *args,V *vp);
void _mkpfins_with_darray(PF pf,Obj *args,int *darray,V *vp);
void appendpfins(V v,V *vp);
void duppfins(V v,V *vp);
void derivvar(VL vl,V pf,V v,Obj *a);
void derivr(VL vl,Obj a,V v,Obj *b);
void substr(VL vl,int partial,Obj a,V v,Obj b,Obj *c);
void substpr(VL vl,int partial,Obj p,V v0,Obj p0,Obj *pr);
void evalr(VL vl,Obj a,int prec,Obj *c);
void evalp(VL vl,P p,int prec,P *pr);
void evalv(VL vl,V v,int prec,Obj *rp);
void evalins(PFINS ins,int prec,Obj *rp);
void devalr(VL vl,Obj a,Obj *c);
void devalp(VL vl,P p,P *pr);
void devalv(VL vl,V v,Obj *rp);
void devalins(PFINS ins,Obj *rp);
void simplify_ins(PFINS ins,Obj *rp);
void instov(PFINS ins,V *vp);
void substfr(VL vl,Obj a,PF u,PF f,Obj *c);
void substfp(VL vl,Obj p,PF u,PF f,Obj *pr);
void substfv(VL vl,V v,PF u,PF f,Obj *c);
int structdef(char *name,NODE member);
void newstruct(int type,COMP *rp);
int structtoindex(char *name);
int membertoindex(int type,char *name);
int getcompsize(int type);
Obj memberofstruct(COMP a,char *name);
void assign_to_member(COMP a,char *name,Obj obj);
void node_concat_dup(NODE n1,NODE n2,NODE *nr);
pointer evall(lid id,Obj a1,Obj a2);
pointer eval(FNODE f);
pointer evalstat(SNODE f);
pointer evalnode(NODE node);
pointer evalf(FUNC f,FNODE a,FNODE opt);
pointer evalmapf(FUNC f,FNODE a);
pointer eval_rec_mapf(FUNC f,FNODE a);
pointer beval_rec_mapf(FUNC f,NODE node);
pointer bevalf(FUNC f,NODE a);
pointer evalif(FNODE f,FNODE a);
pointer evalpf(PF pf,NODE args);
void evalnodebody(NODE sn,NODE *dnp);
void gen_searchf(char *name,FUNC *r);
void searchf(NODE fn,char *name,FUNC *r);
void appenduf(char *name,FUNC *r);
void mkparif(char *name,FUNC *r);
void mkuf(char *name,char *fname,NODE args,SNODE body,int startl,int endl,char *desc,MODULE module);
Obj getopt_from_cpvs(char *key);
void	des_enc(unsigned long *, unsigned char *, unsigned long *);
unsigned long	round_func(unsigned long , unsigned char *);
unsigned long	s_box_func(unsigned char *);
void	des_dec(unsigned long *, unsigned char *, unsigned long *);
void	key_schedule(unsigned char *,unsigned char *);
void debug_init();
void add_alias(char *com,char *alias);
void show_alias(char *alias);
void debug(SNODE f);
void setf(int ac,char **av);
void setbp(char *p);
void settp(char *p);
void clearbp(FUNC f);
int searchbp();
void delbp(int ac,char **av);
void showbps();
void showbp(int n);
void searchsn(SNODE *fp,int n,SNODE **fpp);
void bp(SNODE f);
void println(int ac,char **av,int l);
void printvars(char *s,VS vs);
void showpos();
void showpos_to_string(char *buf);
void change_stack(int level,NODE *pvss);
void show_stack(VS vs);
int estimate_length(VL vl,pointer p);
void send_progress(short per,char *msg);
void set_error(int code,char *reason,char *action);
double get_current_time();
