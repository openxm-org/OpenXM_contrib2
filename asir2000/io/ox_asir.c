/* $OpenXM: OpenXM/src/asir99/io/ox_asir.c,v 1.5 1999/11/18 02:24:02 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "version.h"

void ox_usr1_handler();

extern jmp_buf environnement;

extern int do_message;
extern int ox_flushing;
extern jmp_buf ox_env;
extern MATHCAP my_mathcap;

int ox_sock_id;

static int asir_OperandStackSize;
static Obj *asir_OperandStack;
static int asir_OperandStackPtr = -1;

static void create_error(ERR *,unsigned int ,char *);
static void ox_io_init();
static void ox_asir_init(int,char **);
static Obj asir_pop_one();
static void asir_push_one(Obj);
static void asir_end_flush();
static void asir_executeFunction(int);
static int asir_executeString();
static void asir_evalName(unsigned int);
static void asir_setName(unsigned int);
static void asir_pops();
static void asir_popString();
static void asir_popCMO(unsigned int);
static void asir_popSerializedLocalObject();
static LIST asir_GetErrorList();
static char *name_of_cmd(unsigned int);
static char *name_of_id(int);
static void asir_do_cmd(unsigned int,unsigned int);

#if MPI
extern int mpi_nprocs,mpi_myid;

void ox_mpi_master_init() {
	int i,idx,ret;

	for ( i = 1; i < mpi_nprocs; i++ ) {
		/* client mode */
		idx = get_iofp(i,0,0);
		ret = register_server(idx,idx);
	}
}

void ox_mpi_slave_init() {
	endian_init();
	/* server mode */
	get_iofp(0,0,1);
	fclose(stdin);
	asir_OperandStackSize = BUFSIZ;
	asir_OperandStack = (Obj *)CALLOC(asir_OperandStackSize,sizeof(Obj));
	asir_OperandStackPtr = -1;
}
#endif

static void create_error(ERR *err,unsigned int serial,char *msg)
{
	int len;
	USINT ui;
	NODE n,n1;
	LIST list;
	char *msg1;
	STRING errmsg;

	MKUSINT(ui,serial);
	len = strlen(msg);
	msg1 = (char *)MALLOC(len+1);
	strcpy(msg1,msg);
	MKSTR(errmsg,msg1);
	MKNODE(n1,errmsg,0); MKNODE(n,ui,n1); MKLIST(list,n);
	MKERR(*err,list);
}

void ox_main(int argc,char **argv) {
	int id;
	unsigned int cmd;
	Obj obj;
	USINT ui;
	ERR err;
	LIST list;
	NODE n,n1;
	unsigned int serial;
	int ret;
	extern char LastError[];

	ox_asir_init(argc,argv);
	if ( do_message )
		fprintf(stderr,"I'm an ox_asir, Version %d.\n",ASIR_VERSION);
	if ( setjmp(ox_env) ) {
		while ( NEXT(asir_infile) )
			closecurrentinput();
		ox_send_sync(0);
	}
	while ( 1 ) {
		extern int recv_intr;

		serial = ox_recv(0,&id,&obj);
#if defined(VISUAL)
		if ( recv_intr ) {
			if ( recv_intr == 1 ) {
				recv_intr = 0;
				int_handler(SIGINT);
			} else {
				recv_intr = 0;
				ox_usr1_handler(0);
			}
		}
#endif
		if ( do_message )
			fprintf(stderr,"#%d Got %s",serial,name_of_id(id));
		switch ( id ) {
			case OX_COMMAND:
				cmd = ((USINT)obj)->body;
				if ( ox_flushing )
					break;
				if ( do_message )
					fprintf(stderr," %s\n",name_of_cmd(cmd));
				if ( ret = setjmp(env) ) {
					if ( ret == 1 ) {
						create_error(&err,serial,LastError);
						asir_push_one((Obj)err);
					}
					break;
				}
				asir_do_cmd(cmd,serial);
				break;
			case OX_DATA:
			case OX_LOCAL_OBJECT_ASIR:
				if ( ox_flushing )
					break;
				if ( do_message )
					fprintf(stderr," -> data pushed");
				asir_push_one(obj);
				break;
			case OX_SYNC_BALL:
				asir_end_flush();
				break;
			default:
				break;
		}
		if ( do_message )
			fprintf(stderr,"\n");
	}
}

static void asir_do_cmd(unsigned int cmd,unsigned int serial)
{
	MATHCAP client_mathcap;
	Q q;
	int i;
	LIST list;

	switch ( cmd ) {
		case SM_dupErrors:
			list = asir_GetErrorList();
			asir_push_one((Obj)list);
			break;
		case SM_getsp:
			i = asir_OperandStackPtr+1;
			STOQ(i,q);
			asir_push_one((Obj)q);
			break;
		case SM_popSerializedLocalObject:
			asir_popSerializedLocalObject();
			break;
		case SM_popCMO:
			asir_popCMO(serial);
			break;
		case SM_popString:
			asir_popString();
			break;
		case SM_setName:
			asir_setName(serial);
			break;
		case SM_evalName:
			asir_evalName(serial);
			break;
		case SM_executeStringByLocalParser:
			asir_executeString();
			break;
		case SM_executeStringByLocalParserInBatchMode:
			asir_executeString();
			asir_pop_one();
			break;
		case SM_executeFunction:
			asir_executeFunction(serial);
			break;
		case SM_shutdown:
			asir_terminate(2);
			break;
		case SM_pops:
			asir_pops();
			break;
		case SM_mathcap:
			asir_push_one((Obj)my_mathcap);
			break;
		case SM_setMathcap:
			client_mathcap = (MATHCAP)asir_pop_one();
			store_remote_mathcap(0,client_mathcap);
			break;
		case SM_nop:
		default:
			break;
	}
}

static char *name_of_id(int id)
{
	switch ( id ) {
		case OX_COMMAND:
			return "OX_COMMAND";
			break;
		case OX_DATA:
			return "OX_DATA";
			break;
		case OX_LOCAL_OBJECT_ASIR:
			return "OX_LOCAL_OBJECT_ASIR";
			break;
		case OX_SYNC_BALL:
			return "OX_SYNC_BALL";
			break;
		default:
			return "Unknown id";
			break;
	}
}

static char *name_of_cmd(unsigned cmd)
{
	switch ( cmd ) {
		case SM_popSerializedLocalObject:
			return "SM_popSerializedLocalObject";
			break;
		case SM_popCMO:
			return "SM_popCMO";
			break;
		case SM_popString:
			return "SM_popString";
			break;
		case SM_pops:
			return "SM_pops";
			break;
		case SM_setName:
			return "SM_setName";
			break;
		case SM_evalName:
			return "SM_evalName";
			break;
		case SM_executeStringByLocalParser:
			return "SM_executeString";
			break;
		case SM_executeFunction:
			return "SM_executeFunction";
			break;
		case SM_shutdown:
			return "SM_shutdown";
			break;
		case SM_beginBlock:
			return "SM_beginBlock";
			break;
		case SM_endBlock:
			return "SM_endBlock";
			break;
		case SM_mathcap:
			return "SM_mathcap";
			break;
		case SM_setMathcap:
			return "SM_setMathcap";
			break;
		case SM_getsp:
			return "SM_setMathcap";
			break;
		case SM_dupErrors:
			return "SM_dupErrors";
			break;
		case SM_nop:
			return "SM_nop";
		default:
			return "Unknown cmd";
			break;
	}
}

static LIST asir_GetErrorList()
{
	int i;
	NODE n,n0;
	LIST err;
	Obj obj;

	for ( i = 0, n0 = 0; i <= asir_OperandStackPtr; i++ )
		if ( (obj = asir_OperandStack[i]) && (OID(obj) == O_ERR) ) {
			NEXTNODE(n0,n); BDY(n) = (pointer)obj;
		}
	if ( n0 )
		NEXT(n) = 0;
	MKLIST(err,n0);
	return err;
}

static void asir_popSerializedLocalObject()
{
	Obj obj;
	VL t,vl;

	obj = asir_pop_one();
	get_vars_recursive(obj,&vl);
	for ( t = vl; t; t = NEXT(t) )
		if ( t->v->attr == (pointer)V_UC )
			error("bsave : not implemented");
	ox_send_cmd(0,SM_beginBlock);
	ox_send_local_ring(0,vl);
	ox_send_local_data(0,obj);
	ox_send_cmd(0,SM_endBlock);
}

static void asir_popCMO(unsigned int serial)
{
	Obj obj;
	ERR err;

	obj = asir_pop_one();
	if ( valid_as_cmo(obj) )
		ox_send_data(0,obj);
	else {
		create_error(&err,serial,"cannot convert to CMO object");
		ox_send_data(0,err);
		asir_push_one(obj);
	}
}

static void asir_popString()
{
	Obj val;
	char *buf,*obuf;
	int l;
	STRING str;

	val = asir_pop_one();
	if ( !val )
		obuf = 0;
	else {
		l = estimate_length(CO,val);
		buf = (char *)ALLOCA(l+1);
		soutput_init(buf);
		sprintexpr(CO,val);
		l = strlen(buf);
		obuf = (char *)MALLOC(l+1);
		strcpy(obuf,buf);
	}
	MKSTR(str,obuf);
	ox_send_data(0,str);
}

static void asir_pops()
{
	int n;

	n = (int)(((USINT)asir_pop_one())->body);
	asir_OperandStackPtr = MAX(asir_OperandStackPtr-n,-1);
}

static void asir_setName(unsigned int serial)
{
	char *name;
	int l,n;
	char *dummy = "=0;";
	SNODE snode;
	ERR err;

	name = ((STRING)asir_pop_one())->body;
	l = strlen(name);
	n = l+strlen(dummy)+1;
	parse_strp = (char *)ALLOCA(n);
	sprintf(parse_strp,"%s%s",name,dummy);
	if ( mainparse(&snode) ) {
		create_error(&err,serial,"cannot set to variable");
		asir_push_one((Obj)err);
	} else {
		FA1((FNODE)FA0(snode)) = (pointer)mkfnode(1,I_FORMULA,asir_pop_one());
		evalstat(snode);	
	}
}

static void asir_evalName(unsigned int serial)
{
	char *name;
	int l,n;
	SNODE snode;
	ERR err;
	pointer val;

	name = ((STRING)asir_pop_one())->body;
	l = strlen(name);
	n = l+2;
	parse_strp = (char *)ALLOCA(n);
	sprintf(parse_strp,"%s;",name);
	if ( mainparse(&snode) ) {
		create_error(&err,serial,"no such variable");
		val = (pointer)err;
	} else
		val = evalstat(snode);	
	asir_push_one(val);
}

static int asir_executeString()
{
	SNODE snode;
	pointer val;
	char *cmd;
#if PARI
	recover(0);
	if ( setjmp(environnement) ) {
		avma = top; recover(1);
		resetenv("");
	}
#endif
	cmd = ((STRING)asir_pop_one())->body;
	parse_strp = cmd;
	if ( mainparse(&snode) ) {
		return -1;
	}
	val = evalstat(snode);
	if ( NEXT(asir_infile) ) {
		while ( NEXT(asir_infile) ) {
			if ( mainparse(&snode) ) {
				asir_push_one(val);
				return -1;
			}
			nextbp = 0;
			val = evalstat(snode);
		}
	}
	asir_push_one(val);
	return 0;
}

static void asir_executeFunction(int serial)
{ 
	char *func;
	int argc;
	FUNC f;
	Obj result;
	VL vl;
	NODE n,n1; 
	STRING fname;
	char *path;
	USINT ui;
	ERR err;
	static char buf[BUFSIZ];

	func = ((STRING)asir_pop_one())->body;
	argc = (int)(((USINT)asir_pop_one())->body);

	for ( n = 0; argc; argc-- ) {
		NEXTNODE(n,n1);
		BDY(n1) = (pointer)asir_pop_one();
	}
	if ( n )
		NEXT(n1) = 0;

	if ( !strcmp(func,"load") ) {
		fname = (STRING)BDY(n);
		if ( OID(fname) == O_STR ) {
			searchasirpath(BDY(fname),&path);
			if ( path ) {
				if ( do_message )
					fprintf(stderr,"loading %s\n",path);
				execasirfile(path);
			} else 
				if ( do_message )
					fprintf(stderr,"load : %s not found in the search path\n",BDY(fname));
		}
		result = 0;
	} else {
		searchf(noargsysf,func,&f);
		if ( !f )
			searchf(sysf,func,&f); 
		if ( !f )
			searchf(ubinf,func,&f);
		if ( !f )
			searchf(usrf,func,&f);
		if ( !f ) {
			sprintf(buf,"executeFunction : the function %s not found",func);
			create_error(&err,serial,buf);
			result = (Obj)err;
		} else {
			result = (Obj)bevalf(f,n);
		}
	}
	asir_push_one(result);
}

static void asir_end_flush()
{
	ox_flushing = 0;
}

/* 
  asir_OperandStackPtr points to the surface of the stack.
  That is, the data at the stack top is
	asir_OperandStack[asir_OperandStackPtr].
*/


static void asir_push_one(Obj obj)
{
	if ( !obj || OID(obj) != O_VOID ) {
		asir_OperandStackPtr++;
		if ( asir_OperandStackPtr >= asir_OperandStackSize ) {
			asir_OperandStackSize += BUFSIZ;
			asir_OperandStack 
				= (Obj *)REALLOC(asir_OperandStack,
					asir_OperandStackSize*sizeof(Obj));
		}
		asir_OperandStack[asir_OperandStackPtr] = obj;
	}
}

static Obj asir_pop_one() {
	if ( asir_OperandStackPtr < 0 ) {
		if ( do_message )
			fprintf(stderr,"OperandStack underflow");
		return 0;
	} else {
		if ( do_message )
			fprintf(stderr,"pop at %d\n",asir_OperandStackPtr);
		return asir_OperandStack[asir_OperandStackPtr--];
	}
}

static void ox_asir_init(int argc,char **argv)
{
	int tmp;
	char ifname[BUFSIZ];
	extern int GC_dont_gc;
	extern int read_exec_file;
	extern int do_asirrc;
	extern int do_server_in_X11;
	char *getenv();
	static ox_asir_initialized = 0;
	FILE *ifp;

#if !defined(VISUAL) && !MPI
	do_server_in_X11 = 1; /* XXX */
#endif
	asir_save_handler();
#if PARI
	risa_pari_init();
#endif
	srandom((int)get_current_time());

#if defined(THINK_C)
	param_init();
#endif
	StackBottom = &tmp + 1; /* XXX */
	rtime_init();
	env_init();
	endian_init();
#if !defined(VISUAL) && !defined(THINK_C)
/*	check_key(); */
#endif
	GC_init();
	process_args(--argc,++argv);
	output_init();
	arf_init();
	nglob_init();
	glob_init();
	sig_init();
	tty_init();
	debug_init();
	pf_init();
	sysf_init();
	parif_init();
#if defined(VISUAL)
	init_socket();
#endif
#if defined(UINIT)
	reg_sysf();
#endif
#if defined(THINK_C)
	sprintf(ifname,"asirrc");
#else
	sprintf(ifname,"%s/.asirrc",getenv("HOME"));
#endif
	if ( do_asirrc && (ifp = fopen(ifname,"r")) ) {
		input_init(ifp,ifname);
		if ( !setjmp(env) ) {
			read_exec_file = 1;
			read_eval_loop();
			read_exec_file = 0;
		}
		fclose(ifp);
	}
	input_init(0,"string");
#if !MPI
	ox_io_init();
#endif
	create_my_mathcap("ox_asir");
}

static void ox_io_init() {
	unsigned char c,rc;
	extern int little_endian,ox_sock_id;

	endian_init();
#if defined(VISUAL)
	if ( !ox_sock_id )
		exit(0);
	iofp[0].in = WSIO_open(ox_sock_id,"r");
	iofp[0].out = WSIO_open(ox_sock_id,"w");
#else
	iofp[0].in = fdopen(3,"r");
	iofp[0].out = fdopen(4,"w");

	setbuffer(iofp[0].in,(char *)malloc(LBUFSIZ),LBUFSIZ);
	setbuffer(iofp[0].out,(char *)malloc(LBUFSIZ),LBUFSIZ);
	signal(SIGUSR1,ox_usr1_handler);
#endif
	asir_OperandStackSize = BUFSIZ;
	asir_OperandStack = (Obj *)CALLOC(asir_OperandStackSize,sizeof(Obj));
	asir_OperandStackPtr = -1;
	if ( little_endian )
		c = 1;
	else
		c = 0xff;
	/* server : write -> read */
	write_char(iofp[0].out,&c); ox_flush_stream_force(0);
	read_char(iofp[0].in,&rc);
	iofp[0].conv = c == rc ? 0 : 1;
}
