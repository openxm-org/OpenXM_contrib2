/* $OpenXM: OpenXM/src/asir99/builtin/strobj.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "ctype.h"
#if PARI
#include "genpari.h"
extern jmp_buf environnement;
#endif
extern char *parse_strp;

void Prtostr(), Pstrtov(), Peval_str();

struct ftab str_tab[] = {
	{"rtostr",Prtostr,1},
	{"strtov",Pstrtov,1},
	{"eval_str",Peval_str,1},
	{0,0,0},
};

void Peval_str(arg,rp)
NODE arg;
Obj *rp;
{
	FNODE fnode;
	char *cmd;
#if PARI
	recover(0);
	if ( setjmp(environnement) ) {
		avma = top; recover(1);
		resetenv("");
	}
#endif
	cmd = BDY((STRING)ARG0(arg));
	exprparse(0,cmd,&fnode);
	*rp = eval(fnode);
}

void Prtostr(arg,rp)
NODE arg;
STRING *rp;
{
	char *b;
	int len;

	len = countobj(ARG0(arg));
	b = (char *)MALLOC(len+1);
	soutput_init(b);
	sprintexpr(CO,ARG0(arg));
	MKSTR(*rp,b);
}

void Pstrtov(arg,rp)
NODE arg;
P *rp;
{
	char *p,*t;

	p = BDY((STRING)ARG0(arg));
#if 0
	if ( !islower(*p) )
		*rp = 0;
	else {
		for ( t = p+1; t && (isalnum(*t) || *t == '_'); t++ );
		if ( *t )
			*rp = 0;
		else
			makevar(p,rp);
	}
#else
	makevar(p,rp);
#endif
}
