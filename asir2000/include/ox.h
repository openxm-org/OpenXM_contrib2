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
 * $OpenXM: OpenXM_contrib2/asir2000/include/ox.h,v 1.11 2001/06/15 07:56:05 noro Exp $ 
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

#define CMO_64BIT_MACHINE_DOUBLE   40
#define CMO_ARRAY_OF_64BIT_MACHINE_DOUBLE  41
#define CMO_128BIT_MACHINE_DOUBLE   42
#define CMO_ARRAY_OF_128BIT_MACHINE_DOUBLE  43

#define CMO_BIGFLOAT				50
#define CMO_IEEE_DOUBLE_FLOAT		51

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

#define SM_nop 300

#define SM_control_kill 				1024
#define SM_control_intr 				1025
#define SM_control_reset_connection		1030

typedef FILE *ox_stream;

/* a macro to check whether data are available in the read buffer */
#if defined(linux)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->_IO_read_ptr < (fp)->_IO_read_end)
#elif defined(__FreeBSD__) || (defined(__MACH__) && defined(__ppc__))
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->_r)
#elif defined(sparc) || defined(__alpha) || defined(__SVR4) || defined(mips) || defined(_IBMR2)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->_cnt)
#elif defined(VISUAL)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->p < (fp)->buf_size)
#elif defined(hpux)
#define FP_DATA_IS_AVAILABLE(fp) ((fp)->__cnt)
#else
--->FIXIT
#endif

extern jmp_buf environnement;

extern jmp_buf main_env;
extern int *StackBottom;
extern int ox_do_copy, ox_do_count, ox_count_length;
extern char *ox_copy_bptr;

extern struct IOFP iofp[];

extern char *parse_strp;

#define LBUFSIZ BUFSIZ*10

void ox_usr1_handler();
unsigned int ox_recv();

/* library functions */
void asir_ox_push_cmo(void *);
int asir_ox_pop_cmo(void *, int);
void asir_ox_push_cmd(int);
void asir_ox_execute_string(char *);
int asir_ox_peek_cmo_size();
int asir_ox_init(int);
