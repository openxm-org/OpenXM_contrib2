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
 * $OpenXM$
*/
/* some useful macros */

#define O_VOID -1
#define O_ZERO 0
#define O_NUM 1
#define O_POLY 2
#define O_RAT 3
#define O_LIST 4 
#define O_VECT 5
#define O_MAT 6
#define O_STR 7
#define O_STRUCT 8
#define O_DPOLY 9
#define O_USINT 10
#define O_ERR 11
#define O_GF2MAT 12
#define O_MATHCAP 13
#define O_F 14
#define O_GFMMAT 15
#define O_BYTEARRAY 16
#define O_QUOTE 17
#define O_OPTLIST 18
#define O_SYMBOL 19
#define O_RANGE 20
#define O_TB 21
#define O_DPV 22
#define O_QUOTEARG 23
#define O_VOID -1
#define O_IMAT 24
#define O_NBP 25
#define O_DPM 26

#define N_Q 0
#define N_R 1
#define N_A 2
#define N_B 3
#define N_C 4

#define V_IND 0
#define V_UC 1
#define V_PF 2
#define V_SR 3

#define isvoid(a) (type(a)==O_VOID)
#define isnum(a) (type(a)==O_NUM)
#define ispoly(a) (type(a)==O_POLY)
#define israt(a) (type(a)==O_RAT)
#define islist(a) (type(a)==O_LIST)
#define isvect(a) (type(a)==O_VECT)
#define ismat(a) (type(a)==O_MAT)
#define isstr(a) (type(a)==O_STR)
#define isstruct(A) (type(A)==O_STRUCT)
#define isdpoly(a) (type(a)==O_DPOLY)
#define isusint(a) (type(a)==O_USINT)
#define iserr(a) (type(a)==O_ERR)
#define isgf2mat(a) (type(a)==O_GF2MAT)
#define ismathcap(a) (type(a)==O_MATHCAP)
#define isf(a) (type(a)==O_F)
#define isgfmmat(a) (type(a)==O_GFMMAT)
#define isbytearray(a) (type(a)==O_BYTEARRAY)
#define isquote(a) (type(a)==O_QUOTE)
#define isoptlist(a) (type(a)==O_OPTLIST)
#define issymbol(a) (type(a)==O_SYMBOl)
#define isrange(a) (type(a)==O_RANGE)
#define istb(a) (type(a)==O_TB)
#define isdpv(a) (type(a)==O_DPV)
#define isquotearg(a) (type(a)==O_QUOTEARG)
#define isimat(a) (type(a)==O_IMAT)
#define isnbp(a) (type(a)==O_NBP)
#define isdpm(a) (type(a)==O_DPM)

#define isnum_int(a) ((a)==0 || (type((a))==NUM && ntype((a))==N_Q && dn((a))==1))

#define FIRST(L) (car(L))
#define SECOND(L) (car(cdr(L)))
#define THIRD(L) (car(cdr(cdr(L))))
#define FOURTH(L) (car(cdr(cdr(cdr(L)))))

#define DEG(a) deg(a,var(a))
#define LCOEF(a) coef(a,deg(a,var(a)))
#define LTERM(a) coef(a,deg(a,var(a)))*var(a)^deg(a,var(a))
#define TT(a) car(car(a))
#define TS(a) car(cdr(car(a)))

#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)>(b)?(b):(a))
