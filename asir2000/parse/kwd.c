/* $OpenXM: OpenXM/src/asir99/parse/kwd.c,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "y.tab.h"

struct oTKWD kwd[] = {
	{"for",FOR},
	{"while",WHILE},
	{"do",DO},
	{"if",IF},
	{"else",ELSE},
	{"break",BREAK},
	{"continue",CONTINUE},
	{"return",RETURN},
	{"def",FDEF},
	{"pfdef",PFDEF},
	{"global",GLOBAL},
	{"extern",GLOBAL},
	{"struct",STRUCT},
	{"newstruct",NEWSTRUCT},
	{"car",CAR},
	{"cdr",CDR},
	{"pari",PARIF},
	{"map",MAP},
	{"getopt",GETOPT},
	{"timer",TIMER},
	{0,0}
};
