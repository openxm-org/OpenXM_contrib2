/* $OpenXM: OpenXM/src/asir99/engine/lmi.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "base.h"
#include "inline.h"

typedef struct oGEN_LM {
	int id;
	N dense;
	N sparse;
	int bit;
	unsigned int lower;
} *GEN_LM;

void pwrlm0(N,N,N *);
void gen_simpn(N,N*);
void gen_simpn_force(N,N*);

int lm_lazy;

static GEN_LM current_mod_lm;

void random_lm(r)
LM *r;
{
	N n,t;

	if ( !current_mod_lm )
		error("random_lm : current_mod_lm is not set");
	randomn(current_mod_lm->bit+1,&n);
	gen_simpn_force(n,&t);
	MKLM(t,*r);
}

void ntosparsen(p,bits)
N p;
N *bits;
{
	int l,i,j,nz;
	N r;
	unsigned int *pb,*rb;
	if ( !p )
		*bits = 0;
	else {
		l = n_bits(p);
		pb = BD(p);
		for ( i = l-1, nz = 0; i >= 0; i-- )
			if ( pb[i>>5]&(1<<i&31) ) nz++;
		*bits = r = NALLOC(nz); PL(r) = nz;
		rb = BD(r);
		for ( i = l-1, j = 0; i >= 0; i-- )
			if ( pb[i>>5]&(1<<i&31) ) 
				rb[j++] = i;
	}
}

void setmod_lm(p)
N p;
{
	int i;

	if ( !current_mod_lm )
		current_mod_lm = (GEN_LM)MALLOC(sizeof(struct oGEN_LM));
	bzero((char *)current_mod_lm,sizeof(struct oGEN_LM));
	current_mod_lm->dense = p;
	ntosparsen(p,&current_mod_lm->sparse);
	current_mod_lm->bit = n_bits(p);
	current_mod_lm->id = UP2_DENSE; /* use ordinary rep. by default */
	if ( !(current_mod_lm->bit%32) ) {
		for ( i = PL(p)-1; i >= 1; i-- )
			if ( BD(p)[i] != 0xffffffff )
				break;
		if ( !i ) {
			current_mod_lm->lower = (unsigned int)((((UL)1)<<32)-((UL)BD(p)[0]));
			current_mod_lm->id = UP2_SPARSE; /* XXX */
		}
	}
}

void getmod_lm(p)
N *p;
{
	if ( !current_mod_lm )
		*p = 0;
	else
		*p = current_mod_lm->dense;
}

void simplm(n,r)
LM n;
LM *r;
{
	N rem;

	if ( !n )
		*r = 0;
	else if ( NID(n) != N_LM )
		*r = n;
	else {
		gen_simpn(n->body,&rem);
		MKLM(rem,*r);
	}
}

void qtolm(q,l)
Q q;
LM *l;
{
	N rn;
	LM a,b,c;

	if ( !q || (OID(q)==O_N && ((NID(q) == N_LM) || (NID(q) == N_GFPN))) ) { /* XXX */
		*l = (LM)q;
	} else if ( OID(q) == O_N && NID(q) == N_Q ) {
		gen_simpn(NM(q),&rn); MKLM(rn,a);
		if ( SGN(q) < 0 ) {
			chsgnlm(a,&c); a = c;
		}
		if ( DN(q) ) {
			gen_simpn_force(DN(q),&rn); MKLM(rn,b);
			if ( !b )
				error("qtolm : invalid denominator");
			divlm(a,b,l);
		} else
			*l = a;
	} else
		error("qtolm : invalid argument");
}

#define NZLM(a) ((a)&&(NID(a)==N_LM))

void addlm(a,b,c)
LM a,b;
LM *c;
{
	int s;
	N t,t1;
	LM z;

	qtolm((Q)a,&z); a = z; qtolm((Q)b,&z); b = z;
	if ( !a )
		*c = b;
	else if ( !b )
		*c = a;
	else {
		addn(a->body,b->body,&t);
		gen_simpn(t,&t1);
		MKLM(t1,*c);
	}
}

void sublm(a,b,c)
LM a,b;
LM *c;
{
	int s;
	N t,t1,mod;
	LM z;

	qtolm((Q)a,&z); a = z; qtolm((Q)b,&z); b = z;
	if ( !b )
		*c = a;
	else if ( !a )
		chsgnlm(b,c);
	else {
		s = subn(a->body,b->body,&t);
		if ( !s )
			*c = 0;
		else {
			MKLM(t,z);
			if ( s < 0 )
				chsgnlm(z,c);
			else
				*c = z;	
		}
	}
}

void mullm(a,b,c)
LM a,b;
LM *c;
{
	N t,q,r;
	LM z;

	qtolm((Q)a,&z); a = z; qtolm((Q)b,&z); b = z;
	if ( !a || !b )
		*c = 0;
	else {
		kmuln(a->body,b->body,&t);
		gen_simpn(t,&r);
		MKLM(r,*c);	
	}
}

void divlm(a,b,c)
LM a,b;
LM *c;
{
	LM r;
	Q t,m,i;
	LM z;

	qtolm((Q)a,&z); a = z; qtolm((Q)b,&z); b = z;
	if ( !b )
		error("divlm: division by 0");
	else if ( !a )
		*c = 0;
	else {
		NTOQ(b->body,1,t); NTOQ(current_mod_lm->dense,1,m);
		invl(t,m,&i);
		MKLM(NM(i),r);
		mullm(a,r,c);
	}
}

void chsgnlm(a,c)
LM a,*c;
{
	LM t;
	N s,u;

	if ( !a )
		*c = a;
	else {
		qtolm((Q)a,&t); a = t;
		gen_simpn_force(a->body,&s);
		subn(current_mod_lm->dense,s,&u);
		MKLM(u,*c);
	}
}

void pwrlm(a,b,c)
LM a;
Q b;
LM *c;
{
	LM t;
	N s;

	if ( !b ) {
		MKLM(ONEN,*c);
	} else if ( !a )
		*c = 0;
	else {
		qtolm((Q)a,&t); a = t;
		pwrlm0(a->body,NM(b),&s);
		MKLM(s,*c);
	}
}

void pwrlm0(a,n,c)
N a,n;
N *c;
{
	N n1,t,t1,t2,r1;
	int r;

	if ( !n )
		*c = ONEN;
	else if ( UNIN(n) )
		*c = a;
	else {
		r = divin(n,2,&n1);
		pwrlm0(a,n1,&t);
		kmuln(t,t,&t1); gen_simpn(t1,&r1);
		if ( r ) {
			kmuln(r1,a,&t2); gen_simpn(t2,&r1);
		}
		*c = r1;
	}
}

int cmplm(a,b)
LM a,b;
{
	LM z;

	qtolm((Q)a,&z); a = z; qtolm((Q)b,&z); b = z;
	if ( !a )
		if ( !b )
			return 0;
		else
			return -1;
	else if ( !b )
			return 1;
	else
		return cmpn(a->body,b->body);
}

void remn_special(N,N,int,unsigned int ,N *);

void gen_simpn(a,b)
N a;
N *b;
{
	if ( !current_mod_lm )
		error("gen_simpn: current_mod_lm is not set");
	if ( lm_lazy )
		*b = a;
	else if ( current_mod_lm->id == UP2_SPARSE )
		remn_special(a,current_mod_lm->dense,current_mod_lm->bit,current_mod_lm->lower,b);	
	else
		remn(a,current_mod_lm->dense,b);	
}

void gen_simpn_force(a,b)
N a;
N *b;
{
	if ( !current_mod_lm )
		error("gen_simpn_force: current_mod_lm is not set");
	else if ( current_mod_lm->id == UP2_SPARSE )
		remn_special(a,current_mod_lm->dense,current_mod_lm->bit,current_mod_lm->lower,b);	
	else
		remn(a,current_mod_lm->dense,b);	
}

