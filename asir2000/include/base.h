/* $OpenXM: OpenXM/src/asir99/include/base.h,v 1.1.1.1 1999/11/10 08:12:30 noro Exp $ */
#define BSH 32
#define CBSH (32-BSH)
#define LBASE (((L)1)<<BSH)
#define ULBASE (((UL)1)<<BSH)
#define BMASK (LBASE-1)

#define BSH27 27
#define CBSH27 (32-BSH27)
#define BASE27 (1<<BSH27)
#define BMASK27 (BASE27-1)

#define DLENGTH 8
#define DBASE 100000000
