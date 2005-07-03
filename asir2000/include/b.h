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
 * $OpenXM: OpenXM_contrib2/asir2000/include/b.h,v 1.3 2000/08/22 05:04:14 noro Exp $ 
*/
#ifdef FBASE
#define D_ADDP(x,y,z,u) addp(x,y,z,u)
#define D_SUBP(x,y,z,u) subp(x,y,z,u)
#define D_MULP(x,y,z,u) mulp(x,y,z,u)
#define D_PWRP(x,y,z,u) pwrp(x,y,z,u)
#define D_DIVSRP(x,y,z,u,v) divsrp(x,y,z,u,v)
#define D_DIVSRDCP(x,y,z,u,v) divsrdcp(x,y,z,u,v)
#define D_DIVSP(x,y,z,u) divsp(x,y,z,u)
#define D_DIVSDCP(x,y,z,u) divsdcp(x,y,z,u)
#define D_CHSGNP(x,y) chsgnp(x,y)
#define D_ADDPQ(x,y,z) addpq(x,y,z)
#define D_ADDPTOC(x,y,z,u) addptoc(x,y,z,u)
#define D_MULPQ(x,y,z) mulpq(x,y,z)
#define D_MULPC(x,y,z,u) mulpc(x,y,z,u)

#define ADDP(x,y,z,u) addp(x,y,z,u)
#define SUBP(x,y,z,u) subp(x,y,z,u)
#define MULP(x,y,z,u) mulp(x,y,z,u)
#define PWRP(x,y,z,u) pwrp(x,y,z,u)
#define DIVSRP(x,y,z,u,v) divsrp(x,y,z,u,v)
#define DIVSRDCP(x,y,z,u,v) divsrdcp(x,y,z,u,v)
#define DIVSP(x,y,z,u) divsp(x,y,z,u)
#define DIVSDCP(x,y,z,u) divsdcp(x,y,z,u)
#define CHSGNP(x,y) chsgnp(x,y)
#define ADDPQ(x,y,z) addpq(x,y,z)
#define ADDPTOC(x,y,z,u) addptoc(x,y,z,u)
#define MULPQ(x,y,z) mulpq(x,y,z)
#define MULPC(x,y,z,u) mulpc(x,y,z,u)
#define ADDNUM(x,y,z) (*addnumt[MAX(NID((Q)x),NID((Q)y))])(x,y,z)
#define SUBNUM(x,y,z) (*subnumt[MAX(NID((Q)x),NID((Q)y))])(x,y,z)
#define MULNUM(x,y,z) (*mulnumt[MAX(NID((Q)x),NID((Q)y))])(x,y,z)
#define DIVNUM(x,y,z) (*divnumt[MAX(NID((Q)x),NID((Q)y))])(x,y,z)
#define PWRNUM(x,y,z) (*pwrnumt[NID((Q)x)])(x,y,z)
#define CHSGNNUM(x,y) { \
    if (NID((Q)x)==N_Q) { \
        Q t; DUPQ((Q)(x),t); SGN((Q)t)= -SGN(t); (y)= t; } \
    else if (NID((Q)x)==N_R) { \
        Real t; NEWReal(t); BDY(t)= -BDY((Real)x); (y) = t; } \
    else {P t; (*chsgnnumt[NID((Q)x)])(x,&t); (y) = t; } }
#define ADDQ(x,y,z) addq(x,y,z)
#define SUBQ(x,y,z) subq(x,y,z)
#define MULQ(x,y,z) mulq(x,y,z)
#define DIVQ(x,y,z) divq(x,y,z)
#define PWRQ(x,y,z) pwrq(x,y,z)
#define INVQ(x,y) invq(x,y)
#define REMQ(x,y,z) remq(x,y,z)
#define CMPQ(x,y) cmpq(x,y)
#define MKBC(x,y) mkbc(x,(Q *)y)

#define One ONE
#define Uniq(x) UNIQ((Q)x)
#define Base BASE
#define Dm(x,y,z,u) DM(x,y,z,u)
#define Dma(x,y,z,u,v,w) DMA(x,y,z,u,v,w)
#endif

#ifdef MODULAR
#define D_ADDP(x,y,z,u) addmp(x,mod,y,z,u) int mod;
#define D_SUBP(x,y,z,u) submp(x,mod,y,z,u) int mod;
#define D_MULP(x,y,z,u) mulmp(x,mod,y,z,u) int mod;
#define D_PWRP(x,y,z,u) pwrmp(x,mod,y,z,u) int mod;
#define D_DIVSRP(x,y,z,u,v) divsrmp(x,mod,y,z,u,v) int mod;
#define D_DIVSRDCP(x,y,z,u,v) divsrdcmp(x,mod,y,z,u,v) int mod;
#define D_DIVSP(x,y,z,u) divsmp(x,mod,y,z,u) int mod;
#define D_DIVSDCP(x,y,z,u) divsdcmp(x,mod,y,z,u) int mod;
#define D_CHSGNP(x,y) chsgnmp(mod,x,y) int mod;
#define D_ADDPQ(x,y,z) addmpq(mod,x,y,z) int mod;
#define D_ADDPTOC(x,y,z,u) addmptoc(x,mod,y,z,u) int mod;
#define D_MULPQ(x,y,z) mulmpq(mod,x,y,z) int mod;
#define D_MULPC(x,y,z,u) mulmpc(x,mod,y,z,u) int mod;

#define ADDP(x,y,z,u) addmp(x,mod,y,z,u)
#define SUBP(x,y,z,u) submp(x,mod,y,z,u)
#define MULP(x,y,z,u) mulmp(x,mod,y,z,u)
#define PWRP(x,y,z,u) pwrmp(x,mod,y,z,u)
#define DIVSRP(x,y,z,u,v) divsrmp(x,mod,y,z,u,v)
#define DIVSRDCP(x,y,z,u,v) divsrdcmp(x,mod,y,z,u,v)
#define DIVSP(x,y,z,u) divsmp(x,mod,y,z,u)
#define DIVSDCP(x,y,z,u) divsdcmp(x,mod,y,z,u)
#define CHSGNP(x,y) chsgnmp(mod,x,y)
#define ADDPQ(x,y,z) addmpq(mod,x,y,z)
#define ADDPTOC(x,y,z,u) addmptoc(x,mod,y,z,u)
#define MULPQ(x,y,z) mulmpq(mod,x,y,z)
#define MULPC(x,y,z,u) mulmpc(x,mod,y,z,u)
#define ADDNUM(x,y,z) addmq(mod,(MQ)x,(MQ)y,(MQ *)z)
#define SUBNUM(x,y,z) submq(mod,(MQ)x,(MQ)y,(MQ *)z)
#define MULNUM(x,y,z) mulmq(mod,(MQ)x,(MQ)y,(MQ *)z)
#define DIVNUM(x,y,z) divmq(mod,(MQ)x,(MQ)y,(MQ *)z)
#define PWRNUM(x,y,z) pwrmq(mod,(MQ)x,y,(MQ *)z)
#define CHSGNNUM(x,y) {MQ t; NEWMQ(t),CONT(t)=mod-CONT((MQ)(x)); (y)=t; }
#define ADDQ(x,y,z) addmq(mod,x,y,z)
#define SUBQ(x,y,z) submq(mod,x,y,z)
#define MULQ(x,y,z) mulmq(mod,x,y,z)
#define DIVQ(x,y,z) divmq(mod,x,y,z)
#define PWRQ(x,y,z) pwrmq(mod,x,y,z)
#define MKBC(x,y) mkbcm(mod,x,(MQ *)y)

#define One ONEM
#define Uniq(x) UNIMQ((MQ)x)
#endif
