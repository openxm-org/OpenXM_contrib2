/* This file extracted by Configure */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define GPDATADIR "/pari/data"

#define PARIVERSION "GP/PARI CALCULATOR Version 2.0.13 (alpha)"
#ifdef __cplusplus
# define PARIINFO "C++ Windows NT ix86 (ix86 kernel) 32-bit version"
#else
# define PARIINFO "Windows NT ix86 (ix86 kernel) 32-bit version"
#endif

#define PARI_BYTE_ORDER    1234

/* MSVC inline directive */
#define INLINE __inline

/*  Location of GNU gzip program, enables reading of .Z and .gz files. */
#define GNUZCAT
#define ZCAT "gzip -d -c"

/* No exp2, log2 in libc */
#define NOEXP2

/* Headers are clean - ulong not defined */
#define ULONG_NOT_DEFINED

/* Timings: Don't use times because of the HZ / CLK_TCK confusion. */
#define USE_FTIME 1

#define HAS_STRFTIME

#define DL_DFLT_NAME "libpari.dll"
#define SHELL_Q '\''
#if 1
#include "w_stdio.h"
#undef getc
#define getc w_fgetc
#undef putc
#define putc w_fputc
#define ungetc w_ungetc
#define fgets w_fgets
#define fputs w_fputs
#define printf w_printf
#define fprintf w_fprintf
#define fflush w_fflush
#endif
#endif
