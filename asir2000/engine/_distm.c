/* $OpenXM$ */
#include "ca.h"
#include "inline.h"

#define NV(p) ((p)->nv)
#define C(p) ((p)->c)
#if 0
#define ITOS(p) (((unsigned int)(p))>>1)
#define STOI(i) ((P)((((unsigned int)(i))<<1)|1))
#else
#define ITOS(p) (((unsigned int)(p))&0x7fffffff)
#define STOI(i) ((P)((unsigned int)(i)|0x80000000))
#endif

extern int (*cmpdl)();
extern int do_weyl;

void _dptodp();
void adddl_dup();
void _mulmdm_dup();
void _free_dp();
void _comm_mulmd_dup();
void _weyl_mulmd_dup();
void _weyl_mulmmm_dup();
void _weyl_mulmdm_dup();

MP _mp_free_list;
DP _dp_free_list;
DL _dl_free_list;
int current_dl_length;

#define _NEWDL_NOINIT(d,n) if ((n)!= current_dl_length){_dl_free_list=0; current_dl_length=(n);} if(!_dl_free_list)_DL_alloc(); (d)=_dl_free_list; _dl_free_list = *((DL *)_dl_free_list)
#define _NEWDL(d,n) if ((n)!= current_dl_length){_dl_free_list=0; current_dl_length=(n);} if(!_dl_free_list)_DL_alloc(); (d)=_dl_free_list; _dl_free_list = *((DL *)_dl_free_list); bzero((d),(((n)+1)*sizeof(int)))
#define _NEWMP(m) if(!_mp_free_list)_MP_alloc(); (m)=_mp_free_list; _mp_free_list = NEXT(_mp_free_list)
#define _MKDP(n,m,d) if(!_dp_free_list)_DP_alloc(); (d)=_dp_free_list; _dp_free_list = (DP)BDY(_dp_free_list); (d)->nv=(n); BDY(d)=(m)

#define _NEXTMP(r,c) \
if(!(r)){_NEWMP(r);(c)=(r);}else{_NEWMP(NEXT(c));(c)=NEXT(c);}

#define _NEXTMP2(r,c,s) \
if(!(r)){(c)=(r)=(s);}else{NEXT(c)=(s);(c)=(s);}

#define FREEDL(m) *((DL *)m)=_dl_free_list; _dl_free_list=(m)
#define FREEMP(m) NEXT(m)=_mp_free_list; _mp_free_list=(m)
#define FREEDP(m) BDY(m)=(MP)_dp_free_list; _dp_free_list=(m)


void _DL_alloc()
{
	int *p;
	int i,dl_len;
	static int DL_alloc_count;

/*	fprintf(stderr,"DL_alloc : %d \n",++DL_alloc_count); */
	dl_len = (current_dl_length+1);
	p = (int *)GC_malloc(128*dl_len*sizeof(int));
	for ( i = 0; i < 128; i++, p += dl_len ) {
		*(DL *)p = _dl_free_list;
		_dl_free_list = (DL)p;
	}
}

void _MP_alloc()
{
	MP p;
	int i;
	static int MP_alloc_count;

/*	fprintf(stderr,"MP_alloc : %d \n",++MP_alloc_count); */
	p = (MP)GC_malloc(1024*sizeof(struct oMP));
	for ( i = 0; i < 1024; i++ ) {
		p[i].next = _mp_free_list; _mp_free_list = &p[i];
	}
}
		
void _DP_alloc()
{
	DP p;
	int i;
	static int DP_alloc_count;

/*	fprintf(stderr,"DP_alloc : %d \n",++DP_alloc_count); */
	p = (DP)GC_malloc(1024*sizeof(struct oDP));
	for ( i = 0; i < 1024; i++ ) {
		p[i].body = (MP)_dp_free_list; _dp_free_list = &p[i];
	}
}

/* merge p1 and p2 into pr */

void _addmd_destructive(mod,p1,p2,pr)
int mod;
DP p1,p2,*pr;
{
	int n;
	MP m1,m2,mr,mr0,s;
	int t;

	if ( !p1 )
		*pr = p2;
	else if ( !p2 )
		*pr = p1;
	else {
		for ( n = NV(p1), m1 = BDY(p1), m2 = BDY(p2), mr0 = 0; m1 && m2; )
			switch ( (*cmpdl)(n,m1->dl,m2->dl) ) {
				case 0:
					t = (ITOS(C(m1))+ITOS(C(m2))) - mod;
					if ( t < 0 )
						t += mod;
					s = m1; m1 = NEXT(m1);
					if ( t ) {
						_NEXTMP2(mr0,mr,s); C(mr) = STOI(t);
					} else {
						FREEDL(s->dl); FREEMP(s);
					}
					s = m2; m2 = NEXT(m2); FREEDL(s->dl); FREEMP(s);
					break;
				case 1:
					s = m1; m1 = NEXT(m1); _NEXTMP2(mr0,mr,s);
					break;
				case -1:
					s = m2; m2 = NEXT(m2); _NEXTMP2(mr0,mr,s);
					break;
			}
		if ( !mr0 )
			if ( m1 )
				mr0 = m1;
			else if ( m2 )
				mr0 = m2;
			else {
				*pr = 0;
				return;
			}
		else if ( m1 )
			NEXT(mr) = m1;
		else if ( m2 )
			NEXT(mr) = m2;
		else
			NEXT(mr) = 0;
		_MKDP(NV(p1),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = MAX(p1->sugar,p2->sugar);
		FREEDP(p1); FREEDP(p2);
	}
}

void _mulmd_dup(mod,p1,p2,pr)
int mod;
DP p1,p2,*pr;
{
	if ( !do_weyl )
		_comm_mulmd_dup(mod,p1,p2,pr);
	else
		_weyl_mulmd_dup(mod,p1,p2,pr);
}

void _comm_mulmd_dup(mod,p1,p2,pr)
int mod;
DP p1,p2,*pr;
{
	MP m;
	DP s,t,u;
	int i,l,l1;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else {
		for ( m = BDY(p1), l1 = 0; m; m = NEXT(m), l1++ );
		for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
		if ( l1 < l ) {
			t = p1; p1 = p2; p2 = t;
			l = l1;
		}
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			_mulmdm_dup(mod,p1,w[i],&t); _addmd_destructive(mod,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

void _weyl_mulmd_dup(mod,p1,p2,pr)
int mod;
DP p1,p2,*pr;
{
	MP m;
	DP s,t,u;
	int i,l,l1;
	static MP *w;
	static int wlen;

	if ( !p1 || !p2 )
		*pr = 0;
	else {
		for ( m = BDY(p2), l = 0; m; m = NEXT(m), l++ );
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p2), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( s = 0, i = l-1; i >= 0; i-- ) {
			_weyl_mulmdm_dup(mod,p1,w[i],&t); _addmd_destructive(mod,s,t,&u); s = u;
		}
		bzero(w,l*sizeof(MP));
		*pr = s;
	}
}

void _mulmdm_dup(mod,p,m0,pr)
int mod;
DP p;
MP m0;
DP *pr;
{
	MP m,mr,mr0;
	DL d,dt,dm;
	int c,n,r,i;
	int *pt,*p1,*p2;

	if ( !p )
		*pr = 0;
	else {
		for ( mr0 = 0, m = BDY(p), c = ITOS(C(m0)), d = m0->dl, n = NV(p); 
			m; m = NEXT(m) ) {
			_NEXTMP(mr0,mr);
			C(mr) = STOI(dmar(ITOS(C(m)),c,0,mod));
			_NEWDL_NOINIT(dt,n); mr->dl = dt;
			dm = m->dl;
			dt->td = d->td + dm->td;
			for ( i = 0, pt = dt->d, p1=d->d, p2 = dm->d; i < n; i++ )
				*pt++ = *p1++ + *p2++;
		}
		NEXT(mr) = 0; _MKDP(NV(p),mr0,*pr);
		if ( *pr )
			(*pr)->sugar = p->sugar + m0->dl->td;
	}
}

void _weyl_mulmdm_dup(mod,p,m0,pr)
int mod;
DP p;
MP m0;
DP *pr;
{
	DP r,t,t1;
	MP m;
	int n,l,i;
	static MP *w;
	static int wlen;

	if ( !p )
		*pr = 0;
	else {
		for ( m = BDY(p), l = 0; m; m = NEXT(m), l++ );
		if ( l > wlen ) {
			if ( w ) GC_free(w);
			w = (MP *)MALLOC(l*sizeof(MP));
			wlen = l;
		}
		for ( m = BDY(p), i = 0; i < l; m = NEXT(m), i++ )
			w[i] = m;
		for ( r = 0, i = l-1, n = NV(p); i >= 0; i-- ) {
			_weyl_mulmmm_dup(mod,w[i],m0,n,&t);
			_addmd_destructive(mod,r,t,&t1); r = t1;
		}
		bzero(w,l*sizeof(MP));
		if ( r )
			r->sugar = p->sugar + m0->dl->td;
		*pr = r;
	}
}
/* m0 = x0^d0*x1^d1*... * dx0^d(n/2)*dx1^d(n/2+1)*... */

void _weyl_mulmmm_dup(mod,m0,m1,n,pr)
int mod;
MP m0,m1;
int n;
DP *pr;
{
	MP m,mr,mr0;
	DP r,t,t1;
	int c,c0,c1,cc;
	DL d,d0,d1;
	int i,j,a,b,k,l,n2,s,min,h;
	static int *tab;
	static int tablen;

	if ( !m0 || !m1 )
		*pr = 0;
	else {
		c0 = ITOS(C(m0)); c1 = ITOS(C(m1));
		c = dmar(c0,c1,0,mod);
		d0 = m0->dl; d1 = m1->dl;
		n2 = n>>1;

		_NEWDL(d,n);
		if ( n & 1 )
			/* offset of h-degree */
			d->td = d->d[n-1] = d0->d[n-1]+d1->d[n-1];
		else
			d->td = 0;
		_NEWMP(mr); mr->c = STOI(c); mr->dl = d; NEXT(mr) = 0;
		_MKDP(n,mr,r); r->sugar = d->td;

		/* homogenized computation; dx-xd=h^2 */
		for ( i = 0; i < n2; i++ ) {
			a = d0->d[i]; b = d1->d[n2+i];
			k = d0->d[n2+i]; l = d1->d[i];
			/* degree of xi^a*(Di^k*xi^l)*Di^b */
			s = a+k+l+b;
			/* compute xi^a*(Di^k*xi^l)*Di^b */
			min = MIN(k,l);

			if ( min+1 > tablen ) {
				if ( tab ) GC_free(tab);
				tab = (int *)MALLOC((min+1)*sizeof(int));
				tablen = min+1;
			}
			mkwcm(k,l,mod,tab);
			if ( n & 1 )
				for ( mr0 = 0, j = 0; j <= min; j++ ) {
					_NEXTMP(mr0,mr); _NEWDL(d,n);
					d->d[i] = l-j+a; d->d[n2+i] = k-j+b;
					d->td = s;
					d->d[n-1] = s-(d->d[i]+d->d[n2+i]); 
					mr->c = STOI(tab[j]); mr->dl = d;
				}
			else
				for ( mr0 = 0, s = 0, j = 0; j <= min; j++ ) {
					_NEXTMP(mr0,mr); _NEWDL(d,n);
					d->d[i] = l-j+a; d->d[n2+i] = k-j+b;
					d->td = d->d[i]+d->d[n2+i]; /* XXX */
					s = MAX(s,d->td); /* XXX */
					mr->c = STOI(tab[j]); mr->dl = d;
				}
			bzero(tab,(min+1)*sizeof(int));
			if ( mr0 )
				NEXT(mr) = 0;
			_MKDP(n,mr0,t);
			if ( t )
				t->sugar = s;
			_comm_mulmd_dup(mod,r,t,&t1); 
			_free_dp(r); _free_dp(t);
			r = t1;
		}
		*pr = r;
	}
}

void _dp_red_mod_destructive(p1,p2,mod,rp)
DP p1,p2;
int mod;
DP *rp;
{
	int i,n;
	DL d1,d2,d;
	MP m;
	DP t,s;
	int c,c1;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	_NEWDL(d,n); d->td = d1->td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = d1->d[i]-d2->d[i];
	c = invm(ITOS(BDY(p2)->c),mod); c1 = dmar(c,ITOS(BDY(p1)->c),0,mod);
	_NEWMP(m); m->dl = d; m->c = STOI(mod-c1); NEXT(m) = 0;
	_MKDP(n,m,s); s->sugar = d->td;
	_mulmd_dup(mod,s,p2,&t); _free_dp(s);
	_addmd_destructive(mod,p1,t,rp);
}

void _dp_sp_mod_dup(p1,p2,mod,rp)
DP p1,p2;
int mod;
DP *rp;
{
	int i,n,td;
	int *w;
	DL d1,d2,d;
	MP m;
	DP t,s,u;

	n = p1->nv; d1 = BDY(p1)->dl; d2 = BDY(p2)->dl;
	w = (int *)ALLOCA(n*sizeof(int));
	for ( i = 0, td = 0; i < n; i++ ) {
		w[i] = MAX(d1->d[i],d2->d[i]); td += w[i];
	}
	_NEWDL(d,n); d->td = td - d1->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d1->d[i];
	_NEWMP(m); m->dl = d; m->c = BDY(p2)->c; NEXT(m) = 0;
	_MKDP(n,m,s); s->sugar = d->td; _mulmd_dup(mod,s,p1,&t); _free_dp(s);
	_NEWDL(d,n); d->td = td - d2->td;
	for ( i = 0; i < n; i++ )
		d->d[i] = w[i] - d2->d[i];
	_NEWMP(m); m->dl = d; m->c = STOI(mod - ITOS(BDY(p1)->c)); NEXT(m) = 0;
	_MKDP(n,m,s); s->sugar = d->td; _mulmd_dup(mod,s,p2,&u); _free_dp(s);
	_addmd_destructive(mod,t,u,rp);
}

void _dltodl(d,dr)
DL d;
DL *dr;
{
	int i,n;
	DL t;

	n = current_dl_length;
	NEWDL(t,n); *dr = t;
	t->td = d->td;
	for ( i = 0; i < n; i++ )
		t->d[i] = d->d[i];	
}

void adddl_dup(n,d1,d2,dr)
int n;
DL d1,d2;
DL *dr;
{
	DL dt;
	int i;

	_NEWDL(dt,n); *dr = dt;
	dt->td = d1->td + d2->td;
	for ( i = 0; i < n; i++ )
		dt->d[i] = d1->d[i]+d2->d[i];
}

void _free_dp(f)
DP f;
{
	MP m,s;

	if ( !f )
		return;
	m = f->body;
	while ( m ) {
		s = m; m = NEXT(m); FREEDL(s->dl); FREEMP(s);
	}
	FREEDP(f);
}

void _dptodp(p,r)
DP p;
DP *r;
{
	MP m,mr0,mr;

	if ( !p )
		*r = 0;
	else {
		for ( m = BDY(p), mr0 = 0; m; m = NEXT(m) ) {
			NEXTMP(mr0,mr);
			_dltodl(m->dl,&mr->dl);
			mr->c = m->c;
		}
		NEXT(mr) = 0;
		MKDP(p->nv,mr0,*r);
		(*r)->sugar = p->sugar;
	}
}

void _dp_nf_mod_destructive(b,g,ps,mod,full,rp)
NODE b;
DP g;
DP *ps;
int mod,full;
DP *rp;
{
	DP u,p,d,s,t;
	NODE l;
	MP m,mr,mrd;
	int sugar,psugar,n,h_reducible,i;

	if ( !g ) {
		*rp = 0; return;
	}
	sugar = g->sugar;
	n = g->nv;
	for ( d = 0; g; ) {
		for ( h_reducible = 0, l = b; l; l = NEXT(l) ) {
			if ( dp_redble(g,p = ps[(int)BDY(l)]) ) {
				h_reducible = 1;
				psugar = (BDY(g)->dl->td - BDY(p)->dl->td) + p->sugar;
				_dp_red_mod_destructive(g,p,mod,&u); g = u;
				sugar = MAX(sugar,psugar);
				if ( !g ) {
					if ( d )
						d->sugar = sugar;
					_dptodp(d,rp); _free_dp(d); return;
				}
				break;
			}
		}
		if ( !h_reducible ) {
			/* head term is not reducible */
			if ( !full ) {
				if ( g )
					g->sugar = sugar;
				_dptodp(g,rp); _free_dp(g); return;
			} else {
				m = BDY(g); 
				if ( NEXT(m) ) {
					BDY(g) = NEXT(m); NEXT(m) = 0;
				} else {
					FREEDP(g); g = 0;
				}
				if ( d ) {
					for ( mrd = BDY(d); NEXT(mrd); mrd = NEXT(mrd) );
					NEXT(mrd) = m;
				} else {
					_MKDP(n,m,d);
				}
			}
		}
	}
	if ( d )
		d->sugar = sugar;
	_dptodp(d,rp); _free_dp(d);
}
