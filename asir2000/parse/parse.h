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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/parse.h,v 1.3 2000/08/21 08:31:47 noro Exp $ 
*/
# if defined(THINK_C) || defined(VISUAL)
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
	I_FUNC, I_FUNC_OPT, I_IFUNC, I_MAP, I_PFDERIV,
	I_ANS, I_PVAR, I_ASSPVAR, 
	I_FORMULA, I_LIST, I_STR, I_NEWCOMP, I_CAR, I_CDR, I_CAST,
	I_COM, I_PROC, I_INDEX, I_EV, I_TIMER, I_GF2NGEN, I_GFPNGEN, I_LOP,
	I_OPT, I_GETOPT
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

struct oVS {
	unsigned int n;
	unsigned int asize;
	unsigned int at;
	unsigned int level;
	struct oFUNC *usrf;
	struct oPV *va;
	NODE opt;
};

typedef struct oVS *VS;

struct oPV {
	char *name;
	short attr,type;
	pointer priv;
};

typedef struct oPV *PV;

struct oIN {
	char *name;
	FILE *fp;
	int ln;
	struct oIN *next;
	char *tname;
	int encoded;
	short vol;
};

typedef struct oIN *IN;

struct oTKWD {
	char *name;
	int token;
};

struct oARF {
	char *name;
	void (*fp)();
};

typedef struct oARF *ARF;

struct oFUNC {
	char *name;
	int argc;
	int type;
	aid id;
	union {
		void (*binf)();
		struct oUSRF *usrf;
		struct oPF *puref;
	} f;
};

typedef struct oFUNC *FUNC;

struct oUSRF {
	char *fname;
	short vol;
	int startl,endl;
	NODE args;
	VS pvs;
	char *desc;
	struct oSNODE *body;
};

typedef struct oUSRF *USRF;

struct oPF {
	char *name;
	int argc;
	Obj body;
	V *args;
	Obj *deriv;
	NODE ins;
	int (*pari)();
	double (*libm)();
	int (*simplify)();
};

typedef struct oPF *PF;

struct oPFAD {
	Obj arg;
	int d;
};

typedef struct oPFAD *PFAD;

struct oPFINS {
	struct oPF *pf;	
	struct oPFAD ad[1];
};

typedef struct oPFINS *PFINS;

#define NEWPF(p) ((p)=(PF)MALLOC(sizeof(struct oPF)))

struct oFNODE {
	fid id;
	pointer arg[1];
};

typedef struct oFNODE *FNODE;

#define NEWFNODE(a,b) \
((a)=(FNODE)MALLOC(sizeof(struct oFNODE)+sizeof(pointer)*(b-1)))

struct oSNODE {
	sid id;
	int ln;
	pointer arg[1];
};

typedef struct oSNODE *SNODE;

#define NEWSNODE(a,b) \
((a)=(SNODE)MALLOC(sizeof(struct oSNODE)+sizeof(pointer)*(b-1)),(a)->ln=asir_infile->ln)

struct oPVI {
	int pv;
	NODE index;
};

typedef struct oPVI *PVI;

struct oNODE2 {
	pointer body1,body2;
	struct oNODE2 *next;
};

typedef struct oNODE2 *NODE2;

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
#define MSB (1<<31)
#define GETPV(i,p) \
((int)(i)>=0	? (int)((p)=CPVS->va[(unsigned int)(i)].priv)\
		: (int)((p)=GPVS->va[(unsigned int)(i)&(~MSB)].priv))
#define ASSPV(i,p) \
((int)(i)>=0	? (int)(CPVS->va[(unsigned int)(i)].priv=(pointer)(p))\
		: (int)(GPVS->va[(unsigned int)(i)&(~MSB)].priv=(pointer)(p)))

#define NEWNODE2(a) ((a)=(NODE2)MALLOC(sizeof(struct oNODE2)))
#define MKNODE2(a,b,c,d) \
(NEWNODE2(a),(a)->body1=(pointer)b,(a)->body2=(pointer)c,NEXT(a)=(NODE2)(d))
#define BDY1(a) ((a)->body1)
#define BDY2(a) ((a)->body2)

extern VS GPVS,CPVS,EPVS,APVS;
extern ARF addfs, subfs, mulfs, divfs, remfs, pwrfs;
extern IN asir_infile;
extern NODE usrf,sysf,noargsysf,ubinf,parif,ONENODE;
extern int nextbp,nextbplevel;
extern int Verbose;

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

#if defined(THINK_C) || defined(VISUAL)
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

#if 1 || defined(THINK_C) || defined(VISUAL)
void dp_ptozp2_d(NODE,int,DP,DP,DP *,DP *);
void dp_ptozp_d(NODE,int,DP,DP *);
void dp_ptozp_d_old(NODE,int,DP,DP *);
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
void homogenize_order(struct order_spec *,int,struct order_spec *);
int create_order_spec(Obj,struct order_spec *);

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
void mkpvs(void);
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
int makepvar(char *);
int membertoindex(int,char *);
int qcoefp(Obj);
int qcoefr(Obj);
int rangecheck(Obj,int);
int structtoindex(char *);
pointer eval(FNODE);
pointer evalf(FUNC,FNODE,FNODE);
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
void memberofstruct(FNODE,char *,FNODE *);
void mkpf(char *,Obj ,int ,V *,int (*)(),double (*)(),int (*)(),PF *);
void mkpfins(PF ,V *,V *);
void mkuf(char *,char *,NODE,SNODE,int,int,char *);
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
void structdef(char *,NODE);
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
#else
int arf_comp();
int complist();
int complist();
int getcompsize();
int getpvar();
int getpvar();
int gettype();
int indextotype();
int loadfile();
int loadfiles();
int makepvar();
int membertoindex();
int qcoefp();
int qcoefr();
int rangecheck();
int structtoindex();
pointer eval();
pointer evalf();
pointer evalif();
pointer evalnode();
pointer evalpf();
pointer evalparif();
pointer evalpv();
pointer evalstat();
void _mkpfins();
void appendpfins();
void appenduf();
void arf_add();
void arf_chsgn();
void arf_div();
void arf_init();
void arf_mul();
void arf_pwr();
void arf_remain();
void arf_sub();
void bp();
void clearbp();
void debug();
void debug_init();
void delbp();
void derivr();
void derivvar();
void duppfins();
void error();
void evalins();
void evalnodebody();
void evalp();
void evalr();
void evalv();
void getarrayp();
void getmember();
void getmemberp();
void help();
void instov();
void memberofstruct();
void mkpf();
void mkpfins();
void mkuf();
void newstruct();
void optobj();
void println();
void printvars();
void resetpvs();
void searchc();
void searchf();
void searchpf();
void searchsn();
void setbp();
void setf();
void setstruct();
void showbp();
void showbps();
void showpos();
void simplify_ins();
void sprintvars();
void structdef();
void substfp();
void substfr();
void substfv();
void substpr();
void substr();
void appendtonode();
void getarray();
void putarray();

void addcomp();
void subcomp();
void mulcomp();
void divcomp();
void chsgncomp();
void pwrcomp();
int compcomp();
#endif
