/* $OpenXM: OpenXM/src/asir99/engine/up_gfpn.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include <math.h>

extern GC_dont_gc;
extern struct oEGT eg_chrem,eg_fore,eg_back;
extern int debug_up;
extern int up_lazy;

void crup_lm(ModNum **,int,int *,int,N,N,UP *);

void fft_mulup_lm(n1,n2,nr)
UP n1,n2;
UP *nr;
{
	ModNum *f1,*f2,*w,*fr;
	ModNum *frarray[1024];
	int modarray[1024];
	int frarray_index = 0;
	N m,m1,m2,lm_mod;
	int d1,d2,dmin,i,mod,root,d,cond,bound;
	UP r;

	if ( !n1 || !n2 ) {
		*nr = 0; return;
	}
	d1 = n1->d; d2 = n2->d; dmin = MIN(d1,d2);
	if ( !d1 || !d2 ) {
		mulup(n1,n2,nr); return;
	}
	getmod_lm(&lm_mod);
	if ( !lm_mod )
		error("fft_mulup_lm : current_mod_lm is not set");
	m = ONEN;
	bound = maxblenup(n1)+maxblenup(n2)+int_bits(dmin)+2;
	f1 = (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
	f2 = (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
	w = (ModNum *)ALLOCA(6*(1<<int_bits(d1+d2+1))*sizeof(ModNum));
	for ( i = 0; i < NPrimes; i++ ) {
		FFT_primes(i,&mod,&root,&d);
		if ( (1<<d) < d1+d2+1 )
			continue;
		modarray[frarray_index] = mod;
		frarray[frarray_index++] = fr 
			= (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
		uptofmarray(mod,n1,f1); 
		uptofmarray(mod,n2,f2); 
		cond = FFT_pol_product(d1,f1,d2,f2,fr,i,w);
		if ( cond )
			error("fft_mulup : error in FFT_pol_product");
		STON(mod,m1); muln(m,m1,&m2); m = m2;
		if ( n_bits(m) > bound ) {
/*			GC_dont_gc = 1; */
			crup_lm(frarray,d1+d2,modarray,frarray_index,m,lm_mod,&r);
			uptolmup(r,nr);
			GC_dont_gc = 0;
			return;
		}
	}
	error("fft_mulup : FFT_primes exhausted");
}

void fft_squareup_lm(n1,nr)
UP n1;
UP *nr;
{
	ModNum *f1,*w,*fr;
	ModNum *frarray[1024];
	int modarray[1024];
	int frarray_index = 0;
	N m,m1,m2,lm_mod;
	int d1,dmin,i,mod,root,d,cond,bound;
	UP r;

	if ( !n1 ) {
		*nr = 0; return;
	}
	d1 = n1->d; dmin = d1;
	if ( !d1 ) {
		mulup(n1,n1,nr); return;
	}
	getmod_lm(&lm_mod);
	if ( !lm_mod )
		error("fft_squareup_lm : current_mod_lm is not set");
	m = ONEN;
	bound = 2*maxblenup(n1)+int_bits(d1)+2;
	f1 = (ModNum *)ALLOCA((2*d1+1)*sizeof(ModNum));
	w = (ModNum *)ALLOCA(6*(1<<int_bits(2*d1+1))*sizeof(ModNum));
	for ( i = 0; i < NPrimes; i++ ) {
		FFT_primes(i,&mod,&root,&d);
		if ( (1<<d) < 2*d1+1 )
			continue;
		modarray[frarray_index] = mod;
		frarray[frarray_index++] = fr 
			= (ModNum *)ALLOCA((2*d1+1)*sizeof(ModNum));
		uptofmarray(mod,n1,f1); 
		cond = FFT_pol_square(d1,f1,fr,i,w);
		if ( cond )
			error("fft_mulup : error in FFT_pol_product");
		STON(mod,m1); muln(m,m1,&m2); m = m2;
		if ( n_bits(m) > bound ) {
/*			GC_dont_gc = 1; */
			crup_lm(frarray,2*d1,modarray,frarray_index,m,lm_mod,&r);
			uptolmup(r,nr);
			GC_dont_gc = 0;
			return;
		}
	}
	error("fft_squareup : FFT_primes exhausted");
}

void trunc_fft_mulup_lm(n1,n2,dbd,nr)
UP n1,n2;
int dbd;
UP *nr;
{
	ModNum *f1,*f2,*fr,*w;
	ModNum *frarray[1024];
	int modarray[1024];
	int frarray_index = 0;
	N m,m1,m2,lm_mod;
	int d1,d2,dmin,i,mod,root,d,cond,bound;
	UP r;

	if ( !n1 || !n2 ) {
		*nr = 0; return;
	}
	d1 = n1->d; d2 = n2->d; dmin = MIN(d1,d2);
	if ( !d1 || !d2 ) {
		tmulup(n1,n2,dbd,nr); return;
	}
	getmod_lm(&lm_mod);
	if ( !lm_mod )
		error("trunc_fft_mulup_lm : current_mod_lm is not set");
	m = ONEN;
	bound = maxblenup(n1)+maxblenup(n2)+int_bits(dmin)+2;
	f1 = (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
	f2 = (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
	w = (ModNum *)ALLOCA(6*(1<<int_bits(d1+d2+1))*sizeof(ModNum));
	for ( i = 0; i < NPrimes; i++ ) {
		FFT_primes(i,&mod,&root,&d);
		if ( (1<<d) < d1+d2+1 )
			continue;

		modarray[frarray_index] = mod;
		frarray[frarray_index++] = fr 
			= (ModNum *)ALLOCA((d1+d2+1)*sizeof(ModNum));
		uptofmarray(mod,n1,f1); 
		uptofmarray(mod,n2,f2);
		cond = FFT_pol_product(d1,f1,d2,f2,fr,i,w);
		if ( cond )
			error("fft_mulup : error in FFT_pol_product");
		STON(mod,m1); muln(m,m1,&m2); m = m2;
		if ( n_bits(m) > bound ) {
/*			GC_dont_gc = 1; */
			crup_lm(frarray,MIN(dbd-1,d1+d2),modarray,frarray_index,m,lm_mod,&r);
			uptolmup(r,nr);
			GC_dont_gc = 0;
			return;
		}
	}
	error("trunc_fft_mulup : FFT_primes exhausted");
}

void crup_lm(f,d,mod,index,m,lm_mod,r)
ModNum **f;
int d;
int *mod;
int index;
N m;
N lm_mod;
UP *r;
{
	double *k;
	double c2;
	unsigned int *w;
	unsigned int zi,au,al;
	UL a;
	int i,j,l,len;
	UP s,s1,u;
	struct oUP c;
	N t,ci,mi,qn;
	unsigned int **sum;
	unsigned int *sum_b;
	Q q;
	struct oEGT eg0,eg1;

	if ( !lm_mod )
		error("crup_lm : current_mod_lm is not set");
	k = (double *)ALLOCA((d+1)*sizeof(double));
	for ( j = 0; j <= d; j++ )
		k[j] = 0.5;
	up_lazy = 1;
	sum = (unsigned int **)ALLOCA((d+1)*sizeof(unsigned int *));
	len = (int_bits(index)+n_bits(lm_mod)+31)/32+1;
	w = (unsigned int *)ALLOCA((len+1)*sizeof(unsigned int));
	sum_b = (unsigned int *)MALLOC_ATOMIC((d+1)*len*sizeof(unsigned int));
	for ( j = 0; j <= d; j++ ) {
		sum[j] = sum_b+len*j;
		bzero((char *)sum[j],len*sizeof(unsigned int));
	}
	for ( i = 0, s = 0; i < index; i++ ) {
		divin(m,mod[i],&ci);
		zi = (unsigned int)invm((unsigned int)rem(ci,mod[i]),mod[i]);

		STON(zi,t); muln(ci,t,&mi); 
		divn(mi,lm_mod,&qn,&t);
		if ( t )
			for ( j = 0; j <= d; j++ ) {
				bzero((char *)w,(len+1)*sizeof(unsigned int));
				muln_1(BD(t),PL(t),(unsigned int)f[i][j],w);
				for ( l = PL(t); l >= 0 && !w[l]; l--);
				l++;
				if ( l )
					addarray_to(w,l,sum[j],len);
			}
		c2 = (double)zi/(double)mod[i];
		for ( j = 0; j <= d; j++ )
			k[j] += c2*f[i][j];
	}
	uiarraytoup(sum,len,d,&s);
	GC_free(sum_b);

	u = UPALLOC(d);
	for ( j = 0; j <= d; j++ ) {
#if 1
		a = (UL)floor(k[j]);
#if defined(i386) || defined(__alpha) || defined(VISUAL)
		au = ((unsigned int *)&a)[1];
		al = ((unsigned int *)&a)[0];
#else
		al = ((unsigned int *)&a)[1];
		au = ((unsigned int *)&a)[0];
#endif
		if ( au ) {
			NEWQ(q); SGN(q) = 1; NM(q)=NALLOC(2); DN(q)=0;
			PL(NM(q))=2; BD(NM(q))[0]=al; BD(NM(q))[1] = au;
		} else
			UTOQ(al,q); 
#else
		al = (int)floor(k[j]); STOQ(al,q);
#endif
		 u->c[j] = (Num)q;
	}
	for ( j = d; j >= 0 && !u->c[j]; j-- );
	if ( j < 0 )
		u = 0;
	else
		u->d = j;
	divn(m,lm_mod,&qn,&t); NTOQ(t,-1,q);
	c.d = 0; c.c[0] = (Num)q;
	mulup(u,&c,&s1);
	up_lazy = 0;

	addup(s,s1,r);
}

void fft_rembymulup_special_lm(n1,n2,inv2,nr)
UP n1,n2,inv2;
UP *nr;
{
	int d1,d2,d;
	UP r1,t,s,q,u;

	if ( !n2 )
		error("rembymulup : division by 0");
	else if ( !n1 || !n2->d )
		*nr = 0;
	else if ( (d1 = n1->d) < (d2 = n2->d) )
		*nr = n1;
	else {
		d = d1-d2;
		reverseup(n1,d1,&t); truncup(t,d+1,&r1);
		trunc_fft_mulup_lm(r1,inv2,d+1,&t);
		truncup(t,d+1,&s);
		reverseup(s,d,&q);
		trunc_fft_mulup_lm(q,n2,d2,&t); truncup(t,d2,&u);
		truncup(n1,d2,&s);
		subup(s,u,nr);
	}
}

void uptolmup(n,nr)
UP n;
UP *nr;
{
	int i,d;
	Q *c;
	LM *cr;
	UP r;

	if ( !n )
		*nr = 0;
	else {
		d = n->d; c = (Q *)n->c;
		*nr = r = UPALLOC(d); cr = (LM *)r->c;
		for ( i = 0; i <= d; i++ )
			qtolm(c[i],&cr[i]);
		for ( i = d; i >= 0 && !cr[i]; i-- );
		if ( i < 0 )
			*nr = 0;
		else
			r->d = i;
	}
}

void hybrid_powermodup(f,xp)
UP f;
UP *xp;
{
	N n;
	UP x,y,t,invf,s;
	int k;
	LM lm;
	struct oEGT eg_sq,eg_rem,eg_mul,eg_inv,eg0,eg1,eg2,eg3;

	getmod_lm(&n);
	if ( !n )
		error("hybrid_powermodup : current_mod_lm is not set");
	MKLM(ONEN,lm); 
	x = UPALLOC(1); x->d = 1; x->c[1] = (Num)lm;
	y = UPALLOC(0); y->d = 0; y->c[0] = (Num)lm;

	reverseup(f,f->d,&t);
	invmodup(t,f->d,&invf);
	for ( k = n_bits(n)-1; k >= 0; k-- ) {
		hybrid_squareup(FF_GFP,y,&t);
		hybrid_rembymulup_special(FF_GFP,t,f,invf,&s);
		y = s;
		if ( n->b[k/32] & (1<<(k%32)) ) {
			mulup(y,x,&t);
			remup(t,f,&s);
			y = s;
		}
	}
	*xp = y;
}

void powermodup(f,xp)
UP f;
UP *xp;
{
	N n;
	UP x,y,t,invf,s;
	int k;
	LM lm;
	struct oEGT eg_sq,eg_rem,eg_mul,eg_inv,eg0,eg1,eg2,eg3;

	getmod_lm(&n);
	if ( !n )
		error("powermodup : current_mod_lm is not set");
	MKLM(ONEN,lm); 
	x = UPALLOC(1); x->d = 1; x->c[1] = (Num)lm;
	y = UPALLOC(0); y->d = 0; y->c[0] = (Num)lm;

	reverseup(f,f->d,&t);
	invmodup(t,f->d,&invf);
	for ( k = n_bits(n)-1; k >= 0; k-- ) {
		ksquareup(y,&t);
		rembymulup_special(t,f,invf,&s);
		y = s;
		if ( n->b[k/32] & (1<<(k%32)) ) {
			mulup(y,x,&t);
			remup(t,f,&s);
			y = s;
		}
	}
	*xp = y;
}

/* g^d mod f */

void hybrid_generic_powermodup(g,f,d,xp)
UP g,f;
Q d;
UP *xp;
{
	N e;
	UP x,y,t,invf,s;
	int k;
	LM lm;
	struct oEGT eg_sq,eg_rem,eg_mul,eg_inv,eg0,eg1,eg2,eg3;

	e = NM(d);
	MKLM(ONEN,lm); 
	y = UPALLOC(0); y->d = 0; y->c[0] = (Num)lm;
	remup(g,f,&x);
	if ( !x ) {
		*xp = !d ? y : 0;
		return;
	} else if ( !x->d ) {
		pwrup(x,d,xp);
		return;
	}
	reverseup(f,f->d,&t);
	invmodup(t,f->d,&invf);
	for ( k = n_bits(e)-1; k >= 0; k-- ) {
		hybrid_squareup(FF_GFP,y,&t);
		hybrid_rembymulup_special(FF_GFP,t,f,invf,&s);
		y = s;
		if ( e->b[k/32] & (1<<(k%32)) ) {
			mulup(y,x,&t);
			remup(t,f,&s);
			y = s;
		}
	}
	*xp = y;
}

void generic_powermodup(g,f,d,xp)
UP g,f;
Q d;
UP *xp;
{
	N e;
	UP x,y,t,invf,s;
	int k;
	LM lm;
	struct oEGT eg_sq,eg_rem,eg_mul,eg_inv,eg0,eg1,eg2,eg3;

	e = NM(d);
	MKLM(ONEN,lm); 
	y = UPALLOC(0); y->d = 0; y->c[0] = (Num)lm;
	remup(g,f,&x);
	if ( !x ) {
		*xp = !d ? y : 0;
		return;
	} else if ( !x->d ) {
		pwrup(x,d,xp);
		return;
	}
	reverseup(f,f->d,&t);
	invmodup(t,f->d,&invf);
	for ( k = n_bits(e)-1; k >= 0; k-- ) {
		ksquareup(y,&t);
		rembymulup_special(t,f,invf,&s);
		y = s;
		if ( e->b[k/32] & (1<<(k%32)) ) {
			mulup(y,x,&t);
			remup(t,f,&s);
			y = s;
		}
	}
	*xp = y;
}

void hybrid_powertabup(f,xp,tab)
UP f;
UP xp;
UP *tab;
{
	UP y,t,invf;
	int i,d;
	LM lm;
	struct oEGT eg_rem,eg_mul,eg0,eg1,eg2;

	d = f->d;
	MKLM(ONEN,lm); 
	y = UPALLOC(0); y->d = 0; y->c[0] = (Num)lm;
	tab[0] = y;
	tab[1] = xp;

	reverseup(f,f->d,&t);
	invmodup(t,f->d,&invf);

	for ( i = 2; i < d; i++ ) {
		if ( debug_up )
			fprintf(stderr,".");
		hybrid_mulup(FF_GFP,tab[i-1],xp,&t);
		hybrid_rembymulup_special(FF_GFP,t,f,invf,&tab[i]);
	}
}

void powertabup(f,xp,tab)
UP f;
UP xp;
UP *tab;
{
	UP y,t,invf;
	int i,d;
	LM lm;
	struct oEGT eg_rem,eg_mul,eg0,eg1,eg2;

	d = f->d;
	MKLM(ONEN,lm); 
	y = UPALLOC(0); y->d = 0; y->c[0] = (Num)lm;
	tab[0] = y;
	tab[1] = xp;

	reverseup(f,f->d,&t);
	invmodup(t,f->d,&invf);

	for ( i = 2; i < d; i++ ) {
		if ( debug_up )
			fprintf(stderr,".");
		kmulup(tab[i-1],xp,&t);
		rembymulup_special(t,f,invf,&tab[i]);
	}
}
