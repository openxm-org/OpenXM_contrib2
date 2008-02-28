/* $OpenXM: OpenXM_contrib2/asir2000/engine/nd.c,v 1.153 2007/04/30 02:11:12 noro Exp $ */

#include "nd.h"

int diag_period = 6;
int (*ndl_compare_function)(UINT *a1,UINT *a2);
int nd_dcomp;
NM _nm_free_list;
ND _nd_free_list;
ND_pairs _ndp_free_list;
NODE nd_hcf;

static NODE nd_subst;
static VL nd_vc;
static int nd_ntrans;
static int nd_nalg;
#if 0
static int ndv_alloc;
#endif
#if 1
static int nd_f4_nsp=0x7fffffff;
#else
static int nd_f4_nsp=50;
#endif
static double nd_scale=2;
static UINT **nd_bound;
static struct order_spec *nd_ord;
static EPOS nd_epos;
static BlockMask nd_blockmask;
static int nd_nvar;
static int nd_isrlex;
static int nd_epw,nd_bpe,nd_wpd,nd_exporigin;
static UINT nd_mask[32];
static UINT nd_mask0,nd_mask1;

static NDV *nd_ps;
static NDV *nd_ps_trace;
static RHist *nd_psh;
static int nd_psn,nd_pslen;
static RHist *nd_red;
static int *nd_work_vector;
static int **nd_matrix;
static int nd_matrix_len;
static struct weight_or_block *nd_worb;
static int nd_worb_len;
static int nd_found,nd_create,nd_notfirst;
static int nmv_adv;
static int nd_demand;

NumberField get_numberfield();
UINT *nd_det_compute_bound(NDV **dm,int n,int j);
void nd_det_reconstruct(NDV **dm,int n,int j,NDV d);
void nd_heu_nezgcdnpz(VL vl,P *pl,int m,int full,P *pr);
int nd_monic(int m,ND *p);
NDV plain_vect_to_ndv_q(Q *mat,int col,UINT *s0vect);

extern int Denominator,DP_Multiple;

void nd_free_private_storage()
{
	_nm_free_list = 0;
	_ndp_free_list = 0;
#if 0
	GC_gcollect();
#endif
}

void _NM_alloc()
{
	NM p;
	int i;

	for ( i = 0; i < 1024; i++ ) {
		p = (NM)GC_malloc(sizeof(struct oNM)+(nd_wpd-1)*sizeof(UINT));
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
			+(nd_wpd-1)*sizeof(UINT));
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

INLINE int ndl_reducible(UINT *d1,UINT *d2)
{
	UINT u1,u2;
	int i,j;

	if ( TD(d1) < TD(d2) ) return 0;
#if USE_UNROLL
	switch ( nd_bpe ) {
		case 3:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0x38000000) < (u2&0x38000000) ) return 0;
				if ( (u1& 0x7000000) < (u2& 0x7000000) ) return 0;
				if ( (u1&  0xe00000) < (u2&  0xe00000) ) return 0;
				if ( (u1&  0x1c0000) < (u2&  0x1c0000) ) return 0;
				if ( (u1&   0x38000) < (u2&   0x38000) ) return 0;
				if ( (u1&    0x7000) < (u2&    0x7000) ) return 0;
				if ( (u1&     0xe00) < (u2&     0xe00) ) return 0;
				if ( (u1&     0x1c0) < (u2&     0x1c0) ) return 0;
				if ( (u1&      0x38) < (u2&      0x38) ) return 0;
				if ( (u1&       0x7) < (u2&       0x7) ) return 0;
			}
			return 1;
			break;
		case 4:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xf0000000) < (u2&0xf0000000) ) return 0;
				if ( (u1& 0xf000000) < (u2& 0xf000000) ) return 0;
				if ( (u1&  0xf00000) < (u2&  0xf00000) ) return 0;
				if ( (u1&   0xf0000) < (u2&   0xf0000) ) return 0;
				if ( (u1&    0xf000) < (u2&    0xf000) ) return 0;
				if ( (u1&     0xf00) < (u2&     0xf00) ) return 0;
				if ( (u1&      0xf0) < (u2&      0xf0) ) return 0;
				if ( (u1&       0xf) < (u2&       0xf) ) return 0;
			}
			return 1;
			break;
		case 6:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0x3f000000) < (u2&0x3f000000) ) return 0;
				if ( (u1&  0xfc0000) < (u2&  0xfc0000) ) return 0;
				if ( (u1&   0x3f000) < (u2&   0x3f000) ) return 0;
				if ( (u1&     0xfc0) < (u2&     0xfc0) ) return 0;
				if ( (u1&      0x3f) < (u2&      0x3f) ) return 0;
			}
			return 1;
			break;
		case 8:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xff000000) < (u2&0xff000000) ) return 0;
				if ( (u1&  0xff0000) < (u2&  0xff0000) ) return 0;
				if ( (u1&    0xff00) < (u2&    0xff00) ) return 0;
				if ( (u1&      0xff) < (u2&      0xff) ) return 0;
			}
			return 1;
			break;
		case 16:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				if ( (u1&0xffff0000) < (u2&0xffff0000) ) return 0;
				if ( (u1&    0xffff) < (u2&    0xffff) ) return 0;
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
#else
	for ( i = nd_exporigin; i < nd_wpd; i++ ) {
		u1 = d1[i]; u2 = d2[i];
		for ( j = 0; j < nd_epw; j++ )
			if ( (u1&nd_mask[j]) < (u2&nd_mask[j]) ) return 0;
	}
	return 1;
#endif
}

/*
 * If the current order is a block order,
 * then the last block is length 1 and contains
 * the homo variable. Otherwise, the original
 * order is either 0 or 2.
 */

void ndl_homogenize(UINT *d,UINT *r,int obpe,EPOS oepos,int weight)
{
	int w,i,e,n,omask0;

	omask0 = obpe==32?0xffffffff:((1<<obpe)-1);
	n = nd_nvar-1;
	ndl_zero(r);
	for ( i = 0; i < n; i++ ) {
		e = GET_EXP_OLD(d,i);
		PUT_EXP(r,i,e);
	}
	w = TD(d);
	PUT_EXP(r,nd_nvar-1,weight-w);
	TD(r) = weight;
	if ( nd_blockmask ) ndl_weight_mask(r);
}

void ndl_dehomogenize(UINT *d)
{
	UINT mask;
	UINT h;
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

void ndl_lcm(UINT *d1,unsigned *d2,UINT *d)
{
	UINT t1,t2,u,u1,u2;
	int i,j,l;

#if USE_UNROLL
	switch ( nd_bpe ) {
		case 3:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0x38000000); t2 = (u2&0x38000000); u = t1>t2?t1:t2;
				t1 = (u1& 0x7000000); t2 = (u2& 0x7000000); u |= t1>t2?t1:t2;
				t1 = (u1&  0xe00000); t2 = (u2&  0xe00000); u |= t1>t2?t1:t2;
				t1 = (u1&  0x1c0000); t2 = (u2&  0x1c0000); u |= t1>t2?t1:t2;
				t1 = (u1&   0x38000); t2 = (u2&   0x38000); u |= t1>t2?t1:t2;
				t1 = (u1&    0x7000); t2 = (u2&    0x7000); u |= t1>t2?t1:t2;
				t1 = (u1&     0xe00); t2 = (u2&     0xe00); u |= t1>t2?t1:t2;
				t1 = (u1&     0x1c0); t2 = (u2&     0x1c0); u |= t1>t2?t1:t2;
				t1 = (u1&      0x38); t2 = (u2&      0x38); u |= t1>t2?t1:t2;
				t1 = (u1&       0x7); t2 = (u2&       0x7); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 4:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xf0000000); t2 = (u2&0xf0000000); u = t1>t2?t1:t2;
				t1 = (u1& 0xf000000); t2 = (u2& 0xf000000); u |= t1>t2?t1:t2;
				t1 = (u1&  0xf00000); t2 = (u2&  0xf00000); u |= t1>t2?t1:t2;
				t1 = (u1&   0xf0000); t2 = (u2&   0xf0000); u |= t1>t2?t1:t2;
				t1 = (u1&    0xf000); t2 = (u2&    0xf000); u |= t1>t2?t1:t2;
				t1 = (u1&     0xf00); t2 = (u2&     0xf00); u |= t1>t2?t1:t2;
				t1 = (u1&      0xf0); t2 = (u2&      0xf0); u |= t1>t2?t1:t2;
				t1 = (u1&       0xf); t2 = (u2&       0xf); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 6:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0x3f000000); t2 = (u2&0x3f000000); u = t1>t2?t1:t2;
				t1 = (u1&  0xfc0000); t2 = (u2&  0xfc0000); u |= t1>t2?t1:t2;
				t1 = (u1&   0x3f000); t2 = (u2&   0x3f000); u |= t1>t2?t1:t2;
				t1 = (u1&     0xfc0); t2 = (u2&     0xfc0); u |= t1>t2?t1:t2;
				t1 = (u1&      0x3f); t2 = (u2&      0x3f); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 8:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xff000000); t2 = (u2&0xff000000); u = t1>t2?t1:t2;
				t1 = (u1&  0xff0000); t2 = (u2&  0xff0000); u |= t1>t2?t1:t2;
				t1 = (u1&    0xff00); t2 = (u2&    0xff00); u |= t1>t2?t1:t2;
				t1 = (u1&      0xff); t2 = (u2&      0xff); u |= t1>t2?t1:t2;
				d[i] = u;
			}
			break;
		case 16:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = (u1&0xffff0000); t2 = (u2&0xffff0000); u = t1>t2?t1:t2;
				t1 = (u1&    0xffff); t2 = (u2&    0xffff); u |= t1>t2?t1:t2;
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
#else
	for ( i = nd_exporigin; i < nd_wpd; i++ ) {
		u1 = d1[i]; u2 = d2[i]; 
		for ( j = 0, u = 0; j < nd_epw; j++ ) {
			t1 = (u1&nd_mask[j]); t2 = (u2&nd_mask[j]); u |= t1>t2?t1:t2;
		}
		d[i] = u;
	}
#endif
	TD(d) = ndl_weight(d);
	if ( nd_blockmask ) ndl_weight_mask(d);
}

int ndl_weight(UINT *d)
{
	UINT t,u;
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

void ndl_weight_mask(UINT *d)
{
	UINT t,u;
	UINT *mask;
	int i,j,k,l;

	l = nd_blockmask->n;
	for ( k = 0; k < l; k++ ) {
		mask = nd_blockmask->mask[k];
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
		d[k+1] = t;
	}
}

int ndl_lex_compare(UINT *d1,UINT *d2)
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

int ndl_block_compare(UINT *d1,UINT *d2)
{
	int i,l,j,ord_o,ord_l;
	struct order_pair *op;
	UINT t1,t2,m;
	UINT *mask;

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

int ndl_matrix_compare(UINT *d1,UINT *d2)
{
	int i,j,s;
	int *v;

	for ( j = 0; j < nd_nvar; j++ )
		nd_work_vector[j] = GET_EXP(d1,j)-GET_EXP(d2,j);
	for ( i = 0; i < nd_matrix_len; i++ ) {
		v = nd_matrix[i];
		for ( j = 0, s = 0; j < nd_nvar; j++ )
			s += v[j]*nd_work_vector[j];
		if ( s > 0 ) return 1;
		else if ( s < 0 ) return -1;
	}
	return 0;
}

int ndl_composite_compare(UINT *d1,UINT *d2)
{
	int i,j,s,start,end,len,o;
	int *v;
	struct sparse_weight *sw;

	for ( j = 0; j < nd_nvar; j++ )
		nd_work_vector[j] = GET_EXP(d1,j)-GET_EXP(d2,j);
	for ( i = 0; i < nd_worb_len; i++ ) {
		len = nd_worb[i].length;
		switch ( nd_worb[i].type ) {
			case IS_DENSE_WEIGHT:
				v = nd_worb[i].body.dense_weight;
				for ( j = 0, s = 0; j < len; j++ )
					s += v[j]*nd_work_vector[j];
				if ( s > 0 ) return 1;
				else if ( s < 0 ) return -1;
				break;
			case IS_SPARSE_WEIGHT:
				sw = nd_worb[i].body.sparse_weight;
				for ( j = 0, s = 0; j < len; j++ )
					s += sw[j].value*nd_work_vector[sw[j].pos];
				if ( s > 0 ) return 1;
				else if ( s < 0 ) return -1;
				break;
			case IS_BLOCK:
				o = nd_worb[i].body.block.order;
				start = nd_worb[i].body.block.start;
				switch ( o ) {
					case 0:
						end = start+len;
						for ( j = start, s = 0; j < end; j++ )
							s += MUL_WEIGHT(nd_work_vector[j],j);
						if ( s > 0 ) return 1;
						else if ( s < 0 ) return -1;
						for ( j = end-1; j >= start; j-- )
							if ( nd_work_vector[j] < 0 ) return 1;
							else if ( nd_work_vector[j] > 0 ) return -1;
						break;
					case 1:
						end = start+len;
						for ( j = start, s = 0; j < end; j++ )
							s += MUL_WEIGHT(nd_work_vector[j],j);
						if ( s > 0 ) return 1;
						else if ( s < 0 ) return -1;
						for ( j = start; j < end; j++ )
							if ( nd_work_vector[j] > 0 ) return 1;
							else if ( nd_work_vector[j] < 0 ) return -1;
						break;
					case 2:
						for ( j = start; j < end; j++ )
							if ( nd_work_vector[j] > 0 ) return 1;
							else if ( nd_work_vector[j] < 0 ) return -1;
						break;
				}
				break;
		}
	}
	return 0;
}

/* TDH -> WW -> TD-> RL */

int ndl_ww_lex_compare(UINT *d1,UINT *d2)
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

INLINE int ndl_equal(UINT *d1,UINT *d2)
{
	int i;

	switch ( nd_wpd ) {
		case 2:
			if ( TD(d2) != TD(d1) ) return 0;
			if ( d2[1] != d1[1] ) return 0;
			return 1;
			break;
		case 3:
			if ( TD(d2) != TD(d1) ) return 0;
			if ( d2[1] != d1[1] ) return 0;
			if ( d2[2] != d1[2] ) return 0;
			return 1;
			break;
		default:
			for ( i = 0; i < nd_wpd; i++ )
				if ( *d1++ != *d2++ ) return 0;
			return 1;
			break;
	}
}

INLINE void ndl_copy(UINT *d1,UINT *d2)
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

INLINE void ndl_zero(UINT *d)
{
	int i;
	for ( i = 0; i < nd_wpd; i++ ) d[i] = 0;
}

INLINE void ndl_add(UINT *d1,UINT *d2,UINT *d)
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
INLINE void ndl_addto(UINT *d1,UINT *d2)
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

INLINE void ndl_sub(UINT *d1,UINT *d2,UINT *d)
{
	int i;

	for ( i = 0; i < nd_wpd; i++ ) d[i] = d1[i]-d2[i];
}

int ndl_disjoint(UINT *d1,UINT *d2)
{
	UINT t1,t2,u,u1,u2;
	int i,j;

#if USE_UNROLL
	switch ( nd_bpe ) {
		case 3:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0x38000000; t2 = u2&0x38000000; if ( t1&&t2 ) return 0;
				t1 = u1& 0x7000000; t2 = u2& 0x7000000; if ( t1&&t2 ) return 0;
				t1 = u1&  0xe00000; t2 = u2&  0xe00000; if ( t1&&t2 ) return 0;
				t1 = u1&  0x1c0000; t2 = u2&  0x1c0000; if ( t1&&t2 ) return 0;
				t1 = u1&   0x38000; t2 = u2&   0x38000; if ( t1&&t2 ) return 0;
				t1 = u1&    0x7000; t2 = u2&    0x7000; if ( t1&&t2 ) return 0;
				t1 = u1&     0xe00; t2 = u2&     0xe00; if ( t1&&t2 ) return 0;
				t1 = u1&     0x1c0; t2 = u2&     0x1c0; if ( t1&&t2 ) return 0;
				t1 = u1&      0x38; t2 = u2&      0x38; if ( t1&&t2 ) return 0;
				t1 = u1&       0x7; t2 = u2&       0x7; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 4:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xf0000000; t2 = u2&0xf0000000; if ( t1&&t2 ) return 0;
				t1 = u1& 0xf000000; t2 = u2& 0xf000000; if ( t1&&t2 ) return 0;
				t1 = u1&  0xf00000; t2 = u2&  0xf00000; if ( t1&&t2 ) return 0;
				t1 = u1&   0xf0000; t2 = u2&   0xf0000; if ( t1&&t2 ) return 0;
				t1 = u1&    0xf000; t2 = u2&    0xf000; if ( t1&&t2 ) return 0;
				t1 = u1&     0xf00; t2 = u2&     0xf00; if ( t1&&t2 ) return 0;
				t1 = u1&      0xf0; t2 = u2&      0xf0; if ( t1&&t2 ) return 0;
				t1 = u1&       0xf; t2 = u2&       0xf; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 6:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0x3f000000; t2 = u2&0x3f000000; if ( t1&&t2 ) return 0;
				t1 = u1&  0xfc0000; t2 = u2&  0xfc0000; if ( t1&&t2 ) return 0;
				t1 = u1&   0x3f000; t2 = u2&   0x3f000; if ( t1&&t2 ) return 0;
				t1 = u1&     0xfc0; t2 = u2&     0xfc0; if ( t1&&t2 ) return 0;
				t1 = u1&      0x3f; t2 = u2&      0x3f; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 8:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xff000000; t2 = u2&0xff000000; if ( t1&&t2 ) return 0;
				t1 = u1&  0xff0000; t2 = u2&  0xff0000; if ( t1&&t2 ) return 0;
				t1 = u1&    0xff00; t2 = u2&    0xff00; if ( t1&&t2 ) return 0;
				t1 = u1&      0xff; t2 = u2&      0xff; if ( t1&&t2 ) return 0;
			}
			return 1;
			break;
		case 16:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u1 = d1[i]; u2 = d2[i];
				t1 = u1&0xffff0000; t2 = u2&0xffff0000; if ( t1&&t2 ) return 0;
				t1 = u1&    0xffff; t2 = u2&    0xffff; if ( t1&&t2 ) return 0;
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
#else
	for ( i = nd_exporigin; i < nd_wpd; i++ ) {
		u1 = d1[i]; u2 = d2[i];
		for ( j = 0; j < nd_epw; j++ ) {
			if ( (u1&nd_mask0) && (u2&nd_mask0) ) return 0;
			u1 >>= nd_bpe; u2 >>= nd_bpe;
		}
	}
	return 1;
#endif
}

int ndl_check_bound(UINT *d1,UINT *d2)
{
	UINT u2;
	int i,j,ind,k;

	ind = 0;
#if USE_UNROLL
	switch ( nd_bpe ) {
		case 3:
			for ( i = nd_exporigin; i < nd_wpd; i++ ) {
				u2 = d2[i];
				if ( d1[ind++]+((u2>>27)&0x7) >= 0x8 ) return 1;
				if ( d1[ind++]+((u2>>24)&0x7) >= 0x8 ) return 1;
				if ( d1[ind++]+((u2>>21)&0x7) >= 0x8 ) return 1;
				if ( d1[ind++]+((u2>>18)&0x7) >= 0x8 ) return 1;
				if ( d1[ind++]+((u2>>15)&0x7) >= 0x8 ) return 1;
				if ( d1[ind++]+((u2>>12)&0x7) >= 0x8 ) return 1;
				if ( d1[ind++]+((u2>>9)&0x7) >= 0x8 ) return 1;
				if ( d1[ind++]+((u2>>6)&0x7) >= 0x8 ) return 1;
				if ( d1[ind++]+((u2>>3)&0x7) >= 0x8 ) return 1;
				if ( d1[ind++]+(u2&0x7) >= 0x8 ) return 1;
			}
			return 0;
			break;
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
#else
	for ( i = nd_exporigin; i < nd_wpd; i++ ) {
		u2 = d2[i];
		k = (nd_epw-1)*nd_bpe;
		for ( j = 0; j < nd_epw; j++, k -= nd_bpe )
			if ( d1[ind++]+((u2>>k)&nd_mask0) > nd_mask0 ) return 1;
	}
	return 0;
#endif
}

int ndl_check_bound2(int index,UINT *d2)
{
	return ndl_check_bound(nd_bound[index],d2);
}

INLINE int ndl_hash_value(UINT *d)
{
	int i;
	int r;

	r = 0;
	for ( i = 0; i < nd_wpd; i++ )	
		r = ((r<<16)+d[i])%REDTAB_LEN;
	return r;
}

INLINE int ndl_find_reducer(UINT *dg)
{
	RHist r;
	int d,k,i;

	d = ndl_hash_value(dg);
	for ( r = nd_red[d], k = 0; r; r = NEXT(r), k++ ) {
		if ( ndl_equal(dg,DL(r)) ) {
			if ( k > 0 ) nd_notfirst++;
			nd_found++;
			return r->index;
		}
	}
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

ND nd_merge(ND p1,ND p2)
{
	int n,c;
	int t,can,td1,td2;
	ND r;
	NM m1,m2,mr0,mr,s;

	if ( !p1 ) return p2;
	else if ( !p2 ) return p1;
	else {
		can = 0;
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			c = DL_COMPARE(DL(m1),DL(m2));
			switch ( c ) {
				case 0:
					s = m1; m1 = NEXT(m1);
					can++; NEXTNM2(mr0,mr,s);
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

ND nd_add(int mod,ND p1,ND p2)
{
	int n,c;
	int t,can,td1,td2;
	ND r;
	NM m1,m2,mr0,mr,s;

	if ( !p1 ) return p2;
	else if ( !p2 ) return p1;
	else if ( mod == -1 ) return nd_add_sf(p1,p2);
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

/* XXX on opteron, the inlined manipulation of destructive additon of
 * two NM seems to make gcc optimizer get confused, so the part is
 * done in a function.
 */

int nm_destructive_add_q(NM *m1,NM *m2,NM *mr0,NM *mr)
{
	NM s;
	P t;
	int can;

	addp(nd_vc,CP(*m1),CP(*m2),&t);
	s = *m1; *m1 = NEXT(*m1);
	if ( t ) {
		can = 1; NEXTNM2(*mr0,*mr,s); CP(*mr) = (t);
	} else {
		can = 2; FREENM(s);
	}
	s = *m2; *m2 = NEXT(*m2); FREENM(s);
	return can;
}

ND nd_add_q(ND p1,ND p2)
{
	int n,c,can;
	ND r;
	NM m1,m2,mr0,mr,s;
	P t;

	if ( !p1 ) return p2;
	else if ( !p2 ) return p1;
	else {
		can = 0;
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			c = DL_COMPARE(DL(m1),DL(m2));
			switch ( c ) {
				case 0:
#if defined(__x86_64__)
					can += nm_destructive_add_q(&m1,&m2,&mr0,&mr);
#else
					addp(nd_vc,CP(m1),CP(m2),&t);
					s = m1; m1 = NEXT(m1);
					if ( t ) {
						can++; NEXTNM2(mr0,mr,s); CP(mr) = (t);
					} else {
						can += 2; FREENM(s);
					}
					s = m2; m2 = NEXT(m2); FREENM(s);
#endif
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

ND nd_add_sf(ND p1,ND p2)
{
	int n,c,can;
	ND r;
	NM m1,m2,mr0,mr,s;
	int t;

	if ( !p1 ) return p2;
	else if ( !p2 ) return p1;
	else {
		can = 0;
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; ) {
			c = DL_COMPARE(DL(m1),DL(m2));
			switch ( c ) {
				case 0:
					t = _addsf(CM(m1),CM(m2));
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

ND nd_reduce2(int mod,ND d,ND g,NDV p,NM mul,NDC dn)
{
	int c,c1,c2;
	Q cg,cred,gcd,tq;
	P cgp,credp,gcdp;
	Obj tr,tr1;

	if ( mod == -1 )
		CM(mul) = _mulsf(_invsf(HCM(p)),_chsgnsf(HCM(g)));
	else if ( mod ) {
		c1 = invm(HCM(p),mod); c2 = mod-HCM(g);
		DMAR(c1,c2,0,mod,c); CM(mul) = c;
	} else if ( nd_vc ) {
		ezgcdpz(nd_vc,HCP(g),HCP(p),&gcdp);
		divsp(nd_vc,HCP(g),gcdp,&cgp); divsp(nd_vc,HCP(p),gcdp,&credp);
		chsgnp(cgp,&CP(mul));
		nd_mul_c_q(d,credp); nd_mul_c_q(g,credp);
		if ( dn ) {
			mulr(nd_vc,(Obj)dn->r,(Obj)credp,&tr);
			reductr(nd_vc,tr,&tr1); dn->r = (R)tr1;
		}
	} else {
		igcd_cofactor(HCQ(g),HCQ(p),&gcd,&cg,&cred);
		chsgnq(cg,&CQ(mul));
		nd_mul_c_q(d,(P)cred); nd_mul_c_q(g,(P)cred);
		if ( dn ) {
			mulq(dn->z,cred,&tq); dn->z = tq;
		}
	}
	return nd_add(mod,g,ndv_mul_nm(mod,mul,p));
}

/* ret=1 : success, ret=0 : overflow */
int nd_nf(int mod,ND d,ND g,NDV *ps,int full,NDC dn,ND *rp)
{
	NM m,mrd,tail;
	NM mul;
	int n,sugar,psugar,sugar0,stat,index;
	int c,c1,c2,dummy;
	RHist h;
	NDV p,red;
	Q cg,cred,gcd,tq,qq;
	double hmag;
	P tp,tp1;
	Obj tr,tr1;

	if ( dn ) {
		if ( mod )
			dn->m = 1;
		else if ( nd_vc )
			dn->r = (R)ONE;
		else
			dn->z = ONE;
	}
	if ( !g ) {
		*rp = d;
		return 1;
	}
	if ( !mod ) hmag = ((double)p_mag(HCP(g)))*nd_scale;

	sugar0 = sugar = SG(g);
	n = NV(g);
	mul = (NM)ALLOCA(sizeof(struct oNM)+(nd_wpd-1)*sizeof(UINT));
	if ( d )
		for ( tail = BDY(d); NEXT(tail); tail = NEXT(tail) );
	for ( ; g; ) {
		index = ndl_find_reducer(HDL(g));
		if ( index >= 0 ) {
			h = nd_psh[index];
			ndl_sub(HDL(g),DL(h),DL(mul));
			if ( ndl_check_bound2(index,DL(mul)) ) {
				nd_free(g); nd_free(d);
				return 0;
			}
			p = nd_demand ? ndv_load(index) : ps[index];
			g = nd_reduce2(mod,d,g,p,mul,dn);
			sugar = MAX(sugar,SG(p)+TD(DL(mul)));
			if ( !mod && g && ((double)(p_mag(HCP(g))) > hmag) ) {
				tp = HCP(g);
				nd_removecont2(d,g);
				if ( dn ) {
					if ( nd_vc ) {
						divsp(nd_vc,tp,HCP(g),&tp1);
						divr(nd_vc,(Obj)dn->r,(Obj)tp1,&tr);
						reductr(nd_vc,(Obj)tr,&tr1); dn->r = (R)tr1;
					} else {
						divq((Q)tp,HCQ(g),&qq);
						divq(dn->z,qq,&tq); dn->z = tq;
					}
				}
				hmag = ((double)p_mag(HCP(g)))*nd_scale;
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
	int count = 0;
	int hcount = 0;

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
	mul = (NM)ALLOCA(sizeof(struct oNM)+(nd_wpd-1)*sizeof(UINT));
	while ( 1 ) {
		hindex = mod?head_pbucket(mod,bucket):head_pbucket_q(bucket);
		if ( hindex < 0 ) {
			if ( DP_Print > 3 ) printf("(%d %d)",count,hcount);
			if ( d ) SG(d) = sugar;
			*rp = d;
			return 1;
		}
		g = bucket->body[hindex];
		index = ndl_find_reducer(HDL(g));
		if ( index >= 0 ) {
			count++;
			if ( !d ) hcount++;
			h = nd_psh[index];
			ndl_sub(HDL(g),DL(h),DL(mul));
			if ( ndl_check_bound2(index,DL(mul)) ) {
				nd_free(d);
				free_pbucket(bucket);
				*rp = 0;
				return 0;
			}
			p = ps[index];
			if ( mod == -1 )
				CM(mul) = _mulsf(_invsf(HCM(p)),_chsgnsf(HCM(g)));
 			else if ( mod ) {
				c1 = invm(HCM(p),mod); c2 = mod-HCM(g);
				DMAR(c1,c2,0,mod,c); CM(mul) = c;
			} else {
				igcd_cofactor(HCQ(g),HCQ(p),&gcd,&cg,&cred);
				chsgnq(cg,&CQ(mul));
				nd_mul_c_q(d,(P)cred);
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

/* input : list of NDV, cand : list of NDV */

int ndv_check_candidate(NODE input,int obpe,int oadv,EPOS oepos,NODE cand)
{
	int n,i,stat;
	ND nf,d;
	NDV r;
	NODE t,s;
	union oNDC dn;

	ndv_setup(0,0,cand,0,1);
	n = length(cand);

	/* membercheck : list is a subset of Id(cand) ? */
	for ( t = input; t; t = NEXT(t) ) {
again:
		if ( nd_bpe > obpe )
			r = ndv_dup_realloc((NDV)BDY(t),obpe,oadv,oepos);
		else
			r = (NDV)BDY(t);
		d = ndvtond(0,r);
		stat = nd_nf(0,0,d,nd_ps,0,0,&nf);
		if ( !stat ) {
			nd_reconstruct(0,0);
			goto again;
		} else if ( nf ) return 0;
		if ( DP_Print ) { printf("."); fflush(stdout); }
	}
	if ( DP_Print ) { printf("\n"); }
	/* gbcheck : cand is a GB of Id(cand) ? */
	if ( !nd_gb(0,0,1) ) return 0;
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

ND nd_separate_head(ND p,ND *head)
{
	NM m,m0;
	ND r;

	m = BDY(p);
	if ( !NEXT(m) ) {
		*head = p; p = 0;
	} else {
		m0 = m;
		BDY(p) = NEXT(m); LEN(p)--;
		NEXT(m0) = 0;
		MKND(NV(p),m0,1,r);
		*head = r;
	}
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

void add_pbucket_symbolic(PGeoBucket g,ND d)
{
	int l,i,k,m;

	if ( !d )
		return;
	l = LEN(d);
	for ( k = 0, m = 1; l > m; k++, m <<= 1 );
	/* 2^(k-1) < l <= 2^k (=m) */
	d = nd_merge(g->body[k],d);
	for ( ; d && LEN(d) > m; k++, m <<= 1 ) {
		g->body[k] = 0;
		d = nd_merge(g->body[k+1],d);
	}
	g->body[k] = d;
	g->m = MAX(g->m,k);
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
		nd_mul_c_q(g->body[k],(P)c);
}

NM remove_head_pbucket_symbolic(PGeoBucket g)
{
	int j,i,k,c;
	NM head;
	
	k = g->m;
	j = -1;
	for ( i = 0; i <= k; i++ ) {
		if ( !g->body[i] ) continue;
		if ( j < 0 ) j = i;
		else {
			c = DL_COMPARE(HDL(g->body[i]),HDL(g->body[j]));
			if ( c > 0 )
				j = i;
			else if ( c == 0 )
				g->body[i] = nd_remove_head(g->body[i]);
		}
	}
	if ( j < 0 ) return 0;
	else {
		head = BDY(g->body[j]);
		if ( !NEXT(head) ) {
			FREEND(g->body[j]);
			g->body[j] = 0;
		} else {
			BDY(g->body[j]) = NEXT(head);
			LEN(g->body[j])--;
		}
		return head;
	}
}

int head_pbucket(int mod,PGeoBucket g)
{
	int j,i,c,k,nv,sum;
	UINT *di,*dj;
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
					if ( mod == -1 )
						sum = _addsf(sum,HCM(gi));
					else {
						sum = sum+HCM(gi)-mod;
						if ( sum < 0 ) sum += mod;
					}
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

#if 0
void register_hcf(NDV p)
{
	DCP dc,t;
	P hc,h;
	int c;
	NODE l,l1,prev;

	hc = p->body->c.p;
	if ( !nd_vc || NUM(hc) ) return;
	fctrp(nd_vc,hc,&dc);
	for ( t = dc; t; t = NEXT(t) ) {
		h = t->c;
		if ( NUM(h) ) continue;
		for ( prev = 0, l = nd_hcf; l; prev = l, l = NEXT(l) ) {
			c = compp(nd_vc,h,(P)BDY(l));
			if ( c >= 0 ) break;
		}
		if ( !l || c > 0  ) {
			MKNODE(l1,h,l);
			if ( !prev )
				nd_hcf = l1;
			else
				NEXT(prev) = l1;
		}
	}
}
#else
void register_hcf(NDV p)
{
	DCP dc,t;
	P hc,h,q;
	Q dmy;
	int c;
	NODE l,l1,prev;

	hc = p->body->c.p;
	if ( NUM(hc) ) return;
	ptozp(hc,1,&dmy,&h);
#if 1
	for ( l = nd_hcf; l; l = NEXT(l) ) {
		while ( 1 ) {
			if ( divtpz(nd_vc,h,(P)BDY(l),&q) ) h = q;
			else break;
		}
	}
	if ( NUM(h) ) return;
#endif
	for ( prev = 0, l = nd_hcf; l; prev = l, l = NEXT(l) ) {
		c = compp(nd_vc,h,(P)BDY(l));
		if ( c >= 0 ) break;
	}
	if ( !l || c > 0  ) {
		MKNODE(l1,h,l);
		if ( !prev )
			nd_hcf = l1;
		else
			NEXT(prev) = l1;
	}
}
#endif

int do_diagonalize(int sugar,int m)
{
	int i,nh,stat;
	NODE r,g,t;
	ND h,nf,s,head;
	NDV nfv;
	Q q,num,den;
	P nm,nmp,dn,mnp,dnp;

	for ( i = nd_psn-1; i >= 0 && SG(nd_psh[i]) == sugar; i-- ) {
		if ( nd_demand )
			nfv = ndv_load(i);
		else
			nfv = nd_ps[i];
		s = ndvtond(m,nfv);
		s = nd_separate_head(s,&head);
		stat = nd_nf(m,head,s,nd_ps,1,0,&nf);
		if ( !stat ) return 0;
		ndv_free(nfv);
		nd_removecont(m,nf);
		nfv = ndtondv(m,nf);
		nd_free(nf);
		nd_bound[i] = ndv_compute_bound(nfv);
		if ( !m ) register_hcf(nfv);
		if ( nd_demand ) {
			ndv_save(nfv,i);
			ndv_free(nfv);
		} else
			nd_ps[i] = nfv;
	}
	return 1;
}

/* return value = 0 => input is not a GB */

NODE nd_gb(int m,int ishomo,int checkonly)
{
	int i,nh,sugar,stat;
	NODE r,g,t;
	ND_pairs d;
	ND_pairs l;
	ND h,nf,s,head,nf1;
	NDV nfv;
	Q q,num,den;
	union oNDC dn;
	int diag_count = 0;

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
			if ( ishomo ) {
				diag_count = 0;
				stat = do_diagonalize(sugar,m);
				if ( !stat ) {
					NEXT(l) = d; d = l;
					d = nd_reconstruct(0,d);
					goto again;
				}
			}
			sugar = SG(l);
			if ( DP_Print ) fprintf(asir_out,"%d",sugar);
		}
		stat = nd_sp(m,0,l,&h);
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(0,d);
			goto again;
		}
#if USE_GEOBUCKET
		stat = m?nd_nf_pbucket(m,h,nd_ps,!Top,&nf):nd_nf(m,0,h,nd_ps,!Top,0,&nf);
#else
		stat = nd_nf(m,0,h,nd_ps,!Top,0,&nf);
#endif
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(0,d);
			goto again;
		} else if ( nf ) {
			if ( checkonly ) return 0;
			if ( DP_Print ) { printf("+"); fflush(stdout); }
			nd_removecont(m,nf);
			if ( !m && nd_nalg ) {
				nd_monic(0,&nf);
				nd_removecont(m,nf);
			}
			nfv = ndtondv(m,nf); nd_free(nf);
			nh = ndv_newps(m,nfv,0);
			if ( !m && (ishomo && ++diag_count == diag_period) ) {
				diag_count = 0;
				stat = do_diagonalize(sugar,m);
				if ( !stat ) {
					NEXT(l) = d; d = l;
					d = nd_reconstruct(1,d);
					goto again;
				}
			}
			d = update_pairs(d,g,nh);
			g = update_base(g,nh);
			FREENDP(l);
		} else {
			if ( DP_Print ) { printf("."); fflush(stdout); }
			FREENDP(l);
		}
	}
	if ( nd_demand )
		for ( t = g; t; t = NEXT(t) ) 
			BDY(t) = (pointer)ndv_load((int)BDY(t));
	else
		for ( t = g; t; t = NEXT(t) )
			BDY(t) = (pointer)nd_ps[(int)BDY(t)];
	if ( !checkonly && DP_Print ) { printf("nd_gb done.\n"); fflush(stdout); }
	return g;
}

int do_diagonalize_trace(int sugar,int m)
{
	int i,nh,stat;
	NODE r,g,t;
	ND h,nf,nfq,s,head;
	NDV nfv,nfqv;
	Q q,den,num;

	for ( i = nd_psn-1; i >= 0 && SG(nd_psh[i]) == sugar; i-- ) {
		/* for nd_ps */
		s = ndvtond(m,nd_ps[i]);
		s = nd_separate_head(s,&head);
		stat = nd_nf_pbucket(m,s,nd_ps,1,&nf);
		if ( !stat ) return 0;
		nf = nd_add(m,head,nf);
		ndv_free(nd_ps[i]);
		nd_ps[i] = ndtondv(m,nf);
		nd_free(nf);

		/* for nd_ps_trace */
		if ( nd_demand )
			nfv = ndv_load(i);
		else
			nfv = nd_ps_trace[i];
		s = ndvtond(0,nfv);
		s = nd_separate_head(s,&head);
		stat = nd_nf(0,head,s,nd_ps_trace,1,0,&nf);
		if ( !stat ) return 0;
		ndv_free(nfv);
		nd_removecont(0,nf);
		nfv = ndtondv(0,nf);
		nd_free(nf);
		nd_bound[i] = ndv_compute_bound(nfv);
		register_hcf(nfv);
		if ( nd_demand ) {
			ndv_save(nfv,i);
			ndv_free(nfv);
		} else
			nd_ps_trace[i] = nfv;
	}
	return 1;
}

static struct oEGT eg_invdalg;
struct oEGT eg_le;

void nd_subst_vector(VL vl,P p,NODE subst,P *r)
{
	NODE tn;
	P p1;

	for ( tn = subst; tn; tn = NEXT(NEXT(tn)) ) {
		substp(vl,p,BDY(tn),BDY(NEXT(tn)),&p1); p = p1;
	}
	*r = p;
}

NODE nd_gb_trace(int m,int ishomo)
{
	int i,nh,sugar,stat;
	NODE r,g,t;
	ND_pairs d;
	ND_pairs l;
	ND h,nf,nfq,s,head;
	NDV nfv,nfqv;
	Q q,den,num;
	P hc;
	union oNDC dn;
	struct oEGT eg_monic,egm0,egm1;
	int diag_count = 0;

	init_eg(&eg_monic);
	init_eg(&eg_invdalg);
	init_eg(&eg_le);
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
#if 1
			if ( ishomo ) {
				if ( DP_Print > 2 ) fprintf(asir_out,"|");
				stat = do_diagonalize_trace(sugar,m);
				if ( DP_Print > 2 ) fprintf(asir_out,"|");
				diag_count = 0;
				if ( !stat ) {
					NEXT(l) = d; d = l;
					d = nd_reconstruct(1,d);
					goto again;
				}
			}
#endif
			sugar = SG(l);
			if ( DP_Print ) fprintf(asir_out,"%d",sugar);
		}
		stat = nd_sp(m,0,l,&h);
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(1,d);
			goto again;
		}
#if USE_GEOBUCKET
		stat = nd_nf_pbucket(m,h,nd_ps,!Top,&nf);
#else
		stat = nd_nf(m,0,h,nd_ps,!Top,0,&nf);
#endif
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(1,d);
			goto again;
		} else if ( nf ) {
			if ( nd_demand ) {
				nfqv = ndv_load(nd_psn);
				nfq = ndvtond(0,nfqv);
			} else
				nfq = 0;
			if ( !nfq ) {
				if ( !nd_sp(0,1,l,&h) || !nd_nf(0,0,h,nd_ps_trace,!Top,0,&nfq) ) {
					NEXT(l) = d; d = l;
					d = nd_reconstruct(1,d);
					goto again;
				}
			}
			if ( nfq ) {
				/* m|HC(nfq) => failure */
				if ( nd_vc ) {
					nd_subst_vector(nd_vc,HCP(nfq),nd_subst,&hc); q = (Q)hc;
				} else
					q = HCQ(nfq);
				if ( !rem(NM(q),m) ) return 0;

				if ( DP_Print ) { printf("+"); fflush(stdout); }
				if ( nd_nalg ) {
					/* m|DN(HC(nf)^(-1)) => failure */
					get_eg(&egm0);
					if ( !nd_monic(m,&nfq) ) return 0;
					get_eg(&egm1); add_eg(&eg_monic,&egm0,&egm1);
					nd_removecont(0,nfq); nfqv = ndtondv(0,nfq); nd_free(nfq);
					nfv = ndv_dup(0,nfqv); ndv_mod(m,nfv); nd_free(nf);
				} else {
					nd_removecont(0,nfq); nfqv = ndtondv(0,nfq); nd_free(nfq);
					nd_removecont(m,nf); nfv = ndtondv(m,nf); nd_free(nf);
				}
				nh = ndv_newps(0,nfv,nfqv);
				if ( ishomo && ++diag_count == diag_period ) {
					diag_count = 0;
					if ( DP_Print > 2 ) fprintf(asir_out,"|");
					stat = do_diagonalize_trace(sugar,m);
					if ( DP_Print > 2 ) fprintf(asir_out,"|");
					if ( !stat ) {
						NEXT(l) = d; d = l;
						d = nd_reconstruct(1,d);
						goto again;
					}
				}
				d = update_pairs(d,g,nh);
				g = update_base(g,nh);
			} else {
				if ( DP_Print ) { printf("*"); fflush(stdout); }
			}
		} else {
			if ( DP_Print ) { printf("."); fflush(stdout); }
		}
		FREENDP(l);
	}
	if ( nd_demand ) 
		for ( t = g; t; t = NEXT(t) )
			BDY(t) = (pointer)ndv_load((int)BDY(t));
	else
		for ( t = g; t; t = NEXT(t) )
			BDY(t) = (pointer)nd_ps_trace[(int)BDY(t)];
	if ( nd_nalg ) {
		print_eg("monic",&eg_monic);
		print_eg("invdalg",&eg_invdalg);
		print_eg("le",&eg_le);
	}
	if ( DP_Print ) { printf("nd_gb_trace done.\n"); fflush(stdout); }
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

NODE ndv_reduceall(int m,NODE f)
{
	int i,n,stat;
	ND nf,g,head;
	NODE t,a0,a;
	union oNDC dn;
	NDV *w;
	Q q,num,den;

	n = length(f);
#if 0
	w = (NDV *)ALLOCA(n*sizeof(NDV));
	for ( i = 0, t = f; i < n; i++, t = NEXT(t) ) w[i] = (NDV)BDY(t);
	qsort(w,n,sizeof(NDV),
		(int (*)(const void *,const void *))ndv_compare);
	for ( t = f, i = 0; t; i++, t = NEXT(t) ) BDY(t) = (pointer)w[i];
#endif
	ndv_setup(m,0,f,0,1);
	for ( i = 0; i < n; ) {
		g = ndvtond(m,nd_ps[i]);
		g = nd_separate_head(g,&head);
		stat = nd_nf(m,head,g,nd_ps,1,0,&nf);
		if ( !stat )
			nd_reconstruct(0,0);
		else {
			if ( DP_Print ) { printf("."); fflush(stdout); }
			ndv_free(nd_ps[i]);
			nd_removecont(m,nf);
			nd_ps[i] = ndtondv(m,nf); nd_free(nf);
			nd_bound[i] = ndv_compute_bound(nd_ps[i]);
			i++;
		}
	}
	if ( DP_Print ) { printf("\n"); }
	for ( a0 = 0, i = 0; i < n; i++ ) {
		NEXTNODE(a0,a);
		BDY(a) = (pointer)nd_ps[i];
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
	UINT *dl;
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
	UINT *t,*tl,*lcm;
	int td,tdl;

	if ( !d ) return 0;
	t = DL(nd_psh[s]);
	prev = 0;
	head = cur = d;
	lcm = (UINT *)ALLOCA(nd_wpd*sizeof(UINT));
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
	UINT *id,*jd;

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
	UINT *d;

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
	UINT *dl, *dln;
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
	UINT *lcm;
	int s,td,len,tlen,c,c1;

	if ( !(p = NEXT(m = d)) ) {
		*prest = p;
		NEXT(m) = 0;
		return m;
	}
	s = SG(m);
	if ( !NoSugar ) {
		for ( ml = 0, l = m; p; p = NEXT(l = p) )
			if ( (SG(p) < s) 
				|| ((SG(p) == s) && (DL_COMPARE(LCM(p),LCM(m)) < 0)) ) {
				ml = l; m = p; s = SG(m);
			}
	} else {
		for ( ml = 0, l = m; p; p = NEXT(l = p) )
			if ( DL_COMPARE(LCM(p),LCM(m)) < 0 ) {
				ml = l; m = p; s = SG(m);
			}
	}
	if ( !ml ) *prest = NEXT(m);
	else {
		NEXT(ml) = NEXT(m);
		*prest = d;
	}
	NEXT(m) = 0;
	return m;
}

ND_pairs nd_minsugarp( ND_pairs d, ND_pairs *prest )
{
	int msugar,i;
	ND_pairs t,dm0,dm,dr0,dr;

	for ( msugar = SG(d), t = NEXT(d); t; t = NEXT(t) )
		if ( SG(t) < msugar ) msugar = SG(t);
	dm0 = 0; dr0 = 0;
	for ( i = 0, t = d; t; t = NEXT(t) )
		if ( i < nd_f4_nsp && SG(t) == msugar ) {
			if ( dm0 ) NEXT(dm) = t;
			else dm0 = t;
			dm = t;
			i++;
		} else {
			if ( dr0 ) NEXT(dr) = t;
			else dr0 = t;
			dr = t;
		}
	NEXT(dm) = 0;
	if ( dr0 ) NEXT(dr) = 0;
	*prest = dr0;
	return dm0;
}

int ndv_newps(int m,NDV a,NDV aq)
{
	int len;
	RHist r;
	NDV b;

	if ( nd_psn == nd_pslen ) {
		nd_pslen *= 2;
		nd_ps = (NDV *)REALLOC((char *)nd_ps,nd_pslen*sizeof(NDV));
		nd_ps_trace = (NDV *)REALLOC((char *)nd_ps_trace,nd_pslen*sizeof(NDV));
		nd_psh = (RHist *)REALLOC((char *)nd_psh,nd_pslen*sizeof(RHist));
		nd_bound = (UINT **)
			REALLOC((char *)nd_bound,nd_pslen*sizeof(UINT *));
	}
	NEWRHist(r); nd_psh[nd_psn] = r;
	nd_ps[nd_psn] = a;
	if ( aq ) {
		nd_ps_trace[nd_psn] = aq;
		register_hcf(aq);
		nd_bound[nd_psn] = ndv_compute_bound(aq);
		SG(r) = SG(aq); ndl_copy(HDL(aq),DL(r));
	} else {
		if ( !m ) register_hcf(a);
		nd_bound[nd_psn] = ndv_compute_bound(a);
		SG(r) = SG(a); ndl_copy(HDL(a),DL(r));
	}
	if ( nd_demand ) {
		if ( aq ) {
			ndv_save(nd_ps_trace[nd_psn],nd_psn);
			nd_ps_trace[nd_psn] = 0;
		} else {
			ndv_save(nd_ps[nd_psn],nd_psn);
			nd_ps[nd_psn] = 0;
		}
	}
	return nd_psn++;
}

void ndv_setup(int mod,int trace,NODE f,int dont_sort,int dont_removecont)
{
	int i,j,td,len,max;
	NODE s,s0,f0;
	UINT *d;
	RHist r;
	NDV *w;
	NDV a,am;

	nd_found = 0; nd_notfirst = 0; nd_create = 0;

	for ( nd_psn = 0, s = f; s; s = NEXT(s) ) if ( BDY(s) ) nd_psn++;
	w = (NDV *)ALLOCA(nd_psn*sizeof(NDV));
	for ( i = 0, s = f; s; s = NEXT(s) ) if ( BDY(s) ) w[i++] = BDY(s);
	if ( !dont_sort ) {
		/* XXX heuristic */
		if ( !nd_ord->id && (nd_ord->ord.simple<2) )
			qsort(w,nd_psn,sizeof(NDV),
				(int (*)(const void *,const void *))ndv_compare_rev);
		else
			qsort(w,nd_psn,sizeof(NDV),
				(int (*)(const void *,const void *))ndv_compare);
	}
	nd_pslen = 2*nd_psn;
	nd_ps = (NDV *)MALLOC(nd_pslen*sizeof(NDV));
	nd_ps_trace = (NDV *)MALLOC(nd_pslen*sizeof(NDV));
	nd_psh = (RHist *)MALLOC(nd_pslen*sizeof(RHist));
	nd_bound = (UINT **)MALLOC(nd_pslen*sizeof(UINT *));
	nd_hcf = 0;

	if ( trace && nd_vc )
		makesubst(nd_vc,&nd_subst);
	else
		nd_subst = 0;

	if ( !nd_red )
		nd_red = (RHist *)MALLOC(REDTAB_LEN*sizeof(RHist));
	for ( i = 0; i < REDTAB_LEN; i++ ) nd_red[i] = 0;
	for ( i = 0; i < nd_psn; i++ ) {
		if ( trace ) {
			a = nd_ps_trace[i] = ndv_dup(0,w[i]);
			if ( !dont_removecont) ndv_removecont(0,a);
			register_hcf(a);
			am = nd_ps[i] = ndv_dup(mod,a);
			ndv_mod(mod,am);
			ndv_removecont(mod,am);
		} else {
			a = nd_ps[i] = ndv_dup(mod,w[i]);
			if ( mod || !dont_removecont ) ndv_removecont(mod,a);
			if ( !mod ) register_hcf(a);
		}
		NEWRHist(r); SG(r) = HTD(a); ndl_copy(HDL(a),DL(r));
		nd_bound[i] = ndv_compute_bound(a);
		nd_psh[i] = r;
		if ( nd_demand ) {
			if ( trace ) {
				ndv_save(nd_ps_trace[i],i);
				nd_ps_trace[i] = 0;
			} else {
				ndv_save(nd_ps[i],i);
				nd_ps[i] = 0;
			}
		}
	}
}

struct order_spec *append_block(struct order_spec *spec,
    int nv,int nalg,int ord);

extern VECT current_dl_weight_vector_obj;
static VECT prev_weight_vector_obj;

void preprocess_algcoef(VL vv,VL av,struct order_spec *ord,LIST f,
	struct order_spec **ord1p,LIST *f1p,NODE *alistp)
{
	NODE alist,t,s,r0,r,arg;
	VL tv;
	P poly;
	DP d;
	Alg alpha,dp;
	DAlg inv,da,hc;
	MP m;
	int i,nvar,nalg,n;
	NumberField nf;
	LIST f1,f2;
	struct order_spec *current_spec;
	VECT obj,obj0;
	Obj tmp;

	for ( nvar = 0, tv = vv; tv; tv = NEXT(tv), nvar++);
	for ( nalg = 0, tv = av; tv; tv = NEXT(tv), nalg++);

	for ( alist = 0, tv = av; tv; tv = NEXT(tv) ) {
		NEXTNODE(alist,t); MKV(tv->v,poly); 
		MKAlg(poly,alpha); BDY(t) = (pointer)alpha;
		tv->v = tv->v->priv;
	}
	NEXT(t) = 0;

	/* simplification, makeing polynomials monic */
	setfield_dalg(alist);
	obj_algtodalg(f,&f1);
	for ( t = BDY(f); t; t = NEXT(t) ) {
		initd(ord); ptod(vv,vv,(P)BDY(t),&d);			
		hc = (DAlg)BDY(d)->c;
		if ( NID(hc) == N_DA ) {
			invdalg(hc,&inv);
			for ( m = BDY(d); m; m = NEXT(m) ) {
				muldalg(inv,(DAlg)m->c,&da); m->c = (P)da;
			}
		}
		initd(ord); dtop(vv,vv,d,&poly); BDY(f) = (pointer)poly;
	}
	obj_dalgtoalg(f1,&f);

	/* append alg vars to the var list */
	for ( tv = vv; NEXT(tv); tv = NEXT(tv) );
	NEXT(tv) = av;

	/* append a block to ord */
	*ord1p = append_block(ord,nvar,nalg,2);

	/* create generator list */
	nf = get_numberfield();
	for ( i = nalg-1, t = BDY(f); i >= 0; i-- ) {
		MKAlg(nf->defpoly[i],dp);
		MKNODE(s,dp,t); t = s;
	}
	MKLIST(f1,t);
	*alistp = alist;
	algobjtorat(f1,f1p);

	/* creating a new weight vector */
	prev_weight_vector_obj = obj0 = current_dl_weight_vector_obj;
	n = nvar+nalg+1;
	MKVECT(obj,n);
	if ( obj0 && obj0->len == nvar )
		for ( i = 0; i < nvar; i++ ) BDY(obj)[i] = BDY(obj0)[i];
	else
		for ( i = 0; i < nvar; i++ ) BDY(obj)[i] = (pointer)ONE;
	for ( i = 0; i < nalg; i++ ) BDY(obj)[i+nvar] = 0;
	BDY(obj)[n-1] = (pointer)ONE;
	arg = mknode(1,obj);
	Pdp_set_weight(arg,&tmp);
}

NODE postprocess_algcoef(VL av,NODE alist,NODE r)
{
	NODE s,t,u0,u;
	P p;
	VL tv;
	Obj obj,tmp;
	NODE arg;

	u0 = 0;
	for ( t = r; t; t = NEXT(t) ) {
		p = (P)BDY(t);
		for ( tv = av, s = alist; tv; tv = NEXT(tv), s = NEXT(s) ) {
			substr(CO,0,(Obj)p,tv->v,(Obj)BDY(s),&obj); p = (P)obj;
		}
		if ( OID(p) == O_P || (OID(p) == O_N && NID((Num)p) != N_A) ) {
			NEXTNODE(u0,u);
			BDY(u) = (pointer)p;
		}
	}	
	arg = mknode(1,prev_weight_vector_obj);
	Pdp_set_weight(arg,&tmp);

	return u0;
}

void nd_gr(LIST f,LIST v,int m,int f4,struct order_spec *ord,LIST *rp)
{
	VL tv,fv,vv,vc,av;
	NODE fd,fd0,r,r0,t,x,s,xx,alist;
	int e,max,nvar,i;
	NDV b;
	int ishomo,nalg;
	Alg alpha,dp;
	P p,zp;
	Q dmy;
	LIST f1,f2;
	Obj obj;
	NumberField nf;
	struct order_spec *ord1;

	if ( !m && Demand ) nd_demand = 1;
	else nd_demand = 0;

	if ( DP_Multiple )
		nd_scale = ((double)DP_Multiple)/(double)(Denominator?Denominator:1);
#if 0
	ndv_alloc = 0;
#endif
	get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&nd_vc);
	for ( nvar = 0, tv = vv; tv; tv = NEXT(tv), nvar++ );
	switch ( ord->id ) {
		case 1:
			if ( ord->nv != nvar )
				error("nd_{gr,f4} : invalid order specification");
			break;
		default:
			break;
	}
	nd_nalg = 0;
	av = 0;
	if ( !m ) {
		get_algtree((Obj)f,&av);
		for ( nalg = 0, tv = av; tv; tv = NEXT(tv), nalg++ );
		nd_ntrans = nvar;
		nd_nalg = nalg;
		/* #i -> t#i */
		if ( nalg ) {
			preprocess_algcoef(vv,av,ord,f,&ord1,&f1,&alist);
			ord = ord1;
			f = f1;
		}
		nvar += nalg;
	}
	nd_init_ord(ord);
	for ( t = BDY(f), max = 0; t; t = NEXT(t) )
		for ( tv = vv; tv; tv = NEXT(tv) ) {
			e = getdeg(tv->v,(P)BDY(t));
			max = MAX(e,max);
		}
	nd_setup_parameters(nvar,max);
	ishomo = 1;
	for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
		ptozp((P)BDY(t),1,&dmy,&zp);
		b = (pointer)ptondv(CO,vv,zp);
		if ( ishomo )
			ishomo = ishomo && ndv_ishomo(b);
		if ( m ) ndv_mod(m,b);
		if ( b ) { NEXTNODE(fd0,fd); BDY(fd) = (pointer)b; }
	}
	if ( fd0 ) NEXT(fd) = 0;
	ndv_setup(m,0,fd0,0,0);
	x = f4?nd_f4(m):nd_gb(m,ishomo,0);
	nd_demand = 0;
	x = ndv_reducebase(x);
	x = ndv_reduceall(m,x);
	for ( r0 = 0, t = x; t; t = NEXT(t) ) {
		NEXTNODE(r0,r); 
		BDY(r) = ndvtop(m,CO,vv,BDY(t));
	}
	if ( r0 ) NEXT(r) = 0;
	if ( nalg )
		r0 = postprocess_algcoef(av,alist,r0);
	MKLIST(*rp,r0);
#if 0
	fprintf(asir_out,"ndv_alloc=%d\n",ndv_alloc);
#endif
}

void nd_gr_postproc(LIST f,LIST v,int m,struct order_spec *ord,int do_check,LIST *rp)
{
	VL tv,fv,vv,vc,av;
	NODE fd,fd0,r,r0,t,x,s,xx,alist;
	int e,max,nvar,i;
	NDV b;
	int ishomo,nalg;
	Alg alpha,dp;
	P p,zp;
	Q dmy;
	LIST f1,f2;
	Obj obj;
	NumberField nf;
	struct order_spec *ord1;

	get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&nd_vc);
	for ( nvar = 0, tv = vv; tv; tv = NEXT(tv), nvar++ );
	switch ( ord->id ) {
		case 1:
			if ( ord->nv != nvar )
				error("nd_check : invalid order specification");
			break;
		default:
			break;
	}
	nd_nalg = 0;
	av = 0;
	if ( !m ) {
		get_algtree((Obj)f,&av);
		for ( nalg = 0, tv = av; tv; tv = NEXT(tv), nalg++ );
		nd_ntrans = nvar;
		nd_nalg = nalg;
		/* #i -> t#i */
		if ( nalg ) {
			preprocess_algcoef(vv,av,ord,f,&ord1,&f1,&alist);
			ord = ord1;
			f = f1;
		}
		nvar += nalg;
	}
	nd_init_ord(ord);
	for ( t = BDY(f), max = 0; t; t = NEXT(t) )
		for ( tv = vv; tv; tv = NEXT(tv) ) {
			e = getdeg(tv->v,(P)BDY(t));
			max = MAX(e,max);
		}
	nd_setup_parameters(nvar,max);
	ishomo = 1;
	for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
		ptozp((P)BDY(t),1,&dmy,&zp);
		b = (pointer)ptondv(CO,vv,zp);
		if ( ishomo )
			ishomo = ishomo && ndv_ishomo(b);
		if ( m ) ndv_mod(m,b);
		if ( b ) { NEXTNODE(fd0,fd); BDY(fd) = (pointer)b; }
	}
	if ( fd0 ) NEXT(fd) = 0;
	ndv_setup(m,0,fd0,0,1);
	for ( x = 0, i = 0; i < nd_psn; i++ )
		x = update_base(x,i);
	if ( do_check ) {
		x = nd_gb(m,ishomo,1);
		if ( !x ) {
			*rp = 0;
			return;
		}
	} else {
		for ( t = x; t; t = NEXT(t) )
			BDY(t) = (pointer)nd_ps[(int)BDY(t)];
	}
	x = ndv_reducebase(x);
	x = ndv_reduceall(m,x);
	for ( r0 = 0, t = x; t; t = NEXT(t) ) {
		NEXTNODE(r0,r); 
		BDY(r) = ndvtop(m,CO,vv,BDY(t));
	}
	if ( r0 ) NEXT(r) = 0;
	if ( nalg )
		r0 = postprocess_algcoef(av,alist,r0);
	MKLIST(*rp,r0);
}

void nd_gr_trace(LIST f,LIST v,int trace,int homo,int f4,struct order_spec *ord,LIST *rp)
{
	VL tv,fv,vv,vc,av;
	NODE fd,fd0,in0,in,r,r0,t,s,cand,alist;
	int m,nocheck,nvar,mindex,e,max;
	NDV c;
	NMV a;
	P p,zp;
	Q dmy;
	EPOS oepos;
	int obpe,oadv,wmax,i,len,cbpe,ishomo,nalg;
	Alg alpha,dp;
	P poly;
	LIST f1,f2;
	Obj obj;
	NumberField nf;
	struct order_spec *ord1;
	struct oEGT eg_check,eg0,eg1;

	if ( DP_Multiple )
		nd_scale = ((double)DP_Multiple)/(double)(Denominator?Denominator:1);

	get_vars((Obj)f,&fv); pltovl(v,&vv); vlminus(fv,vv,&nd_vc);
	for ( nvar = 0, tv = vv; tv; tv = NEXT(tv), nvar++ );
	switch ( ord->id ) {
		case 1:
			if ( ord->nv != nvar )
				error("nd_gr_trace : invalid order specification");
			break;
		default:
			break;
	}

	get_algtree((Obj)f,&av);
	for ( nalg = 0, tv = av; tv; tv = NEXT(tv), nalg++ );
	nd_ntrans = nvar;
	nd_nalg = nalg;
	/* #i -> t#i */
	if ( nalg ) {
		preprocess_algcoef(vv,av,ord,f,&ord1,&f1,&alist);
		ord = ord1;
		f = f1;
	}
	nvar += nalg;

	nocheck = 0;
	mindex = 0;

	if ( Demand ) nd_demand = 1;
	else nd_demand = 0;

	/* setup modulus */
	if ( trace < 0 ) {
		trace = -trace;
		nocheck = 1;
	}
	m = trace > 1 ? trace : get_lprime(mindex);
	for ( t = BDY(f), max = 0; t; t = NEXT(t) )
		for ( tv = vv; tv; tv = NEXT(tv) ) {
			e = getdeg(tv->v,(P)BDY(t));
			max = MAX(e,max);
		}
	nd_init_ord(ord);
	nd_setup_parameters(nvar,max);
	obpe = nd_bpe; oadv = nmv_adv; oepos = nd_epos;
	ishomo = 1;
	for ( in0 = 0, fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
		ptozp((P)BDY(t),1,&dmy,&zp);
		c = (pointer)ptondv(CO,vv,zp);
		if ( ishomo )
			ishomo = ishomo && ndv_ishomo(c);
		if ( c ) { 
			NEXTNODE(in0,in); BDY(in) = (pointer)c;
			NEXTNODE(fd0,fd); BDY(fd) = (pointer)ndv_dup(0,c);
		}
	}
	if ( in0 ) NEXT(in) = 0;
	if ( fd0 ) NEXT(fd) = 0;
	if ( !ishomo && homo ) {
		for ( t = in0, wmax = max; t; t = NEXT(t) ) {
			c = (NDV)BDY(t); len = LEN(c);
			for ( a = BDY(c), i = 0; i < len; i++, NMV_ADV(a) )
				wmax = MAX(TD(DL(a)),wmax);
		}
		homogenize_order(ord,nvar,&ord1);
		nd_init_ord(ord1);
		nd_setup_parameters(nvar+1,wmax);
		for ( t = fd0; t; t = NEXT(t) )
			ndv_homogenize((NDV)BDY(t),obpe,oadv,oepos);
	}
	while ( 1 ) {
		if ( Demand )
			nd_demand = 1;
		ndv_setup(m,1,fd0,0,0);
		cand = f4?nd_f4_trace(m):nd_gb_trace(m,ishomo || homo);
		if ( !cand ) {
			/* failure */
			if ( trace > 1 ) { *rp = 0; return; }
			else m = get_lprime(++mindex);
			continue;
		}
		if ( !ishomo && homo ) {
			/* dehomogenization */
			for ( t = cand; t; t = NEXT(t) ) ndv_dehomogenize((NDV)BDY(t),ord);
			nd_init_ord(ord);
			nd_setup_parameters(nvar,0);
		}
		nd_demand = 0;
		cand = ndv_reducebase(cand);
		cand = ndv_reduceall(0,cand);
		cbpe = nd_bpe;
		if ( nocheck )
			break;
		get_eg(&eg0);
		if ( ndv_check_candidate(in0,obpe,oadv,oepos,cand) )
			/* success */
			break;
		else if ( trace > 1 ) {
			/* failure */
			*rp = 0; return;	
		} else {
			/* try the next modulus */
			m = get_lprime(++mindex);
			/* reset the parameters */
			if ( !ishomo && homo ) {
				nd_init_ord(ord1);
				nd_setup_parameters(nvar+1,wmax);
			} else {
				nd_init_ord(ord);
				nd_setup_parameters(nvar,max);
			}
		}
	}
	get_eg(&eg1); init_eg(&eg_check); add_eg(&eg_check,&eg0,&eg1);
	if ( DP_Print )
		fprintf(asir_out,"check=%fsec\n",eg_check.exectime+eg_check.gctime);
	/* dp->p */
	nd_bpe = cbpe;
	nd_setup_parameters(nd_nvar,0);
	for ( r = cand; r; r = NEXT(r) )
		BDY(r) = (pointer)ndvtop(0,CO,vv,BDY(r));
	if ( nalg )
		cand = postprocess_algcoef(av,alist,cand);
	MKLIST(*rp,cand);
}

void dltondl(int n,DL dl,UINT *r)
{
	UINT *d;
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
		ndl_weight_mask(r);
	} else {
		for ( i = 0; i < n; i++ ) PUT_EXP(r,i,d[i]);
		TD(r) = ndl_weight(r);
	}
}

DL ndltodl(int n,UINT *ndl)
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

void ndl_print(UINT *dl)
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
			if ( CM(m) & 0x80000000 ) printf("+@_%d*",IFTOF(CM(m)));
			else printf("+%d*",CM(m));
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
			printexpr(CO,(Obj)CQ(m));
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

	if ( mod == -1 ) nd_mul_c(mod,p,_invsf(HCM(p)));
	else if ( mod ) nd_mul_c(mod,p,invm(HCM(p),mod));
	else {
		for ( m = BDY(p), n = 0; m; m = NEXT(m), n++ );
		w = (Q *)ALLOCA(n*sizeof(Q));
		v.len = n;
		v.body = (pointer *)w;
		for ( m = BDY(p), i = 0; i < n; m = NEXT(m), i++ ) w[i] = CQ(m);
		removecont_array((P *)w,n,1);
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

	n1 = nd_length(p1);
	n2 = nd_length(p2);
	n = n1+n2;
	w = (Q *)ALLOCA(n*sizeof(Q));
	v.len = n;
	v.body = (pointer *)w;
	i = 0;
	if ( p1 )
		for ( m = BDY(p1); i < n1; m = NEXT(m), i++ ) w[i] = CQ(m);
	if ( p2 )
		for ( m = BDY(p2); i < n; m = NEXT(m), i++ ) w[i] = CQ(m);
	removecont_array((P *)w,n,1);
	i = 0;
	if ( p1 )
		for ( m = BDY(p1); i < n1; m = NEXT(m), i++ ) CQ(m) = w[i];	
	if ( p2 )
		for ( m = BDY(p2); i < n; m = NEXT(m), i++ ) CQ(m) = w[i];	
}

void ndv_removecont(int mod,NDV p)
{
	int i,len,all_p;
	Q *c;
	P *w;
	Q dvr,t;
	P g,cont,tp;
	NMV m;

	if ( mod == -1 )
		ndv_mul_c(mod,p,_invsf(HCM(p)));
	else if ( mod )
		ndv_mul_c(mod,p,invm(HCM(p),mod));
	else {
		len = p->len;
		w = (P *)ALLOCA(len*sizeof(P));
		c = (Q *)ALLOCA(len*sizeof(Q));
		for ( m = BDY(p), all_p = 1, i = 0; i < len; NMV_ADV(m), i++ ) {
			ptozp(CP(m),1,&c[i],&w[i]);
			all_p = all_p && !NUM(w[i]);
		}
		if ( all_p ) {
			qltozl(c,len,&dvr); nd_heu_nezgcdnpz(nd_vc,w,len,1,&g);
			mulp(nd_vc,(P)dvr,g,&cont);
			for ( m = BDY(p), i = 0; i < len; NMV_ADV(m), i++ ) {
				divsp(nd_vc,CP(m),cont,&tp); CP(m) = tp;
			}
		} else {
			sortbynm((Q *)c,len);
			qltozl((Q *)c,len,&dvr);
			for ( m = BDY(p), i = 0; i < len; NMV_ADV(m), i++ ) {
				divsp(nd_vc,CP(m),(P)dvr,&tp); CP(m) = tp;
			}
		}
	}
}

void ndv_homogenize(NDV p,int obpe,int oadv,EPOS oepos)
{
	int len,i,max;
	NMV m,mr0,mr,t;

	len = p->len;
	for ( m = BDY(p), i = 0, max = 0; i < len; NMV_OADV(m), i++ )
		max = MAX(max,TD(DL(m)));
	mr0 = nmv_adv>oadv?(NMV)REALLOC(BDY(p),len*nmv_adv):BDY(p);
	m = (NMV)((char *)mr0+(len-1)*oadv);
	mr = (NMV)((char *)mr0+(len-1)*nmv_adv);
	t = (NMV)ALLOCA(nmv_adv);
	for ( i = 0; i < len; i++, NMV_OPREV(m), NMV_PREV(mr) ) {
		ndl_homogenize(DL(m),DL(t),obpe,oepos,max);
		CQ(mr) = CQ(m);
		ndl_copy(DL(t),DL(mr));
	}
	NV(p)++;
	BDY(p) = mr0;
}

void ndv_dehomogenize(NDV p,struct order_spec *ord)
{
	int i,j,adj,len,newnvar,newwpd,newadv,newexporigin;
	Q *w;
	Q dvr,t;
	NMV m,r;

	len = p->len;
	newnvar = nd_nvar-1;
	newexporigin = nd_get_exporigin(ord);
	newwpd = newnvar/nd_epw+(newnvar%nd_epw?1:0)+newexporigin;
	for ( m = BDY(p), i = 0; i < len; NMV_ADV(m), i++ )
		ndl_dehomogenize(DL(m));
	if ( newwpd != nd_wpd ) {
		newadv = ROUND_FOR_ALIGN(sizeof(struct oNMV)+(newwpd-1)*sizeof(UINT));
		for ( m = r = BDY(p), i = 0; i < len; NMV_ADV(m), NDV_NADV(r), i++ ) {
			CQ(r) = CQ(m);
			for ( j = 0; j < newexporigin; j++ ) DL(r)[j] = DL(m)[j];
			adj = nd_exporigin-newexporigin;
			for ( ; j < newwpd; j++ ) DL(r)[j] = DL(m)[j+adj];
		}
	}
	NV(p)--;
}

void nd_heu_nezgcdnpz(VL vl,P *pl,int m,int full,P *pr)
{
	int i;
	P *tpl,*tpl1;
	NODE l;
	P h,gcd,t;

	tpl = (P *)ALLOCA(m*sizeof(P));
	tpl1 = (P *)ALLOCA(m*sizeof(P));
	bcopy(pl,tpl,m*sizeof(P));
	gcd = (P)ONE;
	for ( l = nd_hcf; l; l = NEXT(l) ) {
		h = (P)BDY(l);
		while ( 1 ) {
			for ( i = 0; i < m; i++ )
				if ( !divtpz(vl,tpl[i],h,&tpl1[i]) )
					break;
			if ( i == m ) {
				bcopy(tpl1,tpl,m*sizeof(P));
				mulp(vl,gcd,h,&t); gcd = t;
			} else
				break;
		}
	}
	if ( DP_Print > 2 ){fprintf(asir_out,"[%d]",nmonop(gcd)); fflush(asir_out);}
	if ( full ) {
		heu_nezgcdnpz(vl,tpl,m,&t);
		mulp(vl,gcd,t,pr);
	} else
		*pr = gcd;
}

void removecont_array(P *p,int n,int full)
{
	int all_p,all_q,i;
	Q *c;
	P *w;
	P t,s;

	for ( all_q = 1, i = 0; i < n; i++ )
		all_q = all_q && NUM(p[i]);
	if ( all_q ) {
		removecont_array_q((Q *)p,n);
	} else {
		c = (Q *)ALLOCA(n*sizeof(Q));
		w = (P *)ALLOCA(n*sizeof(P));
		for ( i = 0; i < n; i++ ) {
			ptozp(p[i],1,&c[i],&w[i]);
		}
		removecont_array_q(c,n);
		nd_heu_nezgcdnpz(nd_vc,w,n,full,&t);
		for ( i = 0; i < n; i++ ) {
			divsp(nd_vc,w[i],t,&s); mulp(nd_vc,s,(P)c[i],&p[i]);
		}
	}
}

void removecont_array_q(Q *c,int n)
{
	struct oVECT v;
	Q d0,d1,a,u,u1,gcd;
	int i,j;
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
	if ( mul == 1 ) return;
	if ( mod == -1 )
		for ( m = BDY(p); m; m = NEXT(m) )
			CM(m) = _mulsf(CM(m),mul);
	else
		for ( m = BDY(p); m; m = NEXT(m) ) {
			c1 = CM(m); DMAR(c1,mul,0,mod,c); CM(m) = c;
		}
}

void nd_mul_c_q(ND p,P mul)
{
	NM m;
	P c;

	if ( !p ) return;
	if ( UNIQ(mul) ) return;
	for ( m = BDY(p); m; m = NEXT(m) ) {
		mulp(nd_vc,CP(m),mul,&c); CP(m) = c;
	}
}

void nd_mul_c_p(VL vl,ND p,P mul)
{
	NM m;
	P c;

	if ( !p ) return;
	for ( m = BDY(p); m; m = NEXT(m) ) {
		mulp(vl,CP(m),mul,&c); CP(m) = c;
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

void nd_append_red(UINT *d,int i)
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

UINT *ndv_compute_bound(NDV p)
{
	UINT *d1,*d2,*t;
	UINT u;
	int i,j,k,l,len,ind;
	NMV m;

	if ( !p )
		return 0;
	d1 = (UINT *)ALLOCA(nd_wpd*sizeof(UINT));
	d2 = (UINT *)ALLOCA(nd_wpd*sizeof(UINT));
	len = LEN(p);
	m = BDY(p); ndl_copy(DL(m),d1); NMV_ADV(m);
	for ( i = 1; i < len; i++, NMV_ADV(m) ) {
		ndl_lcm(DL(m),d1,d2);
		t = d1; d1 = d2; d2 = t;
	}
	l = nd_nvar+31;
	t = (UINT *)MALLOC_ATOMIC(l*sizeof(UINT));
	for ( i = nd_exporigin, ind = 0; i < nd_wpd; i++ ) {
		u = d1[i];
		k = (nd_epw-1)*nd_bpe;
		for ( j = 0; j < nd_epw; j++, k -= nd_bpe, ind++ )
			t[ind] = (u>>k)&nd_mask0;
	}
	for ( ; ind < l; ind++ ) t[ind] = 0;
	return t;
}

UINT *nd_compute_bound(ND p)
{
	UINT *d1,*d2,*t;
	UINT u;
	int i,j,k,l,len,ind;
	NM m;

	if ( !p )
		return 0;
	d1 = (UINT *)ALLOCA(nd_wpd*sizeof(UINT));
	d2 = (UINT *)ALLOCA(nd_wpd*sizeof(UINT));
	len = LEN(p);
	m = BDY(p); ndl_copy(DL(m),d1); m = NEXT(m);
	for ( m = NEXT(m); m; m = NEXT(m) ) {
		ndl_lcm(DL(m),d1,d2);
		t = d1; d1 = d2; d2 = t;
	}
	l = nd_nvar+31;
	t = (UINT *)MALLOC_ATOMIC(l*sizeof(UINT));
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
		case 0: case 2:
			return 1;
		case 1:
			/* block order */
			/* d[0]:weight d[1]:w0,...,d[nd_exporigin-1]:w(n-1) */
			return ord->ord.block.length+1;
		case 3:
			error("nd_get_exporigin : composite order is not supported yet.");
	}
}

void nd_setup_parameters(int nvar,int max) {
	int i,j,n,elen,ord_o,ord_l,l,s,wpd;
	struct order_pair *op;

	nd_nvar = nvar;
	if ( max ) {
		/* XXX */
		if ( do_weyl ) nd_bpe = 32;
		else if ( max < 2 ) nd_bpe = 1;
		else if ( max < 4 ) nd_bpe = 2;
		else if ( max < 8 ) nd_bpe = 3;
		else if ( max < 16 ) nd_bpe = 4;
		else if ( max < 32 ) nd_bpe = 5;
		else if ( max < 64 ) nd_bpe = 6;
		else if ( max < 256 ) nd_bpe = 8;
		else if ( max < 1024 ) nd_bpe = 10;
		else if ( max < 65536 ) nd_bpe = 16;
		else nd_bpe = 32;
	}
	nd_epw = (sizeof(UINT)*8)/nd_bpe;
	elen = nd_nvar/nd_epw+(nd_nvar%nd_epw?1:0);
	nd_exporigin = nd_get_exporigin(nd_ord);
	wpd = nd_exporigin+elen;
	if ( wpd != nd_wpd ) {
		nd_free_private_storage();
		nd_wpd = wpd;
	}
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
	nmv_adv = ROUND_FOR_ALIGN(sizeof(struct oNMV)+(nd_wpd-1)*sizeof(UINT));
	nd_epos = nd_create_epos(nd_ord);
	nd_blockmask = nd_create_blockmask(nd_ord);
	nd_work_vector = (int *)REALLOC(nd_work_vector,nd_nvar*sizeof(int));
}

ND_pairs nd_reconstruct(int trace,ND_pairs d)
{
	int i,obpe,oadv,h;
	static NM prev_nm_free_list;
	static ND_pairs prev_ndp_free_list;
	RHist mr0,mr;
	RHist r;
	RHist *old_red;
	ND_pairs s0,s,t;
	EPOS oepos;

	obpe = nd_bpe;
	oadv = nmv_adv;
	oepos = nd_epos;
	if ( obpe < 2 ) nd_bpe = 2;
	else if ( obpe < 3 ) nd_bpe = 3;
	else if ( obpe < 4 ) nd_bpe = 4;
	else if ( obpe < 5 ) nd_bpe = 5;
	else if ( obpe < 6 ) nd_bpe = 6;
	else if ( obpe < 8 ) nd_bpe = 8;
	else if ( obpe < 10 ) nd_bpe = 10;
	else if ( obpe < 16 ) nd_bpe = 16;
	else if ( obpe < 32 ) nd_bpe = 32;
	else error("nd_reconstruct : exponent too large");

	nd_setup_parameters(nd_nvar,0);
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
		ndl_reconstruct(LCM(t),LCM(s),obpe,oepos);
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
			ndl_reconstruct(DL(r),DL(mr),obpe,oepos);
			h = ndl_hash_value(DL(mr));
			NEXT(mr) = nd_red[h];
			nd_red[h] = mr;
		}
	for ( i = 0; i < REDTAB_LEN; i++ ) old_red[i] = 0;
	old_red = 0;
	for ( i = 0; i < nd_psn; i++ ) {
		NEWRHist(r); SG(r) = SG(nd_psh[i]);
		ndl_reconstruct(DL(nd_psh[i]),DL(r),obpe,oepos);
		nd_psh[i] = r;
	}
	if ( s0 ) NEXT(s) = 0;
	prev_nm_free_list = 0;
	prev_ndp_free_list = 0;
#if 0
	GC_gcollect();
#endif
	return s0;
}

void ndl_reconstruct(UINT *d,UINT *r,int obpe,EPOS oepos)
{
	int n,i,ei,oepw,omask0,j,s,ord_l,l;
	struct order_pair *op;

	n = nd_nvar;
	oepw = (sizeof(UINT)*8)/obpe;
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
	NM m1,m2;
	NDV p1,p2;
	ND t1,t2;
	UINT *lcm;
	P gp,tp;
	Q g,t;
	int td;

	if ( !mod && nd_demand ) {
		p1 = ndv_load(p->i1); p2 = ndv_load(p->i2);
	} else {
		if ( trace ) {
			p1 = nd_ps_trace[p->i1]; p2 = nd_ps_trace[p->i2];
		} else {
			p1 = nd_ps[p->i1]; p2 = nd_ps[p->i2];
		}
	}
	lcm = LCM(p);
	NEWNM(m1); ndl_sub(lcm,HDL(p1),DL(m1));
	if ( ndl_check_bound2(p->i1,DL(m1)) ) {
		FREENM(m1); return 0;
	}
	NEWNM(m2); ndl_sub(lcm,HDL(p2),DL(m2));
	if ( ndl_check_bound2(p->i2,DL(m2)) ) {
		FREENM(m1); FREENM(m2); return 0;
	}

	if ( mod == -1 ) {
		CM(m1) = HCM(p2); CM(m2) = _chsgnsf(HCM(p1));
	} else if ( mod ) {
		CM(m1) = HCM(p2); CM(m2) = mod-HCM(p1); 
	} else if ( nd_vc ) {
		ezgcdpz(nd_vc,HCP(p1),HCP(p2),&gp);
		divsp(nd_vc,HCP(p2),gp,&CP(m1));
		divsp(nd_vc,HCP(p1),gp,&tp); chsgnp(tp,&CP(m2));
	} else {
		igcd_cofactor(HCQ(p1),HCQ(p2),&g,&t,&CQ(m1)); chsgnq(t,&CQ(m2));
	}
	t1 = ndv_mul_nm(mod,m1,p1); t2 = ndv_mul_nm(mod,m2,p2);
	*rp = nd_add(mod,t1,t2);
	FREENM(m1); FREENM(m2);
	return 1;
}

void ndv_mul_c(int mod,NDV p,int mul)
{
	NMV m;
	int c,c1,len,i;

	if ( !p ) return;
	len = LEN(p);
	if ( mod == -1 )
		for ( m = BDY(p), i = 0; i < len; i++, NMV_ADV(m) ) 
			CM(m) = _mulsf(CM(m),mul);
	else
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
	UINT *d0;
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
	UINT *d0,*d1,*d,*dt,*ctab;
	Q *ctab_q;
	Q q,q1;
	UINT c0,c1,c;
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
		if ( nd_blockmask ) ndl_weight_mask(DL(m));
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
					if ( nd_blockmask ) ndl_weight_mask(dt);
				}
			curlen *= k+1;
			continue;
		}
		min = MIN(k,l);
		if ( mod ) {
			ctab = (UINT *)ALLOCA((min+1)*sizeof(UINT));
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
			if ( nd_blockmask ) ndl_weight_mask(d);
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

ND ndv_mul_nm_symbolic(NM m0,NDV p)
{
	NM mr,mr0;
	NMV m;
	UINT *d,*dt,*dm;
	int c,n,td,i,c1,c2,len;
	Q q;
	ND r;

	if ( !p ) return 0;
	else {
		n = NV(p); m = BDY(p);
		d = DL(m0);
		len = LEN(p);
		mr0 = 0;
		td = TD(d);
		c = CM(m0);
		for ( i = 0; i < len; i++, NMV_ADV(m) ) {
			NEXTNM(mr0,mr);
			CM(mr) = 1;
			ndl_add(DL(m),d,DL(mr));
		}
		NEXT(mr) = 0; 
		MKND(NV(p),mr0,len,r);
		SG(r) = SG(p) + TD(d);
		return r;
	}
}

ND ndv_mul_nm(int mod,NM m0,NDV p)
{
	NM mr,mr0;
	NMV m;
	UINT *d,*dt,*dm;
	int c,n,td,i,c1,c2,len;
	P q;
	ND r;

	if ( !p ) return 0;
	else if ( do_weyl )
		if ( mod == -1 )
			error("ndv_mul_nm : not implemented (weyl)");
		else
			return weyl_ndv_mul_nm(mod,m0,p);
	else {
		n = NV(p); m = BDY(p);
		d = DL(m0);
		len = LEN(p);
		mr0 = 0;
		td = TD(d);
		if ( mod == -1 ) {
			c = CM(m0);
			for ( i = 0; i < len; i++, NMV_ADV(m) ) {
				NEXTNM(mr0,mr);
				CM(mr) = _mulsf(CM(m),c);
				ndl_add(DL(m),d,DL(mr));
			}
		} else if ( mod ) {
			c = CM(m0);
			for ( i = 0; i < len; i++, NMV_ADV(m) ) {
				NEXTNM(mr0,mr);
				c1 = CM(m);
				DMAR(c1,c,0,mod,c2);
				CM(mr) = c2;
				ndl_add(DL(m),d,DL(mr));
			}
		} else {
			q = CP(m0);
			for ( i = 0; i < len; i++, NMV_ADV(m) ) {
				NEXTNM(mr0,mr);
				mulp(nd_vc,CP(m),q,&CP(mr));
				ndl_add(DL(m),d,DL(mr));
			}
		}
		NEXT(mr) = 0; 
		MKND(NV(p),mr0,len,r);
		SG(r) = SG(p) + TD(d);
		return r;
	}
}

ND nd_quo(int mod,PGeoBucket bucket,NDV d)
{
	NM mq0,mq;
	NMV tm;
	Q q;
	int i,nv,sg,c,c1,c2,hindex;
	ND p,t,r;
	N tnm;

	if ( bucket->m < 0 ) return 0;
	else {
		nv = NV(d);
		mq0 = 0;
		tm = (NMV)ALLOCA(nmv_adv);
		while ( 1 ) {
			hindex = mod?head_pbucket(mod,bucket):head_pbucket_q(bucket);
			if ( hindex < 0 ) break;
			p = bucket->body[hindex];
			NEXTNM(mq0,mq);
			ndl_sub(HDL(p),HDL(d),DL(mq));
			ndl_copy(DL(mq),DL(tm));
			if ( mod ) {
				c1 = invm(HCM(d),mod); c2 = HCM(p);
				DMAR(c1,c2,0,mod,c); CM(mq) = c;
				CM(tm) = mod-c;
			} else {
				divsn(NM(HCQ(p)),NM(HCQ(d)),&tnm);
				NTOQ(tnm,SGN(HCQ(p))*SGN(HCQ(d)),CQ(mq));
				chsgnq(CQ(mq),&CQ(tm));
			}
			t = ndv_mul_nmv_trunc(mod,tm,d,HDL(d));
			bucket->body[hindex] = nd_remove_head(p);
			t = nd_remove_head(t);
			add_pbucket(mod,bucket,t);
		}
		if ( !mq0 )
			r = 0;
		else {
			NEXT(mq) = 0;
			for ( i = 0, mq = mq0; mq; mq = NEXT(mq), i++ );
			MKND(nv,mq0,i,r);
			/* XXX */
			SG(r) = HTD(r);
		}
		return r;
	}
}

void ndv_realloc(NDV p,int obpe,int oadv,EPOS oepos)
{
	NMV m,mr,mr0,t;
	int len,i,k;

	if ( !p ) return;
	m = BDY(p); len = LEN(p);
	mr0 = nmv_adv>oadv?(NMV)REALLOC(BDY(p),len*nmv_adv):BDY(p);
	m = (NMV)((char *)mr0+(len-1)*oadv);
	mr = (NMV)((char *)mr0+(len-1)*nmv_adv);
	t = (NMV)ALLOCA(nmv_adv);
	for ( i = 0; i < len; i++, NMV_OPREV(m), NMV_PREV(mr) ) {
		CQ(t) = CQ(m);
		for ( k = 0; k < nd_wpd; k++ ) DL(t)[k] = 0;
		ndl_reconstruct(DL(m),DL(t),obpe,oepos);
		CQ(mr) = CQ(t);
		ndl_copy(DL(t),DL(mr));
	}
	BDY(p) = mr0;
}

NDV ndv_dup_realloc(NDV p,int obpe,int oadv,EPOS oepos)
{
	NMV m,mr,mr0;
	int len,i;
	NDV r;

	if ( !p ) return 0;
	m = BDY(p); len = LEN(p);
	mr0 = mr = (NMV)MALLOC(len*nmv_adv);
	for ( i = 0; i < len; i++, NMV_OADV(m), NMV_ADV(mr) ) {
		ndl_zero(DL(mr));
		ndl_reconstruct(DL(m),DL(mr),obpe,oepos);
		CQ(mr) = CQ(m);
	}
	MKNDV(NV(p),mr0,len,r);
	SG(r) = SG(p);
	return r;
}

/* duplicate p */

NDV ndv_dup(int mod,NDV p)
{
	NDV d;
	NMV t,m,m0;
	int i,len;

	if ( !p ) return 0;
	len = LEN(p);
	m0 = m = (NMV)(mod?MALLOC_ATOMIC(len*nmv_adv):MALLOC(len*nmv_adv));
	for ( t = BDY(p), i = 0; i < len; i++, NMV_ADV(t), NMV_ADV(m) ) {
		ndl_copy(DL(t),DL(m));
		CQ(m) = CQ(t);
	}
	MKNDV(NV(p),m0,len,d);
	SG(d) = SG(p);
	return d;
}

ND nd_dup(ND p)
{
	ND d;
	NM t,m,m0;

	if ( !p ) return 0;
	for ( m0 = 0, t = BDY(p); t; t = NEXT(t) ) {
		NEXTNM(m0,m);
		ndl_copy(DL(t),DL(m));
		CQ(m) = CQ(t);
	}
	if ( m0 ) NEXT(m) = 0;
	MKND(NV(p),m0,LEN(p),d);
	SG(d) = SG(p);
	return d;
}

/* XXX if p->len == 0 then it represents 0 */

void ndv_mod(int mod,NDV p)
{
	NMV t,d;
	int r,s,u;
	int i,len,dlen;
	P cp;
	Q c;
	Obj gfs;

	if ( !p ) return;
	len = LEN(p);
	dlen = 0;
	if ( mod == -1 )
		for ( t = d = BDY(p), i = 0; i < len; i++, NMV_ADV(t) ) {
			simp_ff((Obj)CP(t),&gfs);
			r = FTOIF(CONT((GFS)gfs));
			CM(d) = r;
			ndl_copy(DL(t),DL(d));
			NMV_ADV(d);
			dlen++;	
		}
	else
		for ( t = d = BDY(p), i = 0; i < len; i++, NMV_ADV(t) ) {
			if ( nd_vc ) {
				nd_subst_vector(nd_vc,CP(t),nd_subst,&cp);
				c = (Q)cp;
			} else
				c = CQ(t);
			r = rem(NM(c),mod);
			if ( r ) {
				if ( SGN(c) < 0 )
					r = mod-r;
				if ( DN(c) ) {
					s = rem(DN(c),mod);
					if ( !s )
						error("ndv_mod : division by 0");
					s = invm(s,mod);
					DMAR(r,s,0,mod,u); r = u;
				}
				CM(d) = r;
				ndl_copy(DL(t),DL(d));
				NMV_ADV(d);
				dlen++;	
			}
		}
	LEN(p) = dlen;
}

NDV ptondv(VL vl,VL dvl,P p)
{
	ND nd;

	nd = ptond(vl,dvl,p);
	return ndtondv(0,nd);
}

ND ptond(VL vl,VL dvl,P p)
{
	int n,i,j,k,e;
	VL tvl;
	V v;
	DCP dc;
	DCP *w;
	ND r,s,t,u;
	P x;
	int c;
	UINT *d;
	NM m,m0;

	if ( !p )
		return 0;
	else if ( NUM(p) ) {
		NEWNM(m);
		ndl_zero(DL(m));
		CQ(m) = (Q)p;
		NEXT(m) = 0;
		MKND(nd_nvar,m,1,r);
		SG(r) = 0;
		return r;
	} else {
		for ( dc = DC(p), k = 0; dc; dc = NEXT(dc), k++ );
		w = (DCP *)ALLOCA(k*sizeof(DCP));
		for ( dc = DC(p), j = 0; j < k; dc = NEXT(dc), j++ ) w[j] = dc;
		for ( i = 0, tvl = dvl, v = VR(p);
			tvl && tvl->v != v; tvl = NEXT(tvl), i++ );
		if ( !tvl ) {
			for ( j = k-1, s = 0, MKV(v,x); j >= 0; j-- ) {
				t = ptond(vl,dvl,COEF(w[j]));
				pwrp(vl,x,DEG(w[j]),&p);
				nd_mul_c_p(CO,t,p); s = nd_add(0,s,t);
			}
			return s;
		} else {
			NEWNM(m0); d = DL(m0);
			for ( j = k-1, s = 0; j >= 0; j-- ) {
				ndl_zero(d); e = QTOS(DEG(w[j])); PUT_EXP(d,i,e);
				TD(d) = MUL_WEIGHT(e,i);
				if ( nd_blockmask) ndl_weight_mask(d);
				t = ptond(vl,dvl,COEF(w[j]));
				for ( m = BDY(t); m; m = NEXT(m) )
					ndl_addto(DL(m),d);
				SG(t) += TD(d);
				s = nd_add(0,s,t);
			}
			FREENM(m0);
			return s;
		}
	}
}

P ndvtop(int mod,VL vl,VL dvl,NDV p)
{
	VL tvl;
	int len,n,j,i,e;
	NMV m;
	Q q;
	P c;
	UINT *d;
	P s,r,u,t,w;
	GFS gfs;

	if ( !p ) return 0;
	else {
		len = LEN(p);
		n = NV(p);
		m = (NMV)(((char *)BDY(p))+nmv_adv*(len-1));
		for ( j = len-1, s = 0; j >= 0; j--, NMV_PREV(m) ) {
			if ( mod == -1 ) {
				e = IFTOF(CM(m)); MKGFS(e,gfs); c = (P)gfs;
			} else if ( mod ) {
				STOQ(CM(m),q); c = (P)q;
			} else
				c = CP(m);
			d = DL(m);
			for ( i = 0, t = c, tvl = dvl; i < n; tvl = NEXT(tvl), i++ ) {
				MKV(tvl->v,r); e = GET_EXP(d,i); STOQ(e,q);
				pwrp(vl,r,q,&u); mulp(vl,t,u,&w); t = w;
			}
			addp(vl,s,t,&u); s = u;
		}
		return s;
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
	if ( mod )
		m0 = m = (NMV)GC_malloc_atomic_ignore_off_page(len*nmv_adv);
	else
		m0 = m = MALLOC(len*nmv_adv);
#if 0
	ndv_alloc += nmv_adv*len;
#endif
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

void ndv_print(NDV p)
{
	NMV m;
	int i,len;
    
	if ( !p ) printf("0\n");
	else {
		len = LEN(p);
		for ( m = BDY(p), i = 0; i < len; i++, NMV_ADV(m) ) {
			if ( CM(m) & 0x80000000 ) printf("+@_%d*",IFTOF(CM(m)));
			else printf("+%d*",CM(m));
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
			printexpr(CO,(Obj)CQ(m));
			printf("*");
			ndl_print(DL(m));
		}
		printf("\n");
	}
}

NODE ndv_reducebase(NODE x)
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
			/* block order */
			/* XXX */
			nd_dcomp = -1;
			nd_isrlex = 0;
			ndl_compare_function = ndl_block_compare;
			break;
		case 2:
			/* matrix order */
			/* XXX */
			nd_dcomp = -1;
			nd_isrlex = 0;
			nd_matrix_len = ord->ord.matrix.row;
			nd_matrix = ord->ord.matrix.matrix;
			ndl_compare_function = ndl_matrix_compare;
			break;
		case 3:
			/* composite order */
			nd_dcomp = -1;
			nd_isrlex = 0;
			nd_worb_len = ord->ord.composite.length;
			nd_worb = ord->ord.composite.w_or_b;
			ndl_compare_function = ndl_composite_compare;
			break;
	}
	nd_ord = ord;
}

BlockMask nd_create_blockmask(struct order_spec *ord)
{
	int n,i,j,s,l;
	UINT *t;
	BlockMask bm;

	/* we only create mask table for block order */
	if ( ord->id != 1 )
		return 0;
	n = ord->ord.block.length;
	bm = (BlockMask)MALLOC(sizeof(struct oBlockMask));
	bm->n = n;
	bm->order_pair = ord->ord.block.order_pair;
	bm->mask = (UINT **)MALLOC(n*sizeof(UINT *));
	for ( i = 0, s = 0; i < n; i++ ) {
		bm->mask[i] = t = (UINT *)MALLOC_ATOMIC(nd_wpd*sizeof(UINT));
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
			/* matrix order */
		case 3:
			/* composite order */
			for ( i = 0; i < nd_nvar; i++ ) {
				epos[i].i = nd_exporigin + i/nd_epw;
				epos[i].s = (nd_epw-(i%nd_epw)-1)*nd_bpe;
			}
			break;
	}
	return epos;
}

/* external interface */

void nd_nf_p(P f,LIST g,LIST v,int m,struct order_spec *ord,P *rp)
{
	NODE t,in0,in;
	ND nd,nf;
	NDV ndv;
	VL vv,tv;
	int stat,nvar,max,e;
	union oNDC dn;
	Q cont;
	P pp;

	if ( !f ) {
		*rp = 0;
		return;
	}
	pltovl(v,&vv);
	for ( nvar = 0, tv = vv; tv; tv = NEXT(tv), nvar++ );

	/* get the degree bound */
	for ( t = BDY(g), max = 0; t; t = NEXT(t) )
		for ( tv = vv; tv; tv = NEXT(tv) ) {
			e = getdeg(tv->v,(P)BDY(t));
			max = MAX(e,max);
		}
	for ( tv = vv; tv; tv = NEXT(tv) ) {
		e = getdeg(tv->v,f);
		max = MAX(e,max);
	}

	nd_init_ord(ord);
	nd_setup_parameters(nvar,max);

	/* conversion to ndv */
	for ( in0 = 0, t = BDY(g); t; t = NEXT(t) ) {
		NEXTNODE(in0,in);
		ptozp((P)BDY(t),1,&cont,&pp);
		BDY(in) = (pointer)ptondv(CO,vv,pp);
		if ( m ) ndv_mod(m,(NDV)BDY(in));
	}
	NEXTNODE(in0,in);
	BDY(in) = (pointer)ptondv(CO,vv,f);
	if ( m ) ndv_mod(m,(NDV)BDY(in));
	NEXT(in) = 0;

	/* dont sort, dont removecont */
	ndv_setup(m,0,in0,1,1);
	nd_psn--;
	nd_scale=2;
	while ( 1 ) {
		nd = (pointer)ndvtond(m,nd_ps[nd_psn]);
		stat = nd_nf(m,0,nd,nd_ps,1,0,&nf);
		if ( !stat ) {
			nd_psn++;
			nd_reconstruct(0,0);
			nd_psn--;
		} else
			break;
	}
	*rp = ndvtop(m,CO,vv,ndtondv(m,nf));
}

int nd_to_vect(int mod,UINT *s0,int n,ND d,UINT *r)
{
	NM m;
	UINT *t,*s;
	int i;

	for ( i = 0; i < n; i++ ) r[i] = 0;
	for ( i = 0, s = s0, m = BDY(d); m; m = NEXT(m) ) {
		t = DL(m);
		for ( ; !ndl_equal(t,s); s += nd_wpd, i++ );
		r[i] = CM(m);
	}
	for ( i = 0; !r[i]; i++ );
	return i;
}

int nd_to_vect_q(UINT *s0,int n,ND d,Q *r)
{
	NM m;
	UINT *t,*s;
	int i;

	for ( i = 0; i < n; i++ ) r[i] = 0;
	for ( i = 0, s = s0, m = BDY(d); m; m = NEXT(m) ) {
		t = DL(m);
		for ( ; !ndl_equal(t,s); s += nd_wpd, i++ );
		r[i] = CQ(m);
	}
	for ( i = 0; !r[i]; i++ );
	return i;
}

Q *nm_ind_pair_to_vect(int mod,UINT *s0,int n,NM_ind_pair pair)
{
	NM m;
	NMV mr;
	UINT *d,*t,*s;
	NDV p;
	int i,j,len;
	Q *r;

	m = pair->mul;
	d = DL(m);
	p = nd_ps[pair->index];
	len = LEN(p);
	r = (Q *)CALLOC(n,sizeof(Q));
	t = (UINT *)ALLOCA(nd_wpd*sizeof(UINT));
	for ( i = j = 0, s = s0, mr = BDY(p); j < len; j++, NMV_ADV(mr) ) {
		ndl_add(d,DL(mr),t);	
		for ( ; !ndl_equal(t,s); s += nd_wpd, i++ );
		r[i] = CQ(mr);
	}
	return r;
}

IndArray nm_ind_pair_to_vect_compress(int mod,UINT *s0,int n,NM_ind_pair pair)
{
	NM m;
	NMV mr;
	UINT *d,*t,*s;
	NDV p;
	unsigned char *ivc;
	unsigned short *ivs;
	UINT *v,*ivi,*s0v;
	int i,j,len,prev,diff,cdiff;
	IndArray r;

	m = pair->mul;
	d = DL(m);
	p = nd_ps[pair->index];
	len = LEN(p);
	t = (UINT *)ALLOCA(nd_wpd*sizeof(UINT));
	v = (unsigned int *)ALLOCA(len*sizeof(unsigned int));
	for ( i = j = 0, s = s0, mr = BDY(p); j < len; j++, NMV_ADV(mr) ) {
		ndl_add(d,DL(mr),t);	
		for ( ; !ndl_equal(t,s); s += nd_wpd, i++ );
		v[j] = i;
	}
	r = (IndArray)MALLOC(sizeof(struct oIndArray));
	r->head = v[0];
	diff = 0;
	for ( i = 1; i < len; i++ ) {
		cdiff = v[i]-v[i-1]; diff = MAX(cdiff,diff);
	}
	if ( diff < 256 ) {
		r->width = 1;
		ivc = (unsigned char *)MALLOC_ATOMIC(len*sizeof(unsigned char));
		r->index.c = ivc;
		for ( i = 1, ivc[0] = 0; i < len; i++ ) ivc[i] = v[i]-v[i-1];
	} else if ( diff < 65536 ) {
		r->width = 2;
		ivs = (unsigned short *)MALLOC_ATOMIC(len*sizeof(unsigned short));
		r->index.s = ivs;
		for ( i = 1, ivs[0] = 0; i < len; i++ ) ivs[i] = v[i]-v[i-1];
	} else {
		r->width = 4;
		ivi = (unsigned int *)MALLOC_ATOMIC(len*sizeof(unsigned int));
		r->index.i = ivi;
		for ( i = 1, ivi[0] = 0; i < len; i++ ) ivi[i] = v[i]-v[i-1];
	}
	return r;
}

int compress_array(Q *svect,Q *cvect,int n)
{
	int i,j;

	for ( i = j = 0; i < n; i++ )
		if ( svect[i] ) cvect[j++] = svect[i];
	return j;
}

void expand_array(Q *svect,Q *cvect,int n)
{
	int i,j;

	for ( i = j = 0; j < n;  i++  )
		if ( svect[i] ) svect[i] = cvect[j++];
}

int ndv_reduce_vect_q(Q *svect,int trace,int col,IndArray *imat,NM_ind_pair *rp0,int nred)
{
	int i,j,k,len,pos,prev,nz;
	Q cs,mcs,c1,c2,cr,gcd,t;
	IndArray ivect;
	unsigned char *ivc;
	unsigned short *ivs;
	unsigned int *ivi;
	NDV redv;
	NMV mr;
	NODE rp;
	int maxrs;
	double hmag;
	Q *cvect;

	maxrs = 0;
	for ( i = 0; i < col && !svect[i]; i++ );
	if ( i == col ) return maxrs;
	hmag = p_mag((P)svect[i])*nd_scale;
	cvect = (Q *)ALLOCA(col*sizeof(Q));
	for ( i = 0; i < nred; i++ ) {
		ivect = imat[i];
		k = ivect->head;
		if ( svect[k] ) {
			maxrs = MAX(maxrs,rp0[i]->sugar);
			redv = trace?nd_ps_trace[rp0[i]->index]:nd_ps[rp0[i]->index];
			len = LEN(redv); mr = BDY(redv);
			igcd_cofactor(svect[k],CQ(mr),&gcd,&cs,&cr);
			chsgnq(cs,&mcs);
			if ( !UNIQ(cr) ) {
				for ( j = 0; j < col; j++ ) {
					mulq(svect[j],cr,&c1); svect[j] = c1;
				}
			}
			svect[k] = 0; prev = k;
			switch ( ivect->width ) {
				case 1:
					ivc = ivect->index.c;
					for ( j = 1, NMV_ADV(mr); j < len; j++, NMV_ADV(mr) ) {
						pos = prev+ivc[j]; prev = pos;
						mulq(CQ(mr),mcs,&c2); addq(svect[pos],c2,&t); svect[pos] = t;
					}
					break;
				case 2:
					ivs = ivect->index.s; 
					for ( j = 1, NMV_ADV(mr); j < len; j++, NMV_ADV(mr) ) {
						pos = prev+ivs[j]; prev = pos;
						mulq(CQ(mr),mcs,&c2); addq(svect[pos],c2,&t); svect[pos] = t;
					}
					break;
				case 4:
					ivi = ivect->index.i;
					for ( j = 1, NMV_ADV(mr); j < len; j++, NMV_ADV(mr) ) {
						pos = prev+ivi[j]; prev = pos;
						mulq(CQ(mr),mcs,&c2); addq(svect[pos],c2,&t); svect[pos] = t;
					}
					break;
			}
			for ( j = k+1; j < col && !svect[j]; j++ );
			if ( j == col ) break;
			if ( hmag && ((double)p_mag((P)svect[j]) > hmag) ) {
				nz = compress_array(svect,cvect,col);
				removecont_array((P *)cvect,nz,1);
				expand_array(svect,cvect,nz);
				hmag = ((double)p_mag((P)svect[j]))*nd_scale;
			}
		}
	}
	nz = compress_array(svect,cvect,col);
	removecont_array((P *)cvect,nz,1);
	expand_array(svect,cvect,nz);
	if ( DP_Print ) { 
		fprintf(asir_out,"-"); fflush(asir_out);
	}
	return maxrs;
}

int ndv_reduce_vect(int m,UINT *svect,int col,IndArray *imat,NM_ind_pair *rp0,int nred)
{
	int i,j,k,len,pos,prev;
	UINT c,c1,c2,c3,up,lo,dmy;
	IndArray ivect;
	unsigned char *ivc;
	unsigned short *ivs;
	unsigned int *ivi;
	NDV redv;
	NMV mr;
	NODE rp;
	int maxrs;

	maxrs = 0;
	for ( i = 0; i < nred; i++ ) {
		ivect = imat[i];
		k = ivect->head; svect[k] %= m;
		if ( c = svect[k] ) {
			maxrs = MAX(maxrs,rp0[i]->sugar);
			c = m-c; redv = nd_ps[rp0[i]->index];
			len = LEN(redv); mr = BDY(redv);
			svect[k] = 0; prev = k;
			switch ( ivect->width ) {
				case 1:
					ivc = ivect->index.c;
					for ( j = 1, NMV_ADV(mr); j < len; j++, NMV_ADV(mr) ) {
						pos = prev+ivc[j]; c1 = CM(mr); c2 = svect[pos];
						prev = pos;
						DMA(c1,c,c2,up,lo);
						if ( up ) { DSAB(m,up,lo,dmy,c3); svect[pos] = c3;
						} else svect[pos] = lo;
					}
					break;
				case 2:
					ivs = ivect->index.s; 
					for ( j = 1, NMV_ADV(mr); j < len; j++, NMV_ADV(mr) ) {
						pos = prev+ivs[j]; c1 = CM(mr); c2 = svect[pos];
						prev = pos;
						DMA(c1,c,c2,up,lo);
						if ( up ) { DSAB(m,up,lo,dmy,c3); svect[pos] = c3;
						} else svect[pos] = lo;
					}
					break;
				case 4:
					ivi = ivect->index.i;
					for ( j = 1, NMV_ADV(mr); j < len; j++, NMV_ADV(mr) ) {
						pos = prev+ivi[j]; c1 = CM(mr); c2 = svect[pos];
						prev = pos;
						DMA(c1,c,c2,up,lo);
						if ( up ) { DSAB(m,up,lo,dmy,c3); svect[pos] = c3;
						} else svect[pos] = lo;
					}
					break;
			}
		}
	}
	for ( i = 0; i < col; i++ )
		if ( svect[i] >= (UINT)m ) svect[i] %= m;
	return maxrs;
}

int ndv_reduce_vect_sf(int m,UINT *svect,int col,IndArray *imat,NM_ind_pair *rp0,int nred)
{
	int i,j,k,len,pos,prev;
	UINT c,c1,c2,c3,up,lo,dmy;
	IndArray ivect;
	unsigned char *ivc;
	unsigned short *ivs;
	unsigned int *ivi;
	NDV redv;
	NMV mr;
	NODE rp;
	int maxrs;

	maxrs = 0;
	for ( i = 0; i < nred; i++ ) {
		ivect = imat[i];
		k = ivect->head; svect[k] %= m;
		if ( c = svect[k] ) {
			maxrs = MAX(maxrs,rp0[i]->sugar);
			c = _chsgnsf(c); redv = nd_ps[rp0[i]->index];
			len = LEN(redv); mr = BDY(redv);
			svect[k] = 0; prev = k;
			switch ( ivect->width ) {
				case 1:
					ivc = ivect->index.c;
					for ( j = 1, NMV_ADV(mr); j < len; j++, NMV_ADV(mr) ) {
						pos = prev+ivc[j]; prev = pos;
						svect[pos] = _addsf(_mulsf(CM(mr),c),svect[pos]);
					}
					break;
				case 2:
					ivs = ivect->index.s; 
					for ( j = 1, NMV_ADV(mr); j < len; j++, NMV_ADV(mr) ) {
						pos = prev+ivs[j]; prev = pos;
						svect[pos] = _addsf(_mulsf(CM(mr),c),svect[pos]);
					}
					break;
				case 4:
					ivi = ivect->index.i;
					for ( j = 1, NMV_ADV(mr); j < len; j++, NMV_ADV(mr) ) {
						pos = prev+ivi[j]; prev = pos;
						svect[pos] = _addsf(_mulsf(CM(mr),c),svect[pos]);
					}
					break;
			}
		}
	}
	return maxrs;
}

NDV vect_to_ndv(UINT *vect,int spcol,int col,int *rhead,UINT *s0vect)
{
	int j,k,len;
	UINT *p;
	UINT c;
	NDV r;
	NMV mr0,mr;

	for ( j = 0, len = 0; j < spcol; j++ ) if ( vect[j] ) len++;
	if ( !len ) return 0;
	else {
		mr0 = (NMV)GC_malloc_atomic_ignore_off_page(nmv_adv*len);
#if 0
		ndv_alloc += nmv_adv*len;
#endif
		mr = mr0; 
		p = s0vect;
		for ( j = k = 0; j < col; j++, p += nd_wpd )
			if ( !rhead[j] ) {
				if ( c = vect[k++] ) {
					ndl_copy(p,DL(mr)); CM(mr) = c; NMV_ADV(mr);
				}
			}
		MKNDV(nd_nvar,mr0,len,r);
		return r;
	}
}

/* for preprocessed vector */

NDV vect_to_ndv_q(Q *vect,int spcol,int col,int *rhead,UINT *s0vect)
{
	int j,k,len;
	UINT *p;
	Q c;
	NDV r;
	NMV mr0,mr;

	for ( j = 0, len = 0; j < spcol; j++ ) if ( vect[j] ) len++;
	if ( !len ) return 0;
	else {
		mr0 = (NMV)GC_malloc(nmv_adv*len);
#if 0
		ndv_alloc += nmv_adv*len;
#endif
		mr = mr0; 
		p = s0vect;
		for ( j = k = 0; j < col; j++, p += nd_wpd )
			if ( !rhead[j] ) {
				if ( c = vect[k++] ) {
					if ( DN(c) )
						error("afo");
					ndl_copy(p,DL(mr)); CQ(mr) = c; NMV_ADV(mr);
				}
			}
		MKNDV(nd_nvar,mr0,len,r);
		return r;
	}
}

/* for plain vector */

NDV plain_vect_to_ndv_q(Q *vect,int col,UINT *s0vect)
{
	int j,k,len;
	UINT *p;
	Q c;
	NDV r;
	NMV mr0,mr;

	for ( j = 0, len = 0; j < col; j++ ) if ( vect[j] ) len++;
	if ( !len ) return 0;
	else {
		mr0 = (NMV)GC_malloc(nmv_adv*len);
#if 0
		ndv_alloc += nmv_adv*len;
#endif
		mr = mr0; 
		p = s0vect;
		for ( j = k = 0; j < col; j++, p += nd_wpd, k++ )
			if ( c = vect[k] ) {
				if ( DN(c) )
					error("afo");
				ndl_copy(p,DL(mr)); CQ(mr) = c; NMV_ADV(mr);
			}
		MKNDV(nd_nvar,mr0,len,r);
		return r;
	}
}

int nd_sp_f4(int m,int trace,ND_pairs l,PGeoBucket bucket)
{
	ND_pairs t;
	NODE sp0,sp;
	int stat;
	ND spol;

	for ( t = l; t; t = NEXT(t) ) {
		stat = nd_sp(m,trace,t,&spol);
		if ( !stat ) return 0;
		if ( spol ) {
			add_pbucket_symbolic(bucket,spol);
		}
	}
	return 1;
}

int nd_symbolic_preproc(PGeoBucket bucket,int trace,UINT **s0vect,NODE *r)
{
	NODE rp0,rp;
	NM mul,head,s0,s;
	int index,col,i,sugar;
	RHist h;
	UINT *s0v,*p;
	NM_ind_pair pair;
	ND red;
	NDV *ps;

	s0 = 0; rp0 = 0; col = 0;
	ps = trace?nd_ps_trace:nd_ps;
	while ( 1 ) {
		head = remove_head_pbucket_symbolic(bucket);
		if ( !head ) break;
		if ( !s0 ) s0 = head;
		else NEXT(s) = head;
		s = head;
		index = ndl_find_reducer(DL(head));
		if ( index >= 0 ) {
			h = nd_psh[index];
			NEWNM(mul);
			ndl_sub(DL(head),DL(h),DL(mul));
			if ( ndl_check_bound2(index,DL(mul)) ) return 0;
			sugar = TD(DL(mul))+SG(ps[index]);
			MKNM_ind_pair(pair,mul,index,sugar);
			red = ndv_mul_nm_symbolic(mul,ps[index]);
			add_pbucket_symbolic(bucket,nd_remove_head(red));
			NEXTNODE(rp0,rp); BDY(rp) = (pointer)pair;
		}
		col++;
	}
	if ( rp0 ) NEXT(rp) = 0;
	NEXT(s) = 0;
	s0v = (UINT *)MALLOC_ATOMIC(col*nd_wpd*sizeof(UINT));
	for ( i = 0, p = s0v, s = s0; i < col;
		i++, p += nd_wpd, s = NEXT(s) ) ndl_copy(DL(s),p);
	*s0vect = s0v;		
	*r = rp0;
	return col;
}

NODE nd_f4(int m)
{
	int i,nh,stat,index;
	NODE r,g;
	ND_pairs d,l,t;
	ND spol,red;
	NDV nf,redv;
	NM s0,s;
	NODE rp0,srp0,nflist;
	int nsp,nred,col,rank,len,k,j,a;
	UINT c;
	UINT **spmat;
	UINT *s0vect,*svect,*p,*v;
	int *colstat;
	IndArray *imat;
	int *rhead;
	int spcol,sprow;
	int sugar;
	PGeoBucket bucket;
	struct oEGT eg0,eg1,eg_f4;

#if 0
	ndv_alloc = 0;
#endif
	g = 0; d = 0;
	for ( i = 0; i < nd_psn; i++ ) {
		d = update_pairs(d,g,i);
		g = update_base(g,i);
	}
	while ( d ) {
		get_eg(&eg0);
		l = nd_minsugarp(d,&d);
		sugar = SG(l);
		bucket = create_pbucket();
		stat = nd_sp_f4(m,0,l,bucket);
		if ( !stat ) {
			for ( t = l; NEXT(t); t = NEXT(t) );
			NEXT(t) = d; d = l;
			d = nd_reconstruct(0,d);
			continue;
		}
		if ( bucket->m < 0 ) continue;
		col = nd_symbolic_preproc(bucket,0,&s0vect,&rp0);
		if ( !col ) {
			for ( t = l; NEXT(t); t = NEXT(t) );
			NEXT(t) = d; d = l;
			d = nd_reconstruct(0,d);
			continue;
		}
		get_eg(&eg1); init_eg(&eg_f4); add_eg(&eg_f4,&eg0,&eg1);
		if ( DP_Print )
			fprintf(asir_out,"sugar=%d,symb=%fsec,",
				sugar,eg_f4.exectime+eg_f4.gctime);
		if ( 1 )
			nflist = nd_f4_red(m,l,0,s0vect,col,rp0,0);
		else
			nflist = nd_f4_red_dist(m,l,s0vect,col,rp0,0);
		/* adding new bases */
		for ( r = nflist; r; r = NEXT(r) ) {
			nf = (NDV)BDY(r);
			ndv_removecont(m,nf);
			if ( !m && nd_nalg ) {
				ND nf1;

				nf1 = ndvtond(m,nf);
				nd_monic(0,&nf1);
				nd_removecont(m,nf1);
				nf = ndtondv(m,nf1);
			}
			nh = ndv_newps(m,nf,0);
			d = update_pairs(d,g,nh);
			g = update_base(g,nh);
		}
	}
	for ( r = g; r; r = NEXT(r) ) BDY(r) = (pointer)nd_ps[(int)BDY(r)];
#if 0
	fprintf(asir_out,"ndv_alloc=%d\n",ndv_alloc);
#endif
	return g;
}

NODE nd_f4_trace(int m)
{
	int i,nh,stat,index;
	NODE r,g;
	ND_pairs d,l,l0,t;
	ND spol,red;
	NDV nf,redv,nfqv,nfv;
	NM s0,s;
	NODE rp0,srp0,nflist;
	int nsp,nred,col,rank,len,k,j,a;
	UINT c;
	UINT **spmat;
	UINT *s0vect,*svect,*p,*v;
	int *colstat;
	IndArray *imat;
	int *rhead;
	int spcol,sprow;
	int sugar;
	PGeoBucket bucket;
	struct oEGT eg0,eg1,eg_f4;

	g = 0; d = 0;
	for ( i = 0; i < nd_psn; i++ ) {
		d = update_pairs(d,g,i);
		g = update_base(g,i);
	}
	while ( d ) {
		get_eg(&eg0);
		l = nd_minsugarp(d,&d);
		sugar = SG(l);
		bucket = create_pbucket();
		stat = nd_sp_f4(m,0,l,bucket);
		if ( !stat ) {
			for ( t = l; NEXT(t); t = NEXT(t) );
			NEXT(t) = d; d = l;
			d = nd_reconstruct(1,d);
			continue;
		}
		if ( bucket->m < 0 ) continue;
		col = nd_symbolic_preproc(bucket,0,&s0vect,&rp0);
		if ( !col ) {
			for ( t = l; NEXT(t); t = NEXT(t) );
			NEXT(t) = d; d = l;
			d = nd_reconstruct(1,d);
			continue;
		}
		get_eg(&eg1); init_eg(&eg_f4); add_eg(&eg_f4,&eg0,&eg1);
		if ( DP_Print )
			fprintf(asir_out,"sugar=%d,symb=%fsec,",
				sugar,eg_f4.exectime+eg_f4.gctime);
		nflist = nd_f4_red(m,l,0,s0vect,col,rp0,&l0);
		if ( !l0 ) continue;
		l = l0;

		/* over Q */
		bucket = create_pbucket();
		stat = nd_sp_f4(0,1,l,bucket);
		if ( !stat ) {
			for ( t = l; NEXT(t); t = NEXT(t) );
			NEXT(t) = d; d = l;
			d = nd_reconstruct(1,d);
			continue;
		}
		if ( bucket->m < 0 ) continue;
		col = nd_symbolic_preproc(bucket,1,&s0vect,&rp0);
		if ( !col ) {
			for ( t = l; NEXT(t); t = NEXT(t) );
			NEXT(t) = d; d = l;
			d = nd_reconstruct(1,d);
			continue;
		}
		nflist = nd_f4_red(0,l,1,s0vect,col,rp0,0);
		/* adding new bases */
		for ( r = nflist; r; r = NEXT(r) ) {
			nfqv = (NDV)BDY(r);
			ndv_removecont(0,nfqv);
			if ( !rem(NM(HCQ(nfqv)),m) ) return 0;
			if ( nd_nalg ) {
				ND nf1;

				nf1 = ndvtond(m,nfqv);
				nd_monic(0,&nf1);
				nd_removecont(0,nf1);
				nfqv = ndtondv(0,nf1); nd_free(nf1);
			}
			nfv = ndv_dup(0,nfqv);
			ndv_mod(m,nfv);
			ndv_removecont(m,nfv);
			nh = ndv_newps(0,nfv,nfqv);
			d = update_pairs(d,g,nh);
			g = update_base(g,nh);
		}
	}
	for ( r = g; r; r = NEXT(r) ) BDY(r) = (pointer)nd_ps_trace[(int)BDY(r)];
#if 0
	fprintf(asir_out,"ndv_alloc=%d\n",ndv_alloc);
#endif
	return g;
}

NODE nd_f4_red(int m,ND_pairs sp0,int trace,UINT *s0vect,int col,NODE rp0,ND_pairs *nz)
{
	IndArray *imat;
	int nsp,nred,i;
	int *rhead;
	NODE r0,rp;
	ND_pairs sp;
	NM_ind_pair *rvect;

	for ( sp = sp0, nsp = 0; sp; sp = NEXT(sp), nsp++ );
	nred = length(rp0);
	imat = (IndArray *)ALLOCA(nred*sizeof(IndArray));
	rhead = (int *)ALLOCA(col*sizeof(int));
	for ( i = 0; i < col; i++ ) rhead[i] = 0;

	/* construction of index arrays */
	rvect = (NM_ind_pair *)ALLOCA(nred*sizeof(NM_ind_pair));
	for ( rp = rp0, i = 0; rp; i++, rp = NEXT(rp) ) {
		rvect[i] = (NM_ind_pair)BDY(rp);
		imat[i] = nm_ind_pair_to_vect_compress(m,s0vect,col,rvect[i]);
		rhead[imat[i]->head] = 1;
	}
	if ( m )
		r0 = nd_f4_red_main(m,sp0,nsp,s0vect,col,rvect,rhead,imat,nred,nz);
	else
		r0 = nd_f4_red_q_main(sp0,nsp,trace,s0vect,col,rvect,rhead,imat,nred);
	return r0;
}

NODE nd_f4_red_main(int m,ND_pairs sp0,int nsp,UINT *s0vect,int col,
        NM_ind_pair *rvect,int *rhead,IndArray *imat,int nred,ND_pairs *nz)
{
	int spcol,sprow,a;
	int i,j,k,l,rank;
	NODE r0,r;
	ND_pairs sp;
	ND spol;
	int **spmat;
	UINT *svect,*v;
	int *colstat;
	struct oEGT eg0,eg1,eg2,eg_f4,eg_f4_1,eg_f4_2;
	int maxrs;
	int *spsugar;
	ND_pairs *spactive;

	spcol = col-nred;
	get_eg(&eg0);
	/* elimination (1st step) */
	spmat = (int **)ALLOCA(nsp*sizeof(UINT *));
	svect = (UINT *)ALLOCA(col*sizeof(UINT));
	spsugar = (int *)ALLOCA(nsp*sizeof(UINT));
	spactive = !nz?0:(ND_pairs *)ALLOCA(nsp*sizeof(ND_pairs));
	for ( a = sprow = 0, sp = sp0; a < nsp; a++, sp = NEXT(sp) ) {
		nd_sp(m,0,sp,&spol);
		if ( !spol ) continue;
		nd_to_vect(m,s0vect,col,spol,svect);
		if ( m == -1 ) 
			maxrs = ndv_reduce_vect_sf(m,svect,col,imat,rvect,nred);
		else
			maxrs = ndv_reduce_vect(m,svect,col,imat,rvect,nred);
		for ( i = 0; i < col; i++ ) if ( svect[i] ) break;
		if ( i < col ) {
			spmat[sprow] = v = (UINT *)MALLOC_ATOMIC(spcol*sizeof(UINT));
			for ( j = k = 0; j < col; j++ )
				if ( !rhead[j] ) v[k++] = svect[j];
			spsugar[sprow] = MAX(maxrs,SG(spol));
			if ( nz )
			spactive[sprow] = sp;
			sprow++;
		}
		nd_free(spol);
	}
	get_eg(&eg1); init_eg(&eg_f4_1); add_eg(&eg_f4_1,&eg0,&eg1);
	if ( DP_Print ) {
		fprintf(asir_out,"elim1=%fsec,",eg_f4_1.exectime+eg_f4_1.gctime);
		fflush(asir_out);
	}
	/* free index arrays */
	for ( i = 0; i < nred; i++ ) GC_free(imat[i]->index.c);

	/* elimination (2nd step) */
	colstat = (int *)ALLOCA(spcol*sizeof(int));
	if ( m == -1 )
		rank = nd_gauss_elim_sf(spmat,spsugar,sprow,spcol,m,colstat);
	else
		rank = nd_gauss_elim_mod(spmat,spsugar,spactive,sprow,spcol,m,colstat);
	r0 = 0;
	for ( i = 0; i < rank; i++ ) {
		NEXTNODE(r0,r); BDY(r) = 
			(pointer)vect_to_ndv(spmat[i],spcol,col,rhead,s0vect);
		SG((NDV)BDY(r)) = spsugar[i];
		GC_free(spmat[i]);
	}
	if ( r0 ) NEXT(r) = 0;

	for ( ; i < sprow; i++ ) GC_free(spmat[i]);
	get_eg(&eg2); init_eg(&eg_f4_2); add_eg(&eg_f4_2,&eg1,&eg2);
	init_eg(&eg_f4); add_eg(&eg_f4,&eg0,&eg2);
	if ( DP_Print ) {
		fprintf(asir_out,"elim2=%fsec\n",eg_f4_2.exectime+eg_f4_2.gctime);
		fprintf(asir_out,"nsp=%d,nred=%d,spmat=(%d,%d),rank=%d  ",
			nsp,nred,sprow,spcol,rank);
		fprintf(asir_out,"%fsec\n",eg_f4.exectime+eg_f4.gctime);
	}
	if ( nz ) {
		for ( i = 0; i < rank-1; i++ ) NEXT(spactive[i]) = spactive[i+1];
		if ( rank > 0 ) {
			NEXT(spactive[rank-1]) = 0;
			*nz = spactive[0];
		} else
			*nz = 0;
	}
	return r0;
}

#if 1
NODE nd_f4_red_q_main(ND_pairs sp0,int nsp,int trace,UINT *s0vect,int col,
        NM_ind_pair *rvect,int *rhead,IndArray *imat,int nred)
{
	int spcol,sprow,a;
	int i,j,k,l,rank;
	NODE r0,r;
	ND_pairs sp;
	ND spol;
	Q **spmat;
	Q *svect,*v;
	int *colstat;
	struct oEGT eg0,eg1,eg2,eg_f4,eg_f4_1,eg_f4_2;
	int maxrs;
	int *spsugar;
	pointer *w;

	spcol = col-nred;
	get_eg(&eg0);
	/* elimination (1st step) */
	spmat = (Q **)ALLOCA(nsp*sizeof(Q *));
	svect = (Q *)ALLOCA(col*sizeof(Q));
	spsugar = (int *)ALLOCA(nsp*sizeof(Q));
	for ( a = sprow = 0, sp = sp0; a < nsp; a++, sp = NEXT(sp) ) {
		nd_sp(0,trace,sp,&spol);
		if ( !spol ) continue;
		nd_to_vect_q(s0vect,col,spol,svect);
		maxrs = ndv_reduce_vect_q(svect,trace,col,imat,rvect,nred);
		for ( i = 0; i < col; i++ ) if ( svect[i] ) break;
		if ( i < col ) {
			spmat[sprow] = v = (Q *)MALLOC(spcol*sizeof(Q));
			for ( j = k = 0; j < col; j++ )
				if ( !rhead[j] ) v[k++] = svect[j];
			spsugar[sprow] = MAX(maxrs,SG(spol));
			sprow++;
		}
/*		nd_free(spol); */
	}
	get_eg(&eg1); init_eg(&eg_f4_1); add_eg(&eg_f4_1,&eg0,&eg1);
	if ( DP_Print ) {
		fprintf(asir_out,"elim1=%fsec,",eg_f4_1.exectime+eg_f4_1.gctime);
		fflush(asir_out);
	}
	/* free index arrays */
/*	for ( i = 0; i < nred; i++ ) GC_free(imat[i]->index.c); */

	/* elimination (2nd step) */
	colstat = (int *)ALLOCA(spcol*sizeof(int));
	rank = nd_gauss_elim_q(spmat,spsugar,sprow,spcol,colstat);
	w = (pointer *)ALLOCA(rank*sizeof(pointer));
	for ( i = 0; i < rank; i++ ) {
		w[rank-i-1] = (pointer)vect_to_ndv_q(spmat[i],spcol,col,rhead,s0vect);
		SG((NDV)w[rank-i-1]) = spsugar[i];
/*		GC_free(spmat[i]); */
	}
#if 0
	qsort(w,rank,sizeof(NDV),
		(int (*)(const void *,const void *))ndv_compare);
#endif
	r0 = 0;
	for ( i = 0; i < rank; i++ ) {
		NEXTNODE(r0,r); BDY(r) = w[i];
	}
	if ( r0 ) NEXT(r) = 0;

/*	for ( ; i < sprow; i++ ) GC_free(spmat[i]); */
	get_eg(&eg2); init_eg(&eg_f4_2); add_eg(&eg_f4_2,&eg1,&eg2);
	init_eg(&eg_f4); add_eg(&eg_f4,&eg0,&eg2);
	if ( DP_Print ) {
		fprintf(asir_out,"elim2=%fsec\n",eg_f4_2.exectime+eg_f4_2.gctime);
		fprintf(asir_out,"nsp=%d,nred=%d,spmat=(%d,%d),rank=%d  ",
			nsp,nred,sprow,spcol,rank);
		fprintf(asir_out,"%fsec\n",eg_f4.exectime+eg_f4.gctime);
	}
	return r0;
}
#else
void printm(Q **mat,int row,int col)
{
	int i,j;
	printf("[");
	for ( i = 0; i < row; i++ ) {
		for ( j = 0; j < col; j++ ) {
			printexpr(CO,mat[i][j]); printf(" ");
		}
		printf("]\n");
	}
}

NODE nd_f4_red_q_main(ND_pairs sp0,int nsp,UINT *s0vect,int col,
        NM_ind_pair *rvect,int *rhead,IndArray *imat,int nred)
{
	int row,a;
	int i,j,rank;
	NODE r0,r;
	ND_pairs sp;
	ND spol;
	Q **mat;
	int *colstat;
	int *sugar;

	row = nsp+nred;
	/* make the matrix */
	mat = (Q **)ALLOCA(row*sizeof(Q *));
	sugar = (int *)ALLOCA(row*sizeof(int));
	for ( row = a = 0, sp = sp0; a < nsp; a++, sp = NEXT(sp) ) {
		nd_sp(0,0,sp,&spol);
		if ( !spol ) continue;
		mat[row] = (Q *)MALLOC(col*sizeof(Q));
		nd_to_vect_q(s0vect,col,spol,mat[row]);
		sugar[row] = SG(spol);
		row++;
	}
	for ( i = 0; i < nred; i++, row++ ) {
		mat[row] = nm_ind_pair_to_vect(0,s0vect,col,rvect[i]);
		sugar[row] = rvect[i]->sugar;
	}
	/* elimination */
	colstat = (int *)ALLOCA(col*sizeof(int));
	rank = nd_gauss_elim_q(mat,sugar,row,col,colstat);
	r0 = 0;
	for ( i = 0; i < rank; i++ ) {
		for ( j = 0; j < col; j++ ) if ( mat[i][j] ) break;
		if ( j == col ) error("nd_f4_red_q_main : cannot happen");
		if ( rhead[j] ) continue;
		NEXTNODE(r0,r); BDY(r) = 
			(pointer)plain_vect_to_ndv_q(mat[i],col,s0vect);
		SG((NDV)BDY(r)) = sugar[i];
	}
	if ( r0 ) NEXT(r) = 0;
	printf("\n");
	return r0;
}
#endif

FILE *nd_write,*nd_read;

void nd_send_int(int a) {
	write_int(nd_write,&a);
}

void nd_send_intarray(int *p,int len) {
	write_intarray(nd_write,p,len);
}

int nd_recv_int() {
	int a;

	read_int(nd_read,&a);
	return a;
}

void nd_recv_intarray(int *p,int len) {
	read_intarray(nd_read,p,len);
}

void nd_send_ndv(NDV p) {
	int len,i;
	NMV m;

	if ( !p ) nd_send_int(0);
	else {
		len = LEN(p);
		nd_send_int(len);
		m = BDY(p);
		for ( i = 0; i < len; i++, NMV_ADV(m) ) {
			nd_send_int(CM(m));	
			nd_send_intarray(DL(m),nd_wpd);
		}
	}
}

void nd_send_nd(ND p) {
	int len,i;
	NM m;

	if ( !p ) nd_send_int(0);
	else {
		len = LEN(p);
		nd_send_int(len);
		m = BDY(p);
		for ( i = 0; i < len; i++, m = NEXT(m) ) {
			nd_send_int(CM(m));	
			nd_send_intarray(DL(m),nd_wpd);
		}
	}
}

NDV nd_recv_ndv()
{
	int len,i;
	NMV m,m0;
	NDV r;

	len = nd_recv_int();
	if ( !len ) return 0;
	else {
		m0 = m = (NMV)GC_malloc_atomic_ignore_off_page(nmv_adv*len);
#if 0
		ndv_alloc += len*nmv_adv;
#endif
		for ( i = 0; i < len; i++, NMV_ADV(m) ) {
			CM(m) = nd_recv_int();	
			nd_recv_intarray(DL(m),nd_wpd);
		}
		MKNDV(nd_nvar,m0,len,r);
		return r;	
	}
}

int ox_exec_f4_red(Q proc)
{
	Obj obj;
	STRING fname;
	NODE arg;
	int s;
	extern int ox_need_conv,ox_file_io;

	MKSTR(fname,"nd_exec_f4_red");
	arg = mknode(2,proc,fname);
	Pox_cmo_rpc(arg,&obj);
	s = get_ox_server_id(QTOS(proc));
	nd_write = iofp[s].out;
	nd_read = iofp[s].in;
	ox_need_conv = ox_file_io = 0;
	return s;
}

NODE nd_f4_red_dist(int m,ND_pairs sp0,UINT *s0vect,int col,NODE rp0,ND_pairs *nz)
{
	int nsp,nred;
	int i,rank,s;
	NODE rp,r0,r;
	ND_pairs sp;
	NM_ind_pair pair;
	NMV nmv;
	NM nm;
	NDV nf;
	Obj proc,dmy;

	ox_launch_main(0,0,&proc);
	s = ox_exec_f4_red((Q)proc);

	nd_send_int(m);
	nd_send_int(nd_nvar);
	nd_send_int(nd_bpe);
	nd_send_int(nd_wpd);
	nd_send_int(nmv_adv);

	saveobj(nd_write,dp_current_spec->obj); fflush(nd_write);

	nd_send_int(nd_psn);
	for ( i = 0; i < nd_psn; i++ ) nd_send_ndv(nd_ps[i]);

	for ( sp = sp0, nsp = 0; sp; sp = NEXT(sp), nsp++ );
	nd_send_int(nsp);
	for ( i = 0, sp = sp0; i < nsp; i++, sp = NEXT(sp) ) {
		nd_send_int(sp->i1); nd_send_int(sp->i2);
	}

	nd_send_int(col); nd_send_intarray(s0vect,col*nd_wpd);

	nred = length(rp0); nd_send_int(nred);
	for ( i = 0, rp = rp0; i < nred; i++, rp = NEXT(rp) ) {
		pair = (NM_ind_pair)BDY(rp);
		nd_send_int(pair->index);
		nd_send_intarray(pair->mul->dl,nd_wpd);
	}
	fflush(nd_write);
	rank = nd_recv_int();
	fprintf(asir_out,"rank=%d\n",rank);
	r0 = 0;
	for ( i = 0; i < rank; i++ ) {
		nf = nd_recv_ndv();
		NEXTNODE(r0,r); BDY(r) = (pointer)nf;
	}
	Pox_shutdown(mknode(1,proc),&dmy);
	return r0;
}

/* server side */

void nd_exec_f4_red_dist()
{
	int m,i,nsp,col,s0size,nred,spcol,j,k;
	NM_ind_pair *rp0;
	NDV nf;
	UINT *s0vect;
	IndArray *imat;
	int *rhead;
	int **spmat;
	UINT *svect,*v;
	ND_pairs *sp0;
	int *colstat;
	int a,sprow,rank;
	struct order_spec *ord;
	Obj ordspec;
	ND spol;
	int maxrs;
	int *spsugar;

	nd_read = iofp[0].in;
	nd_write = iofp[0].out;
	m = nd_recv_int();
	nd_nvar = nd_recv_int();
	nd_bpe = nd_recv_int();
	nd_wpd = nd_recv_int();
	nmv_adv = nd_recv_int();

	loadobj(nd_read,&ordspec);
	create_order_spec(0,ordspec,&ord);
	nd_init_ord(ord);
	nd_setup_parameters(nd_nvar,0);

	nd_psn = nd_recv_int();
	nd_ps = (NDV *)MALLOC(nd_psn*sizeof(NDV));
	nd_bound = (UINT **)MALLOC(nd_psn*sizeof(UINT *));
	for ( i = 0; i < nd_psn; i++ ) {
		nd_ps[i] = nd_recv_ndv();
		nd_bound[i] = ndv_compute_bound(nd_ps[i]);
	}

	nsp = nd_recv_int();
	sp0 = (ND_pairs *)MALLOC(nsp*sizeof(ND_pairs));
	for ( i = 0; i < nsp; i++ ) {
		NEWND_pairs(sp0[i]);
		sp0[i]->i1 = nd_recv_int(); sp0[i]->i2 = nd_recv_int();
		ndl_lcm(HDL(nd_ps[sp0[i]->i1]),HDL(nd_ps[sp0[i]->i2]),LCM(sp0[i]));
	}

	col = nd_recv_int();
	s0size = col*nd_wpd;
	s0vect = (UINT *)MALLOC(s0size*sizeof(UINT));
	nd_recv_intarray(s0vect,s0size);

	nred = nd_recv_int();
	rp0 = (NM_ind_pair *)MALLOC(nred*sizeof(NM_ind_pair));
	for ( i = 0; i < nred; i++ ) {
		rp0[i] = (NM_ind_pair)MALLOC(sizeof(struct oNM_ind_pair));
		rp0[i]->index = nd_recv_int();
		rp0[i]->mul = (NM)MALLOC(sizeof(struct oNM)+(nd_wpd-1)*sizeof(UINT));
		nd_recv_intarray(rp0[i]->mul->dl,nd_wpd);
	}

	spcol = col-nred;
	imat = (IndArray *)MALLOC(nred*sizeof(IndArray));
	rhead = (int *)MALLOC(col*sizeof(int));
	for ( i = 0; i < col; i++ ) rhead[i] = 0;

	/* construction of index arrays */
	for ( i = 0; i < nred; i++ ) {
		imat[i] = nm_ind_pair_to_vect_compress(m,s0vect,col,rp0[i]);
		rhead[imat[i]->head] = 1;
	}

	/* elimination (1st step) */
	spmat = (int **)MALLOC(nsp*sizeof(UINT *));
	svect = (UINT *)MALLOC(col*sizeof(UINT));
	spsugar = (int *)ALLOCA(nsp*sizeof(UINT));
	for ( a = sprow = 0; a < nsp; a++ ) {
		nd_sp(m,0,sp0[a],&spol);
		if ( !spol ) continue;
		nd_to_vect(m,s0vect,col,spol,svect);
		if ( m == -1 ) 
			maxrs = ndv_reduce_vect_sf(m,svect,col,imat,rp0,nred);
		else 
			maxrs = ndv_reduce_vect(m,svect,col,imat,rp0,nred);
		for ( i = 0; i < col; i++ ) if ( svect[i] ) break;
		if ( i < col ) {
			spmat[sprow] = v = (UINT *)MALLOC(spcol*sizeof(UINT));
			for ( j = k = 0; j < col; j++ )
				if ( !rhead[j] ) v[k++] = svect[j];
			spsugar[sprow] = MAX(maxrs,SG(spol));
			sprow++;
		}
		nd_free(spol);
	}
	/* elimination (2nd step) */
	colstat = (int *)ALLOCA(spcol*sizeof(int));
	if ( m == -1 )
		rank = nd_gauss_elim_sf(spmat,spsugar,sprow,spcol,m,colstat);
	else
		rank = nd_gauss_elim_mod(spmat,spsugar,0,sprow,spcol,m,colstat);
	nd_send_int(rank);
	for ( i = 0; i < rank; i++ ) {
		nf = vect_to_ndv(spmat[i],spcol,col,rhead,s0vect);
		nd_send_ndv(nf);
	}
	fflush(nd_write);
}

int nd_gauss_elim_q(Q **mat0,int *sugar,int row,int col,int *colstat)
{
	int mod,i,j,t,c,rank,rank0,inv;
	int *ci,*ri;
	Q dn;
	MAT m,nm;
	int **wmat;

	/* XXX */
	mod = 99999989;
	wmat = (int **)ALLOCA(row*sizeof(int *));
	for ( i = 0; i < row; i++ ) {
		wmat[i] = (int *)ALLOCA(col*sizeof(int));
		for ( j = 0; j < col; j++ ) {
			if ( mat0[i][j] ) {
				t = rem(NM(mat0[i][j]),mod);
				if ( SGN(mat0[i][j]) < 0 ) t = mod-t;
				wmat[i][j] = t;
			} else
				wmat[i][j] = 0;
		}
	}
	rank0 = nd_gauss_elim_mod(wmat,sugar,0,row,col,mod,colstat);
	NEWMAT(m); m->row = row; m->col = col; m->body = (pointer **)mat0;
	rank = generic_gauss_elim(m,&nm,&dn,&ri,&ci);
	if ( rank != rank0 )
		error("afo");
	for ( i = 0; i < row; i++ )
		for ( j = 0; j < col; j++ )
			mat0[i][j] = 0;
	c = col-rank;
	for ( i = 0; i < rank; i++ ) {
		mat0[i][ri[i]] = dn;	
		for ( j = 0; j < c; j++ )
			mat0[i][ci[j]] = (Q)BDY(nm)[i][j];
	}
	inv = invm(rem(NM(dn),mod),mod);
	if ( SGN(dn) < 0 ) inv = mod-inv;
	for ( i = 0; i < row; i++ )
		for ( j = 0; j < col; j++ ) {
			if ( mat0[i][j] ) {
				t = rem(NM(mat0[i][j]),mod);
				if ( SGN(mat0[i][j]) < 0 ) t = mod-t;
			} else
				t = 0;
			c = dmar(t,inv,0,mod);
			if ( wmat[i][j] != c )
				error("afo");
		}
	return rank;
}

int nd_gauss_elim_mod(int **mat0,int *sugar,ND_pairs *spactive,int row,int col,int md,int *colstat)
{
	int i,j,k,l,inv,a,rank,s;
	unsigned int *t,*pivot,*pk;
	unsigned int **mat;
	ND_pairs pair;

	mat = (unsigned int **)mat0;
	for ( rank = 0, j = 0; j < col; j++ ) {
		for ( i = rank; i < row; i++ )
			mat[i][j] %= md;
		for ( i = rank; i < row; i++ )
			if ( mat[i][j] )
				break;
		if ( i == row ) {
			colstat[j] = 0;
			continue;
		} else
			colstat[j] = 1;
		if ( i != rank ) {
			t = mat[i]; mat[i] = mat[rank]; mat[rank] = t;
			s = sugar[i]; sugar[i] = sugar[rank]; sugar[rank] = s;
			if ( spactive ) {
				pair = spactive[i]; spactive[i] = spactive[rank]; 
				spactive[rank] = pair;
			}
		}
		pivot = mat[rank];
		s = sugar[rank];
		inv = invm(pivot[j],md);
		for ( k = j, pk = pivot+k; k < col; k++, pk++ )
			if ( *pk ) {
				if ( *pk >= (unsigned int)md )
					*pk %= md;
				DMAR(*pk,inv,0,md,*pk)
			}
		for ( i = rank+1; i < row; i++ ) {
			t = mat[i];
			if ( a = t[j] ) {
				sugar[i] = MAX(sugar[i],s);
				red_by_vect(md,t+j,pivot+j,md-a,col-j);
			}
		}
		rank++;
	}
	for ( j = col-1, l = rank-1; j >= 0; j-- )
		if ( colstat[j] ) {
			pivot = mat[l];
			s = sugar[l];
			for ( i = 0; i < l; i++ ) {
				t = mat[i];
				t[j] %= md;
				if ( a = t[j] ) {
					sugar[i] = MAX(sugar[i],s);
					red_by_vect(md,t+j,pivot+j,md-a,col-j);
				}
			}
			l--;
		}
	for ( j = 0, l = 0; l < rank; j++ )
		if ( colstat[j] ) {
			t = mat[l];
			for ( k = j; k < col; k++ )
				if ( t[k] >= (unsigned int)md )
					t[k] %= md;
			l++;
		}
	return rank;
}

int nd_gauss_elim_sf(int **mat0,int *sugar,int row,int col,int md,int *colstat)
{
	int i,j,k,l,inv,a,rank,s;
	unsigned int *t,*pivot,*pk;
	unsigned int **mat;

	mat = (unsigned int **)mat0;
	for ( rank = 0, j = 0; j < col; j++ ) {
		for ( i = rank; i < row; i++ )
			if ( mat[i][j] )
				break;
		if ( i == row ) {
			colstat[j] = 0;
			continue;
		} else
			colstat[j] = 1;
		if ( i != rank ) {
			t = mat[i]; mat[i] = mat[rank]; mat[rank] = t;
			s = sugar[i]; sugar[i] = sugar[rank]; sugar[rank] = s;
		}
		pivot = mat[rank];
		s = sugar[rank];
		inv = _invsf(pivot[j]);
		for ( k = j, pk = pivot+k; k < col; k++, pk++ )
			if ( *pk )
				*pk = _mulsf(*pk,inv);
		for ( i = rank+1; i < row; i++ ) {
			t = mat[i];
			if ( a = t[j] ) {
				sugar[i] = MAX(sugar[i],s);
				red_by_vect_sf(md,t+j,pivot+j,_chsgnsf(a),col-j);
			}
		}
		rank++;
	}
	for ( j = col-1, l = rank-1; j >= 0; j-- )
		if ( colstat[j] ) {
			pivot = mat[l];
			s = sugar[l];
			for ( i = 0; i < l; i++ ) {
				t = mat[i];
				if ( a = t[j] ) {
					sugar[i] = MAX(sugar[i],s);
					red_by_vect_sf(md,t+j,pivot+j,_chsgnsf(a),col-j);
				}
			}
			l--;
		}
	return rank;
}

int ndv_ishomo(NDV p)
{
	NMV m;
	int len,h;

	if ( !p ) return 1;
	len = LEN(p);
	m = BDY(p);
	h = TD(DL(m));
	NMV_ADV(m);
	for ( len--; len; len--, NMV_ADV(m) )
		if ( TD(DL(m)) != h ) return 0;
	return 1;
}

void ndv_save(NDV p,int index)
{
	FILE *s;
	char name[BUFSIZ];
	short id;
	int nv,sugar,len,n,i,td,e,j;
	NMV m;
	unsigned int *dl;

	sprintf(name,"%s/%d",Demand,index);
	s = fopen(name,"w");
	savevl(s,0);
	if ( !p ) { 
		saveobj(s,0);
		return;
	}
	id = O_DP;
	nv = NV(p);
	sugar = SG(p);
	len = LEN(p);
	write_short(s,&id); write_int(s,&nv); write_int(s,&sugar);
	write_int(s,&len);

	for ( m = BDY(p), i = 0; i < len; i++, NMV_ADV(m) ) {
		saveobj(s,(Obj)CQ(m));
		dl = DL(m);
		td = TD(dl);
		write_int(s,&td);
		for ( j = 0; j < nv; j++ ) {
			e = GET_EXP(dl,j);	
			write_int(s,&e);
		}
	}
	fclose(s);
}

NDV ndv_load(int index)
{
	FILE *s;
	char name[BUFSIZ];
	short id;
	int nv,sugar,len,n,i,td,e,j;
	NDV d;
	NMV m0,m;
	unsigned int *dl;
	Obj obj;

	sprintf(name,"%s/%d",Demand,index);
	s = fopen(name,"r");
	if ( !s ) return 0;

	skipvl(s);
	read_short(s,&id);
	if ( !id ) return 0;
	read_int(s,&nv);
	read_int(s,&sugar);
	read_int(s,&len);

	m0 = m = MALLOC(len*nmv_adv);
	for ( i = 0; i < len; i++, NMV_ADV(m) ) {
		loadobj(s,&obj); CQ(m) = (Q)obj;
		dl = DL(m);
		ndl_zero(dl);
		read_int(s,&td); TD(dl) = td;
		for ( j = 0; j < nv; j++ ) {
			read_int(s,&e);
			PUT_EXP(dl,j,e);	
		}
		if ( nd_blockmask ) ndl_weight_mask(dl);
	}
	fclose(s);
	MKNDV(nv,m0,len,d);
	SG(d) = sugar;
	return d;
}

void nd_det(int mod,MAT f,P *rp)
{
	VL fv,tv;
	int n,i,j,max,e,nvar,sgn,k0,l0,len0,len,k,l,a;
	pointer **m;
	Q mone;
	P **w;
	P mp,r;
	NDV **dm;
	NDV *t,*mi,*mj;
	NDV d,s,mij,mjj;
	ND u;
	NMV nmv;
	UINT *bound;
	PGeoBucket bucket;
	struct order_spec *ord;
	Q dq,dt,ds;
	N gn,qn,dn0,nm,dn;

	create_order_spec(0,0,&ord);
	nd_init_ord(ord);
	get_vars((Obj)f,&fv);
	if ( f->row != f->col )
		error("nd_det : non-square matrix");
	n = f->row;
	m = f->body;
	for ( nvar = 0, tv = fv; tv; tv = NEXT(tv), nvar++ );

	if ( !nvar ) {
		if ( !mod )
			detp(CO,(P **)m,n,rp);
		else {
			w = (P **)almat_pointer(n,n);
			for ( i = 0; i < n; i++ )
				for ( j = 0; j < n; j++ )
					ptomp(mod,(P)m[i][j],&w[i][j]);
			detmp(CO,mod,w,n,&mp);
			mptop(mp,rp);
		}
		return;
	}
	
	if ( !mod ) {
		w = (P **)almat_pointer(n,n);
		dq = ONE;
		for ( i = 0; i < n; i++ ) {
			dn0 = ONEN;
			for ( j = 0; j < n; j++ ) {
				if ( !m[i][j] ) continue;
				lgp(m[i][j],&nm,&dn);
				gcdn(dn0,dn,&gn); divsn(dn0,gn,&qn); muln(qn,dn,&dn0);
			}
			if ( !UNIN(dn0) ) {
				NTOQ(dn0,1,ds);
				for ( j = 0; j < n; j++ )
					mulp(CO,(P)m[i][j],(P)ds,&w[i][j]);
				mulq(dq,ds,&dt); dq = dt;
			} else
				for ( j = 0; j < n; j++ )
					w[i][j] = (P)m[i][j];
		}
		m = (pointer **)w;
	}

	for ( i = 0, max = 0; i < n; i++ )
		for ( j = 0; j < n; j++ )
			for ( tv = fv; tv; tv = NEXT(tv) ) {
				e = getdeg(tv->v,(P)m[i][j]);
				max = MAX(e,max);
			}
	nd_setup_parameters(nvar,max);
	dm = (NDV **)almat_pointer(n,n);
	for ( i = 0, max = 0; i < n; i++ )
		for ( j = 0; j < n; j++ ) {
			dm[i][j] = ptondv(CO,fv,m[i][j]);
			if ( mod ) ndv_mod(mod,dm[i][j]);
			if ( dm[i][j] && !LEN(dm[i][j]) ) dm[i][j] = 0;
		}
	d = ptondv(CO,fv,(P)ONE);
	if ( mod ) ndv_mod(mod,d);
	chsgnq(ONE,&mone);
	for ( j = 0, sgn = 1; j < n; j++ ) {
		if ( DP_Print ) fprintf(stderr,".",j);
		for ( i = j; i < n && !dm[i][j]; i++ );
		if ( i == n ) {
			*rp = 0;
			return;
		}
		k0 = i; l0 = j; len0 = LEN(dm[k0][l0]);
		for ( k = j; k < n; k++ )
			for ( l = j; l < n; l++ )
				if ( dm[k][l] && LEN(dm[k][l]) < len0 ) {
					k0 = k; l0 = l; len0 = LEN(dm[k][l]);
				}
		if ( k0 != j ) {
			t = dm[j]; dm[j] = dm[k0]; dm[k0] = t;
			sgn = -sgn;
		}
		if ( l0 != j ) {
			for ( k = j; k < n; k++ ) {
				s = dm[k][j]; dm[k][j] = dm[k][l0]; dm[k][l0] = s;
			}
			sgn = -sgn;
		}
		bound = nd_det_compute_bound(dm,n,j);
		for ( k = 0; k < nd_nvar; k++ )
			if ( bound[k]*2 > nd_mask0 ) break;
		if ( k < nd_nvar )
			nd_det_reconstruct(dm,n,j,d);

		for ( i = j+1, mj = dm[j], mjj = mj[j]; i < n; i++ ) {
/*			if ( DP_Print ) fprintf(stderr,"	i=%d\n		",i); */
			mi = dm[i]; mij = mi[j];
			if ( mod )
				ndv_mul_c(mod,mij,mod-1);
			else
				ndv_mul_c_q(mij,mone);
			for ( k = j+1; k < n; k++ ) {
/*				if ( DP_Print ) fprintf(stderr,"k=%d ",k); */
				bucket = create_pbucket();
				if ( mi[k] ) {
					nmv = BDY(mjj); len = LEN(mjj);
					for ( a = 0; a < len; a++, NMV_ADV(nmv) ) {
						u = ndv_mul_nmv_trunc(mod,nmv,mi[k],DL(BDY(d)));
						add_pbucket(mod,bucket,u);
					}
				}
				if ( mj[k] && mij ) {
					nmv = BDY(mij); len = LEN(mij);
					for ( a = 0; a < len; a++, NMV_ADV(nmv) ) {
						u = ndv_mul_nmv_trunc(mod,nmv,mj[k],DL(BDY(d)));
						add_pbucket(mod,bucket,u);
					}
				}
				u = nd_quo(mod,bucket,d);
				mi[k] = ndtondv(mod,u);
			}
/*			if ( DP_Print ) fprintf(stderr,"\n",k); */
		}
		d = mjj;
	}
	if ( DP_Print ) fprintf(stderr,"\n",k);
	if ( sgn < 0 )
		if ( mod )
			ndv_mul_c(mod,d,mod-1);
		else
			ndv_mul_c_q(d,mone);
	r = ndvtop(mod,CO,fv,d);
	if ( !mod && !UNIQ(dq) )
		divsp(CO,r,(P)dq,rp);
	else
		*rp = r;
}

ND ndv_mul_nmv_trunc(int mod,NMV m0,NDV p,UINT *d)
{
	NM mr,mr0;
	NM tnm;
	NMV m;
	UINT *d0,*dt,*dm;
	int c,n,td,i,c1,c2,len;
	Q q;
	ND r;

	if ( !p ) return 0;
	else {
		n = NV(p); m = BDY(p); len = LEN(p);
		d0 = DL(m0);
		td = TD(d);
		mr0 = 0;
		NEWNM(tnm);
		if ( mod ) {
			c = CM(m0);
			for ( i = 0; i < len; i++, NMV_ADV(m) ) {
				ndl_add(DL(m),d0,DL(tnm));
				if ( ndl_reducible(DL(tnm),d) ) {
					NEXTNM(mr0,mr);
					c1 = CM(m); DMAR(c1,c,0,mod,c2); CM(mr) = c2;
					ndl_copy(DL(tnm),DL(mr));
				}
			}
		} else {
			q = CQ(m0);
			for ( i = 0; i < len; i++, NMV_ADV(m) ) {
				ndl_add(DL(m),d0,DL(tnm));
				if ( ndl_reducible(DL(tnm),d) ) {
					NEXTNM(mr0,mr);
					mulq(CQ(m),q,&CQ(mr));
					ndl_copy(DL(tnm),DL(mr));
				}
			}
		}
		if ( !mr0 )
			return 0;
		else {
			NEXT(mr) = 0; 
			for ( len = 0, mr = mr0; mr; mr = NEXT(mr), len++ );
			MKND(NV(p),mr0,len,r);
			SG(r) = SG(p) + TD(d0);
			return r;
		}
	}
}

void nd_det_reconstruct(NDV **dm,int n,int j,NDV d)
{
	int i,obpe,oadv,h,k,l;
	static NM prev_nm_free_list;
	EPOS oepos;

	obpe = nd_bpe;
	oadv = nmv_adv;
	oepos = nd_epos;
	if ( obpe < 2 ) nd_bpe = 2;
	else if ( obpe < 3 ) nd_bpe = 3;
	else if ( obpe < 4 ) nd_bpe = 4;
	else if ( obpe < 5 ) nd_bpe = 5;
	else if ( obpe < 6 ) nd_bpe = 6;
	else if ( obpe < 8 ) nd_bpe = 8;
	else if ( obpe < 10 ) nd_bpe = 10;
	else if ( obpe < 16 ) nd_bpe = 16;
	else if ( obpe < 32 ) nd_bpe = 32;
	else error("nd_det_reconstruct : exponent too large");

	nd_setup_parameters(nd_nvar,0);
	prev_nm_free_list = _nm_free_list;
	_nm_free_list = 0;
	for ( k = j; k < n; k++ )
		for (l = j; l < n; l++ )
			ndv_realloc(dm[k][l],obpe,oadv,oepos);
	ndv_realloc(d,obpe,oadv,oepos);
	prev_nm_free_list = 0;
#if 0
	GC_gcollect();
#endif
}

/* returns a UINT array containing degree bounds */

UINT *nd_det_compute_bound(NDV **dm,int n,int j)
{
	UINT *d0,*d1,*d,*t,*r;
	int k,l,i;

	d0 = (UINT *)MALLOC(nd_nvar*sizeof(UINT));
	for ( k = 0; k < nd_nvar; k++ ) d0[k] = 0;
	for ( k = j; k < n; k++ )
		for ( l = j; l < n; l++ )
			if ( dm[k][l] ) {
				d = ndv_compute_bound(dm[k][l]);
				for ( i = 0; i < nd_nvar; i++ )
					d0[i] = MAX(d0[i],d[i]);
			}
	return d0;
}

DL nd_separate_d(UINT *d,UINT *trans)
{
	int n,td,i,e,j;
	DL a;

	ndl_zero(trans);
	td = 0;
	for ( i = 0; i < nd_ntrans; i++ ) {
		e = GET_EXP(d,i);
		PUT_EXP(trans,i,e);
		td += MUL_WEIGHT(e,i);
	}
	if ( nd_ntrans+nd_nalg < nd_nvar ) {
		/* homogenized */
		i = nd_nvar-1;
		e = GET_EXP(d,i);
		PUT_EXP(trans,i,e);
		td += MUL_WEIGHT(e,i);
	}
	TD(trans) = td;
	if ( nd_blockmask) ndl_weight_mask(trans);
	NEWDL(a,nd_nalg);
	td = 0;
	for ( i = 0; i < nd_nalg; i++ ) {
		j = nd_ntrans+i;
		e = GET_EXP(d,j);
		a->d[i] = e;
		td += e;
	}
	a->td = td;
	return a;
}

int nd_monic(int mod,ND *p)
{
	UINT *trans,*t;
	DL alg;
	MP mp0,mp;
	NM m,m0,m1,ma0,ma,mb,mr0,mr;
	ND r;
	DL dl;
	DP nm;
	NDV ndv;
	DAlg inv,cd;
	ND s,c;
	Q l,mul;
	N ln;
	int n,ntrans,i,e,td,is_lc,len;
	NumberField nf;
	struct oEGT eg0,eg1;

	if ( !(nf = get_numberfield()) )
		error("nd_monic : current_numberfield is not set");

	/* Q coef -> DAlg coef */
	NEWNM(ma0); ma = ma0;
	m = BDY(*p); 
	is_lc = 1;
	while ( 1 ) {
		NEWMP(mp0); mp = mp0;
		mp->c = (P)CQ(m);
		mp->dl = nd_separate_d(DL(m),DL(ma));
		NEWNM(mb);
		for ( m = NEXT(m); m; m = NEXT(m) ) {
			alg = nd_separate_d(DL(m),DL(mb));
			if ( !ndl_equal(DL(ma),DL(mb)) )
				break;
			NEXTMP(mp0,mp); mp->c = (P)CQ(m); mp->dl = alg;
		}
		NEXT(mp) = 0;
		MKDP(nd_nalg,mp0,nm);
		MKDAlg(nm,ONE,cd);
		if ( is_lc == 1 ) {
			/* if the lc is a rational number, we have nothing to do */
			if ( !mp0->dl->td )
				return 1;

			get_eg(&eg0);
			invdalg(cd,&inv);
			get_eg(&eg1); add_eg(&eg_invdalg,&eg0,&eg1);
			/* check the validity of inv */
			if ( mod && !rem(NM(inv->dn),mod) )
				return 0;
			CA(ma) = nf->one;
			is_lc = 0;
			ln = ONEN;
		} else {
			muldalg(cd,inv,&CA(ma));	
			lcmn(ln,NM(CA(ma)->dn),&ln);
		}
		if ( m ) {
			NEXT(ma) = mb; ma = mb;
		} else {
			NEXT(ma) = 0;
			break;
		}
	}
	/* l = lcm(denoms) */
	NTOQ(ln,1,l);
	for ( mr0 = 0, m = ma0; m; m = NEXT(m) ) {
		divq(l,CA(m)->dn,&mul);
		for ( mp = BDY(CA(m)->nm); mp; mp = NEXT(mp) ) {
			NEXTNM(mr0,mr);
			mulq((Q)mp->c,mul,&CQ(mr));
			dl = mp->dl;
			td = TD(DL(m));
			ndl_copy(DL(m),DL(mr));
			for ( i = 0; i < nd_nalg; i++ ) {
				e = dl->d[i];
				PUT_EXP(DL(mr),i+nd_ntrans,e);
				td += MUL_WEIGHT(e,i+nd_ntrans);
			}
			TD(DL(mr)) = td;
			if ( nd_blockmask) ndl_weight_mask(DL(mr));
		}
	}
	NEXT(mr) = 0;
	for ( len = 0, mr = mr0; mr; mr = NEXT(mr), len++ );
	MKND(NV(*p),mr0,len,r);
	/* XXX */
	SG(r) = SG(*p);
	nd_free(*p);
	*p = r;
	return 1;
}
