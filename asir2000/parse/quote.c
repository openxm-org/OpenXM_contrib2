#include "ca.h"
#include "parse.h"

void addquote(vl,a,b,c)
VL vl;
QUOTE a,b;
QUOTE *c;
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,addfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void subquote(vl,a,b,c)
VL vl;
QUOTE a,b;
QUOTE *c;
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,subfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void mulquote(vl,a,b,c)
VL vl;
QUOTE a,b;
QUOTE *c;
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,mulfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void divquote(vl,a,b,c)
VL vl;
QUOTE a,b;
QUOTE *c;
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,divfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void pwrquote(vl,a,b,c)
VL vl;
QUOTE a,b;
QUOTE *c;
{
	FNODE fn;

	if ( !b || OID(b) != O_QUOTE )
		error("pwrquote : invalid argument");
	fn = mkfnode(3,I_BOP,pwrfs,BDY(a),BDY(b));
	MKQUOTE(*c,fn);
}

void chsgnquote(a,c)
QUOTE a;
QUOTE *c;
{
	FNODE fn;

	fn = mkfnode(3,I_BOP,subfs,0,BDY(a));
	MKQUOTE(*c,fn);
}
