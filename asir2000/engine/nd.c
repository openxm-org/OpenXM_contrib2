/* $OpenXM: OpenXM_contrib2/asir2000/engine/nd.c,v 1.59 2003/09/05 13:20:14 noro Exp $ */

#include "ca.h"
#include "inline.h"

#if defined(__GNUC__)
#define INLINE inline
#elif defined(VISUAL)
#define INLINE __inline
#else
#define INLINE
#endif

#define USE_GEOBUCKET 1

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

/* monomial; linked list rep. */
typedef struct oNM {
	struct oNM *next;
	union {
		int m;
		Q z;
	} c;
	unsigned int dl[1];
} *NM;

/* monomial; array rep. */
typedef struct oNMV {
	union {
		int m;
		Q z;
	} c;
	unsigned int dl[1];
} *NMV;

/* history of reducer */
typedef struct oRHist {
	struct oRHist *next;
	int index;
	int sugar;
	unsigned int dl[1];
} *RHist;

/* S-pair list */
typedef struct oND_pairs {
	struct oND_pairs *next;
	int i1,i2;
	int sugar;
	unsigned int lcm[1];
} *ND_pairs;

/* index and shift count for each exponent */
typedef struct oEPOS {
	int i; /* index */
	int s; /* shift */
} *EPOS;

typedef struct oBlockMask {
	int n;
	struct order_pair *order_pair;
	unsigned int **mask;
} *BlockMask;

typedef struct oBaseSet {
	int len;
	NDV *ps;
	unsigned int **bound;
} *BaseSet;

int (*ndl_compare_function)(unsigned int *a1,unsigned int *a2);

static double nd_scale=2;
static unsigned int **nd_bound;
static struct order_spec *nd_ord;
static EPOS nd_epos;
static BlockMask nd_blockmask;
static int nd_nvar;
static int nd_isrlex;
static int nd_epw,nd_bpe,nd_wpd,nd_exporigin;
static unsigned int nd_mask[32];
static unsigned int nd_mask0,nd_mask1;

static NM _nm_free_list;
static ND _nd_free_list;
static ND_pairs _ndp_free_list;

static NDV *nd_ps;
static NDV *nd_ps_trace;
static RHist *nd_psh;
static int nd_psn,nd_pslen;

static RHist *nd_red;

static int nd_found,nd_create,nd_notfirst;
static int nm_adv;
static int nmv_adv;
static int nd_dcomp;

extern int Top,Reverse,dp_nelim,do_weyl;
extern int *current_weyl_weight_vector;

/* fundamental macros */
#define TD(d) (d[0])
#define HDL(d) ((d)->body->dl)
#define HTD(d) (TD(HDL(d)))
#define HCM(d) ((d)->body->c.m)
#define HCQ(d) ((d)->body->c.z)
#define CM(a) ((a)->c.m)
#define CQ(a) ((a)->c.z)
#define DL(a) ((a)->dl)
#define SG(a) ((a)->sugar)
#define LEN(a) ((a)->len)
#define LCM(a) ((a)->lcm)
#define GET_EXP(d,a) (((d)[nd_epos[a].i]>>nd_epos[a].s)&nd_mask0)
#define GET_EXP_MASK(d,a,m) ((((d)[nd_epos[a].i]&(m)[nd_epos[a].i])>>nd_epos[a].s)&nd_mask0)
#define PUT_EXP(r,a,e) ((r)[nd_epos[a].i] |= ((e)<<nd_epos[a].s))
#define XOR_EXP(r,a,e) ((r)[nd_epos[a].i] ^= ((e)<<nd_epos[a].s))

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
((r)=(RHist)MALLOC(sizeof(struct oRHist)+(nd_wpd-1)*sizeof(unsigned int)))
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

/* allocate and link a new object */
#define NEXTRHist(r,c) \
if(!(r)){NEWRHist(r);(c)=(r);}else{NEWRHist(NEXT(c));(c)=NEXT(c);}
#define NEXTNM(r,c) \
if(!(r)){NEWNM(r);(c)=(r);}else{NEWNM(NEXT(c));(c)=NEXT(c);}
#define NEXTNM2(r,c,s) \
if(!(r)){(c)=(r)=(s);}else{NEXT(c)=(s);(c)=(s);}
#define NEXTND_pairs(r,c) \
if(!(r)){NEWND_pairs(r);(c)=(r);}else{NEWND_pairs(NEXT(c));(c)=NEXT(c);}

/* deallocators */
#define FREENM(m) NEXT(m)=_nm_free_list; _nm_free_list=(m)
#define FREENDP(m) NEXT(m)=_ndp_free_list; _ndp_free_list=(m)
#define FREEND(m) BDY(m)=(NM)_nd_free_list; _nd_free_list=(m)

/* macro for increasing pointer to NMV */
#define NMV_ADV(m) (m = (NMV)(((char *)m)+nmv_adv))
#define NMV_PREV(m) (m = (NMV)(((char *)m)-nmv_adv))

/* external functions */
void GC_gcollect();
NODE append_one(NODE,int);

/* manipulation of coefficients */
void nd_removecont(int mod,ND p);
void nd_removecont2(ND p1,ND p2);
void removecont_array(Q *c,int n);

/* GeoBucket functions */
ND normalize_pbucket(int mod,PGeoBucket g);
int head_pbucket(int mod,PGeoBucket g);
int head_pbucket_q(PGeoBucket g);
void add_pbucket(int mod,PGeoBucket g,ND d);
void free_pbucket(PGeoBucket b);
void mulq_pbucket(PGeoBucket g,Q c);
PGeoBucket create_pbucket();

/* manipulation of pairs and bases */
int nd_newps(int mod,ND a,ND aq);
ND_pairs nd_newpairs( NODE g, int t );
ND_pairs nd_minp( ND_pairs d, ND_pairs *prest );
NODE update_base(NODE nd,int ndp);
ND_pairs update_pairs( ND_pairs d, NODE /* of index */ g, int t);
ND_pairs equivalent_pairs( ND_pairs d1, ND_pairs *prest );
ND_pairs crit_B( ND_pairs d, int s );
ND_pairs crit_M( ND_pairs d1 );
ND_pairs crit_F( ND_pairs d1 );
int crit_2( int dp1, int dp2 );

/* top level functions */
void nd_gr(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp);
void nd_gr_trace(LIST f,LIST v,int trace,int homo,struct order_spec *ord,LIST *rp);
NODE nd_gb(int m,int checkonly);
NODE nd_gb_trace(int m);

/* ndl functions */
int ndl_weight(unsigned int *d);
int ndl_weight_mask(unsigned int *d,int i);
void ndl_set_blockweight(unsigned int *d);
void ndl_dehomogenize(unsigned int *p);
void ndl_reconstruct(int obpe,EPOS oepos,unsigned int *d,unsigned int *r);
INLINE int ndl_reducible(unsigned int *d1,unsigned int *d2);
INLINE int ndl_lex_compare(unsigned int *d1,unsigned int *d2);
INLINE int ndl_block_compare(unsigned int *d1,unsigned int *d2);
INLINE int ndl_equal(unsigned int *d1,unsigned int *d2);
INLINE void ndl_copy(unsigned int *d1,unsigned int *d2);
INLINE void ndl_add(unsigned int *d1,unsigned int *d2,unsigned int *d);
INLINE void ndl_addto(unsigned int *d1,unsigned int *d2);
INLINE void ndl_sub(unsigned int *d1,unsigned int *d2,unsigned int *d);
INLINE int ndl_hash_value(unsigned int *d);

/* normal forms */
INLINE int nd_find_reducer(ND g);
INLINE int nd_find_reducer_direct(ND g,NDV *ps,int len);
int nd_sp(int mod,int trace,ND_pairs p,ND *nf);
int nd_nf(int mod,ND g,NDV *ps,int full,ND *nf);
int nd_nf_pbucket(int mod,ND g,NDV *ps,int full,ND *nf);
int nd_nf_direct(int mod,ND g,BaseSet base,int full,ND *rp);

/* finalizers */
NODE nd_reducebase(NODE x);
NODE nd_reduceall(int m,NODE f);
int nd_gbcheck(int m,NODE f);
int nd_membercheck(int m,NODE f);

/* allocators */
void nd_free_private_storage();
void _NM_alloc();
void _ND_alloc();
void nd_free(ND p);
void nd_free_redlist();

/* printing */
void ndl_print(unsigned int *dl);
void nd_print(ND p);
void nd_print_q(ND p);
void ndp_print(ND_pairs d);


/* setup, reconstruct */
void nd_init_ord(struct order_spec *spec);
ND_pairs nd_reconstruct(int mod,int trace,ND_pairs ndp);
void nd_reconstruct_direct(int mod,NDV *ps,int len);
void nd_setup(int mod,int trace,NODE f);
void nd_setup_parameters();
BlockMask nd_create_blockmask(struct order_spec *ord);
EPOS nd_create_epos(struct order_spec *ord);
int nd_get_exporigin(struct order_spec *ord);

/* ND functions */
int nd_check_candidate(NODE input,NODE cand);
void nd_mul_c(int mod,ND p,int mul);
void nd_mul_c_q(ND p,Q mul);
ND nd_remove_head(ND p);
int nd_length(ND p);
void nd_append_red(unsigned int *d,int i);
unsigned int *ndv_compute_bound(NDV p);
ND nd_copy(ND p);
ND nd_add(int mod,ND p1,ND p2);
ND nd_add_q(ND p1,ND p2);
INLINE int nd_length(ND p);

/* NDV functions */
ND weyl_ndv_mul_nm(int mod,NM m0,NDV p);
void weyl_mul_nm_nmv(int n,int mod,NM m0,NMV m1,NM *tab,int tlen);
void ndv_mul_c(int mod,NDV p,int mul);
void ndv_mul_c_q(NDV p,Q mul);
void ndv_realloc(NDV p,int obpe,int oadv,EPOS oepos);
ND ndv_mul_nm(int mod,NM m0,NDV p);
void ndv_dehomogenize(NDV p,struct order_spec *spec);
void ndv_removecont(int mod,NDV p);
void ndv_print(NDV p);
void ndv_print_q(NDV p);
void ndv_free(NDV p);

/* converters */
NDV ndtondv(int mod,ND p);
ND ndvtond(int mod,NDV p);
NDV dptondv(int,DP);
DP ndvtodp(int,NDV);
ND dptond(int,DP);
DP ndtodp(int,ND);

void nd_free_private_storage()
{
	_nd_free_list = 0;
	_nm_free_list = 0;
	_ndp_free_list = 0;
	bzero(nd_red,sizeof(REDTAB_LEN*sizeof(RHist)));
	GC_gcollect();
}

void _NM_alloc()
{
	NM p;
	int i;

	for ( i = 0; i < 1024; i++ ) {
		p = (NM)GC_malloc(sizeof(struct oNM)+(nd_wpd-1)*sizeof(unsigned int));
		p->next = _nm_free_list; _nm_free_list = p;
	}
}

void _ND_alloc()
{
	ND p;
	int i;

	for ( i = 0; i < 1024; i++ ) {
		p = (ND)GC_malloc(sizeof(struct oND));
		p->body = (NM)_nd_free_list; _nd_free_list = p;
	}
}

void _NDP_alloc()
{
	ND_pairs p;
	int i;

	for ( i = 0; i < 1024; i++ ) {
		p = (ND_pairs)GC_malloc(sizeof(struct oND_pairs)
			+(nd_wpd-1)*sizeof(unsigned int));
		p->next = _ndp_free_list; _ndp_free_list = p;
	}
}

INLINE int nd_length(ND p)
{
	NM m;
	int i;

	if ( !p )
		return 0;
	else {
		for ( i = 0, m = BDY(p); m; m = NEXT(m), i++ );
		return i;
	}
}

INLINE int ndl_reducible(unsigned int *d1,unsigned int *d2)
{
	unsigned int u1,u2;
	int i,j;

	if ( TD(d1) < TD(d2) ) return 0;
	switch ( nd_bpe ) {
		case 4:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xf0000000) < (u2&0xf0000000) ) return 0;
				if ( (u1&0xf000000) < (u2&0xf000000) ) return 0;
				if ( (u1&0xf00000) < (u2&0xf00000) ) return 0;
				if ( (u1&0xf0000) < (u2&0xf0000) ) return 0;
				if ( (u1&0xf000) < (u2&0xf000) ) return 0;
				if ( (u1&0xf00) < (u2&0xf00) ) return 0;
				if ( (u1&0xf0) < (u2&0xf0) ) return 0;
				if ( (u1&0xf) < (u2&0xf) ) return 0;
			}
			return 1;
			break;
		case 6:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0x3f000000) < (u2&0x3f000000) ) return 0;
				if ( (u1&0xfc0000) < (u2&0xfc0000) ) return 0;
				if ( (u1&0x3f000) < (u2&0x3f000) ) return 0;
				if ( (u1&0xfc0) < (u2&0xfc0) ) return 0;
				if ( (u1&0x3f) < (u2&0x3f) ) return 0;
			}
			return 1;
			break;
		case 8:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xff000000) < (u2&0xff000000) ) return 0;
				if ( (u1&0xff0000) < (u2&0xff0000) ) return 0;
				if ( (u1&0xff00) < (u2&0xff00) ) return 0;
				if ( (u1&0xff) < (u2&0xff) ) return 0;
			}
			return 1;
			break;
		case 16:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xffff0000) < (u2&0xffff0000) ) return 0;
				if ( (u1&0xffff) < (u2&0xffff) ) return 0;
			}
			return 1;
			break;
		case 32:
			for ( i = nd_exporigin; i < nd_wpd; i++ )
				if ( d1[i] < d2[i] ) return 0;
			return 1;
			break;
		default:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				for ( j = 0; j < nd_epw; j++ )
					if ( (u1&nd_mask[j]) < (u2&nd_mask[j]) ) return 0;
			}
			return 1;
	}
}

void ndl_dehomogenize(unsigned int *d)
{
	unsigned int mask;
	unsigned int h;
	int i,bits;

	if ( nd_blockmask ) {
		h = GET_EXP(d,nd_nvar-1);
		XOR_EXP(d,nd_nvar-1,h);
		TD(d) -= h;
		d[nd_exporigin-1] -= h;
	} else {
		if ( nd_isrlex ) {
			if ( nd_bpe == 32 ) {
				h = d[nd_exporigin];
				for ( i = nd_exporigin+1; i < nd_wpd; i++ )
					d[i-1] = d[i];
				d[i-1] = 0;
				TD(d) -= h;
			} else {
				bits = nd_epw*nd_bpe;
				mask = bits==32?0xffffffff:((1<<(nd_epw*nd_bpe))-1);
				h = (d[nd_exporigin]>>((nd_epw-1)*nd_bpe))&nd_mask0;
				for ( i = nd_exporigin; i < nd_wpd; i++ )
					d[i] = ((d[i]<<nd_bpe)&mask)
						|(i+1<nd_wpd?((d[i+1]>>((nd_epw-1)*nd_bpe))&nd_mask0):0);
				TD(d) -= h;
			}
		} else {
			h = GET_EXP(d,nd_nvar-1);
			XOR_EXP(d,nd_nvar-1,h);
			TD(d) -= h;
		}
	}
}

void ndl_lcm(unsigned int *d1,unsigned *d2,unsigned int *d)
{
	unsigned int t1,t2,u,u1,u2;
	int i,j,l;

	switch ( nd_bpe ) {
		case 4:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xf0000000); t2 = (u2&0xf0000000); u = t1>t2?t1:t2;
				t1 = (u1&0xf000000); t2 = (u2&0xf000000); u |= t1>t2?t1:t2;
				t1 = (u1&0xf00000); t2 = (u2&0xf00000); u |= t1>t2?t1:t2;
				t1 = (u1&0xf0000); t2 = (u2&0xf0000); u |= t1>t2?t1:t2;
				t1 = (u1&0xf000); t2 = (u2&0xf000); u |= t1>t2?t1:t2;
				t1 = (u1&0xf00); t2 = (u2&0xf00); u |= t1>t2?t1:t2;
				t1 = (u1&0xf0); t2 = (u2&0xf0); u |= t1>t2?t1:t2;
				t1 = (u1&0xf); t2 = (u2&0xf); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 6:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0x3f000000); t2 = (u2&0x3f000000); u = t1>t2?t1:t2;
				t1 = (u1&0xfc0000); t2 = (u2&0xfc0000); u |= t1>t2?t1:t2;
				t1 = (u1&0x3f000); t2 = (u2&0x3f000); u |= t1>t2?t1:t2;
				t1 = (u1&0xfc0); t2 = (u2&0xfc0); u |= t1>t2?t1:t2;
				t1 = (u1&0x3f); t2 = (u2&0x3f); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 8:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xff000000); t2 = (u2&0xff000000); u = t1>t2?t1:t2;
				t1 = (u1&0xff0000); t2 = (u2&0xff0000); u |= t1>t2?t1:t2;
				t1 = (u1&0xff00); t2 = (u2&0xff00); u |= t1>t2?t1:t2;
				t1 = (u1&0xff); t2 = (u2&0xff); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 16:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xffff0000); t2 = (u2&0xffff0000); u = t1>t2?t1:t2;
				t1 = (u1&0xffff); t2 = (u2&0xffff); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 32:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i]; 
				d[i] = u1>u2?u1:u2;
			}
			break;
		default:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i]; 
				for ( j = 0, u = 0; j < nd_epw; j++ ) {
					t1 = (u1&nd_mask[j]); t2 = (u2&nd_mask[j]); u |= t1>t2?t1:t2;
				}
				d[i] = u;
			}
			break;
	}
	TD(d) = ndl_weight(d);
	if ( nd_blockmask ) {
		l = nd_blockmask->n;
		for ( j = 0; j < l; j++ )
			d[j+1] = ndl_weight_mask(d,j);
	}
}

void ndl_set_blockweight(unsigned int *d) {
	int l,j;

	if ( nd_blockmask ) {
		l = nd_blockmask->n;
		for ( j = 0; j < l; j++ )
			d[j+1] = ndl_weight_mask(d,j);
	}
}

int ndl_weight(unsigned int *d)
{
	unsigned int t,u;
	int i,j;

	if ( current_dl_weight_vector )
		for ( i = 0, t = 0; i < nd_nvar; i++ ) {
			u = GET_EXP(d,i);
			t += MUL_WEIGHT(u,i);
		}
	else
		for ( t = 0, i = nd_exporigin; i < nd_wpd; i++ ) {
			u = d[i];
			for ( j = 0; j < nd_epw; j++, u>>=nd_bpe )
				t += (u&nd_mask0); 
		}
	return t;
}

int ndl_weight_mask(unsigned int *d,int index)
{
	unsigned int t,u;
	unsigned int *mask;
	int i,j;

	mask = nd_blockmask->mask[index];
	if ( current_dl_weight_vector )
		for ( i = 0, t = 0; i < nd_nvar; i++ ) {
			u = GET_EXP_MASK(d,i,mask);
			t += MUL_WEIGHT(u,i);
		}
	else
		for ( t = 0, i = nd_exporigin; i < nd_wpd; i++ ) {
			u = d[i]&mask[i];
			for ( j = 0; j < nd_epw; j++, u>>=nd_bpe )
				t += (u&nd_mask0); 
		}
	return t;
}

int ndl_lex_compare(unsigned int *d1,unsigned int *d2)
{
	int i;

	d1 += nd_exporigin;
	d2 += nd_exporigin;
	for ( i = nd_exporigin; i < nd_wpd; i++, d1++, d2++ )
		if ( *d1 > *d2 )
			return nd_isrlex ? -1 : 1;
		else if ( *d1 < *d2 )
			return nd_isrlex ? 1 : -1;
	return 0;
}

int ndl_block_compare(unsigned int *d1,unsigned int *d2)
{
	int i,l,j,ord_o,ord_l;
	struct order_pair *op;
	unsigned int t1,t2,m;
	unsigned int *mask;

	l = nd_blockmask->n;
	op = nd_blockmask->order_pair; 
	for ( j = 0; j < l; j++ ) {
		mask = nd_blockmask->mask[j];
		ord_o = op[j].order;
		if ( ord_o < 2 )
			if ( (t1=d1[j+1]) > (t2=d2[j+1]) ) return 1;
			else if ( t1 < t2 ) return -1;
		for ( i = nd_exporigin; i < nd_wpd; i++ ) {
			m = mask[i];
			t1 = d1[i]&m;
			t2 = d2[i]&m;
			if ( t1 > t2 )
				return !ord_o ? -1 : 1;
			else if ( t1 < t2 )
				return !ord_o ? 1 : -1;
		}
	}
	return 0;
}

/* TDH -> WW -> TD-> RL */

int ndl_ww_lex_compare(unsigned int *d1,unsigned int *d2)
{
	int i,m,e1,e2;

	if ( TD(d1) > TD(d2) ) return 1;
	else if ( TD(d1) < TD(d2) ) return -1;
	m = nd_nvar>>1;
	for ( i = 0, e1 = e2 = 0; i < m; i++ ) {
		e1 += current_weyl_weight_vector[i]*(GET_EXP(d1,m+i)-GET_EXP(d1,i));
		e2 += current_weyl_weight_vector[i]*(GET_EXP(d2,m+i)-GET_EXP(d2,i));
	}
	if ( e1 > e2 ) return 1;
	else if ( e1 < e2 ) return -1;
	return ndl_lex_compare(d1,d2);
}

INLINE int ndl_equal(unsigned int *d1,unsigned int *d2)
{
	int i;

	for ( i = 0; i < nd_wpd; i++ )
		if ( *d1++ != *d2++ )
			return 0;
	return 1;
}

INLINE void ndl_copy(unsigned int *d1,unsigned int *d2)
{
	int i;

	switch ( nd_wpd ) {
		case 2:
			TD(d2) = TD(d1);
			d2[1] = d1[1];
			break;
		case 3:
			TD(d2) = TD(d1);
			d2[1] = d1[1];
			d2[2] = d1[2];
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ )
				d2[i] = d1[i];
			break;
	}
}

INLINE void ndl_add(unsigned int *d1,unsigned int *d2,unsigned int *d)
{
	int i;

#if 1
	switch ( nd_wpd ) {
		case 2:
			TD(d) = TD(d1)+TD(d2);
			d[1] = d1[1]+d2[1];
			break;
		case 3:
			TD(d) = TD(d1)+TD(d2);
			d[1] = d1[1]+d2[1];
			d[2] = d1[2]+d2[2];
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ ) d[i] = d1[i]+d2[i];
			break;
	}
#else
	for ( i = 0; i < nd_wpd; i++ ) d[i] = d1[i]+d2[i];
#endif
}

/* d1 += d2 */
INLINE void ndl_addto(unsigned int *d1,unsigned int *d2)
{
	int i;

#if 1
	switch ( nd_wpd ) {
		case 2:
			TD(d1) += TD(d2);
			d1[1] += d2[1];
			break;
		case 3:
			TD(d1) += TD(d2);
			d1[1] += d2[1];
			d1[2] += d2[2];
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ ) d1[i] += d2[i];
			break;
	}
#else
	for ( i = 0; i < nd_wpd; i++ ) d1[i] += d2[i];
#endif
}

INLINE void ndl_sub(unsigned int *d1,unsigned int *d2,unsigned int *d)
{
	int i;

	for ( i = 0; i < nd_wpd; i++ ) d[i] = d1[i]-d2[i];
}

int ndl_disjoint(unsigned int *d1,unsigned int *d2)
{
	unsigned int t1,t2,u,u1,u2;
	int i,j;

	switch ( nd_bpe ) {
		case 4:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xf0000000; t2 = u2&0xf0000000; if ( t1&&t2 ) return 0;
				t1 = u1&0xf000000; t2 = u2&0xf000000; if ( t1&&t2 ) return 0;
				t1 = u1&0xf00000; t2 = u2&0xf00000; if ( t1&&t2 ) return 0;
				t1 = u1&0xf0000; t2 = u2&0xf0000; if ( t1&&t2 ) return 0;
				t1 = u1&0xf000; t2 = u2&0xf000; if ( t1&&t2 ) return 0;
				t1 = u1&0xf00; t2 = u2&0xf00; if ( t1&&t2 ) return 0;
				t1 = u1&0xf0; t2 = u2&0xf0; if ( t1&&t2 ) return 0;
				t1 = u1&0xf; t2 = u2&0xf; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 6:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0x3f000000; t2 = u2&0x3f000000; if ( t1&&t2 ) return 0;
				t1 = u1&0xfc0000; t2 = u2&0xfc0000; if ( t1&&t2 ) return 0;
				t1 = u1&0x3f000; t2 = u2&0x3f000; if ( t1&&t2 ) return 0;
				t1 = u1&0xfc0; t2 = u2&0xfc0; if ( t1&&t2 ) return 0;
				t1 = u1&0x3f; t2 = u2&0x3f; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 8:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xff000000; t2 = u2&0xff000000; if ( t1&&t2 ) return 0;
				t1 = u1&0xff0000; t2 = u2&0xff0000; if ( t1&&t2 ) return 0;
				t1 = u1&0xff00; t2 = u2&0xff00; if ( t1&&t2 ) return 0;
				t1 = u1&0xff; t2 = u2&0xff; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 16:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xffff0000; t2 = u2&0xffff0000; if ( t1&&t2 ) return 0;
				t1 = u1&0xffff; t2 = u2&0xffff; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 32:
			for ( i = nd_exporigin; i < nd_wpd; i++ )
				if ( d1[i] && d2[i] ) return 0;
			return 1;
			break;
		default:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				for ( j = 0; j < nd_epw; j++ ) {
					if ( (u1&nd_mask0) && (u2&nd_mask0) ) return 0;
					u1 >>= nd_bpe; u2 >>= nd_bpe;
				}
			}
			return 1;
			break;
	}
}

int ndl_check_bound2(int index,unsigned int *d2)
{
	unsigned int u2;
	unsigned int *d1;
	int i,j,ind,k;

	d1 = nd_bound[index];
	ind = 0;
	switch ( nd_bpe ) {
		case 4:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>28)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>24)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>20)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>16)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>12)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>8)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>4)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+(u2&0xf) >= 0x10 ) return 1;
			}
			return 0;
			break;
		case 6:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>24)&0x3f) >= 0x40 ) return 1;
				if ( d1[ind++]+((u2>>18)&0x3f) >= 0x40 ) return 1;
				if ( d1[ind++]+((u2>>12)&0x3f) >= 0x40 ) return 1;
				if ( d1[ind++]+((u2>>6)&0x3f) >= 0x40 ) return 1;
				if ( d1[ind++]+(u2&0x3f) >= 0x40 ) return 1;
			}
			return 0;
			break;
		case 8:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>24)&0xff) >= 0x100 ) return 1;
				if ( d1[ind++]+((u2>>16)&0xff) >= 0x100 ) return 1;
				if ( d1[ind++]+((u2>>8)&0xff) >= 0x100 ) return 1;
				if ( d1[ind++]+(u2&0xff) >= 0x100 ) return 1;
			}
			return 0;
			break;
		case 16:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>16)&0xffff) > 0x10000 ) return 1;
				if ( d1[ind++]+(u2&0xffff) > 0x10000 ) return 1;
			}
			return 0;
			break;
		case 32:
			for ( i = nd_exporigin; i < nd_wpd; i++ )
				if ( d1[i]+d2[i]<d1[i] ) return 1;
			return 0;
			break;
		default:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				k = (nd_epw-1)*nd_bpe;
				for ( j = 0; j < nd_epw; j++, k -= nd_bpe )
					if ( d1[ind++]+((u2>>k)&nd_mask0) > nd_mask0 ) return 1;
			}
			return 0;
			break;
	}
}

int ndl_check_bound2_direct(unsigned int *d1,unsigned int *d2)
{
	unsigned int u2;
	int i,j,ind,k;

	ind = 0;
	switch ( nd_bpe ) {
		case 4:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>28)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>24)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>20)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>16)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>12)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>8)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+((u2>>4)&0xf) >= 0x10 ) return 1;
				if ( d1[ind++]+(u2&0xf) >= 0x10 ) return 1;
			}
			return 0;
			break;
		case 6:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>24)&0x3f) >= 0x40 ) return 1;
				if ( d1[ind++]+((u2>>18)&0x3f) >= 0x40 ) return 1;
				if ( d1[ind++]+((u2>>12)&0x3f) >= 0x40 ) return 1;
				if ( d1[ind++]+((u2>>6)&0x3f) >= 0x40 ) return 1;
				if ( d1[ind++]+(u2&0x3f) >= 0x40 ) return 1;
			}
			return 0;
			break;
		case 8:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>24)&0xff) >= 0x100 ) return 1;
				if ( d1[ind++]+((u2>>16)&0xff) >= 0x100 ) return 1;
				if ( d1[ind++]+((u2>>8)&0xff) >= 0x100 ) return 1;
				if ( d1[ind++]+(u2&0xff) >= 0x100 ) return 1;
			}
			return 0;
			break;
		case 16:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>16)&0xffff) > 0x10000 ) return 1;
				if ( d1[ind++]+(u2&0xffff) > 0x10000 ) return 1;
			}
			return 0;
			break;
		case 32:
			for ( i = nd_exporigin; i < nd_wpd; i++ )
				if ( d1[i]+d2[i]<d1[i] ) return 1;
			return 0;
			break;
		default:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				k = (nd_epw-1)*nd_bpe;
				for ( j = 0; j < nd_epw; j++, k -= nd_bpe )
					if ( d1[ind++]+((u2>>k)&nd_mask0) > nd_mask0 ) return 1;
			}
			return 0;
			break;
	}
}

INLINE int ndl_hash_value(unsigned int *d)
{
	int i;
	int r;

	r = 0;
	for ( i = 0; i < nd_wpd; i++ )	
		r = ((r<<16)+d[i])%REDTAB_LEN;
	return r;
}

INLINE int nd_find_reducer(ND g)
{
	RHist r;
	unsigned int *dg;
	int d,k,i;

	dg = HDL(g);
#if 1
	d = ndl_hash_value(HDL(g));
	for ( r = nd_red[d], k = 0; r; r = NEXT(r), k++ ) {
		if ( ndl_equal(dg,DL(r)) ) {
			if ( k > 0 ) nd_notfirst++;
			nd_found++;
			return r->index;
		}
	}
#endif
	if ( Reverse )
		for ( i = nd_psn-1; i >= 0; i-- ) {
			r = nd_psh[i];
			if ( ndl_reducible(dg,DL(r)) ) {
				nd_create++;
				nd_append_red(dg,i);
				return i;
			}
		}
	else
		for ( i = 0; i < nd_psn; i++ ) {
			r = nd_psh[i];
			if ( ndl_reducible(dg,DL(r)) ) {
				nd_create++;
				nd_append_red(dg,i);
				return i;
			}
		}
	return -1;
}

INLINE int nd_find_reducer_direct(ND g,NDV *ps,int len)
{
	NDV r;
	RHist s;
	int d,k,i;

	if ( Reverse )
		for ( i = len-1; i >= 0; i-- ) {
			r = ps[i];
			if ( ndl_reducible(HDL(g),HDL(r)) )
				return i;
		}
	else
		for ( i = 0; i < len; i++ ) {
			r = ps[i];
			if ( ndl_reducible(HDL(g),HDL(r)) )
				return i;
		}
	return -1;
}

ND nd_add(int mod,ND p1,ND p2)
{
	int n,c;
	int t,can,td1,td2;
	ND r;
	NM m1,m2,mr0,mr,s;

	if ( !p1 ) return p2;
	else if ( !p2 ) return p1;
	else if ( !mod ) return nd_add_q(p1,p2);
	else {
		can = 0;
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			c = DL_COMPARE(DL(m1),DL(m2));
			switch ( c ) {
				case 0:
					t = ((CM(m1))+(CM(m2))) - mod;
					if ( t < 0 ) t += mod;
					s = m1; m1 = NEXT(m1);
					if ( t ) {
						can++; NEXTNM2(mr0,mr,s); CM(mr) = (t);
					} else {
						can += 2; FREENM(s);
					}
					s = m2; m2 = NEXT(m2); FREENM(s);
					break;
				case 1:
					s = m1; m1 = NEXT(m1); NEXTNM2(mr0,mr,s);
					break;
				case -1:
					s = m2; m2 = NEXT(m2); NEXTNM2(mr0,mr,s);
					break;
			}
		}
		if ( !mr0 )
			if ( m1 ) mr0 = m1;
			else if ( m2 ) mr0 = m2;
			else return 0;
		else if ( m1 ) NEXT(mr) = m1;
		else if ( m2 ) NEXT(mr) = m2;
		else NEXT(mr) = 0;
		BDY(p1) = mr0;
		SG(p1) = MAX(SG(p1),SG(p2));
		LEN(p1) = LEN(p1)+LEN(p2)-can;
		FREEND(p2);
		return p1;
	}
}

ND nd_add_q(ND p1,ND p2)
{
	int n,c,can;
	ND r;
	NM m1,m2,mr0,mr,s;
	Q t;

	if ( !p1 ) return p2;
	else if ( !p2 ) return p1;
	else {
		can = 0;
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			c = DL_COMPARE(DL(m1),DL(m2));
			switch ( c ) {
				case 0:
					addq(CQ(m1),CQ(m2),&t);
					s = m1; m1 = NEXT(m1);
					if ( t ) {
						can++; NEXTNM2(mr0,mr,s); CQ(mr) = (t);
					} else {
						can += 2; FREENM(s);
					}
					s = m2; m2 = NEXT(m2); FREENM(s);
					break;
				case 1:
					s = m1; m1 = NEXT(m1); NEXTNM2(mr0,mr,s);
					break;
				case -1:
					s = m2; m2 = NEXT(m2); NEXTNM2(mr0,mr,s);
					break;
			}
		}
		if ( !mr0 )
			if ( m1 ) mr0 = m1;
			else if ( m2 ) mr0 = m2;
			else return 0;
		else if ( m1 ) NEXT(mr) = m1;
		else if ( m2 ) NEXT(mr) = m2;
		else NEXT(mr) = 0;
		BDY(p1) = mr0;
		SG(p1) = MAX(SG(p1),SG(p2));
		LEN(p1) = LEN(p1)+LEN(p2)-can;
		FREEND(p2);
		return p1;
	}
}

/* ret=1 : success, ret=0 : overflow */
int nd_nf(int mod,ND g,NDV *ps,int full,ND *rp)
{
	ND d;
	NM m,mrd,tail;
	NM mul;
	int n,sugar,psugar,sugar0,stat,index;
	int c,c1,c2,dummy;
	RHist h;
	NDV p,red;
	Q cg,cred,gcd;
	double hmag;

	if ( !g ) {
		*rp = 0;
		return 1;
	}
	if ( !mod ) hmag = ((double)p_mag((P)HCQ(g)))*nd_scale;

	sugar0 = sugar = SG(g);
	n = NV(g);
	mul = (NM)ALLOCA(sizeof(struct oNM)+(nd_wpd-1)*sizeof(unsigned int));
	for ( d = 0; g; ) {
		index = nd_find_reducer(g);
		if ( index >= 0 ) {
			h = nd_psh[index];
			ndl_sub(HDL(g),DL(h),DL(mul));
			if ( ndl_check_bound2(index,DL(mul)) ) {
				nd_free(g); nd_free(d);
				return 0;
			}
			p = ps[index];
			if ( mod ) {
				c1 = invm(HCM(p),mod); c2 = mod-HCM(g);
				DMAR(c1,c2,0,mod,c); CM(mul) = c;
			} else {
				igcd_cofactor(HCQ(g),HCQ(p),&gcd,&cg,&cred);
				chsgnq(cg,&CQ(mul));
				nd_mul_c_q(d,cred); nd_mul_c_q(g,cred);
			}
			g = nd_add(mod,g,ndv_mul_nm(mod,mul,p));
			sugar = MAX(sugar,SG(p)+TD(DL(mul)));
			if ( !mod && hmag && g && ((double)(p_mag((P)HCQ(g))) > hmag) ) {
				nd_removecont2(d,g);
				hmag = ((double)p_mag((P)HCQ(g)))*nd_scale;
			}
		} else if ( !full ) {
			*rp = g;
			return 1;
		} else {
			m = BDY(g); 
			if ( NEXT(m) ) {
				BDY(g) = NEXT(m); NEXT(m) = 0; LEN(g)--;
			} else {
				FREEND(g); g = 0;
			}
			if ( d ) {
				NEXT(tail)=m; tail=m; LEN(d)++;
			} else {
				MKND(n,m,1,d); tail = BDY(d);
			}
		}
	}
	if ( d ) SG(d) = sugar;
	*rp = d;
	return 1;
}

int nd_nf_pbucket(int mod,ND g,NDV *ps,int full,ND *rp)
{
	int hindex,index;
	NDV p;
	ND u,d,red;
	NODE l;
	NM mul,m,mrd,tail;
	int sugar,psugar,n,h_reducible;
	PGeoBucket bucket;
	int c,c1,c2;
	Q cg,cred,gcd,zzz;
	RHist h;
	double hmag,gmag;

	if ( !g ) {
		*rp = 0;
		return 1;
	}
	sugar = SG(g);
	n = NV(g);
	if ( !mod ) hmag = ((double)p_mag((P)HCQ(g)))*nd_scale;
	bucket = create_pbucket();
	add_pbucket(mod,bucket,g);
	d = 0;
	mul = (NM)ALLOCA(sizeof(struct oNM)+(nd_wpd-1)*sizeof(unsigned int));
	while ( 1 ) {
		hindex = mod?head_pbucket(mod,bucket):head_pbucket_q(bucket);
		if ( hindex < 0 ) {
			if ( d ) SG(d) = sugar;
			*rp = d;
			return 1;
		}
		g = bucket->body[hindex];
		index = nd_find_reducer(g);
		if ( index >= 0 ) {
			h = nd_psh[index];
			ndl_sub(HDL(g),DL(h),DL(mul));
			if ( ndl_check_bound2(index,DL(mul)) ) {
				nd_free(d);
				free_pbucket(bucket);
				*rp = 0;
				return 0;
			}
			p = ps[index];
 			if ( mod ) {
				c1 = invm(HCM(p),mod); c2 = mod-HCM(g);
				DMAR(c1,c2,0,mod,c); CM(mul) = c;
			} else {
				igcd_cofactor(HCQ(g),HCQ(p),&gcd,&cg,&cred);
				chsgnq(cg,&CQ(mul));
				nd_mul_c_q(d,cred);
				mulq_pbucket(bucket,cred);
				g = bucket->body[hindex];
				gmag = (double)p_mag((P)HCQ(g));
			}
			red = ndv_mul_nm(mod,mul,p);
			bucket->body[hindex] = nd_remove_head(g);
			red = nd_remove_head(red);
			add_pbucket(mod,bucket,red);
			psugar = SG(p)+TD(DL(mul));
			sugar = MAX(sugar,psugar);
			if ( !mod && hmag && (gmag > hmag) ) {
				g = normalize_pbucket(mod,bucket);
				if ( !g ) {
					if ( d ) SG(d) = sugar;
					*rp = d;
					return 1;
				}
				nd_removecont2(d,g);
				hmag = ((double)p_mag((P)HCQ(g)))*nd_scale;
				add_pbucket(mod,bucket,g);
			}
		} else if ( !full ) {
			g = normalize_pbucket(mod,bucket);
			if ( g ) SG(g) = sugar;
			*rp = g;
			return 1;
		} else {
			m = BDY(g); 
			if ( NEXT(m) ) {
				BDY(g) = NEXT(m); NEXT(m) = 0; LEN(g)--;
			} else {
				FREEND(g); g = 0;
			}
			bucket->body[hindex] = g;
			NEXT(m) = 0;
			if ( d ) {
				NEXT(tail)=m; tail=m; LEN(d)++;
			} else {
				MKND(n,m,1,d); tail = BDY(d);
			}
		}
	}
}

int nd_nf_direct(int mod,ND g,BaseSet base,int full,ND *rp)
{
	ND d;
	NM m,mrd,tail;
	NM mul;
	NDV *ps;
	int n,sugar,psugar,sugar0,stat,index,len;
	int c,c1,c2;
	unsigned int **bound;
	RHist h;
	NDV p,red;
	Q cg,cred,gcd;
	double hmag;

	if ( !g ) {
		*rp = 0;
		return 1;
	}
#if 0
	if ( !mod )
		hmag = ((double)p_mag((P)HCQ(g)))*nd_scale;
#else
	/* XXX */
	hmag = 0;
#endif

	ps = base->ps;
	bound = base->bound;
	len = base->len;
	sugar0 = sugar = SG(g);
	n = NV(g);
	mul = (NM)ALLOCA(sizeof(struct oNM)+(nd_wpd-1)*sizeof(unsigned int));
	for ( d = 0; g; ) {
		index = nd_find_reducer_direct(g,ps,len);
		if ( index >= 0 ) {
			p = ps[index];
			ndl_sub(HDL(g),HDL(p),DL(mul));
			if ( ndl_check_bound2_direct(bound[index],DL(mul)) ) {
				nd_free(g); nd_free(d);
				return 0;
			}
			if ( mod ) {
				c1 = invm(HCM(p),mod); c2 = mod-HCM(g);
				DMAR(c1,c2,0,mod,c); CM(mul) = c;
			} else {
				igcd_cofactor(HCQ(g),HCQ(p),&gcd,&cg,&cred);
				chsgnq(cg,&CQ(mul));
				nd_mul_c_q(d,cred); nd_mul_c_q(g,cred);
			}
			g = nd_add(mod,g,ndv_mul_nm(mod,mul,p));
			sugar = MAX(sugar,SG(p)+TD(DL(mul)));
			if ( !mod && hmag && g && ((double)(p_mag((P)HCQ(g))) > hmag) ) {
				nd_removecont2(d,g);
				hmag = ((double)p_mag((P)HCQ(g)))*nd_scale;
			}
		} else if ( !full ) {
			*rp = g;
			return 1;
		} else {
			m = BDY(g); 
			if ( NEXT(m) ) {
				BDY(g) = NEXT(m); NEXT(m) = 0; LEN(g)--;
			} else {
				FREEND(g); g = 0;
			}
			if ( d ) {
				NEXT(tail)=m; tail=m; LEN(d)++;
			} else {
				MKND(n,m,1,d); tail = BDY(d);
			}
		}
	}
	if ( d ) SG(d) = sugar;
	*rp = d;
	return 1;
}

/* input : list of DP, cand : list of DP */

int nd_check_candidate(NODE input,NODE cand)
{
	int n,i,stat;
	ND nf,d;
	NODE t,s;

#if 0
	for ( t = 0; cand; cand = NEXT(cand) ) {
		MKNODE(s,BDY(cand),t); t = s;
	}
	cand = t;
#endif

	nd_setup(0,0,cand);
	n = length(cand);

	/* membercheck : list is a subset of Id(cand) ? */
	for ( t = input; t; t = NEXT(t) ) {
again:
		d = dptond(0,(DP)BDY(t));
		stat = nd_nf(0,d,nd_ps,0,&nf);
		if ( !stat ) {
			nd_reconstruct(0,0,0);
			goto again;
		} else if ( nf ) return 0;
		printf("."); fflush(stdout);
	}
	printf("\n");
	/* gbcheck : cand is a GB of Id(cand) ? */
	if ( !nd_gb(0,1) ) return 0;
	/* XXX */
	return 1;
}

ND nd_remove_head(ND p)
{
	NM m;

	m = BDY(p);
	if ( !NEXT(m) ) {
		FREEND(p); p = 0;
	} else {
		BDY(p) = NEXT(m); LEN(p)--;
	}
	FREENM(m);
	return p;
}

PGeoBucket create_pbucket()
{
    PGeoBucket g;
	 
	g = CALLOC(1,sizeof(struct oPGeoBucket));
	g->m = -1;
	return g;
}

void free_pbucket(PGeoBucket b) {
	int i;

	for ( i = 0; i <= b->m; i++ )
		if ( b->body[i] ) {
			nd_free(b->body[i]);
			b->body[i] = 0;
		}
	GC_free(b);
}

void add_pbucket(int mod,PGeoBucket g,ND d)
{
	int l,i,k,m;

	if ( !d )
		return;
	l = LEN(d);
	for ( k = 0, m = 1; l > m; k++, m <<= 1 );
	/* 2^(k-1) < l <= 2^k (=m) */
	d = nd_add(mod,g->body[k],d);
	for ( ; d && LEN(d) > m; k++, m <<= 1 ) {
		g->body[k] = 0;
		d = nd_add(mod,g->body[k+1],d);
	}
	g->body[k] = d;
	g->m = MAX(g->m,k);
}

void mulq_pbucket(PGeoBucket g,Q c)
{
	int k;

	for ( k = 0; k <= g->m; k++ )
		nd_mul_c_q(g->body[k],c);
}

int head_pbucket(int mod,PGeoBucket g)
{
	int j,i,c,k,nv,sum;
	unsigned int *di,*dj;
	ND gi,gj;

	k = g->m;
	while ( 1 ) {
		j = -1;
		for ( i = 0; i <= k; i++ ) {
			if ( !(gi = g->body[i]) )
				continue;
			if ( j < 0 ) {
				j = i;
				gj = g->body[j];
				dj = HDL(gj);
				sum = HCM(gj);
			} else {
				c = DL_COMPARE(HDL(gi),dj);
				if ( c > 0 ) {
					if ( sum ) HCM(gj) = sum;
					else g->body[j] = nd_remove_head(gj);
					j = i;
					gj = g->body[j];
					dj = HDL(gj);
					sum = HCM(gj);
				} else if ( c == 0 ) {
					sum = sum+HCM(gi)-mod;
					if ( sum < 0 ) sum += mod;
					g->body[i] = nd_remove_head(gi);
				}
			}
		}
		if ( j < 0 ) return -1;
		else if ( sum ) {
			HCM(gj) = sum;
			return j;
		} else
			g->body[j] = nd_remove_head(gj);
	}
}

int head_pbucket_q(PGeoBucket g)
{
	int j,i,c,k,nv;
	Q sum,t;
	ND gi,gj;

	k = g->m;
	while ( 1 ) {
		j = -1;
		for ( i = 0; i <= k; i++ ) {
			if ( !(gi = g->body[i]) ) continue;
			if ( j < 0 ) {
				j = i;
				gj = g->body[j];
				sum = HCQ(gj);
			} else {
				nv = NV(gi);
				c = DL_COMPARE(HDL(gi),HDL(gj));
				if ( c > 0 ) {
					if ( sum ) HCQ(gj) = sum;
					else g->body[j] = nd_remove_head(gj);
					j = i;
					gj = g->body[j];
					sum = HCQ(gj);
				} else if ( c == 0 ) {
					addq(sum,HCQ(gi),&t);
					sum = t;
					g->body[i] = nd_remove_head(gi);
				}
			}
		}
		if ( j < 0 ) return -1;
		else if ( sum ) {
			HCQ(gj) = sum;
			return j;
		} else
			g->body[j] = nd_remove_head(gj);
	}
}

ND normalize_pbucket(int mod,PGeoBucket g)
{
	int i;
	ND r,t;

	r = 0;
	for ( i = 0; i <= g->m; i++ ) {
		r = nd_add(mod,r,g->body[i]);
		g->body[i] = 0;
	}
	g->m = -1;
	return r;
}

/* return value = 0 => input is not a GB */

NODE nd_gb(int m,int checkonly)
{
	int i,nh,sugar,stat;
	NODE r,g,t;
	ND_pairs d;
	ND_pairs l;
	ND h,nf;

	g = 0; d = 0;
	for ( i = 0; i < nd_psn; i++ ) {
		d = update_pairs(d,g,i);
		g = update_base(g,i);
	}
	sugar = 0;
	while ( d ) {
again:
		l = nd_minp(d,&d);
		if ( SG(l) != sugar ) {
			sugar = SG(l);
			fprintf(asir_out,"%d",sugar);
		}
		stat = nd_sp(m,0,l,&h);
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(m,0,d);
			goto again;
		}
#if USE_GEOBUCKET
		stat = m?nd_nf_pbucket(m,h,nd_ps,!Top,&nf):nd_nf(m,h,nd_ps,!Top,&nf);
#else
		stat = nd_nf(m,h,nd_ps,!Top,&nf);
#endif
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(m,0,d);
			goto again;
		} else if ( nf ) {
			if ( checkonly ) return 0;
			printf("+"); fflush(stdout);
			nh = nd_newps(m,nf,0);
			d = update_pairs(d,g,nh);
			g = update_base(g,nh);
			FREENDP(l);
		} else {
			printf("."); fflush(stdout);
			FREENDP(l);
		}
	}
	for ( t = g; t; t = NEXT(t) ) BDY(t) = (pointer)nd_ps[(int)BDY(t)];
	return g;
}

NODE nd_gb_trace(int m)
{
	int i,nh,sugar,stat;
	NODE r,g,t;
	ND_pairs d;
	ND_pairs l;
	ND h,nf,nfq;

	g = 0; d = 0;
	for ( i = 0; i < nd_psn; i++ ) {
		d = update_pairs(d,g,i);
		g = update_base(g,i);
	}
	sugar = 0;
	while ( d ) {
again:
		l = nd_minp(d,&d);
		if ( SG(l) != sugar ) {
			sugar = SG(l);
			fprintf(asir_out,"%d",sugar);
		}
		stat = nd_sp(m,0,l,&h);
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(m,1,d);
			goto again;
		}
#if USE_GEOBUCKET
		stat = nd_nf_pbucket(m,h,nd_ps,!Top,&nf);
#else
		stat = nd_nf(m,h,nd_ps,!Top,&nf);
#endif
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(m,1,d);
			goto again;
		} else if ( nf ) {
			/* overflow does not occur */
			nd_sp(0,1,l,&h);
			nd_nf(0,h,nd_ps_trace,!Top,&nfq);
			if ( nfq ) {
				printf("+"); fflush(stdout);
				nh = nd_newps(m,nf,nfq);
				/* failure; m|HC(nfq) */
				if ( nh < 0 ) return 0;
				d = update_pairs(d,g,nh);
				g = update_base(g,nh);
			} else {
				printf("*"); fflush(stdout);
			}
		} else {
			printf("."); fflush(stdout);
		}
		FREENDP(l);
	}
	for ( t = g; t; t = NEXT(t) )
		BDY(t) = (pointer)nd_ps_trace[(int)BDY(t)];
	return g;
}

int ndv_compare(NDV *p1,NDV *p2)
{
	return DL_COMPARE(HDL(*p1),HDL(*p2));
}

int ndv_compare_rev(NDV *p1,NDV *p2)
{
	return -DL_COMPARE(HDL(*p1),HDL(*p2));
}

NODE nd_reduceall(int m,NODE f)
{
	int i,j,n,stat;
	NDV *w,*ps;
	ND nf,g;
	NODE t,a0,a;
	struct oBaseSet base;
	unsigned int **bound;

	for ( n = 0, t = f; t; t = NEXT(t), n++ );
	ps = (NDV *)ALLOCA(n*sizeof(NDV));
	bound = (unsigned int **)ALLOCA(n*sizeof(unsigned int *));
	for ( i = 0, t = f; i < n; i++, t = NEXT(t) ) ps[i] = (NDV)BDY(t);
	qsort(ps,n,sizeof(NDV),(int (*)(const void *,const void *))ndv_compare);
	for ( i = 0; i < n; i++ ) bound[i] = ndv_compute_bound(ps[i]);
	base.ps = (NDV *)ALLOCA((n-1)*sizeof(NDV));
	base.bound = (unsigned int **)ALLOCA((n-1)*sizeof(unsigned int *));
	base.len = n-1;
	i = 0;
	while ( i < n ) {
		for ( j = 0; j < i; j++ ) {
			base.ps[j] = ps[j]; base.bound[j] = bound[j];
		}
		for ( j = i+1; j < n; j++ ) {
			base.ps[j-1] = ps[j]; base.bound[j-1] = bound[j];
		}
		g = ndvtond(m,ps[i]);
		stat = nd_nf_direct(m,g,&base,1,&nf);
		if ( !stat )
			nd_reconstruct_direct(m,ps,n);
		else if ( !nf ) {
			printf("."); fflush(stdout);
			ndv_free(ps[i]);
			for ( j = i+1; j < n; j++ ) {
				ps[j-1] = ps[j]; bound[j-1] = bound[j];
			}
			n--;
			base.len = n-1;
		} else {
			printf("."); fflush(stdout);
			ndv_free(ps[i]);
			nd_removecont(m,nf);
			ps[i] = ndtondv(m,nf);
			bound[i] = ndv_compute_bound(ps[i]);
			nd_free(nf);
			i++;
		}
	}
	printf("\n");
	for ( a0 = 0, i = 0; i < n; i++ ) {
		NEXTNODE(a0,a);
		BDY(a) = (pointer)ps[i];
	}
	NEXT(a) = 0;
	return a0;
}

ND_pairs update_pairs( ND_pairs d, NODE /* of index */ g, int t)
{
	ND_pairs d1,nd,cur,head,prev,remove;

	if ( !g ) return d;
	d = crit_B(d,t);
	d1 = nd_newpairs(g,t);
	d1 = crit_M(d1);
	d1 = crit_F(d1);
	if ( do_weyl )
		head = d1;
	else {
		prev = 0; cur = head = d1;
		while ( cur ) {
			if ( crit_2( cur->i1,cur->i2 ) ) {
				remove = cur;
				if ( !prev ) head = cur = NEXT(cur);
				else cur = NEXT(prev) = NEXT(cur);
				FREENDP(remove);
			} else {
				prev = cur; cur = NEXT(cur);
			}
		}
	}
	if ( !d )
		return head;
	else {
		nd = d;
		while ( NEXT(nd) ) nd = NEXT(nd);
		NEXT(nd) = head;
		return d;
	}
}

ND_pairs nd_newpairs( NODE g, int t )
{
	NODE h;
	unsigned int *dl;
	int ts,s;
	ND_pairs r,r0;

	dl = DL(nd_psh[t]);
	ts = SG(nd_psh[t]) - TD(dl);
	for ( r0 = 0, h = g; h; h = NEXT(h) ) {
		NEXTND_pairs(r0,r);
		r->i1 = (int)BDY(h);
		r->i2 = t;
		ndl_lcm(DL(nd_psh[r->i1]),dl,r->lcm);
		s = SG(nd_psh[r->i1])-TD(DL(nd_psh[r->i1]));
		SG(r) = MAX(s,ts) + TD(LCM(r));
	}
	NEXT(r) = 0;
	return r0;
}

ND_pairs crit_B( ND_pairs d, int s )
{
	ND_pairs cur,head,prev,remove;
	unsigned int *t,*tl,*lcm;
	int td,tdl;

	if ( !d ) return 0;
	t = DL(nd_psh[s]);
	prev = 0;
	head = cur = d;
	lcm = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
	while ( cur ) {
		tl = cur->lcm;
		if ( ndl_reducible(tl,t)
			&& (ndl_lcm(DL(nd_psh[cur->i1]),t,lcm),!ndl_equal(lcm,tl))
			&& (ndl_lcm(DL(nd_psh[cur->i2]),t,lcm),!ndl_equal(lcm,tl)) ) {
			remove = cur;
			if ( !prev ) {
				head = cur = NEXT(cur);
			} else {
				cur = NEXT(prev) = NEXT(cur);
			}
			FREENDP(remove);
		} else {
			prev = cur; cur = NEXT(cur);
		}
	}
	return head;
}

ND_pairs crit_M( ND_pairs d1 )
{
	ND_pairs e,d2,d3,dd,p;
	unsigned int *id,*jd;

	for ( dd = 0, e = d1; e; e = d3 ) {
		if ( !(d2 = NEXT(e)) ) {
			NEXT(e) = dd;
			return e;
		}
		id = LCM(e);
		for ( d3 = 0; d2; d2 = p ) {
			p = NEXT(d2);
			jd = LCM(d2);
			if ( ndl_equal(jd,id) )
				;
			else if ( TD(jd) > TD(id) )
				if ( ndl_reducible(jd,id) ) continue;
				else ;
			else if ( ndl_reducible(id,jd) ) goto delit;
			NEXT(d2) = d3;
			d3 = d2;
		}
		NEXT(e) = dd;
		dd = e;
		continue;
		/**/
	delit:	NEXT(d2) = d3;
		d3 = d2;
		for ( ; p; p = d2 ) {
			d2 = NEXT(p);
			NEXT(p) = d3;
			d3 = p;
		}
		FREENDP(e);
	}
	return dd;
}

ND_pairs crit_F( ND_pairs d1 )
{
	ND_pairs rest, head,remove;
	ND_pairs last, p, r, w;
	int s;

	for ( head = last = 0, p = d1; NEXT(p); ) {
		r = w = equivalent_pairs(p,&rest);
		s = SG(r);
		w = NEXT(w);
		while ( w ) {
			if ( crit_2(w->i1,w->i2) ) {
				r = w;
				w = NEXT(w);
				while ( w ) {
					remove = w;
					w = NEXT(w);
					FREENDP(remove);
				}
				break;
			} else if ( SG(w) < s ) {
				FREENDP(r);
				r = w;
				s = SG(r);
				w = NEXT(w);
			} else {
				remove = w;
				w = NEXT(w);
				FREENDP(remove);
			}
		}
		if ( last ) NEXT(last) = r;
		else head = r;
		NEXT(last = r) = 0;
		p = rest;
		if ( !p ) return head;
	}
	if ( !last ) return p;
	NEXT(last) = p;
	return head;
}

int crit_2( int dp1, int dp2 )
{
	return ndl_disjoint(DL(nd_psh[dp1]),DL(nd_psh[dp2]));
}

ND_pairs equivalent_pairs( ND_pairs d1, ND_pairs *prest )
{
	ND_pairs w,p,r,s;
	unsigned int *d;

	w = d1;
	d = LCM(w);
	s = NEXT(w);
	NEXT(w) = 0;
	for ( r = 0; s; s = p ) {
		p = NEXT(s);
		if ( ndl_equal(d,LCM(s)) ) {
			NEXT(s) = w; w = s;
		} else {
			NEXT(s) = r; r = s;
		}
	}
	*prest = r;
	return w;
}

NODE update_base(NODE nd,int ndp)
{
	unsigned int *dl, *dln;
	NODE last, p, head;

	dl = DL(nd_psh[ndp]);
	for ( head = last = 0, p = nd; p; ) {
		dln = DL(nd_psh[(int)BDY(p)]);
		if ( ndl_reducible( dln, dl ) ) {
			p = NEXT(p);
			if ( last ) NEXT(last) = p;
		} else {
			if ( !last ) head = p;
			p = NEXT(last = p);
		}
	}
	head = append_one(head,ndp);
	return head;
}

ND_pairs nd_minp( ND_pairs d, ND_pairs *prest )
{
	ND_pairs m,ml,p,l;
	unsigned int *lcm;
	int s,td,len,tlen,c,c1;

	if ( !(p = NEXT(m = d)) ) {
		*prest = p;
		NEXT(m) = 0;
		return m;
	}
	s = SG(m);
	for ( ml = 0, l = m; p; p = NEXT(l = p) )
		if ( (SG(p) < s) 
			|| ((SG(p) == s) && (DL_COMPARE(LCM(p),LCM(m)) < 0)) ) {
			ml = l; m = p; s = SG(m);
		}
	if ( !ml ) *prest = NEXT(m);
	else {
		NEXT(ml) = NEXT(m);
		*prest = d;
	}
	NEXT(m) = 0;
	return m;
}

int nd_newps(int mod,ND a,ND aq)
{
	int len;
	RHist r;
	NDV b;

	if ( aq ) {
		/* trace lifting */
		if ( !rem(NM(HCQ(aq)),mod) )
			return -1;
	}
	if ( nd_psn == nd_pslen ) {
		nd_pslen *= 2;
		nd_ps = (NDV *)REALLOC((char *)nd_ps,nd_pslen*sizeof(NDV));
		nd_ps_trace = (NDV *)REALLOC((char *)nd_ps_trace,nd_pslen*sizeof(NDV));
		nd_psh = (RHist *)REALLOC((char *)nd_psh,nd_pslen*sizeof(RHist));
		nd_bound = (unsigned int **)
			REALLOC((char *)nd_bound,nd_pslen*sizeof(unsigned int *));
	}
	NEWRHist(r); nd_psh[nd_psn] = r;
	nd_removecont(mod,a); nd_ps[nd_psn] = ndtondv(mod,a);
	if ( aq ) {
		nd_removecont(0,aq); nd_ps_trace[nd_psn] = ndtondv(0,aq);
		nd_bound[nd_psn] = ndv_compute_bound(nd_ps_trace[nd_psn]);
		SG(r) = SG(aq); ndl_copy(HDL(aq),DL(r));
		nd_free(a); nd_free(aq);
	} else {
		nd_bound[nd_psn] = ndv_compute_bound(nd_ps[nd_psn]);
		SG(r) = SG(a); ndl_copy(HDL(a),DL(r));
		nd_free(a);
	}
	return nd_psn++;
}

void nd_setup(int mod,int trace,NODE f)
{
	int i,j,td,len,max;
	NODE s,s0,f0;
	unsigned int *d;
	RHist r;
	NDV a;
	MP t;

	nd_found = 0; nd_notfirst = 0; nd_create = 0;

	nd_psn = length(f); nd_pslen = 2*nd_psn;
	nd_ps = (NDV *)MALLOC(nd_pslen*sizeof(NDV));
	nd_ps_trace = (NDV *)MALLOC(nd_pslen*sizeof(NDV));
	nd_psh = (RHist *)MALLOC(nd_pslen*sizeof(RHist));
	nd_bound = (unsigned int **)MALLOC(nd_pslen*sizeof(unsigned int *));

	if ( !nd_red )
		nd_red = (RHist *)MALLOC(REDTAB_LEN*sizeof(RHist));
	bzero(nd_red,REDTAB_LEN*sizeof(RHist));

	for ( max = 0, s = f; s; s = NEXT(s) )
		for ( t = BDY((DP)BDY(s)); t; t = NEXT(t) ) {
			d = t->dl->d;
			for ( j = 0; j < nd_nvar; j++ ) max = MAX(d[j],max);
		}

	if ( max < 2 ) nd_bpe = 2;
	else if ( max < 4 ) nd_bpe = 4;
	else if ( max < 64 ) nd_bpe = 6;
	else if ( max < 256 ) nd_bpe = 8;
	else if ( max < 65536 ) nd_bpe = 16;
	else nd_bpe = 32;

	nd_setup_parameters();
	nd_free_private_storage();
	for ( i = 0; i < nd_psn; i++, f = NEXT(f) ) {
		NEWRHist(r);
		a = dptondv(mod,(DP)BDY(f)); ndv_removecont(mod,a);
		SG(r) = HTD(a); ndl_copy(HDL(a),DL(r));
		nd_ps[i] = a;
		if ( trace ) {
			a = dptondv(0,(DP)BDY(f)); ndv_removecont(0,a);
			nd_ps_trace[i] = a;
		}
		nd_bound[i] = ndv_compute_bound(a);
		nd_psh[i] = r;
	}
}

void nd_gr(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp)
{
	struct order_spec ord1;
	VL fv,vv,vc;
	NODE fd,fd0,r,r0,t,x,s,xx;
	DP a,b,c;

	get_vars((Obj)f,&fv); pltovl(v,&vv);
	nd_nvar = length(vv);
	nd_init_ord(ord);
	/* XXX for DP */
	initd(ord);
	for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
		ptod(CO,vv,(P)BDY(t),&b);
		NEXTNODE(fd0,fd); BDY(fd) = (pointer)b;
	}
	if ( fd0 ) NEXT(fd) = 0;
	nd_setup(m,0,fd0);
	x = nd_gb(m,0);
	fprintf(asir_out,"found=%d,notfirst=%d,create=%d\n",
		nd_found,nd_notfirst,nd_create);
	x = nd_reducebase(x);
	x = nd_reduceall(m,x);
	for ( r0 = 0, t = x; t; t = NEXT(t) ) {
		NEXTNODE(r0,r); 
		if ( m ) {
			a = ndvtodp(m,BDY(t));
			_dtop_mod(CO,vv,a,(P *)&BDY(r));
		} else {
			a = ndvtodp(m,BDY(t));
			dtop(CO,vv,a,(P *)&BDY(r));
		}
	}
	if ( r0 ) NEXT(r) = 0;
	MKLIST(*rp,r0);
}

void nd_gr_trace(LIST f,LIST v,int trace,int homo,struct order_spec *ord,LIST *rp)
{
	struct order_spec ord1;
	VL fv,vv,vc;
	NODE fd,fd0,in0,in,r,r0,t,s,cand;
	int m,nocheck,nvar,mindex;
	DP a,b,c,h;
	P p;

	get_vars((Obj)f,&fv); pltovl(v,&vv);
	nvar = length(vv);
	nocheck = 0;
	mindex = 0;

	/* setup modulus */
	if ( trace < 0 ) {
		trace = -trace;
		nocheck = 1;
	}
	m = trace > 1 ? trace : get_lprime(mindex);

	initd(ord);
	if ( homo ) {
		homogenize_order(ord,nd_nvar,&ord1);
		for ( fd0 = 0, in0 = 0, t = BDY(f); t; t = NEXT(t) ) {
			ptod(CO,vv,(P)BDY(t),&c);
			if ( c ) {
				dp_homo(c,&h); NEXTNODE(fd0,fd); BDY(fd) = (pointer)h;
				NEXTNODE(in0,in); BDY(in) = (pointer)c;
			}
		}
		if ( fd0 ) NEXT(fd) = 0;
		if ( in0 ) NEXT(in) = 0;
	} else {
		for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
			ptod(CO,vv,(P)BDY(t),&c);
			if ( c ) {
				NEXTNODE(fd0,fd); BDY(fd) = (pointer)c;
			}
		}
		if ( fd0 ) NEXT(fd) = 0;
		in0 = fd0;
	}
	while ( 1 ) {
		if ( homo ) {
			nd_init_ord(&ord1);
			initd(&ord1);
			nd_nvar = nvar+1;
		} else {
			nd_init_ord(ord);
			nd_nvar = nvar;
		}
		nd_setup(m,1,fd0);
		cand = nd_gb_trace(m);
		if ( !cand ) {
			/* failure */
			if ( trace > 1 ) {
				*rp = 0; return;
			} else
				m = get_lprime(++mindex);
			continue;
		}

		if ( homo ) {
			/* dehomogenization */
			for ( t = cand; t; t = NEXT(t) )
				ndv_dehomogenize((NDV)BDY(t),ord);
			nd_nvar = nvar;
			initd(ord);
			nd_init_ord(ord);
			nd_setup_parameters();
		}
		cand = nd_reducebase(cand);
		fprintf(asir_out,"found=%d,notfirst=%d,create=%d\n",
			nd_found,nd_notfirst,nd_create);
		cand = nd_reduceall(0,cand);
		initd(ord);
		for ( r0 = 0; cand; cand = NEXT(cand) ) {
			NEXTNODE(r0,r); 
			BDY(r) = (pointer)ndvtodp(0,(NDV)BDY(cand));
		}
		if ( r0 ) NEXT(r) = 0;
		cand = r0;
		if ( nocheck || nd_check_candidate(in0,cand) )
			/* success */
			break;
		else if ( trace > 1 ) {
			/* failure */
			*rp = 0; return;	
		} else
			/* try the next modulus */
			m = get_lprime(++mindex);
	}
	/* dp->p */
	for ( r = cand; r; r = NEXT(r) ) {
		dtop(CO,vv,BDY(r),&p);
		BDY(r) = (pointer)p;
	}
	MKLIST(*rp,cand);
}

void dltondl(int n,DL dl,unsigned int *r)
{
	unsigned int *d;
	int i,j,l,s,ord_l;
	struct order_pair *op;

	d = dl->d;
	for ( i = 0; i < nd_wpd; i++ ) r[i] = 0;
	if ( nd_blockmask ) {
		l = nd_blockmask->n;
		op = nd_blockmask->order_pair;
		for ( j = 0, s = 0; j < l; j++ ) {
			ord_l = op[j].length;
			for ( i = 0; i < ord_l; i++, s++ ) PUT_EXP(r,s,d[s]);
		}
		TD(r) = ndl_weight(r);
		for ( j = 0; j < l; j++ )
			r[j+1] = ndl_weight_mask(r,j);
	} else {
		for ( i = 0; i < n; i++ ) PUT_EXP(r,i,d[i]);
		TD(r) = ndl_weight(r);
	}
}

DL ndltodl(int n,unsigned int *ndl)
{
	DL dl;
	int *d;
	int i,j,l,s,ord_l;
	struct order_pair *op;

	NEWDL(dl,n);
	dl->td = TD(ndl);
	d = dl->d;
	if ( nd_blockmask ) {
		l = nd_blockmask->n;
		op = nd_blockmask->order_pair;
		for ( j = 0, s = 0; j < l; j++ ) {
			ord_l = op[j].length;
			for ( i = 0; i < ord_l; i++, s++ ) d[s] = GET_EXP(ndl,s);
		}
	} else {
		for ( i = 0; i < n; i++ ) d[i] = GET_EXP(ndl,i);
	}
	return dl;
}

ND dptond(int mod,DP p)
{
	ND d;
	NM m0,m;
	MP t;
	int n,l;

	if ( !p )
		return 0;
	n = NV(p);
	m0 = 0;
	for ( t = BDY(p), l = 0; t; t = NEXT(t), l++ ) {
		NEXTNM(m0,m);
		if ( mod ) CM(m) = ITOS(C(t));
		else CQ(m) = (Q)C(t);
		dltondl(n,DL(t),DL(m));
	}
	NEXT(m) = 0;
	MKND(n,m0,l,d);
	SG(d) = SG(p);
	return d;
}

DP ndtodp(int mod,ND p)
{
	DP d;
	MP m0,m;
	NM t;
	int n;

	if ( !p )
		return 0;
	n = NV(p);
	m0 = 0;
	for ( t = BDY(p); t; t = NEXT(t) ) {
		NEXTMP(m0,m);
		if ( mod ) C(m) = STOI(CM(t));
		else C(m) = (P)CQ(t);
		DL(m) = ndltodl(n,DL(t));
	}
	NEXT(m) = 0;
	MKDP(n,m0,d);
	SG(d) = SG(p);
	return d;
}

void ndl_print(unsigned int *dl)
{
	int n;
	int i,j,l,ord_l,s,s0;
	struct order_pair *op;

	n = nd_nvar;
	printf("<<");
	if ( nd_blockmask ) {
		l = nd_blockmask->n;
		op = nd_blockmask->order_pair;
		for ( j = 0, s = s0 = 0; j < l; j++ ) {
			ord_l = op[j].length;
			for ( i = 0; i < ord_l; i++, s++ )
				printf(s==n-1?"%d":"%d,",GET_EXP(dl,s));
		}
	} else {
		for ( i = 0; i < n; i++ ) printf(i==n-1?"%d":"%d,",GET_EXP(dl,i));
	}
	printf(">>");
}

void nd_print(ND p)
{
	NM m;

	if ( !p )
		printf("0\n");
	else {
		for ( m = BDY(p); m; m = NEXT(m) ) {
			printf("+%d*",CM(m));
			ndl_print(DL(m));
		}
		printf("\n");
	}
}

void nd_print_q(ND p)
{
	NM m;

	if ( !p )
		printf("0\n");
	else {
		for ( m = BDY(p); m; m = NEXT(m) ) {
			printf("+");
			printexpr(CO,CQ(m));
			printf("*");
			ndl_print(DL(m));
		}
		printf("\n");
	}
}

void ndp_print(ND_pairs d)
{
	ND_pairs t;

	for ( t = d; t; t = NEXT(t) ) printf("%d,%d ",t->i1,t->i2);
	printf("\n");
}

void nd_removecont(int mod,ND p)
{
	int i,n;
	Q *w;
	Q dvr,t;
	NM m;
	struct oVECT v;
	N q,r;

	if ( mod ) nd_mul_c(mod,p,invm(HCM(p),mod));
	else {
		for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
		w = (Q *)ALLOCA(n*sizeof(Q));
		v.len = n;
		v.body = (pointer *)w;
		for ( m = BDY(p), i = 0; i < n; m = NEXT(m), i++ ) w[i] = CQ(m);
		removecont_array(w,n);
		for ( m = BDY(p), i = 0; i < n; m = NEXT(m), i++ ) CQ(m) = w[i];	
	}
}

void nd_removecont2(ND p1,ND p2)
{
	int i,n1,n2,n;
	Q *w;
	Q dvr,t;
	NM m;
	struct oVECT v;
	N q,r;

	if ( !p1 ) {
		nd_removecont(0,p2); return;
	} else if ( !p2 ) {
		nd_removecont(0,p1); return;
	}
	n1 = nd_length(p1);
	n2 = nd_length(p2);
	n = n1+n2;
	w = (Q *)ALLOCA(n*sizeof(Q));
	v.len = n;
	v.body = (pointer *)w;
	for ( m = BDY(p1), i = 0; i < n1; m = NEXT(m), i++ ) w[i] = CQ(m);
	for ( m = BDY(p2); i < n; m = NEXT(m), i++ ) w[i] = CQ(m);
	removecont_array(w,n);
	for ( m = BDY(p1), i = 0; i < n1; m = NEXT(m), i++ ) CQ(m) = w[i];	
	for ( m = BDY(p2); i < n; m = NEXT(m), i++ ) CQ(m) = w[i];	
}

void ndv_removecont(int mod,NDV p)
{
	int i,len;
	Q *w;
	Q dvr,t;
	NMV m;

	if ( mod )
		ndv_mul_c(mod,p,invm(HCM(p),mod));
	else {
		len = p->len;
		w = (Q *)ALLOCA(len*sizeof(Q));
		for ( m = BDY(p), i = 0; i < len; NMV_ADV(m), i++ ) w[i] = CQ(m);
		sortbynm(w,len);
		qltozl(w,len,&dvr);
		for ( m = BDY(p), i = 0; i < len; NMV_ADV(m), i++ ) {
			divq(CQ(m),dvr,&t); CQ(m) = t;
		}
	}
}

void ndv_dehomogenize(NDV p,struct order_spec *ord)
{
	int i,j,adj,len,newnvar,newwpd,newadv,newexporigin;
	Q *w;
	Q dvr,t;
	NMV m,r;
#define NEWADV(m) (m = (NMV)(((char *)m)+newadv))

	len = p->len;
	newnvar = nd_nvar-1;
	newexporigin = nd_get_exporigin(ord);
	newwpd = newnvar/nd_epw+(newnvar%nd_epw?1:0)+newexporigin;
	for ( m = BDY(p), i = 0; i < len; NMV_ADV(m), i++ )
		ndl_dehomogenize(DL(m));
	if ( newwpd != nd_wpd ) {
		newadv = sizeof(struct oNMV)+(newwpd-1)*sizeof(unsigned int);
		for ( m = r = BDY(p), i = 0; i < len; NMV_ADV(m), NEWADV(r), i++ ) {
			CQ(r) = CQ(m);
			for ( j = 0; j < newexporigin; j++ ) DL(r)[j] = DL(m)[j];
			adj = nd_exporigin-newexporigin;
			for ( ; j < newwpd; j++ ) DL(r)[j] = DL(m)[j+adj];
		}
	}
	NV(p)--;
}

void removecont_array(Q *c,int n)
{
	struct oVECT v;
	Q d0,d1,a,u,u1,gcd;
	int i;
	N qn,rn,gn;
	Q *q,*r;

	q = (Q *)ALLOCA(n*sizeof(Q));
	r = (Q *)ALLOCA(n*sizeof(Q));
	v.id = O_VECT; v.len = n; v.body = (pointer *)c;
	igcdv_estimate(&v,&d0);
	for ( i = 0; i < n; i++ ) {
		divn(NM(c[i]),NM(d0),&qn,&rn);
		NTOQ(qn,SGN(c[i])*SGN(d0),q[i]);
		NTOQ(rn,SGN(c[i]),r[i]);
	}
	for ( i = 0; i < n; i++ ) if ( r[i] ) break;
	if ( i < n ) {
		v.id = O_VECT; v.len = n; v.body = (pointer *)r;
		igcdv(&v,&d1);
		gcdn(NM(d0),NM(d1),&gn); NTOQ(gn,1,gcd);
		divsn(NM(d0),gn,&qn); NTOQ(qn,1,a);
		for ( i = 0; i < n; i++ ) {
			mulq(a,q[i],&u);
			if ( r[i] ) {
				divsn(NM(r[i]),gn,&qn); NTOQ(qn,SGN(r[i]),u1);
				addq(u,u1,&q[i]);
			} else
				q[i] = u;
		}
	}
	for ( i = 0; i < n; i++ ) c[i] = q[i];
}

void nd_mul_c(int mod,ND p,int mul)
{
	NM m;
	int c,c1;

	if ( !p ) return;
	for ( m = BDY(p); m; m = NEXT(m) ) {
		c1 = CM(m);
		DMAR(c1,mul,0,mod,c);
		CM(m) = c;
	}
}

void nd_mul_c_q(ND p,Q mul)
{
	NM m;
	Q c;

	if ( !p ) return;
	for ( m = BDY(p); m; m = NEXT(m) ) {
		mulq(CQ(m),mul,&c); CQ(m) = c;
	}
}

void nd_free(ND p)
{
	NM t,s;

	if ( !p ) return;
	t = BDY(p);
	while ( t ) {
		s = NEXT(t);
		FREENM(t);
		t = s;
	}
	FREEND(p);
}

void ndv_free(NDV p)
{
	GC_free(BDY(p));
}

void nd_append_red(unsigned int *d,int i)
{
	RHist m,m0;
	int h;

	NEWRHist(m);
	h = ndl_hash_value(d);
	m->index = i;
	ndl_copy(d,DL(m));
	NEXT(m) = nd_red[h];
	nd_red[h] = m;
}

unsigned int *ndv_compute_bound(NDV p)
{
	unsigned int *d1,*d2,*t;
	unsigned int u;
	int i,j,k,l,len,ind;
	NMV m;

	if ( !p )
		return 0;
	d1 = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
	d2 = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
	len = LEN(p);
	m = BDY(p); ndl_copy(DL(m),d1); NMV_ADV(m);
	for ( i = 1; i < len; i++, NMV_ADV(m) ) {
		ndl_lcm(DL(m),d1,d2);
		t = d1; d1 = d2; d2 = t;
	}
	l = nd_nvar+31;
	t = (unsigned int *)MALLOC_ATOMIC(l*sizeof(unsigned int));
	for ( i = nd_exporigin, ind = 0; i < nd_wpd; i++ ) {
		u = d1[i];
		k = (nd_epw-1)*nd_bpe;
		for ( j = 0; j < nd_epw; j++, k -= nd_bpe, ind++ )
			t[ind] = (u>>k)&nd_mask0;
	}
	for ( ; ind < l; ind++ ) t[ind] = 0;
	return t;
}

int nd_get_exporigin(struct order_spec *ord)
{
	switch ( ord->id ) {
		case 0:
			return 1;
		case 1:
			/* block order */
			/* d[0]:weight d[1]:w0,...,d[nd_exporigin-1]:w(n-1) */
			return ord->ord.block.length+1;
		case 2:
			error("nd_get_exporigin : matrix order is not supported yet.");
	}
}

void nd_setup_parameters() {
	int i,j,n,elen,ord_o,ord_l,l,s;
	struct order_pair *op;

	nd_epw = (sizeof(unsigned int)*8)/nd_bpe;
	elen = nd_nvar/nd_epw+(nd_nvar%nd_epw?1:0);

	nd_exporigin = nd_get_exporigin(nd_ord);
	nd_wpd = nd_exporigin+elen;

	if ( nd_bpe < 32 ) {
		nd_mask0 = (1<<nd_bpe)-1;
	} else {
		nd_mask0 = 0xffffffff;
	}
	bzero(nd_mask,sizeof(nd_mask));
	nd_mask1 = 0;
	for ( i = 0; i < nd_epw; i++ ) {
		nd_mask[nd_epw-i-1] = (nd_mask0<<(i*nd_bpe));
		nd_mask1 |= (1<<(nd_bpe-1))<<(i*nd_bpe);
	}
	nm_adv = sizeof(struct oNM)+(nd_wpd-1)*sizeof(unsigned int);
	nmv_adv = sizeof(struct oNMV)+(nd_wpd-1)*sizeof(unsigned int);
	nd_epos = nd_create_epos(nd_ord);
	nd_blockmask = nd_create_blockmask(nd_ord);
}

ND_pairs nd_reconstruct(int mod,int trace,ND_pairs d)
{
	int i,obpe,oadv,h;
	NM prev_nm_free_list;
	RHist mr0,mr;
	RHist r;
	RHist *old_red;
	ND_pairs s0,s,t,prev_ndp_free_list;
	EPOS oepos;

	obpe = nd_bpe;
	oadv = nmv_adv;
	oepos = nd_epos;
	if ( obpe < 4 ) nd_bpe = 4;
	else if ( obpe < 6 ) nd_bpe = 6;
	else if ( obpe < 8 ) nd_bpe = 8;
	else if ( obpe < 16 ) nd_bpe = 16;
	else if ( obpe < 32 ) nd_bpe = 32;
	else error("nd_reconstruct : exponent too large");

	nd_setup_parameters();
	prev_nm_free_list = _nm_free_list;
	prev_ndp_free_list = _ndp_free_list;
	_nm_free_list = 0;
	_ndp_free_list = 0;
	for ( i = nd_psn-1; i >= 0; i-- ) ndv_realloc(nd_ps[i],obpe,oadv,oepos);
	if ( trace )
		for ( i = nd_psn-1; i >= 0; i-- )
			ndv_realloc(nd_ps_trace[i],obpe,oadv,oepos);
	s0 = 0;
	for ( t = d; t; t = NEXT(t) ) {
		NEXTND_pairs(s0,s);
		s->i1 = t->i1;
		s->i2 = t->i2;
		SG(s) = SG(t);
		ndl_reconstruct(obpe,oepos,LCM(t),LCM(s));
	}
	
	old_red = (RHist *)ALLOCA(REDTAB_LEN*sizeof(RHist));
	for ( i = 0; i < REDTAB_LEN; i++ ) {
		old_red[i] = nd_red[i];
		nd_red[i] = 0;
	}
	for ( i = 0; i < REDTAB_LEN; i++ )
		for ( r = old_red[i]; r; r = NEXT(r) ) {
			NEWRHist(mr);
			mr->index = r->index;
			SG(mr) = SG(r);
			ndl_reconstruct(obpe,oepos,DL(r),DL(mr));
			h = ndl_hash_value(DL(mr));
			NEXT(mr) = nd_red[h];
			nd_red[h] = mr;
		}
	for ( i = 0; i < REDTAB_LEN; i++ ) old_red[i] = 0;
	old_red = 0;
	for ( i = 0; i < nd_psn; i++ ) {
		NEWRHist(r); SG(r) = SG(nd_psh[i]);
		ndl_reconstruct(obpe,oepos,DL(nd_psh[i]),DL(r));
		nd_psh[i] = r;
	}
	if ( s0 ) NEXT(s) = 0;
	prev_nm_free_list = 0;
	prev_ndp_free_list = 0;
	GC_gcollect();
	return s0;
}

void nd_reconstruct_direct(int mod,NDV *ps,int len)
{
	int i,obpe,oadv,h;
	unsigned int **bound;
	NM prev_nm_free_list;
	RHist mr0,mr;
	RHist r;
	RHist *old_red;
	ND_pairs s0,s,t,prev_ndp_free_list;
	EPOS oepos;

	obpe = nd_bpe;
	oadv = nmv_adv;
	oepos = nd_epos;
	if ( obpe < 4 ) nd_bpe = 4;
	else if ( obpe < 6 ) nd_bpe = 6;
	else if ( obpe < 8 ) nd_bpe = 8;
	else if ( obpe < 16 ) nd_bpe = 16;
	else if ( obpe < 32 ) nd_bpe = 32;
	else error("nd_reconstruct_direct : exponent too large");

	nd_setup_parameters();
	prev_nm_free_list = _nm_free_list;
	prev_ndp_free_list = _ndp_free_list;
	_nm_free_list = 0; _ndp_free_list = 0;
	for ( i = len-1; i >= 0; i-- ) ndv_realloc(ps[i],obpe,oadv,oepos);
	prev_nm_free_list = 0;
	prev_ndp_free_list = 0;
	GC_gcollect();
}

void ndl_reconstruct(int obpe,EPOS oepos,unsigned int *d,unsigned int *r)
{
	int n,i,ei,oepw,omask0,j,s,ord_l,l;
	struct order_pair *op;
#define GET_EXP_OLD(d,a) (((d)[oepos[a].i]>>oepos[a].s)&omask0)
#define PUT_EXP_OLD(r,a,e) ((r)[oepos[a].i] |= ((e)<<oepos[a].s))

	n = nd_nvar;
	oepw = (sizeof(unsigned int)*8)/obpe;
	omask0 = (1<<obpe)-1;
	TD(r) = TD(d);
	for ( i = nd_exporigin; i < nd_wpd; i++ ) r[i] = 0;
	if ( nd_blockmask ) {
		l = nd_blockmask->n;
		op = nd_blockmask->order_pair;
		for ( i = 1; i < nd_exporigin; i++ )
			r[i] = d[i];
		for ( j = 0, s = 0; j < l; j++ ) {
			ord_l = op[j].length;
			for ( i = 0; i < ord_l; i++, s++ ) {
				ei =  GET_EXP_OLD(d,s);
				PUT_EXP(r,s,ei);
			}
		}
	} else {
		for ( i = 0; i < n; i++ ) {
			ei = GET_EXP_OLD(d,i);
			PUT_EXP(r,i,ei);
		}
	}
}

ND nd_copy(ND p)
{
	NM m,mr,mr0;
	int c,n;
	ND r;

	if ( !p )
		return 0;
	else {
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTNM(mr0,mr);
			CM(mr) = CM(m);
			ndl_copy(DL(m),DL(mr));
		}
		NEXT(mr) = 0; 
		MKND(NV(p),mr0,LEN(p),r);
		SG(r) = SG(p);
		return r;
	}
}

int nd_sp(int mod,int trace,ND_pairs p,ND *rp)
{
	NM m;
	NDV p1,p2;
	ND t1,t2;
	unsigned int *lcm;
	int td;

	if ( trace ) {
		p1 = nd_ps_trace[p->i1]; p2 = nd_ps_trace[p->i2];
	} else {
		p1 = nd_ps[p->i1]; p2 = nd_ps[p->i2];
	}
	lcm = LCM(p);
	NEWNM(m);
	CQ(m) = HCQ(p2);
	ndl_sub(lcm,HDL(p1),DL(m));
	if ( ndl_check_bound2(p->i1,DL(m)) )
		return 0;
	t1 = ndv_mul_nm(mod,m,p1);
	if ( mod ) CM(m) = mod-HCM(p1); 
	else chsgnq(HCQ(p1),&CQ(m));
	ndl_sub(lcm,HDL(p2),DL(m));
	if ( ndl_check_bound2(p->i2,DL(m)) ) {
		nd_free(t1);
		return 0;
	}
	t2 = ndv_mul_nm(mod,m,p2);
	*rp = nd_add(mod,t1,t2);
	FREENM(m);
	return 1;
}

void ndv_mul_c(int mod,NDV p,int mul)
{
	NMV m;
	int c,c1,len,i;

	if ( !p ) return;
	len = LEN(p);
	for ( m = BDY(p), i = 0; i < len; i++, NMV_ADV(m) ) {
		c1 = CM(m); DMAR(c1,mul,0,mod,c); CM(m) = c;
	}
}

void ndv_mul_c_q(NDV p,Q mul)
{
	NMV m;
	Q c;
	int len,i;

	if ( !p ) return;
	len = LEN(p);
	for ( m = BDY(p), i = 0; i < len; i++, NMV_ADV(m) ) {
		mulq(CQ(m),mul,&c); CQ(m) = c;
	}
}

ND weyl_ndv_mul_nm(int mod,NM m0,NDV p) {
	int n2,i,j,l,n,tlen;
	unsigned int *d0;
	NM *tab,*psum;
	ND s,r;
	NM t;
	NMV m1;

	if ( !p ) return 0;
	n = NV(p); n2 = n>>1;
	d0 = DL(m0);
	l = LEN(p);
	for ( i = 0, tlen = 1; i < n2; i++ ) tlen *= (GET_EXP(d0,n2+i)+1);
	tab = (NM *)ALLOCA(tlen*sizeof(NM));
	psum = (NM *)ALLOCA(tlen*sizeof(NM));
	for ( i = 0; i < tlen; i++ ) psum[i] = 0;
	m1 = (NMV)(((char *)BDY(p))+nmv_adv*(l-1));
	for ( i = l-1; i >= 0; i--, NMV_PREV(m1) ) {
		/* m0(NM) * m1(NMV) => tab(NM) */
		weyl_mul_nm_nmv(n,mod,m0,m1,tab,tlen);
		for ( j = 0; j < tlen; j++ ) {
			if ( tab[j] ) {
				NEXT(tab[j]) = psum[j];	psum[j] = tab[j];
			}
		}		
	}
	for ( i = tlen-1, r = 0; i >= 0; i-- )
		if ( psum[i] ) {
			for ( j = 0, t = psum[i]; t; t = NEXT(t), j++ );
			MKND(n,psum[i],j,s);
			r = nd_add(mod,r,s);
		}
	if ( r ) SG(r) = SG(p)+TD(d0);
	return r;
}

/* product of monomials */
/* XXX block order is not handled correctly */

void weyl_mul_nm_nmv(int n,int mod,NM m0,NMV m1,NM *tab,int tlen)
{
	int i,n2,j,s,curlen,homo,h,a,b,k,l,u,min;
	unsigned int *d0,*d1,*d,*dt,*ctab;
	Q *ctab_q;
	Q q,q1;
	unsigned int c0,c1,c;
	NM *p;
	NM m,t;

	for ( i = 0; i < tlen; i++ ) tab[i] = 0;
	if ( !m0 || !m1 ) return;
	d0 = DL(m0); d1 = DL(m1); n2 = n>>1;
	NEWNM(m); d = DL(m);
	if ( mod ) {
		c0 = CM(m0); c1 = CM(m1); DMAR(c0,c1,0,mod,c); CM(m) = c;
	} else
		mulq(CQ(m0),CQ(m1),&CQ(m));
	for ( i = 0; i < nd_wpd; i++ ) d[i] = 0;
	homo = n&1 ? 1 : 0;
	if ( homo ) {
		/* offset of h-degree */
		h = GET_EXP(d0,n-1)+GET_EXP(d1,n-1);
		PUT_EXP(DL(m),n-1,h);
		TD(DL(m)) = h;
		if ( nd_blockmask ) ndl_set_blockweight(DL(m));
	}
	tab[0] = m;
	NEWNM(m); d = DL(m);
	for ( i = 0, curlen = 1; i < n2; i++ ) {
		a = GET_EXP(d0,i); b = GET_EXP(d1,n2+i);
		k = GET_EXP(d0,n2+i); l = GET_EXP(d1,i);
		/* xi^a*(Di^k*xi^l)*Di^b */
		a += l; b += k;
		s = MUL_WEIGHT(a,i)+MUL_WEIGHT(b,n2+i);
		if ( !k || !l ) {
			for ( j = 0; j < curlen; j++ )
				if ( t = tab[j] ) {
					dt = DL(t);
					PUT_EXP(dt,i,a); PUT_EXP(dt,n2+i,b); TD(dt) += s;
					if ( nd_blockmask ) ndl_set_blockweight(dt);
				}
			curlen *= k+1;
			continue;
		}
		min = MIN(k,l);
		if ( mod ) {
			ctab = (unsigned int *)ALLOCA((min+1)*sizeof(unsigned int));
			mkwcm(k,l,mod,ctab);
		} else {
			ctab_q = (Q *)ALLOCA((min+1)*sizeof(Q));
			mkwc(k,l,ctab_q);
		}
		for ( j = min; j >= 0; j-- ) {
			for ( u = 0; u < nd_wpd; u++ ) d[u] = 0;
			PUT_EXP(d,i,a-j); PUT_EXP(d,n2+i,b-j);
			h = MUL_WEIGHT(a-j,i)+MUL_WEIGHT(b-j,n2+i);
			if ( homo ) {
				TD(d) = s;
				PUT_EXP(d,n-1,s-h);
			} else TD(d) = h;
			if ( nd_blockmask ) ndl_set_blockweight(d);
			if ( mod ) c = ctab[j];
			else q = ctab_q[j];
			p = tab+curlen*j;
			if ( j == 0 ) {
				for ( u = 0; u < curlen; u++, p++ ) {
					if ( tab[u] ) {
						ndl_addto(DL(tab[u]),d);
						if ( mod ) {
							c0 = CM(tab[u]); DMAR(c0,c,0,mod,c1); CM(tab[u]) = c1;
						} else {
							mulq(CQ(tab[u]),q,&q1); CQ(tab[u]) = q1;
						}
					}
				}
			} else {
				for ( u = 0; u < curlen; u++, p++ ) {
					if ( tab[u] ) {
						NEWNM(t);
						ndl_add(DL(tab[u]),d,DL(t));
						if ( mod ) {
							c0 = CM(tab[u]); DMAR(c0,c,0,mod,c1); CM(t) = c1;
						} else
							mulq(CQ(tab[u]),q,&CQ(t));
						*p = t;
					}
				}
			}
		}
		curlen *= k+1;
	}
	FREENM(m);
}

ND ndv_mul_nm(int mod,NM m0,NDV p)
{
	NM mr,mr0;
	NMV m;
	unsigned int *d,*dt,*dm;
	int c,n,td,i,c1,c2,len;
	Q q;
	ND r;

	if ( !p ) return 0;
	else if ( do_weyl )
		return weyl_ndv_mul_nm(mod,m0,p);
	else {
		n = NV(p); m = BDY(p);
		d = DL(m0);
		len = LEN(p);
		mr0 = 0;
		td = TD(d);
		if ( mod ) {
			c = CM(m0);
			for ( i = 0; i < len; i++, NMV_ADV(m) ) {
				NEXTNM(mr0,mr);
				c1 = CM(m);
				DMAR(c1,c,0,mod,c2);
				CM(mr) = c2;
				ndl_add(DL(m),d,DL(mr));
			}
		} else {
			q = CQ(m0);
			for ( i = 0; i < len; i++, NMV_ADV(m) ) {
				NEXTNM(mr0,mr);
				mulq(CQ(m),q,&CQ(mr));
				ndl_add(DL(m),d,DL(mr));
			}
		}
		NEXT(mr) = 0; 
		MKND(NV(p),mr0,len,r);
		SG(r) = SG(p) + TD(d);
		return r;
	}
}

void ndv_realloc(NDV p,int obpe,int oadv,EPOS oepos)
{
	NMV m,mr,mr0,t;
	int len,i,k;

#define NMV_OPREV(m) (m = (NMV)(((char *)m)-oadv))

	if ( p ) {
		m = BDY(p); len = LEN(p);
		mr0 = nmv_adv>oadv?(NMV)REALLOC(BDY(p),len*nmv_adv):BDY(p);
		m = (NMV)((char *)mr0+(len-1)*oadv);
		mr = (NMV)((char *)mr0+(len-1)*nmv_adv);
		t = (NMV)ALLOCA(nmv_adv);
		for ( i = 0; i < len; i++, NMV_OPREV(m), NMV_PREV(mr) ) {
			CQ(t) = CQ(m);
			for ( k = 0; k < nd_wpd; k++ ) DL(t)[k] = 0;
			ndl_reconstruct(obpe,oepos,DL(m),DL(t));
			CQ(mr) = CQ(t);
			ndl_copy(DL(t),DL(mr));
		}
		BDY(p) = mr0;
	}
}

NDV ndtondv(int mod,ND p)
{
	NDV d;
	NMV m,m0;
	NM t;
	int i,len;

	if ( !p ) return 0;
	len = LEN(p);
	m0 = m = (NMV)(mod?MALLOC_ATOMIC(len*nmv_adv):MALLOC(len*nmv_adv));
	for ( t = BDY(p), i = 0; t; t = NEXT(t), i++, NMV_ADV(m) ) {
		ndl_copy(DL(t),DL(m));
		CQ(m) = CQ(t);
	}
	MKNDV(NV(p),m0,len,d);
	SG(d) = SG(p);
	return d;
}

ND ndvtond(int mod,NDV p)
{
	ND d;
	NM m,m0;
	NMV t;
	int i,len;

	if ( !p ) return 0;
	m0 = 0;
	len = p->len;
	for ( t = BDY(p), i = 0; i < len; NMV_ADV(t), i++ ) {
		NEXTNM(m0,m);
		ndl_copy(DL(t),DL(m));
		CQ(m) = CQ(t);
	}
	NEXT(m) = 0;
	MKND(NV(p),m0,len,d);
	SG(d) = SG(p);
	return d;
}

NDV dptondv(int mod,DP p)
{
	NDV d;
	NMV m0,m;
	MP t;
	DP q;
	int l,i,n;

	if ( mod ) {
		_dp_mod(p,mod,0,&q); p = q;
	}
	if ( !p ) return 0;
	for ( t = BDY(p), l = 0; t; t = NEXT(t), l++ );
	if ( mod )
		m0 = m = (NMV)MALLOC_ATOMIC(l*nmv_adv);
	else
		m0 = m = (NMV)MALLOC(l*nmv_adv);
	n = NV(p);
	for ( t = BDY(p); t; t = NEXT(t), NMV_ADV(m) ) {
		if ( mod ) CM(m) = ITOS(C(t));
		else CQ(m) = (Q)C(t);
		dltondl(n,DL(t),DL(m));
	}
	MKNDV(n,m0,l,d);
	SG(d) = SG(p);
	return d;
}

DP ndvtodp(int mod,NDV p)
{
	DP d;
	MP m0,m;
	NMV t;
	int len,i,n;

	if ( !p ) return 0;
	m0 = 0;
	len = LEN(p);
	n = NV(p);
	for ( t = BDY(p), i = 0; i < len; i++, NMV_ADV(t) ) {
		NEXTMP(m0,m);
		if ( mod ) C(m) = STOI(CM(t));
		else C(m) = (P)CQ(t);
		DL(m) = ndltodl(n,DL(t));
	}
	NEXT(m) = 0;
	MKDP(NV(p),m0,d);
	SG(d) = SG(p);
	return d;
}

void ndv_print(NDV p)
{
	NMV m;
	int i,len;
    
	if ( !p ) printf("0\n");
	else {
		len = LEN(p);
		for ( m = BDY(p), i = 0; i < len; i++, NMV_ADV(m) ) {
			printf("+%d*",CM(m));
			ndl_print(DL(m));
		}
		printf("\n");
	}
}

void ndv_print_q(NDV p)
{
	NMV m;
	int i,len;
    
	if ( !p ) printf("0\n");
	else {
		len = LEN(p);
		for ( m = BDY(p), i = 0; i < len; i++, NMV_ADV(m) ) {
			printf("+");
			printexpr(CO,CQ(m));
			printf("*");
			ndl_print(DL(m));
		}
		printf("\n");
	}
}

NODE nd_reducebase(NODE x)
{
	int len,i,j;
	NDV *w;
	NODE t,t0;

	len = length(x);
	w = (NDV *)ALLOCA(len*sizeof(NDV));
	for ( i = 0, t = x; i < len; i++, t = NEXT(t) ) w[i] = BDY(t);
	for ( i = 0; i < len; i++ ) {
		for ( j = 0; j < i; j++ ) {
			if ( w[i] && w[j] )
				if ( ndl_reducible(HDL(w[i]),HDL(w[j])) ) w[i] = 0;
				else if ( ndl_reducible(HDL(w[j]),HDL(w[i])) ) w[j] = 0;
		}
	}
	for ( i = len-1, t0 = 0; i >= 0; i-- ) {
		if ( w[i] ) { NEXTNODE(t0,t); BDY(t) = (pointer)w[i]; }
	}
	NEXT(t) = 0; x = t0;
	return x;
}

/* XXX incomplete */

void nd_init_ord(struct order_spec *ord)
{
	switch ( ord->id ) {
		case 0:
			switch ( ord->ord.simple ) {
				case 0:
					nd_dcomp = 1;
					nd_isrlex = 1;
					break;
				case 1:
					nd_dcomp = 1;
					nd_isrlex = 0;
					break;
				case 2:
					nd_dcomp = 0;
					nd_isrlex = 0;
					ndl_compare_function = ndl_lex_compare;
					break;
				case 11:
					/* XXX */
					nd_dcomp = 0;
					nd_isrlex = 1;
					ndl_compare_function = ndl_ww_lex_compare;
					break;
				default:
					error("nd_gr : unsupported order");
			}
			break;
		case 1:
			/* XXX */
			nd_dcomp = -1;
			nd_isrlex = 0;
			ndl_compare_function = ndl_block_compare;
			break;
		case 2:
			error("nd_init_ord : matrix order is not supported yet.");
			break;
	}
	nd_ord = ord;
}

BlockMask nd_create_blockmask(struct order_spec *ord)
{
	int n,i,j,s,l;
	unsigned int *t;
	BlockMask bm;

	if ( !ord->id )
		return 0;
	n = ord->ord.block.length;
	bm = (BlockMask)MALLOC(sizeof(struct oBlockMask));
	bm->n = n;
	bm->order_pair = ord->ord.block.order_pair;
	bm->mask = (unsigned int **)MALLOC(n*sizeof(unsigned int *));
	for ( i = 0, s = 0; i < n; i++ ) {
		bm->mask[i] = t
			= (unsigned int *)MALLOC_ATOMIC(nd_wpd*sizeof(unsigned int));
		for ( j = 0; j < nd_wpd; j++ ) t[j] = 0;
		l = bm->order_pair[i].length;
		for ( j = 0; j < l; j++, s++ ) PUT_EXP(t,s,nd_mask0);		
	}
	return bm;
}

EPOS nd_create_epos(struct order_spec *ord)
{
	int i,j,l,s,ord_l,ord_o;
	EPOS epos;
	struct order_pair *op;

	epos = (EPOS)MALLOC_ATOMIC(nd_nvar*sizeof(struct oEPOS));
	switch ( ord->id ) {
		case 0:
			if ( nd_isrlex ) {
				for ( i = 0; i < nd_nvar; i++ ) {
					epos[i].i = nd_exporigin + (nd_nvar-1-i)/nd_epw;
					epos[i].s = (nd_epw-((nd_nvar-1-i)%nd_epw)-1)*nd_bpe;
				}
			} else {
				for ( i = 0; i < nd_nvar; i++ ) {
					epos[i].i = nd_exporigin + i/nd_epw;
					epos[i].s = (nd_epw-(i%nd_epw)-1)*nd_bpe;
				}
			}
			break;
		case 1:
			/* block order */
			l = ord->ord.block.length;
			op = ord->ord.block.order_pair;
			for ( j = 0, s = 0; j < l; j++ ) {
				ord_o = op[j].order;
				ord_l = op[j].length;
				if ( !ord_o )
					for ( i = 0; i < ord_l; i++ ) {
						epos[s+i].i = nd_exporigin + (s+ord_l-i-1)/nd_epw;
						epos[s+i].s = (nd_epw-((s+ord_l-i-1)%nd_epw)-1)*nd_bpe;
					}
				else
					for ( i = 0; i < ord_l; i++ ) {
						epos[s+i].i = nd_exporigin + (s+i)/nd_epw;
						epos[s+i].s = (nd_epw-((s+i)%nd_epw)-1)*nd_bpe;
					}
				s += ord_l;
			}
			break;
		case 2:
			error("nd_create_epos : matrix order is not supported yet.");
	}
	return epos;
}

/* external interface */

void nd_nf_p(P f,LIST g,LIST v,int m,struct order_spec *ord,P *rp)
{
	NODE t;
	ND nd,nf;
	VL vv;
	DP d,r,dm;
	int stat;

	pltovl(v,&vv);
	nd_nvar = length(vv);
	nd_init_ord(ord);
	initd(ord);
	for ( t = BDY(g); NEXT(t); t = NEXT(t) ) {
		ptod(CO,vv,(P)BDY(t),&d); BDY(t) = (pointer)d;
	}
	ptod(CO,vv,(P)BDY(t),&d); BDY(t) = (pointer)d;
	NEWNODE(NEXT(t)); ptod(CO,vv,f,&d); BDY(NEXT(t)) = d; NEXT(NEXT(t)) = 0;
	nd_setup(m,0,BDY(g));
	nd_psn--;
	nd_scale=2;
	while ( 1 ) {
		nd = (pointer)ndvtond(m,nd_ps[nd_psn]);
		stat = nd_nf(m,nd,nd_ps,1,&nf);
		if ( !stat ) {
			nd_psn++;
			nd_reconstruct(m,0,0);
			nd_psn--;
		} else
			break;
	}
	r = (DP)ndtodp(m,nf);
	if ( m ) _dtop_mod(CO,vv,r,rp);
	else dtop(CO,vv,r,rp);
}

