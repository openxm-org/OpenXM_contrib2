/* $OpenXM: OpenXM/src/asir99/parse/pvar.c,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
#include "ca.h"
#include "parse.h"

NODE PVSS;

void mkpvs()
 {
	VS pvs;

	pvs = (VS)MALLOC(sizeof(struct oVS));
	pvs->va = (struct oPV *)MALLOC(DEFSIZE*sizeof(struct oPV));
	pvs->n = 0;
	pvs->asize=DEFSIZE;
	CPVS = pvs; 
}

void pushpvs(f)
FUNC f;
{
	VS pvs;
	NODE node;
	int level;
	extern int evalstatline;

	pvs = f->f.usrf->pvs;
	if ( PVSS ) {
		((VS)BDY(PVSS))->at = evalstatline;
		level = ((VS)BDY(PVSS))->level+1;
	} else
		level = 1;
	MKNODE(node,pvs,PVSS);
	PVSS = node; 
	CPVS = (VS)MALLOC(sizeof(struct oVS)); BDY(PVSS) = (pointer)CPVS;
	CPVS->usrf = f;
	CPVS->n = CPVS->asize = pvs->n; 
	CPVS->level = level;
	if ( CPVS->n ) {
		CPVS->va = (struct oPV *)MALLOC(CPVS->n*sizeof(struct oPV));
		bcopy((char *)pvs->va,(char *)CPVS->va,(int)(pvs->n*sizeof(struct oPV)));
	}
	if ( nextbp )
		nextbplevel++;
}

void poppvs() {
	PVSS = NEXT(PVSS);
	if ( PVSS )
		CPVS = (VS)BDY(PVSS);
	else
		CPVS = GPVS;
	if ( nextbp )
		nextbplevel--;
}

int gdef;

int makepvar(str)
char *str;
{
	int c;

	c = getpvar(CPVS,str,0);
	if ( gdef ) {
		getpvar(EPVS,str,0);
		if ( CPVS != GPVS ) {
			getpvar(GPVS,str,0);
			CPVS->va[c].attr = 1;
		}
	} else if ( (CPVS != GPVS) && 
		(CPVS->va[c].attr || (getpvar(EPVS,str,1) >= 0)) ) {
		CPVS->va[c].attr = 1;
		c = (getpvar(GPVS,str,1)|MSB);
	}
	return c;
}

extern FUNC parse_targetf;

int searchpvar(str)
char *str;
{
	VS pvs;
	int i;

	if ( parse_targetf && parse_targetf->id != A_USR )
		return -1;
	pvs = parse_targetf ? parse_targetf->f.usrf->pvs : GPVS;
	i = getpvar(pvs,str,1);
	if ( ((i>=0)&&pvs->va[i].attr) || (i<0) )
		return getpvar(GPVS,str,1)|MSB;
	else
		return i;
}

int getpvar(pvs,str,searchonly)
VS pvs;
char *str;
int searchonly;
{
	struct oPV *va;
	PV v;
	int i;

	for ( va = pvs->va, i = 0; i < (int)pvs->n; i++ )
		if ( va[i].name && !strcmp(va[i].name,str) )
			return i;
	if ( searchonly )
		return -1;
	if ( pvs->asize == pvs->n )
		reallocarray((char **)&pvs->va,(int *)&pvs->asize,(int *)&pvs->n,(int)sizeof(struct oPV));
	v = &pvs->va[pvs->n]; 
	NAME(v) = (char *)CALLOC(strlen(str)+1,sizeof(char));
	strcpy(NAME(v),str); v->priv = 0;
	v->attr= 0; v->type = -1; i = pvs->n; pvs->n++;
	return i;
}

#if defined(VISUAL)
#define PCLOSE _pclose
#else
#define PCLOSE pclose
#endif

void closecurrentinput()
{
	if ( asir_infile && !asir_infile->fp )
		return;

#if defined(THINK_C) || defined(VISUAL)
#if defined(THINK_C)
	void setDir(short);

	fclose(asir_infile->fp); 
	setDir(asir_infile->vol);
	unlink(asir_infile->tname);
	resetDir();
#else
	fclose(asir_infile->fp); 
	unlink(asir_infile->tname);
#endif
#else
	PCLOSE(asir_infile->fp);
#endif
	asir_infile = NEXT(asir_infile);
	resetpvs();
}

void resetpvs()
{
	if ( !NEXT(asir_infile) ) {
		PVSS = 0; CPVS = GPVS; nextbp = 0;
		if ( EPVS->va ) {
			bzero((char *)EPVS->va,(int)(EPVS->asize*sizeof(struct oPV))); EPVS->n = 0;
		}
	}
}

static NODE saved_PVSS;
static VS saved_CPVS;
static int saved_nextbp, saved_nextbplevel;

void savepvs()
{
	saved_PVSS = PVSS;
	saved_CPVS = CPVS;
	saved_nextbp = nextbp;
	saved_nextbplevel = nextbplevel;
}

void restorepvs()
{
	PVSS = saved_PVSS;
	CPVS = saved_CPVS;
	nextbp = saved_nextbp;
	nextbplevel = saved_nextbplevel;
}

void storeans(p)
pointer p;
{
	if ( APVS->asize == APVS->n )
		reallocarray((char **)&APVS->va,(int *)&APVS->asize,(int *)&APVS->n,(int)sizeof(struct oPV));
	APVS->va[APVS->n++].priv = p; 
}

#if 1
pointer evalpv(id,expr,f)
int id;
FNODE expr;
pointer f;
{
	pointer a,val = 0;
	pointer *addr;

	if ( expr->id != I_PVAR && expr->id != I_CAST )
		error("evalpv : invalid assignment");
	switch ( id ) {
		case I_PRESELF:
			getmemberp((FNODE)expr,(Obj **)&addr);
			(*((ARF)f)->fp)(CO,*addr,ONE,&val); *addr = val; break;
		case I_POSTSELF:
			getmemberp((FNODE)expr,(Obj **)&addr);
			val = *addr; (*((ARF)f)->fp)(CO,*addr,ONE,&a); *addr = a; break;
		case I_PVAR:
			getmember((FNODE)expr,(Obj *)&val); break;
		case I_ASSPVAR:
			getmemberp((FNODE)expr,(Obj **)&addr); *addr = val = eval((FNODE)f); break;
	}
	return val;
}
#endif

#if 0
pointer evalpv(id,tree,f)
int id;
NODE2 tree;
pointer f;
{
	pointer a,val = 0;
	pointer *addr;
	int pv;
	NODE ind,tn;

	switch ( id ) {
		case I_PRESELF:
			getmemberp((FNODE)tree,(Obj **)&addr);
			(*((ARF)f)->fp)(CO,*addr,ONE,&val); *addr = val; break;
		case I_POSTSELF:
			getmemberp((FNODE)tree,(Obj **)&addr);
			val = *addr; (*((ARF)f)->fp)(CO,*addr,ONE,&a); *addr = a; break;
		case I_PVAR:
			pv = (int)BDY1(tree); ind = (NODE)BDY2(tree); GETPV(pv,a);
			if ( !ind )
				val = a;
			else {
				evalnodebody(ind,&tn); getarray(a,tn,&val); 
			}
			break;
		case I_ASSPVAR:
			pv = (int)BDY1(tree); ind = (NODE)BDY2(tree);
			if ( !ind ) {
				val = eval(f); ASSPV(pv,val);
			} else {
				GETPV(pv,a);
				if ( a ) { 
					evalnodebody(ind,&tn); putarray(a,tn,val = eval(f)); 
				}
			}
			break;
	}
	return val;
}
#endif
