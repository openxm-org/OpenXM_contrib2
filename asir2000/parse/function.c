/* $OpenXM: OpenXM/src/asir99/parse/function.c,v 1.2 1999/11/18 05:42:03 noro Exp $ */
#include "ca.h"
#include "parse.h"

void appendbinf(flistp,name,func,argc)
NODE *flistp;
char *name;
void (*func)();
int argc;
{
	FUNC t;
	NODE n;

	t = (FUNC)MALLOC(sizeof(struct oFUNC)); 
	t->name = name; t->id = A_BIN; t->argc = argc; t->f.binf = func;
	MKNODE(n,t,*flistp); *flistp = n;
}

void appendparif(flistp,name,func,type)
NODE *flistp;
char *name;
int (*func)();
int type;
{
	FUNC t;
	NODE n;

	t = (FUNC)MALLOC(sizeof(struct oFUNC)); 
	t->name = name; t->id = A_PARI; t->type = type; t->f.binf = (void (*)())func;
	MKNODE(n,t,*flistp); *flistp = n;
}

void appendsysf(name,func,argc)
char *name;
void (*func)();
int argc;
{
	appendbinf(&sysf,name,func,argc);
}

void appendubinf(name,func,argc)
char *name;
void (*func)();
int argc;
{
	appendbinf(&ubinf,name,func,argc);
}

void dcptolist(dc,listp)
DCP dc;
LIST *listp;
{
	NODE node,tnode,ln0,ln1;
	LIST l;

	for ( node = 0; dc; dc = NEXT(dc) ) {
		NEXTNODE(node,tnode);
		MKNODE(ln1,DEG(dc),0); MKNODE(ln0,COEF(dc),ln1);
		MKLIST(l,ln0); BDY(tnode) = (pointer)l;
	}
	NEXT(tnode) = 0; MKLIST(l,node); *listp = l;
}
