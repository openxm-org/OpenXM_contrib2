#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "base.h"
#include "inline.h"
#include <time.h>

#if defined(__GNUC__)
#define INLINE inline
#elif defined(VISUAL)
#define INLINE __inline
#else
#define INLINE
#endif

typedef unsigned int UINT;

#define USE_GEOBUCKET 1
#define USE_UNROLL 1

#define REDTAB_LEN 32003

/* GeoBucket for polynomial addition */

typedef struct oPGeoBucket {
	int m;
	struct oND *body[32];
} *PGeoBucket;

/* distributed polynomial; linked list rep. */
typedef struct oND {
	struct oNM *body;
	int nv;
	int len;
	int sugar;
} *ND;

/* distributed polynomial; array rep. */
typedef struct oNDV {
	struct oNMV *body;
	int nv;
	int len;
	int sugar;
} *NDV;

typedef union oNDC {
	int m;
	Q z;
	P p;
} *NDC;

/* monomial; linked list rep. */
typedef struct oNM {
	struct oNM *next;
	union oNDC c;
	UINT dl[1];
} *NM;

/* monomial; array rep. */
typedef struct oNMV {
	union oNDC c;
	UINT dl[1];
} *NMV;

/* history of reducer */
typedef struct oRHist {
	struct oRHist *next;
	int index;
	int sugar;
	UINT dl[1];
} *RHist;

/* S-pair list */
typedef struct oND_pairs {
	struct oND_pairs *next;
	int i1,i2;
	int sugar;
	UINT lcm[1];
} *ND_pairs;

/* index and shift count for each exponent */
typedef struct oEPOS {
	int i; /* index */
	int s; /* shift */
} *EPOS;

typedef struct oBlockMask {
	int n;
	struct order_pair *order_pair;
	UINT **mask;
} *BlockMask;

typedef struct oBaseSet {
	int len;
	NDV *ps;
	UINT **bound;
} *BaseSet;

typedef struct oNM_ind_pair
{
	NM mul;
	int index,sugar;
} *NM_ind_pair;

typedef struct oIndArray
{
	char width;
	int head;
	union {
		unsigned char *c;
		unsigned short *s;
		unsigned int *i;
	} index;
} *IndArray;

extern int (*ndl_compare_function)(UINT *a1,UINT *a2);
extern int nd_dcomp;

extern NM _nm_free_list;
extern ND _nd_free_list;
extern ND_pairs _ndp_free_list;

extern struct order_spec *dp_current_spec;
extern char *Demand;
extern VL CO;
extern int Top,Reverse,DP_Print,dp_nelim,do_weyl,NoSugar;
extern int *current_weyl_weight_vector;

/* fundamental macros */
#define TD(d) (d[0])
#define HDL(d) ((d)->body->dl)
#define HTD(d) (TD(HDL(d)))
#define HCM(d) ((d)->body->c.m)
#define HCQ(d) ((d)->body->c.z)
#define HCP(d) ((d)->body->c.p)
#define CM(a) ((a)->c.m)
#define CQ(a) ((a)->c.z)
#define CP(a) ((a)->c.p)
#define DL(a) ((a)->dl)
#define SG(a) ((a)->sugar)
#define LEN(a) ((a)->len)
#define LCM(a) ((a)->lcm)
#define GET_EXP(d,a) (((d)[nd_epos[a].i]>>nd_epos[a].s)&nd_mask0)
#define GET_EXP_MASK(d,a,m) ((((d)[nd_epos[a].i]&(m)[nd_epos[a].i])>>nd_epos[a].s)&nd_mask0)
#define PUT_EXP(r,a,e) ((r)[nd_epos[a].i] |= ((e)<<nd_epos[a].s))
#define XOR_EXP(r,a,e) ((r)[nd_epos[a].i] ^= ((e)<<nd_epos[a].s))

#define GET_EXP_OLD(d,a) (((d)[oepos[a].i]>>oepos[a].s)&omask0)
#define PUT_EXP_OLD(r,a,e) ((r)[oepos[a].i] |= ((e)<<oepos[a].s))

#define ROUND_FOR_ALIGN(s) ((((s)+sizeof(void *)-1)/sizeof(void *))*sizeof(void *))

/* macros for term comparison */
#define TD_DL_COMPARE(d1,d2)\
(TD(d1)>TD(d2)?1:(TD(d1)<TD(d2)?-1:ndl_lex_compare(d1,d2)))
#if 0
#define DL_COMPARE(d1,d2)\
(nd_dcomp>0?TD_DL_COMPARE(d1,d2)\
         :(nd_dcomp==0?ndl_lex_compare(d1,d2)\
                     :(nd_blockmask?ndl_block_compare(d1,d2)\
								   :(*ndl_compare_function)(d1,d2))))
#else
#define DL_COMPARE(d1,d2)\
(nd_dcomp>0?TD_DL_COMPARE(d1,d2):(*ndl_compare_function)(d1,d2))
#endif

/* allocators */
#define NEWRHist(r) \
((r)=(RHist)MALLOC(sizeof(struct oRHist)+(nd_wpd-1)*sizeof(UINT)))
#define NEWND_pairs(m) \
if(!_ndp_free_list)_NDP_alloc();\
(m)=_ndp_free_list; _ndp_free_list = NEXT(_ndp_free_list)   
#define NEWNM(m)\
if(!_nm_free_list)_NM_alloc();\
(m)=_nm_free_list; _nm_free_list = NEXT(_nm_free_list)   
#define MKND(n,m,len,d)\
if(!_nd_free_list)_ND_alloc();\
(d)=_nd_free_list; _nd_free_list = (ND)BDY(_nd_free_list);\
NV(d)=(n); LEN(d)=(len); BDY(d)=(m)
#define NEWNDV(d) ((d)=(NDV)MALLOC(sizeof(struct oNDV)))
#define MKNDV(n,m,l,d) NEWNDV(d); NV(d)=(n); BDY(d)=(m); LEN(d) = l;
#define NEWNM_ind_pair(p)\
((p)=(NM_ind_pair)MALLOC(sizeof(struct oNM_ind_pair)))

/* allocate and link a new object */
#define NEXTRHist(r,c) \
if(!(r)){NEWRHist(r);(c)=(r);}else{NEWRHist(NEXT(c));(c)=NEXT(c);}
#define NEXTNM(r,c) \
if(!(r)){NEWNM(r);(c)=(r);}else{NEWNM(NEXT(c));(c)=NEXT(c);}
#define NEXTNM2(r,c,s) \
if(!(r)){(c)=(r)=(s);}else{NEXT(c)=(s);(c)=(s);}
#define NEXTND_pairs(r,c) \
if(!(r)){NEWND_pairs(r);(c)=(r);}else{NEWND_pairs(NEXT(c));(c)=NEXT(c);}
#define MKNM_ind_pair(p,m,i,s) (NEWNM_ind_pair(p),(p)->mul=(m),(p)->index=(i),(p)->sugar = (s))

/* deallocators */
#define FREENM(m) NEXT(m)=_nm_free_list; _nm_free_list=(m)
#define FREENDP(m) NEXT(m)=_ndp_free_list; _ndp_free_list=(m)
#define FREEND(m) BDY(m)=(NM)_nd_free_list; _nd_free_list=(m)

/* macro for increasing pointer to NMV */
#define NMV_ADV(m) (m = (NMV)(((char *)m)+nmv_adv))
#define NMV_OADV(m) (m = (NMV)(((char *)m)+oadv))
#define NDV_NADV(m) (m = (NMV)(((char *)m)+newadv))
#define NMV_PREV(m) (m = (NMV)(((char *)m)-nmv_adv))
#define NMV_OPREV(m) (m = (NMV)(((char *)m)-oadv))

/* external functions */
#if 1
void GC_gcollect();
#endif
NODE append_one(NODE,int);

/* manipulation of coefficients */
void nd_removecont(int mod,ND p);
void nd_removecont2(ND p1,ND p2);
void removecont_array(Q *c,int n);

/* GeoBucket functions */
ND normalize_pbucket(int mod,PGeoBucket g);
int head_pbucket(int mod,PGeoBucket g);
int head_pbucket_q(PGeoBucket g);
void add_pbucket_symbolic(PGeoBucket g,ND d);
void add_pbucket(int mod,PGeoBucket g,ND d);
void free_pbucket(PGeoBucket b);
void mulq_pbucket(PGeoBucket g,Q c);
NM remove_head_pbucket_symbolic(PGeoBucket g);
PGeoBucket create_pbucket();

/* manipulation of pairs and bases */
int nd_newps(int mod,ND a,ND aq);
ND_pairs nd_newpairs( NODE g, int t );
ND_pairs nd_minp( ND_pairs d, ND_pairs *prest );
ND_pairs nd_minsugarp( ND_pairs d, ND_pairs *prest );
NODE update_base(NODE nd,int ndp);
ND_pairs update_pairs( ND_pairs d, NODE /* of index */ g, int t);
ND_pairs equivalent_pairs( ND_pairs d1, ND_pairs *prest );
ND_pairs crit_B( ND_pairs d, int s );
ND_pairs crit_M( ND_pairs d1 );
ND_pairs crit_F( ND_pairs d1 );
int crit_2( int dp1, int dp2 );
int ndv_newps(int m,NDV a,NDV aq);

/* top level functions */
void nd_gr(LIST f,LIST v,int m,int f4,struct order_spec *ord,LIST *rp);
void nd_gr_trace(LIST f,LIST v,int trace,int homo,struct order_spec *ord,LIST *rp);
NODE nd_f4(int m);
NODE nd_gb(int m,int ishomo,int checkonly);
NODE nd_gb_trace(int m,int ishomo);

/* ndl functions */
int ndl_weight(UINT *d);
void ndl_weight_mask(UINT *d);
void ndl_homogenize(UINT *d,UINT *r,int obpe,EPOS oepos,int weight);
void ndl_dehomogenize(UINT *p);
void ndl_reconstruct(UINT *d,UINT *r,int obpe,EPOS oepos);
INLINE int ndl_reducible(UINT *d1,UINT *d2);
INLINE int ndl_lex_compare(UINT *d1,UINT *d2);
INLINE int ndl_block_compare(UINT *d1,UINT *d2);
INLINE int ndl_equal(UINT *d1,UINT *d2);
INLINE void ndl_copy(UINT *d1,UINT *d2);
INLINE void ndl_zero(UINT *d);
INLINE void ndl_add(UINT *d1,UINT *d2,UINT *d);
INLINE void ndl_addto(UINT *d1,UINT *d2);
INLINE void ndl_sub(UINT *d1,UINT *d2,UINT *d);
INLINE int ndl_hash_value(UINT *d);

/* normal forms */
INLINE int ndl_find_reducer(UINT *g);
int nd_sp(int mod,int trace,ND_pairs p,ND *nf);
int nd_nf(int mod,ND g,NDV *ps,int full,NDC dn,ND *nf);
int nd_nf_pbucket(int mod,ND g,NDV *ps,int full,ND *nf);

/* finalizers */
NODE ndv_reducebase(NODE x);
NODE ndv_reduceall(int m,NODE f);

/* allocators */
void nd_free_private_storage();
void _NM_alloc();
void _ND_alloc();
void nd_free(ND p);
void nd_free_redlist();

/* printing */
void ndl_print(UINT *dl);
void nd_print(ND p);
void nd_print_q(ND p);
void ndp_print(ND_pairs d);


/* setup, reconstruct */
void nd_init_ord(struct order_spec *spec);
ND_pairs nd_reconstruct(int mod,int trace,ND_pairs ndp);
void ndv_setup(int mod,int trace,NODE f);
void nd_setup_parameters(int nvar,int max);
BlockMask nd_create_blockmask(struct order_spec *ord);
EPOS nd_create_epos(struct order_spec *ord);
int nd_get_exporigin(struct order_spec *ord);
void ndv_mod(int mod,NDV p);
NDV ndv_dup(int mod,NDV p);
ND nd_dup(ND p);

/* ND functions */
int ndv_check_candidate(NODE input,int obpe,int oadv,EPOS oepos,NODE cand);
void nd_mul_c(int mod,ND p,int mul);
void nd_mul_c_q(ND p,Q mul);
void nd_mul_c_p(VL vl,ND p,P mul);
ND nd_remove_head(ND p);
ND nd_separate_head(ND p,ND *head);
int nd_length(ND p);
void nd_append_red(UINT *d,int i);
UINT *ndv_compute_bound(NDV p);
ND nd_copy(ND p);
ND nd_merge(ND p1,ND p2);
ND nd_add(int mod,ND p1,ND p2);
ND nd_add_q(ND p1,ND p2);
ND nd_add_sf(ND p1,ND p2);
INLINE int nd_length(ND p);
NODE nd_f4_red(int m,ND_pairs sp0,UINT *s0vect,int col,NODE rp0);
NODE nd_f4_red_dist(int m,ND_pairs sp0,UINT *s0vect,int col,NODE rp0);

/* NDV functions */
ND weyl_ndv_mul_nm(int mod,NM m0,NDV p);
void weyl_mul_nm_nmv(int n,int mod,NM m0,NMV m1,NM *tab,int tlen);
void ndv_mul_c(int mod,NDV p,int mul);
void ndv_mul_c_q(NDV p,Q mul);
ND ndv_mul_nm_symbolic(NM m0,NDV p);
ND ndv_mul_nm(int mod,NM m0,NDV p);
void ndv_realloc(NDV p,int obpe,int oadv,EPOS oepos);
NDV ndv_dup_realloc(NDV p,int obpe,int oadv,EPOS oepos);
void ndv_homogenize(NDV p,int obpe,int oadv,EPOS eops);
void ndv_dehomogenize(NDV p,struct order_spec *spec);
void ndv_removecont(int mod,NDV p);
void ndv_print(NDV p);
void ndv_print_q(NDV p);
void ndv_free(NDV p);
void ndv_save(NDV p,int index);
NDV ndv_load(int index);

/* converters */
ND ptond(VL vl,VL dvl,P p);
NDV ptondv(VL vl,VL dvl,P p);
P ndvtop(int mod,VL vl,VL dvl,NDV p);
NDV ndtondv(int mod,ND p);
ND ndvtond(int mod,NDV p);
int nm_ind_pair_to_vect(int m,UINT *s0,int n,NM_ind_pair pair,UINT *r);
IndArray nm_ind_pair_to_vect_compress(int m,UINT *s0,int n,NM_ind_pair pair);
int nd_to_vect(int mod,UINT *s0,int n,ND d,UINT *r);

/* elimination */
int nd_gauss_elim_mod(int **mat0,int *sugar,int row,int col,int md,int *colstat);
int nd_gauss_elim_sf(int **mat0,int *sugar,int row,int col,int md,int *colstat);

int ndl_ww_lex_compare(UINT *a1,UINT *a2);

