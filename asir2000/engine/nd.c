/* $OpenXM: OpenXM_contrib2/asir2000/engine/nd.c,v 1.21 2003/08/01 08:39:54 noro Exp $ */

#include "ca.h"
#include "inline.h"

#if defined(__GNUC__)
#define INLINE inline
#elif defined(VISUAL)
#define INLINE __inline
#else
#define INLINE
#endif

#define REDTAB_LEN 32003

typedef struct oPGeoBucket {
	int m;
	struct oND *body[32];
} *PGeoBucket;

typedef struct oND {
	struct oNM *body;
	int nv;
	int sugar;
} *ND;

typedef struct oNDV {
	struct oNMV *body;
	int nv;
	int sugar;
	int len;
} *NDV;

typedef struct oNM {
	struct oNM *next;
	union {
		int m;
		Q z;
	} c;
	int td;
	unsigned int dl[1];
} *NM;

typedef struct oNMV {
	union {
		int m;
		Q z;
	} c;
	int td;
	unsigned int dl[1];
} *NMV;

typedef struct oRHist {
	struct oRHist *next;
	int index;
	int td,sugar;
	unsigned int dl[1];
} *RHist;

typedef struct oND_pairs {
	struct oND_pairs *next;
	int i1,i2;
	int td,sugar;
	unsigned int lcm[1];
} *ND_pairs;

double nd_scale=2;
static unsigned int **nd_bound;
int nd_nvar;
int is_rlex;
int nd_epw,nd_bpe,nd_wpd;
unsigned int nd_mask[32];
unsigned int nd_mask0,nd_mask1;

NM _nm_free_list;
ND _nd_free_list;
ND_pairs _ndp_free_list;

static NDV *nd_ps;
static NDV *nd_psq;
int *nd_psl;
RHist *nd_psh;
int nd_psn,nd_pslen;

RHist *nd_red;
int nd_red_len;

int nd_found,nd_create,nd_notfirst;
int nm_adv;
int nmv_adv;
int nmv_len;
NDV ndv_red;

extern int Top,Reverse;

#define HTD(d) ((d)->body->td)
#define HDL(d) ((d)->body->dl)
#define HCM(d) ((d)->body->c.m)
#define HCQ(d) ((d)->body->c.z)
#define CM(a) ((a)->c.m)
#define CQ(a) ((a)->c.z)
#define DL(a) ((a)->dl)
#define TD(a) ((a)->td)
#define SG(a) ((a)->sugar)
#define LEN(a) ((a)->len)

#define NM_ADV(m) (m = (NM)(((char *)m)+nm_adv))
#define NEWRHist(r) \
((r)=(RHist)MALLOC(sizeof(struct oRHist)+(nd_wpd-1)*sizeof(unsigned int)))
#define NEWND_pairs(m) if(!_ndp_free_list)_NDP_alloc(); (m)=_ndp_free_list; _ndp_free_list = NEXT(_ndp_free_list)   
#define NEWNM(m) if(!_nm_free_list)_NM_alloc(); (m)=_nm_free_list; _nm_free_list = NEXT(_nm_free_list)   
#define MKND(n,m,d) if(!_nd_free_list)_ND_alloc(); (d)=_nd_free_list; _nd_free_list = (ND)BDY(_nd_free_list); (d)->nv=(n); BDY(d)=(m)

#define NEXTRHist(r,c) \
if(!(r)){NEWRHist(r);(c)=(r);}else{NEWRHist(NEXT(c));(c)=NEXT(c);}
#define NEXTNM(r,c) \
if(!(r)){NEWNM(r);(c)=(r);}else{NEWNM(NEXT(c));(c)=NEXT(c);}
#define NEXTNM2(r,c,s) \
if(!(r)){(c)=(r)=(s);}else{NEXT(c)=(s);(c)=(s);}
#define FREENM(m) NEXT(m)=_nm_free_list; _nm_free_list=(m)
#define FREENDP(m) NEXT(m)=_ndp_free_list; _ndp_free_list=(m)
#define FREEND(m) BDY(m)=(NM)_nd_free_list; _nd_free_list=(m)

#define NEXTND_pairs(r,c) \
if(!(r)){NEWND_pairs(r);(c)=(r);}else{NEWND_pairs(NEXT(c));(c)=NEXT(c);}

void nd_removecont(int mod,ND p);
void nd_removecont2(ND p1,ND p2);
void ndv_removecont(int mod,NDV p);
void ndv_mul_c_q(NDV p,Q mul);
void nd_mul_c_q(ND p,Q mul);

void GC_gcollect();
NODE append_one(NODE,int);

void removecont_array(Q *c,int n);
ND_pairs crit_B( ND_pairs d, int s );
void nd_gr(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp);
void nd_gr_trace(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp);
NODE nd_setup(int mod,NODE f);
int nd_newps(int mod,ND a);
int nd_newps_trace(int mod,ND nf,ND nfq);
ND_pairs nd_minp( ND_pairs d, ND_pairs *prest );
NODE update_base(NODE nd,int ndp);
static ND_pairs equivalent_pairs( ND_pairs d1, ND_pairs *prest );
int crit_2( int dp1, int dp2 );
ND_pairs crit_F( ND_pairs d1 );
ND_pairs crit_M( ND_pairs d1 );
ND_pairs nd_newpairs( NODE g, int t );
ND_pairs update_pairs( ND_pairs d, NODE /* of index */ g, int t);
NODE nd_gb(int m,NODE f);
NODE nd_gb_trace(int m,NODE f);
void nd_free_private_storage();
void _NM_alloc();
void _ND_alloc();
int ndl_td(unsigned int *d);
ND nd_add(int mod,ND p1,ND p2);
ND nd_add_q(ND p1,ND p2);
ND nd_mul_nm(int mod,ND p,NM m0);
ND nd_mul_ind_nm(int mod,int index,NM m0);
int nd_sp(int mod,ND_pairs p,ND *nf);
int nd_find_reducer(ND g);
int nd_nf(int mod,ND g,int full,ND *nf);
ND nd_reduce(ND p1,ND p2);
ND nd_reduce_special(ND p1,ND p2);
void nd_free(ND p);
void ndl_print(unsigned int *dl);
void nd_print(ND p);
void nd_print_q(ND p);
void ndv_print(NDV p);
void ndv_print_q(NDV p);
void ndp_print(ND_pairs d);
int nd_length(ND p);
void nd_mul_c(int mod,ND p,int mul);
void nd_free_redlist();
void nd_append_red(unsigned int *d,int td,int i);
unsigned int *nd_compute_bound(ND p);
unsigned int *dp_compute_bound(DP p);
ND_pairs nd_reconstruct(int mod,int trace,ND_pairs ndp);
void nd_setup_parameters();
void nd_realloc(ND p,int obpe);
ND nd_copy(ND p);
void ndl_dup(int obpe,unsigned int *d,unsigned int *r);

#define NMV_ADV(m) (m = (NMV)(((char *)m)+nmv_adv))
#define NEWNDV(d) ((d)=(NDV)MALLOC(sizeof(struct oNDV)))
#define MKNDV(n,m,l,d) NEWNDV(d); NV(d)=(n); BDY(d)=(m); LEN(d) = l;
void ndv_mul_c(int mod,NDV p,int mul);
ND ndv_add(int mod,ND p1,NDV p2);
ND ndv_add_q(ND p1,NDV p2);
NDV ndtondv(int mod,ND p);
void ndv_mul_nm(int mod,NDV pv,NM m,NDV r);
ND ndv_mul_nm_create(int mod,NDV p,NM m0);
void ndv_realloc(NDV p,int obpe,int oadv);
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

INLINE nd_length(ND p)
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

	switch ( nd_bpe ) {
		case 4:
			for ( i = 0; i < nd_wpd; i++ ) {
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
			for ( i = 0; i < nd_wpd; i++ ) {
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
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xff000000) < (u2&0xff000000) ) return 0;
				if ( (u1&0xff0000) < (u2&0xff0000) ) return 0;
				if ( (u1&0xff00) < (u2&0xff00) ) return 0;
				if ( (u1&0xff) < (u2&0xff) ) return 0;
			}
			return 1;
			break;
		case 16:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xffff0000) < (u2&0xffff0000) ) return 0;
				if ( (u1&0xffff) < (u2&0xffff) ) return 0;
			}
			return 1;
			break;
		case 32:
			for ( i = 0; i < nd_wpd; i++ )
				if ( d1[i] < d2[i] ) return 0;
			return 1;
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				for ( j = 0; j < nd_epw; j++ )
					if ( (u1&nd_mask[j]) < (u2&nd_mask[j]) ) return 0;
			}
			return 1;
	}
}

void ndl_lcm(unsigned int *d1,unsigned *d2,unsigned int *d)
{
	unsigned int t1,t2,u,u1,u2;
	int i,j;

	switch ( nd_bpe ) {
		case 4:
			for ( i = 0; i < nd_wpd; i++ ) {
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
			for ( i = 0; i < nd_wpd; i++ ) {
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
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xff000000); t2 = (u2&0xff000000); u = t1>t2?t1:t2;
				t1 = (u1&0xff0000); t2 = (u2&0xff0000); u |= t1>t2?t1:t2;
				t1 = (u1&0xff00); t2 = (u2&0xff00); u |= t1>t2?t1:t2;
				t1 = (u1&0xff); t2 = (u2&0xff); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 16:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xffff0000); t2 = (u2&0xffff0000); u = t1>t2?t1:t2;
				t1 = (u1&0xffff); t2 = (u2&0xffff); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 32:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i]; 
				d[i] = u1>u2?u1:u2;
			}
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i]; 
				for ( j = 0, u = 0; j < nd_epw; j++ ) {
					t1 = (u1&nd_mask[j]); t2 = (u2&nd_mask[j]); u |= t1>t2?t1:t2;
				}
				d[i] = u;
			}
			break;
	}
}

int ndl_td(unsigned int *d)
{
	unsigned int t,u;
	int i,j;

	for ( t = 0, i = 0; i < nd_wpd; i++ ) {
		u = d[i];
		for ( j = 0; j < nd_epw; j++, u>>=nd_bpe )
			t += (u&nd_mask0); 
	}
	return t;
}

INLINE int ndl_compare(unsigned int *d1,unsigned int *d2)
{
	int i;

	for ( i = 0; i < nd_wpd; i++, d1++, d2++ )
		if ( *d1 > *d2 )
			return is_rlex ? -1 : 1;
		else if ( *d1 < *d2 )
			return is_rlex ? 1 : -1;
	return 0;
}

INLINE int ndl_equal(unsigned int *d1,unsigned int *d2)
{
	int i;

	for ( i = 0; i < nd_wpd; i++ )
		if ( d1[i] != d2[i] )
			return 0;
	return 1;
}

INLINE void ndl_copy(unsigned int *d1,unsigned int *d2)
{
	int i;

	switch ( nd_wpd ) {
		case 1:
			d2[0] = d1[0];
			break;
		case 2:
			d2[0] = d1[0];
			d2[1] = d1[1];
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

	switch ( nd_wpd ) {
		case 1:
			d[0] = d1[0]+d2[0];
			break;
		case 2:
			d[0] = d1[0]+d2[0];
			d[1] = d1[1]+d2[1];
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ )
				d[i] = d1[i]+d2[i];
			break;
	}
}

INLINE void ndl_add2(unsigned int *d1,unsigned int *d2)
{
	int i;

	switch ( nd_wpd ) {
		case 1:
			d2[0] += d1[0];
			break;
		case 2:
			d2[0] += d1[0];
			d2[1] += d1[1];
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ )
				d2[i] += d1[i];
			break;
	}
}

void ndl_sub(unsigned int *d1,unsigned int *d2,unsigned int *d)
{
	int i;

	for ( i = 0; i < nd_wpd; i++ )
		d[i] = d1[i]-d2[i];
}

int ndl_disjoint(unsigned int *d1,unsigned int *d2)
{
	unsigned int t1,t2,u,u1,u2;
	int i,j;

	switch ( nd_bpe ) {
		case 4:
			for ( i = 0; i < nd_wpd; i++ ) {
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
			for ( i = 0; i < nd_wpd; i++ ) {
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
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xff000000; t2 = u2&0xff000000; if ( t1&&t2 ) return 0;
				t1 = u1&0xff0000; t2 = u2&0xff0000; if ( t1&&t2 ) return 0;
				t1 = u1&0xff00; t2 = u2&0xff00; if ( t1&&t2 ) return 0;
				t1 = u1&0xff; t2 = u2&0xff; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 16:
			for ( i = 0; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xffff0000; t2 = u2&0xffff0000; if ( t1&&t2 ) return 0;
				t1 = u1&0xffff; t2 = u2&0xffff; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 32:
			for ( i = 0; i < nd_wpd; i++ )
				if ( d1[i] && d2[i] ) return 0;
			return 1;
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ ) {
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
			for ( i = 0; i < nd_wpd; i++ ) {
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
			for ( i = 0; i < nd_wpd; i++ ) {
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
			for ( i = 0; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>24)&0xff) >= 0x100 ) return 1;
				if ( d1[ind++]+((u2>>16)&0xff) >= 0x100 ) return 1;
				if ( d1[ind++]+((u2>>8)&0xff) >= 0x100 ) return 1;
				if ( d1[ind++]+(u2&0xff) >= 0x100 ) return 1;
			}
			return 0;
			break;
		case 16:
			for ( i = 0; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>16)&0xffff) > 0x10000 ) return 1;
				if ( d1[ind++]+(u2&0xffff) > 0x10000 ) return 1;
			}
			return 0;
			break;
		case 32:
			for ( i = 0; i < nd_wpd; i++ )
				if ( d1[i]+d2[i]<d1[i] ) return 1;
			return 0;
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ ) {
				u2 = d2[i];
				k = (nd_epw-1)*nd_bpe;
				for ( j = 0; j < nd_epw; j++, k -= nd_bpe )
					if ( d1[ind++]+((u2>>k)&nd_mask0) > nd_mask0 ) return 1;
			}
			return 0;
			break;
	}
}

INLINE int ndl_hash_value(int td,unsigned int *d)
{
	int i;
	int r;

	r = td;
	for ( i = 0; i < nd_wpd; i++ )	
		r = ((r<<16)+d[i])%REDTAB_LEN;
	return r;
}

INLINE int nd_find_reducer(ND g)
{
	RHist r;
	int d,k,i;

	d = ndl_hash_value(HTD(g),HDL(g));
	for ( r = nd_red[d], k = 0; r; r = NEXT(r), k++ ) {
		if ( HTD(g) == TD(r) && ndl_equal(HDL(g),DL(r)) ) {
			if ( k > 0 ) nd_notfirst++;
			nd_found++;
			return r->index;
		}
	}

	if ( Reverse )
		for ( i = nd_psn-1; i >= 0; i-- ) {
			r = nd_psh[i];
			if ( HTD(g) >= TD(r) && ndl_reducible(HDL(g),DL(r)) ) {
				nd_create++;
				nd_append_red(HDL(g),HTD(g),i);
				return i;
			}
		}
	else
		for ( i = 0; i < nd_psn; i++ ) {
			r = nd_psh[i];
			if ( HTD(g) >= TD(r) && ndl_reducible(HDL(g),DL(r)) ) {
				nd_create++;
				nd_append_red(HDL(g),HTD(g),i);
				return i;
			}
		}
	return -1;
}

ND nd_add(int mod,ND p1,ND p2)
{
	int n,c;
	int t;
	ND r;
	NM m1,m2,mr0,mr,s;

	if ( !p1 )
		return p2;
	else if ( !p2 )
		return p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			if ( TD(m1) > TD(m2) )
				c = 1;
			else if ( TD(m1) < TD(m2) )
				c = -1;
			else
				c = ndl_compare(DL(m1),DL(m2));
			switch ( c ) {
				case 0:
					t = ((CM(m1))+(CM(m2))) - mod;
					if ( t < 0 )
						t += mod;
					s = m1; m1 = NEXT(m1);
					if ( t ) {
						NEXTNM2(mr0,mr,s); CM(mr) = (t);
					} else {
						FREENM(s);
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
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else
				return 0;
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		BDY(p1) = mr0;
		SG(p1) = MAX(SG(p1),SG(p2));
		FREEND(p2);
		return p1;
	}
}

ND nd_add_q(ND p1,ND p2)
{
	int n,c;
	ND r;
	NM m1,m2,mr0,mr,s;
	Q t;

	if ( !p1 )
		return p2;
	else if ( !p2 )
		return p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			if ( TD(m1) > TD(m2) )
				c = 1;
			else if ( TD(m1) < TD(m2) )
				c = -1;
			else
				c = ndl_compare(DL(m1),DL(m2));
			switch ( c ) {
				case 0:
					addq(CQ(m1),CQ(m2),&t);
					s = m1; m1 = NEXT(m1);
					if ( t ) {
						NEXTNM2(mr0,mr,s); CQ(mr) = (t);
					} else {
						FREENM(s);
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
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else
				return 0;
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		BDY(p1) = mr0;
		SG(p1) = MAX(SG(p1),SG(p2));
		FREEND(p2);
		return p1;
	}
}

#if 1
/* ret=1 : success, ret=0 : overflow */
int nd_nf(int mod,ND g,int full,ND *rp)
{
	ND d;
	NM m,mrd,tail;
	NM mul;
	int n,sugar,psugar,sugar0,stat,index;
	int c,c1,c2;
	RHist h;
	NDV p,red;
	Q cg,cred,gcd;
	double hmag;

	if ( !g ) {
		*rp = 0;
		return 1;
	}
	if ( !mod )
		hmag = ((double)p_mag((P)HCQ(g)))*nd_scale;

	sugar0 = sugar = SG(g);
	n = NV(g);
	mul = (NM)ALLOCA(sizeof(struct oNM)+(nd_wpd-1)*sizeof(unsigned int));
	for ( d = 0; g; ) {
		index = nd_find_reducer(g);
		if ( index >= 0 ) {
			h = nd_psh[index];
			ndl_sub(HDL(g),DL(h),DL(mul));
			TD(mul) = HTD(g)-TD(h);
#if 0
			if ( d && (SG(p)+TD(mul)) > sugar ) {
				goto afo;				
			}
#endif
			if ( ndl_check_bound2(index,DL(mul)) ) {
				nd_free(g); nd_free(d);
				return 0;
			}
			if ( mod ) {
				p = nd_ps[index];
				c1 = invm(HCM(p),mod); c2 = mod-HCM(g);
				DMAR(c1,c2,0,mod,c); CM(mul) = c;
			} else {
				p = nd_psq[index];
				igcd_cofactor(HCQ(g),HCQ(p),&gcd,&cg,&cred);
				chsgnq(cg,&CQ(mul));
				nd_mul_c_q(d,cred); nd_mul_c_q(g,cred);
			}
			ndv_mul_nm(mod,p,mul,ndv_red);
			g = ndv_add(mod,g,ndv_red);
			sugar = MAX(sugar,SG(ndv_red));
			if ( !mod && hmag && g && ((double)(p_mag((P)HCQ(g))) > hmag) ) {
				nd_removecont2(d,g);
				hmag = ((double)p_mag((P)HCQ(g)))*nd_scale;
			}
		} else if ( !full ) {
			*rp = g;
			return 1;
		} else {
afo:
			m = BDY(g); 
			if ( NEXT(m) ) {
				BDY(g) = NEXT(m); NEXT(m) = 0;
			} else {
				FREEND(g); g = 0;
			}
			if ( d ) {
				NEXT(tail)=m;
				tail=m;
			} else {
				MKND(n,m,d);
				tail = BDY(d);
			}
		}
	}
	if ( d )
		SG(d) = sugar;
	*rp = d;
	return 1;
}
#else

ND nd_remove_head(ND p)
{
	NM m;

	m = BDY(p);
	if ( !NEXT(m) ) {
		FREEND(p);
		p = 0;
	} else
		BDY(p) = NEXT(m);
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

void add_pbucket(int mod,PGeoBucket g,ND d)
{
	int l,k,m;

	l = nd_length(d);
	for ( k = 0, m = 1; l > m; k++, m <<= 2 );
	/* 4^(k-1) < l <= 4^k */
	d = nd_add(mod,g->body[k],d);
	for ( ; d && nd_length(d) > 1<<(2*k); k++ ) {
		g->body[k] = 0;
		d = nd_add(int mod,g->body[k+1],d);
	}
	g->body[k] = d;
	g->m = MAX(g->m,k);
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
				di = HDL(gi);
				nv = NV(gi);
				if ( HTD(gi) > HTD(gj) )
					c = 1;
				else if ( HTD(gi) < HTD(gj) )
					c = -1;
				else
					c = ndl_compare(di,dj);
				if ( c > 0 ) {
					if ( sum )
						HCM(gj) = sum;
					else
						g->body[j] = nd_remove_head(gj);
					j = i;
					gj = g->body[j];
					dj = HDL(gj);
					sum = HCM(gj);
				} else if ( c == 0 ) {
					sum = sum+HCM(gi)-mod;
					if ( sum < 0 )
						sum += mod;
					g->body[i] = nd_remove_head(gi);
				}
			}
		}
		if ( j < 0 )
			return -1;
		else if ( sum ) {
			HCM(gj) = sum;
			return j;
		} else
			g->body[j] = nd_remove_head(gj);
	}
}

ND normalize_pbucket(PGeoBucket g)
{
	int i;
	ND r,t;

	r = 0;
	for ( i = 0; i <= g->m; i++ )
		r = nd_add(mod,r,g->body[i]);
	return r;
}

ND nd_nf(ND g,int full)
{
	ND u,p,d,red;
	NODE l;
	NM m,mrd;
	int sugar,psugar,n,h_reducible,h;
	PGeoBucket bucket;

	if ( !g ) {
		return 0;
	}
	sugar = SG(g);
	n = NV(g);
	bucket = create_pbucket();
	add_pbucket(bucket,g);
	d = 0;
	while ( 1 ) {
		h = head_pbucket(bucket);
		if ( h < 0 ) {
			if ( d )
				SG(d) = sugar;
			return d;
		}
		g = bucket->body[h];
		red = nd_find_reducer(g);
		if ( red ) {
			bucket->body[h] = nd_remove_head(g);
			red = nd_remove_head(red);
			add_pbucket(bucket,red);
			sugar = MAX(sugar,SG(red));
		} else if ( !full ) {
			g = normalize_pbucket(bucket);
			if ( g )
				SG(g) = sugar;
			return g;
		} else {
			m = BDY(g); 
			if ( NEXT(m) ) {
				BDY(g) = NEXT(m); NEXT(m) = 0;
			} else {
				FREEND(g); g = 0;
			}
			bucket->body[h] = g;
			NEXT(m) = 0;
			if ( d ) {
				for ( mrd = BDY(d); NEXT(mrd); mrd = NEXT(mrd) );
				NEXT(mrd) = m;
			} else {
				MKND(n,m,d);
			}
		}
	}
}
#endif

NODE nd_gb(int m,NODE f)
{
	int i,nh,sugar,stat;
	NODE r,g,gall;
	ND_pairs d;
	ND_pairs l;
	ND h,nf;

	for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
		i = (int)BDY(r);
		d = update_pairs(d,g,i);
		g = update_base(g,i);
		gall = append_one(gall,i);
	}
	sugar = 0;
	while ( d ) {
again:
		l = nd_minp(d,&d);
		if ( SG(l) != sugar ) {
			sugar = SG(l);
			fprintf(asir_out,"%d",sugar);
		}
		stat = nd_sp(m,l,&h);
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(m,0,d);
			goto again;
		}
		stat = nd_nf(m,h,!Top,&nf);
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(m,0,d);
			goto again;
		} else if ( nf ) {
			printf("+"); fflush(stdout);
			nh = nd_newps(m,nf);
			d = update_pairs(d,g,nh);
			g = update_base(g,nh);
			gall = append_one(gall,nh);
			FREENDP(l);
		} else {
			printf("."); fflush(stdout);
			FREENDP(l);
		}
	}
	return g;
}

NODE nd_gb_trace(int m,NODE f)
{
	int i,nh,sugar,stat;
	NODE r,g,gall;
	ND_pairs d;
	ND_pairs l;
	ND h,nf,nfq;

	for ( gall = g = 0, d = 0, r = f; r; r = NEXT(r) ) {
		i = (int)BDY(r);
		d = update_pairs(d,g,i);
		g = update_base(g,i);
		gall = append_one(gall,i);
	}
	sugar = 0;
	while ( d ) {
again:
		l = nd_minp(d,&d);
		if ( SG(l) != sugar ) {
			sugar = SG(l);
			fprintf(asir_out,"%d",sugar);
		}
		stat = nd_sp(m,l,&h);
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(m,1,d);
			goto again;
		}
		stat = nd_nf(m,h,!Top,&nf);
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(m,1,d);
			goto again;
		} else if ( nf ) {
			/* overflow does not occur */
			nd_sp(0,l,&h);
			nd_nf(0,h,!Top,&nfq);
			if ( nfq ) {
				printf("+"); fflush(stdout);
				nh = nd_newps_trace(m,nf,nfq);
				d = update_pairs(d,g,nh);
				g = update_base(g,nh);
				gall = append_one(gall,nh);
			} else {
				printf("*"); fflush(stdout);
			}
		} else {
			printf("."); fflush(stdout);
		}
		FREENDP(l);
	}
	return g;
}

ND_pairs update_pairs( ND_pairs d, NODE /* of index */ g, int t)
{
	ND_pairs d1,nd,cur,head,prev,remove;

	if ( !g ) return d;
	d = crit_B(d,t);
	d1 = nd_newpairs(g,t);
	d1 = crit_M(d1);
	d1 = crit_F(d1);
	prev = 0; cur = head = d1;
	while ( cur ) {
		if ( crit_2( cur->i1,cur->i2 ) ) {
			remove = cur;
			if ( !prev ) {
				head = cur = NEXT(cur);
			} else {
				cur = NEXT(prev) = NEXT(cur);
			}
			FREENDP(remove);
		} else {
			prev = cur;
			cur = NEXT(cur);
		}
	}
	if ( !d )
		return head;
	else {
		nd = d;
		while ( NEXT(nd) )
			nd = NEXT(nd);
		NEXT(nd) = head;
		return d;
	}
}

ND_pairs nd_newpairs( NODE g, int t )
{
	NODE h;
	unsigned int *dl;
	int td,ts,s;
	ND_pairs r,r0;

	dl = DL(nd_psh[t]);
	td = TD(nd_psh[t]);
	ts = SG(nd_psh[t]) - td;
	for ( r0 = 0, h = g; h; h = NEXT(h) ) {
		NEXTND_pairs(r0,r);
		r->i1 = (int)BDY(h);
		r->i2 = t;
		ndl_lcm(DL(nd_psh[r->i1]),dl,r->lcm);
		TD(r) = ndl_td(r->lcm);
		s = SG(nd_psh[r->i1])-TD(nd_psh[r->i1]);
		SG(r) = MAX(s,ts) + TD(r);
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
			prev = cur;
			cur = NEXT(cur);
		}
	}
	return head;
}

ND_pairs crit_M( ND_pairs d1 )
{
	ND_pairs e,d2,d3,dd,p;
	unsigned int *id,*jd;
	int itd,jtd;

	for ( dd = 0, e = d1; e; e = d3 ) {
		if ( !(d2 = NEXT(e)) ) {
			NEXT(e) = dd;
			return e;
		}
		id = e->lcm;
		itd = TD(e);
		for ( d3 = 0; d2; d2 = p ) {
			p = NEXT(d2),
			jd = d2->lcm;
			jtd = TD(d2);
			if ( jtd == itd  )
				if ( id == jd );
				else if ( ndl_reducible(jd,id) ) continue;
				else if ( ndl_reducible(id,jd) ) goto delit;
				else ;
			else if ( jtd > itd )
				if ( ndl_reducible(jd,id) ) continue;
				else ;
			else if ( ndl_reducible(id,jd ) ) goto delit;
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

static ND_pairs equivalent_pairs( ND_pairs d1, ND_pairs *prest )
{
	ND_pairs w,p,r,s;
	unsigned int *d;
	int td;

	w = d1;
	d = w->lcm;
	td = TD(w);
	s = NEXT(w);
	NEXT(w) = 0;
	for ( r = 0; s; s = p ) {
		p = NEXT(s);
		if ( td == TD(s) && ndl_equal(d,s->lcm) ) {
			NEXT(s) = w;
			w = s;
		} else {
			NEXT(s) = r;
			r = s;
		}
	}
	*prest = r;
	return w;
}

NODE update_base(NODE nd,int ndp)
{
	unsigned int *dl, *dln;
	NODE last, p, head;
	int td,tdn;

	dl = DL(nd_psh[ndp]);
	td = TD(nd_psh[ndp]);
	for ( head = last = 0, p = nd; p; ) {
		dln = DL(nd_psh[(int)BDY(p)]);
		tdn = TD(nd_psh[(int)BDY(p)]);
		if ( tdn >= td && ndl_reducible( dln, dl ) ) {
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
	int s,td,len,tlen,c;

	if ( !(p = NEXT(m = d)) ) {
		*prest = p;
		NEXT(m) = 0;
		return m;
	}
	lcm = m->lcm;
	s = SG(m);
	td = TD(m);
	len = nd_psl[m->i1]+nd_psl[m->i2];
	for ( ml = 0, l = m; p; p = NEXT(l = p) ) {
		if (SG(p) < s)
			goto find;
		else if ( SG(p) == s ) {
			if ( TD(p) < td )
				goto find;
			else if ( TD(p) == td ) {
				c = ndl_compare(p->lcm,lcm);
				if ( c < 0 )
					goto find;
#if 0
				else if ( c == 0 ) {
					tlen = nd_psl[p->i1]+nd_psl[p->i2];
					if ( tlen < len )
						goto find;
				}
#endif
			}
		}
		continue;
find:
		ml = l;
		m = p;
		lcm = m->lcm;
		s = SG(m);
		td = TD(m);
		len = tlen;
	}
	if ( !ml ) *prest = NEXT(m);
	else {
		NEXT(ml) = NEXT(m);
		*prest = d;
	}
	NEXT(m) = 0;
	return m;
}

int nd_newps(int mod,ND a)
{
	int len;
	RHist r;
	NDV b;

	if ( nd_psn == nd_pslen ) {
		nd_pslen *= 2;
		nd_psl = (int *)REALLOC((char *)nd_psl,nd_pslen*sizeof(int));
		nd_ps = (NDV *)REALLOC((char *)nd_ps,nd_pslen*sizeof(NDV));
		nd_psq = (NDV *)REALLOC((char *)nd_psq,nd_pslen*sizeof(NDV));
		nd_psh = (RHist *)REALLOC((char *)nd_psh,nd_pslen*sizeof(RHist));
		nd_bound = (unsigned int **)
			REALLOC((char *)nd_bound,nd_pslen*sizeof(unsigned int *));
	}
	nd_removecont(mod,a);
	nd_bound[nd_psn] = nd_compute_bound(a);
	NEWRHist(r); SG(r) = SG(a); TD(r) = HTD(a); ndl_copy(HDL(a),DL(r)); 
	nd_psh[nd_psn] = r;
	b = ndtondv(mod,a);
	len = LEN(b);
	if ( mod )
		nd_ps[nd_psn] = b;
	else
		nd_psq[nd_psn] = b;
	nd_psl[nd_psn] = len;
	nd_free(a);
	if ( len > nmv_len ) {
		nmv_len = 2*len;
		BDY(ndv_red) = (NMV)REALLOC(BDY(ndv_red),nmv_len*nmv_adv);
	}
	return nd_psn++;
}

int nd_newps_trace(int mod,ND nf,ND nfq)
{
	int len;
	RHist r;
	NDV b;

	if ( nd_psn == nd_pslen ) {
		nd_pslen *= 2;
		nd_psl = (int *)REALLOC((char *)nd_psl,nd_pslen*sizeof(int));
		nd_ps = (NDV *)REALLOC((char *)nd_ps,nd_pslen*sizeof(NDV));
		nd_psq = (NDV *)REALLOC((char *)nd_psq,nd_pslen*sizeof(NDV));
		nd_psh = (RHist *)REALLOC((char *)nd_psh,nd_pslen*sizeof(RHist));
		nd_bound = (unsigned int **)
			REALLOC((char *)nd_bound,nd_pslen*sizeof(unsigned int *));
	}
	nd_removecont(mod,nf);
	nd_ps[nd_psn] = ndtondv(mod,nf);

	nd_removecont(0,nfq);
	nd_psq[nd_psn] = ndtondv(0,nfq);

	nd_bound[nd_psn] = nd_compute_bound(nfq);
	NEWRHist(r); SG(r) = SG(nf); TD(r) = HTD(nf); ndl_copy(HDL(nf),DL(r)); 
	nd_psh[nd_psn] = r;

	len = LEN(nd_psq[nd_psn]);
	nd_psl[nd_psn] = len;

	nd_free(nf); nd_free(nfq);
	if ( len > nmv_len ) {
		nmv_len = 2*len;
		BDY(ndv_red) = (NMV)REALLOC(BDY(ndv_red),nmv_len*nmv_adv);
	}
	return nd_psn++;
}

NODE nd_setup(int mod,NODE f)
{
	int i,j,td,len,max;
	NODE s,s0,f0;
	unsigned int *d;
	RHist r;
	NDV a;

	nd_found = 0; nd_notfirst = 0; nd_create = 0;

	nd_psn = length(f); nd_pslen = 2*nd_psn;
	nd_psl = (int *)MALLOC(nd_pslen*sizeof(int));
	nd_ps = (NDV *)MALLOC(nd_pslen*sizeof(NDV));
	nd_psq = (NDV *)MALLOC(nd_pslen*sizeof(NDV));
	nd_psh = (RHist *)MALLOC(nd_pslen*sizeof(RHist));
	nd_bound = (unsigned int **)MALLOC(nd_pslen*sizeof(unsigned int *));
	for ( max = 0, i = 0, s = f; i < nd_psn; i++, s = NEXT(s) ) {
		nd_bound[i] = d = dp_compute_bound((DP)BDY(s));
		for ( j = 0; j < nd_nvar; j++ )
			max = MAX(d[j],max);
	}
	if ( !nd_red )
		nd_red = (RHist *)MALLOC(REDTAB_LEN*sizeof(RHist));
	bzero(nd_red,REDTAB_LEN*sizeof(RHist));

	if ( max < 2 )
		nd_bpe = 2;
	else if ( max < 4 )
		nd_bpe = 4;
	else if ( max < 64 )
		nd_bpe = 6;
	else if ( max < 256 )
		nd_bpe = 8;
	else if ( max < 65536 )
		nd_bpe = 16;
	else
		nd_bpe = 32;

	nd_setup_parameters();
	nd_free_private_storage();
	len = 0;
	for ( i = 0; i < nd_psn; i++, f = NEXT(f) ) {
		NEWRHist(r);
		a = dptondv(mod,(DP)BDY(f));
		ndv_removecont(mod,a);
		len = MAX(len,LEN(a));
		SG(r) = HTD(a); TD(r) = HTD(a); ndl_copy(HDL(a),DL(r));
		if ( mod )
			nd_ps[i] = a;
		else
			nd_psq[i] = a;
		nd_psh[i] = r;
	}
	nmv_len = 16*len;
	NEWNDV(ndv_red);
	if ( mod )
		BDY(ndv_red) = (NMV)MALLOC_ATOMIC(nmv_len*nmv_adv);
	else
		BDY(ndv_red) = (NMV)MALLOC(nmv_len*nmv_adv);
	for ( s0 = 0, i = 0; i < nd_psn; i++ ) {
		NEXTNODE(s0,s); BDY(s) = (pointer)i;
	}
	if ( s0 ) NEXT(s) = 0;
	return s0;
}

NODE nd_setup_trace(int mod,NODE f)
{
	int i,j,td,len,max;
	NODE s,s0,f0;
	unsigned int *d;
	RHist r;
	NDV a;

	nd_found = 0; nd_notfirst = 0; nd_create = 0;

	nd_psn = length(f); nd_pslen = 2*nd_psn;
	nd_psl = (int *)MALLOC(nd_pslen*sizeof(int));
	nd_ps = (NDV *)MALLOC(nd_pslen*sizeof(NDV));
	nd_psq = (NDV *)MALLOC(nd_pslen*sizeof(NDV));
	nd_psh = (RHist *)MALLOC(nd_pslen*sizeof(RHist));
	nd_bound = (unsigned int **)MALLOC(nd_pslen*sizeof(unsigned int *));
	for ( max = 0, i = 0, s = f; i < nd_psn; i++, s = NEXT(s) ) {
		nd_bound[i] = d = dp_compute_bound((DP)BDY(s));
		for ( j = 0; j < nd_nvar; j++ )
			max = MAX(d[j],max);
	}
	if ( !nd_red )
		nd_red = (RHist *)MALLOC(REDTAB_LEN*sizeof(RHist));
	bzero(nd_red,REDTAB_LEN*sizeof(RHist));

	if ( max < 2 )
		nd_bpe = 2;
	else if ( max < 4 )
		nd_bpe = 4;
	else if ( max < 64 )
		nd_bpe = 6;
	else if ( max < 256 )
		nd_bpe = 8;
	else if ( max < 65536 )
		nd_bpe = 16;
	else
		nd_bpe = 32;

	nd_setup_parameters();
	nd_free_private_storage();
	len = 0;
	for ( i = 0; i < nd_psn; i++, f = NEXT(f) ) {
		a = dptondv(mod,(DP)BDY(f)); ndv_removecont(mod,a); nd_ps[i] = a;
		a = dptondv(0,(DP)BDY(f)); ndv_removecont(0,a); nd_psq[i] = a;
		NEWRHist(r);
		len = MAX(len,LEN(a));
		SG(r) = HTD(a); TD(r) = HTD(a); ndl_copy(HDL(a),DL(r));
		nd_psh[i] = r;
	}
	nmv_len = 16*len;
	NEWNDV(ndv_red);
	BDY(ndv_red) = (NMV)MALLOC(nmv_len*nmv_adv);
	for ( s0 = 0, i = 0; i < nd_psn; i++ ) {
		NEXTNODE(s0,s); BDY(s) = (pointer)i;
	}
	if ( s0 ) NEXT(s) = 0;
	return s0;
}

void nd_gr(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp)
{
	struct order_spec ord1;
	VL fv,vv,vc;
	NODE fd,fd0,r,r0,t,x,s,xx;
	DP a,b,c;

	get_vars((Obj)f,&fv); pltovl(v,&vv);
	nd_nvar = length(vv);
	if ( ord->id )
		error("nd_gr : unsupported order");
	switch ( ord->ord.simple ) {
		case 0:
			is_rlex = 1;
			break;
		case 1:
			is_rlex = 0;
			break;
		default:
			error("nd_gr : unsupported order");
	}
	initd(ord);
	for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
		ptod(CO,vv,(P)BDY(t),&b);
		NEXTNODE(fd0,fd); BDY(fd) = (pointer)b;
	}
	if ( fd0 ) NEXT(fd) = 0;
	s = nd_setup(m,fd0);
	x = nd_gb(m,s);
#if 0
	x = nd_reduceall(x,m);
#endif
	for ( r0 = 0; x; x = NEXT(x) ) {
		NEXTNODE(r0,r); 
		if ( m ) {
			a = ndvtodp(m,nd_ps[(int)BDY(x)]);
			_dtop_mod(CO,vv,a,(P *)&BDY(r));
		} else {
			a = ndvtodp(m,nd_psq[(int)BDY(x)]);
			dtop(CO,vv,a,(P *)&BDY(r));
		}
	}
	if ( r0 ) NEXT(r) = 0;
	MKLIST(*rp,r0);
	fprintf(asir_out,"found=%d,notfirst=%d,create=%d\n",
		nd_found,nd_notfirst,nd_create);
}

void nd_gr_trace(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp)
{
	struct order_spec ord1;
	VL fv,vv,vc;
	NODE fd,fd0,r,r0,t,x,s,xx;
	DP a,b,c;

	get_vars((Obj)f,&fv); pltovl(v,&vv);
	nd_nvar = length(vv);
	if ( ord->id )
		error("nd_gr : unsupported order");
	switch ( ord->ord.simple ) {
		case 0:
			is_rlex = 1;
			break;
		case 1:
			is_rlex = 0;
			break;
		default:
			error("nd_gr : unsupported order");
	}
	initd(ord);
	for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
		ptod(CO,vv,(P)BDY(t),&c);
		if ( c ) {
			NEXTNODE(fd0,fd); BDY(fd) = (pointer)c;
		}
	}
	if ( fd0 ) NEXT(fd) = 0;
	/* setup over GF(m) */
	s = nd_setup_trace(m,fd0);
	x = nd_gb_trace(m,s);
#if 0
	x = nd_reduceall(x,m);
#endif
	for ( r0 = 0; x; x = NEXT(x) ) {
		NEXTNODE(r0,r); 
		a = ndvtodp(0,nd_psq[(int)BDY(x)]);
		dtop(CO,vv,a,(P *)&BDY(r));
	}
	if ( r0 ) NEXT(r) = 0;
	MKLIST(*rp,r0);
	fprintf(asir_out,"found=%d,notfirst=%d,create=%d\n",
		nd_found,nd_notfirst,nd_create);
}

void dltondl(int n,DL dl,unsigned int *r)
{
	unsigned int *d;
	int i;

	d = dl->d;
	bzero(r,nd_wpd*sizeof(unsigned int));
	if ( is_rlex )
		for ( i = 0; i < n; i++ )
			r[(n-1-i)/nd_epw] |= (d[i]<<((nd_epw-((n-1-i)%nd_epw)-1)*nd_bpe));
	else
		for ( i = 0; i < n; i++ )
			r[i/nd_epw] |= d[i]<<((nd_epw-(i%nd_epw)-1)*nd_bpe);
}

DL ndltodl(int n,int td,unsigned int *ndl)
{
	DL dl;
	int *d;
	int i;

	NEWDL(dl,n);
	TD(dl) = td;
	d = dl->d;
	if ( is_rlex )
		for ( i = 0; i < n; i++ )
			d[i] = (ndl[(n-1-i)/nd_epw]>>((nd_epw-((n-1-i)%nd_epw)-1)*nd_bpe))
				&((1<<nd_bpe)-1);
	else
		for ( i = 0; i < n; i++ )
			d[i] = (ndl[i/nd_epw]>>((nd_epw-(i%nd_epw)-1)*nd_bpe))
				&((1<<nd_bpe)-1);
	return dl;
}

ND dptond(int mod,DP p)
{
	ND d;
	NM m0,m;
	MP t;
	int n;

	if ( !p )
		return 0;
	n = NV(p);
	m0 = 0;
	for ( t = BDY(p); t; t = NEXT(t) ) {
		NEXTNM(m0,m);
		if ( mod )
			CM(m) = ITOS(C(t));
		else
			CQ(m) = (Q)C(t);
		TD(m) = TD(DL(t));
		dltondl(n,DL(t),DL(m));
	}
	NEXT(m) = 0;
	MKND(n,m0,d);
	NV(d) = n;
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
		if ( mod )
			C(m) = STOI(CM(t));
		else
			C(m) = (P)CQ(t);
		DL(m) = ndltodl(n,TD(t),DL(t));
	}
	NEXT(m) = 0;
	MKDP(n,m0,d);
	SG(d) = SG(p);
	return d;
}

void ndl_print(unsigned int *dl)
{
	int n;
	int i;

	n = nd_nvar;
	printf("<<");
	if ( is_rlex )
		for ( i = 0; i < n; i++ )
			printf(i==n-1?"%d":"%d,",
				(dl[(n-1-i)/nd_epw]>>((nd_epw-((n-1-i)%nd_epw)-1)*nd_bpe))
					&((1<<nd_bpe)-1));
	else
		for ( i = 0; i < n; i++ )
			printf(i==n-1?"%d":"%d,",
				(dl[i/nd_epw]>>((nd_epw-(i%nd_epw)-1)*nd_bpe))
					&((1<<nd_bpe)-1));
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

	for ( t = d; t; t = NEXT(t) ) {
		printf("%d,%d ",t->i1,t->i2);
	}
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

	if ( mod )
		nd_mul_c(mod,p,invm(HCM(p),mod));
	else {
		for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
		w = (Q *)ALLOCA(n*sizeof(Q));
		v.len = n;
		v.body = (pointer *)w;
		for ( m = BDY(p), i = 0; i < n; m = NEXT(m), i++ )
			w[i] = CQ(m);
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
	for ( m = BDY(p1), i = 0; i < n1; m = NEXT(m), i++ )
		w[i] = CQ(m);
	for ( m = BDY(p2); i < n; m = NEXT(m), i++ )
		w[i] = CQ(m);
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
		for ( m = BDY(p), i = 0; i < len; NMV_ADV(m), i++ )
			w[i] = CQ(m);
		sortbynm(w,len);
		qltozl(w,len,&dvr);
		for ( m = BDY(p), i = 0; i < len; NMV_ADV(m), i++ ) {
			divq(CQ(m),dvr,&t); CQ(m) = t;
		}
	}
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
	for ( i = 0; i < n; i++ )
		if ( r[i] )
			break;
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
	for ( i = 0; i < n; i++ )
		c[i] = q[i];
}

void nd_mul_c(int mod,ND p,int mul)
{
	NM m;
	int c,c1;

	if ( !p )
		return;
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

	if ( !p )
		return;
	for ( m = BDY(p); m; m = NEXT(m) ) {
		mulq(CQ(m),mul,&c); CQ(m) = c;
	}
}

void nd_free(ND p)
{
	NM t,s;

	if ( !p )
		return;
	t = BDY(p);
	while ( t ) {
		s = NEXT(t);
		FREENM(t);
		t = s;
	}
	FREEND(p);
}

void nd_append_red(unsigned int *d,int td,int i)
{
	RHist m,m0;
	int h;

	NEWRHist(m);
	h = ndl_hash_value(td,d);
	m->index = i;
	TD(m) = td;
	ndl_copy(d,DL(m));
	NEXT(m) = nd_red[h];
	nd_red[h] = m;
}

unsigned int *dp_compute_bound(DP p)
{
	unsigned int *d,*d1,*d2,*t;
	MP m;
	int i,l;

	if ( !p )
		return 0;
	d1 = (unsigned int *)ALLOCA(nd_nvar*sizeof(unsigned int));
	d2 = (unsigned int *)ALLOCA(nd_nvar*sizeof(unsigned int));
	m = BDY(p);
	bcopy(DL(m)->d,d1,nd_nvar*sizeof(unsigned int));
	for ( m = NEXT(BDY(p)); m; m = NEXT(m) ) {
		d = DL(m)->d;
		for ( i = 0; i < nd_nvar; i++ )
			d2[i] = d[i] > d1[i] ? d[i] : d1[i];
		t = d1; d1 = d2; d2 = t;
	}
	l = (nd_nvar+31);
	t = (unsigned int *)MALLOC_ATOMIC(l*sizeof(unsigned int));
	bzero(t,l*sizeof(unsigned int));
	bcopy(d1,t,nd_nvar*sizeof(unsigned int));
	return t;
}

unsigned int *nd_compute_bound(ND p)
{
	unsigned int *d1,*d2,*t;
	int i,l;
	NM m;

	if ( !p )
		return 0;
	d1 = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
	d2 = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
	bcopy(HDL(p),d1,nd_wpd*sizeof(unsigned int));
	for ( m = NEXT(BDY(p)); m; m = NEXT(m) ) {
		ndl_lcm(DL(m),d1,d2);
		t = d1; d1 = d2; d2 = t;
	}
	l = nd_nvar+31;
	t = (unsigned int *)MALLOC_ATOMIC(l*sizeof(unsigned int));
	bzero(t,l*sizeof(unsigned int));
	for ( i = 0; i < nd_nvar; i++ )
		t[i] = (d1[i/nd_epw]>>((nd_epw-(i%nd_epw)-1)*nd_bpe))&nd_mask0;
	return t;
}

void nd_setup_parameters() {
	int i;

	nd_epw = (sizeof(unsigned int)*8)/nd_bpe;
	nd_wpd = nd_nvar/nd_epw+(nd_nvar%nd_epw?1:0);
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
}

/* mod < 0 => realloc nd_ps and pd_psq */

ND_pairs nd_reconstruct(int mod,int trace,ND_pairs d)
{
	int i,obpe,oadv;
	NM prev_nm_free_list;
	RHist mr0,mr;
	RHist r;
	ND_pairs s0,s,t,prev_ndp_free_list;

	obpe = nd_bpe;
	oadv = nmv_adv;
	if ( obpe < 4 )
		nd_bpe = 4;
	else if ( obpe < 6 )
		nd_bpe = 6;
	else if ( obpe < 8 )
		nd_bpe = 8;
	else if ( obpe < 16 )
		nd_bpe = 16;
	else if ( obpe < 32 )
		nd_bpe = 32;
	else
		error("nd_reconstruct : exponent too large");

	nd_setup_parameters();
	prev_nm_free_list = _nm_free_list;
	prev_ndp_free_list = _ndp_free_list;
	_nm_free_list = 0;
	_ndp_free_list = 0;
	if ( mod != 0 )
		for ( i = nd_psn-1; i >= 0; i-- )
			ndv_realloc(nd_ps[i],obpe,oadv);
	if ( !mod || trace )
		for ( i = nd_psn-1; i >= 0; i-- )
			ndv_realloc(nd_psq[i],obpe,oadv);
	s0 = 0;
	for ( t = d; t; t = NEXT(t) ) {
		NEXTND_pairs(s0,s);
		s->i1 = t->i1;
		s->i2 = t->i2;
		TD(s) = TD(t);
		SG(s) = SG(t);
		ndl_dup(obpe,t->lcm,s->lcm);
	}
	for ( i = 0; i < REDTAB_LEN; i++ ) {
		for ( mr0 = 0, r = nd_red[i]; r; r = NEXT(r) ) {
			NEXTRHist(mr0,mr);
			mr->index = r->index;
			SG(mr) = SG(r);
			TD(mr) = TD(r);
			ndl_dup(obpe,DL(r),DL(mr));
		}
		if ( mr0 ) NEXT(mr) = 0;
		nd_red[i] = mr0;
	}
	for ( i = 0; i < nd_psn; i++ ) {
		NEWRHist(r); SG(r) = SG(nd_psh[i]);
		TD(r) = TD(nd_psh[i]); ndl_dup(obpe,DL(nd_psh[i]),DL(r));
		nd_psh[i] = r;
	}
	if ( s0 ) NEXT(s) = 0;
	prev_nm_free_list = 0;
	prev_ndp_free_list = 0;
	BDY(ndv_red) = (NMV)REALLOC(BDY(ndv_red),nmv_len*nmv_adv);
	GC_gcollect();
	return s0;
}

void ndl_dup(int obpe,unsigned int *d,unsigned int *r)
{
	int n,i,ei,oepw,cepw,cbpe;

	n = nd_nvar;
	oepw = (sizeof(unsigned int)*8)/obpe;
	cepw = nd_epw;
	cbpe = nd_bpe;
	for ( i = 0; i < nd_wpd; i++ )
		r[i] = 0;
	if ( is_rlex )
		for ( i = 0; i < n; i++ ) {
			ei = (d[(n-1-i)/oepw]>>((oepw-((n-1-i)%oepw)-1)*obpe))
				&((1<<obpe)-1);
			r[(n-1-i)/cepw] |= (ei<<((cepw-((n-1-i)%cepw)-1)*cbpe));
		}
	else
		for ( i = 0; i < n; i++ ) {
			ei = (d[i/oepw]>>((oepw-(i%oepw)-1)*obpe))
				&((1<<obpe)-1);
			r[i/cepw] |= (ei<<((cepw-(i%cepw)-1)*cbpe));
		}
}

void nd_realloc(ND p,int obpe)
{
	NM m,mr,mr0;

	if ( p ) {
		m = BDY(p);
		for ( mr0 = 0; m; m = NEXT(m) ) {
			NEXTNM(mr0,mr);
			CM(mr) = CM(m);
			TD(mr) = TD(m);
			ndl_dup(obpe,DL(m),DL(mr));
		}
		NEXT(mr) = 0; 
		BDY(p) = mr0;
	}
}

ND nd_copy(ND p)
{
	NM m,mr,mr0;
	int c,n,s;
	ND r;

	if ( !p )
		return 0;
	else {
		s = sizeof(struct oNM)+(nd_wpd-1)*sizeof(unsigned int);
		for ( mr0 = 0, m = BDY(p); m; m = NEXT(m) ) {
			NEXTNM(mr0,mr);
			CM(mr) = CM(m);
			TD(mr) = TD(m);
			ndl_copy(DL(m),DL(mr));
		}
		NEXT(mr) = 0; 
		MKND(NV(p),mr0,r);
		SG(r) = SG(p);
		return r;
	}
}

int nd_sp(int mod,ND_pairs p,ND *rp)
{
	NM m;
	NDV p1,p2;
	ND t1,t2;
	unsigned int *lcm;
	int td;

	if ( mod ) {
		p1 = nd_ps[p->i1]; p2 = nd_ps[p->i2];
	} else {
		p1 = nd_psq[p->i1]; p2 = nd_psq[p->i2];
	}
	lcm = p->lcm;
	td = TD(p);
	NEWNM(m);
	CQ(m) = HCQ(p2);
	TD(m) = td-HTD(p1); ndl_sub(lcm,HDL(p1),DL(m));
	if ( ndl_check_bound2(p->i1,DL(m)) )
		return 0;
	t1 = ndv_mul_nm_create(mod,p1,m);
	if ( mod )
		CM(m) = mod-HCM(p1); 
	else
		chsgnq(HCQ(p1),&CQ(m));
	TD(m) = td-HTD(p2); ndl_sub(lcm,HDL(p2),DL(m));
	if ( ndl_check_bound2(p->i2,DL(m)) ) {
		nd_free(t1);
		return 0;
	}
	ndv_mul_nm(mod,p2,m,ndv_red);
	FREENM(m);
	*rp = ndv_add(mod,t1,ndv_red);
	return 1;
}

void ndv_mul_c(int mod,NDV p,int mul)
{
	NMV m;
	int c,c1,len,i;

	if ( !p )
		return;
	len = LEN(p);
	for ( m = BDY(p), i = 0; i < len; i++, NMV_ADV(m) ) {
		c1 = CM(m);
		DMAR(c1,mul,0,mod,c);
		CM(m) = c;
	}
}

void ndv_mul_c_q(NDV p,Q mul)
{
	NMV m;
	Q c;
	int len,i;

	if ( !p )
		return;
	len = LEN(p);
	for ( m = BDY(p), i = 0; i < len; i++, NMV_ADV(m) ) {
		mulq(CQ(m),mul,&c); CQ(m) = c;
	}
}

void ndv_mul_nm(int mod,NDV p,NM m0,NDV r)
{
	NMV m,mr,mr0;
	unsigned int *d,*dt,*dm;
	int c,n,td,i,c1,c2,len;
	Q q;

	if ( !p )
		/* XXX */
		LEN(r) = 0;
	else {
		n = NV(p); m = BDY(p); len = LEN(p);
		d = DL(m0); td = TD(m0);
		mr = BDY(r);
		if ( mod ) {
			c = CM(m0);
			for ( ; len > 0; len--, NMV_ADV(m), NMV_ADV(mr) ) {
				c1 = CM(m); DMAR(c1,c,0,mod,c2); CM(mr) = c2;
				TD(mr) = TD(m)+td; ndl_add(DL(m),d,DL(mr));
			}
		} else {
			q = CQ(m0);
			for ( ; len > 0; len--, NMV_ADV(m), NMV_ADV(mr) ) {
				mulq(CQ(m),q,&CQ(mr));
				TD(mr) = TD(m)+td; ndl_add(DL(m),d,DL(mr));
			}
		}
		NV(r) = NV(p);
		LEN(r) = LEN(p);
		SG(r) = SG(p) + td;
	}
}

ND ndv_mul_nm_create(int mod,NDV p,NM m0)
{
	NM mr,mr0;
	NMV m;
	unsigned int *d,*dt,*dm;
	int c,n,td,i,c1,c2,len;
	Q q;
	ND r;

	if ( !p )
		return 0;
	else {
		n = NV(p); m = BDY(p);
		d = DL(m0); td = TD(m0); 
		len = LEN(p);
		mr0 = 0;
		if ( mod ) {
			c = CM(m0);
			for ( i = 0; i < len; i++, NMV_ADV(m) ) {
				NEXTNM(mr0,mr);
				c1 = CM(m);
				DMAR(c1,c,0,mod,c2);
				CM(mr) = c2;
				TD(mr) = TD(m)+td;
				ndl_add(DL(m),d,DL(mr));
			}
		} else {
			q = CQ(m0);
			for ( i = 0; i < len; i++, NMV_ADV(m) ) {
				NEXTNM(mr0,mr);
				mulq(CQ(m),q,&CQ(mr));
				TD(mr) = TD(m)+td;
				ndl_add(DL(m),d,DL(mr));
			}
		}
		NEXT(mr) = 0; 
		MKND(NV(p),mr0,r);
		SG(r) = SG(p) + td;
		return r;
	}
}

ND ndv_add(int mod,ND p1,NDV p2)
{
	register NM prev,cur,new;
	int c,c1,c2,t,td,td2,mul,len,i;
	NM head;
	unsigned int *d;
	NMV m2;
	Q q;

	if ( !p1 )
		return 0;
	else if ( !mod )
		return ndv_add_q(p1,p2);
	else {
		prev = 0; head = cur = BDY(p1);
		NEWNM(new); len = LEN(p2);
		for ( m2 = BDY(p2), i = 0; cur && i < len; ) {
			td2 = TD(new) = TD(m2);
			if ( TD(cur) > td2 ) {
				prev = cur; cur = NEXT(cur);
				continue;
			} else if ( TD(cur) < td2 ) c = -1;
			else if ( nd_wpd == 1 ) {
				if ( DL(cur)[0] > DL(m2)[0] ) c = is_rlex ? -1 : 1;
				else if ( DL(cur)[0] < DL(m2)[0] ) c = is_rlex ? 1 : -1;
				else c = 0;
			}
			else c = ndl_compare(DL(cur),DL(m2));
			switch ( c ) {
				case 0:
					t = CM(m2)+CM(cur)-mod;
					if ( t < 0 ) t += mod;
					if ( t ) CM(cur) = t;
					else if ( !prev ) {
						head = NEXT(cur); FREENM(cur); cur = head;
					} else {
						NEXT(prev) = NEXT(cur); FREENM(cur); cur = NEXT(prev);
					}
					NMV_ADV(m2); i++;
					break;
				case 1:
					prev = cur; cur = NEXT(cur);
					break;
				case -1:
					ndl_copy(DL(m2),DL(new));
					CQ(new) = CQ(m2);
					if ( !prev ) {
						/* cur = head */
						prev = new; NEXT(prev) = head; head = prev;
					} else {
						NEXT(prev) = new; NEXT(new) = cur; prev = new;
					}
					NEWNM(new); NMV_ADV(m2); i++;
					break;
			}
		}
		for ( ; i < len; i++, NMV_ADV(m2) ) {
			td2 = TD(new) = TD(m2); CQ(new) = CQ(m2); ndl_copy(DL(m2),DL(new));
			if ( !prev ) {
				prev = new; NEXT(prev) = 0; head = prev;
			} else {
				NEXT(prev) = new; NEXT(new) = 0; prev = new;
			}
			NEWNM(new);
		}
		FREENM(new);
		if ( head ) {
			BDY(p1) = head; SG(p1) = MAX(SG(p1),SG(p2));
			return p1;
		} else {
			FREEND(p1);
			return 0;
		}
			
	}
}

ND ndv_add_q(ND p1,NDV p2)
{
	register NM prev,cur,new;
	int c,c1,c2,t,td,td2,mul,len,i;
	NM head;
	unsigned int *d;
	NMV m2;
	Q q;

	if ( !p1 )
		return 0;
	else {
		prev = 0; head = cur = BDY(p1);
		NEWNM(new); len = LEN(p2);
		for ( m2 = BDY(p2), i = 0; cur && i < len; ) {
			td2 = TD(new) = TD(m2);
			if ( TD(cur) > td2 ) {
				prev = cur; cur = NEXT(cur);
				continue;
			} else if ( TD(cur) < td2 ) c = -1;
			else if ( nd_wpd == 1 ) {
				if ( DL(cur)[0] > DL(m2)[0] ) c = is_rlex ? -1 : 1;
				else if ( DL(cur)[0] < DL(m2)[0] ) c = is_rlex ? 1 : -1;
				else c = 0;
			}
			else c = ndl_compare(DL(cur),DL(m2));
			switch ( c ) {
				case 0:
					addq(CQ(cur),CQ(m2),&q);
					if ( q )
						CQ(cur) = q;
					else if ( !prev ) {
						head = NEXT(cur); FREENM(cur); cur = head;
					} else {
						NEXT(prev) = NEXT(cur); FREENM(cur); cur = NEXT(prev);
					}
					NMV_ADV(m2); i++;
					break;
				case 1:
					prev = cur; cur = NEXT(cur);
					break;
				case -1:
					ndl_copy(DL(m2),DL(new));
					CQ(new) = CQ(m2);
					if ( !prev ) {
						/* cur = head */
						prev = new; NEXT(prev) = head; head = prev;
					} else {
						NEXT(prev) = new; NEXT(new) = cur; prev = new;
					}
					NEWNM(new); NMV_ADV(m2); i++;
					break;
			}
		}
		for ( ; i < len; i++, NMV_ADV(m2) ) {
			td2 = TD(new) = TD(m2); CQ(new) = CQ(m2); ndl_copy(DL(m2),DL(new));
			if ( !prev ) {
				prev = new; NEXT(prev) = 0; head = prev;
			} else {
				NEXT(prev) = new; NEXT(new) = 0; prev = new;
			}
			NEWNM(new);
		}
		FREENM(new);
		if ( head ) {
			BDY(p1) = head; SG(p1) = MAX(SG(p1),SG(p2));
			return p1;
		} else {
			FREEND(p1);
			return 0;
		}
			
	}
}

void ndv_realloc(NDV p,int obpe,int oadv)
{
	NMV m,mr,mr0,t;
	int len,i,k;

#define NMV_OPREV(m) (m = (NMV)(((char *)m)-oadv))
#define NMV_PREV(m) (m = (NMV)(((char *)m)-nmv_adv))

	if ( p ) {
		m = BDY(p); len = LEN(p);
		if ( nmv_adv > oadv )
			mr0 = (NMV)REALLOC(BDY(p),len*nmv_adv);
		else
			mr0 = BDY(p);
		m = (NMV)((char *)mr0+(len-1)*oadv);
		mr = (NMV)((char *)mr0+(len-1)*nmv_adv);
		t = (NMV)ALLOCA(nmv_adv);
		for ( i = 0; i < len; i++, NMV_OPREV(m), NMV_PREV(mr) ) {
			CQ(t) = CQ(m);
			TD(t) = TD(m);
			for ( k = 0; k < nd_wpd; k++ ) DL(t)[k] = 0;
			ndl_dup(obpe,DL(m),DL(t));
			CQ(mr) = CQ(t);
			TD(mr) = TD(t);
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

	if ( !p )
		return 0;
	len = nd_length(p);	
	if ( mod )
		m0 = m = (NMV)MALLOC_ATOMIC(len*nmv_adv);
	else
		m0 = m = (NMV)MALLOC(len*nmv_adv);
	for ( t = BDY(p), i = 0; t; t = NEXT(t), i++, NMV_ADV(m) ) {
		TD(m) = TD(t);
		ndl_copy(DL(t),DL(m));
		CQ(m) = CQ(t);
	}
	MKNDV(NV(p),m0,len,d);
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

	if ( !p )
		return 0;
	for ( t = BDY(p), l = 0; t; t = NEXT(t), l++ );
	if ( mod ) {
		_dp_mod(p,mod,0,&q); p = q;
		m0 = m = (NMV)MALLOC_ATOMIC(l*nmv_adv);
	} else
		m0 = m = (NMV)MALLOC(l*nmv_adv);
	n = NV(p);
	for ( t = BDY(p), i = 0; i < l; i++, t = NEXT(t), NMV_ADV(m) ) {
		if ( mod )
			CM(m) = ITOS(C(t));
		else
			CQ(m) = (Q)C(t);
		TD(m) = TD(DL(t));
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

	if ( !p )
		return 0;
	m0 = 0;
	len = LEN(p);
	n = NV(p);
	for ( t = BDY(p), i = 0; i < len; i++, NMV_ADV(t) ) {
		NEXTMP(m0,m);
		if ( mod ) 
			C(m) = STOI(CM(t));
		else
			C(m) = (P)CQ(t);
		DL(m) = ndltodl(n,TD(t),DL(t));
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
    
	if ( !p )
		printf("0\n");
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
    
	if ( !p )
		printf("0\n");
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
