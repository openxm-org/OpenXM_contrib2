/* $OpenXM: OpenXM/src/asir99/parse/struct.c,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "comp.h"

struct oSS oLSS;
SS LSS = &oLSS;

void structdef(name,member)
char *name;
NODE member;
{
	int i,j,k,type;
	SDEF sdef,s;
	NODE n,ms;
	char *mname,*sname;

	for ( s = LSS->sa, i = 0; i < LSS->n; i++ )
		if ( !strcmp(s[i].name,name) ) {
			fprintf(stderr,"redeclaration of %s\n",name); break;
		}
	if ( !LSS->sa || ((i == LSS->n)&&(LSS->n==LSS->asize)) )
		reallocarray((char **)&LSS->sa,(int *)&LSS->asize,(int *)&LSS->n,(int)sizeof(struct oSDEF));
	sdef = &LSS->sa[i];
	if ( i == LSS->n )
		LSS->n++;
	for ( i = 0, n = member; n; n = NEXT(n), i++ );
	sdef->name = (char *)MALLOC(strlen(name)+1);
	bcopy(name,sdef->name,strlen(name)+1);
	for ( j = 0, n = member; n; n = NEXT(n) )
		for ( ms = NEXT((NODE)BDY(n)); ms; ms = NEXT(ms), j++ );
	sdef->n = j;
	sdef->f = (struct oFIELD *)MALLOC(sdef->n*sizeof(struct oFIELD));
	for ( j = 0, n = member; n; n = NEXT(n) ) {
		ms = (NODE)BDY(n);
		if ( sname = (char *)BDY(ms) ) {
			for ( s = LSS->sa, k = 0; k < LSS->n; k++ )
				if ( !strcmp(s[k].name,sname) )
					break;
			if ( k == LSS->n ) {
				fprintf(stderr,"%s: undefined structure\n",sname); return;
			} else
				type = k;
		} else
			type = -1;
		for ( ms = NEXT(ms); ms; ms = NEXT(ms), j++ ) {
			mname = (char *)MALLOC(strlen(BDY(ms))+1);	
			bcopy(BDY(ms),mname,strlen(BDY(ms))+1);
			sdef->f[j].name = mname; sdef->f[j].type = type;		
		}
	}
}

void setstruct(name,vars)
char *name;
NODE vars;
{
	SDEF s;
	int i;
	NODE n;

	for ( s = LSS->sa, i = 0; i < LSS->n; i++ )
		if ( !strcmp(s[i].name,name) )
			break;
	if ( i == LSS->n ) {
		fprintf(stderr,"%s: undefined structure\n",name); return;
	}
	s = &LSS->sa[i];
	for ( n = vars; n; n = NEXT(n) )
		CPVS->va[(int)BDY(n)].type = i;
}

void newstruct(type,rp)
int type;
COMP *rp;
{
	NEWCOMP(*rp,LSS->sa[type].n); (*rp)->type = type;
}

int structtoindex(name)
char *name;
{
	SDEF s;
	int i;

	for ( s = LSS->sa, i = 0; i < LSS->n; i++ )
		if ( !strcmp(s[i].name,name) )
			break;
	if ( i == LSS->n ) {
		fprintf(stderr,"%s: undefined structure\n",name); return -1; /* XXX */
	} else
		return i;
}

int membertoindex(type,name)
int type;
char *name;
{
	SDEF s;
	FIELD f;
	int i;

	s = &LSS->sa[type];
	for ( f = s->f, i = 0; i < s->n; i++ )
		if ( !strcmp(f[i].name,name) )
			break;
	if ( i == s->n ) {
		fprintf(stderr,"structure has no member named `%s'\n",name);
		return -1;
	} else
		return i;
}

int indextotype(type,index)
int type,index;
{
	return LSS->sa[type].f[index].type;
}

int gettype(index)
unsigned int index;
{
	if ( MSB & index )
		return (int)GPVS->va[index&~MSB].type;
	else
		return (int)CPVS->va[index].type;
}

int getcompsize(type)
int type;
{
	return LSS->sa[type].n;
}

void getmember(expr,memp)
FNODE expr;
Obj *memp;
{
	int i;
	FNODE root;
	COMP t;
	PV v0;
	NODE2 mchain;

	root = (FNODE)FA1(expr); mchain = (NODE2)FA2(expr);
	if ( ID(root) == I_PVAR ) {
		i = (int)FA0(root);
		v0 = i>=0?&CPVS->va[(unsigned int)i]:&GPVS->va[((unsigned int)i)&~MSB];
		t = (COMP)v0->priv;
	} else
		t = (COMP)eval(root);
	for ( ; mchain; mchain = NEXT(mchain) )
		t = (COMP)(t->member[(int)BDY1(mchain)]);
	*memp = (Obj)t;
}

void getmemberp(expr,addrp)
FNODE expr;
Obj **addrp;
{
	int i;
	FNODE root;
	COMP t;
	PV v0;
	COMP *addr;
	NODE2 mchain;

	root = (FNODE)FA1(expr); mchain = (NODE2)FA2(expr);
	if ( ID(root) == I_PVAR ) {
		i = (int)FA0(root);
		v0 = i>=0?&CPVS->va[(unsigned int)i]:&GPVS->va[((unsigned int)i)&~MSB];
		addr = (COMP *)&v0->priv;
	} else {
		t = (COMP)eval(root);
		addr = (COMP *)&t;
	}
	for ( ; mchain; mchain = NEXT(mchain) )
		addr = (COMP *)&((*addr)->member[(int)BDY1(mchain)]);
	*addrp = (Obj *)addr;
}

void getarrayp(a,ind,addrp)
Obj a;
NODE ind;
Obj **addrp;
{
	Obj len,row,col;
	Obj *addr;

	switch ( OID(a) ) {
		case O_VECT:
			len = (Obj)BDY(ind);
			if ( !rangecheck(len,((VECT)a)->len) )
				error("getarrayp : Out of range");
			else
				addr = (Obj *)&(BDY((VECT)a)[QTOS((Q)len)]);
			break;
		case O_MAT:
			row = (Obj)BDY(ind); col = (Obj)BDY(NEXT(ind));
			if ( !rangecheck(row,((MAT)a)->row) 
				|| !rangecheck(col,((MAT)a)->col) )
				error("getarrayp : Out of range");
			else
				addr = (Obj *)&(BDY((MAT)a)[QTOS((Q)row)][QTOS((Q)col)]);
			break;
		default:
				addr = 0;
			break;
	}
	*addrp = addr;
}

void memberofstruct(pexpr,name,exprp)
FNODE pexpr;
char *name;
FNODE *exprp;
{
	NODE2 n,*np;
	int type,ind;

	if ( pexpr->id != I_CAST ) {
		if ( pexpr->id == I_PVAR )
			pexpr = (FNODE)mkfnode(3,I_CAST,gettype((unsigned int)pexpr->arg[0]),pexpr,0);
		else
			error("memberofstruct : ???");
	}		
	if ( !pexpr->arg[2] ) {
		np = (NODE2 *)&(pexpr->arg[2]); type = (int)pexpr->arg[0];
	} else {
		for ( n = (NODE2)pexpr->arg[2]; NEXT(n); n = NEXT(n) );
		np = &NEXT(n); type = (int)BDY2(n);
	}
	ind = membertoindex(type,name);
	MKNODE2(*np,ind,indextotype(type,ind),0);
	*exprp = pexpr;
}
