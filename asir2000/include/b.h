/* $OpenXM: OpenXM/src/asir99/include/b.h,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
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
#define CHSGNNUM(x,y) \
(NID((Q)x)==N_Q?(DUPQ((Q)x,(Q)y),SGN((Q)y)= -SGN((Q)y),(y))\
	:NID((Q)x)==N_R?(NEWReal((Real)y),BDY((Real)y)= -BDY((Real)x),(y))\
		:((*chsgnnumt[NID((Q)x)])(x,&y),(y)))
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
#define CHSGNNUM(x,y) (NEWMQ((MQ)y),CONT((MQ)(y))=mod-CONT((MQ)(x)))
#define ADDQ(x,y,z) addmq(mod,x,y,z)
#define SUBQ(x,y,z) submq(mod,x,y,z)
#define MULQ(x,y,z) mulmq(mod,x,y,z)
#define DIVQ(x,y,z) divmq(mod,x,y,z)
#define PWRQ(x,y,z) pwrmq(mod,x,y,z)
#define MKBC(x,y) mkbcm(mod,x,(MQ *)y)

#define One ONEM
#define Uniq(x) UNIMQ((MQ)x)
#endif
