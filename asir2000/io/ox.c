/* $OpenXM: OpenXM/src/asir99/io/ox.c,v 1.7 1999/11/19 04:15:49 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "wsio.h"
#include "ox.h"

#define ISIZ sizeof(int)

void ox_flush_stream(),ox_write_int(),ox_write_cmo();
void ox_read_int(),ox_read_cmo();
void mclist_to_mc();
void ox_read_local();

extern Obj VOIDobj;

extern int ox_need_conv;
extern int ox_usr1_sent, ox_int_received, critical_when_signal;
unsigned int ox_serial;
int ox_flushing;
int ox_batch;
int ox_check=1;
jmp_buf ox_env;

MATHCAP my_mathcap;

struct oxcap {
	unsigned int ox;
	int ncap;
	int *cap;
};

struct mathcap {
	LIST mc;
	unsigned int version;
	char *servername;
	int nsmcap;
	unsigned int *smcap;
	int noxcap;
	struct oxcap *oxcap;
};

struct oxcap *my_oxcap;

static struct mathcap my_mc;
static struct mathcap *remote_mc;
static int remote_mc_len;

void ox_resetenv(s)
char *s;
{
	fprintf(stderr,"%s\n",s);
	longjmp(ox_env,1);
}

static int available_cmo[] = {
	CMO_NULL, CMO_INT32, CMO_DATUM, CMO_STRING, CMO_MATHCAP,
	CMO_ERROR, CMO_ERROR2, CMO_LIST, CMO_MONOMIAL32,
	CMO_ZZ, CMO_QQ, CMO_ZERO,
	CMO_DMS_GENERIC, CMO_DMS_OF_N_VARIABLES,
	CMO_RING_BY_NAME, CMO_DISTRIBUTED_POLYNOMIAL,
	CMO_RECURSIVE_POLYNOMIAL, CMO_UNIVARIATE_POLYNOMIAL,
	CMO_INDETERMINATE,
	0
};

static int available_sm[] = {
	SM_dupErrors, SM_getsp, SM_popSerializedLocalObject,
	SM_popCMO, SM_popString, SM_setName,
	SM_evalName, SM_executeStringByLocalParser,
	SM_executeStringByLocalParserInBatchMode,
	SM_executeFunction, SM_shutdown, SM_pops,
	SM_mathcap, SM_setMathcap, SM_nop,
	SM_beginBlock, SM_endBlock,
	0
};

/*
	mathcap = 
		[
			version list,
			SMlist,
			[
				[OX tag,CMO tag list],
				[OX tag,CMO tag list],
				...
			]
		]
*/

void create_my_mathcap(char *system)
{
	NODE n,n0;
	int i,k;
	STRING str;
	LIST sname,smlist,oxlist,cmolist,asirlist,oxtag,oxasir,r;
	USINT tag,t,t1;

	if ( my_mathcap )
		return;
	/* version */
	MKSTR(str,system);
	MKUSINT(t,OX_VERSION);
	n0 = mknode(2,t,str); MKLIST(sname,n0);

	/* cmo tag */
	for ( n0 = 0, i = 0; k = available_sm[i]; i++ ) {
		NEXTNODE(n0,n); MKUSINT(t,k); BDY(n) = (pointer)t;
	}
	NEXT(n) = 0; MKLIST(smlist,n0);

	/* creation of [OX_DATA,CMO list] */
	/* ox tag */
	MKUSINT(tag,OX_DATA);
	/* cmo tag */
	for ( n0 = 0, i = 0; k = available_cmo[i]; i++ ) {
		NEXTNODE(n0,n); MKUSINT(t,k); BDY(n) = (pointer)t;
	}
	NEXT(n) = 0; MKLIST(cmolist,n0);
	/* [ox tag, cmo list] */
	n0 = mknode(2,tag,cmolist);
	MKLIST(oxlist,n0);

	/* creation of [OX_LOCAL_OBJECT_ASIR,ASIR tag] */
	/* ox tag */
	MKUSINT(tag,OX_LOCAL_OBJECT_ASIR);
	/* local tag */
	MKUSINT(t,ASIR_VL);
	MKUSINT(t1,ASIR_OBJ);
	n0 = mknode(2,t,t1); MKLIST(cmolist,n0);
	/* [ox tag, local list] */
	n0 = mknode(2,tag,cmolist);
	 MKLIST(asirlist,n0);

	/* [oxlist,asirlist] */
	n0 = mknode(2,oxlist,asirlist); MKLIST(oxasir,n0);

	/* [version,sm,oxasir] */
	n0 = mknode(3,sname,smlist,oxasir); MKLIST(r,n0);

	MKMATHCAP(my_mathcap,r);
	mclist_to_mc(r,&my_mc);
	my_oxcap = my_mc.oxcap;
}

void store_remote_mathcap(int s,MATHCAP mc)
{
	if ( !remote_mc ) {
		 remote_mc_len = 16;
		 remote_mc = (struct mathcap *)
		 	CALLOC(remote_mc_len,sizeof(struct mathcap));
	}
	if ( s >= remote_mc_len ) {
		remote_mc_len *= 2;
		remote_mc = (struct mathcap *)REALLOC(remote_mc,
			remote_mc_len*sizeof(struct mathcap));
	}
	mclist_to_mc(BDY(mc),&remote_mc[s]);
}

/*
	mathcap = 
		[
			version list,
			SMlist,
			[
				[OX tag,CMO tag list],
				[OX tag,CMO tag list],
				...
			]
		]

    ===>

	mathcap
		| version | &servername | nsmcap | 	&smcap | noxcap | &oxcap |
	smcap
		| SM_xxx | SM_yyy | ... |
	oxcap
		| oxcap[0] | oxcap[1] | ... |
	oxcap[i]
		| ox | ncap | &cap |
	cap
		| CMO_xxx | CMO_yyy | ... |
*/

void mclist_to_mc(LIST mclist,struct mathcap *mc)
{
	int id,l,i,j;
	NODE n,t,oxcmo,ox,cap;
	int *ptr;

	/*
		[ 
			[ version,servername ]
			[sm1,sm2,...],
			[
				[o1,[n11,n12,...]],
				[o2,[n21,n22,...]],
				...
			]
	   	]
	*/
	n = BDY(mclist);
	mc->mc = mclist;
	mc->version = BDY((USINT)BDY(BDY((LIST)BDY(n))));
	mc->servername = BDY((STRING)BDY(NEXT(BDY((LIST)BDY(n)))));

	/* smcap */
	n = NEXT(n);
	t = BDY((LIST)BDY(n));
	mc->nsmcap = length(t);
	mc->smcap = (int *)MALLOC_ATOMIC(mc->nsmcap*sizeof(int));
	for ( j = 0, ptr = mc->smcap; j < mc->nsmcap; j++, t = NEXT(t) )
		ptr[j] = BDY((USINT)BDY(t));

	n = NEXT(n);
	n = BDY((LIST)BDY(n));
	/* n -> BDY([[OX1,CMOlist1], [OX2,CMOlist2], ...]) */
	mc->noxcap = length(n);
	mc->oxcap = (struct oxcap *)MALLOC(mc->noxcap*sizeof(struct oxcap));
	for ( j = 0; j < mc->noxcap; j++, n = NEXT(n) ) {
		oxcmo = BDY((LIST)BDY(n));
		/* oxcmo = BDY([OXj,CMOlistj]) */
		mc->oxcap[j].ox = BDY((USINT)BDY(oxcmo));
		cap = BDY((LIST)BDY(NEXT(oxcmo)));
		/* cap ->BDY(CMOlistj) */
		l = length(cap);
		mc->oxcap[j].ncap = l;
		mc->oxcap[j].cap = (unsigned int *)CALLOC(l+1,sizeof(unsigned int));
		for ( t = cap, ptr = mc->oxcap[j].cap, i = 0; i < l; t = NEXT(t), i++ )
			ptr[i] = BDY((USINT)BDY(t));
	}
}

int check_sm_by_mc(s,smtag)
int s;
unsigned int smtag;
{
	struct mathcap *rmc;
	int nsmcap,i;
	unsigned int *smcap;

	/* XXX : return 1 if remote_mc is not available. */
	if ( !remote_mc )
		return 1;
	rmc = &remote_mc[s];
	nsmcap = rmc->nsmcap;
	smcap = rmc->smcap;
	if ( !smcap )
		return 1;
	for ( i = 0; i < nsmcap; i++ )
		if ( smcap[i] == smtag )
			break;
	if ( i == nsmcap )
		return 0;
	else
		return 1;
}

int check_by_mc(s,oxtag,cmotag)
int s;
unsigned int oxtag,cmotag;
{
	struct mathcap *rmc;
	int noxcap,ncap,i,j;
	struct oxcap *oxcap;
	unsigned int *cap;

	/* XXX : return 1 if remote_mc is not available. */
	if ( !remote_mc )
		return 1;
	rmc = &remote_mc[s];
	noxcap = rmc->noxcap;
	oxcap = rmc->oxcap;
	if ( !oxcap )
		return 1;
	for ( i = 0; i < noxcap; i++ )
		if ( oxcap[i].ox == oxtag )
			break;
	if ( i == noxcap )
		return 0;
	ncap = oxcap[i].ncap;
	cap = oxcap[i].cap;
	for ( j = 0; j < ncap; j++ )
		if ( cap[j] == cmotag )
			break;
	if ( j == ncap )
		return 0;
	else
		return 1;
}

void begin_critical() {
	critical_when_signal = 1;
}

void end_critical() {
	critical_when_signal = 0;
	if ( ox_usr1_sent ) {
		ox_usr1_sent = 0; ox_usr1_handler();
	}
	if ( ox_int_received ) {
		ox_int_received = 0; int_handler(SIGINT);
	}
}

void ox_usr1_handler(sig)
int sig;
{
	extern jmp_buf env;
	unsigned int cmd;

#if !defined(VISUAL)
	signal(SIGUSR1,ox_usr1_handler);
#endif
	if ( critical_when_signal ) {
		fprintf(stderr,"usr1 : critical\n");
		ox_usr1_sent = 1;
	} else {
		ox_flushing = 1;
		ox_resetenv("usr1 : return to toplevel by SIGUSR1");
	}
}

void clear_readbuffer()
{
	char c;
	fd_set r,w,e;
	struct timeval interval;
	int n,sock;

#if defined(linux)
	iofp[0].in->_IO_read_ptr = iofp[0].in->_IO_read_end;
#elif defined(__FreeBSD__)
	fpurge(iofp[0].in);
#endif
/*
	sock = fileno(iofp[0].in);
	interval.tv_sec = (int)0;
	interval.tv_usec = (int)0;

	FD_ZERO(&r); FD_ZERO(&w); FD_ZERO(&e);
	FD_SET(sock,&r);
	while ( 1 ) {
		n = select(FD_SETSIZE,&r,&w,&e,&interval);
		if ( !n )
			break;
		read(sock,&c,1);
	}
*/
}

#if MPI
int ox_data_is_available(int s)
{
	return 1;
}

void wait_for_data(int s)
{
	return;
}
#else
int ox_data_is_available(int s)
{
	return FP_DATA_IS_AVAILABLE(iofp[s].in);
}

void wait_for_data(int s)
{
	fd_set r;
	int sock;

	if ( !FP_DATA_IS_AVAILABLE(iofp[s].in) ) {
#if defined(VISUAL)
		sock = iofp[s].in->fildes;
		FD_ZERO(&r);
		FD_SET(sock,&r);
		select(0,&r,NULL,NULL,NULL);
#else
		sock = fileno(iofp[s].in);
		FD_ZERO(&r);
		FD_SET(sock,&r);
		select(FD_SETSIZE,&r,NULL,NULL,NULL);
#endif
	}
}
#endif

void ox_send_data(int s,pointer p)
{
	if ( ox_check && !ox_check_cmo(s,(Obj)p) )
		error("ox_send_data : Mathcap violation");
	begin_critical();
	ox_write_int(s,OX_DATA);
	ox_write_int(s,ox_serial++);
	ox_write_cmo(s,p);
	ox_flush_stream(s);
	end_critical();
}

void ox_send_cmd(int s,int id)
{
	if ( ox_check && !check_sm_by_mc(s,id) )
		error("ox_send_cmd : Mathcap violation");
	begin_critical();
	ox_write_int(s,OX_COMMAND);
	ox_write_int(s,ox_serial++);
	ox_write_int(s,id);
	ox_flush_stream(s);
	end_critical();
}

void ox_send_sync(int s)
{
	begin_critical();
	ox_write_int(s,OX_SYNC_BALL);
	ox_write_int(s,ox_serial++);
	ox_flush_stream(s);
	end_critical();
}

void ox_send_local_data(int s,Obj p)
{
	begin_critical();
	ox_write_int(s,OX_LOCAL_OBJECT_ASIR);
	ox_write_int(s,ox_serial++);
	ox_write_int(s,ASIR_OBJ);
	saveobj(iofp[s].out,p);
	ox_flush_stream(s);
	end_critical();
}

void ox_send_local_ring(int s,VL vl)
{
	begin_critical();
	ox_write_int(s,OX_LOCAL_OBJECT_ASIR);
	ox_write_int(s,ox_serial++);
	ox_write_int(s,ASIR_VL);
	savevl(iofp[s].out,vl);
	ox_flush_stream(s);
	end_critical();
}

unsigned int ox_recv(int s, int *id, pointer *p)
{
	unsigned int cmd,serial;
	USINT ui;

	wait_for_data(s);
	begin_critical();
	ox_read_int(s,id);
	ox_read_int(s,&serial);
	switch ( *id ) {
		case OX_COMMAND:
			ox_read_int(s,&cmd);
			MKUSINT(ui,cmd);
			*p = (pointer)ui;
			break;
		case OX_DATA:
			ox_read_cmo(s,p);
			break;
		case OX_LOCAL_OBJECT_ASIR:
			ox_read_local(s,p);
			break;
		default:
			*p = 0;
			break;
	}
	end_critical();
	return serial;
}

void ox_get_result(s,rp)
int s;
Obj *rp;
{
	int id;
	Obj obj,r;
	int level;

	level = 0;
	r = 0;
	do {
		ox_recv(s,&id,(pointer *)&obj);
		if ( id == OX_COMMAND ) {
			switch ( ((USINT)obj)->body ) {
				case SM_beginBlock:
					level++;
					break;
				case SM_endBlock:
					level--;
			}
		} else 
			r = obj;
	} while ( level );
	*rp = r;
}

void ox_read_int(int s, int *n)
{
	ox_need_conv = iofp[s].conv;
	read_int(iofp[s].in,n);
}

void ox_read_cmo(int s, Obj *rp)
{
	ox_need_conv = iofp[s].conv;
	read_cmo(iofp[s].in,rp);
}

void ox_read_local(int s, Obj *rp)
{
	int id;

	ox_need_conv = iofp[s].conv;
	read_int(iofp[s].in,&id);
	switch ( id ) {
		case ASIR_VL:
			loadvl(iofp[s].in);
			*rp = VOIDobj;
			break;
		case ASIR_OBJ:
			loadobj(iofp[s].in,rp);
			break;
		default:
			error("ox_read_local : unsupported id");
			break;
	}
}

void ox_write_int(int s, int n)
{
	ox_need_conv = iofp[s].conv;
	write_int(iofp[s].out,&n);
}

void ox_write_cmo(int s, Obj obj)
{
	ox_need_conv = iofp[s].conv;
	write_cmo(iofp[s].out,obj);
}

int ox_check_cmo(int s, Obj obj)
{
	NODE m;

	if ( !obj )
		return 1;
	switch ( OID(obj) ) {
		case O_MATHCAP: case O_STR: case O_ERR: case O_USINT: case O_VOID:
			return 1;
		case O_P:
			if ( !check_by_mc(s,OX_DATA,CMO_RECURSIVE_POLYNOMIAL) )
				return 0;
			else
				return ox_check_cmo_p(s,(P)obj);	
		case O_R:
			if ( !check_by_mc(s,OX_DATA,CMO_RATIONAL) )
				return 0;
			else if ( !check_by_mc(s,OX_DATA,CMO_RECURSIVE_POLYNOMIAL) )
				return 0;
			else
				return ox_check_cmo_p(s,NM((R)obj)) && ox_check_cmo_p(s,DN((R)obj));
		case O_DP:
			return ox_check_cmo_dp(s,(DP)obj);	
		case O_N:
			if ( NID((Num)obj) == N_Q ) {
				if ( INT((Q)obj) )
					return check_by_mc(s,OX_DATA,CMO_ZZ);
				else
					return check_by_mc(s,OX_DATA,CMO_QQ);
			} else
				return 0;
		case O_LIST:
			for ( m = BDY((LIST)obj); m; m = NEXT(m) )
				if ( !ox_check_cmo(s,(BDY(m))) )
					return 0;
			return 1;
		default:
			return 0;
	}
}

void ox_get_serverinfo(int s, LIST *rp)
{
	if ( remote_mc )
		*rp = remote_mc[s].mc;
	else {
		MKLIST(*rp,0);
	}
}

int ox_check_cmo_p(int s, P p)
{
	DCP dc;

	if ( NUM(p) )
		return ox_check_cmo(s,(Obj)p);
	else {
		for ( dc = DC(p); dc; dc = NEXT(dc) )
			if ( !ox_check_cmo_p(s,COEF(dc)) )
				return 0;
		return 1;
	}
}

int ox_check_cmo_dp(int s, DP p)
{
	MP m;

	for ( m = BDY(p); m; m = NEXT(m) )
		if ( !ox_check_cmo(s,(Obj)m->c) )
			return 0;
	return 1;
}

void ox_flush_stream(s)
int s;
{
	if ( ox_batch )
		return;
#if defined(VISUAL)
	if ( _fileno(&iofp[s].out->fp) < 0 )
		cflush(iofp[s].out);
	else
#elif MPI
	if ( (char)fileno(&iofp[s].out->fp) < 0 )
		cflush(iofp[s].out);
	else
#endif
	fflush(iofp[s].out);
}

void ox_flush_stream_force(s)
int s;
{
#if defined(VISUAL)
	if ( _fileno(&iofp[s].out->fp) < 0 )
		cflush(iofp[s].out);
	else
#elif MPI
	if ( (char)fileno(&iofp[s].out->fp) < 0 )
		cflush(iofp[s].out);
	else
#endif
	fflush(iofp[s].out);
}
