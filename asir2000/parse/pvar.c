/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 *
 * $OpenXM: OpenXM_contrib2/asir2000/parse/pvar.c,v 1.5 2000/12/05 01:24:57 noro Exp $ 
*/
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

void pushpvs(FUNC f)
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

int makepvar(char *str)
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

int searchpvar(char *str)
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

int getpvar(VS pvs,char *str,int searchonly)
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

#if defined(VISUAL)
	fclose(asir_infile->fp); 
	unlink(asir_infile->tname);
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

void storeans(pointer p)
{
	if ( APVS->asize == APVS->n )
		reallocarray((char **)&APVS->va,(int *)&APVS->asize,(int *)&APVS->n,(int)sizeof(struct oPV));
	APVS->va[APVS->n++].priv = p; 
}

#if 0
pointer evalpv(int id,NODE2 tree,pointer f)
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
