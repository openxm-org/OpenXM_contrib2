/* $OpenXM: OpenXM/src/asir99/builtin/time.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "parse.h"

void Ptime(), Pcputime(), Pcurrenttime(), Ptstart(), Ptstop();

struct ftab time_tab[] = {
	{"time",Ptime,0},
	{"cputime",Pcputime,1},
	{"currenttime",Pcurrenttime,0},
	{"tstart",Ptstart,0},
	{"tstop",Ptstop,0},
	{0,0,0},
};

static struct oEGT egt0;
static double r0;
double get_rtime(), get_current_time();

void Pcurrenttime(rp)
Q *rp;
{
	int t;

	t = (int)get_current_time(); STOQ(t,*rp);
}

void Ptstart(rp)
pointer *rp;
{
	get_eg(&egt0); r0 = get_rtime(); *rp = 0;
}

void Ptstop(rp)
pointer *rp;
{
	struct oEGT egt1;

	get_eg(&egt1); printtime(&egt0,&egt1,get_rtime()-r0); *rp = 0;
}

int prtime;

void Pcputime(arg,rp)
NODE arg;
pointer *rp;
{
	prtime = ARG0(arg) ? 1 : 0; *rp = 0;
}

void Ptime(listp)
LIST *listp;
{
	struct oEGT eg;
	Real re,rg,rr;
	NODE a,b,w,r;
	Q words;
	int t;
	double rtime;
	double get_rtime();

	rtime = get_rtime(); MKReal(rtime,rr);
	t = get_allocwords(); STOQ(t,words);
	get_eg(&eg); MKReal(eg.exectime,re); MKReal(eg.gctime,rg);
	MKNODE(r,rr,0); MKNODE(w,words,r); MKNODE(a,rg,w); MKNODE(b,re,a);
	MKLIST(*listp,b);
}

void printtime(egt0,egt1,r)
struct oEGT *egt0,*egt1;
double r;
{
	double e,g;

	e = egt1->exectime - egt0->exectime;
	if ( e < 0 ) e = 0;
	g = egt1->gctime - egt0->gctime;
	if ( g < 0 ) g = 0;
	if ( g )
		fprintf(stderr,"%.4gsec + gc : %.4gsec(%.4gsec)\n",e,g,r);
	else
		fprintf(stderr,"%.4gsec(%.4gsec)\n",e,r);
}
