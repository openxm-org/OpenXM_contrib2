/* $OpenXM: OpenXM/src/asir99/include/al.h,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
/* ----------------------------------------------------------------------
   $Id$
   ----------------------------------------------------------------------
   File al.h: Real quantifier elimination code for RISA/ASIR

   Copyright (c) 1996, 1997, 1998 by
   Andreas Dolzmann and Thomas Sturm, University of Passau, Germany
   dolzmann@uni-passau.de, sturm@uni-passau.de
   ----------------------------------------------------------------------
*/

/* Operator bitstrings: (qua, junct, bin, uni, atomic, x1, x2, x3)
   where x1, x3, x3 are
   atomic: direction, strict, order
   uni: -, -, -
   bin: direction, -, order
   junct: conjunctive, conjunctive, -
   qua: conjunctive, conjunctive, - */

#define AL_TRUE 0x00
#define AL_FALSE 0x06
#define AL_EX 0x80
#define AL_ALL 0x86
#define AL_OR 0x40
#define AL_AND 0x46
#define AL_NOT 0x10
#define AL_IMPL 0x25
#define AL_REPL 0x21
#define AL_EQUIV 0x20

#define AL_EQUAL 0x08
#define AL_NEQ 0x0E
#define AL_LESSP 0x0B
#define AL_GREATERP 0x0F
#define AL_LEQ 0x09
#define AL_GEQ 0x0D

#define AL_QUANT(op) ((op) & 0x80)
#define AL_JUNCT(op) ((op) & 0x40)
#define AL_TVAL(op) (!((op) & 0xF9))
#define AL_UNI(op) ((op) & 0x10)
#define AL_EXT(op) ((op) & 0x20)
#define AL_ATOMIC(op) ((op) & 0x08)
#define AL_ORDER(op) ((op) & 0x01)
#define AL_ANEGREL(r) ((r) ^ 0x04)
#define AL_ANEGJUNCT(r) ((r) ^ 0x06)
#define AL_NEUTRAL(j) ((j) ^ 0x46)
#define AL_OMNIPOT(j) ((j) & 0x06)

#define AL_MKSTRICT(op) ((op) | 0x02)
#define AL_LNEGOP(op) ((op) ^ 0x06)

/* Formal lower bound: AL_GREATERP or AL_GEQ */
#define AL_FLB(rel) (rel == AL_GREATERP || rel == AL_GEQ)

/* Transcendent substitution expansion:
   AL_EQUAL -> AL_AND, AL_NEQ -> AL_OR, used by subtrans_a_no */
#define AL_TRSUBEXP(no) ((no) ^ 0x4E)


/* Public Data Structures */

/* #define O_F 14; defined in ca.h */

#define LBFLB(x) ((x)->label)
#define LBFF(x) ((x)->formula)

#define FOP(x) ((x)->op)

#define FPL(x) ((x)->arg.faarg)
#define FARG(x) ((x)->arg.fuarg)
#define FJARG(x) ((x)->arg.fjarg)

#define FBARG(x) ((x)->arg.fbarg)
#define FQARG(x) ((x)->arg.fqarg)

#define FLHS(x) (FBARG(x)->lhs)
#define FRHS(x) (FBARG(x)->rhs)
#define FQVR(x) VR(FQARG(x))
#define MAT(x) ((x)->mat)
#define FQMAT(x) MAT(FQARG(x))

#define NEWLBF(l) ((l)=(LBF)MALLOC(sizeof(struct oLBF)))
#define NEWF(f) ((f)=(F)MALLOC(sizeof(struct oF)),OID(f)=O_F)
#define NEWFQARG(x) ((x)=(FQARG)MALLOC(sizeof(struct oFQARG)))
#define NEWFBARG(x) ((x)=(FBARG)MALLOC(sizeof(struct oFBARG)))

#define MKLBF(x,f,l) (NEWLBF(x),LBFF(x)=f,LBFLB(x)=l)
#define MKTV(f,tv) (NEWF(f),FOP(f)=tv,(f)->arg.dummy=0)
#define MKAF(f,r,p) (NEWF(f),FOP(f)=r,FPL(f)=p)
#define MKUF(f,op,arg) (NEWF(f),FOP(f)=op,FARG(f)=arg)
#define MKBF(f,op,lhs,rhs) \
(NEWF(f),FOP(f)=op,NEWFBARG(FBARG(f)),FLHS(f)=lhs,FRHS(f)=rhs)
#define MKJF(f,j,argl) (NEWF(f),FOP(f)=j,FJARG(f)=argl)
#define MKQF(f,q,var,m) \
(NEWF(f),FOP(f)=q,NEWFQARG(FQARG(f)),FQVR(f)=var,FQMAT(f)=m)

typedef unsigned int oFOP;

struct oF {
   short id;
   oFOP op;
   union {
      void *dummy;
      P faarg;
      struct oF *fuarg;
      struct oFBARG *fbarg;
      NODE fjarg;
      struct oFQARG *fqarg;
   } arg;
};

typedef struct oF *F;

struct oFBARG {
   F lhs;
   F rhs;
};

typedef struct oFBARG *FBARG;

struct oFQARG {
  V v;
  F mat;
};

typedef struct oFQARG *FQARG;

struct oLBF {
  F formula;
  int label;
};

typedef struct oLBF *LBF;

extern F F_TRUE;
extern F F_FALSE;

