/* $OpenXM: OpenXM/src/asir99/plot/ox_plot.c,v 1.3 1999/11/19 05:51:24 noro Exp $ */
#include "ca.h"
#include "parse.h"
#include "ox.h"
#include "ifplot.h"
#include "version.h"

void ox_usr1_handler();

extern jmp_buf environnement;

extern int do_message;
extern int ox_flushing;
extern jmp_buf ox_env;
extern MATHCAP my_mathcap;

static int plot_OperandStackSize;
static Obj *plot_OperandStack;
static int plot_OperandStackPtr = -1;

static void create_error(ERR *,unsigned int ,char *);
static void process_ox();
static void ox_io_init();
static void ox_asir_init(int,char **);
static Obj asir_pop_one();
static void asir_push_one(Obj);
static void asir_end_flush();
static void asir_executeFunction();
static int asir_executeString();
static void asir_evalName(unsigned int);
static void asir_setName(unsigned int);
static void asir_pops();
static void asir_popString();
static void asir_popCMO(unsigned int);
static void asir_popSerializedLocalObject();
static char *name_of_cmd(unsigned int);
static char *name_of_id(int);
static void asir_do_cmd(unsigned int,unsigned int);

static void create_error(ERR *err,unsigned int serial,char *msg)
{
	USINT ui;
	NODE n,n1;
	LIST list;
	STRING errmsg;

	MKUSINT(ui,serial);
	MKSTR(errmsg,msg);
	MKNODE(n1,errmsg,0); MKNODE(n,ui,n1); MKLIST(list,n);
	MKERR(*err,list);
}

void ox_plot_main(int argc,char **argv) {
	int ds;
	fd_set r;
	int n;

	ox_asir_init(argc,argv);
	init_plot_display(argc,argv);
	ds = ConnectionNumber(display);
	if ( do_message )
		fprintf(stderr,"I'm an ox_plot, Version %d.\n",ASIR_VERSION);

	if ( setjmp(ox_env) ) {
		while ( NEXT(asir_infile) )
			closecurrentinput();
		reset_current_computation();
		ox_send_sync(0);
	}
	while ( 1 ) {
		if ( ox_data_is_available(0) )
			process_ox();
		else {
			FD_ZERO(&r);
			FD_SET(3,&r); FD_SET(ds,&r);
			select(FD_SETSIZE,&r,NULL,NULL,NULL);
			if ( FD_ISSET(3,&r) )
				process_ox();
			else if ( FD_ISSET(ds,&r) )
				process_xevent();
		}
	}
}

static void process_ox()
{
	int id;
	unsigned int cmd;
	Obj obj;
	ERR err;
	unsigned int serial;
	int ret;
	extern char LastError[];

	serial = ox_recv(0,&id,&obj);
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

static void asir_do_cmd(unsigned int cmd,unsigned int serial)
{
	MATHCAP client_mathcap;

	switch ( cmd ) {
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
		case SM_executeFunction:
			asir_executeFunction();
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
		default:
			return "Unknown cmd";
			break;
	}
}

static void asir_popSerializedLocalObject()
{
	Obj obj;
	VL t,vl;

	obj = asir_pop_one();
	get_vars(obj,&vl);
	for ( t = vl; t; t = NEXT(t) )
		if ( t->v->attr == (pointer)V_UC || t->v->attr == (pointer)V_PF )
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
	plot_OperandStackPtr = MAX(plot_OperandStackPtr-n,-1);
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

static void asir_executeFunction()
{ 
	char *func;
	int argc;
	int id;
	FUNC f;
	Q ret;
	VL vl;
	NODE n,n1; 

	func = ((STRING)asir_pop_one())->body;
	argc = (int)(((USINT)asir_pop_one())->body);

	for ( n = 0; argc; argc-- ) {
		NEXTNODE(n,n1);
		BDY(n1) = (pointer)asir_pop_one();
	}
	if ( n )
		NEXT(n1) = 0;
	id = -1;
	if ( !strcmp(func,"plot") )
		id = plot(n);
	else if ( !strcmp(func,"arrayplot") )
		id = arrayplot(n);
	else if ( !strcmp(func,"plotover") )
		id = plotover(n);
	else if ( !strcmp(func,"drawcircle") )
		id = drawcircle(n);
	STOQ(id,ret);
#if 0
	asir_push_one((Obj)ret);
#endif
}

static void asir_end_flush()
{
	ox_flushing = 0;
}

static void asir_push_one(Obj obj)
{
	if ( !obj || OID(obj) != O_VOID ) {
		plot_OperandStackPtr++;
		if ( plot_OperandStackPtr >= plot_OperandStackSize ) {
			plot_OperandStackSize += BUFSIZ;
			plot_OperandStack
				= (Obj *)REALLOC(plot_OperandStack,
					plot_OperandStackSize*sizeof(Obj));
		}
		plot_OperandStack[plot_OperandStackPtr] = obj;
	}
}

static Obj asir_pop_one() {
	if ( plot_OperandStackPtr < 0 ) {
		if ( do_message )
			fprintf(stderr,"OperandStack underflow");
		return 0;
	} else {
		if ( do_message )
			fprintf(stderr,"pop at %d\n",plot_OperandStackPtr);
		return plot_OperandStack[plot_OperandStackPtr--];
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

	do_server_in_X11 = 1; /* XXX */
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
#if 0
	copyright();
#endif
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
	ox_io_init();
	create_my_mathcap("ox_plot");
}

static void ox_io_init() {
	unsigned char c,rc;
	extern int little_endian;

	endian_init();
	iofp[0].in = fdopen(3,"r");
	iofp[0].out = fdopen(4,"w");
	setbuffer(iofp[0].in,(char *)malloc(LBUFSIZ),LBUFSIZ);
	setbuffer(iofp[0].out,(char *)malloc(LBUFSIZ),LBUFSIZ);
	plot_OperandStackSize = BUFSIZ;
	plot_OperandStack = (Obj *)CALLOC(plot_OperandStackSize,sizeof(Obj));
	plot_OperandStackPtr = -1;
	signal(SIGUSR1,ox_usr1_handler);
	if ( little_endian )
		c = 1;
	else
		c = 0xff;
	write_char(iofp[0].out,&c); ox_flush_stream(0);
	read_char(iofp[0].in,&rc);
	iofp[0].conv = c == rc ? 0 : 1;
}
