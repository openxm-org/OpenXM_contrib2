/* $OpenXM: OpenXM/src/asir99/builtin/compobj.c,v 1.1.1.1 1999/11/10 08:12:25 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "comp.h"

void Parfreg();

struct ftab comp_tab[] = {
	{"arfreg",Parfreg,8},
	{0,0,0},
};

void Parfreg(arg,rp)
NODE arg;
Q *rp;
{
	char *name;
	P t;
	SDEF s;
	int i;

	asir_assert(ARG0(arg),O_STR,"arfreg");
	name = ((STRING)ARG0(arg))->body;
	for ( s = LSS->sa, i = 0; i < LSS->n; i++ )
		if ( !strcmp(s[i].name,name) )
			break;
	if ( i == LSS->n )
		error("argreg : no such structure");
		
	t = (P)ARG1(arg); s[i].arf.add = !t ? 0 : (FUNC)VR(t)->priv;
	t = (P)ARG2(arg); s[i].arf.sub = !t ? 0 : (FUNC)VR(t)->priv;
	t = (P)ARG3(arg); s[i].arf.mul = !t ? 0 : (FUNC)VR(t)->priv;
	t = (P)ARG4(arg); s[i].arf.div = !t ? 0 : (FUNC)VR(t)->priv;
	t = (P)ARG5(arg); s[i].arf.pwr = !t ? 0 : (FUNC)VR(t)->priv;
	t = (P)ARG6(arg); s[i].arf.chsgn = !t ? 0 : (FUNC)VR(t)->priv;
	t = (P)ARG7(arg); s[i].arf.comp = !t ? 0 : (FUNC)VR(t)->priv;
	*rp = ONE;
}
