/* $OpenXM: OpenXM/src/asir99/engine/init.c,v 1.1.1.1 1999/11/10 08:12:26 noro Exp $ */
#include "ca.h"
#include "version.h"

struct oN oUNIN,oTWON,oTHREEN,oFOURN,oFIVEN,oSIXN,oSEVENN,oEIGHTN;
struct oQ oUNIQ,oTWOQ,oTHREEQ,oFOURQ,oFIVEQ,oSIXQ,oSEVENQ,oEIGHTQ;

struct oUP2 oONEUP2;

N ONEN = &oUNIN;
Q ONE = &oUNIQ;
Q TWO = &oTWOQ;
Q THREE = &oTHREEQ;
Q FOUR = &oFOURQ;
Q FIVE = &oFIVEQ;
Q SIX = &oSIXQ;
Q SEVEN = &oSEVENQ;
Q EIGHT = &oEIGHTQ;

LM THREE_LM,FOUR_LM,EIGHT_LM;

struct oR oUNIR;
struct oMQ oUNIMQ;
struct oLM oUNILM;
struct oC oIU;
struct oUSINT oVOID;

MQ ONEM = &oUNIMQ;
LM ONELM = &oUNILM;
UP2 ONEUP2 = &oONEUP2;
C IU = &oIU;
USINT VOIDobj = &oVOID;

int bigfloat;
int nez = 1;
int current_mod = 0;

#if PARI
#if defined(THINK_C)
int paristack = 1<<16;
#else
int paristack = 1<<20;
#endif

void (*addnumt[])() = { addq, addreal, addalg, addbf, addcplx, addmi, addlm, addgf2n, addgfpn };
void (*subnumt[])() = { subq, subreal, subalg, subbf, subcplx, submi, sublm, subgf2n, subgfpn };
void (*mulnumt[])() = { mulq, mulreal, mulalg, mulbf, mulcplx, mulmi, mullm, mulgf2n, mulgfpn };
void (*divnumt[])() = { divq, divreal, divalg, divbf, divcplx, divmi, divlm, divgf2n, divgfpn };
void (*pwrnumt[])() = { pwrq, pwrreal, pwralg, pwrbf, pwrcplx, pwrmi, pwrlm, pwrgf2n, pwrgfpn };
void (*chsgnnumt[])() = { chsgnq, chsgnreal, chsgnalg, chsgnbf, chsgncplx, chsgnmi, chsgnlm, chsgngf2n, chsgngfpn };
int (*cmpnumt[])() = { cmpq, cmpreal, cmpalg, cmpbf, cmpcplx, cmpmi, cmplm, cmpgf2n, cmpgfpn };
#else
void (*addnumt[])() = { addq, addreal, addalg, 0, addcplx, addmi, addlm, addgf2n, addgfpn };
void (*subnumt[])() = { subq, subreal, subalg, 0, subcplx, submi, sublm, subgf2n, subgfpn };
void (*mulnumt[])() = { mulq, mulreal, mulalg, 0, mulcplx, mulmi, mullm, mulgf2n, mulgfpn };
void (*divnumt[])() = { divq, divreal, divalg, 0, divcplx, divmi, divlm, divgf2n,divgfpn };
void (*pwrnumt[])() = { pwrq, pwrreal, pwralg, 0, pwrcplx, pwrmi, pwrlm, pwrgf2n, pwrgfpn };
void (*chsgnnumt[])() = { chsgnq, chsgnreal, chsgnalg, 0, chsgncplx, chsgnmi, chsgnlm, chsgngf2n, chsgngfpn };
int (*cmpnumt[])() = { cmpq, cmpreal, cmpalg, 0, cmpcplx, cmpmi, cmplm, cmpgf2n, cmpgfpn };
#endif

double get_current_time();

void nglob_init() {
	PL(&oUNIN) = 1; BD(&oUNIN)[0] = 1;
	PL(&oTWON) = 1; BD(&oTWON)[0] = 2;
	PL(&oTHREEN) = 1; BD(&oTHREEN)[0] = 3;
	PL(&oFOURN) = 1; BD(&oFOURN)[0] = 4;
	PL(&oFIVEN) = 1; BD(&oFIVEN)[0] = 5;
	PL(&oSIXN) = 1; BD(&oSIXN)[0] = 6;
	PL(&oSEVENN) = 1; BD(&oSEVENN)[0] = 7;
	PL(&oEIGHTN) = 1; BD(&oEIGHTN)[0] = 8;

	oONEUP2.w = 1; oONEUP2.b[0] = 1;

	OID(&oUNIQ) = O_N; NID(&oUNIQ) = N_Q;
	SGN(&oUNIQ) = 1; NM(&oUNIQ) = &oUNIN; DN(&oUNIQ) = 0;

	OID(&oTWOQ) = O_N; NID(&oTWOQ) = N_Q;
	SGN(&oTWOQ) = 1; NM(&oTWOQ) = &oTWON; DN(&oTWOQ) = 0;

	OID(&oTHREEQ) = O_N; NID(&oTHREEQ) = N_Q;
	SGN(&oTHREEQ) = 1; NM(&oTHREEQ) = &oTHREEN; DN(&oTHREEQ) = 0;

	OID(&oFOURQ) = O_N; NID(&oFOURQ) = N_Q;
	SGN(&oFOURQ) = 1; NM(&oFOURQ) = &oFOURN; DN(&oFOURQ) = 0;

	OID(&oFIVEQ) = O_N; NID(&oFIVEQ) = N_Q;
	SGN(&oFIVEQ) = 1; NM(&oFIVEQ) = &oFIVEN; DN(&oFIVEQ) = 0;

	OID(&oSIXQ) = O_N; NID(&oSIXQ) = N_Q;
	SGN(&oSIXQ) = 1; NM(&oSIXQ) = &oSIXN; DN(&oSIXQ) = 0;

	OID(&oSEVENQ) = O_N; NID(&oSEVENQ) = N_Q;
	SGN(&oSEVENQ) = 1; NM(&oSEVENQ) = &oSEVENN; DN(&oSEVENQ) = 0;

	OID(&oEIGHTQ) = O_N; NID(&oEIGHTQ) = N_Q;
	SGN(&oEIGHTQ) = 1; NM(&oEIGHTQ) = &oEIGHTN; DN(&oEIGHTQ) = 0;

	OID(&oUNIR) = O_R; NM(&oUNIR) = (P)&oUNIQ; DN(&oUNIR) = (P)&oUNIQ; oUNIR.reduced = 1;
	OID(&oUNIMQ) = O_N; NID(&oUNIMQ) = N_M; CONT(&oUNIMQ) = 1;
	OID(&oUNILM) = O_N; NID(&oUNILM) = N_LM; BDY(&oUNILM) = ONEN;
	OID(&oIU) = O_N; NID(&oIU) = N_C; oIU.r = 0; oIU.i = (Num)ONE;

	MKLM(&oTHREEN,THREE_LM);
	MKLM(&oFOURN,FOUR_LM);
	MKLM(&oEIGHTN,EIGHT_LM);

	OID(&oVOID) = O_VOID;

/* moved to parse/main.c */
#if 0
#if PARI
	risa_pari_init();
#endif
	srandom((int)get_current_time());
#endif
	init_up2_tab();
}

extern double gctime;
double suspend_start;
double suspended_time=0;

void get_eg(p)
struct oEGT *p;
{
	p->exectime = get_clock() - gctime - suspended_time; p->gctime = gctime;
}

void init_eg(eg)
struct oEGT *eg;
{
	bzero((char *)eg,sizeof(struct oEGT));
}

void add_eg(base,start,end)
struct oEGT *base,*start,*end;
{
	base->exectime += end->exectime - start->exectime;
	base->gctime += end->gctime - start->gctime;
}

void print_eg(item,eg)
char *item;
struct oEGT *eg;
{
	printf("%s=(%.4g,%.4g)",item,eg->exectime,eg->gctime);
}

void print_split_eg(start,end)
struct oEGT *start,*end;
{
	struct oEGT base;

	init_eg(&base); add_eg(&base,start,end);
	printf("(%.4g,%.4g)",base.exectime,base.gctime);
}

void print_split_e(start,end)
struct oEGT *start,*end;
{
	struct oEGT base;

	init_eg(&base); add_eg(&base,start,end);
	printf("(%.4g)",base.exectime);
}

void suspend_timer() {
	suspend_start = get_clock();
}

void resume_timer() {
	suspended_time += get_clock()-suspend_start;
}

extern int lm_lazy, up_lazy;
extern GC_dont_gc;

void reset_engine() {
	lm_lazy = 0; up_lazy = 0;
	GC_dont_gc = 0;
}

unsigned int get_asir_version() {
	return ASIR_VERSION;
}
