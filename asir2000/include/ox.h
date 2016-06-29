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
 * $OpenXM: OpenXM_contrib2/asir2000/include/ox.h,v 1.29 2015/08/18 02:26:05 noro Exp $ 
*/
#include "com.h"

/* version */

#define OX_VERSION		199901160

/* header */

#define OX_COMMAND		513
#define OX_DATA			514
#define OX_SYNC_BALL	515

#define OX_DATA_WITH_SIZE		521
#define OX_DATA_ASIR_BINARY_EXPRESSION		522

#define OX_LOCAL_OBJECT	0x7fcdef30

#define OX_LOCAL_OBJECT_ASIR (OX_LOCAL_OBJECT+0)
#define OX_LOCAL_OBJECT_SM1	(OX_LOCAL_OBJECT+1)

/* cmo_tags */

#define CMO_LARGE_ID	0x7f000000

#define CMO_ERROR			CMO_LARGE_ID+1
#define CMO_ERROR2			CMO_LARGE_ID+2

#define CMO_NULL			1
#define CMO_INT32			2
#define CMO_DATUM			3
#define CMO_STRING			4
#define CMO_MATHCAP			5

#define CMO_ARRAY				16
#define CMO_LIST				17
#define CMO_ATOM				18
#define CMO_MONOMIAL32			19
#define CMO_ZZ					20
#define CMO_QQ					21
#define CMO_ZERO				22

#define CMO_DMS_GENERIC			24
#define CMO_DMS_OF_N_VARIABLES	25
#define CMO_RING_BY_NAME		26
#define CMO_RECURSIVE_POLYNOMIAL		27

#define CMO_DISTRIBUTED_POLYNOMIAL		31
#define CMO_UNIVARIATE_POLYNOMIAL		33
#define CMO_RATIONAL		34
#define CMO_COMPLEX		35

#define CMO_64BIT_MACHINE_DOUBLE   40
#define CMO_ARRAY_OF_64BIT_MACHINE_DOUBLE  41
#define CMO_128BIT_MACHINE_DOUBLE   42
#define CMO_ARRAY_OF_128BIT_MACHINE_DOUBLE  43

#define CMO_BIGFLOAT				50
#define CMO_IEEE_DOUBLE_FLOAT		51
#define CMO_BIGFLOAT32				52

#define CMO_INDETERMINATE		60
#define CMO_TREE				61
#define CMO_LAMBDA				62

/* asir local object id */

#define ASIR_VL					0
#define ASIR_OBJ				1

/* commands */

#define SM_popSerializedLocalObject 258
#define SM_popCMO 262
#define SM_popString 263

#define SM_mathcap 264
#define SM_pops 265
#define SM_setName 266
#define SM_evalName 267
#define SM_executeStringByLocalParser 268
#define SM_executeFunction 269

#define SM_beginBlock 270
#define SM_endBlock 271
#define SM_shutdown 272
#define SM_setMathcap 273
#define SM_executeStringByLocalParserInBatchMode 274
#define SM_getsp 275
#define SM_dupErrors 276
#define SM_pushCMOtag 277

#define SM_set_rank_102 278
#define SM_tcp_accept_102 279
#define SM_tcp_connect_102 280
#define SM_reset_102 281
#define SM_bcast_102 282
#define SM_reduce_102 283

#define SM_nop 300

/* local SM command in ox_asir */
#define SM_executeFunctionSync 400

#define SM_control_kill 				1024
#define SM_control_intr 				1025
#define SM_control_reset_connection		1030

typedef FILE *ox_stream;

/* a macro to check whether data are available in the read buffer */
#if defined(linux)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->_IO_read_ptr < (fp)->_IO_read_end)
#elif defined(__FreeBSD__) || defined(__DARWIN__) || (defined(__MACH__) && defined(__ppc__)) || defined(__CYGWIN__) || defined(__INTERIX)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->_r)
#elif defined(sparc) || defined(__alpha) || defined(__SVR4) || defined(mips) || defined(_IBMR2)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->_cnt)
#elif defined(VISUAL) || defined(__MINGW32__)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->p < (fp)->buf_size)
#elif defined(hpux)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->__cnt)
#else
--->FIXIT
#endif

/* environement is defined in libpari.a */
extern jmp_buf environnement;

extern JMP_BUF main_env;

extern int *StackBottom;
extern int ox_do_copy, ox_do_count, ox_count_length;
extern char *ox_copy_bptr;

extern struct IOFP iofp[];
extern struct IOFP iofp_102[];

extern char *parse_strp;

#if 0
#define LBUFSIZ BUFSIZ*10
#else
#define LBUFSIZ BUFSIZ
#endif

/* prototypes */

void cmoname_to_localname(char *a,char **b);
void localname_to_cmoname(char *a,char **b);
void read_cmo_tree_as_list(FILE *s,LIST *rp);
void read_cmo_upoly(FILE *s,P *rp);
void read_cmo_p(FILE *s,P *rp);
void read_cmo_monomial(FILE *s,DP *rp);
void read_cmo_dp(FILE *s,DP *rp);
void read_cmo_list(FILE *s,Obj *rp);
void read_cmo_zz(FILE *s,int *sgn,N *rp);
void read_cmo_uint(FILE *s,USINT *rp);
void read_cmo_bf(FILE *s,BF *q);
void read_cmo_complex(FILE *s,C *q);
void read_cmo(FILE *s,Obj *rp);
void write_cmo_tree(FILE *s,LIST l);
void write_cmo_error(FILE *s,ERR e);
void write_cmo_bytearray(FILE *s,BYTEARRAY array);
void write_cmo_string(FILE *s,STRING str);
void write_cmo_list(FILE *s,LIST list);
void write_cmo_monomial(FILE *s,MP m,int n);
void write_cmo_dp(FILE *s,DP dp);
void write_cmo_r(FILE *s,R f);
void write_cmo_upoly(FILE *s,VL vl,P p);
void write_cmo_p(FILE *s,P p);
void write_cmo_zz(FILE *s,int sgn,N n);
void write_cmo_real(FILE *s,Real real);
void write_cmo_q(FILE *s,Q q);
void write_cmo_bf(FILE *s,BF q);
void write_cmo_complex(FILE *s,C q);
void write_cmo_uint(FILE *s,USINT ui);
void write_cmo_matrix_as_list(FILE *s,MAT m);
void write_cmo_mathcap(FILE *s,MATHCAP mc);
int cmo_tag(Obj obj,int *tag);
void write_cmo(FILE *s,Obj obj);
int valid_as_cmo(Obj obj);
void ox_flush_stream_force(int s);
void ox_flush_stream_force_102(int s);
void ox_flush_stream(int s);
void ox_flush_stream_102(int s);
int ox_check_cmo_dp(int s, DP p);
int ox_check_cmo_p(int s, P p);
void ox_get_serverinfo(int s, LIST *rp);
int ox_check_cmo(int s, Obj obj);
void ox_write_cmo(int s, Obj obj);
void ox_write_cmo_102(int s, Obj obj);
void ox_write_int(int s, int n);
void ox_write_int_102(int s, int n);
void ox_read_local(int s, Obj *rp);
void ox_read_local_102(int s, Obj *rp);
void ox_read_cmo(int s, Obj *rp);
void ox_read_cmo_102(int s, Obj *rp);
void ox_read_int(int s, int *n);
void ox_read_int_102(int s, int *n);
void ox_get_result(int s,Obj *rp);
unsigned int ox_recv(int s, int *id, Obj *p);
unsigned int ox_recv_102(int s, int *id, Obj *p);
void ox_send_local_ring(int s,VL vl);
void ox_send_local_ring_102(int s,VL vl);
void ox_send_local_data(int s,Obj p);
void ox_send_local_data_102(int s,Obj p);
void ox_send_sync(int s);
void ox_send_sync_102(int s);
void ox_send_cmd(int s,int id);
void ox_send_data(int s,pointer p);
void ox_send_data_102(int s,pointer p);
void wait_for_data(int s);
void wait_for_data_102(int s);
int ox_data_is_available(int s);
void clear_readbuffer();
void end_critical();
void begin_critical();
int check_by_mc(int s,unsigned int oxtag,unsigned int cmotag);
int check_sm_by_mc(int s,unsigned int smtag);
void store_remote_mathcap(int s,MATHCAP mc);
void create_my_mathcap(char *system);
void ox_resetenv(char *s);
void cleanup_events();
void reset_io();
void endian_init();
int countobj(Obj p);
int count_as_cmo(Obj p);
int countvl(VL vl);
void ox_copy_init(char *s);
void ox_obj_to_buf_as_cmo(Obj p);
void ox_buf_to_obj_as_cmo(Obj *p);
void ox_vl_to_buf(VL vl);
int gen_fread (char *ptr,int size,int nitems,FILE *stream);
int gen_fwrite (char *ptr,int size,int nitems,FILE *stream);
void write_char(FILE *f,unsigned char *p);
void write_short(FILE *f,unsigned short *p);
void write_int(FILE *f,unsigned int *p);
void write_int64(FILE *f,UL *p);
void init_deskey();
void write_intarray(FILE *f,unsigned int *p,int l);
void write_longarray(FILE *f,unsigned long *p,int l);
void write_double(FILE *f,double *p);
void write_string(FILE *f,unsigned char *p,int l);
void read_char(FILE *f,unsigned char *p);
void read_short(FILE *f,unsigned short *p);
void read_int(FILE *f,unsigned int *p);
void read_int64(FILE *f,UL *p);
void read_intarray(FILE *f,unsigned int *p,int l);
void read_longarray(FILE *f,unsigned long *p,int l);
void read_string(FILE *f,unsigned char *p,int l);
void read_double(FILE *f,double *p);
int getremotesocket(int s);
void getremotename(int s,char *name);
void generate_port(int use_unix,char *port_str);
int try_bind_listen(int use_unix,char *port_str);
int try_accept(int af_unix,int s);
int try_connect(int use_unix,char *host,char *port_str);
void close_allconnections();
void free_iofp(int s);
int get_iofp(int s1,char *af_sock,int is_server);
void init_socket();
int get_fd(int index);
int get_index(int fd);
void ox_launch_generic(char *host,char *launcher,char *server,
		int use_unix,int use_ssh,int use_x,int conn_to_serv,Q *rp);
void spawn_server(char *host,char *launcher,char *server,
	int use_unix,int use_ssh,int use_x,int conn_to_serv,
	char *control_port_str,char *server_port_str);
void ox_launch_main(int with_x,NODE arg,Obj *p);
int register_server(int af_unix,int m,int c,int fd);
int get_mcindex(int i);
void shutdown_all();

/* library functions */
void asir_ox_push_cmo(void *);
int asir_ox_pop_cmo(void *, int);
void asir_ox_push_cmd(int);
void asir_ox_execute_string(char *);
int asir_ox_peek_cmo_size();
int asir_ox_init(int);

