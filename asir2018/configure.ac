dnl $OpenXM: OpenXM_contrib2/asir2018/configure.ac,v 1.4 2020/01/09 02:34:59 ohara Exp $

AC_INIT(asir,1.16)
AC_CONFIG_SRCDIR([LICENSE])
AC_PREREQ(2.69)
AC_CANONICAL_TARGET
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE
LT_INIT

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB
AC_PROG_YACC
AC_PROG_MKDIR_P

AC_PATH_XTRA

AC_ARG_ENABLE([static_asir],
[  --enable-static-asir    force to link statically with libasir. [[yes]]],
[enable_static_asir=${enableval}],[enable_static_asir=yes])
AM_CONDITIONAL([ASIR_STATIC],[test "${enable_static_asir}" = yes])

if test "${ac_test_CFLAGS}" != set -a "${GCC}" = yes ; then
    if test "${ac_cv_prog_cc_g}" = yes; then
        CFLAGS="-g -O"
    else
        CFLAGS="-O"
    fi
elif test `basename ./"${CC}"` = "icc" ; then
    CFLAGS="-g -O"
fi

AC_ARG_WITH([asir-gc],
[  --with-asir-gc          modify Boehm's GC [[yes]]],
[with_asir_gc=${withval}],[with_asir_gc=yes])
if test ${with_asir_gc:=yes} != no; then
   GCLIB='-L${prefix}/lib -lasir-gc'
else
   GCLIB='-L${prefix}/lib -lgc'
   AC_DEFINE([NO_ASIR_GC])
fi
GCINC='-I${prefix}/include'
AC_DEFINE([GC7])
AC_SUBST([GCLIB])
AC_SUBST([GCINC])

AC_CHECK_TYPES([unsigned long long])
AC_CHECK_SIZEOF(long)
if test "${ac_cv_sizeof_long}" = 8; then
    AC_DEFINE([LONG_IS_64BIT])
else
    AC_DEFINE([LONG_IS_32BIT])
fi
if test "${no_x}" != yes ; then
    if test -f "${x_libraries}/libXpm.a" -o -f "${x_libraries}/libXpm.dll.a" ; then
	    XPMLIB="-lXpm"
    fi
    AC_ARG_ENABLE([plot],
    [  --enable-plot           enable plot feature. [[no]]],
    [enable_plot=yes],[enable_plot=no])
fi
AC_SUBST([XPMLIB])

dnl sufx is suffix for libasir
sufx=""
if test ${enable_plot:=no} != no ; then
    sufx=${sufx}_X
    AC_DEFINE([DO_PLOT])
fi
AM_CONDITIONAL([USE_PLOT],[test "$enable_plot" = yes])

GMPLIB="-L${libdir} -lmpfi -lmpc -lmpfr -lgmp"
AC_ARG_ENABLE([static_gmp],
[  --enable-static-gmp     force to link statically with gmp. [[no]]],
[enable_static_gmp=yes],[enable_static_gmp=no])
if test ${enable_static_gmp:=no} != no ; then
    GMPLIB='${libdir}/libmpfi.a ${libdir}/libmpc.a ${libdir}/libmpfr.a ${libdir}/libgmp.a'
fi
AC_SUBST([GMPLIB])

AC_ARG_ENABLE([interval],
[  --enable-interval       enable interval feature. [[no]]],
[enable_interval=${enableval}],[enable_interval=no])

if test ${enable_interval:=no} != no ; then
    sufx=${sufx}_itv
    AC_DEFINE([INTERVAL])
    if test "${enable_interval}" = debug ; then
       AC_DEFINE([ITVDEBUG])
    fi
fi

AC_ARG_ENABLE([fep],
[  --enable-fep            enable fep for human interfaces. [[no]]],
[enable_fep=yes],[enable_fep=no])

if test "${enable_fep:=no}" != no ; then
    AC_DEFINE([FEP])
    FEPLIB="-L${prefix}/lib -ledit"
fi
AC_SUBST([FEPLIB])

AC_ARG_ENABLE([fft-float],
[  --enable-fft-float      enable FFT-FLOAT feature. [[no]]],
[enable_fft_float=yes],[enable_fft_float=no])

if test "${enable_fft_float:=no}" != no ; then
    AC_DEFINE([USE_FLOAT])
fi

AC_ARG_WITH([mpi],
[  --with-mpi              use the MPI library. [[no]]],
[with_mpi=${withval}],[with_mpi=no])

if test "${with_mpi:=no}" != no ; then
    AC_DEFINE([MPI])
fi

dnl AC_ARG_WITH([lapack],
dnl [  --with-lapack           use the LAPACK library. [[no]]],
dnl [with_lapack=${withval}],[with_lapack=no])

if test "${with_lapack:=no}" != no ; then
    LAPACKLIB="-llapack -lblas -lg2c"
    AC_DEFINE([LAPACK])
fi
AC_SUBST([LAPACKLIB])

AC_SUBST([sufx])

case "${host}" in
[i[3-6]86-*-linux*]|x86-*-linux*)
    AC_DEFINE([_BSD_SOURCE])
    ;;
arm*-*-linux*)
    if test "${ac_test_CFLAGS}" != set -a "${GCC}" = yes ; then
        CFLAGS="${CFLAGS} -fsigned-char"
    fi
    ;;
[*-*-interix*])
    AC_DEFINE([_ALL_SOURCE])
    ;;
sparc-sun-solaris2.*)
    case "`uname -m`" in
    sun4u*)     if test "${ac_test_CFLAGS}" != set -a "${GCC}" = yes ; then
                    CFLAGS="${CFLAGS} -m64"
                fi
                AC_SUBST([USE_GC_SPARC64])
                ;;
    esac
    AC_DEFINE([SYSV])
    ;;
*-apple-darwin*)
    AC_DEFINE([__DARWIN__])
    if test -d /usr/X11/include -a x"${X_CFLAGS}" = x ; then
        X_CFLAGS=-I/usr/X11/include 
    fi
    ;;
*-*-mingw*)
	WSLIB="-lws2_32"
    ;;
esac

AC_SUBST([WSLIB])

AC_CHECK_FUNCS(sigaction)
dnl for Solaris 2.x
AC_CHECK_FUNC(socket, , [AC_CHECK_LIB(socket,socket)])
AC_CHECK_FUNC(gethostbyname, , [AC_CHECK_LIB(nsl,gethostbyname)])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
