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
 * $OpenXM: OpenXM_contrib2/asir2000/include/com.h,v 1.7 2001/12/28 04:28:20 noro Exp $ 
*/
#ifndef _COM_H_
#define _COM_H_
#if defined(VISUAL) || MPI
#include "wsio.h"
#endif

#include <stdio.h>

#if !defined(VISUAL)
#include <netdb.h>
#if defined(__CYGWIN__)
#include <cygwin/types.h>
#include <cygwin/socket.h>
#include <cygwin/in.h>
#define IS_CYGWIN 1
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <rpc/rpc.h>
#define IS_CYGWIN 0
#endif
#include <setjmp.h>
#else
#define IS_CYGWIN 0
#endif

#define N32 0x80

#define ISIZ sizeof(int)
#define MAXIOFP 1024

#define C_ZERO 0
#define C_OBJ 1
#define C_VL 2
#define C_INT 3
#define C_CLOSE 4
#define C_ENDARG 5
#define C_PRIV C_ENDARG+1
#define C_SHUTDOWN 6

#if defined(VISUAL) || MPI
struct IOFP {
	STREAM *in,*out;
	char *inbuf,*outbuf;
	int s,conv;
	char *socket;
};

void ws_loadv(STREAM *,V *);
void ws_savev(STREAM *,V);
void ws_loadvl(STREAM *);
void ws_savevl(STREAM *,VL);
V ws_load_convv(int);
int ws_save_convv(V);
#else
struct IOFP {
	FILE *in,*out;
	char *inbuf,*outbuf;
	int s,conv;
	char *socket;
};
#endif
#endif /* _COM_H_ */
