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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/pvar.c,v 1.18 2006/06/21 09:46:06 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"

NODE PVSS;
extern char *CUR_FUNC;
int gdef,mgdef,ldef;


void mkpvs(char *fname)
{
	VS pvs;
	char *fullname,*buf;
	FUNC f;

	if ( CUR_MODULE ) {
		/* function must be declared in advance */
		searchf(CUR_MODULE->usrf_list,fname,&f);
		if ( !f ) {
			buf = ALLOCA(strlen("undeclared function "+strlen(fname)+10));
			sprintf(buf,"undeclared function `%s'",fname);
			yyerror(buf);
			error("cannot continue to read inputs");
		}
	}
	pvs = (VS)MALLOC(sizeof(struct oVS));
	pvs->va = (struct oPV *)MALLOC(DEFSIZE*sizeof(struct oPV));
	pvs->n = 0;
	pvs->asize=DEFSIZE;
	CPVS = pvs; 

	/* XXX */
	if ( CUR_MODULE ) {
		fullname = 
			(char *)MALLOC_ATOMIC(strlen(CUR_MODULE->name)+strlen(fname)+1);
		sprintf(fullname,"%s.%s",CUR_MODULE->name,fname);
		CUR_FUNC = fullname;
	} else
		CUR_FUNC = fname;
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

#define IS_LOCAL 0
#define IS_GLOBAL 1
#define IS_MGLOBAL 2
#define IS_PATTERN 3


unsigned int makepvar(char *str)
{
	int c,c1,created;
	char *buf;

	if ( str[0] == '_' ) {
		/* pattern variable */
		c1 = getpvar(PPVS,str,0);
		c = PVPATTERN((unsigned int)c1);
		PPVS->va[c1].attr = IS_PATTERN;
	} else if ( gdef ) {
		/* EPVS : global list of the current file */
		/* add to the local variable list */
		/* also add to the external variable list */
		c = getpvar(CPVS,str,0);
		getpvar(EPVS,str,0);
		if ( CUR_MODULE ) {
			c1 = getpvar(CUR_MODULE->pvs,str,1);
			if ( c1 >= 0 ) goto CONFLICTION;
		}
		if ( CPVS != GPVS ) {
			/* inside function : we add the name to the global list */
			getpvar(GPVS,str,0);
			CPVS->va[c].attr = IS_GLOBAL;
		}
	} else if ( mgdef ) {
		c = getpvar(CPVS,str,0);
		getpvar(CUR_MODULE->pvs,str,0);
		c1 = getpvar(EPVS,str,1);
		if ( c1 >= 0 ) goto CONFLICTION;
		if ( CPVS != GPVS ) {
			/* inside function */
			CPVS->va[c].attr = IS_MGLOBAL;
		}
	} else if ( ldef > 0 ) {
		/* if ldef > 0, then local variables are being declared */
		c = getpvar(CPVS,str,0);
		c1 = getpvar(EPVS,str,1);
		if ( c1 >= 0 ) {
			if ( CUR_MODULE )
				goto CONFLICTION;
			else {
				fprintf(stderr,"Warning: \"%s\", near line %d: conflicting declarations for `%s'\n",
					asir_infile->name,asir_infile->ln,str);
				fprintf(stderr,"         `%s' is bound to the global variable\n",str);
				CPVS->va[c].attr = IS_GLOBAL;
				c1 = getpvar(GPVS,str,1); c = PVGLOBAL((unsigned int)c1);
			}
		} else {
			if ( CUR_MODULE ) {
				c1 = getpvar(CUR_MODULE->pvs,str,1);
			}
			if ( c1 >= 0 ) goto CONFLICTION;
			CPVS->va[c].attr = IS_LOCAL;
		}
	} else if ( CPVS != GPVS ) {
		/* inside function */
		if ( CUR_MODULE ) {
			/* search only */
			c = getpvar(CPVS,str,1);
			if ( c < 0 ) {
				c = getpvar(CPVS,str,0);
				created = 1;		
			} else
				created = 0;
		} else {
			/* may be created */
			c = getpvar(CPVS,str,0);
		}
		switch ( CPVS->va[c].attr ) {
			case IS_GLOBAL:
				c1 = getpvar(GPVS,str,1); c = PVGLOBAL((unsigned int)c1);
				break;
			case IS_MGLOBAL:
				c1 = getpvar(CUR_MODULE->pvs,str,1); c = PVMGLOBAL((unsigned int)c1);
				break;
			case IS_LOCAL:
			default:
				if ( CUR_MODULE && 
					((c1 = getpvar(CUR_MODULE->pvs,str,1)) >= 0) ) {
					CPVS->va[c].attr = IS_MGLOBAL;
					c = PVMGLOBAL((unsigned int)c1);
				} else if ( getpvar(EPVS,str,1) >= 0 ) {
					CPVS->va[c].attr = IS_GLOBAL;
					c1 = getpvar(GPVS,str,1); c = PVGLOBAL((unsigned int)c1);
				} else if ( CUR_MODULE && created && (ldef == 0) ) {
					/* not declared */
					/* if ldef == 0, at least one local variables has been
					   declared */
					fprintf(stderr,
						"Warning: \"%s\", near line %d: undeclared local variable `%s'\n",
						asir_infile->name,asir_infile->ln,str);
				}
				break;
		}
	} else if ( CUR_MODULE ) {
		/* outside function, inside module */
		if ( (c = getpvar(CUR_MODULE->pvs,str,1)) >= 0 )
			c = PVMGLOBAL((unsigned int)c);
		else if ( getpvar(EPVS,str,1) >= 0 ) {
			c = getpvar(GPVS,str,1);
			c = PVGLOBAL((unsigned int)c);
		} else {
			/* not declared as static or extern */
			buf = ALLOCA(strlen("undeclared variable"+strlen(str)+10));
			sprintf(buf,"undeclared variable `%s'",str);
			yyerror(buf);
		}
	} else {
		/* outside function, outside module */
		c = getpvar(GPVS,str,0);
	}
	return c;

CONFLICTION:
	buf = ALLOCA(strlen("conflicting declarations for "+strlen(str)+10));
	sprintf(buf,"conflicting declarations for `%s'",str);
	yyerror(buf);
}

extern FUNC parse_targetf;

int searchpvar(char *str)
{
	VS pvs;
	MODULE mod;
	int c;

	if ( parse_targetf && parse_targetf->id != A_USR )
		c = -1;
	else if ( parse_targetf ) {
		pvs = parse_targetf->f.usrf->pvs;
		mod = parse_targetf->f.usrf->module;
		if ( (c = getpvar(pvs,str,1)) >= 0 ) {
			switch ( pvs->va[c].attr ) {
				case IS_GLOBAL:
					c = getpvar(GPVS,str,1);
					c = PVGLOBAL((unsigned int)c);
					break;
				case IS_MGLOBAL:
					c = getpvar(mod->pvs,str,1);
					c = PVMGLOBAL((unsigned int)c);
					break;
				default:
					break;
			}
		} else if ( mod && (c = getpvar(mod->pvs,str,1)) >= 0 )
			c = PVMGLOBAL((unsigned int)c);
		else if ( (c = getpvar(GPVS,str,1)) >= 0 )
			c = PVGLOBAL((unsigned int)c);
		else
			c = -1;
	}
	return c;
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
	v->attr= IS_LOCAL; v->type = -1; i = pvs->n; pvs->n++;
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
	if ( asir_infile->fp != stdin )
		PCLOSE(asir_infile->fp);
#endif
	asir_infile = NEXT(asir_infile);
}

void resetpvs()
{
	if ( asir_infile && !NEXT(asir_infile) ) {
		PVSS = 0; CPVS = GPVS; MPVS = 0; CUR_MODULE = 0; nextbp = 0;
		gdef = mgdef = ldef = 0;
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
