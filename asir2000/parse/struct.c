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
 * e-mail at risa-admin@flab.fujitsu.co.jp of the detailed specification
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
 * $OpenXM: OpenXM_contrib2/asir2000/parse/struct.c,v 1.1.1.1 1999/12/03 07:39:12 noro Exp $ 
*/
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
