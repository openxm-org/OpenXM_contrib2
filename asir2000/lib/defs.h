/* $OpenXM: OpenXM/src/asir99/lib/defs.h,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
/* some useful macros */

#define ZERO 0
#define NUM 1
#define POLY 2
#define RAT 3
#define LIST 4 
#define VECT 5
#define MAT 6
#define STR 7

#define N_Q 0
#define N_R 1
#define N_A 2
#define N_B 3
#define N_C 4

#define V_IND 0
#define V_UC 1
#define V_PF 2
#define V_SR 3

#define isnum(a) (type(a)==NUM)
#define ispoly(a) (type(a)==POLY)
#define israt(a) (type(a)==RAT)
#define islist(a) (type(a)==LIST)
#define isvect(a) (type(a)==VECT)
#define ismat(a) (type(a)==MAT)
#define isstr(a) (type(a)==STR)

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
