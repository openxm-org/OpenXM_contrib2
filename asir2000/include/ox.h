/* $OpenXM: OpenXM_contrib2/asir2000/include/ox.h,v 1.3 2000/02/08 04:47:10 noro Exp $ */
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

#define SM_nop 300

#define SM_control_kill 				1024
#define SM_control_intr 				1025
#define SM_control_reset_connection		1030

typedef FILE *ox_stream;

/* a macro to check whether data are available in the read buffer */
#if defined(linux)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->_IO_read_ptr < (fp)->_IO_read_end)
#elif defined(__FreeBSD__)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->_r)
#elif defined(sparc) || defined(__alpha) || defined(__SVR4) || defined(mips)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->_cnt)
#elif defined(VISUAL)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->p < (fp)->buf_size)
#elif defined(hpux)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->__cnt)
#else
--->FIXIT
#endif

extern jmp_buf environnement;

extern jmp_buf env;
extern int *StackBottom;
extern int ox_do_copy, ox_do_count, ox_count_length;
extern char *ox_copy_bptr;

extern struct IOFP iofp[];

extern char *parse_strp;

#define LBUFSIZ BUFSIZ*10

void ox_usr1_handler();
unsigned int ox_recv();
