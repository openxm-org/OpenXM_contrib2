/* $OpenXM: OpenXM/src/asir99/parse/evall.c,v 1.2 1999/11/18 05:42:03 noro Exp $ */
#include <ctype.h>
#include "ca.h"
#include "parse.h"
#include "base.h"
#include "al.h"
#if !defined(THINK_C)
#include <sys/types.h>
#include <sys/stat.h>
#endif

pointer evall();

pointer evall(id,a1,a2)
lid id;
Obj a1,a2;
{
	F f;
	Obj d;
	oFOP op;
	NODE n1,n2;

	switch ( id ) {
		case L_EQ:
			op = AL_EQUAL; break;
		case L_NE:
			op = AL_NEQ; break;
		case L_GT:
			op = AL_GREATERP; break;
		case L_LT:
			op = AL_LESSP; break;
		case L_GE:
			op = AL_GEQ; break;
		case L_LE:
			op = AL_LEQ; break;
		case L_AND:
			op = AL_AND; break;
		case L_OR:
			op = AL_OR; break;
		case L_NOT:
			op = AL_NOT; break;
		case L_IMPL:
			op = AL_IMPL; break;
		case L_REPL:
			op = AL_REPL; break;
		case L_EQUIV:
			op = AL_EQUIV; break;
		default:
			error("evall : unknown id");
	}
	if ( AL_ATOMIC(op) ) {
		arf_sub(CO,a1,a2,&d);
		MKAF(f,op,(P)d);
	} else if ( AL_JUNCT(op) ) {
		if ( FOP((F)a1) == op ) {
			if ( FOP((F)a2) == op )
				n2 = FJARG((F)a2);
			else
				MKNODE(n2,(F)a2,0);
			node_concat_dup(FJARG((F)a1),n2,&n1);
		} else if ( FOP((F)a2) == op )
			MKNODE(n1,a1,FJARG((F)a2));
		else {
			MKNODE(n2,a2,0); MKNODE(n1,a1,n2);
		}
		MKJF(f,op,n1);
	} else if ( AL_UNI(op) )
		MKUF(f,op,(F)a1);
	else if ( AL_EXT(op) )
		MKBF(f,op,(F)a1,(F)a2);
	return (pointer)f;
}

node_concat_dup(n1,n2,nr)
NODE n1,n2,*nr;
{
	NODE r0,r,n;

	for ( r0 = 0, n = n1; n; n = NEXT(n) ) {
		NEXTNODE(r0,r); BDY(r) = BDY(n);
	}
	if ( !r0 )
		*nr = n2;
	else {
		NEXT(r) = n2; *nr = r0;
	}
}
