/*
 * Copyright (c) 1994-2000 FUJITSU LABORATORIES LIMITED 
 * All rights reserved.
 * 
 * FUJITSU LABORATORIES LIMITED ("FLL") hereby grants you a limited,
 * non-exclusive and royalty-free license to use, copy, modify and
 * redistribute, solely for non-commercial and non-profit purposes, the
 * computer program, "Risa/Asir" ("SOFTWARE"), subject to the terms and
 * conditions of this Agreement. For the avoidance of doubt, you acquire
 * only a limited right to use the SOFTWARE hereunder, and FLL or any
 * third party developer retains all rights, including but not limited to
 * copyrights, in and to the SOFTWARE.
 * 
 * (1) FLL does not grant you a license in any way for commercial
 * purposes. You may use the SOFTWARE only for non-commercial and
 * non-profit purposes only, such as academic, research and internal
 * business use.
 * (2) The SOFTWARE is protected by the Copyright Law of Japan and
 * international copyright treaties. If you make copies of the SOFTWARE,
 * with or without modification, as permitted hereunder, you shall affix
 * to all such copies of the SOFTWARE the above copyright notice.
 * (3) An explicit reference to this SOFTWARE and its copyright owner
 * shall be made on your publication or presentation in any form of the
 * results obtained by use of the SOFTWARE.
 * (4) In the event that you modify the SOFTWARE, you shall notify FLL by
 * e-mail at risa-admin@sec.flab.fujitsu.co.jp of the detailed specification
 * for such modification or the source code of the modified part of the
 * SOFTWARE.
 * 
 * THE SOFTWARE IS PROVIDED AS IS WITHOUT ANY WARRANTY OF ANY KIND. FLL
 * MAKES ABSOLUTELY NO WARRANTIES, EXPRESSED, IMPLIED OR STATUTORY, AND
 * EXPRESSLY DISCLAIMS ANY IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT OF THIRD PARTIES'
 * RIGHTS. NO FLL DEALER, AGENT, EMPLOYEES IS AUTHORIZED TO MAKE ANY
 * MODIFICATIONS, EXTENSIONS, OR ADDITIONS TO THIS WARRANTY.
 * UNDER NO CIRCUMSTANCES AND UNDER NO LEGAL THEORY, TORT, CONTRACT,
 * OR OTHERWISE, SHALL FLL BE LIABLE TO YOU OR ANY OTHER PERSON FOR ANY
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, PUNITIVE OR CONSEQUENTIAL
 * DAMAGES OF ANY CHARACTER, INCLUDING, WITHOUT LIMITATION, DAMAGES
 * ARISING OUT OF OR RELATING TO THE SOFTWARE OR THIS AGREEMENT, DAMAGES
 * FOR LOSS OF GOODWILL, WORK STOPPAGE, OR LOSS OF DATA, OR FOR ANY
 * DAMAGES, EVEN IF FLL SHALL HAVE BEEN INFORMED OF THE POSSIBILITY OF
 * SUCH DAMAGES, OR FOR ANY CLAIM BY ANY OTHER PARTY. EVEN IF A PART
 * OF THE SOFTWARE HAS BEEN DEVELOPED BY A THIRD PARTY, THE THIRD PARTY
 * DEVELOPER SHALL HAVE NO LIABILITY IN CONNECTION WITH THE USE,
 * PERFORMANCE OR NON-PERFORMANCE OF THE SOFTWARE.
 *
 * $OpenXM: OpenXM_contrib2/asir2000/engine/init.c,v 1.25 2006/02/13 11:41:11 noro Exp $ 
*/
#include "ca.h"
#include "version.h"
#include "prime.h"

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
int StrassenSize = 0;
int outputstyle = 0;

static int *lprime;
static int lprime_size;

#if defined(PARI)
int paristack = 1<<16;
#define ADDBF addbf
#define SUBBF subbf
#define MULBF mulbf
#define DIVBF divbf
#define PWRBF pwrbf
#define CHSGNBF chsgnbf
#define CMPBF cmpbf
# else
#define ADDBF 0
#define SUBBF 0
#define MULBF 0
#define DIVBF 0
#define PWRBF 0
#define CHSGNBF 0
#define CMPBF 0
#endif

#if defined(INTERVAL)
int zerorewrite = 0;
void (*addnumt[])() = { addq, addreal, addalg, ADDBF, 0, additvp, additvd, 0, additvf, addcplx, addmi, addlm, addgf2n, addgfpn, addgfs, addgfsn, adddalg };
void (*subnumt[])() = { subq, subreal, subalg, SUBBF, 0, subitvp, subitvd, 0, subitvf, subcplx, submi, sublm, subgf2n, subgfpn, subgfs, subgfsn, subdalg };
void (*mulnumt[])() = { mulq, mulreal, mulalg, MULBF, 0, mulitvp, mulitvd, 0, mulitvf, mulcplx, mulmi, mullm, mulgf2n, mulgfpn, mulgfs, mulgfsn, muldalg };
void (*divnumt[])() = { divq, divreal, divalg, DIVBF, 0, divitvp, divitvd, 0, divitvf, divcplx, divmi, divlm, divgf2n, divgfpn, divgfs, divgfsn, divdalg };
void (*pwrnumt[])() = { pwrq, pwrreal, pwralg, PWRBF, 0, pwritvp, pwritvd, 0, pwritvf, pwrcplx, pwrmi, pwrlm, pwrgf2n, pwrgfpn, pwrgfs, pwrgfsn, pwrdalg };
void (*chsgnnumt[])() = { chsgnq, chsgnreal, chsgnalg, CHSGNBF, 0, chsgnitvp, chsgnitvd, 0, chsgnitvf, chsgncplx, chsgnmi, chsgnlm, chsgngf2n, chsgngfpn, chsgngfs , chsgngfsn, chsgndalg};
int (*cmpnumt[])() = { cmpq, cmpreal, cmpalg, CMPBF, 0, cmpitvp, cmpitvd, 0, cmpitvf, cmpcplx, cmpmi, cmplm, cmpgf2n, cmpgfpn, cmpgfs, cmpgfsn, cmpdalg };
#else
void (*addnumt[])() = { addq, addreal, addalg, ADDBF, addcplx, addmi, addlm, addgf2n, addgfpn, addgfs, addgfsn, adddalg };
void (*subnumt[])() = { subq, subreal, subalg, SUBBF, subcplx, submi, sublm, subgf2n, subgfpn, subgfs, subgfsn, subdalg };
void (*mulnumt[])() = { mulq, mulreal, mulalg, MULBF, mulcplx, mulmi, mullm, mulgf2n, mulgfpn, mulgfs, mulgfsn, muldalg };
void (*divnumt[])() = { divq, divreal, divalg, DIVBF, divcplx, divmi, divlm, divgf2n, divgfpn, divgfs, divgfsn, divdalg };
void (*pwrnumt[])() = { pwrq, pwrreal, pwralg, PWRBF, pwrcplx, pwrmi, pwrlm, pwrgf2n, pwrgfpn, pwrgfs, pwrgfsn, pwrdalg };
void (*chsgnnumt[])() = { chsgnq, chsgnreal, chsgnalg, CHSGNBF, chsgncplx, chsgnmi, chsgnlm, chsgngf2n, chsgngfpn, chsgngfs, chsgngfsn, chsgndalg };
int (*cmpnumt[])() = { cmpq, cmpreal, cmpalg, CMPBF, cmpcplx, cmpmi, cmplm, cmpgf2n, cmpgfpn, cmpgfs, cmpgfsn, cmpdalg };
#endif

double get_current_time();
void init_lprime();

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
#if defined(PARI)
	risa_pari_init();
#endif
	srandom((int)get_current_time());
#endif
	init_up2_tab();

	init_lprime();
}

extern double gctime;
double suspend_start;
double suspended_time=0;

void get_eg(struct oEGT *p)
{
	p->exectime = get_clock() - gctime - suspended_time; p->gctime = gctime;
}

void init_eg(struct oEGT *eg)
{
	bzero((char *)eg,sizeof(struct oEGT));
}

void add_eg(struct oEGT *base,struct oEGT *start,struct oEGT *end)
{
	base->exectime += end->exectime - start->exectime;
	base->gctime += end->gctime - start->gctime;
}

void print_eg(char *item,struct oEGT *eg)
{
	printf("%s=(%.4g,%.4g)",item,eg->exectime,eg->gctime);
}

void print_split_eg(struct oEGT *start,struct oEGT *end)
{
	struct oEGT base;

	init_eg(&base); add_eg(&base,start,end);
	printf("(%.4g,%.4g)",base.exectime,base.gctime);
}

void print_split_e(struct oEGT *start,struct oEGT *end)
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
extern int GC_dont_gc;
extern int do_weyl;
extern int dp_fcoeffs;

void reset_engine() {
	lm_lazy = 0;
	up_lazy = 0;
	do_weyl = 0;
	dp_fcoeffs = 0;
	GC_dont_gc = 0;
}

unsigned int get_asir_version() {
	return ASIR_VERSION;
}

char *get_asir_distribution() {
	return ASIR_DISTRIBUTION;
}


void create_error(ERR *err,unsigned int serial,char *msg,LIST trace)
{
	int len;
	USINT ui,notsupp;
	NODE n,n1;
	LIST list;
	char *msg1;
	STRING errmsg;

	MKUSINT(ui,serial);
	MKUSINT(notsupp,-1);
	len = strlen(msg);
	msg1 = (char *)MALLOC(len+1);
	strcpy(msg1,msg);
	MKSTR(errmsg,msg1);
	if ( !trace )
		MKLIST(trace,0);
	n = mknode(4,ui,notsupp,errmsg,trace); MKLIST(list,n);
	MKERR(*err,list);
}

void init_lprime()
{
	int s,i;

	s = sizeof(lprime_init);
	lprime = (int *)GC_malloc_atomic(s);
	lprime_size = s/sizeof(int);	
	for ( i = 0; i < lprime_size; i++ )
		lprime[i] = lprime_init[lprime_size-i-1];
}

void create_new_lprimes(int);

int get_lprime(index)
{
	if ( index >= lprime_size )
		create_new_lprimes(index);
	return lprime[index];
}

void create_new_lprimes(int index)
{
	int count,p,i,j,d;

	if ( index < lprime_size )
		return;
	count = index-lprime_size+1;
	if ( count < 256 )
		count = 256;
	lprime = (int *)GC_realloc(lprime,(lprime_size+count)*sizeof(int));
	p = lprime[lprime_size-1]+2;
	for ( i = 0; i < count; p += 2 ) {
		for ( j = 0; d = sprime[j]; j++ ) {
			if ( d*d > p ) {
				lprime[i+lprime_size] = p;
				i++;
				break;
			}
			if ( !(p%d) )
				break;
		}
	}
	lprime_size += count;
}
