/* $OpenXM: OpenXM/src/asir99/parse/comp.h,v 1.1.1.1 1999/11/10 08:12:34 noro Exp $ */
struct oFIELD {
	char *name;
	int type;		/* type < 0 -> atomic; type >= 0 -> struct */
};

typedef struct oFIELD *FIELD;

struct comp_arf {
	FUNC add,sub,mul,div,pwr,chsgn,comp;
};

struct oSDEF {
	char *name;
	int n;
	struct comp_arf arf;
	struct oFIELD *f;
};

typedef struct oSDEF *SDEF;

struct oSS {
	int n;
	int asize;
	struct oSDEF *sa;
};

typedef struct oSS *SS;

extern struct oSS oLSS;
extern SS LSS;
