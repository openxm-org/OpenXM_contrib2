/* $OpenXM: OpenXM/src/asir99/include/bf.h,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
struct oBf {
	short id;
	char nid;
	char sgn:4,esgn:4;
	N e,m;
};

typedef struct oBf *Bf;

#define NEWBf(b) ((b)=(Bf)MALLOC(sizeof(struct oBf)))
#define MKBf(exp,esgn,sgn,man,b)\
(NEWBf(b),(b)->e=(exp),(b)->esgn=(esgn),(b)->sgn=(sgn),(b)->m=(man))
