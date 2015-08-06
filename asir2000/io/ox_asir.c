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
 * $OpenXM: OpenXM_contrib2/asir2000/io/ox_asir.c,v 1.73 2015/08/04 06:20:45 noro Exp $ 
*/
#include "ca.h"
#include "parse.h"
#include "signal.h"
#include "ox.h"
#include "version.h"

char *find_asirrc();
void ox_usr1_handler();
int asir_ox_init();

/* environement is defined in libpari.a */
#if 0
# if !( PARI_VERSION_CODE > 131588)
extern jmp_buf environnement;
# endif
#endif
extern int myrank_102,nserver_102;

extern int do_message;
extern int ox_flushing;
extern JMP_BUF ox_env;
extern MATHCAP my_mathcap;

extern int little_endian,ox_sock_id;
extern char LastError[];
extern LIST LastStackTrace;

int ox_sock_id;
int lib_ox_need_conv;

void create_error(ERR *,unsigned int ,char *,LIST trace);

int asir_OperandStackSize;
Obj *asir_OperandStack;
int asir_OperandStackPtr = -1;

void ox_io_init();
void ox_asir_init(int,char **,char *);
Obj asir_pop_one();
Obj asir_peek_one();
void asir_push_one(Obj);
void asir_end_flush();
int asir_executeString();
void asir_evalName(unsigned int);
void asir_setName(unsigned int);
void asir_pops();
void asir_popString();
void asir_popCMO(unsigned int);
void asir_popSerializedLocalObject();
void asir_pushCMOtag(unsigned int);
void asir_set_rank_102(unsigned int);
void asir_tcp_accept_102(unsigned int);
void asir_tcp_connect_102(unsigned int);
void asir_reset_102(unsigned int serial);
void asir_bcast_102(unsigned int serial);
void asir_reduce_102(unsigned int serial);
LIST asir_GetErrorList();
char *name_of_cmd(int);
char *name_of_id(int);

static void asir_do_cmd(int,unsigned int);
static void asir_executeFunction(int);
static void asir_executeFunctionSync(int);

#if defined(MPI)
/* XXX : currently MPI version supports only a homogeneous cluster. */

extern int mpi_nprocs,mpi_myid;

void ox_mpi_master_init() {
	int i,idx;

	for ( i = 0; i < mpi_nprocs; i++ ) {
		/* ordering information is not exchanged */
		/* idx should be equal to i */
		idx = get_iofp(i,0,0);
		register_server(0,idx,idx,-1);
	}
}

void ox_mpi_slave_init() {
	int i,idx;

	endian_init();
	fclose(stdin);
	for ( i = 0; i < mpi_nprocs; i++ ) {
		/* ordering information is not exchanged */
		/* idx should be equal to i */
		idx = get_iofp(i,0,0);
		register_server(0,idx,idx,-1);
	}
	asir_OperandStackSize = BUFSIZ;
	asir_OperandStack = (Obj *)CALLOC(asir_OperandStackSize,sizeof(Obj));
	asir_OperandStackPtr = -1;
}
#endif

void ox_main(int argc,char **argv) {
	int id;
	int cmd;
	Obj obj;
	ERR err;
	unsigned int serial;
	int ret;

	ox_asir_init(argc,argv,"ox_asir");
	if ( do_message )
		fprintf(stderr,"I'm an ox_asir, Version %d.\n",ASIR_VERSION);
	if ( SETJMP(ox_env) ) {
		while ( NEXT(asir_infile) )
			closecurrentinput();
		resetpvs();
		reset_engine();
		reset_io();
		ox_send_sync(0);
	}
	while ( 1 ) {
		extern int recv_intr;

		serial = ox_recv(0,&id,&obj);
#if defined(VISUAL) || defined(__MINGW32__) || defined(__MINGW64__)
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
				if ( ret = SETJMP(main_env) ) {
					if ( ret == 1 ) {
						create_error(&err,serial,LastError,LastStackTrace);
						asir_push_one((Obj)err);
						while ( NEXT(asir_infile) )
							closecurrentinput();
						resetpvs();
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

static void asir_do_cmd(int cmd,unsigned int serial)
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
		case SM_executeFunctionSync:
			asir_executeFunctionSync(serial);
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
		case SM_pushCMOtag:
			asir_pushCMOtag(serial);
			break;
		case SM_set_rank_102:
			asir_set_rank_102(serial);		
			break;
		case SM_tcp_accept_102:
			asir_tcp_accept_102(serial);
			break;
		case SM_tcp_connect_102:
			asir_tcp_connect_102(serial);
			break;
		case SM_reset_102:
			asir_reset_102(serial);
			break;
		case SM_bcast_102:
			asir_bcast_102(serial);
			break;
		case SM_reduce_102:
			asir_reduce_102(serial);
			break;
		case SM_nop:
		default:
			break;
	}
}

char *name_of_id(int id)
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

char *name_of_cmd(int cmd)
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
		case SM_pushCMOtag:
			return "SM_pushCMOtag";
		case SM_set_rank_102:
			return "SM_set_rank_102";
			break;
		case SM_tcp_accept_102:
			return "SM_tcp_accept_102";
			break;
		case SM_tcp_connect_102:
			return "SM_tcp_connect_102";
		case SM_reset_102:
			return "SM_reset_102";
			break;
		case SM_bcast_102:
			return "SM_bcast_102";
			break;
		case SM_reduce_102:
			return "SM_reduce_102";
			break;
		case SM_executeFunctionSync:
			return "SM_executeFunctionSync";
			break;
		default:
			return "Unknown cmd";
			break;
	}
}

LIST asir_GetErrorList()
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

void asir_popSerializedLocalObject()
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

void asir_popCMO(unsigned int serial)
{
	Obj obj;
	ERR err;

	obj = asir_pop_one();
	if ( valid_as_cmo(obj) )
		ox_send_data(0,obj);
	else {
		create_error(&err,serial,"cannot convert to CMO object",0);
		ox_send_data(0,err);
		asir_push_one(obj);
	}
}

void asir_reduce_102(unsigned int serial)
{
	Q r;
	int root;
	Obj data,obj;
	ERR err;
	STRING op;
	char *opname;
	void (*func)();

	func = 0;
	op = (STRING)asir_pop_one();
	opname = BDY(op);
	r = (Q)asir_pop_one();
	root = QTOS(r);
	if ( !strcmp(opname,"+") )
		func = arf_add;
	else if ( !strcmp(opname,"*") )
		func = arf_mul;
	if ( !func ) {
		create_error(&err,serial,"Invalid opration in ox_reduce_102",0);
		asir_push_one(obj);
	} else
		ox_reduce_102(root,func);
}

void asir_bcast_102(unsigned int serial)
{
	Q r;
	int root;
	Obj data;

	r = (Q)asir_pop_one();
	root = QTOS(r);
	ox_bcast_102(root);
}

void asir_reset_102(unsigned int serial)
{
	int i,j,id;
	Obj obj;

	for ( i = 0; i < myrank_102; i++ )
		do {
			ox_recv_102(i,&id,&obj);
		} while ( id != OX_SYNC_BALL );
	for ( i = myrank_102+1; i < nserver_102; i++ )
		ox_send_sync_102(i);
}

void asir_set_rank_102(unsigned int serial)
{
	Obj obj;
	Q rank,nserver;
	int n,r,stat;
	NODE arg;
	ERR err;

	rank = (Q)asir_pop_one();
	nserver = (Q)asir_pop_one();
	stat = 0;
	if ( !nserver || !INT(nserver) || !INT(rank) ) {
		stat = -1;
	} else {
		n = QTOS(nserver); r = QTOS(rank);
		if ( n <= 0 || r < 0 || r >= n ) {
			stat = -1;
		}
		myrank_102 = r;
		nserver_102 = n;
	}
	if ( !stat ) return;
	else {
		create_error(&err,serial,"Invalid argument(s) in ox_set_rank_102",0);
		asir_push_one(obj);
	}
}

void asir_tcp_accept_102(unsigned int serial)
{
	Obj obj;
	Q r,p;
	ERR err;
	char port_str[BUFSIZ];
	int port,s,use_unix,rank;

	r = (Q)asir_pop_one();
	p = (Q)asir_pop_one();
	if ( IS_CYGWIN || !p || NUM(p) ) {
		port = QTOS(p);
		sprintf(port_str,"%d",port);
		use_unix = 0;
	} else {
		strcpy(port_str,BDY((STRING)p));
		use_unix = 1;
	}
	s = try_bind_listen(use_unix,port_str);
	s = try_accept(use_unix,s);
	rank = QTOS((Q)r);
	if ( register_102(s,rank,1) < 0 ) {
		create_error(&err,serial,
			"failed to bind or accept in ox_tcp_accept_102",0);
		asir_push_one((Obj)err);
	}
}

void asir_tcp_connect_102(unsigned int serial)
{
	Obj obj;
	Q r,p;
	STRING h;
	ERR err;
	char *host;
	char port_str[BUFSIZ];
	int port,s,use_unix,rank;

	r = (Q)asir_pop_one();
	p = (Q)asir_pop_one();
	h = (STRING)asir_pop_one();
	if ( IS_CYGWIN || !p || NUM(p) ) {
		port = QTOS(p);
		sprintf(port_str,"%d",port);
		use_unix = 0;
		host = BDY((STRING)h);
	} else {
		strcpy(port_str,BDY((STRING)p));
		use_unix = 1;
		host = 0;
	}
	s = try_connect(use_unix,host,port_str);
	rank = QTOS((Q)r);
	if ( register_102(s,rank,1) < 0 ) {
		create_error(&err,serial,
			"failed to connect in ox_tcp_connect_102",0);
		asir_push_one((Obj)err);
	}
}

void asir_pushCMOtag(unsigned int serial)
{
	Obj obj;
	ERR err;
	USINT ui;
	int tag;

	obj = asir_peek_one();
	if ( cmo_tag(obj,&tag) ) {
		MKUSINT(ui,tag);
		asir_push_one((Obj)ui);
	} else {
		create_error(&err,serial,"cannot convert to CMO object",0);
		asir_push_one((Obj)err);
	}
}

void print_to_wfep(Obj obj)
{
	asir_push_one(obj);
	asir_popString();
}

extern int wfep_mode;

void asir_popString()
{
	Obj val;
	char *buf;
	int l;
	STRING str;

	val = asir_pop_one();
	if ( !val )
		buf = "0";
	else if ( wfep_mode && OID(val) == O_ERR ) {
		/* XXX : for wfep */
		ox_send_data(0,val); return;
	} else {
		l = estimate_length(CO,val);
		buf = (char *)ALLOCA(l+1);
		soutput_init(buf);
		sprintexpr(CO,val);
	}
	MKSTR(str,buf);
	ox_send_data(0,str);
}

void asir_pops()
{
	int n;

	n = (int)(((USINT)asir_pop_one())->body);
	asir_OperandStackPtr = MAX(asir_OperandStackPtr-n,-1);
}

void asir_setName(unsigned int serial)
{
	char *name;
	size_t l,n;
	char *dummy = "=0;";
	SNODE snode;
	ERR err;

	name = ((STRING)asir_pop_one())->body;
	l = strlen(name);
	n = l+strlen(dummy)+1;
	parse_strp = (char *)ALLOCA(n);
	sprintf(parse_strp,"%s%s",name,dummy);
	if ( mainparse(&snode) ) {
		create_error(&err,serial,"cannot set to variable",0);
		asir_push_one((Obj)err);
	} else {
		FA1((FNODE)FA0(snode)) = (pointer)mkfnode(1,I_FORMULA,asir_pop_one());
		evalstat(snode);	
	}
}

void asir_evalName(unsigned int serial)
{
	char *name;
	size_t l,n;
	SNODE snode;
	ERR err;
	pointer val;

	name = ((STRING)asir_pop_one())->body;
	l = strlen(name);
	n = l+2;
	parse_strp = (char *)ALLOCA(n);
	sprintf(parse_strp,"%s;",name);
	if ( mainparse(&snode) ) {
		create_error(&err,serial,"no such variable",0);
		val = (pointer)err;
	} else
		val = evalstat(snode);	
	asir_push_one(val);
}

char *augment_backslash(char *s)
{
	char *p,*r;
	int i;

	for ( i = 0, p = s; *p; p++, i++ ) if ( *p == '\\' ) i++;
	r = (char *)MALLOC_ATOMIC((i+1)*sizeof(char));
	for ( i = 0, p = s; *p; p++, i++ ) {
		if ( *p == '\\' ) r[i++] = '\\';
		r[i] = *p;
	}
	return r;
}

int asir_executeString()
{
	SNODE snode;
	pointer val;
	char *cmd;
#if 0
#if defined(PARI)
	recover(0);
	/* environement is defined in libpari.a */
# if !(PARI_VERSION_CODE > 131588 )
	if ( setjmp(environnement) ) {
		avma = top; recover(1);
		resetenv("");
	}
# endif
#endif
#endif
	cmd = ((STRING)asir_pop_one())->body;
/* XXX : probably this is useless */
#if 0
	parse_strp = augment_backslash(cmd);
#else
	parse_strp = cmd;
#endif
	asir_infile->ln = 1;
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
	NODE n,n1; 
	STRING fname;
	char *path;
	ERR err;
	Obj arg;
	static char buf[BUFSIZ];

	arg = asir_pop_one();
	if ( !arg || OID(arg) != O_STR ) {
		sprintf(buf,"executeFunction : invalid function name");
		goto error;
	} else
		func = ((STRING)arg)->body;

	arg = asir_pop_one();
	if ( !arg || OID(arg) != O_USINT ) {
		sprintf(buf,"executeFunction : invalid argc");
		goto error;
	} else
		argc = (int)(((USINT)arg)->body);

	for ( n = 0; argc; argc-- ) {
		NEXTNODE(n,n1);
		BDY(n1) = (pointer)asir_pop_one();
	}
	if ( n )
		NEXT(n1) = 0;

#if 0
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
#endif
		searchf(noargsysf,func,&f);
		if ( !f )
			gen_searchf_searchonly(func,&f); 
		if ( !f ) {
			sprintf(buf,"executeFunction : the function %s not found",func);
			goto error;
		} else {
			result = (Obj)bevalf(f,n);
		}
#if 0
	}
#endif
	printf("executeFunction done\n");
	asir_push_one(result);
	return;

error:
	create_error(&err,serial,buf,0);
	result = (Obj)err;
	asir_push_one(result);
}

static void asir_executeFunctionSync(int serial)
{ 
	char *func;
	int argc,i;
	FUNC f;
	Obj result=0;
	NODE n,n1; 
	STRING fname;
	char *path;
	ERR err;
	Obj arg;
	static char buf[BUFSIZ];

	arg = asir_pop_one();
	if ( !arg || OID(arg) != O_STR ) {
		sprintf(buf,"executeFunction : invalid function name");
		goto error;
	} else
		func = ((STRING)arg)->body;

	arg = asir_pop_one();
	if ( !arg || OID(arg) != O_USINT ) {
		sprintf(buf,"executeFunction : invalid argc");
		goto error;
	} else
		argc = (int)(((USINT)arg)->body);

	for ( n = 0; argc; argc-- ) {
		NEXTNODE(n,n1);
		BDY(n1) = (pointer)asir_pop_one();
	}
	if ( n )
		NEXT(n1) = 0;

	ox_send_data(0,ONE);

#if 0
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
#endif
		searchf(noargsysf,func,&f);
		if ( !f )
			gen_searchf_searchonly(func,&f); 
		if ( !f ) {
			sprintf(buf,"executeFunction : the function %s not found",func);
			goto error;
		} else {
			result = (Obj)bevalf(f,n);
		}
#if 0
	}
#endif
	printf("executeFunctionSync done\n");
	ox_send_data(0,result);
	return;

error:
	create_error(&err,serial,buf,0);
	result = (Obj)err;
	ox_send_data(0,result);
}

void asir_end_flush()
{
	ox_flushing = 0;
}

/* 
  asir_OperandStackPtr points to the surface of the stack.
  That is, the data at the stack top is
	asir_OperandStack[asir_OperandStackPtr].
*/


void asir_push_one(Obj obj)
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

Obj asir_pop_one() {
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

Obj asir_peek_one() {
	if ( asir_OperandStackPtr < 0 ) {
		if ( do_message )
			fprintf(stderr,"OperandStack underflow");
		return 0;
	} else {
		if ( do_message )
			fprintf(stderr,"peek at %d\n",asir_OperandStackPtr);
		return asir_OperandStack[asir_OperandStackPtr];
	}
}

void ox_asir_init(int argc,char **argv,char *servername)
{
	char *ifname;
	extern int GC_dont_gc;
	extern int do_asirrc;
	extern int do_server_in_X11;
	extern char displayname[];
	static ox_asir_initialized = 0;
	int do_server_sav;
#if !defined(VISUAL) && !defined(__MINGW32__) && !defined(__MINGW64__)
	int tmp;
#endif

	GC_init();
#if !defined(VISUAL) && !defined(__MINGW32__) && !defined(__MINGW64__) && !defined(MPI)
	do_server_in_X11 = 1; /* XXX */
#endif
	asir_save_handler();
#if 0
#if defined(PARI)
	risa_pari_init();
#endif
#endif
	srandom((int)get_current_time());

	rtime_init();
	env_init();
	endian_init();
	cppname_init();
	process_args(--argc,++argv);
#if defined(__CYGWIN__)
	if ( !displayname[0] )
		do_server_in_X11 = 0; /* XXX */
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
	order_init();
#if defined(VISUAL) || defined(__MINGW32__) || defined(__MINGW64__)
	init_socket();
#endif
#if defined(UINIT)
	reg_sysf();
#endif
	/* the bottom of the input stack */
	input_init(0,"string");

	if ( do_asirrc && (ifname = find_asirrc()) ) {
		do_server_sav = do_server_in_X11;
		do_server_in_X11 = 0;
		if ( !SETJMP(main_env) )
			execasirfile(ifname);
		do_server_in_X11 = do_server_sav;
	}

/* XXX Windows compatibility */
	ox_io_init();
	create_my_mathcap(servername);
}

void ox_io_init() {
	unsigned char c,rc;
	extern int I_am_server;

	/* XXX : ssh forwards stdin to a remote host on PC Unix */
#if defined(linux)
#include <sys/param.h>
	int i;

	close(0);
	for ( i = 5; i < NOFILE; i++ )
		close(i);
#elif defined(__FreeBSD__)
#include <sys/resource.h>
	int i;
	struct rlimit rl;

	getrlimit(RLIMIT_NOFILE,&rl);
	close(0);
	for ( i = 5; i < rl.rlim_cur; i++ )
		close(i);
#endif

	I_am_server = 1;
	endian_init();
#if defined(VISUAL) || defined(__MINGW32__) || defined(__MINGW64__)
	if ( !ox_sock_id )
		exit(0);
	iofp[0].in = WSIO_open(ox_sock_id,"r");
	iofp[0].out = WSIO_open(ox_sock_id,"w");
#else
	iofp[0].in = fdopen(3,"r");
	iofp[0].out = fdopen(4,"w");

#if !defined(__CYGWIN__)
	setbuffer(iofp[0].in,(char *)malloc(LBUFSIZ),LBUFSIZ);
	setbuffer(iofp[0].out,(char *)malloc(LBUFSIZ),LBUFSIZ);
#endif
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
	/* XXX; for raw I/O */
	register_server(0,0,0,-1);
}

#if !defined(VISUAL) && !defined(__MINGW32__) && !defined(__MINGW64__)
/*
 * Library mode functions
 */

/*
 * Converts a binary encoded CMO into a risa object
 * and pushes it onto the stack.
 */

void asir_ox_push_cmo(void *cmo)
{
	Obj obj;

	ox_copy_init(cmo);
	ox_buf_to_obj_as_cmo(&obj);
	asir_push_one(obj);
}

/*
 * Pop an object from the stack and converts it
 * into a binary encoded CMO.
 */

int asir_ox_pop_cmo(void *cmo, int limit)
{
	Obj obj;
	int len;
	ERR err;

	obj = asir_pop_one();
	if ( !valid_as_cmo(obj) ) {
		asir_push_one(obj);
		create_error(&err,0,"The object at the stack top is invalid as a CMO.",0);
		obj = (Obj)err;
	}
	len = count_as_cmo(obj);
	if ( len <= limit ) {
		ox_copy_init(cmo);
		ox_obj_to_buf_as_cmo(obj);
		return len;
	} else
		return -1;
}

int asir_ox_pop_string(void *string, int limit)
{
	Obj val;
	int l;

	val = asir_pop_one();
	if ( !val ) {
		if ( limit >= 2 ) {
			sprintf(string,"0");
			l = strlen(string);
		} else
			l = -1;
	} else {
		l = estimate_length(CO,val);
		if ( l+1 <= limit ) {
			soutput_init(string);
			sprintexpr(CO,val);
			l = strlen(string);
		} else
			l = -1;
	}
	return l;
}

/*
 * Executes an SM command.
 */

void asir_ox_push_cmd(int cmd)
{
	int ret;
	ERR err;
	extern char LastError[];

	if ( ret = SETJMP(main_env) ) {
		asir_reset_handler();
		if ( ret == 1 ) {
			create_error(&err,0,LastError,LastStackTrace); /* XXX */
			asir_push_one((Obj)err);
		}
	} else {
		asir_save_handler();
		asir_set_handler();
		asir_do_cmd(cmd,0);
		asir_reset_handler();
	}
}

/*
 * Executes a string written in Asir. 
 */

void asir_ox_execute_string(char *s)
{
	STRING str;
	int ret;
	ERR err;
	extern char LastError[];

	MKSTR(str,s);
	asir_push_one((Obj)str);
	if ( ret = SETJMP(main_env) ) {
		asir_reset_handler();
		if ( ret == 1 ) {
			create_error(&err,0,LastError,LastStackTrace); /* XXX */
			asir_push_one((Obj)err);
		}
	} else {
		asir_save_handler();
		asir_set_handler();
		asir_executeString();
		asir_reset_handler();
	}
}

/*
 * Returns the size as a CMO of the object 
 * at the top of the stack.
 */

int asir_ox_peek_cmo_size()
{
	Obj obj;
	int len;

	obj = asir_peek_one();
	if ( !valid_as_cmo(obj) ) {
		fprintf(stderr,"The object at the stack top is invalid as a CMO.\n");
		return 0;
	}
	len = count_as_cmo(obj);
	return len;
}

int asir_ox_peek_cmo_string_length()
{
	Obj obj;
	int len;

	obj = asir_peek_one();
	if ( !valid_as_cmo(obj) ) {
		fprintf(stderr,"The object at the stack top is invalid as a CMO.\n");
		return 0;
	}
	len = estimate_length(CO,obj);
	return len+1;
}

/*
 * Initialization.
 * byteorder=0 => native
 *          =1 => network byte order
 */

int asir_ox_init(int byteorder)
{
	int tmp;
	char *ifname;
	extern int GC_dont_gc;
	extern int do_asirrc;
	extern int do_server_in_X11;
	static ox_asir_initialized = 0;

	GC_init();
#if !defined(VISUAL) && !defined(__MINGW32__) && !defined(__MINGW64__) && !defined(MPI)
	do_server_in_X11 = 0; /* XXX */
#endif
	asir_save_handler();
#if 0
#if defined(PARI)
	risa_pari_init();
#endif
#endif
	srandom((int)get_current_time());

	rtime_init();
	env_init();
	endian_init();
/*	process_args(argc,argv); */
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
#if defined(VISUAL) || defined(__MINGW32__) || defined(__MINGW64__)
	init_socket();
#endif
#if defined(UINIT)
	reg_sysf();
#endif
	input_init(0,"string");
	if ( do_asirrc && (ifname = find_asirrc()) ) {
		if ( !SETJMP(main_env) )
			execasirfile(ifname);
	}

	asir_OperandStackSize = BUFSIZ;
	asir_OperandStack = (Obj *)CALLOC(asir_OperandStackSize,sizeof(Obj));
	asir_OperandStackPtr = -1;
	if ( little_endian && byteorder )
		lib_ox_need_conv = 1;
	else
		lib_ox_need_conv = 0;
	do_message = 0;
	create_my_mathcap("ox_asir");
	asir_reset_handler();
	return 0;
}
#endif
