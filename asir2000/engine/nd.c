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

typedef struct oNM {
	struct oNM *next;
	int td;
	int c;
	unsigned int dl[1];
} *NM;

typedef struct oND_pairs {
	struct oND_pairs *next;
	int i1,i2;
	int td,sugar;
	unsigned int lcm[1];
} *ND_pairs;

static ND *nd_ps;
static unsigned int **nd_bound;
int nd_mod,nd_nvar;
int is_rlex;
int nd_epw,nd_bpe,nd_wpd;
unsigned int nd_mask[32];
unsigned int nd_mask0,nd_mask1;
NM _nm_free_list;
ND _nd_free_list;
ND_pairs _ndp_free_list;
NM *nd_red;
int nd_red_len;

extern int Top,Reverse;
int nd_psn,nd_pslen;
int nd_found,nd_create,nd_notfirst;

void GC_gcollect();
NODE append_one(NODE,int);

#define HTD(d) ((d)->body->td)
#define HDL(d) ((d)->body->dl)
#define HC(d) ((d)->body->c)

#define NEWND_pairs(m) if(!_ndp_free_list)_NDP_alloc(); (m)=_ndp_free_list; _ndp_free_list = NEXT(_ndp_free_list)   
#define NEWNM(m) if(!_nm_free_list)_NM_alloc(); (m)=_nm_free_list; _nm_free_list = NEXT(_nm_free_list)   
#define MKND(n,m,d) if(!_nd_free_list)_ND_alloc(); (d)=_nd_free_list; _nd_free_list = (ND)BDY(_nd_free_list); (d)->nv=(n); BDY(d)=(m)

#define NEXTNM(r,c) \
if(!(r)){NEWNM(r);(c)=(r);}else{NEWNM(NEXT(c));(c)=NEXT(c);}
#define NEXTNM2(r,c,s) \
if(!(r)){(c)=(r)=(s);}else{NEXT(c)=(s);(c)=(s);}
#define FREENM(m) NEXT(m)=_nm_free_list; _nm_free_list=(m)
#define FREENDP(m) NEXT(m)=_ndp_free_list; _ndp_free_list=(m)
#define FREEND(m) BDY(m)=(NM)_nd_free_list; _nd_free_list=(m)

#define NEXTND_pairs(r,c) \
if(!(r)){NEWND_pairs(r);(c)=(r);}else{NEWND_pairs(NEXT(c));(c)=NEXT(c);}

ND_pairs crit_B( ND_pairs d, int s );
void nd_gr(LIST f,LIST v,int m,struct order_spec *ord,LIST *rp);
NODE nd_setup(NODE f);
int nd_newps(ND a);
ND_pairs nd_minp( ND_pairs d, ND_pairs *prest );
NODE update_base(NODE nd,int ndp);
static ND_pairs equivalent_pairs( ND_pairs d1, ND_pairs *prest );
int crit_2( int dp1, int dp2 );
ND_pairs crit_F( ND_pairs d1 );
ND_pairs crit_M( ND_pairs d1 );
ND_pairs nd_newpairs( NODE g, int t );
ND_pairs update_pairs( ND_pairs d, NODE /* of index */ g, int t);
NODE nd_gb(NODE f);
void nd_free_private_storage();
void _NM_alloc();
void _ND_alloc();
int ndl_td(unsigned int *d);
ND nd_add(ND p1,ND p2);
ND nd_mul_nm(ND p,NM m0);
ND nd_mul_term(ND p,int td,unsigned int *d);
int nd_sp(ND_pairs p,ND *nf);
int nd_find_reducer(ND g,ND *red);
int nd_nf(ND g,int full,ND *nf);
ND nd_reduce(ND p1,ND p2);
ND nd_reduce_special(ND p1,ND p2);
void nd_free(ND p);
void ndl_print(unsigned int *dl);
void nd_print(ND p);
void ndp_print(ND_pairs d);
int nd_length(ND p);
void nd_monic(ND p);
void nd_mul_c(ND p,int mul);
void nd_free_redlist();
void nd_append_red(unsigned int *d,int td,int i);
unsigned int *nd_compute_bound(ND p);
ND_pairs nd_reconstruct(ND_pairs);
void nd_setup_parameters();
ND nd_dup(ND p,int obpe);
void ndl_dup(int obpe,unsigned int *d,unsigned int *r);

void nd_free_private_storage()
{
	_nd_free_list = 0;
	_nm_free_list = 0;
	nd_red = 0;
	GC_gcollect();
}

void _NM_alloc()
{
	NM p;
	int i;

	for ( i = 0; i < 16; i++ ) {
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

	for ( i = 0; i < 10240; i++ ) {
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

INLINE void ndl_add(unsigned int *d1,unsigned int *d2,unsigned int *d)
{
	int i;

	for ( i = 0; i < nd_wpd; i++ ) {
		d[i] = d1[i]+d2[i];
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

ND nd_reduce(ND p1,ND p2)
{
	int c,c1,c2,t,td,td2,mul;
	NM m2,prev,head,cur,new;
	unsigned int *d;

	if ( !p1 )
		return 0;
	else {
		c2 = invm(HC(p2),nd_mod);
		c1 = nd_mod-HC(p1);
		DMAR(c1,c2,0,nd_mod,mul);
		td = HTD(p1)-HTD(p2);
		d = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
		ndl_sub(HDL(p1),HDL(p2),d);
		prev = 0; head = cur = BDY(p1);
		NEWNM(new);
		for ( m2 = BDY(p2); m2; ) {
			td2 = new->td = m2->td+td;
			ndl_add(m2->dl,d,new->dl);
			if ( !cur ) {
				c1 = C(m2);
				DMAR(c1,mul,0,nd_mod,c2);
				C(new) = c2;
				if ( !prev ) {
					prev = new;
					NEXT(prev) = 0;
					head = prev;
				} else {
					NEXT(prev) = new;
					NEXT(new) = 0;
					prev = new;
				}
				m2 = NEXT(m2);
				NEWNM(new);
				continue;
			}
			if ( cur->td > td2 )
				c = 1;
			else if ( cur->td < td2 )
				c = -1;
			else
				c = ndl_compare(cur->dl,new->dl);
			switch ( c ) {
				case 0:
					c2 = C(m2);
					c1 = C(cur);
					DMAR(c2,mul,c1,nd_mod,t);
					if ( t )
						C(cur) = t;
					else if ( !prev ) {
						head = NEXT(cur);
						FREENM(cur);
						cur = head;
					} else {
						NEXT(prev) = NEXT(cur);
						FREENM(cur);
						cur = NEXT(prev);
					}
					m2 = NEXT(m2);
					break;
				case 1:
					prev = cur;
					cur = NEXT(cur);
					break;
				case -1:
					if ( !prev ) {
						/* cur = head */
						prev = new;
						c2 = C(m2);
						DMAR(c2,mul,0,nd_mod,c1);
						C(prev) = c1;
						NEXT(prev) = head;
						head = prev;
					} else {
						c2 = C(m2);
						DMAR(c2,mul,0,nd_mod,c1);
						C(new) = c1;
						NEXT(prev) = new;
						NEXT(new) = cur;
						prev = new;
					}
					NEWNM(new);
					m2 = NEXT(m2);
					break;
			}
		}
		FREENM(new);
		if ( head ) {
			BDY(p1) = head;
			p1->sugar = MAX(p1->sugar,p2->sugar+td);
			return p1;
		} else {
			FREEND(p1);
			return 0;
		}
			
	}
}

/* HDL(p1) = HDL(p2) */

ND nd_reduce_special(ND p1,ND p2)
{
	int c,c1,c2,t,td,td2,mul;
	NM m2,prev,head,cur,new;

	if ( !p1 )
		return 0;
	else {
		c2 = invm(HC(p2),nd_mod);
		c1 = nd_mod-HC(p1);
		DMAR(c1,c2,0,nd_mod,mul);
		prev = 0; head = cur = BDY(p1);
		NEWNM(new);
		for ( m2 = BDY(p2); m2; ) {
			td2 = new->td = m2->td;
			if ( !cur ) {
				c1 = C(m2);
				DMAR(c1,mul,0,nd_mod,c2);
				C(new) = c2;
				bcopy(m2->dl,new->dl,nd_wpd*sizeof(unsigned int));
				if ( !prev ) {
					prev = new;
					NEXT(prev) = 0;
					head = prev;
				} else {
					NEXT(prev) = new;
					NEXT(new) = 0;
					prev = new;
				}
				m2 = NEXT(m2);
				NEWNM(new);
				continue;
			}
			if ( cur->td > td2 )
				c = 1;
			else if ( cur->td < td2 )
				c = -1;
			else
				c = ndl_compare(cur->dl,m2->dl);
			switch ( c ) {
				case 0:
					c2 = C(m2);
					c1 = C(cur);
					DMAR(c2,mul,c1,nd_mod,t);
					if ( t )
						C(cur) = t;
					else if ( !prev ) {
						head = NEXT(cur);
						FREENM(cur);
						cur = head;
					} else {
						NEXT(prev) = NEXT(cur);
						FREENM(cur);
						cur = NEXT(prev);
					}
					m2 = NEXT(m2);
					break;
				case 1:
					prev = cur;
					cur = NEXT(cur);
					break;
				case -1:
					bcopy(m2->dl,new->dl,nd_wpd*sizeof(unsigned int));
					if ( !prev ) {
						/* cur = head */
						prev = new;
						c2 = C(m2);
						DMAR(c2,mul,0,nd_mod,c1);
						C(prev) = c1;
						NEXT(prev) = head;
						head = prev;
					} else {
						c2 = C(m2);
						DMAR(c2,mul,0,nd_mod,c1);
						C(new) = c1;
						NEXT(prev) = new;
						NEXT(new) = cur;
						prev = new;
					}
					NEWNM(new);
					m2 = NEXT(m2);
					break;
			}
		}
		FREENM(new);
		if ( head ) {
			BDY(p1) = head;
			p1->sugar = MAX(p1->sugar,p2->sugar+td);
			return p1;
		} else {
			FREEND(p1);
			return 0;
		}
			
	}
}

INLINE int ndl_check_bound(unsigned int *d)
{
	int i;

	for ( i = 0; i < nd_wpd; i++ )
		if ( d[i] & nd_mask1 )
			return 1;
	return 0;
}

int nd_sp(ND_pairs p,ND *rp)
{
	NM m;
	ND p1,p2,t1,t2;
	unsigned int *lcm,*check;
	int td;

	check = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
	p1 = nd_ps[p->i1];
	p2 = nd_ps[p->i2];
	lcm = p->lcm;
	td = p->td;
	NEWNM(m);
	C(m) = HC(p2); m->td = td-HTD(p1); ndl_sub(lcm,HDL(p1),m->dl); NEXT(m) = 0;
	ndl_add(nd_bound[p->i1],m->dl,check);
	if ( ndl_check_bound(check) )
		return 0;
	t1 = nd_mul_nm(p1,m);
	C(m) = nd_mod-HC(p1); m->td = td-HTD(p2); ndl_sub(lcm,HDL(p2),m->dl);
	ndl_add(nd_bound[p->i2],m->dl,check);
	if ( ndl_check_bound(check) ) {
		nd_free(t1);
		return 0;
	}
	t2 = nd_mul_nm(p2,m);
	FREENM(m);
	*rp = nd_add(t1,t2);
	return 1;
}

int ndl_hash_value(int td,unsigned int *d)
{
	int i;
	int r;

	r = td;
	for ( i = 0; i < nd_wpd; i++ )	
		r = ((r<<16)+d[i])%REDTAB_LEN;
	return r;
}

int nd_find_reducer(ND g, ND *rp)
{
	NM m;
	ND r,p;
	int i,c1,c2,c;
	int d,k,append,index;
	unsigned int *check;
	NM t;

	d = ndl_hash_value(HTD(g),HDL(g));
	for ( m = nd_red[d], k = 0; m; m = NEXT(m), k++ ) {
		if ( HTD(g) == m->td && ndl_equal(HDL(g),m->dl) ) {
			if ( k > 0 ) nd_notfirst++;
			index = m->c;
			append = 0;
			nd_found++;
			goto found;
		}
	}

	for ( i = 0; i < nd_psn; i++ ) {
		p = nd_ps[i];
		if ( HTD(g) >= HTD(p) && ndl_reducible(HDL(g),HDL(p)) ) {
			index = i;
			append = 1;
			nd_create++;
			goto found;
		}
	}
	return 0;

found:
	NEWNM(m);
	p = nd_ps[index];
	ndl_sub(HDL(g),HDL(p),m->dl);

	check = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));	
	ndl_add(nd_bound[index],m->dl,check);
	if ( ndl_check_bound(check) ) {
		FREENM(m);
		return -1;
	}

	c1 = invm(HC(p),nd_mod);
	c2 = nd_mod-HC(g);
	DMAR(c1,c2,0,nd_mod,c);
	C(m) = c;
	m->td = HTD(g)-HTD(p);
	NEXT(m) = 0;
	*rp = r = nd_mul_nm(p,m);
	FREENM(m);

	if ( append ) nd_append_red(HDL(g),HTD(g),i);
	return 1;
}

ND nd_find_monic_reducer(ND g)
{
	int *d;
	ND p,r;
	int i;

	for ( i = 0; i < nd_psn; i++ ) {
		p = nd_ps[i];
		if ( HTD(g) >= HTD(p) && ndl_reducible(HDL(g),HDL(p)) ) {
			d = (int *)ALLOCA(nd_wpd*sizeof(int));
			ndl_sub(HDL(g),HDL(p),d);
			r = nd_mul_term(p,HTD(g)-HTD(p),d);
			return r;
		}
	}
	return 0;
}

ND nd_add(ND p1,ND p2)
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
			if ( m1->td > m2->td )
				c = 1;
			else if ( m1->td < m2->td )
				c = -1;
			else
				c = ndl_compare(m1->dl,m2->dl);
			switch ( c ) {
				case 0:
					t = ((C(m1))+(C(m2))) - nd_mod;
					if ( t < 0 )
						t += nd_mod;
					s = m1; m1 = NEXT(m1);
					if ( t ) {
						NEXTNM2(mr0,mr,s); C(mr) = (t);
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
		p1->sugar = MAX(p1->sugar,p2->sugar);
		FREEND(p2);
		return p1;
	}
}

ND nd_mul_nm(ND p,NM m0)
{
	NM m,mr,mr0;
	unsigned int *d,*dt,*dm;
	int c,n,td,i,c1,c2;
	int *pt,*p1,*p2;
	ND r;

	if ( !p )
		return 0;
	else {
		n = NV(p); m = BDY(p);
		d = m0->dl; td = m0->td; c = C(m0);
		mr0 = 0;
		for ( ; m; m = NEXT(m) ) {
			NEXTNM(mr0,mr);
			c1 = C(m);
			DMAR(c1,c,0,nd_mod,c2);
			C(mr) = c2;
			mr->td = m->td+td;
			ndl_add(m->dl,d,mr->dl);
		}
		NEXT(mr) = 0; 
		MKND(NV(p),mr0,r);
		r->sugar = p->sugar + td;
		return r;
	}
}

ND nd_mul_term(ND p,int td,unsigned int *d)
{
	NM m,mr,mr0;
	int c,n;
	ND r;

	if ( !p )
		return 0;
	else {
		n = NV(p); m = BDY(p);
		for ( mr0 = 0; m; m = NEXT(m) ) {
			NEXTNM(mr0,mr);
			C(mr) = C(m);
			mr->td = m->td+td;
			ndl_add(m->dl,d,mr->dl);
		}
		NEXT(mr) = 0; 
		MKND(NV(p),mr0,r);
		r->sugar = p->sugar + td;
		return r;
	}
}

#if 1
/* ret=1 : success, ret=0 : overflow */
int nd_nf(ND g,int full,ND *rp)
{
	ND p,d,red;
	NM m,mrd,tail;
	int n,sugar,psugar,stat;

	if ( !g ) {
		*rp = 0;
		return 1;
	}
	sugar = g->sugar;
	n = NV(g);
	for ( d = 0; g; ) {
		/* stat=1 : found, stat=0 : not found, stat=-1 : overflow */
		stat = nd_find_reducer(g,&red);
		if ( stat == 1 ) {
#if 1
			g = nd_add(g,red);
			sugar = MAX(sugar,red->sugar);
#else
			psugar = (HTD(g)-HTD(red))+red->sugar;
			g = nd_reduce(g,red);
			sugar = MAX(sugar,psugar);
#endif
		} else if ( stat == -1 ) {
			nd_free(g);
			nd_free(d);
			return 0;
		} else if ( !full ) {
			*rp = g;
			return 1;
		} else {
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
		d->sugar = sugar;
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

void add_pbucket(PGeoBucket g,ND d)
{
	int l,k,m;

	l = nd_length(d);
	for ( k = 0, m = 1; l > m; k++, m <<= 2 );
	/* 4^(k-1) < l <= 4^k */
	d = nd_add(g->body[k],d);
	for ( ; d && nd_length(d) > 1<<(2*k); k++ ) {
		g->body[k] = 0;
		d = nd_add(g->body[k+1],d);
	}
	g->body[k] = d;
	g->m = MAX(g->m,k);
}

int head_pbucket(PGeoBucket g)
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
				sum = HC(gj);
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
						HC(gj) = sum;
					else
						g->body[j] = nd_remove_head(gj);
					j = i;
					gj = g->body[j];
					dj = HDL(gj);
					sum = HC(gj);
				} else if ( c == 0 ) {
					sum = sum+HC(gi)-nd_mod;
					if ( sum < 0 )
						sum += nd_mod;
					g->body[i] = nd_remove_head(gi);
				}
			}
		}
		if ( j < 0 )
			return -1;
		else if ( sum ) {
			HC(gj) = sum;
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
		r = nd_add(r,g->body[i]);
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
	sugar = g->sugar;
	n = g->nv;
	bucket = create_pbucket();
	add_pbucket(bucket,g);
	d = 0;
	while ( 1 ) {
		h = head_pbucket(bucket);
		if ( h < 0 ) {
			if ( d )
				d->sugar = sugar;
			return d;
		}
		g = bucket->body[h];
		red = nd_find_reducer(g);
		if ( red ) {
			bucket->body[h] = nd_remove_head(g);
			red = nd_remove_head(red);
			add_pbucket(bucket,red);
			sugar = MAX(sugar,red->sugar);
		} else if ( !full ) {
			g = normalize_pbucket(bucket);
			if ( g )
				g->sugar = sugar;
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

NODE nd_gb(NODE f)
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
		if ( l->sugar != sugar ) {
			sugar = l->sugar;
			fprintf(asir_out,"%d",sugar);
		}
		stat = nd_sp(l,&h);
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(d);
			goto again;
		}
		stat = nd_nf(h,!Top,&nf);
		if ( !stat ) {
			NEXT(l) = d; d = l;
			d = nd_reconstruct(d);
			goto again;
		} else if ( nf ) {
			printf("+"); fflush(stdout);
			nh = nd_newps(nf);
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

	dl = HDL(nd_ps[t]);
	td = HTD(nd_ps[t]);
	ts = nd_ps[t]->sugar - td;
	for ( r0 = 0, h = g; h; h = NEXT(h) ) {
		NEXTND_pairs(r0,r);
		r->i1 = (int)BDY(h);
		r->i2 = t;
		ndl_lcm(HDL(nd_ps[r->i1]),dl,r->lcm);
		r->td = ndl_td(r->lcm);
		s = nd_ps[r->i1]->sugar-HTD(nd_ps[r->i1]);
		r->sugar = MAX(s,ts) + r->td;
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
	t = HDL(nd_ps[s]);
	prev = 0;
	head = cur = d;
	lcm = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
	while ( cur ) {
		tl = cur->lcm;
		if ( ndl_reducible(tl,t)
			&& (ndl_lcm(HDL(nd_ps[cur->i1]),t,lcm),!ndl_equal(lcm,tl))
			&& (ndl_lcm(HDL(nd_ps[cur->i2]),t,lcm),!ndl_equal(lcm,tl)) ) {
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
		itd = e->td;
		for ( d3 = 0; d2; d2 = p ) {
			p = NEXT(d2),
			jd = d2->lcm;
			jtd = d2->td;
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
		s = r->sugar;
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
			} else if ( w->sugar < s ) {
				FREENDP(r);
				r = w;
				s = r->sugar;
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
	return ndl_disjoint(HDL(nd_ps[dp1]),HDL(nd_ps[dp2]));
}

static ND_pairs equivalent_pairs( ND_pairs d1, ND_pairs *prest )
{
	ND_pairs w,p,r,s;
	unsigned int *d;
	int td;

	w = d1;
	d = w->lcm;
	td = w->td;
	s = NEXT(w);
	NEXT(w) = 0;
	for ( r = 0; s; s = p ) {
		p = NEXT(s);
		if ( td == s->td && ndl_equal(d,s->lcm) ) {
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

	dl = HDL(nd_ps[ndp]);
	td = HTD(nd_ps[ndp]);
	for ( head = last = 0, p = nd; p; ) {
		dln = HDL(nd_ps[(int)BDY(p)]);
		tdn = HTD(nd_ps[(int)BDY(p)]);
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
	s = m->sugar;
	td = m->td;
	len = nd_length(nd_ps[m->i1])+nd_length(nd_ps[m->i2]);
	for ( ml = 0, l = m; p; p = NEXT(l = p) ) {
		if (p->sugar < s)
			goto find;
		else if ( p->sugar == s ) {
			if ( p->td < td )
				goto find;
			else if ( p->td == td ) {
				c = ndl_compare(p->lcm,lcm);
				if ( c < 0 )
					goto find;
				else if ( c == 0 ) {
					tlen = nd_length(nd_ps[p->i1])+nd_length(nd_ps[p->i2]);
					if ( tlen < len )
						goto find;
				}
			}
		}
		continue;
find:
		ml = l;
		m = p;
		lcm = m->lcm;
		s = m->sugar;
		td = m->td;
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

int nd_newps(ND a)
{
	if ( nd_psn == nd_pslen ) {
		nd_pslen *= 2;
		nd_ps = (ND *)REALLOC((char *)nd_ps,nd_pslen*sizeof(ND));
		nd_bound = (unsigned int **)
			REALLOC((char *)nd_bound,nd_pslen*sizeof(unsigned int *));
	}
	nd_monic(a);
	nd_ps[nd_psn] = a;
	nd_bound[nd_psn] = nd_compute_bound(a);
	return nd_psn++;
}

NODE NODE_sortb(NODE f,int);
ND dptond(DP);
DP ndtodp(ND);

NODE nd_setup(NODE f)
{
	int i,td;
	NODE s,s0,f0;

	nd_found = 0;
	nd_notfirst = 0;
	nd_create = 0;
#if 0
	f0 = f = NODE_sortb(f,1);
#endif
	nd_psn = length(f); nd_pslen = 2*nd_psn;
	nd_ps = (ND *)MALLOC(nd_pslen*sizeof(ND));
	nd_bound = (unsigned int **)MALLOC(nd_pslen*sizeof(unsigned int *));
	nd_bpe = 4;
	nd_setup_parameters();
	nd_free_private_storage();
	for ( i = 0; i < nd_psn; i++, f = NEXT(f) ) {
		nd_ps[i] = dptond((DP)BDY(f));
		nd_monic(nd_ps[i]);
		nd_bound[i] = nd_compute_bound(nd_ps[i]);
	}
	nd_red = (NM *)MALLOC(REDTAB_LEN*sizeof(NM));
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
	nd_mod = m;
	for ( fd0 = 0, t = BDY(f); t; t = NEXT(t) ) {
		ptod(CO,vv,(P)BDY(t),&b);
		_dp_mod(b,m,0,&c);
		if ( c ) {
			NEXTNODE(fd0,fd); BDY(fd) = (pointer)c;
		}
	}
	if ( fd0 ) NEXT(fd) = 0;
	s = nd_setup(fd0);
	x = nd_gb(s);
#if 0
	x = nd_reduceall(x,m);
#endif
	for ( r0 = 0; x; x = NEXT(x) ) {
		NEXTNODE(r0,r); 
		a = ndtodp(nd_ps[(int)BDY(x)]);
		_dtop_mod(CO,vv,a,(P *)&BDY(r));
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
	dl->td = td;
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

ND dptond(DP p)
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
		m->c = ITOS(t->c);
		m->td = t->dl->td;
		dltondl(n,t->dl,m->dl);
	}
	NEXT(m) = 0;
	MKND(n,m0,d);
	d->nv = n;
	d->sugar = p->sugar;
	return d;
}

DP ndtodp(ND p)
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
		m->c = STOI(t->c);
		m->dl = ndltodl(n,t->td,t->dl);
	}
	NEXT(m) = 0;
	MKDP(n,m0,d);
	d->sugar = p->sugar;
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
			printf("+%d*",m->c);
			ndl_print(m->dl);
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

void nd_monic(ND p)
{
	if ( !p )
		return;
	else
		nd_mul_c(p,invm(HC(p),nd_mod));
}

void nd_mul_c(ND p,int mul)
{
	NM m;
	int c,c1;

	if ( !p )
		return;
	for ( m = BDY(p); m; m = NEXT(m) ) {
		c1 = C(m);
		DMAR(c1,mul,0,nd_mod,c);
		C(m) = c;
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
	NM m,m0;
	int h;

	NEWNM(m);
	h = ndl_hash_value(td,d);
	m->c = i;
	m->td = td;
	bcopy(d,m->dl,nd_wpd*sizeof(unsigned int));
	NEXT(m) = nd_red[h];
	nd_red[h] = m;
}

unsigned int *nd_compute_bound(ND p)
{
	unsigned int *d1,*d2,*t;
	NM m;

	if ( !p )
		return 0;
	d1 = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
	d2 = (unsigned int *)ALLOCA(nd_wpd*sizeof(unsigned int));
	bcopy(HDL(p),d1,nd_wpd*sizeof(unsigned int));
	for ( m = NEXT(BDY(p)); m; m = NEXT(m) ) {
		ndl_lcm(m->dl,d1,d2);
		t = d1; d1 = d2; d2 = t;
	}
	t = (unsigned int *)MALLOC_ATOMIC(nd_wpd*sizeof(unsigned int));
	bcopy(d1,t,nd_wpd*sizeof(unsigned int));
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
}

ND_pairs nd_reconstruct(ND_pairs d)
{
	int i,obpe;
	NM prev_nm_free_list;
	ND_pairs s0,s,t,prev_ndp_free_list;
	
	obpe = nd_bpe;
	switch ( nd_bpe ) {
		case 4: nd_bpe = 6; break;
		case 6: nd_bpe = 8; break;
		case 8: nd_bpe = 16; break;
		case 16: nd_bpe = 32; break;
	}
	nd_setup_parameters();
	prev_nm_free_list = _nm_free_list;
	prev_ndp_free_list = _ndp_free_list;
	_nm_free_list = 0;
	_ndp_free_list = 0;
	for ( i = 0; i < nd_psn; i++ ) {
		nd_ps[i] = nd_dup(nd_ps[i],obpe);
		nd_bound[i] = nd_compute_bound(nd_ps[i]);
	}
	s0 = 0;
	for ( t = d; t; t = NEXT(t) ) {
		NEXTND_pairs(s0,s);
		s->i1 = t->i1;
		s->i2 = t->i2;
		s->td = t->td;
		s->sugar = t->sugar;
		ndl_dup(obpe,t->lcm,s->lcm);
	}
	if ( s0 ) NEXT(s) = 0;
	prev_nm_free_list = 0;
	prev_ndp_free_list = 0;
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

ND nd_dup(ND p,int obpe)
{
	NM m,mr,mr0;
	int c,n;
	ND r;

	if ( !p )
		return 0;
	else {
		n = NV(p); m = BDY(p);
		for ( mr0 = 0; m; m = NEXT(m) ) {
			NEXTNM(mr0,mr);
			C(mr) = C(m);
			mr->td = m->td;
			ndl_dup(obpe,m->dl,mr->dl);
		}
		NEXT(mr) = 0; 
		MKND(NV(p),mr0,r);
		r->sugar = p->sugar;
		return r;
	}
}
